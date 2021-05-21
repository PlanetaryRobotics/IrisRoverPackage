#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""
Standalone codec process which converts bytes->packet or payloads->packet 
(so long as any network headers have already been stripped off).

TODO: Redirect logs to an independent logging process.

@author: Connor W. Colombo (CMU)
@last-updated: 05/20/2020
"""
from __future__ import annotations  # Activate postponed annotations (for using classes as return type in their own methods)

from enum import Enum

from .packet import parse_packet
from .payload import PayloadCollection

import IrisBackendv3.ipc as ipc


class SubTopics(Enum):
    BYTES_TO_PACKET = b'\xDE\xAD'
    PAYLOADS_TO_PACKET = b'\xBE\xEF'


def run() -> None:
    context = ipc.create_context()
    socket = ipc.create_socket(context, ipc.SocketType.SERVER, ipc.Port.CODEC)

    while True:
        # Grab new incoming raw data:
        ipc_raw = ipc.read_from(socket)

        # Process the incoming data:
        if ipc_raw.subtopic == SubTopics.BYTES_TO_PACKET.value:
            packet = parse_packet(ipc_raw.msg)
            if packet is None:
                response = b''
            else:
                response = packet
            ipc.send_to(socket, response, subtopic=ipc_raw.subtopic)

        elif ipc_raw.subtopic == SubTopics.PAYLOADS_TO_PACKET.value:
            payloads = PayloadCollection.from_ipc_bytes(ipc_raw.msg)
            # packet_class =
            # !TODO
            # ipc.

        ipc.send_to(socket, data)


if __name__ == '__main__':
    run()
