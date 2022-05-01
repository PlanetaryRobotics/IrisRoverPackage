"""
Implementation of Watchdog Heartbeat Packet and its Interface.
This is a larger version of the normal Watchdog Heartbeat packet that was used
for statusing during TVAC and is occassionally still used for debugging.

@author: Connor W. Colombo (CMU)
@last-updated: 05/01/2022
"""
from __future__ import annotations  # Activate postponed annotations (for using classes as return type in their own methods)

from .packet import CT
from .custom_payload import CustomPayloadPacket, CPCT

from typing import List, Any, Optional, ClassVar, cast, Union, Type

import struct
import numpy as np  # type: ignore

from ..payload import PayloadCollection

from ..settings import ENDIANNESS_CODE
from ..exceptions import PacketDecodingException

from IrisBackendv3.data_standards.module import Module


class WatchdogTvacHeartbeatPacketInterface(CustomPayloadPacket[CT, CPCT]):
    # Name of the corresponding prebuilt `Module` used for mapping this packet's
    # data to telemetry streams:
    PREBUILT_MODULE_NAME: ClassVar[str] = 'WatchdogHeartbeatTvac'

    # Empty __slots__ allows super's __slots__ to not turn into __dict__:
    __slots__: List[str] = []

    class CustomPayload():
        """
        Core custom WatchdogTvacHeartbeat payload.
        Members must have same names as corresponding telemetry channels in the
        `WatchdogHeartbeatTvac` prebuilt module.
        Note: an error will get thrown by `WatchdogTvacHeartbeatPacket.__init__`
        when building `payloads` from the `custom_payload` if all the channels
        in the `WatchdogHeartbeatTvac` prebuilt module don't have a corresponding
        attr in this `CustomPayload`.

        *NOTE:* For this to work effectively, all fields, including computed
        properties, must match their names from the prebuilt module
        AND the order of the args in `__init__` must match the order of the
        bytes in the packet. [SEE THE NOTE IN `CustomPayloadPacket` FOR MORE
        DETAILS]
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
        _WatchdogMode: int
        _HeaterStatus: int
        _HeatingControlEnabled: int
        _HeaterPwmDutyCycle: int

        @property
        def _module(self) -> Module:
            return WatchdogTvacHeartbeatPacketInterface.get_ds_module()

        @property
        def AdcTempKelvin(self) -> float:
            return float(np.interp(self._AdcTempRaw, self.THERMISTOR_LOOKUP_TABLE['Vadc'][::-1], self.THERMISTOR_LOOKUP_TABLE['degC'][::-1]) + 273.15)

        @property
        def ChargeMah(self) -> float:
            return 0  # Not implemented in TVAC Heartbeats (conversion unknown)

        @property
        def Voltage(self) -> float:
            return self._VoltageRaw * 0.00108033875

        @property
        def CurrentAmps(self) -> float:
            return 0.0000390636921 * (self._CurrentRaw - 32767)

        @property
        def FuelTempKelvin(self) -> float:
            return 0.00778210117 * self._FuelTempRaw

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
        def HeaterStatus(self) -> int: return self._HeaterStatus

        @property
        def WatchdogMode(self) -> int: return self._WatchdogMode

        @property
        def WatchdogModeName(self) -> str:
            return cast(str, self._module.telemetry['WatchdogMode'].get_enum_name(
                self.WatchdogMode
            ))

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
                     WatchdogMode: Union[int, str],
                     HeaterStatus: int,
                     HeatingControlEnabled: int,
                     HeaterPwmDutyCycle: int
                     ) -> None:
            self._AdcTempRaw = AdcTempRaw
            self._ChargeRaw = struct.unpack(
                '>H', struct.pack('<H', ChargeRaw))[0]
            self._VoltageRaw = struct.unpack(
                '>H', struct.pack('<H', VoltageRaw))[0]
            self._CurrentRaw = struct.unpack(
                '>H', struct.pack('<H', CurrentRaw))[0]
            self._FuelTempRaw = struct.unpack(
                '>H', struct.pack('<H', FuelTempRaw))[0]
            self._KpHeater = KpHeater
            self._HeaterSetpoint = HeaterSetpoint
            self._HeaterWindow = HeaterWindow
            self._HeaterPwmLimit = HeaterPwmLimit
            self._HeaterStatus = HeaterStatus
            self._HeatingControlEnabled = HeatingControlEnabled
            self._HeaterPwmDutyCycle = HeaterPwmDutyCycle

            if isinstance(WatchdogMode, int):
                self._WatchdogMode = WatchdogMode
            else:
                self._WatchdogMode = cast(int, self._module.telemetry['WatchdogMode'].get_enum_value(
                    cast(str, WatchdogMode)
                ))

        def __repr__(self) -> str:
            return (
                f"{self.WatchdogModeName}:\t"
                f"[Heat: {'ON' if self.HeaterStatus else 'OFF'}, Ctrl: {'ON' if self.HeatingControlEnabled else 'OFF'}] \t"
                f"{self.AdcTempKelvin:.1f}°K -> "
                f"{self.HeaterSetpointKelvin:.1f}°K +- {self.HeaterWindowKelvin:.2f}K° \t"
                f"Kp = {self.KpHeater} @ Duty Cycle: {self.HeaterPwmDutyCycle}/{0xFFFF}"
            )


# Some useful type alias that make the subsequent class more readable:
WTHB_PI = WatchdogTvacHeartbeatPacketInterface
WTHB_CP = WTHB_PI.CustomPayload


class WatchdogTvacHeartbeatPacket(WTHB_PI[WTHB_PI, WTHB_CP]):
    # Properties (r-only class variables):
    START_FLAG: bytes = b'\xFF'  # Required start flag
    # Specify the `CUSTOM_PAYLOAD_CLASS` used by `CustomPayloadPacket` superclass (weird type signature is used to match the `Optional[Type[CPCT]]` used in the superclass):
    CUSTOM_PAYLOAD_CLASS: Optional[Type[WTHB_CP]] = WTHB_CP

    # Empty __slots__ allows super's __slots__ to not turn into __dict__:
    __slots__: List[str] = []

    def __repr__(self) -> str:
        return self.custom_payload.__repr__()

    @ classmethod
    def decode(cls,
               data: bytes,
               endianness_code: str = ENDIANNESS_CODE
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
        return WatchdogTvacHeartbeatPacket(
            custom_payload=custom_payload,
            raw=data,
            endianness_code=endianness_code
        )

    def encode(self, **kwargs: Any) -> bytes:
        #! TODO (not really a typical use case so not super necessary besides for completeness)
        #!! TODO: IS NECESSARY FOR IPC (OR JUST ENCODE THAT STUFF IN A STATE) <- Not with new `Packet`-specific `__reduce__` strategy
        raise NotImplementedError()

    @classmethod
    def is_valid(cls, data: bytes, endianness_code: str = ENDIANNESS_CODE) -> bool:
        """
        Determines whether the given bytes constitute a valid packet of this type.
        """
        right_start = len(data) > 0 and data[:1] == cls.START_FLAG
        right_length = len(data) == 24  # Bytes

        return right_start and right_length

    @classmethod
    def build_minimum_packet(
        cls,
        payloads: PayloadCollection,
        raw: Optional[bytes],
        endianness_code: str
    ) -> WatchdogTvacHeartbeatPacket:
        """ Builds a minimum representation of this Packet (before any
        additional elements from `__getstate__` are added back). Used by
        `Packet.__reduce__` for unpacking serialized data.
        """
        return cls(
            payloads=payloads,
            raw=raw,
            endianness_code=endianness_code
        )
