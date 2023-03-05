"""
Defines the Structure, Content, and Serialization for an IPC Message
containing a group of Packets received in the downlink direction during a read
event.

@author: Connor W. Colombo (CMU)
@last-updated: 03/04/2023
"""
from typing import List
import attr

from IrisBackendv3.ipc.inter_process_message import IpmSubclassFactory, MessageContentAttrMixin
from IrisBackendv3.codec.packet import Packet


@attr.s(frozen=True, cmp=True, slots=True, auto_attribs=True)
class DownlinkedPacketsContent(MessageContentAttrMixin):
    """
    Defines the Message Content wrapping a collection `Packets` received from
    the downlink direction during one read.
    """
    packets: List[Packet]


"""
Defines the Message Structure and Serialization Scheme for a `Packet` 
received from the downlink direction.
"""
DownlinkedPacketsMessage = IpmSubclassFactory(DownlinkedPacketsContent)
