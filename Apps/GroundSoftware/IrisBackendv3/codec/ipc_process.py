#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""
Standalone codec process which converts bytes->packet or payloads->packet 
(so long as any network headers have already been stripped off).

TODO: Redirect logs to an independent logging process.

@author: Connor W. Colombo (CMU)
@last-updated: 07/15/2021
"""
# Activate postponed annotations (for using classes as return type in their own methods)
from __future__ import annotations

from typing import Callable, coerce
from enum import Enum

from .packet import parse_packet
from .payload import PayloadCollection

import IrisBackendv3.ipc as ipc
from IrisBackendv3.ipc.wrapper import InterProcessMessage

IpcMessageHandler = Callable[[bytes], InterProcessMessage]


@coerce(IpcMessageHandler)
def bytes_to_packet(incoming_msg_data: bytes) -> InterProcessMessage:
    raise NotImplementedError()
    # packet = parse_packet(ipc_raw.msg)
    # if packet is None:
    #     response = b''
    # else:
    #     response = packet


@coerce(IpcMessageHandler)
def payloads_to_packet(incoming_msg_data: bytes) -> InterProcessMessage:
    raise NotImplementedError()
    # payloads = PayloadCollection.from_ipc_bytes(ipc_raw.msg)


class SubTopic(Enum):
    BYTES_TO_PACKET = b'\xDE\xAD', bytes_to_packet
    PAYLOADS_TO_PACKET = b'\xBE\xEF', payloads_to_packet

    # Function which handles responding to messages on the given subtopic:
    handler: Callable[[bytes], InterProcessMessage]

    def __new__(cls, val: bytes, handler: Callable[[bytes], InterProcessMessage]):
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

        # ?! TODO: WTF? Why not callable? Seems like old mypy bug: https://github.com/python/mypy/issues/5485
        response: InterProcessMessage = subtopic.handler.__call__(ipc_raw.msg)
        response: InterProcessMessage = subtopic.handler(ipc_raw.msg)
        ipc.send_to(socket, response, subtopic=ipc_raw.subtopic)

    # TODO: explicitly close socket on close signal from cli


if __name__ == '__main__':
    run()
    #! TODO: standard cli process wrapper with standard args for close, help, etc.(?)
    # ... maybe base internal-to-process threading on ARTEMIS backend threading?
