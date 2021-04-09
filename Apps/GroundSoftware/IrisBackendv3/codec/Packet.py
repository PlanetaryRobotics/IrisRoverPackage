#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""
Defines Common Data Required for Packets. Support for Building and Parsing
Packets.

@author: Connor W. Colombo (CMU)
@last-updated: 04/08/2020
"""
from __future__ import annotations  # Activate postponed annotations (for using classes as return type in their own methods)

from typing import List, Tuple, Any, Optional, Callable, Generic, TypeVar
from abc import ABC, abstractmethod
from enum import Enum
import struct

from math import ceil

from .magic import Magic, MAGIC_SIZE

from .logging import logger
from .exceptions import PacketDecodingException

ENDIANNESS_CODE = "<"  # little-endian by default
CPH_SIZE = 4
# *TODO: Make all these^ settable via cli args (opts?)

CT = TypeVar('CT')

# TODO: Add update hooks?

# TODO: Add `__str__` / `__repr__`s


class ContainerCodec(Generic[CT], ABC):
    """
    Core interface definition for all container classes whose data should be
    encoded/decoded to be passed in/out of the Transceiver layer.

    Also serializable (pickleable) as raw data.

    Note: often [CT] is the implementing subclass.
    """

    __slots__: List[str] = [
        # Python Struct Endianness Code (used for en/decoding):
        '_endianness_code',
        # Raw bytes from the Transceiver which were parsed:
        # Kept for forensics in case something goes wrong.
        # `None` if this Container is constructed and hasn't yet been encoded.
        # Also used for efficient serialization since all the data in this
        # object should be able to be reconstructed by just knowing the raw data
        # and the endianness.
        '_raw'
    ]

    _endianness_code: str
    _raw: Optional[bytes]

    def __init__(self,
                 raw: Optional[bytes] = None,
                 endianness_code: str = ENDIANNESS_CODE
                 ) -> None:
        self._raw = raw
        self._endianness_code = endianness_code

    # Make public get, private set to signal that you can freely use these values
    # but modifying them directly can yield undefined behavior (specifically
    # `raw` not syncing up with whatever other data is in the container)
    @property
    def endianness_code(self) -> str: return self._endianness_code
    @property
    def raw(self) -> Optional[bytes]: return self.raw

    @classmethod
    @abstractmethod
    def decode(self, data: bytes, endianness_code=ENDIANNESS_CODE) -> CT:
        """
        Decodes the given bytes buffer.
        """
        raise NotImplementedError()

    @abstractmethod
    def encode(self, **kwargs: Any) -> bytes:
        """
        Encodes the data contained in this instance using this class' formatting.
        (normally should just use instance's contained data and not require any
        kwargs). Just make sure all overriding methods still work if kwargs
        aren't supplied.
        """
        raise NotImplementedError()

    def __reduce__(self) -> Tuple[Callable, Tuple[bytes, str], ]:
        # *Don't* automatically re-encode to update fields in case some part of
        # algorithm is broken and received raw data is lost.
        # (The first point of storing the raw is to be able to forensically
        # reconstruct what was seen during mission, so use same inputs to get
        # the same outputs).
        # This also allows for easy re-evaluation of all telemetry by simply
        # adjusting the decode / parsers and deserializing again.
        if self._raw is None:
            self._raw = self.encode()

        # "Callable object" returned will be the decoding function:
        # If a subclassed object is reduced, it will call that subclass' `decode`
        # function (assuming it's been implemented).
        return (self.__class__.decode, (self._raw, self._endianness_code))


class Packet(ContainerCodec[CT], ABC):
    # Mainly an aliasing class for now (allows for creating List[Packet])

    __slots__: List[str] = []  # empty but lets the slots from parent continue

    def __init__(self,
                 raw: Optional[bytes] = None,
                 endianness_code: str = ENDIANNESS_CODE
                 ) -> None:
        super().__init__(raw=raw, endianness_code=endianness_code)  # passthru

    @staticmethod
    @abstractmethod
    def is_valid(data: bytes, endianness_code=ENDIANNESS_CODE):
        """
        Determines whether the given bytes constitute a valid packet of this type.
        """
        raise NotImplementedError()


class WatchdogHeartbeatPacket(Packet[WatchdogHeartbeatPacket]):
    # Properties (r-only class variables):
    START_FLAG: bytes = b'\xFF'  # Required start flag
    # Battery charge value in mAh when telemetry value is (0x00, 0xFF):
    BATT_CHARGE_RANGE: Tuple[float, float] = (0, 3500)
    # Battery draw current value in mA when telemetry value is (0x00, 0xFF):
    BATT_CURRENT_RANGE: Tuple[float, float] = (0, 500)
    # Battery temperature value in degC when telemetry value is (0x00, 0xFF):
    BATT_TEMP_RANGE: Tuple[float, float] = (75, -12.31)

    __slots__: List[str] = [
        '_batt_charge',  # - Battery charge level [mAh]
        '_heater_on',  # - Is heater on?
        '_batt_current',  # - Battery current draw [mA]
        '_battery_voltage_ok',  # - Is battery voltage in a safe range?
        '_batt_temp'  # - Battery temperature reading [degC]
    ]

    _batt_charge: float
    _heater_on: bool
    _batt_current: float
    _battery_voltage_ok: bool
    _batt_temp: float

    # Make public get, private set to signal that you can freely use these values
    # but modifying them directly can yield undefined behavior (specifically
    # `raw` not syncing up with whatever other data is in the container)
    @property
    def batt_charge(self) -> float: return self._batt_charge
    @property
    def heater_on(self) -> bool: return self._heater_on
    @property
    def batt_current(self) -> float: return self._batt_current
    @property
    def battery_voltage_ok(self) -> bool: return self._battery_voltage_ok
    @property
    def batt_temp(self) -> float: return self._batt_temp

    def __init__(self,
                 batt_charge: float,
                 heater_on: bool,
                 batt_current: float,
                 battery_voltage_ok: bool,
                 batt_temp: float,
                 raw: Optional[bytes] = None,
                 endianness_code: str = ENDIANNESS_CODE
                 ) -> None:
        self._batt_charge = batt_charge
        self._heater_on = heater_on
        self._batt_current = batt_current
        self._battery_voltage_ok = battery_voltage_ok
        self._batt_temp = batt_temp
        super().__init__(raw=raw, endianness_code=endianness_code)

    @abstractmethod
    def decode(self, data: bytes, endianness_code=ENDIANNESS_CODE) -> CT:
        #! TODO
        raise NotImplementedError()

    @abstractmethod
    def encode(self, **kwargs: Any) -> bytes:
        #! TODO
        raise NotImplementedError()

    @staticmethod
    def is_valid(data: bytes, endianness_code=ENDIANNESS_CODE):
        """
        Determines whether the given bytes constitute a valid packet of this type.
        """
        right_start = len(data) > 0 and data[0] == self.START_FLAG
        right_length = len(data) == ceil((8 + 7 + 1 + 7 + 1 + 8) / 8)  # Bytes

        return right_start and right_length


class WatchdogCommandResponsePacket(Packet[WatchdogCommandResponsePacket]):
    # Properties (r-only class variables):
    START_FLAG: bytes = b'\x0A'  # Required start flag

    #! TODO: Create an `install` function which adds WatchdogCommandResponsePacket events to standards

    class ErrorFlag(Enum):
        """
        Known WatchdogCommandResponse Error Flags:
        """
        NO_ERROR = b'\x00'  # all okay
        BAD_PACKET_LEN = b'\x01'
        CHECKSUM_FAILED = b'\x02'
        BAD_MODULE_ID = b'\x03'
        BAD_COMMAND_ID = b'\x04'
        BAD_COMMAND_PARAMETER = b'\x05'
        BAD_COMMAND_ORDER = b'\x06'  # example deploy before prep for deploy

    __slots__: List[str] = [
        '_command_id',  # - ID of command being responded to
        '_error_flag',  # - Error status after watchdog processed command
    ]

    _command_id: bytes
    _error_flag: WatchdogCommandResponsePacket.ErrorFlag

    # Make public get, private set to signal that you can freely use these values
    # but modifying them directly can yield undefined behavior (specifically
    # `raw` not syncing up with whatever other data is in the container)
    @property
    def command_id(self) -> bytes: return self._command_id

    @property
    def error_flag(self) -> WatchdogCommandResponsePacket.ErrorFlag:
        return self._error_flag

    def __init__(self,
                 command_id: bytes,
                 error_flag: WatchdogCommandResponsePacket.ErrorFlag,
                 raw: Optional[bytes] = None,
                 endianness_code: str = ENDIANNESS_CODE
                 ) -> None:
        self._command_id = command_id
        self._error_flag = error_flag
        super().__init__(raw=raw, endianness_code=endianness_code)

    @abstractmethod
    def decode(self, data: bytes, endianness_code=ENDIANNESS_CODE) -> CT:
        #! TODO
        raise NotImplementedError()

    @abstractmethod
    def encode(self, **kwargs: Any) -> bytes:
        #! TODO
        raise NotImplementedError()

    @staticmethod
    def is_valid(data: bytes, endianness_code=ENDIANNESS_CODE):
        right_start = len(data) > 0 and data[0] == self.START_FLAG
        right_length = len(data) == ceil((8 + 8 + 8) / 8)  # Bytes

        return right_start and right_length
        raise NotImplementedError()


class IrisCommonPacket(Packet[IrisCommonPacket]):
    """
    Defines Common Data Required for All Packets.

    Packets follow the standard defined in the IRIS Command & Telemetry List.

    All data is stored in the unpacked form since that's how it will most often
    be handled and transformed and only packed into bytes when needed.

    @author: Connor W. Colombo (CMU)
    @last-updated: 04/09/2020
    """

    # [Bytes] - Maximum Transmission Units for Packets thru Watchdog:
    MTU_WATCHDOG: int = 255
    # [Bytes] - Maximum Transmission Units for Packets thru Hercules:
    MTU_HERCULES: int = 1006

    class CommonPacketHeader(ContainerCodec[IrisCommonPacket.CommonPacketHeader]):
        """
        Container for Handling and Processing CommonPacketHeaders.

        All data is stored in the unpacked form since that's how it will most
        often be handled and transformed and only packed into bytes when needed.
        """

        __slots__: List[str] = [
            '_seq_num',  # - Sequence Number
            '_vlp_len',  # - Variable Length Payload (VLP) Length
            '_checksum'  # - Checksum of Entire Variable Length Payload (VLP)
        ]

        _seq_num: int
        _vlp_len: int
        _checksum: bytes

        # Make public get, private set to signal that you can freely use these values
        # but modifying them directly can yield undefined behavior (specifically
        # `raw` not syncing up with whatever other data is in the container)
        @property
        def seq_num(self) -> int: return self._seq_num
        @property
        def vlp_len(self) -> int: return self._vlp_len
        @property
        def checksum(self) -> bytes: return self._checksum

        def __init__(self,
                     seq_num: int,
                     vlp_len: int,
                     checksum: bytes,
                     raw: Optional[bytes] = None,
                     endianness_code: str = ENDIANNESS_CODE
                     ) -> None:
            self._seq_num = seq_num
            self._vlp_len = vlp_len
            self._checksum = checksum
            super().__init__(raw=raw, endianness_code=endianness_code)

        def __str__(self) -> str:
            return f"CPH[{self.seq_num}]: {self.vlp_len}B"

        @classmethod
        def decode(cls,
                   data: bytes,
                   endianness_code=ENDIANNESS_CODE
                   ) -> IrisPacket.CommonPacketHeader:
            """Extract all data in the given raw common packet header."""
            cph_head, checksum = data[:-1], data[-1:]
            seq_num, vlp_len = struct.unpack(endianness_code+' B H', cph_head)
            print(seq_num, vlp_len, checksum)

            return cls(
                endianness_code=endianness_code,
                seq_num=seq_num,
                vlp_len=vlp_len,
                checksum=checksum,
                raw=data
            )

        def encode(self, **kwargs: Any) -> bytes:
            """Pack data into a bytes object."""
            cph_head = struct.pack(
                self.endianness_code+' B H',
                self.seq_num, self.vlp_len
            )

            self.raw = cph_head + self.checksum

            return self.raw

    __slots__: List[str] = [
        # CommonPacketHeader Data:
        '_common_packet_header',
        # Collections of Payloads (Extracted or to be sent):
        '_command_payloads',
        '_telemetry_payloads',
        '_event_payloads'
    ]

    _common_packet_header: IrisCommonPacket.CommonPacketHeader
    _command_payloads: List[Payload]
    _telemetry_payloads: List[Payload]
    _event_payloads: List[Payload]

    # Make public get, private set to signal that you can freely use these values
    # but modifying them directly can yield undefined behavior (specifically
    # `raw` not syncing up with whatever other data is in the container)
    @property
    def common_packet_header(self) -> IrisCommonPacket.CommonPacketHeader:
        return self._common_packet_header

    @property
    def command_payloads(self) -> List[Payload]:
        return self._command_payloads

    @property
    def telemetry_payloads(self) -> List[Payload]:
        return self._telemetry_payloads

    @property
    def event_payloads(self) -> List[Payload]:
        return self._event_payloads

    def __init__(self,
                 common_packet_header: CommonPacketHeader,
                 command_payloads: List[Payload],
                 telemetry_payloads: List[Payload],
                 event_payloads: List[Payload],
                 raw: Optional[bytes] = None,
                 endianness_code: str = ENDIANNESS_CODE
                 ) -> None:
        self._common_packet_header = common_packet_header
        self._command_payloads = command_payloads
        self._telemetry_payloads = telemetry_payloads
        self._event_payloads = event_payloads
        super().__init__(raw=raw, endianness_code=endianness_code)

    @classmethod
    def decode(cls,
               data: bytes,
               endianness_code=ENDIANNESS_CODE
               ) -> IrisPacket.CommonPacketHeader:
        """Construct a Iris Packet Object from Bytes."""

        cph_data = packet_bytes[:CPH_SIZE]
        CPH = IrisPacket.CommonPacketHeader.decode(cph_data)
        #! TODO: Perform checksum check. (not impl. in FSW atm)
        actual_vlp_len = len(packet_bytes) - CPH_SIZE
        if CPH.vlp_len != actual_vlp_len:
            raise PacketDecodingException(
                cph_data,
                (
                    f"CPH {CPH} was successfully decoded *but* the length it "
                    f"expects for the VLP({CPH.vlp_len}B) doesn't match the "
                    f"actual VLP length ({actual_vlp_len}B)."
                )
            )

        # Unpack Variable Length Payload:
        while len(VLP) > 0:
            # vlp_data = [:] # (?) why
            # Strip off the magic:
            magic_bytes, VLP = VLP[:MAGIC_SIZE], VLP[MAGIC_SIZE:]
            magic = Magic.decode(magic_bytes, byte_order=ENDIANNESS_CODE)
            if magic == Magic.COMMAND or magic == Magic.WATCHDOG_COMMAND:
                pass
            elif magic == Magic.TELEMETRY:
                pass
            elif magic == Magic.EVENT:
                pass
            elif magic == Magic.FILE:
                pass
            # build payload from vlp
            # next...

        pass

    @staticmethod
    def is_valid(data: bytes, endianness_code=ENDIANNESS_CODE):
        """
        Determines whether the given bytes constitute a valid packet of this type.
        """
        # CPH + 4B Magic + 1B data:
        min_length = len(data) > (CPH_SIZE + MAGIC_SIZE + 1)
        try:
            Magic.decode(
                data[CPH_SIZE:CPH_SIZE+MAGIC_SIZE],
                byte_order=ENDIANNESS_CODE
            )
            contains_magic = True
        except (PacketDecodingException, IndexError):
            contains_magic = False

        return min_length and contains_magic


class IrisDownlinkPacket(IrisPacket):
    """
    Defines Common Data Required for All Downlinked Packets (Moon to Earth).

    All the same core data as any other IrisPacket but with special additional 
    metadata coming from 

    @author: Connor W. Colombo (CMU)
    @last-updated: 12/25/2020
    """

    __slots__: List[str] = [
        'time-received',
        'time-sent'
    ]

    def __init__(self) -> None:
        pass


class IrisUplinkPacket(Packet):
    """
    Defines Common Data Required for All Uplinked Packets (Earth to Moon).

    @author: Connor W. Colombo (CMU)
    @last-updated: 12/25/2020
    """

    __slots__: List[str] = [
        'time-issued',
        'time-logged',
        'time-processed',
        'time-sent',
        'time-confirmed',
        'time-received'
    ]

    def __init__(self) -> None:
        pass
