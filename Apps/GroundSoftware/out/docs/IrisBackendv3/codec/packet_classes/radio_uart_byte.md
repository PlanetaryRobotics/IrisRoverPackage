Module IrisBackendv3.codec.packet_classes.radio_uart_byte
=========================================================
Defines `RadioUartBytePacket`, a `Packet` wrapper for a byte received by 
Hercules from the radio forwarded from the Radio in BGAPI Passthrough / 
Transparency Mode.
This packet type doesn't contain any telemetry (for now - eventually, depending on contents,
should get converted to a `EventPayload` or a `TelemetryPayload` containing radio state,
activity, etc.) and is just printed to the console.

@author: Connor W. Colombo (CMU)
@last-updated: 10/26/2022

Classes
-------

`RadioUartBytePacket(payloads: Optional[EnhancedPayloadCollection] = None, raw: Optional[bytes] = None, endianness_code: str = '<')`
:   Creates a data-structure to allow storing and handling a status message from
    the Radio.
    
    @author: Connor W. Colombo (CMU)
    @last-updated: 10/26/2022

    ### Ancestors (in MRO)

    * IrisBackendv3.codec.packet_classes.radio_uart_byte.RadioUartBytePacketInterface
    * IrisBackendv3.codec.packet_classes.packet.Packet
    * IrisBackendv3.codec.container.ContainerCodec
    * typing.Generic
    * abc.ABC

    ### Static methods

    `build_minimum_packet(payloads: EnhancedPayloadCollection, raw: Optional[bytes], endianness_code: str) ‑> IrisBackendv3.codec.packet_classes.radio_uart_byte.RadioUartBytePacket`
    :   Minimum packet is just the packet.

    `is_valid(data: bytes, endianness_code: str = '<') ‑> bool`
    :   Valid if the packet starts with `FIXED_PREFIX`.

`RadioUartBytePacketInterface(payloads: EnhancedPayloadCollection, raw: Optional[bytes] = None, endianness_code: str = '<')`
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

    * IrisBackendv3.codec.packet_classes.radio_uart_byte.RadioUartBytePacket