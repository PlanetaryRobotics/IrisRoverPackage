"""
Parses and replays data from an archive file (archived from iTVAC using
`_peregrine_tvac_fetcher.py`), using the ArchiveTransceiver.

Formerly `h5_xcvr.py`.

NOTE: If this gives protobuf warnings, either resolve them or run:
- `PROTOCOL_BUFFERS_PYTHON_IMPLEMENTATION=python ./run-script.sh ...`
- instead of: `./run-script.sh ...`

@author: Connor W. Colombo (CMU)
@last-updated: 09/29/2024
"""

# Activate postponed annotations:
from __future__ import annotations

if __name__ == "__main__":
    # Loading dependencies can take a bit (builds DataStandards as part of
    # this). Acknowledge that it's doing something if running as main:
    print('Booting . . .')

import traceback
from datetime import datetime, timezone

import IrisBackendv3 as IB3
import IrisBackendv3.ipc as ipc
from IrisBackendv3.logs import VALID_LOG_LEVELS
from IrisBackendv3.transceiver.archive_transceiver import ArchiveTransceiver, ArchiveParseOpts
from IrisBackendv3.transceiver.logs import logger_setConsoleLevel as xcvrLoggerLevel

from IrisBackendv3.ipc.messages import (
    DownlinkedPacketsMessage, DownlinkedPacketsContent
)

import argparse

import pandas as pd  # type: ignore

IB3.init_from_latest()

app = ipc.IpcAppHelper("ArchiveTransceiverDemo")
app.setLogLevel('VERBOSE')


def get_opts(
    description: str = 'IRIS Lunar Rover — GSW Backend — Archive Transceiver Demo',
    default_log_level: str = 'INFO'
):
    """
    Return CLI options wrapped in argparse.

    Primarily used to change settings in 
    `transceiver.archive_transceiver.ArchiveParseOpts`.
    """
    def str_to_log_level(s):
        if isinstance(s, str) and s.upper() in VALID_LOG_LEVELS:
            return s.upper()
        else:
            raise argparse.ArgumentTypeError(
                f'Valid log level expected. Log levels are: {VALID_LOG_LEVELS}'
            )

    def str_to_optional_datetime(dts: str) -> datetime | None:
        dts = str(dts)
        # Check for timezone, add UTC if not:
        if dts.upper() == "NONE":
            return None
        else:
            dt = datetime.fromisoformat(dts)
            # If no timezone info was found, set it to UTC:
            if dt.tzinfo is None:
                dt = dt.replace(tzinfo=timezone.utc)
            return dt

    parser = argparse.ArgumentParser(
        description=description
    )

    parser.add_argument('-n', '--name', default='fm1_mission_archive.yamcs',  # 'int-tvac-data2',
                        help=r'Name of dataset ({db-dir}/{name}.{ext}).')
    parser.add_argument('-d', '--db-dir', default='./out/databases',
                        help='Directory where databases are kept.')
    parser.add_argument('-e', '--ext', default='parquet',
                        help='Archive file extension.')

    parser.add_argument('-s', '--playback-speed', type=float, default=100.0,
                        help="Playback speed. How many seconds of time in the "
                             "archive should be played for each second in the "
                             "real world.")

    parser.add_argument('--loop', default=True,
                        action=argparse.BooleanOptionalAction,
                        help="Whether or not to loop the archive.")

    parser.add_argument('--start-time-utc', default=datetime(2020, 1, 1, tzinfo=timezone.utc),
                        type=str_to_optional_datetime,
                        help="Earliest time to allow from the archive (UTC) "
                             "or `None` for no limits on start time. "
                             "Default is Jan. 1, 2020 (before start of any "
                             "archive) because some data in some archives was "
                             "erroneously labelled as being from 2001, so we "
                             "need to not include that.")
    parser.add_argument('--end-time-utc', default=None,
                        type=str_to_optional_datetime,
                        help="Latest time to allow from the archive (UTC) "
                             "or `None` for no limits on end time.")
    parser.add_argument('--jump-to-iris', default=True,
                        action=argparse.BooleanOptionalAction,
                        help="Jumps to the first instance after start-time "
                        "that Iris is emits telem.")

    parser.add_argument('--rx-is-generation-time', default=True,
                        action=argparse.BooleanOptionalAction,
                        help="Whether the reception_time (amcc_rx, pmcc_rx) should "
                        "be generation_time  (lander_rx), if this data is "
                        "being replayed, or 'now' if the data came in recently "
                        "and we want it to sync up with the current time "
                        "(rarely useful). In most cases you'll want this to be "
                        "`True`."
                        )

    parser.add_argument('--log-level', type=str_to_log_level, default=default_log_level,
                        help=('Logging level to be used (i.e. how annoying the logging '
                              'printouts should be). Only logs with importance greater '
                              'than or equal to the specified logging level are '
                              f'displayed. Valid logging levels are: {VALID_LOG_LEVELS}'))

    opts = parser.parse_args()
    return opts


if __name__ == '__main__':
    opts = get_opts()

    xcvrLoggerLevel(opts.log_level)
    app.setLogLevel(opts.log_level)

    xcvr = ArchiveTransceiver(ArchiveParseOpts(**{
        k: v
        for k, v in vars(opts).items()
        if k in ArchiveParseOpts.field_names()
    }))
    xcvr.begin()

    # Setup IPC:
    manager = ipc.IpcAppManagerSync(socket_specs={
        'pub': ipc.SocketSpec(
            sock_type=ipc.SocketType.PUBLISHER,
            port=ipc.Port.TRANSCEIVER_PUB,
            topics=[ipc.Topic.DL_PACKETS],
            publish_only=True
        )
    })

    n_packets_sent: int = 0  # count total packets sent for diagnostics
    while len(packets := xcvr.read()) != 0 or opts.loop:
        try:
            msg = DownlinkedPacketsMessage(DownlinkedPacketsContent(
                packets=packets
            ))
            manager.send_to('pub', msg, subtopic_bytes=b'archive')
            n_packets_sent += len(packets)
            app.logger.notice(
                f"[{n_packets_sent:5d}] "
                f"Sent {msg.content.simple_str()} -> {ipc.Topic.DL_PACKETS}"
            )
        except Exception as e:
            trace = '\n'.join(traceback.format_tb(e.__traceback__))
            app.logger.error(
                f"Uncaught exception in encoding packet(s): `{packets}`. \n"
                f"Exception: `{e}`."
                f"The trace was:\n{trace}."
            )
