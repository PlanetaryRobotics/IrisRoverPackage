"""
Simple IPC App that processes all Downlinked Packets from any/all active
transceivers.
Opens all downlinked packets, extracts the payloads, and sends them to the
DL_PAYLOADS topic.

TODO: Handle MetaChannel processing here.
TODO: Handle DL packet statistics here (top table in TelemetryDisplay). Emit as a MetaChannel / MetaChannels in a Module?

@author: Connor W. Colombo (CMU)
@last-updated: 04/30/2023
"""
import IrisBackendv3 as IB3
import IrisBackendv3.ipc as ipc
from IrisBackendv3.ipc.messages import (
    DownlinkedPacketsMessage, DownlinkedPacketsContent,
    DownlinkedPayloadsMessage, DownlinkedPayloadsContent
)

IB3.init_from_latest()

# Setup:
app = ipc.IpcAppHelper("DownlinkProcessor")
manager = ipc.IpcAppManagerSync(socket_specs={
    'sub': ipc.SocketSpec(
        sock_type=ipc.SocketType.SUBSCRIBER,
        port=ipc.Port.TRANSCEIVER_SUB,
        topics=[ipc.Topic.DL_PACKETS]
    ),
    'pub': ipc.SocketSpec(
        sock_type=ipc.SocketType.PUBLISHER,
        port=ipc.Port.TRANSCEIVER_PUB,
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
            msg = ipc.guard_msg(ipc_payload.message, DownlinkedPacketsMessage)
            packets = msg.content.packets
        except Exception as e:
            app.logger.error(
                f"Failed to decode IPC message of `{ipc_payload=}` b/c: `{e}`."
            )
            # Skip the rest of this loop:
            continue

        # Process the Packets:
        for packet in packets:
            if len(packet.payloads) == 0:
                # Packet has no payloads. Don't bother with the rest of this:
                continue

            # Forward all payloads:
            msg = DownlinkedPayloadsMessage(DownlinkedPayloadsContent(
                payloads=packet.payloads
            ))
            manager.send_to(
                'pub', msg,
                subtopic_bytes=ipc_payload.subtopic_bytes
            )
