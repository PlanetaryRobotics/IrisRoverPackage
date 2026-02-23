"""
Defines a subclass of `WatchdogDebugPacket`, for any debug packets that include
"Important" keywords that mean this DEBUG message should be highlighted.
(see `IMPORTANT_DEBUG_KEYWORDS` for a list).

@author: Connor W. Colombo (CMU)
@last-updated: 09/07/2022
"""
from IrisBackendv3.codec.packet_classes.watchdog_debug import *

from typing import Final, List

# Keywords to highlight if in debug msg (NOT case-sensitive):
IMPORTANT_DEBUG_KEYWORDS: Final[List[bytes]] = [
    b"Err",
    b"Warn",
    b"Crit",
    b"Fatal",
    b"Important",
    b"Notice",
    b"Boot"
]


class WatchdogDebugImportantPacket(WatchdogDebugPacket):
    """
    Creates a data-structure to allow storing and handling an important debug
    string from the Watchdog.

    @author: Connor W. Colombo (CMU)
    @last-updated: 09/07/2022
    """
    __slots__: List[str] = []  # empty __slots__ preserves parent __slots__

    @classmethod
    def is_valid(cls, data: bytes, endianness_code: str = ENDIANNESS_CODE) -> bool:
        """Valid if the packet starts with `b'DEBUG'`."""
        return (
            # Is a valid DEBUG packet:
            WatchdogDebugPacket.is_valid(data, endianness_code)
            # And contains an important keyword:
            and any(k.lower() in data.lower() for k in IMPORTANT_DEBUG_KEYWORDS)
        )

    def __repr__(self) -> str:
        if self._raw is None:
            l = 0
            data = b''
        else:
            l = len(self._raw)
            data = self._raw
        return f"WatchdogDebugImportantPacket[{l}B]: {data!r} "

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

        return (
            f" \033[48;5;172m\033[38;5;15m\033[1m[{len(data[5:])}B]: {formatted_data}\033[0m"
            # f"\n \033[1m\033[30m WatchdogDebugImportantPacket[{l}B]-str: {data!r} \033[0m\n"
            # f"\WatchdogDebugImportantPacket[{l}B]-raw: {' '.join('{:02x}'.format(x) for x in data)}"
        )
