Module IrisBackendv3.storage.dataset
====================================
Collection for representing and storing archived data from an entire activity
/ mission.

Presently, this is handled as a light wrapper for a DataFrame so all the data
processing tools that come with pandas can be used here too.

TODO: Migrate all Iris-specific table generation (specifically, for Excel) out
to a `config` or `ipc_app` file.

@author: Colombo, CMU
@last-updated: 10/06/2024

Classes
-------

`DataSeries(data: pd.DataFrame)`
:   Series of timestamped data-points
    (DataFrame with a TimeSeries Datetimeindex of times and a column of values)

    ### Class variables

    `data: pandas.core.frame.DataFrame`
    :

    ### Static methods

    `from_lists(name: str, times: List[datetime], values: List[Any]) ‑> IrisBackendv3.storage.dataset.DataSeries`
    :

    `from_tuples_list(name: str, data: List[Tuple[datetime, Any]]) ‑> IrisBackendv3.storage.dataset.DataSeries`
    :

`DataSet(data: pd.DataFrame)`
:   Collection of data containing multiple named DataSeries comprised of a
    multiple timestamped data points
    (basically just a way for merging a bunch of Timeseries DataFrames into
    one).

    ### Class variables

    `data: pandas.core.frame.DataFrame`
    :

    ### Static methods

    `from_dataseries(datas: List[DataSeries], time_tolerance: pd.Timedelta = Timedelta('0 days 00:00:00.000001')) ‑> IrisBackendv3.storage.dataset.DataSet`
    :

    `from_tuples_dict(data: Dict[str, List[Tuple[datetime, Any]]], time_tolerance: pd.Timedelta = Timedelta('0 days 00:00:00.000001')) ‑> IrisBackendv3.storage.dataset.DataSet`
    :   Create from a dictionary that maps field names to lists of (time, data)
        tuples.
        When merging data series, events that happened within `time_tolerance`
        of each other will be considered to have happened at the same time (and
        thus be listed on the same row).

    `load_from_file(file_path: str, key: str) ‑> IrisBackendv3.storage.dataset.DataSet`
    :   General archive file loader. Loads archive from the given archive
        file. Accesses using the given key if the archive is keyed.
        Supports `h5` and `parquet`.

    `load_from_h5(file_path: str, key: str) ‑> IrisBackendv3.storage.dataset.DataSet`
    :   Loads from the given key in an HDFStore in an `h5` file, including
        managing the lifecycle of the DB accessor.

    `load_from_parquet(file_path: str) ‑> IrisBackendv3.storage.dataset.DataSet`
    :   Loads archive from the given parquet file.

    ### Methods

    `hstack(self, other: DataSet) ‑> IrisBackendv3.storage.dataset.DataSet`
    :   Stacks this `DataSet` horizontally to the left of `DataSet` `other`
        (stacking, here, is in the sense of `numpy.hstack`, columns from
        `other` will be added to the right of the columns of this DataSet.
        Any rows that `other` has but this doesn't have will be added).

    `save_to_excel(self, file_path: str, generate_telem_tables: bool = False, work_on_copy: bool = True) ‑> None`
    :   Save this DataSet as an excel file.
        This operation modifies the dataset (changing column formats, etc), so
        it works on a copy and doesn't make any changes to the source data
        unless `work_on_copy=False`. Only do this if the dataset is
        prohibitively large to copy.
        
        If `generate_telem_tables`, this will assume it's working on a
        telemetry dataset and build numerous telemetry tables.

    `save_to_h5(self, file_path: str, key: str) ‑> None`
    :   Saves to the given key in an HDFStore in an `h5` file, including
        managing the lifecycle of the DB accessor.

    `save_to_parquet(self, file_path: str) ‑> None`
    :   Saves to the given parquet file.

    `stack(self, other: DataSet) ‑> IrisBackendv3.storage.dataset.DataSet`
    :   Stacks this `DataSet` vertically on top of an `other` `DataSet`
        (stacking, here, is in the sense of `numpy.vstack`, rows from `other`
        will be added below the rows of this DataSet. Any columns that `other`
        has but this doesn't have will be added).