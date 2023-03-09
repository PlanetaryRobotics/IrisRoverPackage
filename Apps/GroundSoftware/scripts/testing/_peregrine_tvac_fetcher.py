"""
YAMCS Testing Script from Peregrine TVAC.
Grabs the full history of all parameters Iris' PMCC has access to from a
YAMCS instance (which is running a linked DB archive).
NOTE: If this gives protobuf warnings, either resolve them or run:
- `PROTOCOL_BUFFERS_PYTHON_IMPLEMENTATION=python ./run-script.sh ...`
- instead of: `./run-script.sh ...`

NOTE: For some reason when spinning your own YAMCS server from docker in
Windows, ports 8090 and 443 don't work but 0 does (basically just says to find
it).
"""
# Activate postponed annotations:
from __future__ import annotations
from IrisBackendv3.data_standards import DataStandards
from IrisBackendv3.codec.packet import parse_packet
from IrisBackendv3.data_standards.logging import (
    logger as DsLogger,
    logger_setConsoleLevel as DsLoggerLevel
)
from IrisBackendv3.data_standards.prebuilt import (
    add_to_standards, ALL_PREBUILT_MODULES
)
from IrisBackendv3.codec.settings import set_codec_standards
import warnings

if __name__ == "__main__":
    # Loading dependencies can take a bit (builds DataStandards as part of
    # this). Acknowledge that it's doing something if running as main:
    print('Booting . . .')

from yamcs.client import YamcsClient, MDBClient, ArchiveClient  # type: ignore
from yamcs.tmtc.client import ProcessorClient  # type: ignore
from yamcs.tmtc.model import ParameterValue  # type: ignore
from yamcs.core.auth import Credentials as YamcsCredentials  # type: ignore

from typing import Any, Final, Tuple, List, Dict
from dataclasses import dataclass
from datetime import datetime, timedelta
import os.path

import argparse

from dataclasses import dataclass
from collections import OrderedDict
from termcolor import colored

from scripts.utils.trans_tools import *
from IrisBackendv3.codec.payload import EventPayload

import ulid
import numpy as np
import pandas as pd
import scapy.all as scp  # type: ignore

from matplotlib import pyplot as plt
from matplotlib import dates as mdates
from matplotlib import units as munits
from matplotlib import ticker as mticker
import seaborn as sns
plt.rcParams['text.usetex'] = False
sns.set()  # Initialize Seaborn. Use default theme.


# Suppress PerformanceWarning about pytables needing to pickle some fields
# (that's fine - this is a once daily script that runs in minutes, performance
# really doesn't matter here):
warnings.simplefilter(action='ignore', category=pd.errors.PerformanceWarning)

# Load Dependencies:

TITLE: Final[str] = 'IRIS Lunar Rover — Peregrine TVAC YAMCS Fetcher — CLI'

# All params Iris PMCC has access to:
IRIS_PARAMS: Final[List[str]] = [
    '/Peregrine/PL1/LSS1_Derived/LSS1_HK_Derived/Iris_Operational_EnabledFet',
    '/Peregrine/PL1/LSS1_Derived/LSS1_HK_Derived/Iris_Release_EnabledFet',
    '/Peregrine/PL1/LSS1_Derived/LSS1_HK_Derived/Iris_Operational_avgCurrent',
    '/Peregrine/PL1/LSS1_Derived/LSS1_HK_Derived/Iris_Release_avgCurrent',
    '/Peregrine/PL1/LSS1_Derived/LSS1_HK_Derived/Iris_Operational_maxCurrent',
    '/Peregrine/PL1/LSS1_Derived/LSS1_HK_Derived/Iris_Release_maxCurrent',
    '/Peregrine/PL1/SLIPsvc@3Status',
    '/Peregrine/PL1/WIFIsvc@3Status',
    '/Peregrine/FCPU/TMGR/TCS_DECKD_1/TCS_DECKD_1',
    '/Peregrine/FCPU/TMGR/TCS_DECKD_2/TCS_DECKD_2',
    '/Peregrine/FCPU/TMGR/TCS_DECKD_3/TCS_DECKD_3',
    '/Peregrine/payloads/iris/iris-payload-tm-rs422',
    '/Peregrine/payloads/iris/iris-payload-tm-wlan'
]
# Parameter that indicates when Iris has power:
IRIS_OPERATIONAL_POWER_PARAM: Final[str] = \
    '/Peregrine/PL1/LSS1_Derived/LSS1_HK_Derived/Iris_Operational_EnabledFet'
# Parameter(s) that contain Iris packets:
IRIS_TELEM_PARAMS: Final[List[str]] = [
    '/Peregrine/payloads/iris/iris-payload-tm-rs422',
    '/Peregrine/payloads/iris/iris-payload-tm-wlan'
]

# Columns to plot:
PLOT_TELEM_COLUMNS_AXIS_TEMP: Final[Dict[str, str]] = {
    'DeckD1': 'Peregrine_DeckD1TempKelvin',
    'DeckD2 (*closest)': 'Peregrine_DeckD2TempKelvin',
    'DeckD3': 'Peregrine_DeckD3TempKelvin',
    'IrisBattTempRT': 'WatchdogHeartbeat_BattAdcTempKelvin',
    'HeaterOnTemp': 'WatchdogDetailedStatus_Heater_OnTempKelvin',
    'HeaterOffTemp': 'WatchdogDetailedStatus_Heater_OffTempKelvin'
}
PLOT_TELEM_COLUMNS_AXIS_VOLTS: Final[Dict[str, str]] = {
    'V_Lander': 'WatchdogDetailedStatus_Adc_LanderVoltage',
    'Vcc28': 'WatchdogDetailedStatus_Adc_Vcc28Voltage',
    'V_BattSwitch (*no calib)':
    'WatchdogDetailedStatus_Adc_SwitchedBatteryVoltage',
    'V_Heater': 'WatchdogDetailedStatus_Heater_EffectiveVoltage'
}
PLOT_TELEM_COLUMNS_AXIS_WATTS: Final[Dict[str, str]] = {
    'P_PeregrineOperationalAvg': 'Peregrine_IrisOperationalAvgCurrent',  # * VLander
    'P_PeregrineOperationalMax': 'Peregrine_IrisOperationalMaxCurrent',  # * VLander
    'P_HeaterSetting': 'WatchdogDetailedStatus_Heater_EffectivePower',
    'P_HeaterUsing':
    'WatchdogDetailedStatus_Heater_EffectivePower'  # * IsHeating
}

# Re-build the `DataStandards` fresh from latest fprime and add in all prebuilt
# (WD Telem) Modules.
# These `DataStandards` will serve as TMTC definitions be used by the `Codec`
# layer used by the `Transceiver` layer to interpret packets.
DsLoggerLevel('CRITICAL')
standards = DataStandards.build_standards()
add_to_standards(standards, ALL_PREBUILT_MODULES)
set_codec_standards(standards)

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
    parser.add_argument('-s', '--server', default='localhost',
                        help='Server Address')
    parser.add_argument('--port', type=int, default=0,
                        help='Server Port')
    parser.add_argument('-i', '--instance', default='Astrobotic-M1',
                        help='YAMCS instance to connect to.')
    parser.add_argument('-r', '--processor', default='realtime',
                        help='YAMCS processor to use (eg. realtime vs replay)')
    parser.add_argument('-u', '--username', default=None,
                        help='YAMCS Username (skip if no cred. for local)')
    parser.add_argument('-p', '--password', default=None,
                        help='YAMCS Password (skip if no cred. for local)')
    parser.add_argument('--tls', default=False,
                        action=argparse.BooleanOptionalAction,
                        help='Whether or not to use TLS.')
    parser.add_argument('--only-iris-power-on-data', default=False,
                        action=argparse.BooleanOptionalAction,
                        help="Whether to only grab data from YAMCS for the "
                             "periods when Iris is given power.")
    parser.add_argument('--plot-start-time-utc', default="2023-01-01",
                        help="Earliest time to allow in the plot (UTC) "
                             "or `None` for no limits on start time. "
                             "Default is Jan. 1, 2023 (before start) because "
                             "some data in the archive was erroneously "
                             "labelled as being from 2001, so we need to not "
                             "plot that.")
    parser.add_argument('--plot-end-time-utc', default=None,
                        help="Latest time to allow in the plot (UTC) "
                             "or `None` for no limits on end time.")
    parser.add_argument('--just-plot', default=False,
                        action=argparse.BooleanOptionalAction,
                        help='Whether to just load data from DB and plot it.')
    parser.add_argument('--skip-plotting', default=False,
                        action=argparse.BooleanOptionalAction,
                        help='Whether to skip plotting after fetching data.')
    parser.add_argument('--plot-dpi', type=int, default=1200,
                        help='DPI for plots produced. '
                             'Higher = better resolution. '
                             'Lower = faster plots and smaller files.')
    parser.add_argument('--plot-interp-fill', default=True,
                        action=argparse.BooleanOptionalAction,
                        help="Whether or not plots should use linear "
                             "interpolation to fill time entries where "
                             "there's no data so every field has an entry for "
                             "every timestamp. Pro: smoother, easier to "
                             "follow curves for short time durations. Con: "
                             "might result in lines suggesting data was "
                             "present when there actually wasn't any (though "
                             "there are markers that are only on the actual "
                             "non-interpolated data points, which should make "
                             "them easy to distinguish).")
    parser.add_argument('--print-new-messages', default=True,
                        action=argparse.BooleanOptionalAction,
                        help='Whether to print new (non-alarm) messages.')
    parser.add_argument('--print-new-alarms', default=True,
                        action=argparse.BooleanOptionalAction,
                        help='Whether to print new alarms.')
    parser.add_argument('--print-stored-messages', default=False,
                        action=argparse.BooleanOptionalAction,
                        help='Whether to print (non-alarm) messages from DB.')
    parser.add_argument('--print-stored-alarms', default=False,
                        action=argparse.BooleanOptionalAction,
                        help='Whether to print alarms from DB.')

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


class YamcsInterface:
    """Wrapper for common YAMCS interface tasks."""
    # Private read-only:
    _server: Final[str]
    _port: Final[int]
    _instance: Final[str]
    _processor_name: Final[str]
    _username: Final[Optional[str]]
    _password: Final[Optional[str]]
    _tls: Final[bool]

    # Public:
    client: Optional[YamcsClient]
    mdb: Optional[MDBClient]
    archive: Optional[ArchiveClient]
    processor: Optional[ProcessorClient]

    def __init__(
        self,
        server: str,
        port: int,
        instance: str,
        processor: str,
        username: Optional[str] = None,
        password: Optional[str] = None,
        tls: bool = True,
        auto_begin: bool = True,
        **_  # allow (and ignore) extraneous kwargs - from opts.
    ) -> None:
        self._server = server
        self._port = port
        self._instance = instance
        self._processor_name = processor
        self._username = username
        self._password = password
        self._tls = tls

        self.client = None
        self.mdb = None
        self.processor = None
        self.archive = None

        if auto_begin:
            self.begin()

    def begin(self) -> None:
        self._obtain_client()
        self._fetch_mdb()
        self._obtain_archive_client()
        self._get_processor()

    def _obtain_client(self) -> None:
        if self.client is None:
            credentials: Optional[Any] = None
            if self._username is not None or self._password is not None:
                credentials = YamcsCredentials(
                    username=self._username,
                    password=self._password
                )
            self.client = YamcsClient(
                f"{self._server}:{self._port}",
                credentials=credentials,
                tls=(self._tls),
                tls_verify=(not self._tls)
            )

    def _fetch_mdb(self) -> None:
        """Fetch the MDB (Mission Database - YAMCS version of DataStandards)"""
        if self.mdb is None and self.client is not None:
            self.mdb = self.client.get_mdb(instance=self._instance)

    def _obtain_archive_client(self) -> None:
        """Obtain a client to access the data archive."""
        if self.archive is None and self.client is not None:
            self.archive = self.client.get_archive(self._instance)

    def _get_processor(self) -> None:
        """Gets a hook to the specified processor from the client."""
        if self.processor is None and self.client is not None:
            self.processor = self.client.get_processor(
                instance=self._instance,
                processor=self._processor_name
            )

    def get_param_names(self) -> List[str]:
        """Helper function that returns a list of the qualified names of all
        parameters available in the MDB."""
        names: List[str] = []
        if self.mdb is not None:
            params = [p for p in self.mdb.list_parameters()]
            names = [p.qualified_name for p in params]
        return names

    def get_param_history(
        self,
        name: str,
        start: Optional[datetime] = None,
        stop: Optional[datetime] = None,
        descending: bool = False
    ) -> List[ParameterValue]:
        """Gets a list of values for the parameter with the given name from
        the archive between the specified start and stop time (or all time if
        not specified).
        Values are returned in descending order (most recent first).

        :param name: parameter to query.
        :type name: str
        :param start: Datetime to start looking, defaults to `None`
            (beginning of time).
        :type start: Optional[datetime], optional
        :param stop: Datetime to stop looking, defaults to `None`
            (end of time).
        :type stop: Optional[datetime], optional
        :param descending: Most recent first if `True`, most recent last if
            `False`, defaults to `False`.
        :type descending: bool, optional
        :return: List of all parameter values matching the query.
        :rtype: List[ParameterValue]
        """
        history: List[ParameterValue] = []
        if self.archive is not None:
            history = self.archive.list_parameter_values(
                parameter=name,
                start=start,
                stop=stop,
                descending=descending,
                parameter_cache=self._processor_name
            )
        return [h for h in history]  # decompose generator

    def get_param_datapoints(
        self,
        name: str,
        start: Optional[datetime] = None,
        stop: Optional[datetime] = None,
        descending: bool = False
    ) -> List[Tuple[datetime, Any]]:
        """Creates a list of (generation_time, eng_value) datapoint tuples for
        the parameter with the given name from the archive between the
        specified start and stop time (or all time if not specified).
        Values are returned in descending order (most recent first).

        :param name: parameter to query.
        :type name: str
        :param start: Datetime to start looking, defaults to `None`
            (beginning of time).
        :type start: Optional[datetime], optional
        :param stop: Datetime to stop looking, defaults to `None`
            (end of time).
        :type stop: Optional[datetime], optional
        :param descending: Most recent first if `True`, most recent last if
            `False`, defaults to `False`.
        :type descending: bool, optional
        :return: List of all parameter values matching the query.
        :rtype: List[ParameterValue]
        """
        history = self.get_param_history(name, start, stop, descending)
        return [(p.generation_time, p.eng_value) for p in history]

    def get_params_dataset(
        self,
        params: List[str],
        start: Optional[datetime] = None,
        stop: Optional[datetime] = None
    ) -> DataSet:
        """
        Builds a `DataSet` out of the historical data for the given params over
        the time span specified (or all history if not specified).

        :param params: List of parameter names to dump.
        :type params: List[str]
        :param start: Datetime to start looking, defaults to `None`
            (beginning of time).
        :type start: Optional[datetime], optional
        :param stop: Datetime to stop looking, defaults to `None`
            (end of time).
        :type stop: Optional[datetime], optional
        """
        # Load all data lists:
        yamcs_data = {
            p: self.get_param_datapoints(p, start=start, stop=stop)
            for p in params
        }

        # Return as DataSet:
        return DataSet.from_tuples_dict(yamcs_data)


def load_iris_yamcs_data(opts) -> DataSet:
    """Loads all data from YAMCS that Iris has access to from the span over
    which Iris was given power."""
    # Connect to YAMCS:
    yamcs = YamcsInterface(**opts.__dict__, auto_begin=True)
    # Find the time range Iris had power:
    pwr_pts = yamcs.get_param_datapoints(IRIS_OPERATIONAL_POWER_PARAM)
    # Be very liberal with definition of "enabled" in case AMCC changes their
    # derived strings to some text other than "Enabled" and "Disabled", we want
    # this to still include all the data we're interested (bascially, make sure
    # this fails into including too much data rather than too little):
    pwr_on_times = [t for t, s in pwr_pts if "DISABLED" not in s.upper()]
    if len(pwr_on_times) > 0:
        pwr_on_start, pwr_on_end = min(pwr_on_times), max(pwr_on_times)
        print(f"\t Iris on span: {pwr_on_end - pwr_on_start}.")
    else:
        print(f"\t Iris not powered on in this YAMCS archive.")

    if (
        len(pwr_on_times) > 0 or (
            not opts.only_iris_power_on_data and len(pwr_pts) > 0)
    ):
        if opts.only_iris_power_on_data:
            # Pad cutoff times by 5 minutes (HK seems to be every ~4 mins):
            # (first telem burst preceed ENABLED signal by a few seconds):
            pwr_on_start -= timedelta(minutes=5)
            pwr_on_end += timedelta(minutes=5)
            start, stop = pwr_on_start, pwr_on_end
        else:
            # grab data for the full time period this archive covers:
            start, stop = None, None

        # Grab data for all params we care about for all the time care about:
        dataset = yamcs.get_params_dataset(
            IRIS_PARAMS, start=start, stop=stop
        )
    else:
        print(f"\t No relevant data in YAMCS. Skipping data load.")
        dataset = DataSet(pd.DataFrame())
    return dataset


def extract_packets(
    dataset: DataSet,
    packet_cols: List[str]
) -> Tuple[List[datetime], List[Packet]]:
    """
    Extracts packets that are contained in the columns `packet_cols` of the
    given dataset. Returns a tuple containing a time stamp list and a
    corresponding Packet list.
    """
    times: List[datetime] = []
    packets: List[Packet] = []
    # For every packet column that's actually present in the dataset:
    for col in packet_cols:
        if col in dataset.data:
            # Grab all non-NaN data:
            valid = dataset.data[col][~dataset.data[col].isna()]
            times.extend(valid.index.tolist())
            packets.extend(parse_packet(pb) for pb in valid.tolist())
    return times, packets


def packet_to_telem_row(time: datetime, packet: Packet) -> pd.DataFrame:
    """Extracts telemetry (and events) from the given timestamped packet and
    creates a DataFrame representing one row of "telemetry" for the telemetry
    archive (in quotes here because non-telem items are also contained)."""
    # Add packet as a field:
    packet_data: Dict[str, List[Any]] = {
        'time': [time],
        'packet': [str(packet)]
    }
    # Add all TelemetryPayloads:
    for p in packet.payloads[TelemetryPayload]:
        p = cast(TelemetryPayload, p)
        packet_data[f"{p.module.name}_{p.channel.name}"] = [p.data]
    # Add all EventPayloads as strings:
    for e in packet.payloads[EventPayload]:
        e = cast(EventPayload, e)
        packet_data[f"{e.module.name}_{e.event.name}"] = [str(e)]
    # Convert data into df:
    df = pd.DataFrame(packet_data)
    df.set_index('time', inplace=True)
    return df


def packet_extract_messages(packet: Packet) -> List[str]:
    """ Determines if the given packet represents a message (contains an
    event or itself is a message string (e.g. WatchdogDebug) <- note these
    message string only packets are being deprecated and will be converted to
    events but, for the time being, they have to be supported).
    If mesage string(s) (or events), a list of all strings will be returned
    (empty if none).
    """
    messages: List[str] = []

    # Extract telem and events:
    telem = [*packet.payloads[TelemetryPayload]]
    events = [*packet.payloads[EventPayload]]

    # Exceptions to the rule that no telem and no events = message packet.
    # Include these packet classes as messages, even though they have no telem
    # or events:
    msg_packet_include = [
        WatchdogCommandResponsePacket  # contain useful string
    ]
    # Exclude these packet classes as messages, even though they should count:
    msg_packet_exclude = [
        RadioUartBytePacket  # clog the logs if emitted
    ]

    # Determine if this is a message packet:
    # that is: if the packet doesn't contain any telemetry or events (i.e. log,
    # debug print, etc.):
    if (
        (len(telem) == 0 and len(events) == 0
         or isinstance(packet, tuple(msg_packet_include)))
        and not isinstance(packet, tuple(msg_packet_exclude))
    ):
        messages.append(str(packet))

    # Also list as a message if this is a DetailedStatus packet containing a
    # reset notice flag:
    if (
        isinstance(packet, WatchdogDetailedStatusPacket)
        and len(packet.custom_payload.Watchdog_ResetEventsList) != 0
    ):
        messages.append(
            f"Uncleared reset flags: "
            f"{', '.join(packet.custom_payload.Watchdog_ResetEventsList)}"
        )

    # Also add all events (do this after packet so they appear below packet).
    messages.extend(str(e) for e in events)

    return messages


def check_packet_for_alarms(packet: Packet) -> List[str]:
    """Checks if there's anything we should be alarmed about in this packet.
    Returns a list of strings of all alarm messages (empty if no alarms)."""
    alarms: List[str] = []
    # Extract telem:
    telem = [
        cast(TelemetryPayload, t) for t in packet.payloads[TelemetryPayload]]
    data = {f'{t.channel.name}': t for t in telem}

    def range_check(
        key: str,
        printed_name: str,
        units: str,
        allowed_range: Tuple[Any, Any]
    ) -> bool:
        val = data[key].data
        bad: bool = False
        if val < allowed_range[0]:
            alarms.append(
                f"{printed_name.upper()} LOW: "
                f"{val}{units} < {allowed_range[0]}{units}"
            )
            bad = True
        if val > allowed_range[1]:
            alarms.append(
                f"{printed_name.upper()} HIGH: "
                f"{val}{units} > {allowed_range[1]}{units}"
            )
            bad = True
        return bad

    # Check for issues:
    if isinstance(packet, WatchdogHeartbeatPacket):
        # Check for out of range temperature:
        battRangeKelvin = (273.15 - 15, 273.15 + 35)
        range_check('BattAdcTempKelvin', 'BATTERY TEMP', 'K', battRangeKelvin)
        # Check for possible thermistor fault (open circuit):
        battTempAdc = data['BattAdcTempRaw'].data
        if battTempAdc > 4040:
            alarms.append(
                f"BATT RT THERMISTOR BREAK: ADC reading of {battTempAdc} "
                f"> 4040 suggests that a thermistor break has occurred."
            )
        if battTempAdc < 10:
            alarms.append(
                f"BATT RT THERMISTOR SHORT: ADC reading of {battTempAdc} < 10 "
                f"suggests that a thermistor short has occurred."
            )

    if isinstance(packet, WatchdogDetailedStatusPacket):
        charger_state = data['Io_ChargerState'].data
        if charger_state not in ['OFF', 0b00]:
            alarms.append(f"CHARGER IS NOT OFF: state is {charger_state}.")

        batt_conn_state = data['Io_BatteryState'].data
        if batt_conn_state not in ['DISCONNECTED', 0b00]:
            alarms.append(
                f"BATT IS NOT DISCONNECTED: state is {batt_conn_state}.")

        wd_state = data['Watchdog_State'].data
        if wd_state not in [
            'RS_INIT', 0,
            'RS_ENTERING_KEEP_ALIVE', 7,
            'RS_KEEP_ALIVE', 8
        ]:
            alarms.append(f"WD IS IN DISALLOWED STATE: {wd_state}.")

        dep_state = data['Watchdog_DeploymentStatus'].data
        if dep_state not in ['NOT_DEPLOYED', 0b00]:
            alarms.append(f"ROVER IS NOT NOT_DEPLOYED: {dep_state}.")

        herc_uart_state = data['Watchdog_Uart0State'].data
        if herc_uart_state not in ['OFF', 0x00]:
            alarms.append(
                f"HERCULES UART IS NOT OFF: state is {herc_uart_state}.")

        lander_uart_state = data['Watchdog_Uart1State'].data
        if lander_uart_state not in ['INITIALIZED_&_ACTIVE', 0x01]:
            alarms.append(
                f"LANDER UART IS NOT ON: state is {lander_uart_state}.")

        # Make sure all important digital states are in a safe value.
        # List of (pin_name, [allowed_states]) tuples for all important digital
        # pins:
        important_digital_states: List[Tuple[str, List[int]]] = [
            ('V_LANDER_REG_EN', [0]),
            ('DEPLOYMENT', [0]),
            ('LATCH_BATT', [0]),
            ('3V3_EN', [0]),
            ('HERCULES_ON', [0]),
            ('FPGA_ON', [0]),
            ('MOTOR_ON', [0]),
            ('CHRG_EN', [0]),  # input capable (0b10=Hi-Z)
            ('BATTERY_EN', [0]),
            ('V_SYS_ALL_EN', [0, 2]),  # input capable (0b10=Hi-Z)
            ('HERCULES_N_RST', [0]),  # input capable (0b10=Hi-Z)
            ('HERCULES_N_PORRST', [0]),  # input capable (0b10=Hi-Z)
            ('FPGA_N_RST', [0]),  # input capable (0b10=Hi-Z)
            ('RADIO_N_RST', [0]),  # input capable (0b10=Hi-Z)
            ('RADIO_ON', [0]),
            ('LATCH_SET', [2]),
            ('LATCH_RESET', [2]),
            ('BATT_STAT', [2])
        ]
        # Check the current digital states:
        cust_payload = packet.custom_payload
        digital_states = cust_payload.Watchdog_CombinedDigitalStates_Dict
        for pin_name, allowed_states in important_digital_states:
            if digital_states[pin_name] not in allowed_states:
                alarms.append(
                    f"BAD PIN STATE: digital pin {pin_name} has state "
                    f"{digital_states[pin_name]} which is not one of the "
                    f"allowed states: {allowed_states}."
                )

        # Make sure lander voltage is within spec:
        # NOTE: Adc_LanderVoltage is higher res than Vcc28. Vcc28 just provides
        # redundancy.
        VLanderRange = (0.9 * 28, 1.1 * 28)
        range_check('Adc_LanderVoltage', 'LANDER VOLTAGE', 'V', VLanderRange)

        # Make sure no subsystems are powered somehow:
        VSysAll = data['Adc_FullSystemVoltage'].data
        if VSysAll > 5:
            alarms.append(
                f"V_SYS_ALL ON or FLOATING HIGH: {VSysAll:.3f} > 5V (should be "
                f"~1.2V at STP when off)."
            )
        VBattSw = data['Adc_SwitchedBatteryVoltage'].data
        if VBattSw > 10:
            alarms.append(
                f"BATTERY ON or FLOATING HIGH: {VBattSw:.3f} > 10V. This sensor "
                f"is only loosely calibrated should be less than ~5 at STP "
                f"when off)."
            )
        Vcc24 = data['Adc_Vcc24Voltage'].data
        if Vcc24 > 5:
            alarms.append(
                f"24V MOTOR PWR ON or FLOATING HIGH: {Vcc24:.3f} > 5V (should "
                f"be ~0.5V at STP when off)."
            )

    return alarms


def print_message(
    time: datetime, message: str,
    color: str = 'blue',
    prefix: str = ''
) -> None:
    """Prints the given timestamped message."""
    print(
        prefix
        + colored(
            f" {time.strftime('%m-%d %H:%M:%S')} ",
            'grey', f'on_{color}', attrs=['bold']
        )
        + " "
        + colored(message, color, attrs=['bold'])
    )


def process_packets(opts, dataset: DataSet, packet_cols: List[str]) -> DataSet:
    """
    Processes packets that are contained in the columns `packet_cols` of the
    given dataset.
    """
    # Extract all the packets:
    times, packets = extract_packets(dataset, packet_cols)

    # Print new messages if requested:
    if opts.print_new_messages:
        # List of (time, message) tuples to print:
        print_messages: List[Tuple[datetime, str]] = []
        for time, packet in zip(times, packets):
            messages = packet_extract_messages(packet)
            print_messages.extend((time, m) for m in messages)
        print("\tIris Messages: ")
        for time, message in print_messages:
            print_message(time, message, 'blue', '\t\t')

    # Print new alarms if requested:
    if opts.print_new_alarms:
        print_alarms: List[Tuple[datetime, str]] = []
        for time, packet in zip(times, packets):
            alarms = check_packet_for_alarms(packet)
            print_alarms.extend((time, a) for a in alarms)
        print("\tIris Alarms: ")
        for time, message in print_alarms:
            print_message(time, message, 'red', '\t\t')

    # Build the dataset:
    telem_rows = [packet_to_telem_row(t, p) for t, p in zip(times, packets)]
    data = pd.concat([*telem_rows], axis=0, ignore_index=False)

    # Build and return DataSet:
    return DataSet(data)


def process_peregrine_data(dataset: DataSet) -> DataSet:
    # Transpose the Peregrine Parameters into Iris GDS Telemetry:
    df = pd.DataFrame()
    df['Peregrine_IrisOperationalEnabledFet'] = dataset.data[
        '/Peregrine/PL1/LSS1_Derived/'
        'LSS1_HK_Derived/Iris_Operational_EnabledFet']
    df['Peregrine_IrisReleaseEnabledFet'] = dataset.data[
        '/Peregrine/PL1/LSS1_Derived/'
        'LSS1_HK_Derived/Iris_Release_EnabledFet']
    df['Peregrine_IrisOperationalAvgCurrent'] = dataset.data[
        '/Peregrine/PL1/LSS1_Derived/'
        'LSS1_HK_Derived/Iris_Operational_avgCurrent']
    df['Peregrine_IrisReleaseAvgCurrent'] = dataset.data[
        '/Peregrine/PL1/LSS1_Derived/'
        'LSS1_HK_Derived/Iris_Release_avgCurrent']
    df['Peregrine_IrisOperationalMaxCurrent'] = dataset.data[
        '/Peregrine/PL1/LSS1_Derived/'
        'LSS1_HK_Derived/Iris_Operational_maxCurrent']
    df['Peregrine_IrisReleaseMaxCurrent'] = dataset.data[
        '/Peregrine/PL1/LSS1_Derived/'
        'LSS1_HK_Derived/Iris_Release_maxCurrent']
    df['Peregrine_SlipServiceStatus'] = dataset.data[
        '/Peregrine/PL1/SLIPsvc@3Status']
    df['Peregrine_WifiServiceStatus'] = dataset.data[
        '/Peregrine/PL1/WIFIsvc@3Status']
    df['Peregrine_DeckD1TempKelvin'] = dataset.data[
        '/Peregrine/FCPU/TMGR/TCS_DECKD_1/TCS_DECKD_1'] + 273.15  # C to K
    df['Peregrine_DeckD2TempKelvin'] = dataset.data[
        '/Peregrine/FCPU/TMGR/TCS_DECKD_2/TCS_DECKD_2'] + 273.15  # C to K
    df['Peregrine_DeckD3TempKelvin'] = dataset.data[
        '/Peregrine/FCPU/TMGR/TCS_DECKD_3/TCS_DECKD_3'] + 273.15  # C to K
    df.index = dataset.data.index
    return DataSet(df)


def plot_tvac_telem(opts, telem: DataSet) -> None:
    # Convenient handle (NOTE: NOT a copy, just a different view on FS DB data)
    plot_data = telem.data
    # Filter telem data by plotting start and end times:
    if (
        opts.plot_start_time_utc is not None
        # check for text in case someone misread the help and typed it out:
        and opts.plot_start_time_utc.upper() != "NONE"
    ):
        start_bound = pd.to_datetime(str(opts.plot_start_time_utc), utc=True)
        plot_data = plot_data[plot_data.index >= start_bound]
    if (
        opts.plot_end_time_utc is not None
        # check for text in case someone misread the help and typed it out:
        and opts.plot_end_time_utc.upper() != "NONE"
    ):
        end_bound = pd.to_datetime(str(opts.plot_end_time_utc), utc=True)
        plot_data = plot_data[plot_data.index <= end_bound]

    # Copy over and label the columns of interest:
    df_temp = pd.DataFrame()
    for label, column in PLOT_TELEM_COLUMNS_AXIS_TEMP.items():
        df_temp[label] = plot_data[column]
    df_temp.index = plot_data.index
    df_volts = pd.DataFrame()
    for label, column in PLOT_TELEM_COLUMNS_AXIS_VOLTS.items():
        df_volts[label] = plot_data[column]
    df_volts.index = plot_data.index
    df_watts = pd.DataFrame()
    for label, column in PLOT_TELEM_COLUMNS_AXIS_WATTS.items():
        df_watts[label] = plot_data[column]
    df_watts.index = plot_data.index

    # Make sure the heater's calculated effective voltage field only shows
    # voltage when it's actually heating:
    # for some reason this bool ends up being stored as a float so we can just
    # multiply by it (I think this is b/c the field is an enum, which is backed
    # by an int (so not a bool) which gets converted to float).
    is_heating_field = 'WatchdogDetailedStatus_Heater_IsHeating'
    is_heating_float = plot_data[is_heating_field].astype(float)
    df_volts['V_Heater'] = df_volts['V_Heater'] * is_heating_float

    # Apply the same thing to power:
    df_watts['P_HeaterUsing'] = df_watts['P_HeaterUsing'] * is_heating_float

    # And combine Iris voltage readings with Peregrine current readings to
    # approx. how much power Peregrine sees us using:
    # First create interp versions of both fields so they all have the same
    # rows:
    I_OperationalAvg_interp = plot_data['Peregrine_IrisOperationalAvgCurrent']
    I_OperationalAvg_interp = I_OperationalAvg_interp.interpolate('linear')
    I_OperationalMax_interp = plot_data['Peregrine_IrisOperationalMaxCurrent']
    I_OperationalMax_interp = I_OperationalMax_interp.interpolate('linear')
    V_Lander_interp = plot_data['WatchdogDetailedStatus_Adc_LanderVoltage']
    V_Lander_interp = V_Lander_interp.interpolate('linear')
    # Calc fields:
    df_watts['P_PeregrineOperationalAvg'] = \
        V_Lander_interp * I_OperationalAvg_interp
    df_watts['P_PeregrineOperationalMax'] = \
        V_Lander_interp * I_OperationalMax_interp

    # Set up date formatting:
    locator = mdates.AutoDateLocator()
    formatter = mdates.ConciseDateFormatter(locator)
    formatter.formats = ['%y',       # ticks are mostly years
                         '%b',       # ticks are mostly months
                         '%d',       # ticks are mostly days
                         '%H:%M',    # hrs
                         '%H:%M',    # min
                         '%S.%f', ]  # secs
    # these are mostly just the level above...
    formatter.zero_formats = [''] + formatter.formats[:-1]
    # ...except for ticks that are mostly hours, then it is nice to have
    # month-day:
    formatter.zero_formats[3] = '%d-%b'
    formatter.offset_formats = ['UTC%z',
                                '%Y UTC%z',
                                '%b %Y UTC%z',
                                '%d %b %Y UTC%z',
                                '%d %b %Y UTC%z',
                                '%d %b %Y %H:%M UTC%z']

    # Make the figure:
    fsize = (14, 15)
    fig, axs = plt.subplots(
        nrows=2,
        figsize=fsize,
        dpi=opts.plot_dpi,
        constrained_layout=True
    )
    fig.suptitle('Iris Integrated TVAC')
    colors_set = ['#1f77b4', '#ff7f0e', '#2ca02c', '#d62728', '#9467bd',
                  '#8c564b', '#e377c2', '#7f7f7f', '#bcbd22', '#17becf']
    colors = deque(colors_set)

    # Create secondary y-axis(es) on the Volts plot:
    axs = axs.tolist()  # convert from numpy array
    ax_volts = axs[1]
    for i in range(num_extra_y_axes := 1):
        axs.append(ax_volts.twinx())
        axs[-1].spines['right'].set_position(('axes', 1.0 + i * 0.05))

    # Assign which axes (axis indices) are on which plots (plot indices):
    plot_axes = OrderedDict([(0, [0]), (1, [1, 2])])

    # Draw interp with smaller markers beneath main plots if requested:
    # (fill in NaN entries in each time row, so every field has an entry for
    # every timestamp)
    def plot_interp(xy_label: Tuple[str, str], df: pd.DataFrame, ax_idx: int):
        """DRY definition of how to handle interpolated background plots:"""
        df_interp = df.interpolate(method='linear', inplace=False)
        df_interp.plot(
            figsize=fsize,
            grid=True,
            ax=axs[ax_idx],
            xlabel=xy_label[0],
            ylabel=xy_label[1],
            legend=False,
            color=[*colors],
            marker='.', markersize=1, linestyle='none'
        )
        colors.rotate(-len(df.columns))  # advance color cycle

    if opts.plot_interp_fill:
        plot_interp(('', 'Temperature [K]'), df_temp, 0)
        colors = deque(colors_set)  # reset color order between subplots
        plot_interp(('generated_time', 'Volts'), df_volts, 1)
        plot_interp(('generated_time', 'Power [W]'), df_watts, 2)
        # Reset the color cycle so actual datapoints end up using the same
        # colors:
        colors = deque(colors_set)
        for ax in axs:
            ax.set_prop_cycle(None)

    # Plot the actual data **points** (only observations):
    def plot_main(
        xy_label: Tuple[str, str],
        df: pd.DataFrame,
        ax_idx: int
    ):
        """DRY definition of how to handle main plots:"""
        df.plot(
            figsize=fsize,
            grid=True,
            ax=axs[ax_idx],
            xlabel=xy_label[0],
            ylabel=xy_label[1],
            legend=False,
            color=[*colors],
            marker='o', markersize=2, linestyle='none'
        )
        colors.rotate(-len(df.columns))  # advance color cycle

    plot_main(('', 'Temperature [K]'), df_temp, 0)
    colors = deque(colors_set)  # reset color order between subplots
    plot_main(('generated_time', 'Volts'), df_volts, 1)
    plot_main(('generated_time', 'Power [W]'), df_watts, 2)

    # Build and apply legends to each plot:
    for plot_idx, ax_idxs in plot_axes.items():
        handles, legends = [], []
        for ax_idx in ax_idxs:
            hdl, leg = axs[ax_idx].get_legend_handles_labels()
            handles.extend(hdl)
            legends.extend(leg)
        # Only keep the last appearance of each legend name and its
        # corresponding handle:
        # (there will be duplicates if interp plots are added behind):
        prune = [x in legends[i + 1:] for i, x in enumerate(legends)]
        handles = [x for i, x in enumerate(handles) if not prune[i]]
        legends = [x for i, x in enumerate(legends) if not prune[i]]

        # Apply to *last* pair of axes on the plot (so it's on top):
        axs[ax_idxs[-1]].legend(
            handles, legends,
            markerscale=3.5,
            shadow=False,
            framealpha=0.85
        )

    # Add second y-axis to temp plot to show temp in degC:
    ax_degK = axs[0]
    # Create a clone secondary axis (unity conversion functions) so that the
    # grid lines & ticks remain in exactly the same place.
    ax_degC = ax_degK.secondary_yaxis(
        'right',
        functions=(lambda T: T, lambda T: T)
    )

    # Then just use a label formatting function to change what we display:
    def degK_to_degCLabel(tempK: float, *_) -> str:
        return f"{(tempK - 273.15):.1f}"
    ax_degC.yaxis.set_major_formatter(mticker.FuncFormatter(degK_to_degCLabel))
    ax_degC.set_ylabel('Temperature [°C]')

    # Align ticks on both y-axes on volts/watts plot:
    n_ticks = max(len(axs[i].get_yticks()) for i in plot_axes[1])
    for i in plot_axes[1]:
        axs[i].yaxis.set_major_locator(mticker.LinearLocator(n_ticks))
        axs[i].yaxis.set_major_formatter(mticker.FormatStrFormatter('%.2f'))
        # Make sure ticks don't extend beyond the range of the data:
        axs[i].autoscale(enable=True, axis='y', tight=True)
        # Save the new tick values:
        ticks = axs[i].get_yticks()
        tick_labels = axs[i].get_yticklabels()
        # # Add some margin to the y limits *after* autoscaling:
        tight_ylim = axs[i].get_ylim()
        ylim_range = tight_ylim[1] - tight_ylim[0]
        new_ylim = [tight_ylim[0] - 0.05 * ylim_range,
                    tight_ylim[1] + 0.05 * ylim_range]
        axs[i].set_ylim(new_ylim, auto=False)
        # # Reapply tick labels:
        axs[i].set_yticks(ticks)
        axs[i].set_yticklabels(tick_labels)

    # Make sure axes have the same xlims so they're comparable (choose the
    # widest range):
    xlims = [ax.get_xlim() for ax in axs]
    xlim_wide = [min(a for a, _ in xlims), max(b for _, b in xlims)]
    # Make sure the xlims still conform to the time bounds:
    if (
        opts.plot_start_time_utc is not None
        # check for text in case someone misread the help and typed it out:
        and opts.plot_start_time_utc.upper() != "NONE"
    ):
        start_bound = pd.to_datetime(str(opts.plot_start_time_utc), utc=True)
        start_bound = mdates.date2num(start_bound)
        xlim_wide[0] = max(xlim_wide[0], start_bound)
    if (
        opts.plot_end_time_utc is not None
        # check for text in case someone misread the help and typed it out:
        and opts.plot_end_time_utc.upper() != "NONE"
    ):
        end_bound = pd.to_datetime(str(opts.plot_end_time_utc), utc=True)
        end_bound = mdates.date2num(end_bound)
        xlim_wide[1] = min(xlim_wide[1], end_bound)
    # Apply limits:
    for ax in axs:
        ax.set_xlim(xlim_wide)

    # Apply time formatting to x axes:
    for ax in axs:
        ax.xaxis.set_major_locator(locator)
        ax.xaxis.set_major_formatter(formatter)

    # Save the results:
    plots_ulid = ulid.new()
    out_file = os.path.join(
        opts.db_dir,
        "plots",
        f"{opts.name}_{plots_ulid!s}.png"
    )
    plt.savefig(out_file, dpi=opts.plot_dpi)
    plt.close()
    print(f"Results saved to: {out_file}")

    # Make a second plot for Tdot:
    print("Plotting Thermal Time Derivative . . .")
    # Compute time derivatives for tempertatures:
    print("\t Computing ...")

    def compute_TDot(s):
        """Computes Tdot for series in degC/hr"""
        # Make *sure* data is sorted by time:
        series_sorted = s.sort_index()
        # Remove any NA values:
        series = series_sorted[~series_sorted.isna()]
        # Drop any rows where the value hasn't changed:
        # (since the data is discretized, these changes are often abrupt and
        # would cause a reading of 10,000s of degC/hr over the very short time
        # sample between data points, so we want to remove the constant values)
        series = series.loc[series.diff() != 0]

        def core_compute_DTdt(s):
            """Core computation, minus any filtering:"""
            dt_ms = np.diff(s.index.values).astype('timedelta64[ms]')
            dt_ms = dt_ms.astype('int64')
            # Compute diff:
            diff = s.diff()[1:]
            # Drop any rows with 0ms DT (repeat values from dataset overlap):
            keep = (dt_ms != 0)
            dt_ms = dt_ms[keep]
            diff = diff[keep]
            # Drop any remaining NaN rows:
            keep = ~diff.isna()
            dt_ms = dt_ms[keep]
            diff = diff[keep]
            # Convert to time derivative:
            return diff.div(dt_ms) * 3_600_000

        def remove_outliers(DTdt, factor: float):
            """Core operation to remove outliers:"""
            # Do some basic IQR outlier removal (surprisingly effective):
            q1, q3 = DTdt.quantile([0.25, 0.75])
            IQR = abs(q3 - q1)
            DTdt = DTdt[DTdt > (q1 - factor * IQR)]
            DTdt = DTdt[DTdt < (q3 + factor * IQR)]
            return DTdt

        # First pass:
        DTdt = core_compute_DTdt(series)
        DTdt = remove_outliers(DTdt, 1.0)

        # # To recreate smoothed data (without time jumps):
        # # - go back to original data,
        # # - only select rows where the timestamp matches a row were the data is
        # # - under control:
        # keep = series_sorted[DTdt.index]
        # # NaN out everything else:
        # # copy to prevent modifying original data
        # series_filtered = series_sorted.copy()
        # series_filtered.iloc[:] = np.nan
        # # Add back kept values:
        # series_filtered[series_filtered.index.isin(DTdt.index)] = keep.values
        # # Interp to fill in gaps:
        # series_filtered.interpolate('linear', inplace=True)
        # # Drop any remaining NAN:
        # series_filtered.dropna(inplace=True)
        # # There are still some duplicates in the dataset, drop them (diff=0):
        # series_filtered = series_filtered.loc[series_filtered.diff() != 0]
        # # Second pass:
        # DTdt = core_compute_DTdt(series_filtered)
        # DTdt = remove_outliers(DTdt, 10.0)

        df = pd.DataFrame({
            'time': DTdt.index,
            DTdt.name: DTdt
        })
        return df

    df_Tdot_KperHr = pd.DataFrame()
    for c in df_temp.columns:
        col_Tdot = compute_TDot(df_temp[c])
        # Stack vertically:
        df_Tdot_KperHr = pd.concat(
            [df_Tdot_KperHr, col_Tdot],
            axis=0,
            ignore_index=False
        )
    df_Tdot_KperHr.set_index('time', inplace=True)
    # Sort all the data:
    df_Tdot_KperHr.sort_index(ascending=True, inplace=True)

    # Print statistics on the changes:
    print("\t\t Descriptive Statistics:")
    print(df_Tdot_KperHr.describe())

    print("\t Plotting ...")
    # Only plot for the Iris temperature (deck data too noisy, possibly due to
    # sparkle filter?)
    df_Tdot_KperHr = df_Tdot_KperHr['IrisBattTempRT'].to_frame()
    fsize = (14, 8)
    fig, axs = plt.subplots(
        nrows=1,
        figsize=fsize,
        dpi=opts.plot_dpi,
        constrained_layout=True
    )
    fig.suptitle(
        "Iris Integrated TVAC Temperature Rates"
    )
    if not isinstance(axs, list):
        axs = [axs]
    ax = axs[0]
    colors = deque(colors_set)  # reset color order between subplots
    plot_interp(('', 'Temperature Rate [K/hr]'), df_Tdot_KperHr, 0)
    # Apply time formatting to x axes:
    ax.xaxis.set_major_locator(locator)
    ax.xaxis.set_major_formatter(formatter)
    # Make sure the ylims are reasonable for the TDot plot:
    ylims = ax.get_ylim()
    ylims = [max(ylims[0], -50), min(50, ylims[1])]
    ax.set_ylim(ylims)

    ax.legend(
        markerscale=3.5,
        shadow=False,
        framealpha=0.85
    )

    # Save the Tdot results:
    out_file = os.path.join(
        opts.db_dir,
        "plots",
        f"{opts.name}_TDOT_{plots_ulid!s}.png"
    )
    plt.savefig(out_file, dpi=opts.plot_dpi)
    plt.close()
    print(f"Results saved to: {out_file}")


def plot_from_db(opts) -> None:
    """Helper function to just plot stored data if you're running this from a
    console."""
    print("Connecting to Local DB . . .")
    db = pd.HDFStore(os.path.join(opts.db_dir, f"{opts.name}.h5"))
    print("Loading Telem . . .")
    telem = DataSet.load_from('telem', db)
    print("Plotting TVAC Progress . . .")
    plot_tvac_telem(opts, telem)


def print_from_db(opts) -> None:
    """Helper function to extract and print stored messages if you're running
    this from a console."""

    print("Connecting to Local DB . . .")
    db = pd.HDFStore(os.path.join(opts.db_dir, f"{opts.name}.h5"))
    print("Loading Stored YAMCS Data . . .")
    lander_data = DataSet.load_from('yamcs', db)

    # There are duplicates of all rows in some time windows due to overlaps in
    # YAMCS archives that were replayed and reprocessing of some archives.
    # Only keep the first row of a collection of rows with the **exact** same
    # timestamp:
    keep = ~lander_data.data.index.duplicated(keep='first')
    lander_data.data = lander_data.data[keep]

    # Extract all the packets:
    print("Extracting and processing all Iris Packets. . .")
    times, packets = extract_packets(lander_data, IRIS_TELEM_PARAMS)
    print(f"\t{len(packets)} packets found.")

    # Print new messages if requested:
    if opts.print_stored_messages:
        print(f"Finding Messages . . .")
        # List of (time, message) tuples to print:
        print_messages: List[Tuple[datetime, str]] = []
        for time, packet in zip(times, packets):
            messages = packet_extract_messages(packet)
            print_messages.extend((time, m) for m in messages)
        print("\tIris Messages: ")
        for time, message in print_messages:
            print_message(time, message, 'blue', '\t\t')
        print(f"Printed {len(print_messages)} messages.")

    # Print new alarms if requested:
    if opts.print_stored_alarms:
        print(f"Finding Alarms . . .")
        print_alarms: List[Tuple[datetime, str]] = []
        for time, packet in zip(times, packets):
            alarms = check_packet_for_alarms(packet)
            print_alarms.extend((time, a) for a in alarms)
        print("\tIris Alarms: ")
        for time, message in print_alarms:
            print_message(time, message, 'red', '\t\t')
        print(f"Printed {len(print_alarms)} alarms.")


def fetch_and_plot_new_data_from_yamcs(opts) -> None:
    """Fetch new data from YAMCS, extract telemetry, store it, and plot the
    results."""
    # Connect to database (or create if it doesn't exist yet):
    print("Connecting to Local DB . . .")
    db = pd.HDFStore(os.path.join(opts.db_dir, f"{opts.name}.h5"))

    # Load any existing YAMCS data:
    old_yamcs = DataSet.load_from('yamcs', db)
    # Load all new data from this YAMCS archive:
    print("Fetching YAMCS Data . . .")
    new_yamcs = load_iris_yamcs_data(opts)

    # If no new data was found in this YAMCS archive, just exit:
    if len(new_yamcs.data.index) == 0:
        return

    # Stack the DataSets:
    print("Saving YAMCS Data . . .")
    yamcs_data = old_yamcs.stack(new_yamcs)
    # Store the combined dataset:
    yamcs_data.store_in('yamcs', db)

    # Load existing generated data:
    old_telem = DataSet.load_from('telem', db)
    # Extract telem from **new** data:
    print("Parsing New Iris Packets . . .")
    new_telem = process_packets(opts, new_yamcs, IRIS_TELEM_PARAMS)
    # Generate new data fields from Peregrine data:
    print("Ingesting Peregrine Telemetry . . .")
    peregrine_telem = process_peregrine_data(new_yamcs)
    # Stack it horizontally with Iris packet data:
    new_telem = new_telem.hstack(peregrine_telem)
    # Stack the new and old DataSets:
    telem = old_telem.stack(new_telem)

    print("Saving Generated Data . . .")
    # Store the combined dataset:
    telem.store_in('telem', db)

    # Plot results:
    if opts.skip_plotting:
        print("Skipping plotting by CLI argument.")
    else:
        print("Plotting TVAC Progress . . .")
        plot_tvac_telem(opts, telem)


def main(opts):
    print(TITLE)

    if opts.print_stored_messages or opts.print_stored_alarms:
        print_from_db(opts)
    elif opts.just_plot:
        plot_from_db(opts)
    else:
        fetch_and_plot_new_data_from_yamcs(opts)

    print("Done.")


if __name__ == "__main__":
    main(get_opts())
    print("Closing . . .")
