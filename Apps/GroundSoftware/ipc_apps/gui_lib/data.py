"""
IPC Data Pipeline for GDS GUI.

Author: Connor W. Colombo (colombo@cmu.edu)
Last Updated: 05/30/2023
"""
from typing import Any, Final, Callable, Protocol, ClassVar, List, cast, Dict, Type, TypedDict, Tuple
from datetime import datetime, timedelta
import re

import redis
import pandas as pd

from IrisBackendv3.codec.packet_classes.packet import Packet
from IrisBackendv3.codec.payload import TelemetryPayload, EventPayload
from IrisBackendv3.codec.payload_collection import EnhancedPayloadCollection
from IrisBackendv3.utils.console_display import (
    init_telemetry_payload_log_dataframe,
    update_telemetry_payload_log_from_payloads,
    init_packet_log_dataframe,
    update_packet_log_dataframe
)

import IrisBackendv3 as IB3
import IrisBackendv3.ipc as ipc
from IrisBackendv3.ipc.messages import (
    DownlinkedPacketsMessage, DownlinkedPayloadsMessage
)

from .context import GuiContext
from .backend import DatabaseKey, db_ready, InternalDatabase


def channel_to_ts_key(module_name: str, channel_name: str) -> str:
    """Returns the standardized time series key that should be used for the
    given `TelemetryChannel` inside the given `Module`."""
    return f"ts:{module_name}_{channel_name}"


TS_DATETIME_RESOLUTION: Final[float] = 1e6  # us


def ts_datetime_to_timestamp(dt: datetime) -> int:
    """Converts the given datetime object to an integer timestamp used to index
    the TimeSeries database.
    Timestamps are microsecond epochs.
    This is acceptable since timestamps are stored as U64, so we have enough
    space for this.
    """
    return int(dt.timestamp() * TS_DATETIME_RESOLUTION)


def ts_timestamp_to_datetime(timestamp: int) -> datetime:
    """Converts the given integer timestamp used to index the TimeSeries
    database to a datetime object."""
    return datetime.fromtimestamp(timestamp/TS_DATETIME_RESOLUTION)


STREAM_DATETIME_RESOLUTION: Final[float] = 1e3  # ms


def stream_datetime_to_timestamp(dt: datetime) -> int:
    """Converts the given datetime object to an integer timestamp in
    milliseconds used as PART of the index for a Stream in a Redis database.
    Redis streams use a two-part index string of the format "{time}-{seqnum}".
    This only returns the time portion.
    """
    return int(dt.timestamp() * STREAM_DATETIME_RESOLUTION)


def stream_timestamp_to_datetime(timestamp: int) -> datetime:
    """Converts the given integer timestamp used to index the TimeSeries
    database to a datetime object."""
    return datetime.fromtimestamp(timestamp/STREAM_DATETIME_RESOLUTION)


def stream_datetime_to_id(dt: datetime, seqnum: int | None = None) -> str:
    """Converts the given datetime object to an id for a Stream in a Redis
    database.
    Redis streams use a two-part index string of the format "{time}-{seqnum}".
    The seqnum is the index of the entry among all entries with the same time
    value.
    If `seqnum is None`, a special character will be used to tell redis to
    automatically figure out the appropriate seqnum (inc. of the previous
    seqnum for the timestamp).
    """
    time: int = stream_datetime_to_timestamp(dt)
    seq_str = '*' if seqnum is None else str(int(seqnum))
    return f"{time}-{seq_str}"


def stream_parse_id(stream_id: str) -> Tuple[datetime, int]:
    """Converts the RedisStream id given datetime object to a datetime and a
    seqnum.
    Redis streams use a two-part index string of the format "{time}-{seqnum}".
    The seqnum is the index of the entry among all entries with the same time
    value.
    """
    time, seq_str = stream_id.split('-', 1)
    seq_num: int
    try:
        seq_num = int(seq_str)
    except:
        seq_num = 0  # str is wildcard like '*'
    return stream_timestamp_to_datetime(int(time)), seq_num


_ANSI_ESCAPE_PATTERN: Final = re.compile(r'(\x9B|\x1B\[)[0-?]*[ -\/]*[@-~]')


def remove_ansi_escape_codes(msg: str) -> str:
    """Helper function to remove ANSI escape strings from text that was
    designed for the command line:"""
    return _ANSI_ESCAPE_PATTERN.sub('', msg)


def event_to_html_string(event: EventPayload) -> str:
    """Creates a standard format for logging an `EventPayload` as an HTML
    string."""
    # Remove any lingering ANSI escape codes (for the command line):
    event_fstr = remove_ansi_escape_codes(event.formatted_string)

    msg = (
        f"<b>{event.event.severity_str}</b>&nbsp;&nbsp;"
        f"[{event.event.severity.value}]&nbsp;&nbsp;"
        f"<`0x{event.opcode:04X}`>"
        "<br />"
        f"`{event.event.name}`@{event.timestamp}:"
        "<br />"
        f"{event_fstr}"
    )

    # Make sure string is encoded safely for UTF-8:
    msg = msg.encode('utf-8').decode('unicode_escape')

    return msg


class EventDbPayload(TypedDict):
    """Standard format for storing an `EventPayload` as a Database Payload.
    NOTE: Events are stored in the DB as a timestamped stream, so time
    *doesn't* need to be captured in the payload.
    """
    # Pathway (how'd it get here):
    pathway_str: str
    # Severity:
    severity_lvl: int
    severity_str: str
    # Name of Module Containing this Event:
    module_name: str
    # Event Name:
    event_name: str
    # Html representing the event:
    event_html: str


def event_to_db_payload(event: EventPayload) -> EventDbPayload:
    """Creates a standardized `EventDbPayload` from the given `EventPayload`.
    """
    return EventDbPayload(
        pathway_str=event.pathway.name,
        severity_lvl=event.event.severity.value,
        severity_str=event.event.severity_str,
        module_name=event.module.name,
        event_name=event.event.name,
        event_html=event_to_html_string(event)
    )


def database_init(context: GuiContext) -> None:
    # Make sure all non-ts keys are present:
    init_keys(context)
    # Ensure all time series are present and set up correctly:
    init_all_telem_time_series(context, context.STANDARDS)
    # Ensure all streams are present and set up correctly:
    init_all_streams(context)


def init_keys(context: GuiContext) -> None:
    """Initializes all non-timeseries keys in `DatabaseKey`."""
    context.ipc_app.logger.info("Creating database keys . . .")
    if not db_ready(context.inner_db):
        raise ValueError(
            "Inner DB should have been initialized before attempting "
            "`init_keys`."
        )

    if not context.inner_db._redis.exists(DatabaseKey.TELEM_DF.value):
        context.inner_db.save(
            DatabaseKey.TELEM_DF,
            init_telemetry_payload_log_dataframe(pd.DataFrame())
        )
    if not context.inner_db._redis.exists(DatabaseKey.PACKET_DF.value):
        context.inner_db.save(
            DatabaseKey.PACKET_DF,
            init_packet_log_dataframe(pd.DataFrame())
        )


def init_all_telem_time_series(
    context: GuiContext,
    standards: IB3.data_standards.DataStandards
) -> None:
    """Creates (if necessary) time series for all telemetry channels in all
    modules in the given datastandards and applies a standard set of policies.
    """
    context.ipc_app.logger.info("Creating/altering time series policies . . .")
    if not db_ready(context.inner_db):
        raise ValueError(
            "Inner DB should have been initialized before attempting "
            "`init_all_telem_time_series`."
        )

    # Init All Telemetry Timeseries:
    telem_policies = dict(
        # let data expire after 1 month - way longer than we'll need
        # (this is just in the store used by the GUI, **NOT** the
        # archive store):
        retention_msecs=30*24*60*60*1000,
        duplicate_policy='LAST'  # for eq val & timestamps, keep latest
    )
    for module in standards.modules.vals:
        for channel in module.telemetry.vals:
            key = channel_to_ts_key(module.name, channel.name)
            ts = context.inner_db._redis.ts()
            if context.inner_db._redis.exists(key):
                context.ipc_app.logger.verbose(
                    f"Altering time series policies for {key}..."
                )
                ts.alter(key, **telem_policies)  # type: ignore
            else:
                context.ipc_app.logger.verbose(
                    f"Creating time series for {key} and adding policies..."
                )
                ts.create(key, **telem_policies)  # type: ignore


def init_all_streams(context: GuiContext) -> None:
    """Creates (if necessary) all streams and applies a standard set of
    policies. Currently this is just the events stream.
    """
    context.ipc_app.logger.info("Creating/altering stream policies . . .")
    if not db_ready(context.inner_db):
        raise ValueError(
            "Inner DB should have been initialized before attempting "
            "`init_all_streams`."
        )

    # Init Event Stream (standard policies EXCEPT collisions are not allowed):
    key = DatabaseKey.EVENT_STREAM.value
    if not context.inner_db._redis.exists(key):
        context.ipc_app.logger.verbose(
            f"Initializing stream for {key}..."
        )
        # Create dummy event and push it to the stream with the standard
        # formatting (each Redis XADD carries the formatting rules):
        now = datetime.now()
        _push_events_to_stream(
            context, context.inner_db,
            RoverTimeEstimator(EnhancedPayloadCollection(), now),
            event_db_entries=[(
                stream_datetime_to_id(now),
                EventDbPayload(
                    pathway_str='NONE',
                    severity_lvl=0,
                    severity_str='None',
                    module_name='None',
                    event_name='None',
                    event_html="<b>Redis Event Stream Initialized.</b>"
                )
            )]
        )


class RoverTimeEstimator:
    """Class that centralizes context and processing for estimating the time a
    payload was emitted in rover time based on all available timestamp data.

    Current estimation technique is a pretty shitty approximation:
    For telemetry & Event timestamps w/out time-sync, we treat that max
    rover-timestamp as equal to PMCC_RX (of its packet) and then offset all
    other timestamps with the same PMCC_RX (same packet) in reverse.
    TODO: Use better `downlink_times` once populated and time-sync is active.
    """
    now: datetime
    pmcc_rx_to_max_timestamp: Dict[datetime, int]

    def __init__(
        self,
        payloads: EnhancedPayloadCollection,
        now: datetime | None = None
    ) -> None:
        """Initializes the context for estimating rover time for any payload
        in the given collection. Performs useful pre-computations while still
        leaving the actual estimations to be computed at runtime."""
        # Grab a singular fixed value to consistently represent `now` for all
        # time operations on this collection:
        if not isinstance(now, datetime):
            self.now = datetime.now()
        else:
            self.now = now
        self._store_max_rover_timestamp_for_each_pmcc_rx(payloads)

    def _store_max_rover_timestamp_for_each_pmcc_rx(
        self,
        payloads: EnhancedPayloadCollection
    ) -> Dict[datetime, int]:
        """
        For each unique PMCC_RX time in the given payload collection (likely only
        1 or 2), find the max rover timestamp of any `EventPayload` or
        `TelemetryPayload`.
        * Overwrites existing `self.pmcc_rx_to_max_timestamp`.
        * `now` must be set to a useful value before calling.
        """
        self.pmcc_rx_to_max_timestamp: Dict[datetime, int] = dict()
        for dlp_type in [TelemetryPayload, EventPayload]:
            dlp_type = cast(Type[TelemetryPayload] |
                            Type[EventPayload], dlp_type)
            for p in payloads[dlp_type]:
                p = cast(TelemetryPayload | EventPayload, p)
                timestamp: int = p.timestamp
                pmcc_rx: datetime
                if p.downlink_times is None or p.downlink_times.pmcc_rx is None:
                    pmcc_rx = self.now  # no pmcc_rx provided, so just use `now`
                else:
                    pmcc_rx = p.downlink_times.pmcc_rx
                if pmcc_rx not in self.pmcc_rx_to_max_timestamp:
                    self.pmcc_rx_to_max_timestamp[pmcc_rx] = timestamp
                elif timestamp > self.pmcc_rx_to_max_timestamp[pmcc_rx]:
                    self.pmcc_rx_to_max_timestamp[pmcc_rx] = timestamp
        return self.pmcc_rx_to_max_timestamp

    def estimate_rover_time(
        self,
        payload: TelemetryPayload | EventPayload
    ) -> datetime:
        """Estimates the rover time when the given `payload` was generated,
        assuming this payload belongs to the collection used to initialize this
        `RoverTimeEstimator`.

        Performs the rover timestamp offset described above, given an 
        externally set datetime to use as `now` (as a fallback) and a 
        dictionary `pmcc_rx_to_max_timestamp` that maps each `pmcc_rx` 
        `datetime` to the latest *rover* timestamp that has that particular 
        `pmcc_rx` `datetime`.
        """
        pmcc_rx: datetime
        if payload.downlink_times is not None and payload.downlink_times.pmcc_rx is not None:
            pmcc_rx = payload.downlink_times.pmcc_rx
        else:
            pmcc_rx = self.now  # use 'now' if None present

        if payload.timestamp == 0:
            # don't actually have rover timestamp information, use RX:
            return pmcc_rx

        # Find the max rover timestamp associated with this PMCC_RX time
        # (assumes packet was sent the same ms this timestamp was generated and
        # that there's 0 transit time from rover to PMCC (or at least uniform
        # delay for uniform offset)):
        offset_ms: int
        if pmcc_rx in self.pmcc_rx_to_max_timestamp:
            max_timestamp = self.pmcc_rx_to_max_timestamp[pmcc_rx]
            offset_ms = max_timestamp - payload.timestamp
        else:
            offset_ms = 0
        # Offset the PMCC_RX time and return:
        return pmcc_rx - timedelta(milliseconds=offset_ms)


def _push_telemetry_time_series(
    context: GuiContext,
    inner_db: InternalDatabase,
    time_est: RoverTimeEstimator,
    payloads: EnhancedPayloadCollection
) -> None:
    """Pushes data for all telemetry in the given payload collection to the
    appropriate timeseries in the `InternalDatabase`, timestamping using the
    given `RoverTimeEstimator`."""
    # Create a pipeline where all the adds happen at once (much faster):
    pipe = inner_db._redis.pipeline()

    # Add all telemetry to its own time series:
    for i, telem in enumerate([*payloads[TelemetryPayload]]):
        telem = cast(TelemetryPayload, telem)
        time = time_est.estimate_rover_time(telem)
        # Add a microsecond offset of the index to try to ensure timestamps are
        # unique so there isn't collision:
        time = time + timedelta(microseconds=i)
        timestamp = ts_datetime_to_timestamp(time)
        key = channel_to_ts_key(telem.module.name, telem.channel.name)
        # Add the command to the pipe:
        pipe.execute_command('ts.add', key, timestamp, telem.data)

    # Run all the commands in the pipe (push all the data):
    try:
        pipe.execute()
    except Exception as e:
        context.ipc_app.logger.error(
            f"Failed to push time series for telemetry in payload collection: "
            f"`{payloads}` b/c: `{e}` (likely only a few values are at fault "
            "here)."
        )


def get_telemetry_channel_data(
    context: GuiContext,
    full_channel_names: List[str],
    from_time: datetime | int,
    to_time: datetime | int,
    span_if_blank: bool = False
) -> Dict[str, Tuple[List[datetime], List[Any]]]:
    """Extracts corresponding time and value vectors for channels with
    the given fully qualified names over the given timespan from the 
    Time Series Database.

    If there's no data to return, a tuple of empty lists will be
    returned **UNLESS** `span_if_blank`, then a 2-tuple spanning the
    start to end window will be returned.
    """
    # Make sure time endpoints are timestamps:
    if not isinstance(from_time, int):
        from_time = ts_datetime_to_timestamp(from_time)
    if not isinstance(to_time, int):
        to_time = ts_datetime_to_timestamp(to_time)

    # Craft a blank series that spans the requested time (in case we
    # can't plot, we don't want to change the view frame):
    blank_series: Tuple[List[datetime], List[Any]]
    if span_if_blank:
        blank_series = (
            [ts_timestamp_to_datetime(from_time),
             ts_timestamp_to_datetime(to_time)],
            [0, 0]
        )
    else:
        blank_series = ([], [])

    if not db_ready(context.inner_db):
        # Can't get data. Return empty data:
        return {n: blank_series for n in full_channel_names}

    # Grab all data in the time range for each channel:
    results: Dict[str, Tuple[List[datetime], List[Any]]] = dict()
    for full_ch_name in full_channel_names:
        full_ch_name = cast(str, full_ch_name)
        module_name, channel_name = full_ch_name.split('_', 1)
        key = channel_to_ts_key(module_name, channel_name)
        try:
            ts = context.inner_db._redis.ts()
            val_tuples: List[Tuple[int, Any]] = ts.range(
                key, from_time=from_time, to_time=to_time
            )
        except redis.exceptions.ResponseError as e:
            context.ipc_app.logger.error(
                f"Failed to load data for channel {full_ch_name} -> "
                f"{key=} b/c: `{e}`."
            )
            val_tuples = []
        # Unzip into a time series and a value series:
        if len(val_tuples) > 0:
            times, vs = [*zip(*val_tuples)]
            ds = [ts_timestamp_to_datetime(t) for t in times]
            results[full_ch_name] = (ds, vs)
        else:
            results[full_ch_name] = blank_series

    return results


def _push_events_to_stream(
    context: GuiContext,
    inner_db: InternalDatabase,
    time_est: RoverTimeEstimator,
    payloads: EnhancedPayloadCollection | None = None,
    event_db_entries: List[Tuple[str | datetime,
                                 EventDbPayload]] | None = None,
    expiry_time: timedelta | None = timedelta(days=31)
) -> None:
    """Pushes data for all events in the given payload collection to the 
    central events stream in the `InternalDatabase`, timestamping using
    the given `RoverTimeEstimator`, and applying consistent rules around
    limits, etc.

    Accepts an `EnhancedPayloadCollection` containing at least 0
    payloads OR a list of at least 0 pre-packaged (stream_id_string | datetime,
    `EventDbPayload`) tuples. Both may be supplied but at least 1 needs be.

    If `expiry_time is not None`, events older this timedelta will be expired
    while processing this add operation.
    """
    # Make sure we got exactly 1 specifier for payloads:
    if payloads is None and event_db_entries is None:
        raise ValueError(
            "In `_push_events_to_stream`, `payloads` OR "
            "`event_db_entries` must be given but both were `None`."
        )
    if event_db_entries is None:
        event_db_entries = []

    # Build `EventPayloads` into `EventDbPayloads`:
    if payloads is not None:
        for event in payloads[EventPayload]:
            event = cast(EventPayload, event)
            time = time_est.estimate_rover_time(event)
            # Convert `EventPayload` to a DB-compatible form:
            event_db_entries.append((time, event_to_db_payload(event)))

    # Make sure data is sorted by time:
    event_db_entries = sorted(event_db_entries, key=lambda x: x[0])

    # Expire data older than cutoff date:
    cutoff_id: str | None
    if expiry_time is None:
        cutoff_id = None
    else:
        cutoff_date = datetime.now() - expiry_time
        cutoff_id = stream_datetime_to_id(cutoff_date, seqnum=0)

    # Create a pipeline where all the adds happen at once (much faster):
    pipe = inner_db._redis.pipeline()
    for stream_id, event_db_entry in event_db_entries:
        if isinstance(stream_id, datetime):
            stream_id = stream_datetime_to_id(time)

        # Add command to the pipe:
        pipe.xadd(
            name=DatabaseKey.EVENT_STREAM.value,
            id=str(stream_id),
            fields=event_db_entry,
            maxlen=None,
            minid=cutoff_id  # prune events older than a cutoff date
        )

    # Run all the commands in the pipe (push all the data):
    try:
        pipe.execute()
    except Exception as e:
        context.ipc_app.logger.error(
            f"Failed to push to event stream for events: `{event_db_entries}` "
            f"b/c: `{e}` (likely only a few values are at fault "
            "here)."
        )


class _DbStreamRangeGetter(Protocol):
    """Structural typing protocol for expressing the type signature of ranged
    stream getters like `redis.xrange` and `redis.xrevrange`"""

    def __call__(
        self,
        name: Any,
        min: str,
        max: str,
        count: int | None = None
    ) -> Any: ...


def get_events_from_stream(
    inner_db: InternalDatabase,
    start_timestamp: int | None = None,
    end_timestamp: int | None = None,
    count: int | None = None,
    from_end: bool = True
) -> List[Tuple[str, Dict]]:
    """Fetches all events between the given start and end timestamp (timestamps
    according to `stream_datetime_to_timestamp`).

    If `start_timestamp` is None, the earliest time will be used.
    If `end_timestamp` is None, the latest time will be used.

    If `count is not None`, only up to `count` items will be fetched.
    If `from_end==True`, the fetching will start at the most recent items and
    older items may be cut if needed to stay under the count limit.
    If `from_end==False`, the fetching will start at the oldest items and newer
    items may be cut if needed to stay under the count limit.

    Returns a list of entry tuples containing the stream id (per
    `stream_parse_id`) and a dictionary containing the contents of the stream
    entry fields (*should* conform to `EventDbPayload` since that's what was
    put in but this won't be explictly checked at retrieval for runtime
    efficiency).
    """

    getter: _DbStreamRangeGetter
    if from_end:
        getter = cast(_DbStreamRangeGetter, inner_db._redis.xrevrange)
    else:
        getter = cast(_DbStreamRangeGetter, inner_db._redis.xrange)

    min_id: str
    if start_timestamp is None:
        min_id = '-'
    else:
        # start at first entry at this timestamp:
        min_id = f"{int(start_timestamp)}-0"

    max_id: str
    if end_timestamp is None:
        max_id = '+'
    else:
        # stop at first entry of next timestamp:
        max_id = f"{int(end_timestamp)+1}-0"

    event_entries_raw = cast(List[Tuple[bytes, Dict[bytes, Any]]], getter(
        name=DatabaseKey.EVENT_STREAM.value,
        min=min_id,
        max=max_id,
        count=count
    ))

    # Decode binary (only do it here and not using `decode_responses=True` for
    # the redis parser because we generally don't care about parsing bytes
    # outside of events (using `decode_responses=True` caused problems for
    # dataframes stored as JSON in Redis - e.g. the Telemetry and Packet
    # Dataframes):
    def decode(b: bytes) -> str:
        return b.decode(encoding='utf-8', errors='backslashreplace')

    event_entries: List[Tuple[str, Dict[str, Any]]] = [
        (
            decode(stream_id),
            {
                decode(k): decode(v) if isinstance(v, bytes) else v
                for k, v in entry.items()
            }
        )
        for stream_id, entry in event_entries_raw
    ]

    return event_entries


def make_ipc_manager(context: GuiContext) -> ipc.IpcAppManagerAsync:
    class Sub(ipc.SocketTopicHandlerAsync['Sub']):
        """Handles subscriptions to all topics we care about."""
        _raise_on_unhandled_topics: ClassVar[bool] = False
        _require_unhandled_topic_handler: ClassVar[bool] = False
        # Decorator shorthand (also plays more nicely with syntax highlighting):
        topic_handler = ipc.SocketTopicHandlerAsync['Sub'].TopicHandlerFactory(
        )

        # Setup internal state:
        telem_df: pd.DataFrame

        def __init__(self, *args, **kwargs) -> None:
            # Init internal copy of dataframes:
            # NOTE: We're using an internal copy of the telem_df instead of
            # loading it from db first because, if a DB push fails for some
            # reason, we don't want to lose our updates. We'll just layer on
            # another update next time and try the push again.
            self.telem_df = init_telemetry_payload_log_dataframe(
                pd.DataFrame()
            )
            # Set a specific `dtype` for each column so it's serializable
            # (can't change this inside `init_telemetry_payload_log_dataframe`
            # because it needs to provide legacy support to CLI apps).
            self.telem_df = self.telem_df.astype(dtype={
                'Module': 'string',
                'Channel': 'string',
                'nRX': 'Int64',
                'Updated': 'datetime64',
                'Current Value': 'string',
                'H+1': 'string',
                'H+2': 'string',
                'H+3': 'string'
            })

            self.packet_df = init_packet_log_dataframe(
                pd.DataFrame()
            )

            super().__init__(*args, **kwargs)

        @topic_handler
        async def dl_payload_handler(
            self,
            manager: ipc.IpcAppManagerAsync,
            ipc_payload: ipc.IpcPayload
        ) -> None:
            payloads: EnhancedPayloadCollection
            try:
                msg = ipc.guard_msg(ipc_payload.message,
                                    DownlinkedPayloadsMessage)
                payloads = msg.content.payloads
            except Exception as e:
                context.ipc_app.logger.error(
                    f"Failed to decode IPC message `{ipc_payload.msg_bytes!r}` "
                    f"of `{ipc_payload=}` b/c: `{e}`."
                )
                # Just make an empty payload collection:
                payloads = EnhancedPayloadCollection()

            # Update the payload dataframe with whatever we got (if anything):
            self.telem_df = update_telemetry_payload_log_from_payloads(
                self.telem_df, payloads
            )

            # Update the internal database with this record (to make the data
            # available to Dash):
            if db_ready(context.inner_db):
                context.inner_db.save(DatabaseKey.TELEM_DF, self.telem_df)

                # Create a tool to estimate the on-rover emission datetime for
                # any payloads in this collection:
                time_est = RoverTimeEstimator(payloads)

                # Push all data:
                _push_telemetry_time_series(
                    context, context.inner_db, time_est, payloads)
                _push_events_to_stream(
                    context, context.inner_db, time_est, payloads)

        @topic_handler
        async def dl_packet_handler(
            self,
            manager: ipc.IpcAppManagerAsync,
            ipc_payload: ipc.IpcPayload
        ) -> None:
            packets: List[Packet]
            try:
                msg = ipc.guard_msg(ipc_payload.message,
                                    DownlinkedPacketsMessage)
                packets = msg.content.packets
            except Exception as e:
                context.ipc_app.logger.error(
                    f"Failed to decode IPC message `{ipc_payload.msg_bytes!r}` "
                    f"of `{ipc_payload=}` b/c: `{e}`."
                )
                packets = []  # Just make an empty list:

            # Update the payload dataframe with whatever we got (if anything):
            for packet in packets:
                self.packet_df = update_packet_log_dataframe(
                    self.packet_df, packet
                )

            # Update the internal database with this record (to make the data
            # available to Dash):
            if db_ready(context.inner_db):
                context.inner_db.save(DatabaseKey.PACKET_DF, self.packet_df)

        _topic_handlers: ClassVar[ipc.SocketTopicHandlerAsync.TopicHandlerRegistry] = {
            ipc.Topic.DL_PAYLOADS: dl_payload_handler,
            ipc.Topic.DL_PACKETS: dl_packet_handler
        }

    return ipc.IpcAppManagerAsync(socket_specs={
        'sub': ipc.SocketSpec(
            sock_type=ipc.SocketType.SUBSCRIBER,
            port=ipc.Port.TRANSCEIVER_SUB,
            topics=Sub.TOPICS(),
            rx_handler=Sub()
        )
    })
