# -*- coding: utf-8 -*-

"""
Defines the Structure, Content, and Serialization for all incoming and outgoing 
Messages for the Codec layer over IPC.

@author: Connor W. Colombo (CMU)
@last-updated: 05/20/2020
"""
from __future__ import annotations  # Activate postponed annotations (for using classes as return type in their own methods)

from typing import Type
import attr

from .packet import Packet
from .payload import PayloadCollection

from IrisBackendv3.ipc.wrapper import InterProcessMessage


@attr.s(frozen=True, cmp=True, slots=True, auto_attribs=True)
class PayloadsToPacketContent:
    """
    Defines the Message Content for a Request to Convert a `PayloadCollection` 
    to a `Packet` of a given type.
    """
    payloads: PayloadCollection
    packet_type: Type[Packet]


class PayloadsToPacketMessage(InterProcessMessage[PayloadsToPacketContent]):
    """
    Defines the Message Structure and Serialization Scheme for a 
    """

    def to_ipc_bytes(self) -> bytes:
        raise NotImplementedError()  # !TODO

    def from_ipc_bytes(cls, data: bytes) -> PayloadsToPacketContent:
        raise NotImplementedError()  # !TODO
