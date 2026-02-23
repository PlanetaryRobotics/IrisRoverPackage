Module IrisBackendv3.codec.packet_classes.packet
================================================
Defines `Packet` class, a special `CodecContainer` used for Packet data.

@author: Connor W. Colombo (CMU)
@last-updated: 09/15/2022

Classes
-------

`Packet(payloads: EnhancedPayloadCollection, raw: Optional[bytes] = None, endianness_code: str = '<')`
:   A special `CodecContainer` used for Packet data which combines the
    standardized data-encoding features of `ContainerCodec` with a standard
    `EnhancedPayloadCollection`. All packets are essentially 
    `EnhancedPayloadCollection`s with specialized encoding and decoding schemes
    applied. Each `Packet` en/decodes raw bytes sent over a `Transceiver` layer
    with any network headers (IP etc.) already stripped off.

    ### Ancestors (in MRO)

    * IrisBackendv3.codec.container.ContainerCodec
    * typing.Generic
    * abc.ABC

    ### Descendants

    * IrisBackendv3.codec.packet_classes.custom_payload.CustomPayloadPacket
    * IrisBackendv3.codec.packet_classes.gds_packet_event_mixin.GdsPacketEventPacket
    * IrisBackendv3.codec.packet_classes.iris_common.IrisCommonPacketInterface
    * IrisBackendv3.codec.packet_classes.peregrine_dummy_packet.PeregrineDummyPacketInterface
    * IrisBackendv3.codec.packet_classes.radio_ground.RadioGroundPacketInterface
    * IrisBackendv3.codec.packet_classes.radio_hello.RadioHelloPacketInterface

    ### Static methods

    `build_minimum_packet(payloads: EnhancedPayloadCollection, raw: Optional[bytes], endianness_code: str) ‑> ~CT`
    :   Builds a minimum representation of this Packet (before any
        additional elements from `__getstate__` are added back). Used by
        `Packet.__reduce__` for unpacking serialized data.

    `is_valid(data: bytes, endianness_code: str = '<') ‑> bool`
    :   Determines whether the given bytes constitute a valid packet of this type.

    ### Instance variables

    `pathway: IrisBackendv3.codec.metadata.DataPathway`
    :   Returns the pathway for the packet.

    `payloads: IrisBackendv3.codec.payload_collection.EnhancedPayloadCollection`
    :   Wrapper for `_payloads`. This getter can be overridden in subclasses
        for additional functionality (e.g. autocaching & updating of data used
        to generate or generated from payloads).

    `source: IrisBackendv3.codec.metadata.DataSource`
    :   Returns the source for the packet.