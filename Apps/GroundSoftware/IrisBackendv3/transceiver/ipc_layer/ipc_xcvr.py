"""
Standalone codec process which manages the reading of telemetry and sending of
commands using a transceiver.

@author: Connor W. Colombo (CMU)
@last-updated: 03/06/2023
"""
# Activate postponed annotations (for using classes as return type in their own methods)
from __future__ import annotations

from typing import Any, Final, ClassVar, Awaitable, Callable, Dict, TypeAlias, Type

import asyncio
import argparse

from IrisBackendv3.transceiver.logging import logger
from IrisBackendv3.transceiver.transceiver import Transceiver
from IrisBackendv3.transceiver.prebuilts import build_xcvr_by_name

import IrisBackendv3 as IB3

import IrisBackendv3.ipc as ipc
from IrisBackendv3.ipc import (
    IpcPayload, InterProcessMessage,
    IPMHandler, IsIPMHandler
)
from IrisBackendv3.ipc.app_manager import (
    SocketSpec, SocketTopicHandler,
    PayloadHandlerResponseTypeAsync,
    SocketTopicHandlerAsync, IpcAppManagerAsync
)

# Create `Literal` names for each socket:
PUB_SOCK: Final = 'pub'
SUB_SOCK: Final = 'sub'

parser = argparse.ArgumentParser(description=(
    'IRIS Lunar Rover — Transceiver IPC App — CLI'
))


def get_opts():
    """
    Return settings wrapped in argparse.
    """
    parser.add_argument('-n', '--xcvr-name', type=str, required=True,
                        help=(
                            'Name of the prebuilt XCVR to use.'
                        ))
    # TODO: Add this --.|.
    parser.add_argument('-x', '--xcvr-args-yaml', type=str, required=False,
                        help=(
                            'Path to a YAML file to be used as optional args '
                            'for the transceiver. '
                            'This YAML file gets loaded as a dictionary and '
                            'passed into the Transceiver constructor as '
                            'keyword arguments.'
                        ))
    return parser.parse_args()


def build_app(opts) -> IpcAppManagerAsync:
    """Builds the app based on the settings in opts."""
    # Build shared resources:
    # Build the transceiver:
    xcvr = build_xcvr_by_name(opts.xcvr_name)

    # Build the downlink handler class:
    async def downlink_handler(manager: IpcAppManagerAsync):
        socket = manager.sockets[PUB_SOCK]
        # TODO: grab XCVR kwargs from YAML

        # Grab packets from the XCVR:
        packets = await xcvr.async_read()
        # Pack them up in a message and publish to the Topic:
        topic = ipc.Topic.DL_PACKETS
        message_content = topic.definition.message_def
        message = topic.definition.message_def.to_ipc_bytes(message_content)
        ipc.send_to(socket, message)

    class Sub(SocketTopicHandlerAsync['Sub']):
        _raise_on_unhandled_topics: ClassVar[bool] = False
        _require_unhandled_topic_handler: ClassVar[bool] = False
        # Decorator shorthand (also plays more nicely with syntax highlighting):
        topic_handler = SocketTopicHandlerAsync['Sub'].TopicHandlerFactory()

        @topic_handler
        async def uplink_handler(
            self,
            manager: IpcAppManagerAsync,
            payload: IpcPayload
        ) -> None:
            # Decode the message:
            message = ipc.messages.Decx
            # Pass to XCVR for uplink.
            # TODO
            # ! TODO: (WORKING-HERE) ... what about async behaviors?
            pass

        _topic_handlers: ClassVar[SocketTopicHandlerAsync.TopicHandlerRegistry] = {
            ipc.Topic.UL_PAYLOADS: uplink_handler
        }

    manager = IpcAppManagerAsync(
        socket_specs={
            PUB_SOCK: SocketSpec(
                sock_type=ipc.SocketType.PUBLISHER,
                port=ipc.Port.TRANSCEIVER
            ),
            SUB_SOCK: SocketSpec(
                sock_type=ipc.SocketType.SUBSCRIBER,
                port=ipc.Port.TRANSCEIVER,
                topics=Sub.TOPICS(),
                rx_handler=Sub()
            )
        }
    )

    manager.spawn_core_tasks()
    manager.add_coros([downlink_handler(manager)])

    return manager


async def run_main(opts) -> None:
    # TODO: Print start message right at beginning, incl. all relevant settings
    # Build the app:
    app = build_app(opts)
    logger.notice(f"Transceiver IPC process initialized with: {app=}.")

    # Run the IpcAppManager's tasks until completion:
    await manager.run(return_when=asyncio.FIRST_COMPLETED)

    # TODO: explicitly close socket on close signal from cli
    # Make sure zmq context and socket closes are same for asyncio versions


def main():
    # Start the runner:
    asyncio.run(run_main(opts))


if __name__ == '__main__':
    # Load command line options:
    opts = get_opts()
    # Initialize the Backend with the latest codec, standards, etc:
    IB3.init_from_latest()
    # Run the program:
    main(opts)
    #! TODO: standard cli process wrapper with standard args for close, help, etc.(?)
    # ... maybe base internal-to-process threading on ARTEMIS backend threading?
