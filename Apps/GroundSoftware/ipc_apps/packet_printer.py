"""
Monitors the Downlinked Packets (`DL_PACKETS`) and Uplinked Packets
(`UL_PACKETS`) topics and prints the packets flowing through these topics.
A separate Transceiver App (or Apps) should be publishing to/handling packets
on these topics.

Also serves as a demo/test of one of the simplest possible real-world IPC Apps
that use `IpcAppManagerAsync`.

Last Updated: 03/08/2023
"""
import asyncio
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

IB3.init_from_latest()

# Set app name (for logs, etc):
app = ipc.IpcAppHelper("PacketPrinter")
app.setLogLevel('VERBOSE')


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
            # Pick log-level based on packet type:
            log_func = packet_to_log_level(packet)
            log_func(
                f"[DOWNLINK - {packet.pathway}] {packet.source} -> \t "
                f"({payload.subtopic_bytes!r})\t "
                f"{packet!s}"
            )

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
            f"[UPLINK - {packet.pathway}]\t "
            f"({payload.subtopic_bytes!r}) \t "
            f"{msg.content.packet!s} "
            f"-> \t{msg.content.target_xcvr.name}"
        )

    _topic_handlers: ClassVar[ipc.SocketTopicHandlerAsync.TopicHandlerRegistry] = {
        ipc.Topic.DL_PACKETS: downlink_handler,
        ipc.Topic.UL_PACKET: uplink_handler
    }


manager = IB3.ipc.IpcAppManagerAsync(socket_specs={
    'sub': IB3.ipc.SocketSpec(
        sock_type=IB3.ipc.SocketType.SUBSCRIBER,
        port=IB3.ipc.Port.TRANSCEIVER,
        topics=Sub.TOPICS(),
        rx_handler=Sub(),
        bind=False
    )
})

if __name__ == "__main__":
    asyncio.run(manager.run())
