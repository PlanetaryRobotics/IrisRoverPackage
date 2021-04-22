#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""
Defines Common Data Required for Packets. Support for Building and Parsing
Packets.

@author: Connor W. Colombo (CMU)
@last-updated: 04/09/2020
"""
from __future__ import annotations  # Activate postponed annotations (for using classes as return type in their own methods)

from typing import List, Tuple, Any, Optional, Callable, Generic, TypeVar, cast
from abc import ABC, abstractmethod
from enum import Enum
import struct
import numpy as np
import time

from math import ceil

from .magic import Magic, MAGIC_SIZE
from .metadata import DataPathway, DataSource
from .container import ContainerCodec
from .payload import PayloadCollection, TelemetryPayload, extract_downlinked_payloads

from .settings import ENDIANNESS_CODE, settings
from .logging import logger
from .exceptions import PacketDecodingException

from IrisBackendv3.utils.basic import print_bytearray_hex as printraw

CPH_SIZE = 4

CT = TypeVar('CT')

# TODO: Add update hooks?

# TODO: Add `__str__` / `__repr__`s

#! TODO: Handle serialization (must replace container scheme, augment by storing payloads with their metadata)


class Packet(ContainerCodec[CT], ABC):
    # Mainly an aliasing class for now (allows for creating List[Packet])

    __slots__: List[str] = [
        'pathway',  # DataPathway through which this data was received or should be sent
        'source',  # DataSource of this data (how it entered the GSW)
        'payloads'  # PayloadCollection of all Payloads, separated by type
    ]  # empty but lets the slots from parent continue

    pathway: DataPathway
    source: DataSource
    payloads: PayloadCollection

    def __init__(self,
                 payloads: PayloadCollection,
                 pathway: DataPathway = DataPathway.NONE,
                 source: DataSource = DataSource.NONE,
                 raw: Optional[bytes] = None,
                 endianness_code: str = ENDIANNESS_CODE
                 ) -> None:
        self.payloads = payloads
        self.pathway = pathway
        self.source = source
        super().__init__(raw=raw, endianness_code=endianness_code)  # passthru

    @classmethod
    @abstractmethod
    def decode(cls,
               data: bytes,
               endianness_code: str = ENDIANNESS_CODE,
               pathway: DataPathway = DataPathway.NONE,
               source: DataSource = DataSource.NONE
               ) -> CT:
        raise NotImplementedError()

    @classmethod
    @abstractmethod
    def is_valid(cls, data: bytes, endianness_code: str = ENDIANNESS_CODE) -> bool:
        """
        Determines whether the given bytes constitute a valid packet of this type.
        """
        raise NotImplementedError()


class WatchdogTvacHeartbeatPacketInterface(Packet[CT]):

    class CustomPayload():
        """
        Core custom WatchdogTvacHeartbeat payload.
        Members must have same names as corresponding telemetry channels in the
        `WatchdogHeartbeatTvac` prebuilt module.

        *NOTE:* For this to work effectively, all fields, including computed
        properties, must match their names from the prebuilt module
        AND the order of the args in `__init__` must match the order of the
        bytes in the packet.
        """
        THERMISTOR_LOOKUP_TABLE = {  # for 5k thermistor: https://www.tdk-electronics.tdk.com/inf/50/db/ntc/NTC_Mini_sensors_S863.pdf
            'degC': np.asarray([-15, -10, -5, 0, 5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 65, 70, 75, 80, 85, 90, 95, 100, 105, 110, 115, 120, 125, 130, 135, 140, 145, 150, 155]),
            'Vadc': np.asarray([4242, 3970, 3670, 3352, 3023, 2695, 2378, 2077, 1801, 1552, 1330, 1137, 969, 825, 702, 598, 510, 435, 372, 319, 274, 237, 204, 177, 154, 134, 117, 103, 90, 79, 70, 62, 55, 49, 44])
        }

        __slots__: List[str] = [
            '_AdcTempRaw',
            '_ChargeRaw',
            '_VoltageRaw',
            '_CurrentRaw',
            '_FuelTempRaw',
            '_KpHeater',
            '_HeaterSetpoint',
            '_HeaterWindow',
            '_HeaterPwmLimit',
            '_WatchdogMode',
            '_HeaterStatus',
            '_HeatingControlEnabled',
            '_HeaterPwmDutyCycle'
        ]

        _AdcTempRaw: int
        _ChargeRaw: int
        _VoltageRaw: int
        _CurrentRaw: int
        _FuelTempRaw: int
        _KpHeater: int
        _HeaterSetpoint: int
        _HeaterWindow: int
        _HeaterPwmLimit: int
        _WatchdogMode: str
        _HeaterStatus: int
        _HeatingControlEnabled: int
        _HeaterPwmDutyCycle: int

        @property
        def AdcTempKelvin(self) -> float:
            return float(np.interp(self._AdcTempRaw, self.THERMISTOR_LOOKUP_TABLE['Vadc'][::-1], self.THERMISTOR_LOOKUP_TABLE['degC'][::-1]) + 273.15)

        @property
        def ChargeMah(self) -> float:
            return 0  # ! TODO: Get the conversion

        @property
        def Voltage(self) -> float:
            return self._VoltageRaw * 0.00108033875

        @property
        def CurrentAmps(self) -> float:
            return 0.0000390636921 * (self._CurrentRaw - 32767)

        @property
        def FuelTempKelvin(self) -> float:
            return 0  # ! TODO: Get the conversion

        @property
        def HeaterSetpointKelvin(self) -> float:
            return float(np.interp(self._HeaterSetpoint, self.THERMISTOR_LOOKUP_TABLE['Vadc'][::-1], self.THERMISTOR_LOOKUP_TABLE['degC'][::-1]) + 273.15)

        @property
        def HeaterWindowKelvin(self) -> float:
            upper = float(np.interp(self._HeaterSetpoint-self._HeaterWindow,
                          self.THERMISTOR_LOOKUP_TABLE['Vadc'][::-1], self.THERMISTOR_LOOKUP_TABLE['degC'][::-1]))
            lower = float(np.interp(self._HeaterSetpoint+self._HeaterWindow,
                          self.THERMISTOR_LOOKUP_TABLE['Vadc'][::-1], self.THERMISTOR_LOOKUP_TABLE['degC'][::-1]))
            return abs(upper-lower)/2

        # Make public get, private set to signal that you can freely use these values
        # but modifying them directly can yield undefined behavior (specifically
        # `raw` not syncing up with whatever other data is in the container)
        @property
        def AdcTempRaw(self) -> int: return self._AdcTempRaw
        @property
        def ChargeRaw(self) -> int: return self._ChargeRaw
        @property
        def VoltageRaw(self) -> int: return self._VoltageRaw
        @property
        def CurrentRaw(self) -> int: return self._CurrentRaw
        @property
        def FuelTempRaw(self) -> int: return self._FuelTempRaw
        @property
        def KpHeater(self) -> int: return self._KpHeater
        @property
        def HeaterSetpoint(self) -> int: return self._HeaterSetpoint
        @property
        def HeaterWindow(self) -> int: return self._HeaterWindow
        @property
        def HeaterPwmLimit(self) -> int: return self._HeaterPwmLimit
        @property
        def WatchdogMode(self) -> str: return self._WatchdogMode
        @property
        def HeaterStatus(self) -> int: return self._HeaterStatus

        @property
        def HeatingControlEnabled(self) -> int:
            return self._HeatingControlEnabled

        @property
        def HeaterPwmDutyCycle(self) -> int: return self._HeaterPwmDutyCycle

        def __init__(self,
                     AdcTempRaw: int,
                     ChargeRaw: int,
                     VoltageRaw: int,
                     CurrentRaw: int,
                     FuelTempRaw: int,
                     KpHeater: int,
                     HeaterSetpoint: int,
                     HeaterWindow: int,
                     HeaterPwmLimit: int,
                     WatchdogMode: str,
                     HeaterStatus: int,
                     HeatingControlEnabled: int,
                     HeaterPwmDutyCycle: int
                     ) -> None:
            self._AdcTempRaw = AdcTempRaw
            self._ChargeRaw = ChargeRaw
            self._VoltageRaw = VoltageRaw
            self._CurrentRaw = CurrentRaw
            self._FuelTempRaw = FuelTempRaw
            self._KpHeater = KpHeater
            self._HeaterSetpoint = HeaterSetpoint
            self._HeaterWindow = HeaterWindow
            self._HeaterPwmLimit = HeaterPwmLimit
            self._WatchdogMode = WatchdogMode
            self._HeaterStatus = HeaterStatus
            self._HeatingControlEnabled = HeatingControlEnabled
            self._HeaterPwmDutyCycle = HeaterPwmDutyCycle

        def __repr__(self) -> str:
            return (
                f"{self.WatchdogMode}:\t"
                f"[Heat: {'ON' if self.HeaterStatus else 'OFF'}, Ctrl: {'ON' if self.HeatingControlEnabled else 'OFF'}] \t"
                f"{self.AdcTempKelvin:.1f}°K -> "
                f"{self.HeaterSetpointKelvin:.1f}°K +- {self.HeaterWindowKelvin:.2f}K° \t"
                f"Kp = {self.KpHeater} @ Duty Cycle: {self.HeaterPwmDutyCycle}/{0xFFFF}"
            )

    __slots__: List[str] = [
        'custom_payload'
    ]
    custom_payload: CustomPayload


class WatchdogTvacHeartbeatPacket(WatchdogTvacHeartbeatPacketInterface[WatchdogTvacHeartbeatPacketInterface]):
    # ADC temp:
    # Charge:
    # Raw Voltage: (U16) * [voltage int reading] * 0.00108033875 = voltage [V]
    # 0.0000390636921 * ( [current int reading] - 32767 ) = current [A]
    # 0.00778210117 * [fuel gauge temp int reading] = temperature [K]
    # Properties (r-only class variables):
    START_FLAG: bytes = b'\xFF'  # Required start flag

    def __init__(self,
                 custom_payload: WatchdogTvacHeartbeatPacket.CustomPayload,
                 pathway: DataPathway = DataPathway.NONE,
                 source: DataSource = DataSource.NONE,
                 raw: Optional[bytes] = None,
                 endianness_code: str = ENDIANNESS_CODE
                 ) -> None:

        self.custom_payload = custom_payload

        # Autopopulate payloads based on name:
        payloads: PayloadCollection = PayloadCollection(
            CommandPayload=[],
            TelemetryPayload=[],
            EventPayload=[],
            FileBlockPayload=[]
        )

        try:
            module = settings['STANDARDS'].modules['WatchdogHeartbeatTvac']
        except KeyError:
            raise TypeError(
                "Attempted to parse a `WatchdogTvacHeartbeatPacket` which "
                "requires the `WatchdogHeartbeatTvac` special `prebuilt` "
                "module to be loaded into the standards but it can't be found."
            )
        for channel in module.telemetry.vals:
            payloads.TelemetryPayload.append(TelemetryPayload(
                module_id=module.ID,
                channel_id=channel.ID,
                data=getattr(custom_payload, channel.name),
                timestamp=int(time.time()),
                magic=Magic.TELEMETRY,
                pathway=pathway,
                source=source,
                endianness_code=endianness_code
            ))

        super().__init__(payloads=payloads, pathway=pathway, source=source,
                         raw=raw, endianness_code=endianness_code)

    def __repr__(self) -> str:
        return self.custom_payload.__repr__()

    @ classmethod
    def decode(cls,
               data: bytes,
               endianness_code: str = ENDIANNESS_CODE,
               pathway: DataPathway = DataPathway.NONE,
               source: DataSource = DataSource.NONE,
               ) -> WatchdogTvacHeartbeatPacket:
        flag, core_data = data[:1], data[1:]
        if cls.START_FLAG != flag:
            raise PacketDecodingException(
                data,
                "Start flag for `WatchdogTvacHeartbeatPacket` was invalid. "  # type: ignore
                f"Expected {cls.START_FLAG}, Got: {flag} ."
            )
        custom_payload = WatchdogTvacHeartbeatPacket.CustomPayload(
            *struct.unpack(endianness_code + '9H 3B H', core_data)
        )
        # Fix the Watchdog Mode:
        module = settings['STANDARDS'].modules['WatchdogHeartbeatTvac']
        custom_payload._WatchdogMode = cast(str, module.telemetry['WatchdogMode'].get_enum_name(
            cast(int, custom_payload.WatchdogMode)  # it comes in as an int
        ))
        return WatchdogTvacHeartbeatPacket(
            custom_payload=custom_payload,
            pathway=pathway,
            source=source,
            raw=data,
            endianness_code=endianness_code
        )

    def encode(self, **kwargs: Any) -> bytes:
        #! TODO
        raise NotImplementedError()

    @classmethod
    def is_valid(cls, data: bytes, endianness_code: str = ENDIANNESS_CODE) -> bool:
        """
        Determines whether the given bytes constitute a valid packet of this type.
        """
        right_start = len(data) > 0 and data[:1] == cls.START_FLAG
        right_length = len(data) == 24  # Bytes

        return right_start and right_length


# class WatchdogHeartbeatPacket(Packet[WatchdogHeartbeatPacket]):
#     # Properties (r-only class variables):
#     START_FLAG: bytes = b'\xFF'  # Required start flag
#     # Battery charge value in mAh when telemetry value is (0x00, 0xFF):
#     BATT_CHARGE_RANGE: Tuple[float, float] = (0, 3500)
#     # Battery draw current value in mA when telemetry value is (0x00, 0xFF):
#     BATT_CURRENT_RANGE: Tuple[float, float] = (0, 500)
#     # Battery temperature value in degC when telemetry value is (0x00, 0xFF):
#     BATT_TEMP_RANGE: Tuple[float, float] = (75, -12.31)

#     __slots__: List[str] = [
#         '_batt_charge',  # - Battery charge level [mAh]
#         '_heater_on',  # - Is heater on?
#         '_batt_current',  # - Battery current draw [mA]
#         '_battery_voltage_ok',  # - Is battery voltage in a safe range?
#         '_batt_temp'  # - Battery temperature reading [degC]
#     ]

#     _batt_charge: float
#     _heater_on: bool
#     _batt_current: float
#     _battery_voltage_ok: bool
#     _batt_temp: float

#     # Make public get, private set to signal that you can freely use these values
#     # but modifying them directly can yield undefined behavior (specifically
#     # `raw` not syncing up with whatever other data is in the container)
#     @property
#     def batt_charge(self) -> float: return self._batt_charge
#     @property
#     def heater_on(self) -> bool: return self._heater_on
#     @property
#     def batt_current(self) -> float: return self._batt_current
#     @property
#     def battery_voltage_ok(self) -> bool: return self._battery_voltage_ok
#     @property
#     def batt_temp(self) -> float: return self._batt_temp

#     def __init__(self,
#                  batt_charge: float,
#                  heater_on: bool,
#                  batt_current: float,
#                  battery_voltage_ok: bool,
#                  batt_temp: float,
#                  raw: Optional[bytes] = None,
#                  endianness_code: str = ENDIANNESS_CODE
#                  ) -> None:
#         self._batt_charge = batt_charge
#         self._heater_on = heater_on
#         self._batt_current = batt_current
#         self._battery_voltage_ok = battery_voltage_ok
#         self._batt_temp = batt_temp
#         super().__init__(raw=raw, endianness_code=endianness_code)

#     @classmethod
#     def decode(cls, data: bytes, endianness_code: str = ENDIANNESS_CODE) -> CT:
#         #! TODO
#         raise NotImplementedError()

#     def encode(self, **kwargs: Any) -> bytes:
#         #! TODO
#         raise NotImplementedError()

#     @classmethod
#     def is_valid(cls, data: bytes, endianness_code: str = ENDIANNESS_CODE) -> bool:
#         """
#         Determines whether the given bytes constitute a valid packet of this type.
#         """
#         right_start = len(data) > 0 and data[0] == cls.START_FLAG
#         right_length = len(data) == ceil((8 + 7 + 1 + 7 + 1 + 8) / 8)  # Bytes

#         return right_start and right_length


# class WatchdogCommandResponsePacket(Packet[WatchdogCommandResponsePacket]):
#     # Properties (r-only class variables):
#     START_FLAG: bytes = b'\x0A'  # Required start flag

#     #! TODO: Create an `install` function which adds WatchdogCommandResponsePacket events to standards

#     class ErrorFlag(Enum):
#         """
#         Known WatchdogCommandResponse Error Flags:
#         """
#         NO_ERROR = b'\x00'  # all okay
#         BAD_PACKET_LEN = b'\x01'
#         CHECKSUM_FAILED = b'\x02'
#         BAD_MODULE_ID = b'\x03'
#         BAD_COMMAND_ID = b'\x04'
#         BAD_COMMAND_PARAMETER = b'\x05'
#         BAD_COMMAND_ORDER = b'\x06'  # example deploy before prep for deploy

#     __slots__: List[str] = [
#         '_command_id',  # - ID of command being responded to
#         '_error_flag',  # - Error status after watchdog processed command
#     ]

#     _command_id: bytes
#     _error_flag: WatchdogCommandResponsePacket.ErrorFlag

#     # Make public get, private set to signal that you can freely use these values
#     # but modifying them directly can yield undefined behavior (specifically
#     # `raw` not syncing up with whatever other data is in the container)
#     @property
#     def command_id(self) -> bytes: return self._command_id

#     @property
#     def error_flag(self) -> WatchdogCommandResponsePacket.ErrorFlag:
#         return self._error_flag

#     def __init__(self,
#                  command_id: bytes,
#                  error_flag: WatchdogCommandResponsePacket.ErrorFlag,
#                  raw: Optional[bytes] = None,
#                  endianness_code: str = ENDIANNESS_CODE
#                  ) -> None:
#         self._command_id = command_id
#         self._error_flag = error_flag
#         super().__init__(raw=raw, endianness_code=endianness_code)

#     @classmethod
#     def decode(self, data: bytes, endianness_code: str = ENDIANNESS_CODE) -> CT:
#         #! TODO
#         raise NotImplementedError()

#     def encode(self, **kwargs: Any) -> bytes:
#         #! TODO
#         raise NotImplementedError()

#     @classmethod
#     def is_valid(cls, data: bytes, endianness_code=ENDIANNESS_CODE) -> bool:
#         right_start = len(data) > 0 and data[0] == cls.START_FLAG
#         right_length = len(data) == ceil((8 + 8 + 8) / 8)  # Bytes

#         return right_start and right_length
#         raise NotImplementedError()


class IrisCommonPacketInterface(Packet[CT]):
    pass


class IrisCommonPacket(IrisCommonPacketInterface[IrisCommonPacketInterface]):
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

    class CommonPacketHeaderInterface(ContainerCodec[CT]):
        pass

    class CommonPacketHeader(CommonPacketHeaderInterface[CommonPacketHeaderInterface]):
        """
        Container for Handling and Processing CommonPacketHeaders.

        All data is stored in the unpacked form since that's how it will most
        often be handled and transformed and only packed into bytes when needed.
        """

        SEQ_NUM_SYM: str = 'B'  # struct symbol string for en/decoding seq_num
        VLP_LEN_SYM: str = 'H'  # struct symbol string for en/decoding vlp_len
        # struct symbol string for encoding checksum (externally from int):
        CHECKSUM_SYM: str = 'B'

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
                     checksum: Optional[bytes] = None,
                     raw: Optional[bytes] = None,
                     endianness_code: str = ENDIANNESS_CODE
                     ) -> None:
            self._seq_num = seq_num
            self._vlp_len = vlp_len

            if checksum is None:
                # init as all zeros (before it's computed later):
                self._checksum = struct.pack(
                    endianness_code + IrisCommonPacket.CommonPacketHeader.CHECKSUM_SYM, 0x00)
            else:
                self._checksum = checksum

            super().__init__(raw=raw, endianness_code=endianness_code)

        def __str__(self) -> str:
            return f"CPH[{self.seq_num}]: {self.vlp_len}B"

        @classmethod
        def decode(cls,
                   data: bytes,
                   endianness_code: str = ENDIANNESS_CODE
                   ) -> IrisCommonPacket.CommonPacketHeader:
            """Extract all data in the given raw common packet header."""
            cph_head, checksum = data[:-1], data[-1:]

            sns = IrisCommonPacket.CommonPacketHeader.SEQ_NUM_SYM
            vls = IrisCommonPacket.CommonPacketHeader.VLP_LEN_SYM
            struct_str = endianness_code + ' ' + sns + '' + vls
            seq_num, vlp_len = struct.unpack(struct_str, cph_head)

            return cls(
                endianness_code=endianness_code,
                seq_num=seq_num,
                vlp_len=vlp_len,
                checksum=checksum,
                raw=data
            )

        def encode(self, **kwargs: Any) -> bytes:
            """Pack data into a bytes object."""

            sns = IrisCommonPacket.CommonPacketHeader.SEQ_NUM_SYM
            vls = IrisCommonPacket.CommonPacketHeader.VLP_LEN_SYM
            struct_str = self.endianness_code + ' ' + sns + '' + vls
            cph_head = struct.pack(struct_str, self.seq_num, self.vlp_len)

            self._raw = cph_head + self.checksum

            return self._raw

    __slots__: List[str] = [
        # CommonPacketHeader Data:
        '_common_packet_header'
    ]

    _common_packet_header: IrisCommonPacket.CommonPacketHeader

    # Make public get, private set to signal that you can freely use these values
    # but modifying them directly can yield undefined behavior (specifically
    # `raw` not syncing up with whatever other data is in the container)
    @property
    def common_packet_header(self) -> IrisCommonPacket.CommonPacketHeader:
        return self._common_packet_header

    @staticmethod
    def _count_vlp_len(payloads: PayloadCollection) -> int:
        """
        Calculates the VLP length necessary to send the given PayloadsCollection.
        Sum of all Payload sizes + MAGIC_SIZE * num_payloads
        """
        total_size = 0
        num_payloads = 0
        for c in payloads:
            c = cast(List, c)  # mypy doesn't get this by itself
            for p in c:
                total_size += len(p.encode())
                num_payloads += 1
        total_size += num_payloads * MAGIC_SIZE  # Each will require a magic
        return total_size

    def __init__(self,
                 payloads: PayloadCollection,
                 seq_num: int = 0,
                 common_packet_header: Optional[CommonPacketHeader] = None,
                 pathway: DataPathway = DataPathway.NONE,
                 source: DataSource = DataSource.NONE,
                 raw: Optional[bytes] = None,
                 endianness_code: str = ENDIANNESS_CODE
                 ) -> None:

        if common_packet_header is None:
            self._common_packet_header = IrisCommonPacket.CommonPacketHeader(
                seq_num=seq_num,
                vlp_len=IrisCommonPacket._count_vlp_len(payloads)
            )
        else:
            self._common_packet_header = common_packet_header
        super().__init__(payloads=payloads, pathway=pathway,
                         source=source, raw=raw, endianness_code=endianness_code)

    @classmethod
    def decode(cls,
               data: bytes,
               endianness_code: str = ENDIANNESS_CODE,
               pathway: DataPathway = DataPathway.NONE,
               source: DataSource = DataSource.NONE
               ) -> IrisCommonPacket:
        """Construct a Iris Packet Object from Bytes."""

        cph_data = data[:CPH_SIZE]
        CPH = IrisCommonPacket.CommonPacketHeader.decode(cph_data)
        #! TODO: Perform checksum check. (not impl. in FSW atm)
        actual_vlp_len = len(data) - CPH_SIZE
        if CPH.vlp_len != actual_vlp_len:
            raise PacketDecodingException(
                cph_data,
                (
                    f"CPH {CPH} was successfully decoded *but* the length it "
                    f"expects for the VLP({CPH.vlp_len}B) doesn't match the "
                    f"actual VLP length ({actual_vlp_len}B)."
                )
            )
        # Extract the Variable Length Payload
        VLP = data[CPH_SIZE:]

        # Parse VLP:
        try:
            #! TODO: handle uplink packets as well.
            #! TODO: Handle UplinkTimes/DownlinkTimes objects
            payloads = extract_downlinked_payloads(
                VLP=VLP,
                pathway=pathway,
                source=source
            )

        except Exception as e:
            trace = e  # traceback.format_exc()
            logger.warning(
                f"Had to abort packet parsing due to the following exception: {trace}"
            )

        return IrisCommonPacket(
            common_packet_header=CPH,
            payloads=payloads,
            pathway=pathway,
            source=source,
            raw=data,
            endianness_code=endianness_code
        )

    def encode(self, **kwargs: Any) -> bytes:
        # Lookup table that lists all payload types which should be encoded and
        # which Magics to use for them:
        encoded_payload_magics_lookup = {
            'CommandPayload': Magic.COMMAND.encode(),
            'TelemetryPayload': Magic.TELEMETRY.encode(),
            'EventPayload': Magic.EVENT.encode(),
            'FileBlockPayload': Magic.FILE.encode()
        }

        # Compile all payloads:
        VLP = b''
        for payload_type in encoded_payload_magics_lookup.keys():
            for cp in getattr(self.payloads, payload_type):
                VLP += encoded_payload_magics_lookup[payload_type] + cp.encode()

        CPH = self.common_packet_header.encode()
        assert self.common_packet_header.vlp_len == len(VLP)

        packet = CPH + VLP

        # Compute Checksum over entire packet (with checksum defaulted to 0):
        self.common_packet_header._checksum = struct.pack(
            self.endianness_code + IrisCommonPacket.CommonPacketHeader.CHECKSUM_SYM,
            ~np.uint8(sum(bytearray(packet)) % 256)
        )

        # Rebuild CPH with checksum:
        CPH = self.common_packet_header.encode()

        # Rebuild Packet with checksum:
        packet = CPH + VLP

        return packet

    @ classmethod
    def is_valid(cls, data: bytes, endianness_code: str = ENDIANNESS_CODE) -> bool:
        """
        Determines whether the given bytes constitute a valid packet of this type.
        """
        # CPH + 4B Magic + 1B data:
        min_length = len(data) > (CPH_SIZE + MAGIC_SIZE + 1)
        try:
            magic = Magic.decode(
                data[CPH_SIZE:CPH_SIZE+MAGIC_SIZE],
                byte_order=ENDIANNESS_CODE
            )
            contains_magic = magic != Magic.MISSING
        except (PacketDecodingException, IndexError):
            contains_magic = False

        return min_length and contains_magic


class IrisDownlinkPacket(IrisCommonPacket):
    """
    Defines Common Data Required for All Downlinked Packets (Moon to Earth).

    All the same core data as any other IrisPacket but with special additional
    metadata coming from

    @author: Connor W. Colombo (CMU)
    @last-updated: 12/25/2020
    """

    __slots__: List[str] = [
        'time_received',
        'time_sent'
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
        'time_issued',
        'time_logged',
        'time_processed',
        'time_sent',
        'time_confirmed',
        'time_received'
    ]

    def __init__(self) -> None:
        pass
