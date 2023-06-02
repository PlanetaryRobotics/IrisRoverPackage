"""
Defines `UnsupportedPacket`, a `Packet` wrapper for unsupported sequences of raw
bytes.

@author: Connor W. Colombo (CMU)
@last-updated: 06/01/2023
"""
from __future__ import annotations  # Activate postponed annotations (for using classes as return type in their own methods)

from IrisBackendv3.codec.packet_classes.packet import Packet, CT
from .gds_packet_event_mixin import GdsPacketEventPacket, GDS_EVT_PT

from typing import List, Any, Optional

from scapy.utils import hexdump  # type: ignore

from IrisBackendv3.codec.payload_collection import EnhancedPayloadCollection

from IrisBackendv3.codec.settings import ENDIANNESS_CODE
from IrisBackendv3.codec.logs import logger


class UnsupportedPacketInterface(GdsPacketEventPacket[GDS_EVT_PT]):
    # empty __slots__ preserves parent class __slots__
    __slots__: List[str] = []


class UnsupportedPacket(UnsupportedPacketInterface[UnsupportedPacketInterface]):
    """
    Creates a data-structure to allow storing and handling un-parsable
    (possibly corrupted or just not using a supported format) packets alongside
    normal packets, instead of just throwing the data out.

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
                "An Unsupported Packet was constructed with no `payloads` and "
                "no `raw` data. This suggests it's being created from "
                "nothing as a completely empty packet. Was this "
                "intentional or is this a bug?"
            )

        if payloads is None:
            # Except possibly in future subclasses, this should normally be
            # empty for an `UnsupportedPacket`. `payloads` needs to stay as an
            # `__init__` arg to avoid violating the Liskov substitution
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
    ) -> UnsupportedPacket:
        """ Minimum packet is just the packet. """
        return cls(
            payloads=payloads,
            raw=raw,
            endianness_code=endianness_code
        )

    @classmethod
    def is_valid(cls, data: bytes, endianness_code: str = ENDIANNESS_CODE) -> bool:
        """Anything can count as an `UnsupportedPacket` so long as it failed
        the `is_valid` checks of whatever `Packets` were considered supported,
        so this just returns `True`."""
        return True

    def __repr__(self) -> str:
        if self._raw is None:
            l = 0
            data = b''
        else:
            l = len(self._raw)
            data = self._raw
        return f"UnsupportedPacket[{l}B]: {data!r}"

    def __str__(self) -> str:
        if self._raw is None:
            l = 0
            data = b''
        else:
            l = len(self._raw)
            data = self._raw
        return f"UnsupportedPacket[{l}B]:\n{hexdump(data, dump=True)}"
