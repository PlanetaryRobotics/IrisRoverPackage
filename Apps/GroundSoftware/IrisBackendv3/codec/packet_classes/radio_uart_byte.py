"""
Defines `RadioUartBytePacket`, a `Packet` wrapper for a byte received by 
Hercules from the radio forwarded from the Radio in BGAPI Passthrough / 
Transparency Mode.
This packet type doesn't contain any telemetry (for now - eventually, depending on contents,
should get converted to a `EventPayload` or a `TelemetryPayload` containing radio state,
activity, etc.) and is just printed to the console.

@author: Connor W. Colombo (CMU)
@last-updated: 06/01/2023
"""
# Activate postponed annotations (for using classes as return type in their own methods)
from __future__ import annotations

from IrisBackendv3.codec.packet_classes.packet import Packet, CT
from .gds_packet_event_mixin import GdsPacketEventPacket, GDS_EVT_PT

from typing import Any, Final, Optional, List

from scapy.all import hexdump

from termcolor import colored

from IrisBackendv3.codec.payload_collection import EnhancedPayloadCollection

from IrisBackendv3.codec.settings import ENDIANNESS_CODE
from IrisBackendv3.codec.logs import logger

# Fixed prefix. All `RadioBgApiPacket`s start with b'DEBUG' then `BGB: `
# (BGapi Byte). Note the b'DEBUG' prefix is there b/c this is sent through
# the Hercules then through the Watchdog using the debug messaging system.
FIXED_PREFIX: bytes = b'DEBUGBGB: '


class RadioUartBytePacketInterface(GdsPacketEventPacket[GDS_EVT_PT]):
    # empty __slots__ preserves parent class __slots__
    __slots__: List[str] = []


class RadioUartBytePacket(RadioUartBytePacketInterface[RadioUartBytePacketInterface]):
    """
    Creates a data-structure to allow storing and handling a status message from
    the Radio.

    @author: Connor W. Colombo (CMU)
    @last-updated: 06/01/2023
    """
    __slots__: List[str] = ['_byte']

    _byte: Optional[int]

    def __init__(self,
                 payloads: Optional[EnhancedPayloadCollection] = None,
                 raw: Optional[bytes] = None,
                 endianness_code: str = ENDIANNESS_CODE
                 ) -> None:

        if raw is None and (payloads is None or payloads.is_empty()):
            # If both `payloads` and `raw` are `None`, what even caused
            # this to be generated?
            logger.debug(
                "A `RadioUartBytePacket` was constructed with no `payloads` and "
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

        if self._raw is None:
            self._raw = self.encode()

        # Strip off fixed prefix:
        data = self._raw[len(FIXED_PREFIX):]
        # Grab only the byte data (excluding termination):
        data = data[:4]

        # Decode BGMsg:
        try:
            self._byte = int(data, 16)
        except Exception as e:
            logger.warning(
                f"Failed to decode UART/BGAPI byte inside `RadioUartBytePacket` "
                f"with data `{hexdump(raw, dump=True)}` because: {e}."
            )
            self._byte = None

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
    ) -> RadioUartBytePacket:
        """ Minimum packet is just the packet. """
        return cls(
            payloads=payloads,
            raw=raw,
            endianness_code=endianness_code
        )

    @classmethod
    def is_valid(cls, data: bytes, endianness_code: str = ENDIANNESS_CODE) -> bool:
        """Valid if the packet starts with `FIXED_PREFIX`."""
        return data[:len(FIXED_PREFIX)] == FIXED_PREFIX

    def __repr__(self) -> str:
        if self._raw is None:
            l = 0
            data = b''
        else:
            l = len(self._raw) - len(FIXED_PREFIX)
            data = self._raw

        if self._byte is None:
            msg = colored(f'!! BYTE DECODING FAILED !! {data!r}', 'red')
        else:
            msg = colored(f'0x{self._byte:02X}', 'grey')

        return f"Radio-UART Byte: {msg}."

    def __str__(self) -> str:
        return self.__repr__()
