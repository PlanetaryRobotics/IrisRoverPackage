"""
Defines `HerculesRadioUplinkAckPacket`, a `Packet` sent by Hercules at the
Radio-driver level as soon as it receives uplinked data from the Radio.
Lets us know that the data got there, even if it was corrupted later.

@author: Connor W. Colombo (CMU)
@last-updated: 06/22/2023
"""
# Activate postponed annotations (for using classes as return type in their own methods)
from __future__ import annotations

from .gds_packet_event_mixin import GdsPacketEventPacket, GDS_EVT_PT
from IrisBackendv3.codec.packet_classes.packet import Packet, CT

from typing import List, Any, Final, Optional

from IrisBackendv3.codec.payload_collection import EnhancedPayloadCollection

from IrisBackendv3.codec.settings import ENDIANNESS_CODE
from IrisBackendv3.codec.logs import logger


class HerculesRadioUplinkAckPacketInterface(GdsPacketEventPacket[GDS_EVT_PT]):
    # empty __slots__ preserves parent class __slots__
    __slots__: List[str] = []


class HerculesRadioUplinkAckPacket(HerculesRadioUplinkAckPacketInterface[HerculesRadioUplinkAckPacketInterface]):
    """
    `Packet` sent by Hercules at the Radio-driver level as soon as it receives
    uplinked data from the Radio. Lets us know that the data got there, even
    if it was corrupted later.

    @author: Connor W. Colombo (CMU)
    @last-updated: 06/22/2023
    """
    _FIXED_PACKET_HEADER: Final[bytes] = b'DEBUGRADIO UPL: '

    __slots__: List[str] = ['_bgmsg']
    _ack_data: bytes

    @property
    def ack_data(self) -> bytes:
        return self._ack_data

    def __init__(
        self,
        payloads: Optional[EnhancedPayloadCollection] = None,
        raw: Optional[bytes] = None,
        endianness_code: str = ENDIANNESS_CODE
    ) -> None:

        if raw is None and (payloads is None or payloads.is_empty()):
            # If both `payloads` and `raw` are `None`, what even caused
            # this to be generated?
            logger.debug(
                "A `HerculesRadioUplinkAckPacket`"
                "was constructed with no `payloads` and no `raw` data. "
                "This suggests it's being created from "
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

        if self._raw is None:
            self._raw = self.encode()

        # Strip off fixed prefix:
        self._ack_data = self._raw[len(self._FIXED_PACKET_HEADER):].strip()

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
    ) -> HerculesRadioUplinkAckPacket:
        """ Minimum packet is just the packet. """
        return cls(
            payloads=payloads,
            raw=raw,
            endianness_code=endianness_code
        )

    @classmethod
    def is_valid(cls, data: bytes, endianness_code: str = ENDIANNESS_CODE) -> bool:
        """Valid if the packet starts with the fixed packet header."""
        return data[:len(cls._FIXED_PACKET_HEADER)].upper() == cls._FIXED_PACKET_HEADER

    def __repr__(self) -> str:
        if self._raw is None:
            l = 0
            data = b''
        else:
            l = len(self._ack_data)
            data = self._ack_data
        return f"HerculesRadioUplinkAckPacket[{l}B]: {data!r} "

    def __str__(self) -> str:
        ack_data = b'' if self._raw is None else self._ack_data
        # Format ACK bytes:
        ack_data_str = "0x" + ':'.join([f'{x:02X}' for x in ack_data])

        return f"HERC-RADIO-UPL-ACK: {ack_data_str}"
