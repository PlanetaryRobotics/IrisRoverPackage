Module IrisBackendv3.codec.packet_classes.watchdog_debug_important
==================================================================
Defines a subclass of `WatchdogDebugPacket`, for any debug packets that include
"Important" keywords that mean this DEBUG message should be highlighted.
(see `IMPORTANT_DEBUG_KEYWORDS` for a list).

@author: Connor W. Colombo (CMU)
@last-updated: 09/07/2022

Classes
-------

`WatchdogDebugImportantPacket(payloads: Optional[EnhancedPayloadCollection] = None, raw: Optional[bytes] = None, endianness_code: str = '<')`
:   Creates a data-structure to allow storing and handling an important debug
    string from the Watchdog.
    
    @author: Connor W. Colombo (CMU)
    @last-updated: 09/07/2022

    ### Ancestors (in MRO)

    * IrisBackendv3.codec.packet_classes.watchdog_debug.WatchdogDebugPacket
    * IrisBackendv3.codec.packet_classes.watchdog_debug.WatchdogDebugPacketInterface
    * IrisBackendv3.codec.packet_classes.gds_packet_event_mixin.GdsPacketEventPacket
    * IrisBackendv3.codec.packet_classes.packet.Packet
    * IrisBackendv3.codec.container.ContainerCodec
    * typing.Generic
    * abc.ABC
    * IrisBackendv3.codec.packet_classes.gds_packet_event_mixin.GdsPacketEventMixin