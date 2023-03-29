Module IrisBackendv3.ipc.messages.m_uplink_packet_request
=========================================================
Defines the Structure, Content, and Serialization for an IPC Message
requesting that a Transceiver(s) Uplink a Packet.

@author: Connor W. Colombo (CMU)
@last-updated: 03/06/2023

Classes
-------

`UplinkPacketRequestContent(packet: IrisBackendv3.codec.packet_classes.packet.Packet, target_xcvr: IrisBackendv3.transceiver.xcvr_enum.TransceiverEnum = TransceiverEnum.ALL)`
:   Defines the Message Content for a Request to Send a `Packet` in the uplink 
    direction.
    
    Args:
        `packet` (`Packet`): Packet to uplink.
    
        `target_xcvr` (`TransceiverEnum`): Which transceiver classes should be
        allowed to handle this (or `ALL` to allow Packets to be uplinked by
        ANY/ALL Transceivers, in which case XCVRs will make their own decisions
        based on packet contents, e.g. Pathway).
    
    Method generated by attrs for class UplinkPacketRequestContent.

    ### Ancestors (in MRO)

    * IrisBackendv3.ipc.inter_process_message.MessageContentAttrMixin
    * IrisBackendv3.ipc.inter_process_message.MessageContentInterface
    * abc.ABC

    ### Instance variables

    `packet: IrisBackendv3.codec.packet_classes.packet.Packet`
    :   Return an attribute of instance, which is of type owner.

    `target_xcvr: IrisBackendv3.transceiver.xcvr_enum.TransceiverEnum`
    :   Return an attribute of instance, which is of type owner.