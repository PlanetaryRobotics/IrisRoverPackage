"""
Dummy "Packet" for holding telemetry received from Peregrine.

This exists because transceivers are expected to emit packets but the YAMCS
transceiver just receives raw payloads, so we create a dummy wrapper around the
payloads so they can be transmitted.

@author: Connor W. Colombo (CMU)
@last-updated: 01/04/2024
"""
# Activate postponed annotations (for using classes as return type in their own methods)
from __future__ import annotations

from IrisBackendv3.codec.packet_classes.packet import Packet, CT

from typing import Any, Optional, List

from IrisBackendv3.codec.payload_collection import EnhancedPayloadCollection
from IrisBackendv3.codec.settings import ENDIANNESS_CODE
from IrisBackendv3.codec.logs import logger


class PeregrineDummyPacketInterface(Packet[CT]):
    # empty __slots__ preserves parent class __slots__
    __slots__: List[str] = []


class PeregrineDummyPacket(PeregrineDummyPacketInterface[PeregrineDummyPacketInterface]):
    """
    Wrapper around an EnhancedPayloadCollection of payloads that came from
    Peregrine.

    @author: Connor W. Colombo (CMU)
    @last-updated: 01/04/2024
    """
    __slots__: List[str] = []  # empty __slots__ preserves parent __slots__

    def __init__(self,
                 payloads: Optional[EnhancedPayloadCollection] = None,
                 raw: Optional[bytes] = None,
                 endianness_code: str = ENDIANNESS_CODE
                 ) -> None:

        if (payloads is None or payloads.is_empty()):
            # If both `payloads` and `raw` are `None`, what even caused
            # this to be generated?
            logger.debug(
                "A `PeregrineDummyPacket` was constructed with no `payloads`. "
                "This suggests it's being created from nothing as a completely "
                "empty packet. Was this intentional or is this a bug?"
            )

        if payloads is None:
            payloads = EnhancedPayloadCollection()

        super().__init__(
            payloads=payloads,
            raw=raw,
            endianness_code=endianness_code
        )  # passthru

    @classmethod
    def decode(cls,
               data: bytes,
               endianness_code: str = ENDIANNESS_CODE
               ) -> PeregrineDummyPacket:
        raise NotImplementedError(
            "Decoding a `PeregrineDummyPacket` is not meaningful since it's"
            "just a `payloads` wrapper and we never have it in a raw form."
        )

    def encode(self, **kwargs: Any) -> bytes:
        # There's no encoding or decoding to do. This just wraps `payloads`.
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
    ) -> PeregrineDummyPacket:
        """ Minimum packet is just the packet of payloads. """
        return cls(
            payloads=payloads,
            raw=raw,
            endianness_code=endianness_code
        )

    @classmethod
    def is_valid(cls, data: bytes, endianness_code: str = ENDIANNESS_CODE) -> bool:
        """This is never a valid way to decode raw data (has no `decode`
        implementation, since that's meaningless here)."""
        return False

    def __repr__(self) -> str:
        n = len([*self.payloads.all_payloads])
        return f"PeregrineDummyPacket: {n} payloads."

    def __str__(self) -> str:
        return self.__repr__()
