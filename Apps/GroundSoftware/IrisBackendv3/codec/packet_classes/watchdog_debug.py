"""
Defines `WatchdogDebugPacket`, a `Packet` wrapper for a debug string from the
Watchdog. This packet type doesn't contain any telemetry (for now, should get
converted to a `EventPayload` eventually) and is just printed to the console.

@author: Connor W. Colombo (CMU)
@last-updated: 06/01/2023
"""
from __future__ import annotations

# Activate postponed annotations (for using classes as return type in their own methods)
from prompt_toolkit import formatted_text

from .gds_packet_event_mixin import GdsPacketEventPacket, GDS_EVT_PT
from IrisBackendv3.codec.packet_classes.packet import Packet, CT

from typing import List, Any, Optional

from scapy.utils import hexdump  # type: ignore

from IrisBackendv3.codec.payload_collection import EnhancedPayloadCollection

from IrisBackendv3.codec.settings import ENDIANNESS_CODE
from IrisBackendv3.codec.logs import logger


class WatchdogDebugPacketInterface(GdsPacketEventPacket[GDS_EVT_PT]):
    # empty __slots__ preserves parent class __slots__
    __slots__: List[str] = []


class WatchdogDebugPacket(WatchdogDebugPacketInterface[WatchdogDebugPacketInterface]):
    """
    Creates a data-structure to allow storing and handling a debug string from
    the Watchdog.

    @author: Connor W. Colombo (CMU)
    @last-updated: 06/01/2023
    """
    __slots__: List[str] = []  # empty __slots__ preserves parent __slots__

    def __init__(self,
                 payloads: Optional[EnhancedPayloadCollection] = None,
                 raw: Optional[bytes] = None,
                 endianness_code: str = ENDIANNESS_CODE
                 ) -> None:

        if raw is None and (payloads is None or payloads.is_empty()):
            # If both `payloads` and `raw` are `None`, what even caused
            # this to be generated?
            logger.debug(
                "A `WatchdogDebugPacket` was constructed with no `payloads` and "
                "no `raw` data. This suggests it's being created from "
                "nothing as a completely empty packet. Was this "
                "intentional or is this a bug?"
            )

        if payloads is None:
            # Except possibly in future subclasses, this should normally be
            # empty for an `WatchdogDebugPacket`. `payloads` needs to stay as
            # an `__init__` arg to avoid violating the Liskov substitution
            # principle.
            payloads = EnhancedPayloadCollection()

        super().__init__(
            payloads=payloads,
            raw=raw,
            endianness_code=endianness_code
        )  # passthru

    def encode(self, **kwargs: Any) -> bytes:
        # There's no encoding to do. It's just raw data.
        if self._raw is None:
            return b''
        else:
            return self._raw

    @classmethod
    def build_minimum_packet(
        cls,
        payloads: EnhancedPayloadCollection,
        raw: Optional[bytes],
        endianness_code: str
    ) -> WatchdogDebugPacket:
        """ Minimum packet is just the packet. """
        return cls(
            payloads=payloads,
            raw=raw,
            endianness_code=endianness_code
        )

    @classmethod
    def is_valid(cls, data: bytes, endianness_code: str = ENDIANNESS_CODE) -> bool:
        """Valid if the packet starts with `b'DEBUG'`."""
        return (
            data[:5].upper() == b'DEBUG'
        )

    def __repr__(self) -> str:
        if self._raw is None:
            l = 0
            data = b''
        else:
            l = len(self._raw)
            data = self._raw
        return f"WatchdogDebugPacket[{l}B]: {data!r} "

    def __str__(self) -> str:
        if self._raw is None:
            l = 0
            data = b''
        else:
            l = len(self._raw)
            data = self._raw

        try:
            formatted_data = data[5:].decode('utf-8').rstrip('\x00').rstrip()
        except UnicodeDecodeError:
            # Contains non-unicode characters
            formatted_data = str(data[5:])

        if data[:5].upper() == b'DL-FL':
            # TODO: Make DL flushes their own thing
            formatted_data = 'DL FLUSH ' + formatted_data

        return (
            f" \033[1m\033[38;5;243m[{len(data[5:])}B]: {formatted_data}\033[0m"
            # f"\n \033[1m\033[30m WatchdogDebugPacket[{l}B]-str: {data!r} \033[0m\n"
            # f"\nWatchdogDebugPacket[{l}B]-raw: {' '.join('{:02x}'.format(x) for x in data)}"
        )
