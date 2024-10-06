"""
Defines Common Data Required for Packets. Support for Building and Parsing
Packets.

@author: Connor W. Colombo (CMU)
@last-updated: 06/22/2023
"""
from __future__ import annotations  # Activate postponed annotations (for using classes as return type in their own methods)
# Since we're instantiating `UnsupportedPacket` here, import it from root to prevent pickle problems:
# Absolute imports are paramount here to make sure one and only one copy of every module exists (otherwise pickle gets thrown off)
from IrisBackendv3.codec.packet_classes.packet import Packet
from IrisBackendv3.codec.packet_classes.unsupported import UnsupportedPacket
from IrisBackendv3.codec.packet_classes.watchdog_heartbeat import WatchdogHeartbeatPacket
from IrisBackendv3.codec.packet_classes.watchdog_command_response import WatchdogCommandResponsePacket
from IrisBackendv3.codec.packet_classes.iris_common import IrisCommonPacket, Legacy2020IrisCommonPacket
from IrisBackendv3.codec.packet_classes.watchdog_detailed_status import WatchdogDetailedStatusPacket
from IrisBackendv3.codec.packet_classes.watchdog_tvac_heartbeat import WatchdogTvacHeartbeatPacket
from IrisBackendv3.codec.packet_classes.radio_ground import RadioGroundPacket
from IrisBackendv3.codec.packet_classes.radio_hello import RadioHelloPacket
from IrisBackendv3.codec.packet_classes.radio_bgapi_packet import RadioBgApiPacket
from IrisBackendv3.codec.packet_classes.radio_uart_byte import RadioUartBytePacket
from IrisBackendv3.codec.packet_classes.radio_direct_message import RadioDirectMessagePacket
from IrisBackendv3.codec.packet_classes.watchdog_reset_specific_ack import WatchdogResetSpecificAckPacket
from IrisBackendv3.codec.packet_classes.watchdog_hello import WatchdogHelloPacket
from IrisBackendv3.codec.packet_classes.watchdog_radio_debug import WatchdogRadioDebugPacket
from IrisBackendv3.codec.packet_classes.watchdog_debug_important import WatchdogDebugImportantPacket
from IrisBackendv3.codec.packet_classes.watchdog_debug import WatchdogDebugPacket
from IrisBackendv3.codec.packet_classes.radio_downlink_flush import RadioDownlinkFlushPacket
from IrisBackendv3.codec.packet_classes.herc_radio_upl_ack import HerculesRadioUplinkAckPacket
from IrisBackendv3.codec.packet_classes.safety_timer import WatchdogSafetyTimerPacket
from IrisBackendv3.codec.packet_classes.peregrine_dummy_packet import PeregrineDummyPacket

from typing import List, Optional, Type

import inspect
import traceback
import scapy.all as scp  # type: ignore

from IrisBackendv3.codec.logs import logger

# TODO: Add `__str__` / `__repr__`s


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
            # RS ACK needs to be above safety timer (so safety timer RS ack doesn't get treated as a Safety Timer msg)
            WatchdogResetSpecificAckPacket,
            WatchdogHelloPacket,
            WatchdogSafetyTimerPacket,
            HerculesRadioUplinkAckPacket,  # subtype of WatchdogRadioDebug
            WatchdogRadioDebugPacket,
            WatchdogDebugImportantPacket,
            WatchdogDebugPacket,
            RadioDownlinkFlushPacket,
            PeregrineDummyPacket  # never a valid decoder but included for completeness
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
        trace = traceback.format_exc()
        logger.warning(
            f"Had to abort packet parsing due to the following exception: `{e}`"
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
