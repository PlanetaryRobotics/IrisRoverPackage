Module IrisBackendv3.codec.packet_classes.safety_timer
======================================================
Defines `WatchdogSafetyTimerPacket`, a `Packet` wrapper for a safety timer
status message string from the Watchdog. This packet type doesn't contain any
Telemetry but will be treated as an Event.

Since we couldn't get WD to cleanly export cs as an *unsigned* int, we downlink
as hex and this packet converts hex timestamps to integers for strings and
events (raw bytes are left intact for archive purposes).
Also handles converting tenth-timer times to full-timer times and converting cs
to sec or min.

@author: Connor W. Colombo (CMU)
@last-updated: 10/19/2023

Functions
---------

    
`convert_hex_times_cs(time_vals_hex: List[bytes]) ‑> List[int | None]`
:   Processes the given list of hex time strings in centiseconds to and
    converts them to decimal, multiplying by a coefficient if required.

    
`cs_time_to_human_time(time: int) ‑> bytes`
:   Converts a time value in centiseconds to a human readable time in
    minutes, seconds, or ms.

    
`extract_first_hex(data: bytes) ‑> int | None`
:   Extracts & converts the first hex number from the given bytes sequence.

    
`humanize_all_hex_cs_times(data: bytes) ‑> bytes`
:   Extracts all centisecond hex times from the given string, converts them
    to decimal, multiplying as necessary, and them converts them to a
    human-readable form (sensible units).

    
`humanize_hex_cs_match(time_val_hex_match: re.Match[bytes]) ‑> bytes`
:   Converts the given hex centisecond match obj. (from `TIME_REGEX_CS`)
    to centiseconds (multiplying by a coeff if necessary) and converts it to
    human-readable units.
    Helper function for `re.sub`.

    
`humanize_safety_timer_status_report(data: bytes) ‑> bytes`
:   Reports a human-readable version of Safety Timer Status Reports
    (part of Report Status responses).

Classes
-------

`WatchdogSafetyTimerPacket(payloads: Optional[EnhancedPayloadCollection] = None, raw: Optional[bytes] = None, endianness_code: str = '<')`
:   Creates a data-structure to allow storing and handling a safety timer
    update from the Watchdog.
    
    @author: Connor W. Colombo (CMU)
    @last-updated: 10/19/2023

    ### Ancestors (in MRO)

    * IrisBackendv3.codec.packet_classes.safety_timer.WatchdogSafetyTimerPacketInterface
    * IrisBackendv3.codec.packet_classes.gds_packet_event_mixin.GdsPacketEventPacket
    * IrisBackendv3.codec.packet_classes.packet.Packet
    * IrisBackendv3.codec.container.ContainerCodec
    * typing.Generic
    * abc.ABC
    * IrisBackendv3.codec.packet_classes.gds_packet_event_mixin.GdsPacketEventMixin

    ### Static methods

    `build_minimum_packet(payloads: EnhancedPayloadCollection, raw: Optional[bytes], endianness_code: str) ‑> IrisBackendv3.codec.packet_classes.safety_timer.WatchdogSafetyTimerPacket`
    :   Minimum packet is just the packet.

    `is_valid(data: bytes, endianness_code: str = '<') ‑> bool`
    :   Valid if the packet starts with `b'DEBUG'` and contains the SAFETY
        TIMER message identifier.

`WatchdogSafetyTimerPacketInterface(payloads: Optional[EnhancedPayloadCollection] = None, raw: Optional[bytes] = None, endianness_code: str = '<')`
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

    * IrisBackendv3.codec.packet_classes.safety_timer.WatchdogSafetyTimerPacket