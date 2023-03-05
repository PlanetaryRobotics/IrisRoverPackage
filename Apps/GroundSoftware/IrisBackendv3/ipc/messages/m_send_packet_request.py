"""
Defines the Structure, Content, and Serialization for an IPC Message
requesting that a Transceiver Uplink a Packet.

@author: Connor W. Colombo (CMU)
@last-updated: 03/03/2023
"""
import attr

from IrisBackendv3.ipc.inter_process_message import IpmSubclassFactory, MessageContentAttrMixin
from IrisBackendv3.codec.packet import Packet


@attr.s(frozen=True, cmp=True, slots=True, auto_attribs=True)
class UplinkPacketRequestContent(MessageContentAttrMixin):
    """
    Defines the Message Content for a Request to Send a `Packet` in the uplink 
    direction.
    """
    packet: Packet
    # Add a target XCVR field so we can specify if we care?


"""
Defines the Message Structure and Serialization Scheme for a `Packet` 
received from the downlink direction.
"""
UplinkPacketRequestMessage = IpmSubclassFactory(UplinkPacketRequestContent)
