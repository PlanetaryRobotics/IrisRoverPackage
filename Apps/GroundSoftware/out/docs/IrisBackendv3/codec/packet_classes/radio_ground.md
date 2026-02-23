Module IrisBackendv3.codec.packet_classes.radio_ground
======================================================
Implementation of Radio-Ground Packet used by data sent directly from the
Radio's internal MCU to Ground (over WiFi).

The Standard formatting followed by all Radio-Ground messages is:
  - "RAD:" header
  - [Message Type]: Sequence of ASCII characters defining message type
  - "|"
  - A Series of >=0 fields.
      - Each must be separated by "|" (final field is not followed by a "|")
      - Field content itself must not contain the "|" character.
      - Any arbitrary raw hex sent instead of ASCII characters in a field must be directly preceded by an "h" and must either:
          - Have a known / fixed length (i.e. this field is always X bytes)
          - If variable in length, be the final field in the message (so the field's length doesn't need to be transmitted)

Note: All raw data buffers will be little-endian.

Note: this is all going to be wrapped inside a UDP packet, so we don't need
length information, CRC, etc. in the header of this message.

@author: Connor W. Colombo (CMU)
@last-updated: 09/17/2022

Functions
---------

    
`decode_radio_downlink_str(str_data: bytes) ‑> str`
:   Decodes a string sent as part of a Radio packet and makes it
    human-readable. We do this separately from the normal FPrime decoding
    since, for efficiency reasons, the Radio doesn't encode its strings with
    the FPrime standard (since it doesn't run FPrime):

    
`grab_event_arg(event: Event, name: str) ‑> IrisBackendv3.data_standards.module.Argument`
:   

    
`handle_bad_endpoint(**fields: bytes) ‑> IrisBackendv3.codec.payload_collection.EnhancedPayloadCollection`
:   

    
`handle_bad_herc(**fields: bytes) ‑> IrisBackendv3.codec.payload_collection.EnhancedPayloadCollection`
:   

    
`handle_bad_udp(**fields: bytes) ‑> IrisBackendv3.codec.payload_collection.EnhancedPayloadCollection`
:   

    
`handle_cmd_ack(**fields: bytes) ‑> IrisBackendv3.codec.payload_collection.EnhancedPayloadCollection`
:   

    
`handle_cmd_bad(**fields: bytes) ‑> IrisBackendv3.codec.payload_collection.EnhancedPayloadCollection`
:   

    
`handle_cmd_cb(event_name: str, **fields: bytes) ‑> IrisBackendv3.codec.payload_collection.EnhancedPayloadCollection`
:   General handler for all Radio command callbacks:

    
`handle_cmd_done(**fields: bytes) ‑> IrisBackendv3.codec.payload_collection.EnhancedPayloadCollection`
:   

    
`handle_critical_reset(**fields: bytes) ‑> IrisBackendv3.codec.payload_collection.EnhancedPayloadCollection`
:   

    
`handle_critical_reset_swe(**fields: bytes) ‑> IrisBackendv3.codec.payload_collection.EnhancedPayloadCollection`
:   

    
`handle_echo(**fields: bytes) ‑> IrisBackendv3.codec.payload_collection.EnhancedPayloadCollection`
:   

    
`handle_heartbeat(**fields: bytes) ‑> IrisBackendv3.codec.payload_collection.EnhancedPayloadCollection`
:   

    
`parse_event_args(event: Event, args: Dict[str, bytes]) ‑> Dict[str, Any]`
:   

Classes
-------

`RadioGroundPacket(subpacket_name: Optional[str] = None, payloads: Optional[EnhancedPayloadCollection] = None, raw: Optional[bytes] = None, endianness_code: str = '<')`
:   Creates a data-structure to allow storing and handling messages sent
    directly from the Radio's internal MCU.
    
    @author: Connor W. Colombo (CMU)
    @last-updated: 09/14/2022

    ### Ancestors (in MRO)

    * IrisBackendv3.codec.packet_classes.radio_ground.RadioGroundPacketInterface
    * IrisBackendv3.codec.packet_classes.packet.Packet
    * IrisBackendv3.codec.container.ContainerCodec
    * typing.Generic
    * abc.ABC

    ### Static methods

    `build_minimum_packet(payloads: EnhancedPayloadCollection, raw: Optional[bytes], endianness_code: str) ‑> IrisBackendv3.codec.packet_classes.radio_ground.RadioGroundPacket`
    :   Minimum packet is just the packet.

    `is_valid(data: bytes, endianness_code: str = '<') ‑> bool`
    :   Valid if the packet starts with `b'DEBUG'`.

    ### Instance variables

    `subpacket_name: str`
    :

`RadioGroundPacketInterface(payloads: EnhancedPayloadCollection, raw: Optional[bytes] = None, endianness_code: str = '<')`
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

    * IrisBackendv3.codec.packet_classes.radio_ground.RadioGroundPacket

`RadioGroundSubpacketDefinition(handler: RadioGroundSubpacketHandler, length: int, prefix: bytes, fields: OrderedDict[str, RadioGroundSubpacketField])`
:   RadioGroundSubpacketDefinition(handler: 'RadioGroundSubpacketHandler', length: 'int', prefix: 'bytes', fields: 'OrderedDict[str, RadioGroundSubpacketField]')

    ### Class variables

    `fields: collections.OrderedDict[str, IrisBackendv3.codec.packet_classes.radio_ground.RadioGroundSubpacketField]`
    :

    `handler: IrisBackendv3.codec.packet_classes.radio_ground.RadioGroundSubpacketHandler`
    :

    `length: int`
    :

    `prefix: bytes`
    :

    ### Methods

    `get_field_data(self, data: bytes) ‑> Dict[str, bytes]`
    :   Splits the given data up into fields.
        Returns a map from each field name found to the bytes in that field,
        excluding the initiator.

    `is_valid(self, data: bytes) ‑> bool`
    :   Returns whether the given data constitutes a valid instance of this
        Radio-Ground subpacket.

    `parse(self, data: bytes) ‑> IrisBackendv3.codec.payload_collection.EnhancedPayloadCollection`
    :   Parses the given data into Payloads using the handler after grabbing
        the component fields from the data.
        
        NOTE: This assumes the data has already been checked with `is_valid`.

`RadioGroundSubpacketField(initiator: bytes, length: int)`
:   RadioGroundSubpacketField(initiator: 'bytes', length: 'int')

    ### Class variables

    `initiator: bytes`
    :

    `length: int`
    :

`RadioGroundSubpacketHandler(*args, **kwargs)`
:   Structural Typing Protocol defining the structure of the callbacks that
    handle converting the fields of a RadioGroundSubpacket to a collection of
    Payloads.

    ### Ancestors (in MRO)

    * typing.Protocol
    * typing.Generic