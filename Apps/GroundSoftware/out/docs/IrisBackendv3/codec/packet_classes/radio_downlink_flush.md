Module IrisBackendv3.codec.packet_classes.radio_downlink_flush
==============================================================
Defines `RadioDownlinkFlushPacket`, a `Packet` used to flush the Radio downlink
interface when downlinking large packets.

@author: Connor W. Colombo (CMU)
@last-updated: 06/01/2023

Classes
-------

`RadioDownlinkFlushPacket(payloads: Optional[EnhancedPayloadCollection] = None, raw: Optional[bytes] = None, endianness_code: str = '<')`
:   `Packet` used to flush the Radio downlink interface when downlinking large
    packets.
    
    @author: Connor W. Colombo (CMU)
    @last-updated: 06/01/2023

    ### Ancestors (in MRO)

    * IrisBackendv3.codec.packet_classes.radio_downlink_flush.RadioDownlinkFlushPacketInterface
    * IrisBackendv3.codec.packet_classes.gds_packet_event_mixin.GdsPacketEventPacket
    * IrisBackendv3.codec.packet_classes.packet.Packet
    * IrisBackendv3.codec.container.ContainerCodec
    * typing.Generic
    * abc.ABC
    * IrisBackendv3.codec.packet_classes.gds_packet_event_mixin.GdsPacketEventMixin

    ### Static methods

    `build_minimum_packet(payloads: EnhancedPayloadCollection, raw: Optional[bytes], endianness_code: str) ‑> IrisBackendv3.codec.packet_classes.radio_downlink_flush.RadioDownlinkFlushPacket`
    :   Minimum packet is just the packet.

    `is_valid(data: bytes, endianness_code: str = '<') ‑> bool`
    :   Valid if the packet starts with `b'DL-FL'`.

`RadioDownlinkFlushPacketInterface(payloads: Optional[EnhancedPayloadCollection] = None, raw: Optional[bytes] = None, endianness_code: str = '<')`
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

    * IrisBackendv3.codec.packet_classes.radio_downlink_flush.RadioDownlinkFlushPacket