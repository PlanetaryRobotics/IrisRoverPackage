"""
Defines the Structure, Content, and Serialization for all incoming and outgoing 
Messages for the Transceiver layer over IPC.

@author: Connor W. Colombo (CMU)
@last-updated: 02/26/2022
"""
from __future__ import annotations  # Activate postponed annotations (for using classes as return type in their own methods)

from typing import Type, List
import attr

from IrisBackendv3.ipc.wrapper import InterProcessMessage
from IrisBackendv3.codec.packet import UplinkPacket, DownlinkPacket
from IrisBackendv3.codec.metadata import DataPathway, DataSource


@attr.s(frozen=True, cmp=True, slots=True, auto_attribs=True)
class SendPacketRequestContent:
    """
    Defines the Message Content for a Request to Send a `Packet` in the uplink 
    direction.
    """
    packet: UplinkPacket


class SendPacketRequestMessage(InterProcessMessage[SendPacketRequestContent]):
    """
    Defines the Message Structure and Serialization Scheme for a Request to 
    Send a `Packet` in the uplink direction.
    """

    def to_ipc_bytes(self) -> bytes:
        raise NotImplementedError()  # !TODO

    def from_ipc_bytes(cls, data: bytes) -> SendPacketRequestContent:
        raise NotImplementedError()  # !TODO


@attr.s(frozen=True, cmp=True, slots=True, auto_attribs=True)
class ReceivedPacketContent:
    """
    Defines the Message Content wrapping a `Packet` received from the downlink 
    direction.
    """
    packet: DownlinkPacket


class ReceivedPacketMessage(InterProcessMessage[ReceivedPacketContent]):
    """
    Defines the Message Structure and Serialization Scheme for a `Packet` 
    received from the downlink direction.
    """

    def to_ipc_bytes(self) -> bytes:
        raise NotImplementedError()  # !TODO

    def from_ipc_bytes(cls, data: bytes) -> ReceivedPacketContent:
        raise NotImplementedError()  # !TODO
