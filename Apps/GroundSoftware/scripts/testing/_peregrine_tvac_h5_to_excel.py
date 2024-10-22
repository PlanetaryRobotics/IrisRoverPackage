"""
Converts an `h5` (HDF) archive from integrated TVAC to an excel file.
NOTE: This only extracts the `telem` table containing parsed telemtery and not
the raw `yamcs` table containing raw data received from YAMCS.
NOTE: If this gives protobuf warnings, either resolve them or run:
- `PROTOCOL_BUFFERS_PYTHON_IMPLEMENTATION=python ./run-script.sh ...`
- instead of: `./run-script.sh ...`
"""
# Activate postponed annotations:
from __future__ import annotations

if __name__ == "__main__":
    # Loading dependencies can take a bit (builds DataStandards as part of
    # this). Acknowledge that it's doing something if running as main:
    print('Booting . . .')

from typing import Any, Final, Tuple, List, Dict
from dataclasses import dataclass
from datetime import datetime
import os.path

import argparse

from dataclasses import dataclass
from termcolor import colored

import pandas as pd

import warnings
# Suppress PerformanceWarning about pytables needing to pickle some fields
# (that's fine - this is a once daily script that runs in minutes, performance
# really doesn't matter here):
warnings.simplefilter(action='ignore', category=pd.errors.PerformanceWarning)

TITLE: Final[str] = 'IRIS Lunar Rover — Peregrine TVAC H5 to Excel Tool — CLI'

parser = argparse.ArgumentParser(
    description=TITLE)


def get_opts():
    """
    Return settings wrapped in argparse.
    """
    parser.add_argument('-n', '--name', default='int-tvac-data2',
                        help=r'Name of dataset ({db-dir}/{name}.h5)')
    parser.add_argument('-d', '--db-dir', default='./out/databases',
                        help='Directory where databases are kept.')
    parser.add_argument('--start-time-utc', default="2023-01-01",
                        help="Earliest time to allow in the export (UTC) "
                             "or `None` for no limits on start time. "
                             "Default is Jan. 1, 2023 (before start) because "
                             "some data in the archive was erroneously "
                             "labelled as being from 2001, so we need to not "
                             "plot that.")
    parser.add_argument('--end-time-utc', default=None,
                        help="Latest time to allow in the plot (UTC) "
                             "or `None` for no limits on end time.")

    opts = parser.parse_args()
    return opts


@dataclass
class DataSeries:
    """Series of timestamped data-points
    (DataFrame with a TimeSeries Datetimeindex of times and a column of values)
    """
    data: pd.DataFrame

    def __post_init__(self) -> None:
        """Make sure DataFrame formatting is correct."""
        # Make sure index is datetime:
        self.data.index = pd.to_datetime(self.data.index, utc=True)
        # Make sure index is sorted by ascending time:
        self.data.sort_index(ascending=True, inplace=True)

    @classmethod
    def from_lists(
        cls,
        name: str,
        times: List[datetime],
        values: List[Any]
    ) -> DataSeries:
        # Create basic data series:
        time_name = 'time'  # DataSet requires this to be the same across all
        df = pd.DataFrame({
            time_name: times,
            name: values
        })
        # Ensure proper time formatting and add Datetimeindex:
        try:
            df[time_name] = pd.to_datetime(df[time_name], utc=True)
            df.set_index(time_name, inplace=True)
        except Exception as e:
            # Add some context:
            print(df)
            # print(times)
            # Re-raise:
            raise e
        return DataSeries(df)

    @classmethod
    def from_tuples_list(
        cls,
        name: str,
        data: List[Tuple[datetime, Any]]
    ) -> DataSeries:
        # Unzip tuples into two lists:
        if len(data) > 0:
            times, values = [*zip(*data)]  # type: ignore
            return cls.from_lists(name, times, values)
        else:
            return cls.from_lists(name, [], [])


@dataclass
class DataSet:
    """Collection of data containing multiple named DataSeries comprised of a
    multiple timestamped data points
    (basically just a way for merging a bunch of Timeseries DataFrames into
    one).
    """
    data: pd.DataFrame

    def __post_init__(self) -> None:
        """Make sure DataFrame formatting is correct."""
        # Make sure index is datetime:
        self.data.index = pd.to_datetime(self.data.index, utc=True)
        # Make sure index is sorted by ascending time:
        self.data.sort_index(ascending=True, inplace=True)

    def stack(self, other: DataSet) -> DataSet:
        """Stacks this `DataSet` vertically on top of an `other` `DataSet`
        (stacking, here, is in the sense of `numpy.vstack`, rows from `other`
        will be added below the rows of this DataSet. Any columns that `other`
        has but this doesn't have will be added)."""
        df = pd.concat(
            [self.data, other.data],
            axis=0,
            ignore_index=False
        )
        return DataSet(df)

    def hstack(self, other: DataSet) -> DataSet:
        """Stacks this `DataSet` horizontally to the left of `DataSet` `other`
        (stacking, here, is in the sense of `numpy.hstack`, columns from
        `other` will be added to the right of the columns of this DataSet.
        Any rows that `other` has but this doesn't have will be added)."""
        df = pd.concat(
            [self.data, other.data],
            axis=1,
            ignore_index=False
        )
        return DataSet(df)

    def store_in(self, name: str, store: pd.HDFStore) -> None:
        """ Save in the given `HDFStore` with the given name.
        Wrapped in case we want to add any pre-save logic.
        """
        store[name] = self.data

    @classmethod
    def load_from(cls, name: str, store: pd.HDFStore) -> DataSet:
        """ Load from the given `HDFStore` with the given name.
        Wrapped so we can perform any post-load logic.
        """
        df = pd.DataFrame()
        if name in store:
            df = store[name]
        else:
            df = pd.DataFrame()  # just create a DataSet around an empty df
        return cls(df)

    @classmethod
    def from_dataseries(
        cls,
        datas: List[DataSeries],
        time_tolerance: pd.Timedelta = pd.Timedelta('1us')
    ) -> DataSet:
        df = pd.DataFrame()
        if len(datas) == 0:
            df = pd.DataFrame()
        elif len(datas) == 1:
            df = datas[0]
        else:
            # Slice off the first dataset so it can serve as our base:
            df, *remaining_datas = datas
            df = df.data
            # Round time index to time_tolerance before merging:
            df.index = df.index.round(time_tolerance)
            # Successively merge in each data series by time (index), ensuring
            # all rows from both datasets are preserved but merging rows if
            # they have the same time value:
            for ds in remaining_datas:
                df2 = ds.data
                df2.index = df2.index.round(time_tolerance)
                df = df.join(df2, how='outer')

        return cls(df)

    @classmethod
    def from_tuples_dict(
        cls,
        data: Dict[str, List[Tuple[datetime, Any]]],
        time_tolerance: pd.Timedelta = pd.Timedelta('1us')
    ) -> DataSet:
        """
        Create from a dictionary that maps field names to lists of (time, data)
        tuples.
        When merging data series, events that happened within `time_tolerance`
        of each other will be considered to have happened at the same time (and
        thus be listed on the same row).
        """
        # Create a properly formatted DataSeries from each entry:
        datas = [
            DataSeries.from_tuples_list(name, points)
            for name, points in data.items()
            if len(points) > 0
        ]
        # Build DataSet:
        return cls.from_dataseries(datas, time_tolerance=time_tolerance)


def time_filter(opts, df: pd.DataFrame) -> pd.DataFrame:
    # Only keep telem from the specified time window
    # For example, can ignore about 12000 entries "from 2001" during a
    # timeserver glitch).
    if (
        opts.start_time_utc is not None
        # check for text in case someone misread the help and typed it out:
        and str(opts.start_time_utc).upper() != "NONE"
    ):
        start_bound = pd.to_datetime(str(opts.start_time_utc), utc=True)
        df = df[df.index >= start_bound]
    if (
        opts.end_time_utc is not None
        # check for text in case someone misread the help and typed it out:
        and str(opts.end_time_utc).upper() != "NONE"
    ):
        end_bound = pd.to_datetime(str(opts.end_time_utc), utc=True)
        df = df[df.index <= end_bound]
    return df


def main(opts):
    print(TITLE)

    in_file = os.path.join(opts.db_dir, f"{opts.name}.h5")
    out_file = os.path.join(opts.db_dir, f"{opts.name}_telem_export.xlsx")

    print(f"Connecting to Local DB: {in_file}. . .")
    db = pd.HDFStore(in_file)
    print("Loading Telem . . .")
    telem = DataSet.load_from('telem', db)
    telem.data = time_filter(opts, telem.data)

    print(f"Exporting Telem to Excel: {out_file}. . .")
    # Remove timezones (excel doesn't support) - convert to UTC:
    telem.data.index = telem.data.index.tz_convert(None)

    # Export:
    try:
        telem.data.to_excel(out_file, engine='xlsxwriter')
    except Exception as e:
        print(e)
        raise e

    print("Done.")


if __name__ == "__main__":
    main(get_opts())
    print("Closing . . .")
