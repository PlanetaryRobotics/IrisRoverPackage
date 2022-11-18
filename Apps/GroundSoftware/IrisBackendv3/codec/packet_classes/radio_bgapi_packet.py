"""
Defines `RadioBgApiPacket`, a `Packet` wrapper for a BGAPI packet forwarded
from the Radio in BGAPI Passthrough / Transparency Mode.
This packet type doesn't contain any telemetry (for now - eventually, depending on contents,
should get converted to a `EventPayload` or a `TelemetryPayload` containing radio state,
activity, etc.) and is just printed to the console.

@author: Connor W. Colombo (CMU)
@last-updated: 10/26/2022
"""
from __future__ import annotations

# Activate postponed annotations (for using classes as return type in their own methods)
from prompt_toolkit import formatted_text

from .packet import Packet, CT

from typing import Any, Final, Optional, List

from scapy.utils import hexdump  # type: ignore
from termcolor import colored

from ..payload_collection import EnhancedPayloadCollection

from ..settings import ENDIANNESS_CODE
from ..logging import logger


import IrisBackendv3.codec.bgapi as bgapi

# Fixed prefix. All `RadioBgApiPacket`s start with b'DEBUG' then `BGP:`
# (BGapi Packet). Note the b'DEBUG' prefix is there b/c this is sent through
# the Hercules then through the Watchdog using the debug messaging system.
FIXED_PREFIX: bytes = b'DEBUGBGP:'


class RadioBgApiPacketInterface(Packet[CT]):
    # empty __slots__ preserves parent class __slots__
    __slots__: List[str] = []


class RadioBgApiPacket(RadioBgApiPacketInterface[RadioBgApiPacketInterface]):
    """
    Creates a data-structure to allow storing and handling a status message from
    the Radio.

    @author: Connor W. Colombo (CMU)
    @last-updated: 10/26/2022
    """
    __slots__: List[str] = ['_bgmsg']

    _bgmsg: Optional[bgapi.BGMsg]

    def getMessage(self) -> Optional[bgapi.BGMsg]:
        return self._bgmsg

    def __init__(self,
                 payloads: Optional[EnhancedPayloadCollection] = None,
                 raw: Optional[bytes] = None,
                 endianness_code: str = ENDIANNESS_CODE
                 ) -> None:

        if raw is None and (payloads is None or payloads.is_empty()):
            # If both `payloads` and `raw` are `None`, what even caused
            # this to be generated?
            logger.debug(
                "A `RadioBgApiPacket` was constructed with no `payloads` and "
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
        # Decode BGMsg:
        try:
            self._bgmsg = bgapi.decode(
                bgapi.BGAPI_WIFI_DECODER, data, fromHost=False
            )
        except Exception as e:
            logger.warning(
                f"Failed to decode BGAPI message inside `RadioBgApiPacket` "
                f"with data `{hexdump(raw, dump=True)}` because: {e}."
            )
            self._bgmsg = None

    @classmethod
    def decode(cls,
               data: bytes,
               endianness_code: str = ENDIANNESS_CODE
               ) -> RadioBgApiPacket:
        return cls(raw=data, endianness_code=endianness_code)

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
    ) -> RadioBgApiPacket:
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

        if self._bgmsg is None:
            msg = colored(f'!! BGAPI DECODING FAILED !! {data!r}', 'red')
        else:
            msg = colored(str(self._bgmsg), 'magenta')

        prefix = colored(f" BGAPI[{l}B] ", 'white', 'on_magenta', ['bold'])
        return f"{prefix} {msg}"

    def __str__(self) -> str:
        return self.__repr__()
