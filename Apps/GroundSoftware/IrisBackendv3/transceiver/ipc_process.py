#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""
Standalone codec process which manages the reading of telemetry and sending of
commands using a transceiver.

TODO: Redirect logs to an independent logging process.

@author: Connor W. Colombo (CMU)
@last-updated: 02/26/2022
"""
# Activate postponed annotations (for using classes as return type in their own methods)
from __future__ import annotations

from typing import Dict

from .logging import logger

import IrisBackendv3.ipc as ipc
from IrisBackendv3.ipc.wrapper import InterProcessMessage, IPMHandler, IsIPMHandler


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


subscription_topic_handlers: Dict[ipc.Topic, IPMHandler] = {
    ipc.Topic.TX_PAYLOADS: send_payloads
}


def subscriber_update(
    socket: ipc.Socket,
    port: ipc.Port,
    topic_handlers: Dict[ipc.Topic, IPMHandler],
) -> None:
    # Grab new incoming raw data:
    ipc_payload = ipc.read_from(socket)

    # Make sure the topic is a topic that exists at all:
    try:
        # Type ignore note: mypy doesn't recognize that Enum() doesn't call
        # __new__ but is instead an Enum indexing operation and, thus,
        # doesn't need __new__'s arguments.
        topic = ipc.Topic(ipc_payload.topic_bytes)  # type: ignore
    except ValueError as e:
        logger.error(
            "ValueError: "
            f"Bad topic bytes sent to `{port.name}` server on port `{port}`. "
            f"Got `0x{ipc_payload.topic_bytes!r:04X}`. "
            f"Valid topics are: {[t for t in ipc.Topic]} ."
        )

    # Make sure a handler exists for this topic:
    if topic not in topic_handlers:
        logger.error(
            "ValueError: "
            f"Bad topic sent to `{port.name}` server on port `{port}`. "
            f"Got `{topic}` "
            f"but `{port.name}` only has handlers for: "
            f"{[n for n in topic_handlers.keys()]}"
        )

    # Now that we know this is a valid topic and a handler exists, call it:
    topic_handlers[topic](ipc_payload.msg_bytes)


def run() -> None:
    # TODO: Print start message right at beginning, incl. all relevant settings
    port = ipc.Port.TRANSCEIVER

    context = ipc.create_context()
    pub_socket = ipc.create_socket(
        context,
        socket_type=ipc.SocketType.PUBLISHER,
        ports=port
    )
    sub_socket = ipc.create_socket(
        context,
        socket_type=ipc.SocketType.SUBSCRIBER,
        ports=port
    )

    while True:
        subscriber_update(sub_socket, port, subscription_topic_handlers)

        # TODO: publisher_update

    # TODO: explicitly close socket on close signal from cli


if __name__ == '__main__':
    run()
    #! TODO: standard cli process wrapper with standard args for close, help, etc.(?)
    # ... maybe base internal-to-process threading on ARTEMIS backend threading?
