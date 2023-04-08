Module IrisBackendv3.codec.packet
=================================
Defines Common Data Required for Packets. Support for Building and Parsing
Packets.

@author: Connor W. Colombo (CMU)
@last-updated: 03/05/2023

Functions
---------

    
`parse_packet(packet_bytes: bytes, codecs: Optional[List[Type[Packet]]] = None) ‑> IrisBackendv3.codec.packet_classes.packet.Packet`
:   Parses the given packet bytes (with any network headers already stripped off)
    and returns a packet if the given bytes fit any of the given packet `codecs`
    (Packet classes). Returns the `packet_bytes` wrapped in an
    `UnsupportedPacket` if not.