"""
Standalone codec process which manages the reading of telemetry and sending of
commands using a transceiver.

TODO: Redirect logs to an independent logging process.

@author: Connor W. Colombo (CMU)
@last-updated: 03/04/2023
"""
# Activate postponed annotations (for using classes as return type in their own methods)
from __future__ import annotations

from typing import Any, Final, ClassVar, Awaitable, Callable, Dict, TypeAlias, Type

import asyncio
import argparse

from .logging import logger
from .transceiver import Transceiver
from .prebuilts import build_xcvr_by_name

import IrisBackendv3 as IB3

import IrisBackendv3.ipc as ipc
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


@IsIPMHandler
def bytes_to_packet(incoming_msg_data: bytes) -> InterProcessMessage:
    raise NotImplementedError()
    # packet = parse_packet(ipc_raw.msg)
    # if packet is None:
    #     response = b''
    # else:
    #     response = packet


@IsIPMHandler
def send_packet(incoming_msg_data: bytes) -> InterProcessMessage:

    raise NotImplementedError()
    # payloads = EnhancedPayloadCollection.from_ipc_bytes(ipc_raw.msg)


class PubHandler(SocketHandlerAsync):
    async def async_call(
        self,
        manager: IpcAppManagerAsync,
        payload: IpcPayload
    ) -> None:
        # Nothing to do here. We only pub.
        pass


def build_dl_process(opts):
    xcvr = build_xcvr_by_name(opts.xcvr_name)
    # TODO: grab XCVR kwargs from YAML

    async def task_core():
        # Grab packets from the XCVR:
        packets = await xcvr.async_read()
        # Pack them up in a message and publish to the Topic:
        topic = ipc.Topic.DL_PACKETS
        message_content = topic.definition.message_def
        message = topic.definition.message.to_ipc_bytes(message_content)
        ipc.send_to(socket, message)

    return task_core


class SubHandler(SocketTopicHandlerAsync['SubHandler']):
    _raise_on_unhandled_topics: ClassVar[bool] = False
    _require_unhandled_topic_handler: ClassVar[bool] = False
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
    async def uplink_handler(
        self,
        manager: IpcAppManagerAsync,
        payload: IpcPayload
    ) -> None:
        # Pass to XCVR for uplink.
        # TODO
        # ! TODO: (WORKING-HERE) ... what about async behaviors?
        pass

    _topic_handlers: ClassVar[SocketTopicHandlerAsync.TopicHandlerRegistry] = {
        ipc.Topic.UL_PAYLOADS: uplink_handler
    }


async def run() -> None:
    # TODO: Print start message right at beginning, incl. all relevant settings
    port = ipc.Port.TRANSCEIVER

    # Create `Literal` names for each socket:
    PUB_SOCK: Final = 'pub'
    SUB_SOCK: Final = 'sub'

    manager = IpcAppManagerAsync(
        socket_specs={
            PUB_SOCK: SocketSpec(
                sock_type=ipc.SocketType.PUBLISHER,
                port=port,
                rx_handler=PubHandler()
            ),
            SUB_SOCK: SocketSpec(
                sock_type=ipc.SocketType.SUBSCRIBER,
                port=port,
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
        # this controls the base draw rate of the screen (i.e. how frequently we update if no packets or key presses are received)
        asyncio.create_task(ticker(1, tick_queue))
    ]

    # Clean up if the above closes for some reason:
    try:
        done, pending = await asyncio.wait(
            tasks,
            loop=None,
            timeout=None,
            return_when=asyncio.FIRST_EXCEPTION
        )

        # We're done. Log results:
        for task in done:
            result, exception = [None]*2
            try:
                result = task.result()
            except Exception as e:
                exception = e
            logger.notice(
                f"Task {task.get_name()} ended in {manager!s} "
                f"with `{result=}`, `{exception=}`."
            )
        for task in pending:
            task.cancel()
            logger.notice(
                f"Task {task.get_name()} didn't end in {manager!s}. Now cancelled."
            )
    except Exception as e:
        logger.error(f"Exception occurred while handling {manager!s}:\n\t{e}.")

    # Spin up thread for receiver and transmitter

    while True:
        subscriber_update(sub_socket, port, subscription_topic_handlers)

        # TODO: publisher_update

    # TODO: explicitly close socket on close signal from cli
    # Make sure zmq context and socket closes are same for asyncio versions


async def handle_server_socket(Mgr, Msg) -> None:
    if Msg.wants_something:
        await Mgr.send(CLISOCK, something)
    elif Msg.wants_something_else:
        await Mgr.send(CLISOCK, the_other_thing)


# if sub, socket_handler (dispatch)
async def handle_sub_socket(Mgr, Msg) -> None:
    if Msg.payload.name == special_tlm_chan:
        meta_channel_data = somefunc(Msg.payload)


def main():
    # Start the runner:
    asyncio.run(run())


if __name__ == '__main__':
    # Initialize the Backend with the latest codec, standards, etc:
    IB3.init_from_latest()
    # Run the program:
    main()
    #! TODO: standard cli process wrapper with standard args for close, help, etc.(?)
    # ... maybe base internal-to-process threading on ARTEMIS backend threading?
