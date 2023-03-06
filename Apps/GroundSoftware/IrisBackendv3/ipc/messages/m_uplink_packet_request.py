"""
Defines the Structure, Content, and Serialization for an IPC Message
requesting that a Transceiver(s) Uplink a Packet.

@author: Connor W. Colombo (CMU)
@last-updated: 03/06/2023
"""
import attr

from IrisBackendv3.ipc.inter_process_message import IpmSubclassFactory, MessageContentAttrMixin
from IrisBackendv3.transceiver.xcvr_enum import TransceiverEnum
from IrisBackendv3.codec.packet import Packet


@attr.s(frozen=True, cmp=True, slots=True, auto_attribs=True)
class UplinkPacketRequestContent(MessageContentAttrMixin):
    """
    Defines the Message Content for a Request to Send a `Packet` in the uplink 
    direction.

    Args:
        `packet` (`Packet`): Packet to uplink.

        `target_xcvr` (`TransceiverEnum`): Which transceiver classes should be
        allowed to handle this (or `ALL` to allow Packets to be uplinked by
        ANY/ALL Transceivers, in which case XCVRs will make their own decisions
        based on packet contents, e.g. Pathway).
    """
    packet: Packet
    target_xcvr: TransceiverEnum = TransceiverEnum.ALL


UplinkPacketRequestMessage = IpmSubclassFactory(UplinkPacketRequestContent)
