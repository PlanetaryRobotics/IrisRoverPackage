"""
Monitors the Downlinked Packets (`DL_PACKETS`) and Uplinked Packets
(`UL_PACKETS`) topics and prints the packets flowing through these topics.
A separate Transceiver App (or Apps) should be publishing to/handling packets
on these topics.

Optionally will also print messages (events, strings, etc) inside those
packets.

Also serves as a demo/test of one of the simplest possible real-world IPC Apps
that use `IpcAppManagerAsync`.

Last Updated: 03/08/2023
"""
import asyncio
import argparse
from typing import Callable, ClassVar

if __name__ == "__main__":
    # very slow (embedded) machines may take a while to import the library,
    # so print this first to let the user know things are happening:
    print("Booting . . .")

import IrisBackendv3 as IB3
import IrisBackendv3.ipc as ipc
from IrisBackendv3.ipc.messages import (
    DownlinkedPacketsMessage,
    UplinkPacketRequestMessage
)

from IrisBackendv3.utils import console_display

parser = argparse.ArgumentParser(description=(
    'IRIS Lunar Rover — IPC Packet Printer — CLI'
))


def get_opts():
    def str_to_log_level(s) -> str:
        if (level := IB3.logging.str_to_log_level(s)) is not None:
            return level
        else:
            raise argparse.ArgumentTypeError(
                f"Valid log level expected. "
                f"Log levels are: {IB3.logging.VALID_LOG_LEVELS}"
            )

    parser.add_argument('-n', '--name', type=str, default="PacketPrinter",
                        help=(
                            "Name of this App (can be configured to "
                            "accomplish different goals)."
                        ))
    parser.add_argument('--log-level', type=str_to_log_level, default='VERBOSE',
                        help=(
                            "Console logging level to be used (i.e. how "
                            "annoying the logging printouts should be). Only "
                            "logs with this level or greater are displayed in "
                            "the console. Valid logging levels are: "
                            f"{IB3.logging.VALID_LOG_LEVELS}"
                        ))
    parser.add_argument('--full-packets', default=True,
                        action=argparse.BooleanOptionalAction,
                        help="Whether to print full packet strings or just their classes.")
    parser.add_argument('--message-packets', default=False,
                        action=argparse.BooleanOptionalAction,
                        help=(
                            "Whether or not to print packets which are, "
                            "themselves, messages - e.g. command response."
                        ))
    parser.add_argument('--events', default=False,
                        action=argparse.BooleanOptionalAction,
                        help="Whether to print all events in a packet.")
    parser.add_argument('--packet-bytes', default=False,
                        action=argparse.BooleanOptionalAction,
                        help="Whether or not to print packet bytes.")
    parser.add_argument('--bind', default=False,
                        action=argparse.BooleanOptionalAction,
                        help="Whether or not to bind the port.")
    return parser.parse_args()


opts = get_opts()

# Set app name config:
app = ipc.IpcAppHelper(opts.name)
app.setLogLevel(opts.log_level)
IB3.init_from_latest()


def packet_to_log_level(packet: IB3.codec.packet.Packet) -> Callable:
    """Convenience function to pick a log-level for a packet based on what type
    it is."""
    # (remember: order matters in structural pattern checking):
    match packet:
        case IB3.codec.packet.UnsupportedPacket():
            log_func = app.logger.warn

        case IB3.codec.packet.WatchdogCommandResponsePacket() as cr if cr.custom_payload.ErrorFlagName == 'NO_ERROR':
            log_func = app.logger.success
        case IB3.codec.packet.WatchdogCommandResponsePacket():
            log_func = app.logger.error

        case IB3.codec.packet.WatchdogResetSpecificAckPacket() as ra if ra.resetResult == 'NO_ERROR':
            log_func = app.logger.success
        case IB3.codec.packet.WatchdogResetSpecificAckPacket():
            log_func = app.logger.error

        case IB3.codec.packet.WatchdogHelloPacket():
            log_func = app.logger.success
        case IB3.codec.packet.RadioHelloPacket():
            log_func = app.logger.success

        case IB3.codec.packet.WatchdogDebugImportantPacket():
            log_func = app.logger.notice

        case IB3.codec.packet.RadioGroundPacket():
            log_func = app.logger.notice
        case IB3.codec.packet.RadioDirectMessagePacket():
            log_func = app.logger.notice
        case IB3.codec.packet.RadioBgApiPacket():
            log_func = app.logger.notice

        case IB3.codec.packet.WatchdogDetailedStatusPacket():
            log_func = app.logger.info
        case IB3.codec.packet.WatchdogDebugPacket():
            log_func = app.logger.info

        case _:
            # Everything else gets printed only at verbose level:
            log_func = app.logger.verbose

    return log_func


# Setup the app:

class Sub(ipc.SocketTopicHandlerAsync['Sub']):
    """A demo Subscriber that only cares about 1 topic and ends everything
    after it gets 10 messages."""
    _raise_on_unhandled_topics: ClassVar[bool] = False
    _require_unhandled_topic_handler: ClassVar[bool] = False
    # Decorator shorthand (also plays more nicely with syntax highlighting):
    topic_handler = ipc.SocketTopicHandlerAsync['Sub'].TopicHandlerFactory()

    def __init__(self, *args, **kwargs) -> None:
        super().__init__(*args, **kwargs)

    @topic_handler
    async def downlink_handler(
        self,
        manager: ipc.IpcAppManagerAsync,
        payload: ipc.IpcPayload
    ) -> None:
        # Decode the message:
        msg = ipc.guard_msg(payload.message, DownlinkedPacketsMessage)

        # Print each packet:
        for packet in msg.content.packets:
            # Build the log string progressively:
            pathway = packet.pathway.name if packet is not None else 'NONE'
            source = packet.source.name if packet is not None else 'NONE'
            log = (
                f"[DOWNLINK - {pathway}] {source} "
                f"({payload.subtopic_bytes!r}) -> \t "
            )
            # Add the packet string:
            if opts.full_packets:
                log += f"{packet!s}"
            just_packet_classes = not opts.full_packets

            # Add any optional packet messages:
            if (
                opts.message_packets
                or opts.events
                or opts.packet_bytes
                or just_packet_classes
            ):
                messages = console_display.packet_to_messages(
                    packet,
                    echo_packet_classes=just_packet_classes,
                    echo_message_packets=opts.message_packets,
                    echo_events=opts.events,
                    echo_all_packet_bytes=opts.packet_bytes
                )
                log += "\n" + '\n'.join(messages) + "\n"

            # Pick log-level based on packet type and log:
            log_func = packet_to_log_level(packet)
            log_func(log)

    @topic_handler
    async def uplink_handler(
        self,
        manager: ipc.IpcAppManagerAsync,
        payload: ipc.IpcPayload
    ) -> None:
        # Decode the message:
        msg = ipc.guard_msg(payload.message, UplinkPacketRequestMessage)
        packet = msg.content.packet

        # Print the packet and the request metadata:
        app.logger.notice(
            f"[UPLINK - {packet.pathway.name}]\t "
            f"({payload.subtopic_bytes!r}) \t "
            f"{msg.content.packet!s} "
            f"-> \t{msg.content.target_xcvr.name}"
        )

    _topic_handlers: ClassVar[ipc.SocketTopicHandlerAsync.TopicHandlerRegistry] = {
        ipc.Topic.DL_PACKETS: downlink_handler,
        ipc.Topic.UL_PACKET: uplink_handler
    }


manager = ipc.IpcAppManagerAsync(socket_specs={
    'sub': ipc.SocketSpec(
        sock_type=ipc.SocketType.SUBSCRIBER,
        port=ipc.Port.TRANSCEIVER,
        topics=Sub.TOPICS(),
        rx_handler=Sub(),
        bind=opts.bind
    )
})

if __name__ == "__main__":
    asyncio.run(manager.run())
