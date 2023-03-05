"""
Monitors the Downlinked Packets (`DL_PACKETS`) and Uplinked Packets
(`UL_PACKETS`) topics and prints the packets flowing through these topics.
A separate Transceiver App (or Apps) should be publishing to/handling packets
on these topics.

Also serves as a demo/test of one of the simplest possible IPC App.

Last Updated: 03/03/2023
"""
# Activate postponed annotations (for using classes as return type in their own methods)
from __future__ import annotations
if __name__ == "__main__":
    # very slow (embedded) machines may take a while to import the library,
    # so print this first to let the user know things are happening:
    print("Booting . . .")

import IrisBackendv3 as IB3
from abc import ABC, abstractmethod
from typing import Any, Awaitable, Callable, Generic, Optional, ClassVar, Protocol, Dict, List, cast, TypeVar, Tuple
from typing_extensions import TypeAlias
from types import NoneType
import dataclasses

import IrisBackendv3.ipc as ipc
from IrisBackendv3.ipc.logging import logger
from IrisBackendv3.ipc import (
    IpcPayload, InterProcessMessage,
    IPMHandler, IsIPMHandler
)
from IrisBackendv3.ipc.app_manager import (
    SocketSpec, SocketTopicHandler,
    PayloadHandlerResponseTypeAsync,
    SocketHandlerAsync, SocketTopicHandlerAsync,
    IpcAppManager, IpcAppManagerAsync
)


def packet_to_log_level(packet: IB3.codec.packet.Packet) -> Callable:
    """Convenience function to pick a log-level for a packet based on what type
    it is."""
    # (remember: order matters in structural pattern checking):
    match packet:
        case IB3.codec.packet.UnsupportedPacket():
            log_func = logger.warn

        case IB3.codec.packet.WatchdogCommandResponsePacket() as cr if cr.custom_payload.ErrorFlagName == 'NO_ERROR':
            log_func = logger.success
        case IB3.codec.packet.WatchdogCommandResponsePacket():
            log_func = logger.error

        case IB3.codec.packet.WatchdogResetSpecificAckPacket() as ra if ra.resetResult == 'NO_ERROR':
            log_func = logger.success
        case IB3.codec.packet.WatchdogResetSpecificAckPacket():
            log_func = logger.error

        case IB3.codec.packet.WatchdogHelloPacket():
            log_func = logger.success
        case IB3.codec.packet.RadioHelloPacket():
            log_func = logger.success

        case IB3.codec.packet.WatchdogDebugImportantPacket():
            log_func = logger.notice

        case IB3.codec.packet.RadioGroundPacket():
            log_func = logger.notice
        case IB3.codec.packet.RadioDirectMessagePacket():
            log_func = logger.notice
        case IB3.codec.packet.RadioBgApiPacket():
            log_func = logger.notice

        case IB3.codec.packet.WatchdogDetailedStatusPacket():
            log_func = logger.info
        case IB3.codec.packet.WatchdogDebugPacket():
            log_func = logger.info

        case _:
            # Everything else gets printed only at verbose level:
            log_func = logger.verbose

    return log_func


# Setup the app:
class SubHandler(SocketHandlerAsync):
    _raise_on_unhandled_topics: ClassVar[bool] = False
    # Decorator shorthand (also plays more nicely with syntax highlighting):
    topic_handler = SocketTopicHandlerAsync['SubHandler'].TopicHandlerFactory()

    def handle_unhandled_topic(
        self,
        manager: IpcAppManagerAsync,
        payload: IpcPayload
    ) -> Awaitable[None]:
        """ Handler called for a payload whose topic has no registered handler
        """
        async def do_nothing(self, *_) -> None:
            # Do nothing. Just toss this data out.
            return None
        return do_nothing(*[])

    @topic_handler
    async def downlink_handler(
        self,
        manager: IpcAppManagerAsync,
        payload: IpcPayload
    ) -> None:
        # TODO: Add this bit to IpcPayload: (would new TypeGuard narrowing be useful here?)
        topic = payload.topic
        subtopic = payload.subtopic_bytes
        packets = topic.definition.message_def.from_ipc_bytes(
            payload.msg_bytes)

        # Print each packet:
        for packet in packets:
            # Pick log-level based on packet type:
            log_func = packet_to_log_level(packet)
            log_func(
                f"[DOWNLINK]\t "
                f"({topic.name}, {subtopic!r})\t "
                f"{packet!s}"
            )

    @topic_handler
    async def uplink_handler(
        self,
        manager: IpcAppManagerAsync,
        payload: IpcPayload
    ) -> None:
        # TODO: Add this bit to IpcPayload: (would new TypeGuard narrowing be useful here?)
        topic = payload.topic
        subtopic = payload.subtopic_bytes
        packets = topic.definition.message_def.from_ipc_bytes(
            payload.msg_bytes)

        # Print each packet:
        for packet in packets:
            # Pick log-level based on packet type:
            logger.notice(
                f"[UPLINK]\t "
                f"({topic.name}, {subtopic!r}) >\t "
                f"{packet!s}"
            )
    _topic_handlers: ClassVar[SocketTopicHandlerAsync.TopicHandlerRegistry] = {
        ipc.Topic.DL_PACKETS: downlink_handler,
        ipc.Topic.UL_PACKETS: uplink_handler
    }


async def run() -> None:
    logger.notice("IRIS Lunar Rover - PACKET_PRINTER - IPC CLI App")

    # Create `Literal` names for each socket:
    manager = IpcAppManagerAsync(
        socket_specs={
            'sub': SocketSpec(
                sock_type=ipc.SocketType.SUBSCRIBER,
                port=ipc.Port.TRANSCEIVER,
                topics=SubHandler.TOPICS(),
                rx_handler=SubHandler()
            )
        }
    )

    logger.notice(f"Transceiver IPC process started with: {manager!s}.")

    manager.spawn_tasks()
    manager.run()

    tasks = [
        asyncio.create_task(manager.sub_socket_task(), name=f"Task"),
    ]


def main():
    # Start the runner:
    asyncio.run(run())


if __name__ == "__main__":
    # Load the latest datastandards:
    IB3.init_from_latest()
    # Run:
    main()
