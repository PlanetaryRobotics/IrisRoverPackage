"""
Defines the Structure, Content, and Serialization for an IPC Message
requesting that a Transceiver(s) Uplink these Payloads, either as one or
multiple payloads.

@author: Connor W. Colombo (CMU)
@last-updated: 03/06/2023
"""
import attr
from enum import Enum
from typing import List, Type

from IrisBackendv3.ipc.inter_process_message import IpmSubclassFactory, MessageContentAttrMixin
from IrisBackendv3.transceiver.xcvr_enum import TransceiverEnum
from IrisBackendv3.codec.payload import Payload
from IrisBackendv3.codec.packet_classes.packet import Packet


class UplinkPayloadsPacketSplit(Enum):
    """How to split up these payloads into packets."""
    # Each payload **must** go into its own packet and they'll be uplinked in
    # the order given:
    INDIVIDUAL = 1
    # The payloads **must** all go in one packet (or an error will be thrown if
    # that's not possible):
    TOGETHER = 10
    # The IPC App handling the payloads->packets transformation gets to decide
    # how best to pack these payloads into a packet:
    ANY = 20


@attr.s(frozen=True, cmp=True, slots=True, auto_attribs=True)
class UplinkPayloadsRequestContent(MessageContentAttrMixin):
    """
    Defines the Message Content for a Request to a collection of `Payloads` in
    the uplink direction.

    Args:
        `payloads` (`List[Payload]`): Payloads to uplink:
            (using `List` not `EnhancedPayloadCollection` to make desired order
            obvious).

        `split` (`UplinkPayloadsPacketSplit`): How these `Payload`s should be split
            into `Packet`s.

        `packet_class` (`Type[Packet] | None`): What type of packet to put these
            payloads in (or `None` if the packet packer gets to decide).

        `target_xcvr` (`TransceiverEnum`): Which transceiver classes should be
            allowed to handle this (or `ALL` to allow Packets to be uplinked by
            ANY/ALL Transceivers, in which case XCVRs will make their own
            decisions based on packet contents, e.g. Pathway).
    """
    payloads: List[Payload]
    split: UplinkPayloadsPacketSplit = UplinkPayloadsPacketSplit.ANY
    packet_class: Type[Packet] | None = None
    target_xcvr: TransceiverEnum = TransceiverEnum.ALL

    def simple_str(self) -> str:
        return (
            f"{self.__class__.__name__}("
            f"{len(self.payloads)} Payloads, "
            f"{self.split=}, "
            f"{self.packet_class=}, "
            f"{self.target_xcvr=})"
        )


UplinkPayloadsRequestMessage = IpmSubclassFactory(UplinkPayloadsRequestContent)
