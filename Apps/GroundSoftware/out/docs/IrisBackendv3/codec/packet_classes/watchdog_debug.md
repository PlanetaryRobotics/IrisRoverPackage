Module IrisBackendv3.codec.packet_classes.watchdog_debug
========================================================
Defines `WatchdogDebugPacket`, a `Packet` wrapper for a debug string from the
Watchdog. This packet type doesn't contain any telemetry (for now, should get
converted to a `EventPayload` eventually) and is just printed to the console.

@author: Connor W. Colombo (CMU)
@last-updated: 05/08/2022

Classes
-------

`WatchdogDebugPacket(payloads: Optional[EnhancedPayloadCollection] = None, raw: Optional[bytes] = None, endianness_code: str = '<')`
:   Creates a data-structure to allow storing and handling a debug string from
    the Watchdog.
    
    @author: Connor W. Colombo (CMU)
    @last-updated: 05/08/2022

    ### Ancestors (in MRO)

    * IrisBackendv3.codec.packet_classes.watchdog_debug.WatchdogDebugPacketInterface
    * IrisBackendv3.codec.packet_classes.packet.Packet
    * IrisBackendv3.codec.container.ContainerCodec
    * typing.Generic
    * abc.ABC

    ### Descendants

    * IrisBackendv3.codec.packet_classes.watchdog_debug_important.WatchdogDebugImportantPacket

    ### Static methods

    `build_minimum_packet(payloads: EnhancedPayloadCollection, raw: Optional[bytes], endianness_code: str) ‑> IrisBackendv3.codec.packet_classes.watchdog_debug.WatchdogDebugPacket`
    :   Minimum packet is just the packet.

    `is_valid(data: bytes, endianness_code: str = '<') ‑> bool`
    :   Valid if the packet starts with `b'DEBUG'`.

`WatchdogDebugPacketInterface(payloads: EnhancedPayloadCollection, raw: Optional[bytes] = None, endianness_code: str = '<')`
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

    * IrisBackendv3.codec.packet_classes.watchdog_debug.WatchdogDebugPacket