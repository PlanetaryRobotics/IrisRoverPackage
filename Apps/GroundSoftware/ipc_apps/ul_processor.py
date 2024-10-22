"""
Simple IPC App that processes all Payloads to be Uplinked and packs them into
Payloads.

TODO: Keep a log of what's been uplinked (successfully), monitor for command responses, and
emit GDS events as commands get various stages checked off.

@author: Connor W. Colombo (CMU)
@last-updated: 06/20/2023
"""
from typing import Any, Final, Dict, Type, List, TypeVar, cast

import argparse

import IrisBackendv3 as IB3
from IrisBackendv3.codec.payload import CommandPayload
from IrisBackendv3.codec.payload_collection import EnhancedPayloadCollection
import IrisBackendv3.ipc as ipc
from IrisBackendv3.ipc.messages import (
    UplinkPayloadsRequestMessage, UplinkPayloadsRequestContent, UplinkPayloadsPacketSplit,
    UplinkPacketRequestMessage, UplinkPacketRequestContent
)

_APP_NAME: Final = "UplinkProcessor"

_PKT_T = TypeVar('_PKT_T', bound=IB3.codec.packet.Packet)


def get_cli_opts():
    parser = argparse.ArgumentParser(description=(
        f"IRIS Lunar Rover — {_APP_NAME} — CLI"
    ))
    parser.add_argument('--use-seq-num', default=False,
                        action=argparse.BooleanOptionalAction,
                        help=(
                            "Whether or not to use seq_nums in uplinked "
                            "packets (where applicable). If `False`, any "
                            "packets that require a `seq_num` will all get "
                            "`seq_num = 0`."
                        ))
    parser.add_argument('-l', '--log-level',
                        type=str, required=False, default='WARNING',
                        choices=IB3.logs.VALID_LOG_LEVELS,
                        help="Logging level.")
    return parser.parse_args()


def payloads_to_packets(
    app: ipc.IpcAppHelper,
    packet_class: Type[_PKT_T],
    payloads: List[IB3.codec.payload.Payload],
    split: UplinkPayloadsPacketSplit,
    seq_num_tracker: Dict[Type[IB3.codec.packet.Packet], int],
    use_seq_num: bool = True
) -> List[_PKT_T]:
    """Packs payloads into packet(s) of the given packet class, partitioned
    according to the given `split`.

    Increments `seq_num_tracker` entry for the given packet_class for each new
    packet generated, starting at 0.
    `seq_num_tracker` contains the **next** `seq_num` to use for each packet
    type.

    Sequence numbers will only actually be applied to the packet data if
    `use_seq_num`, otherwise a `seq_num` of 0 will be applied. Sequence numbers
    will be tracked in `seq_num_log` either way. If this tracking behavior is
    undesired, an empty dictionary can be supplied for `seq_num_tracker`.

    Returns a list of all packet(s) created.
    """
    packets: List[_PKT_T] = []
    # Track sequence numbers per packet type:
    # (start at 0 if this is the first)
    seq_num = seq_num_tracker.get(packet_class, 0)

    if split == UplinkPayloadsPacketSplit.ANY:
        # Default to individual if not specified:
        split = UplinkPayloadsPacketSplit.INDIVIDUAL

    # Partition payloads into collections that will be put into packets:
    payload_collections: List[EnhancedPayloadCollection] = []
    if split == UplinkPayloadsPacketSplit.INDIVIDUAL:
        # Put each payload into it's own packet:
        for payload in payloads:
            epc = EnhancedPayloadCollection()
            epc.append(payload)
            payload_collections.append(epc)
    elif split == UplinkPayloadsPacketSplit.TOGETHER:
        # Pack payloads together into as few packets as possible:
        # TODO: Account for max packet size. Currently it's possible for this
        # to exceed the max packet size if too many commands given. Not likely
        # with our current use case / con-ops but necessary for completeness.
        epc = EnhancedPayloadCollection()
        epc.extend(payloads)
        payload_collections.append(epc)

    # Build packets around each payload collection:
    for epc in payload_collections:
        try:
            # Only provide a `seq_num` arg if the packet class expects it:
            packet_args = dict()
            if 'seq_num' in packet_class.__init__.__annotations__:
                packet_args['seq_num'] = seq_num if use_seq_num else 0
            # Create packet and append to list:
            packets.append(
                packet_class(payloads=epc, **packet_args)  # type: ignore
            )
            seq_num += 1
        except Exception as e:
            app.logger.error(
                f"Failed to build payloads to packets. \n"
                f"Payloads: `{epc}` from larger collection: `{payloads}`.\n"
                f"...into packet class: `{packet_class}.`\n"
                f"b/c: `{e!s}`."
            )

    # Update log:
    seq_num_tracker[packet_class] = seq_num

    return packets


def main(app: ipc.IpcAppHelper, opts: Any) -> None:
    # Setup Manager:
    manager = ipc.IpcAppManagerSync(socket_specs={
        'sub': ipc.SocketSpec(
            sock_type=ipc.SocketType.SUBSCRIBER,
            port=ipc.Port.TRANSCEIVER_SUB,
            topics=[ipc.Topic.UL_PAYLOADS]
        ),
        'pub': ipc.SocketSpec(
            sock_type=ipc.SocketType.PUBLISHER,
            port=ipc.Port.TRANSCEIVER_PUB,
            topics=[ipc.Topic.UL_PACKET]
        ),
    })

    # Log of **next** `seq_num` to use for each packet class.
    # See `payloads_to_packets` for more details:
    seq_num_tracker: Dict[Type[IB3.codec.packet.Packet], int] = dict()

    while True:
        # Load a Message:
        try:
            ipc_payload = manager.read('sub')
            in_msg = ipc.guard_msg(ipc_payload.message,
                                   UplinkPayloadsRequestMessage)
            app.logger.verbose(
                f"Got Uplink Payloads Request: `{in_msg}`."
            )
        except Exception as e:
            app.logger.error(
                f"Failed to decode IPC message "
                f"of `{ipc_payload=}` b/c: `{e}`."
            )
            # Skip the rest of this loop:
            continue

        # Pick packet class to build:
        packet_class: Type[IB3.codec.packet.Packet]
        if in_msg.content.packet_class is None:
            packet_class = IB3.codec.packet.IrisCommonPacket
        else:
            packet_class = in_msg.content.packet_class

        # Build packet(s):
        packets = payloads_to_packets(
            app,
            packet_class=packet_class,
            payloads=in_msg.content.payloads,
            split=in_msg.content.split,
            seq_num_tracker=seq_num_tracker,
            use_seq_num=opts.use_seq_num
        )

        # Emit the packed packet(s) over IPC:
        for packet in packets:
            commands = packet.payloads[CommandPayload]
            for command in commands:
                command_payload = cast(CommandPayload, command)

                # Build dummy uplink packet:
                dummy_ul_packet = IB3.codec.packet.IrisCommonPacket(
                    seq_num=0,  # just using 0 for now
                    payloads=IB3.codec.payload_collection.EnhancedPayloadCollection(
                        CommandPayload=[command_payload]
                    )
                )

                # Print bytes for pasting into YAMCS:
                pkt_data_str = ''.join(
                    f'{x:02x}' for x in dummy_ul_packet.encode()
                )
                pathway_str = command.pathway.yamcs_suffix
                app.logger.success(
                    f"UL-REQ: {command_payload.command.name}_{pathway_str}:\t\t"
                    f"0x  {pkt_data_str}"
                )
        for packet in packets:
            # Forward all payloads:
            try:
                pkt_msg = UplinkPacketRequestMessage(UplinkPacketRequestContent(
                    packet=packet,
                    target_xcvr=in_msg.content.target_xcvr
                ))
                manager.send_to(
                    'pub', pkt_msg, subtopic_bytes=ipc_payload.subtopic_bytes
                )
                app.logger.notice(
                    f"Successfully sent Uplink Packet Request: `{pkt_msg}`."
                )
            except Exception as e:
                app.logger.error(
                    f"Failed to send Packet Uplink Request `{pkt_msg}` "
                    f"b/c: `{e!s}`."
                )


# Run:
if __name__ == "__main__":
    IB3.init_from_latest()
    # Setup App:
    app = ipc.IpcAppHelper(_APP_NAME)
    opts = get_cli_opts()
    app.setLogLevel(opts.log_level)
    # Run:
    main(app, opts)
