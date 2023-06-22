"""
Defines `WatchdogResetSpecificAckPacket`, a `Packet` wrapper for a debug string
acknowledging a reset specific command and providing info about what occurred.
This packet type doesn't contain any telemetry (for now, should get converted
to a `EventPayload` eventually) and is just printed to the console.

@author: Connor W. Colombo (CMU)
@last-updated: 06/01/2023
"""
# Activate postponed annotations (for using classes as return type in their own methods)
from __future__ import annotations

from .gds_packet_event_mixin import GdsPacketEventPacket, GDS_EVT_PT
from IrisBackendv3.codec.packet_classes.packet import Packet, CT

from typing import List, Any, Optional

from scapy.utils import hexdump  # type: ignore
from termcolor import colored

from IrisBackendv3.codec.payload_collection import EnhancedPayloadCollection

from IrisBackendv3.codec.settings import ENDIANNESS_CODE
from IrisBackendv3.codec.settings import settings as CodecSettings
from IrisBackendv3.codec.logs import logger
from IrisBackendv3.codec.exceptions import PacketDecodingException

# Fixed prefix. All `RadioBgApiPacket`s start with b'DEBUG' then `RESET:`.
# Note the b'DEBUG' prefix is there b/c this is sent from the Watchdog using
# the debug messaging system.
FIXED_PREFIX: bytes = b'DEBUGRESET:'


class WatchdogResetSpecificAckPacketInterface(GdsPacketEventPacket[GDS_EVT_PT]):
    # empty __slots__ preserves parent class __slots__
    __slots__: List[str] = []


class WatchdogResetSpecificAckPacket(WatchdogResetSpecificAckPacketInterface[WatchdogResetSpecificAckPacketInterface]):
    """
    Creates a data-structure to allow storing and handling a reset specific ack.

    @author: Connor W. Colombo (CMU)
    @last-updated: 06/01/2023
    """
    __slots__: List[str] = [
        '_resetId',
        '_resultStatusCode',
        '_allowPowerOn',
        '_allowDisableRS422',
        '_allowDeploy',
        '_allowUndeploy'
    ]

    _resetId: int
    _resultStatusCode: int
    _allowPowerOn: bool
    _allowDisableRS422: bool
    _allowDeploy: bool
    _allowUndeploy: bool

    def __init__(self,
                 payloads: Optional[EnhancedPayloadCollection] = None,
                 raw: Optional[bytes] = None,
                 endianness_code: str = ENDIANNESS_CODE
                 ) -> None:

        if raw is None and (payloads is None or payloads.is_empty()):
            # If both `payloads` and `raw` are `None`, what even caused
            # this to be generated?
            logger.debug(
                "A `WatchdogResetSpecificAckPacket` was constructed with no "
                "`payloads` and no `raw` data. This suggests it's being "
                "created from nothing as a completely empty packet. Was this "
                "intentional or is this a bug?"
            )

        if payloads is None:
            # Except possibly in future subclasses, this should normally be
            # empty for an `WatchdogResetSpecificAckPacket`. `payloads` needs
            # to stay as an `__init__` arg to avoid violating the Liskov
            # substitution principle.
            payloads = EnhancedPayloadCollection()

        super().__init__(
            payloads=payloads,
            raw=raw,
            endianness_code=endianness_code
        )  # passthru

        if self._raw is None:
            self._raw = self.encode()

        # Strip off fixed prefix:
        core_data = self._raw[len(FIXED_PREFIX):]
        # Unpack fields from format: "RESET:%d -> %d with 0x%x\n"
        fields = core_data.strip().split(b' ')
        bad_formatting = False
        if len(fields) != 5:
            bad_formatting = True
        else:
            try:
                # Formatting is correct. Extract data:
                self._resetId = int(fields[0])
                self._resultStatusCode = int(fields[2])
                resetConditions = int(fields[-1], 16)
                self._allowPowerOn = bool(resetConditions & 0b1000)
                self._allowDisableRS422 = bool(resetConditions & 0b0100)
                self._allowDeploy = bool(resetConditions & 0b0010)
                self._allowUndeploy = bool(resetConditions & 0b0001)
            except Exception as e:
                bad_formatting = True
        if bad_formatting:
            # Raise an exception (so this will become an `UnsupportedPacket`):
            raise PacketDecodingException(
                self._raw,
                (
                    f"Failed to decode Reset Specific message inside "
                    f"`WatchdogResetSpecificAckPacket` with data "
                    f"`{hexdump(raw, dump=True)}` because the formatting did "
                    f"not match expectations."
                )
            )

    @property
    def resetFieldName(self) -> str:
        if self._resetId == 0:
            return 'NONE'
        elif not self._resetId:
            return 'BAD'
        else:
            try:
                module = CodecSettings['STANDARDS'].modules['WatchDogInterface']
                command = module.commands['WatchDogInterface_ResetSpecific']
                arg = command.args[0]
                name = arg.get_enum_name(self._resetId)
                return str(name)
            except Exception:
                return f'NOT-FOUND ({self._resetId})'

    @property
    def resetResult(self) -> str:
        if self._resultStatusCode == 0:
            return 'NO_ERROR'
        elif not self._resultStatusCode:
            return 'BAD'
        else:
            try:
                module = CodecSettings['STANDARDS'].modules['WatchdogCommandResponse']
                channel = module.telemetry['ErrorFlag']
                name = channel.get_enum_name(self._resultStatusCode)
                return str(name)
            except Exception:
                return f'NOT-FOUND ({self._resultStatusCode})'

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
    ) -> WatchdogResetSpecificAckPacket:
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
        if self._raw is None or len(self._raw) < len(FIXED_PREFIX):
            msg = ''
        else:
            msg = self._raw.decode().strip()
        return (
            colored(f" {msg} ", 'white', 'on_yellow', ['bold']) +
            colored(f": {self.resetFieldName} -> {self.resetResult}", 'yellow', attrs=['bold']) +
            colored(
                f", allowing\t "
                f"(PowerOn: {'1' if self._allowPowerOn else '0'},\t "
                f"Rs422Off: {'1' if self._allowDisableRS422 else '0'},\t "
                f"Deploy: {'1' if self._allowDeploy else '0'},\t "
                f"Undeploy: {'1' if self._allowUndeploy else '0'}).",
                'white'
            )
        )

    def __str__(self) -> str:
        return self.__repr__()
