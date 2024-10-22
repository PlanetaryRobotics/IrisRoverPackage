"""
Defines the Transceiver class for reading telemetry from an archive file
(e.g. Mission `parquet` file or iTVAC h5 file from `_peregrine_tvac_fetcher.py`).

Includes any supporting functions necessary for processing an archive file (h5,
parquet, etc).

Formerly: `h5_transceiver.py`.

TODO: Read COMMAND columns from YAMCS archive, build command payloads, and
replay into Uplink pathway (low priority b/c that wouldn't really go anywhere
besides right back into the transceivers, which are likely just this one, and
some displays.

@author: Connor W. Colombo (CMU)
@last-updated: 10/01/2024
"""
from IrisBackendv3.storage.settings import settings as STOR_SETTINGS
import pandas as pd  # type: ignore
from yamcs.protobuf.pvalue import pvalue_pb2  # type: ignore
import yamcs.protobuf.yamcs_pb2 as yamcs_pb2  # type: ignore
from yamcs.tmtc.model import ParameterValue  # type: ignore
from typing import Any, Optional, Dict, List, cast

import attr
import asyncio
import traceback

from IrisBackendv3.transceiver.transceiver import Transceiver
from IrisBackendv3.transceiver.endec import Endec
from IrisBackendv3.transceiver.logs import logger

from IrisBackendv3.codec.metadata import DataPathway, DataSource

from IrisBackendv3.transceiver.yamcs_helper import IRIS_TELEM_PARAMS

from typing import Any, Final, Tuple, List, Dict
from datetime import datetime, timedelta, timezone

from IrisBackendv3.codec.packet_classes.packet import Packet
from IrisBackendv3.codec.metadata import DataSource, DataPathway
from IrisBackendv3.transceiver.logs import logger_setConsoleLevel as xcvrLoggerLevel
from IrisBackendv3.transceiver.exceptions import TransceiverConnectionException
from IrisBackendv3.storage.dataset import DataSet
from IrisBackendv3.storage.telemetry import generate_all_packets
from IrisBackendv3.storage.formatting import format_time

import os.path


@attr.s(auto_attribs=True)
class ArchiveParseOpts:
    # Name of dataset ({db-dir}/{name}.{ext}):
    name: str = 'fm1_mission_archive.yamcs'
    # Directory where databases are kept:
    db_dir: str = './out/databases'
    # File extension (determines archive type). Parquet is now recommended over H5:
    ext: str = 'parquet'
    # Playback speed (how many seconds of time in the archive should be played
    # for each second in the real world).
    playback_speed: float = 1.0
    # Whether or not to loop the archive at the end:
    loop: bool = True
    # Earliest time to allow from the archive (UTC) or `None` for no limits on
    # start time. Should have one since some data in the archives was
    # erroneously labelled as being from 2001:
    start_time_utc: datetime | None = datetime(2020, 1, 1,
                                               tzinfo=timezone.utc)
    # Latest time to allow from the archive (UTC) or `None` for no limits on
    # end time:
    end_time_utc: datetime | None = None
    # Whether to jump to the first instance after start-time that Iris is emits
    # telem:
    jump_to_iris: bool = True

    # Whether the reception_time (amcc_rx, pmcc_rx) should be generation_time
    # (lander_rx), if this data is being replayed, or "now" if the data came in
    # recently and we want it to sync up with the current time (rarely useful).
    # For backwards compatibility, this defaults to `False` but in most cases
    # you'll want it to be `True`.
    rx_is_generation_time: bool = True

    @staticmethod
    def field_names() -> List[str]:
        return [x.name for x in attr.fields(ArchiveParseOpts)]


def load_raw_yamcs_data(opts: ArchiveParseOpts) -> pd.DataFrame | None:
    """Loads raw YAMCS data from the Archive and returns it."""
    in_file = os.path.join(opts.db_dir, f"{opts.name}.{opts.ext}")
    logger.notice(f"Opening Archive: {in_file} . . .")
    yamcs_archive = DataSet.load_from_file(in_file, STOR_SETTINGS['YAMCS_KEY'])
    yamcs_df = yamcs_archive.data

    return yamcs_df


def records_to_packets(
    records: pd.DataFrame,
    rx_is_generation_time: bool = False
) -> List[Packet]:
    """Creates all packets that should be generated from the given records,
    sorted chronologically."""
    # Wrap records in DataSet:
    ds_records = DataSet(records)
    # Generate packets:
    times, packets = generate_all_packets(ds_records, rx_is_generation_time)
    # Sort chronologically (blends Peregrine packets into Iris packets):
    sorted_packets = [
        p for _, p in sorted(zip(times, packets), key=lambda pair: pair[0])
    ]
    return sorted_packets


def filter_archive_by_time(archive: pd.DataFrame, opts) -> pd.DataFrame:
    """Filters out all records that aren't within the time window specified by
    the given options.

    If desired, further filters out any records before Iris was first powered
    on after the start time.
    """
    if (
        opts.start_time_utc is not None
        # check for text in case someone misread the help and typed it out:
        and str(opts.start_time_utc).upper() != "NONE"
    ):
        start_bound = format_time(str(opts.start_time_utc))
        archive = archive[archive.index >= start_bound]
    if (
        opts.end_time_utc is not None
        # check for text in case someone misread the help and typed it out:
        and str(opts.end_time_utc).upper() != "NONE"
    ):
        end_bound = format_time(str(opts.end_time_utc))
        archive = archive[archive.index <= end_bound]

    # Jump to the first time iris starts emitting telem, if desired:
    if (
        opts.jump_to_iris is not None
        # check for text in case someone misread the help and typed it out:
        and str(opts.jump_to_iris).upper() != "NONE"
    ):
        # Grab all times when Iris was emitting telem (on any interface):
        telem_cols = [c for c in IRIS_TELEM_PARAMS if c in archive.columns]
        iris_telem = archive[telem_cols]
        valid = iris_telem.index == iris_telem.index  # Trues
        for c in telem_cols:
            valid = valid & ~iris_telem[c].isna()
        iris_telem = iris_telem[valid]
        iris_telem_ts = iris_telem.index.tolist()
        if len(iris_telem_ts) > 0:
            telem_start, telem_end = min(iris_telem_ts), max(iris_telem_ts)
            logger.info(
                f"\t Iris Telem span: {telem_end - telem_start} "
                f"from {telem_start} to {telem_end}."
            )
        else:
            logger.notice(
                f"\t Iris not emitting telem in this YAMCS archive in the "
                "specified time window."
            )

        # Pad cutoff times by 5 minutes (HK seems to be every ~4 sec):
        # (first telem burst precede ENABLED signal by a few seconds):
        new_start_time = telem_start - timedelta(seconds=5)
        archive = archive[archive.index >= new_start_time]

    return archive


class ArchiveTransceiver(Transceiver):
    parser_opts: ArchiveParseOpts
    _archive: pd.DataFrame | None

    _t_start_world: datetime
    _t_start_arch: datetime
    _t_end_arch: datetime
    _t_last_arch: datetime

    def _reset_timing(self) -> None:
        """Resets all the time pointers."""
        # Start time in the real world:
        self._t_start_world = datetime.now(timezone.utc)

        # Start and end of the archive:
        if self._archive is None:
            self._t_start_arch = datetime.now(timezone.utc)
            self._t_end_arch = datetime.now(timezone.utc)
        else:
            self._t_start_arch = self._archive.index[0].to_pydatetime()
            self._t_end_arch = self._archive.index[-1].to_pydatetime()

        # Archive time of last record emitted:
        self._t_last_arch = self._t_start_arch - pd.Timedelta(1, 's')

    def restart(self) -> None:
        """Resets the state of the transceiver."""
        return self._reset_timing()

    def __init__(
        self,
        parser_opts: ArchiveParseOpts,
        endecs: Optional[List[Endec]] = None,
        pathway: DataPathway = DataPathway.NONE,  # Not used here
        source: DataSource = DataSource.NONE,  # Not used here
        **kwargs
    ) -> None:
        """ Initializes an `ArchiveTransceiver`.

        NOTE: You'll need to supply the appropriate list of `endecs` which were
        used to encode the data that was put into the Archive (likely none).
        """
        full_kwargs: Dict[str, Any] = {
            'name': 'archive',  # allow this to be overridden in kwargs
            **kwargs  # fwd all other kwargs to parent
        }
        super().__init__(
            endecs=endecs,
            pathway=pathway,
            source=source,
            **full_kwargs
        )

        self.parser_opts = parser_opts

        # Initialize internal state:
        self._t_start_world = datetime.now(timezone.utc)
        self._t_start_arch = datetime.now(timezone.utc)
        self._t_end_arch = datetime.now(timezone.utc)
        self._t_last_arch = datetime.now(timezone.utc)
        self._archive = None

    def begin(self) -> None:
        """ Initialize any special registers, etc. for this transceiver.
        Can also be used to reset the state of the transceiver.
        """
        super().begin()

        # Parse the file and store the data:
        logger.notice(  # type: ignore
            f"Beginning `ArchiveTransceiver` with:\n{self.parser_opts}"
        )

        # Load the archive data:
        self._archive = load_raw_yamcs_data(self.parser_opts)
        self._archive = filter_archive_by_time(self._archive, self.parser_opts)
        logger.success("Data loaded from Archive!")

        # Only **after** the archive is loaded...
        # (Re)set the time pointers:
        self._reset_timing()

    async def _async_downlink_byte_packets(self) -> List[bytes]:
        """This transceiver does not (exclusively) transact in raw bytes,
        so byte_packets are not meaningful here."""
        raise NotImplementedError(
            "The ArchiveTransceiver does not (exclusively) transact in raw bytes,"
            "so byte_packets are not meaningful here."
        )

    async def _async_read_core(self) -> List[Packet]:
        """Asynchronously awaits the next available packet(s) on the
        transceiver input.

        We need to replace the default implementation for reading since YAMCS
        parameters provides extra metadata than the inputs to byte-stream
        transceivers, so we need to process more data than bytes and bypass
        both `_async_downlink_byte_packets` and `_read_processor`.
        """
        if self._archive is None:
            logger.warning(
                "Attempting to read from ArchiveTransceiver but no archive has "
                "been loaded. Has this transceiver been initialized with "
                "`begin()`?"
            )
            return []

        if self._t_last_arch >= self._t_end_arch:
            logger.verbose(
                f"`ArchiveTransceiver` has reached the end of the archive loaded "
                f"from `{self.parser_opts.name}`."
            )
            if self.parser_opts.loop:
                logger.verbose("Looping the archive . . .")
                self.restart()  # set time back to start
            else:
                # We're done. Just sleep a bit, return nothing (and repeat...)
                await asyncio.sleep(5)
                return []

        # Nominal flow:
        packets: List[Packet] = []
        speed = self.parser_opts.playback_speed

        # Find time of next record to play:
        next_up = self._archive[self._archive.index > self._t_last_arch]
        t_next_arch = next_up.index[0].to_pydatetime()

        # Find current time in archive time:
        t_current_world = datetime.now(timezone.utc)
        t_elapsed_world = (t_current_world - self._t_start_world).total_seconds()  # noqa
        t_elapsed_arch = timedelta(seconds=t_elapsed_world * speed)
        t_current_arch = self._t_start_arch + t_elapsed_arch

        # If needed, wait until we can emit a record:
        wait_seconds = (t_next_arch - t_current_arch).total_seconds()
        if wait_seconds > 0:
            await asyncio.sleep(wait_seconds + 1/1000)
            # Re-compute replay time now that we've waited:
            t_current_world = datetime.now(timezone.utc)
            t_elapsed_world = (t_current_world - self._t_start_world).total_seconds()  # noqa
            t_elapsed_arch = timedelta(seconds=t_elapsed_world * speed)
            t_current_arch = self._t_start_arch + t_elapsed_arch

        # Grab all records in range:
        in_range = (self._archive.index > self._t_last_arch) & \
                   (self._archive.index <= t_current_arch)

        records_in_range = self._archive[in_range]

        if records_in_range.size > 0:
            packets = records_to_packets(
                records_in_range,
                rx_is_generation_time=self.parser_opts.rx_is_generation_time
            )

        self._t_last_arch = t_current_arch

        return packets

    async def async_read(self) -> List[Packet]:
        """Asynchronously awaits the next available packet(s) on the
        transceiver input."""
        try:
            return await self._async_read_core()
        except TransceiverConnectionException as tce:
            # Log it:
            trace = '\n'.join(traceback.format_tb(tce.__traceback__))
            logger.error(
                f"While attempting to read bytes from Transceiver {self}, "
                f"a TransceiverConnectionException occurred: `{tce}`. "
                f"The trace was:\n{trace}."
            )
            # Then re-raise to get out of here:
            raise tce
        except Exception as e:
            trace = '\n'.join(traceback.format_tb(e.__traceback__))
            logger.error(
                "An otherwise unresolved error occurred during packet "
                f"streaming: {e}. "
                f"The trace was:\n{trace}."
            )
            # Nothing to process, just return empty:
            return []

    def read(self) -> List[Packet]:
        """Reads all available packets on the transceiver input.
        Synchronous wrapper for `async_read`.
        """
        # NOTE: This approach may not work if running from a Jupyter notebook
        # ... but, in that case, you can simply `await` the async version.
        return asyncio.get_event_loop().run_until_complete(
            self.async_read()
        )

    def _uplink_byte_packet(self, packet_bytes: bytes, **_) -> bool:
        """ This would transmit the given packet of bytes on this
        `Transceiver`'s uplink transmission line. **HOWEVER** this is a
        recording. You can't send commands to a recording. So this will just
        toss the bytes.

        NOTE: `**uplink_metadata` is given in the superclass and contains any
        special data needed by methods further down the uplink pipeline, but is
        unused by `ArchiveTransceiver` since it doesn't even have a meaningful
        uplink.
        """
        logger.debug(  # type: ignore
            "`ArchiveTransceiver._uplink_byte_packet` was called. "
            "This `Transceiver` replays a recording, so you can't send using "
            f"it. The data will be thrown out. Data was: `{packet_bytes!r}`."
        )

        # Just return `True`. It didn't fail and it's likely this function is
        # being called as part of a pipeline for a reason so, let's no break
        # that pipeline.
        return True
