"""
Simple IPC App to archive traffic in all IPC `Topic`s into an AOF DB.

TODO: Add replay functionality.

@author: Connor W. Colombo (CMU)
@last-updated: 01/11/2024
"""
from typing import cast, Final, List, Dict, Type, Set
from datetime import datetime, timedelta, timezone
from dataclasses import dataclass

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

_UPDATE_STR_INTERVAL: Final = timedelta(seconds=5)

# Setup:
app = ipc.IpcAppHelper("Archiver")
manager = ipc.IpcAppManagerSync(socket_specs={
    'sub': ipc.SocketSpec(
        sock_type=ipc.SocketType.SUBSCRIBER,
        port=ipc.Port.TRANSCEIVER_SUB,
        topics=[t for t in ipc.Topic]
    ),
})


@dataclass
class IpcArchiveEntry:
    # `IpcPayload` received:
    ipc_payload: ipc.IpcPayload
    # When this `IpcPayload` was received by the archiver
    time_arrived_from_ipc: datetime


def archive(entry: IpcArchiveEntry) -> None:
    # Create binary:
    restricted_pickler.dumps()

    raise NotImplementedError()


# Run:
if __name__ == "__main__":
    app.setLogLevel('INFO')

    time_of_last_update: datetime = datetime.now(timezone.utc)
    n_archived_since_update: int = 0
    topics_archived_since_update: Set[ipc.Topic] = set()

    while True:
        # Load a Message:
        try:
            ipc_payload = manager.read('sub')
        except Exception as e:
            app.logger.error(
                f"Failed to decode read IPC message because of: `{e}`."
            )
            # Skip the rest of this loop:
            continue

        now = datetime.now(timezone.utc)
        # Create the archive entry:
        entry = IpcArchiveEntry(
            ipc_payload=ipc_payload,
            time_arrived_from_ipc=now
        )

        # Archive it:
        archive(entry)
        n_archived_since_update += 1
        try:
            topics_archived_since_update.add(ipc_payload.topic)
        except Exception as e:
            app.logger.error(
                f"[{now}] Failed to decode IPC Topic of `{ipc_payload=}` "
                f"b/c: `{e}`. Still archived it."
            )

        # See if we should update the console:
        if now > (time_of_last_update + _UPDATE_STR_INTERVAL):
            app.logger.info(
                f"[{now}] Archived {n_archived_since_update} "
                f"across {len(topics_archived_since_update) } Topics "
                f"({[*topics_archived_since_update]})"
            )
            time_of_last_update = now
            n_archived_since_update = 0
            topics_archived_since_update = set()
