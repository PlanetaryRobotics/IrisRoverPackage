# -*- coding: utf-8 -*-

"""
Defines the Structure, Content, and Serialization for all incoming and outgoing 
Messages for the Codec layer over IPC.

@author: Connor W. Colombo (CMU)
@last-updated: 05/20/2021
"""
from __future__ import annotations  # Activate postponed annotations (for using classes as return type in their own methods)

from typing import Type, List
import attr

from IrisBackendv3.codec.packet_classes.packet import Packet
from IrisBackendv3.codec.payload_collection import EnhancedPayloadCollection

from IrisBackendv3.ipc import InterProcessMessage


@attr.s(frozen=True, cmp=True, slots=True, auto_attribs=True)
class PayloadsToPacketRequestContent:
    """
    Defines the Message Content for a Request to pack an
    `EnhancedPayloadCollection` to into a `Packet` of a given type.
    """
    payloads: EnhancedPayloadCollection
    packet_type: Type[Packet]


class PayloadsToPacketRequestMessage(InterProcessMessage[PayloadsToPacketRequestContent]):
    """
    Defines the Message Structure and Serialization Scheme for a Request to 
    pack an `EnhancedPayloadCollection` to into a `Packet` of a given type.
    """

    def to_ipc_bytes(self) -> bytes:
        raise NotImplementedError()  # !TODO

    def from_ipc_bytes(cls, data: bytes) -> PayloadsToPacketRequestContent:
        raise NotImplementedError()  # !TODO
