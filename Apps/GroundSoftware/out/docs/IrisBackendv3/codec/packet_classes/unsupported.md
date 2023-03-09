Module IrisBackendv3.codec.packet_classes.unsupported
=====================================================
Defines `UnsupportedPacket`, a `Packet` wrapper for unsupported sequences of raw
bytes.

@author: Connor W. Colombo (CMU)
@last-updated: 04/15/2022

Classes
-------

`UnsupportedPacket(payloads: Optional[EnhancedPayloadCollection] = None, raw: Optional[bytes] = None, endianness_code: str = '<')`
:   Creates a data-structure to allow storing and handling un-parsable
    (possibly corrupted or just not using a supported format) packets alongside
    normal packets, instead of just throwing the data out.
    
    @author: Connor W. Colombo (CMU)
    @last-updated: 04/15/2022

    ### Ancestors (in MRO)

    * IrisBackendv3.codec.packet_classes.unsupported.UnsupportedPacketInterface
    * IrisBackendv3.codec.packet_classes.packet.Packet
    * IrisBackendv3.codec.container.ContainerCodec
    * typing.Generic
    * abc.ABC

    ### Static methods

    `build_minimum_packet(payloads: EnhancedPayloadCollection, raw: Optional[bytes], endianness_code: str) ‑> IrisBackendv3.codec.packet_classes.unsupported.UnsupportedPacket`
    :   Minimum packet is just the packet.

    `is_valid(data: bytes, endianness_code: str = '<') ‑> bool`
    :   Anything can count as an `UnsupportedPacket` so long as it failed
        the `is_valid` checks of whatever `Packets` were considered supported,
        so this just returns `True`.

`UnsupportedPacketInterface(payloads: EnhancedPayloadCollection, raw: Optional[bytes] = None, endianness_code: str = '<')`
:   A special `CodecContainer` used for Packet data which combines the
    standardized data-encoding features of `ContainerCodec` with a standard
    `EnhancedPayloadCollection`. All packets are essentially 
    `EnhancedPayloadCollection`s with specialized encoding and decoding schemes
    applied. Each `Packet` en/decodes raw bytes sent over a `Transceiver` layer
    with any network headers (IP etc.) already stripped off.

    ### Ancestors (in MRO)

    * IrisBackendv3.codec.packet_classes.packet.Packet
    * IrisBackendv3.codec.container.ContainerCodec
    * typing.Generic
    * abc.ABC

    ### Descendants

    * IrisBackendv3.codec.packet_classes.unsupported.UnsupportedPacket