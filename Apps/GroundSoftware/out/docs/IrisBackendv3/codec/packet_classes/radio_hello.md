Module IrisBackendv3.codec.packet_classes.radio_hello
=====================================================
Defines `RadioHelloPacket`, a `Packet` wrapper for a hello message from the
Radio, emitted whenever the Radio (re)connects to a network. This packet type
doesn't contain any telemetry but does contain a single EventPayload and is
just printed to the console.

@author: Connor W. Colombo (CMU)
@last-updated: 06/22/2023

Classes
-------

`RadioHelloPacket(payloads: Optional[EnhancedPayloadCollection] = None, raw: Optional[bytes] = None, endianness_code: str = '<')`
:   Creates a data-structure to allow storing and handling a "Hello" message
    from the Radio.
    
    @author: Connor W. Colombo (CMU)
    @last-updated: 06/22/2023

    ### Ancestors (in MRO)

    * IrisBackendv3.codec.packet_classes.radio_hello.RadioHelloPacketInterface
    * IrisBackendv3.codec.packet_classes.packet.Packet
    * IrisBackendv3.codec.container.ContainerCodec
    * typing.Generic
    * abc.ABC

    ### Class variables

    `EVENT_NAME: Final[str]`
    :

    `EXPECTED_MESSAGE: Final[bytes]`
    :

    `MODULE_NAME: Final[str]`
    :

    ### Static methods

    `build_minimum_packet(payloads: EnhancedPayloadCollection, raw: Optional[bytes], endianness_code: str) ‑> IrisBackendv3.codec.packet_classes.radio_hello.RadioHelloPacket`
    :   Minimum packet is just the packet.

    `is_valid(data: bytes, endianness_code: str = '<') ‑> bool`
    :   Valid if the packet starts with the expected message.
        This still leaves some room in case extra garbage somehow got tacked
        onto the end.

    ### Methods

    `grab_event_payload(self) ‑> Optional[IrisBackendv3.codec.payload.EventPayload]`
    :   Grabs the first EventPayload that matches MODULE_NAME::EVENT_NAME.
        There should be exactly one of these, no more, no fewer, so long as 
        no one's messed with this packet. Still, for robustness, we have to
        grab this properly in case that assumption doesn't hold.

`RadioHelloPacketInterface(payloads: EnhancedPayloadCollection, raw: Optional[bytes] = None, endianness_code: str = '<')`
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

    * IrisBackendv3.codec.packet_classes.radio_hello.RadioHelloPacket