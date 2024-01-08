"""
Simple IPC App that processes all Downlinked Packets from any/all active
transceivers.
Opens all downlinked packets, extracts the payloads, and sends them to the
DL_PAYLOADS topic.

TODO: Handle MetaChannel processing here.
TODO: Handle DL packet statistics here (top table in TelemetryDisplay). Emit as a MetaChannel / MetaChannels in a Module?
        - ^ as a metachannel?

@author: Connor W. Colombo (CMU)
@last-updated: 01/06/2024
"""
from typing import cast, Final, List, Dict, Type
from datetime import datetime, timedelta

import IrisBackendv3 as IB3
import IrisBackendv3.ipc as ipc
from IrisBackendv3.ipc.messages import (
    DownlinkedPacketsMessage, DownlinkedPacketsContent,
    DownlinkedPayloadsMessage, DownlinkedPayloadsContent
)
from IrisBackendv3.codec.payload_collection import EnhancedPayloadCollection
from IrisBackendv3.codec.payload import (
    TelemetryPayload, EventPayload, FileBlockPayload, DownlinkedPayload
)

from ipc_apps.dl_processor_lib.timestamping import RoverTimeEstimator

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


def process_metachannels(payloads: EnhancedPayloadCollection) -> EnhancedPayloadCollection:
    """"""


def process_dl_payloads(
    manager: ipc.IpcAppManager,
    payloads: IB3.codec.payload_collection.EnhancedPayloadCollection
) -> IB3.codec.payload_collection.EnhancedPayloadCollection:
    """Performs post-processing on all the given payloads."""
    # Create a tool to estimate the on-rover emission datetime for
    # any payloads in this collection:
    time_est = RoverTimeEstimator(payloads)

    # Add SCET-estimate to all payloads that don't already have one:
    for i, p in enumerate(payloads[DownlinkedPayload]):
        p = cast(DownlinkedPayload, p)
        scet, delay = time_est.estimate_rover_scet(p)
        # Add a microsecond offset of the index to increase the odds that
        # timestamps are unique to minimize the odds of a collision:
        scet = scet + timedelta(microseconds=i)
        # Add to payload times:
        p.downlink_times.scet_est = scet
        p.downlink_times.scet_dl_delay_est = delay

    process_metachannels(payloads)

    return payloads


def handle_dl_packet(
    manager: ipc.IpcAppManager,
    packet: IB3.codec.packet.Packet
) -> None:
    """Handles all processing and forwarding on a downlinked packet."""
    if len(packet.payloads) == 0:
        # Packet has no payloads. Don't bother with the rest of this:
        return

    # Process all payloads in this packet:
    processed_payloads = process_dl_payloads(manager, packet.payloads)

    # Generate meta-payloads
    # TODO.

    # Forward all payloads:
    # (forward downlinked payloads and meta-payloads at the same time):
    msg = DownlinkedPayloadsMessage(DownlinkedPayloadsContent(
        payloads=processed_payloads
    ))
    manager.send_to(
        'pub', msg,
        subtopic_bytes=ipc_payload.subtopic_bytes
    )


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
            handle_dl_packet(manager, packet)
