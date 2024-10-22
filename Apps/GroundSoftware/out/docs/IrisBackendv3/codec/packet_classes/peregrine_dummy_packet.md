Module IrisBackendv3.codec.packet_classes.peregrine_dummy_packet
================================================================
Dummy "Packet" for holding telemetry received from Peregrine.

This exists because transceivers are expected to emit packets but the YAMCS
transceiver just receives raw payloads, so we create a dummy wrapper around the
payloads so they can be transmitted.

@author: Connor W. Colombo (CMU)
@last-updated: 01/04/2024

Classes
-------

`PeregrineDummyPacket(payloads: Optional[EnhancedPayloadCollection] = None, raw: Optional[bytes] = None, endianness_code: str = '<')`
:   Wrapper around an EnhancedPayloadCollection of payloads that came from
    Peregrine.
    
    @author: Connor W. Colombo (CMU)
    @last-updated: 01/04/2024

    ### Ancestors (in MRO)

    * IrisBackendv3.codec.packet_classes.peregrine_dummy_packet.PeregrineDummyPacketInterface
    * IrisBackendv3.codec.packet_classes.packet.Packet
    * IrisBackendv3.codec.container.ContainerCodec
    * typing.Generic
    * abc.ABC

    ### Static methods

    `build_minimum_packet(payloads: EnhancedPayloadCollection, raw: Optional[bytes], endianness_code: str) ‑> IrisBackendv3.codec.packet_classes.peregrine_dummy_packet.PeregrineDummyPacket`
    :   Minimum packet is just the packet of payloads.

    `is_valid(data: bytes, endianness_code: str = '<') ‑> bool`
    :   This is never a valid way to decode raw data (has no `decode`
        implementation, since that's meaningless here).

`PeregrineDummyPacketInterface(payloads: EnhancedPayloadCollection, raw: Optional[bytes] = None, endianness_code: str = '<')`
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

    * IrisBackendv3.codec.packet_classes.peregrine_dummy_packet.PeregrineDummyPacket