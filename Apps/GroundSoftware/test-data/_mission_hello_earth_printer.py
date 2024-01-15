"""
Replay the first "Hello Earth" we got during Mission.

@author: Connor W. Colombo (CMU)
@last-updated: 01/13/2024
"""
import os
import os.path
from typing import cast, Final, List, Dict, Type
from datetime import datetime, timedelta

import logging

import IrisBackendv3 as IB3
import IrisBackendv3.ipc as ipc
from IrisBackendv3.ipc.messages import (
    DownlinkedPayloadsMessage, DownlinkedPayloadsContent
)
from IrisBackendv3.codec.payload_collection import EnhancedPayloadCollection
from IrisBackendv3.codec.payload import (
    TelemetryPayload, EventPayload, FileBlockPayload, DownlinkedPayload
)

from ipc_apps.dl_processor_lib.timestamping import RoverTimeEstimator

IB3.init_from_latest()

# Setup:
app = ipc.IpcAppHelper("MissionHelloPrinter")
manager = ipc.IpcAppManagerSync(socket_specs={
    'sub': ipc.SocketSpec(
        sock_type=ipc.SocketType.SUBSCRIBER,
        port=ipc.Port.TRANSCEIVER_SUB,
        topics=[ipc.Topic.DL_PAYLOADS]
    ),
})


# Run:
if __name__ == "__main__":
    app.setLogLevel('VERBOSE')

    # Change formatter to not incl. timestamps:
    for handler in app.logger.handlers:
        handler.setFormatter(logging.Formatter("%(message)s"))

    log_funcs = {
        ': VERBOSE ': app.logger.verbose,
        ': INFO ': app.logger.info,
        ': NOTICE ': app.logger.notice,
        ': SUCCESS ': app.logger.success,
        ': WARNING ': app.logger.warning,
        ': ERROR ': app.logger.error
    }

    dir = os.path.dirname(__file__)
    file = open(os.path.join(dir, '_mission_hello_earth.ib3.log'), 'r')
    lines = [line for line in file.readlines() if len(line) > 0]
    for line in lines:
        content = line
        log_func = app.logger.info
        for lvl, func in log_funcs.items():
            if lvl in line:
                log_func = func
                content = line.split(lvl)[-1]
                break

        log_func(content)
