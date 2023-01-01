"""
Defines Common Data Required for Packets. Support for Building and Parsing
Packets.

@author: Connor W. Colombo (CMU)
@last-updated: 10/26/2022
"""
from __future__ import annotations  # Activate postponed annotations (for using classes as return type in their own methods)
from .packet_classes import *

from typing import List, Optional, Type

import traceback
import scapy.all as scp  # type: ignore

from .logging import logger


# TODO: Add update hooks?
# ^- I think IPC will just take care of this (make it redundant).

# TODO: Add `__str__` / `__repr__`s

#! TODO: Handle serialization (must replace container scheme, augment by storing payloads with their metadata)
#! ^- or don't serialize as packets?... no, must be able to serialize to send over IPC network.
#! ^- augmenting containers with __reduce__ and __getstate__ seems like it will work for IPC.


def parse_packet(
    packet_bytes: bytes,
    codecs: Optional[List[Type[Packet]]] = None
) -> Packet:
    """
    Parses the given packet bytes (with any network headers already stripped off)
    and returns a packet if the given bytes fit any of the given packet `codecs`
    (Packet classes). Returns the `packet_bytes` wrapped in an
    `UnsupportedPacket` if not.
    """
    # Default: All available packet codecs (in order of use preference):
    if codecs is None:
        codecs = [
            WatchdogHeartbeatPacket,
            WatchdogCommandResponsePacket,
            IrisCommonPacket,
            WatchdogDetailedStatusPacket,
            WatchdogTvacHeartbeatPacket,
            Legacy2020IrisCommonPacket,
            RadioGroundPacket,
            RadioHelloPacket,
            RadioBgApiPacket,
            RadioUartBytePacket,
            RadioDirectMessagePacket,
            WatchdogResetSpecificAckPacket,
            WatchdogHelloPacket,
            WatchdogRadioDebugPacket,
            WatchdogDebugImportantPacket,
            WatchdogDebugPacket
        ]

    # Codecs which support this packet:
    supported = [c for c in codecs if c.is_valid(packet_bytes)]

    # Check for issues:
    if len(supported) == 0:
        logger.warning(
            f"Invalid packet detected. Does not conform to any supported specs: "  # type: ignore
            f"{packet_bytes}"
        )

    # Log a warning if this data is valid for multiple packet types, *IGNORING*
    # `WatchdogDebugPacket` and its direct variants (since it's used as a carrier
    # for several other packet types sent using WD `debugPrint...` in the FSW)
    # as well as `RadioHelloPacket` since we know it also would pass as a
    # `WatchdogHelloPacket` (which is why it has higher preference):
    if sum(1 for s in supported if s not in [WatchdogDebugPacket, WatchdogDebugImportantPacket, RadioHelloPacket]) > 1:
        logger.warning(
            f"Multiple codecs "  # type: ignore
            f"({supported}) support received packet. Using "
            f"highest in preference order: {supported[0]}. "
            f"Packet data: \n{scp.hexdump(packet_bytes, dump=True)}\n"
        )

    # Parse Packet:
    packet: Optional[Packet] = None  # un-parsed indicator
    try:
        if len(supported) > 0:
            # Parse:
            packet = supported[0].decode(
                packet_bytes
            )

    except Exception as e:
        err = e
        trace = traceback.format_exc()
        logger.warning(
            f"Had to abort packet parsing due to the following exception: `{err}`"
            f"The packet bytes being parsed were: \n"
            f"{scp.hexdump(packet_bytes, dump=True)}\n"
        )
        # Add more information if desired:
        logger.verbose(  # type: ignore
            f"\t > The stack trace of this error was: `{trace}`."
        )

    if packet is None:
        # if packet parsing failed or wasn't performed for any reason,
        # encapsulate in UnsupportedPacket:
        packet = UnsupportedPacket(raw=packet_bytes)

    return packet
