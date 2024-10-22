Module IrisBackendv3.codec.packet_classes.radio_direct_message
==============================================================
Defines `RadioDirectMessagePacket`, a `Packet` wrapper for a direct status message from the Radio.
This packet type doesn't contain any telemetry (for now - eventually, depending on contents,
should get converted to a `EventPayload` or a `TelemetryPayload` containing radio state,
activity, etc.) and is just printed to the console.

@author: Connor W. Colombo (CMU)
@last-updated: 06/01/2023

Classes
-------

`RadioDirectMessagePacket(payloads: Optional[EnhancedPayloadCollection] = None, raw: Optional[bytes] = None, endianness_code: str = '<')`
:   Creates a data-structure to allow storing and handling a status message from
    the Radio.
    
    @author: Connor W. Colombo (CMU)
    @last-updated: 06/01/2023

    ### Ancestors (in MRO)

    * IrisBackendv3.codec.packet_classes.radio_direct_message.RadioDirectMessagePacketInterface
    * IrisBackendv3.codec.packet_classes.gds_packet_event_mixin.GdsPacketEventPacket
    * IrisBackendv3.codec.packet_classes.packet.Packet
    * IrisBackendv3.codec.container.ContainerCodec
    * typing.Generic
    * abc.ABC
    * IrisBackendv3.codec.packet_classes.gds_packet_event_mixin.GdsPacketEventMixin

    ### Static methods

    `build_minimum_packet(payloads: EnhancedPayloadCollection, raw: Optional[bytes], endianness_code: str) ‑> IrisBackendv3.codec.packet_classes.radio_direct_message.RadioDirectMessagePacket`
    :   Minimum packet is just the packet.

    `is_valid(data: bytes, endianness_code: str = '<') ‑> bool`
    :   Valid if the packet starts with b'DEBUG' then `0xE6E7E7E6`.
        Note the b'DEBUG' prefix is there b/c this is sent through the Hercules
        then through the Watchdog using the debug messaging system.

`RadioDirectMessagePacketInterface(payloads: Optional[EnhancedPayloadCollection] = None, raw: Optional[bytes] = None, endianness_code: str = '<')`
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

    * IrisBackendv3.codec.packet_classes.radio_direct_message.RadioDirectMessagePacket