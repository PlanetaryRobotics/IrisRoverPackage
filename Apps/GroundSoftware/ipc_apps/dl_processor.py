"""
Simple IPC App that processes all Downlinked Packets from any/all active
transceivers.
Opens all downlinked packets, extracts the payloads, and sends them to the
DL_PAYLOADS topic.

TODO: Handle DL packet statistics here (top table in TelemetryDisplay). Emit as a MetaChannel / MetaChannels in a Module?
        - ^ as a metachannel?

@author: Connor W. Colombo (CMU)
@last-updated: 01/10/2024
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
from IrisBackendv3.codec.metadata import DownlinkTimes
from IrisBackendv3.meta.metafield import process_payloads_for_meta_modules

from ipc_apps.dl_processor_lib.timestamping import RoverTimeEstimator


from config.metafields import ALL_META_MODULES

IB3.init_from_latest()


def generate_metafields(payloads: EnhancedPayloadCollection) -> EnhancedPayloadCollection:
    """Generates all metafields that can be generated, adds them to the given
    `payloads` collection, and returns it (for chaining)."""
    meta_payloads = process_payloads_for_meta_modules(
        modules=ALL_META_MODULES,
        payloads=[
            cast(DownlinkedPayload, p) for p in payloads[DownlinkedPayload]
        ]
    )
    payloads.extend(meta_payloads)
    return payloads


def process_dl_payloads(
    payloads: IB3.codec.payload_collection.EnhancedPayloadCollection
) -> IB3.codec.payload_collection.EnhancedPayloadCollection:
    """Performs post-processing on all the given payloads.
    - Calculates and applies the appropriate SCET to each payload.
    - Generates metafields.
    """
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
        if p.downlink_times is None:
            p.downlink_times = DownlinkTimes()
        p.downlink_times.scet_est = scet
        p.downlink_times.scet_dl_delay_est = delay

    payloads = generate_metafields(payloads)

    return payloads


def handle_dl_packet(
    app: ipc.IpcAppHelper,
    manager: ipc.IpcAppManager,
    packet: IB3.codec.packet.Packet
) -> None:
    """Handles all processing and forwarding on a downlinked packet."""
    if len(packet.payloads) == 0:
        # Packet has no payloads. Don't bother with the rest of this:
        return

    # Process all payloads in this packet:
    processed_payloads = process_dl_payloads(packet.payloads)

    # Report what we got (for addl. archiving):
    data_str = ""
    if isinstance(packet._raw, bytes):
        data_str = f"0x{':'.join(f'{x:02X}' for x in packet._raw)}"
    payloads: List[DownlinkedPayload] = [
        cast(DownlinkedPayload, p) for p in packet.payloads[DownlinkedPayload]
    ]
    if len(payloads) > 0:
        app.logger.debug(
            f"Got: {data_str} at t0={payloads[0].downlink_times} with: \n\t"
            + '\n\t'.join(p.__str__() for p in payloads)
        )

    # Forward all payloads:
    # (forward downlinked payloads and meta-payloads at the same time):
    msg = DownlinkedPayloadsMessage(DownlinkedPayloadsContent(
        payloads=processed_payloads
    ))
    if hasattr(manager, 'send_to'):
        # Little hacky b/c Sync and Async Manager's handle this differently so
        # they don't inherit from same base method but both have same signature
        # so this works fine
        manager.send_to(
            'pub', msg,
            subtopic_bytes=ipc_payload.subtopic_bytes
        )
    else:
        fatal_msg = (
            "Incompatible `IpcAppManager` used for DownlinkProcessor. "
            "Somehow it doesn't have a `send_to` method. "
            "Likely a new manager type was added that's incomplete. "
            "See `IpcAppManagerSync` and `IpcAppManagerAsync` for examples of "
            "correct implementations."
        )
        app.logger.critical(fatal_msg)
        raise ValueError(fatal_msg)


# Run:
if __name__ == "__main__":
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
            handle_dl_packet(app, manager, packet)
