"""
Defines `RadioDownlinkFlushPacket`, a `Packet` used to flush the Radio downlink
interface when downlinking large packets.

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


class RadioDownlinkFlushPacketInterface(GdsPacketEventPacket[GDS_EVT_PT]):
    # empty __slots__ preserves parent class __slots__
    __slots__: List[str] = []


class RadioDownlinkFlushPacket(RadioDownlinkFlushPacketInterface[RadioDownlinkFlushPacketInterface]):
    """
    Creates a data-structure to allow storing and handling a debug string from
    the Radio (or Hercules' Radio process).

    @author: Connor W. Colombo (CMU)
    @last-updated: 06/01/2023
    """
    __slots__: List[str] = []  # empty __slots__ preserves parent __slots__

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
    ) -> RadioDownlinkFlushPacket:
        """ Minimum packet is just the packet. """
        return cls(
            payloads=payloads,
            raw=raw,
            endianness_code=endianness_code
        )

    @classmethod
    def is_valid(cls, data: bytes, endianness_code: str = ENDIANNESS_CODE) -> bool:
        """Valid if the packet starts with `b'DL-FL'`."""
        return data[:5].upper() == b'DL-FL'

    def __repr__(self) -> str:
        if self._raw is None:
            l = 0
            data = b''
        else:
            l = len(self._raw)
            data = self._raw
        return f"RadioDownlinkFlushPacket[{l}B]: {data!r} "

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

        return f"DL-FL: {formatted_data}"
