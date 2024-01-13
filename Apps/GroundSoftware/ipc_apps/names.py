"""
Simple IPC App to print the names and messages from donors.

@author: Connor W. Colombo (CMU)
@last-updated: 01/09/2024
"""
from typing import cast, Final, List, Dict, Type
from datetime import datetime, timedelta

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
app = ipc.IpcAppHelper("RollCredits")
manager = ipc.IpcAppManagerSync(socket_specs={
    'sub': ipc.SocketSpec(
        sock_type=ipc.SocketType.SUBSCRIBER,
        port=ipc.Port.TRANSCEIVER_SUB,
        topics=[ipc.Topic.DL_PAYLOADS]
    ),
})


# Run:
if __name__ == "__main__":
    app.setLogLevel('INFO')

    while True:
        # Load a Message:
        try:
            ipc_payload = manager.read('sub')
            msg = ipc.guard_msg(ipc_payload.message, DownlinkedPayloadsMessage)
            payloads = msg.content.payloads
        except Exception as e:
            app.logger.error(
                f"Failed to decode IPC message of `{ipc_payload=}` b/c: `{e}`."
            )
            # Skip the rest of this loop:
            continue

        # Process the Packets:
        for payload in payloads[EventPayload]:
            payload = cast(EventPayload, payload)
            if payload.event.name == "BroughtToYouBy":
                name = payload.args['name']
                app.logger.success(f"Brought to you by: {name}.")
            if payload.event.name == "SpecialMessage":
                name = payload.args['name']
                message = payload.args['message']
                app.logger.info(f"{name} says: '{message}'")
            if payload.event.name == "NoOpStringReceived":
                message = payload.args['message']
                app.logger.notice(" >" + message)
