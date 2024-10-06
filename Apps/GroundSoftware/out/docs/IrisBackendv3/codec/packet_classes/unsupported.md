Module IrisBackendv3.codec.packet_classes.unsupported
=====================================================
Defines `UnsupportedPacket`, a `Packet` wrapper for unsupported sequences of raw
bytes.

@author: Connor W. Colombo (CMU)
@last-updated: 06/01/2023

Classes
-------

`UnsupportedPacket(payloads: Optional[EnhancedPayloadCollection] = None, raw: Optional[bytes] = None, endianness_code: str = '<')`
:   Creates a data-structure to allow storing and handling un-parsable
    (possibly corrupted or just not using a supported format) packets alongside
    normal packets, instead of just throwing the data out.
    
    @author: Connor W. Colombo (CMU)
    @last-updated: 06/01/2023

    ### Ancestors (in MRO)

    * IrisBackendv3.codec.packet_classes.unsupported.UnsupportedPacketInterface
    * IrisBackendv3.codec.packet_classes.gds_packet_event_mixin.GdsPacketEventPacket
    * IrisBackendv3.codec.packet_classes.packet.Packet
    * IrisBackendv3.codec.container.ContainerCodec
    * typing.Generic
    * abc.ABC
    * IrisBackendv3.codec.packet_classes.gds_packet_event_mixin.GdsPacketEventMixin

    ### Static methods

    `build_minimum_packet(payloads: EnhancedPayloadCollection, raw: Optional[bytes], endianness_code: str) ‑> IrisBackendv3.codec.packet_classes.unsupported.UnsupportedPacket`
    :   Minimum packet is just the packet.

    `is_valid(data: bytes, endianness_code: str = '<') ‑> bool`
    :   Anything can count as an `UnsupportedPacket` so long as it failed
        the `is_valid` checks of whatever `Packets` were considered supported,
        so this just returns `True`.

`UnsupportedPacketInterface(payloads: Optional[EnhancedPayloadCollection] = None, raw: Optional[bytes] = None, endianness_code: str = '<')`
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

    * IrisBackendv3.codec.packet_classes.unsupported.UnsupportedPacket