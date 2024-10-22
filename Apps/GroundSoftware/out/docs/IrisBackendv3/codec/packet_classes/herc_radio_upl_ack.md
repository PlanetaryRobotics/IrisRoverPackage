Module IrisBackendv3.codec.packet_classes.herc_radio_upl_ack
============================================================
Defines `HerculesRadioUplinkAckPacket`, a `Packet` sent by Hercules at the
Radio-driver level as soon as it receives uplinked data from the Radio.
Lets us know that the data got there, even if it was corrupted later.

@author: Connor W. Colombo (CMU)
@last-updated: 06/22/2023

Classes
-------

`HerculesRadioUplinkAckPacket(payloads: Optional[EnhancedPayloadCollection] = None, raw: Optional[bytes] = None, endianness_code: str = '<')`
:   `Packet` sent by Hercules at the Radio-driver level as soon as it receives
    uplinked data from the Radio. Lets us know that the data got there, even
    if it was corrupted later.
    
    @author: Connor W. Colombo (CMU)
    @last-updated: 06/22/2023

    ### Ancestors (in MRO)

    * IrisBackendv3.codec.packet_classes.herc_radio_upl_ack.HerculesRadioUplinkAckPacketInterface
    * IrisBackendv3.codec.packet_classes.gds_packet_event_mixin.GdsPacketEventPacket
    * IrisBackendv3.codec.packet_classes.packet.Packet
    * IrisBackendv3.codec.container.ContainerCodec
    * typing.Generic
    * abc.ABC
    * IrisBackendv3.codec.packet_classes.gds_packet_event_mixin.GdsPacketEventMixin

    ### Static methods

    `build_minimum_packet(payloads: EnhancedPayloadCollection, raw: Optional[bytes], endianness_code: str) ‑> IrisBackendv3.codec.packet_classes.herc_radio_upl_ack.HerculesRadioUplinkAckPacket`
    :   Minimum packet is just the packet.

    `is_valid(data: bytes, endianness_code: str = '<') ‑> bool`
    :   Valid if the packet starts with the fixed packet header.

    ### Instance variables

    `ack_data: bytes`
    :

`HerculesRadioUplinkAckPacketInterface(payloads: Optional[EnhancedPayloadCollection] = None, raw: Optional[bytes] = None, endianness_code: str = '<')`
:   Stronger form of `GdsPacketEventMixin` that acts as a base class
    alternative to `Packet[CT]`.
    
    This is to be used for packets which don't contain payloads inside them
    but, instead, represent one singular event payload.

    ### Ancestors (in MRO)

    * IrisBackendv3.codec.packet_classes.gds_packet_event_mixin.GdsPacketEventPacket
    * IrisBackendv3.codec.packet_classes.packet.Packet
    * IrisBackendv3.codec.container.ContainerCodec
    * typing.Generic
    * abc.ABC
    * IrisBackendv3.codec.packet_classes.gds_packet_event_mixin.GdsPacketEventMixin

    ### Descendants

    * IrisBackendv3.codec.packet_classes.herc_radio_upl_ack.HerculesRadioUplinkAckPacket