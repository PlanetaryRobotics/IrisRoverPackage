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
from IrisBackendv3.codec.packet_classes.packet import Packet


@attr.s(frozen=True, cmp=True, slots=True, auto_attribs=True)
class DownlinkedPacketsContent(MessageContentAttrMixin):
    """
    Defines the Message Content wrapping a collection `Packets` received from
    the downlink direction during one read by a transceiver.

    Args:
        `packets` (`List[Packet]`): Packets downlinked.
    """
    packets: List[Packet]

    def simple_str(self) -> str:
        return f"{self.__class__.__name__}({len(self.packets)} Packets)"


"""
Defines the Message Structure and Serialization Scheme for a `Packet` 
received from the downlink direction.
"""
DownlinkedPacketsMessage = IpmSubclassFactory(DownlinkedPacketsContent)