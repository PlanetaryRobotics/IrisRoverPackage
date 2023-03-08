Module IrisBackendv3.codec.packet_classes.watchdog_radio_debug
==============================================================
Defines `WatchdogRadioDebugPacket`, a `Packet` wrapper for a debug string from the
Radio via Hercules via the Watchdog. This packet type doesn't contain any telemetry
(for now, should get converted to a `EventPayload` eventually) and is just printed
to the console.

@author: Connor W. Colombo (CMU)
@last-updated: 08/26/2022

Classes
-------

`WatchdogRadioDebugPacket(payloads: Optional[EnhancedPayloadCollection] = None, raw: Optional[bytes] = None, endianness_code: str = '<')`
:   Creates a data-structure to allow storing and handling a debug string from
    the Radio (or Hercules' Radio process).
    
    @author: Connor W. Colombo (CMU)
    @last-updated: 08/26/2022

    ### Ancestors (in MRO)

    * IrisBackendv3.codec.packet_classes.watchdog_radio_debug.WatchdogRadioDebugPacketInterface
    * IrisBackendv3.codec.packet_classes.packet.Packet
    * IrisBackendv3.codec.container.ContainerCodec
    * typing.Generic
    * abc.ABC

    ### Static methods

    `build_minimum_packet(payloads: EnhancedPayloadCollection, raw: Optional[bytes], endianness_code: str) ‑> IrisBackendv3.codec.packet_classes.watchdog_radio_debug.WatchdogRadioDebugPacket`
    :   Minimum packet is just the packet.

    `is_valid(data: bytes, endianness_code: str = '<') ‑> bool`
    :   Valid if the packet starts with `b'DEBUGRADIO'`.

`WatchdogRadioDebugPacketInterface(payloads: EnhancedPayloadCollection, raw: Optional[bytes] = None, endianness_code: str = '<')`
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

    * IrisBackendv3.codec.packet_classes.watchdog_radio_debug.WatchdogRadioDebugPacket