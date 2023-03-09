#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""
Standalone codec process which converts bytes->packet or payloads->packet 
(so long as any network headers have already been stripped off).

TODO: Redirect logs to an independent logging process.

@author: Connor W. Colombo (CMU)
@last-updated: 02/26/2022
"""
# Activate postponed annotations (for using classes as return type in their own methods)
from __future__ import annotations

from typing import Callable, Protocol
from enum import Enum

from IrisBackendv3.codec.packet import parse_packet
from IrisBackendv3.codec.payload_collection import EnhancedPayloadCollection

import IrisBackendv3.ipc as ipc
from IrisBackendv3.ipc import InterProcessMessage, IPMHandler, IsIPMHandler


@IsIPMHandler
def bytes_to_packet(incoming_msg_data: bytes) -> InterProcessMessage:
    raise NotImplementedError()
    # packet = parse_packet(ipc_raw.msg)
    # if packet is None:
    #     response = b''
    # else:
    #     response = packet


@IsIPMHandler
def payloads_to_packet(incoming_msg_data: bytes) -> InterProcessMessage:
    raise NotImplementedError()
    # payloads = EnhancedPayloadCollection.from_ipc_bytes(ipc_raw.msg)


class SubTopic(Enum):
    BYTES_TO_PACKET = b'\xDE\xAD', bytes_to_packet
    PAYLOADS_TO_PACKET = b'\xBE\xEF', payloads_to_packet

    # Function which handles responding to messages on the given subtopic:
    handler: IPMHandler

    def __new__(cls, val: bytes, handler: IPMHandler):
        """Constructs a new instance of the Enum."""
        obj = object.__new__(cls)
        obj._value_ = val

        obj.handler = handler

        return obj


def run() -> None:
    context = ipc.create_context()
    socket = ipc.create_socket(context, ipc.SocketType.SERVER, ipc.Port.CODEC)

    while True:
        # Grab new incoming raw data:
        ipc_raw = ipc.read_from(socket)

        # Process the incoming data based on subtopic:
        try:
            # Type ignore note: mypy doesn't recognize that Enum() doesn't call
            # __new__ but is instead an Enum indexing operation and, thus,
            # doesn't need __new__'s arguments.
            subtopic = SubTopic(ipc_raw.subtopic)  # type: ignore
        except ValueError as e:
            raise ValueError(
                f"Bad packet subtopic to `Codec` server on port `{ipc.Port.CODEC}`. "
                f"Got `{ipc_raw.subtopic!r}`. "
                f"Valid subtopics are: {[s for s in SubTopic]} ."
            )

        response: InterProcessMessage = subtopic.handler(ipc_raw.msg)
        ipc.send_to(socket, response, subtopic=ipc_raw.subtopic)

    # TODO: explicitly close socket on close signal from cli


if __name__ == '__main__':
    run()
    #! TODO: standard cli process wrapper with standard args for close, help, etc.(?)
    # ... maybe base internal-to-process threading on ARTEMIS backend threading?
