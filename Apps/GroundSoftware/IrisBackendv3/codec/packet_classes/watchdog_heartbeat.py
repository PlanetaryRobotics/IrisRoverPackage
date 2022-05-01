"""
Implementation of Watchdog TVAC Heartbeat Packet and its Interface.
This is used as a dense and minimally sized way to communicate the most
important Watchdog statuses during cis-lunar transit.

@author: Connor W. Colombo (CMU)
@last-updated: 05/01/2022
"""
from __future__ import annotations  # Activate postponed annotations (for using classes as return type in their own methods)

from .packet import CT
from .custom_payload import CustomPayloadPacket, CPCT

from typing import List, Any, Optional, ClassVar, Type

import bitstruct  # type: ignore

from ..payload import PayloadCollection

from ..settings import ENDIANNESS_CODE
from ..exceptions import PacketDecodingException

from IrisBackendv3.data_standards.module import Module


class WatchdogHeartbeatPacketInterface(CustomPayloadPacket[CT, CPCT]):
    # Name of the corresponding prebuilt `Module` used for mapping this packet's
    # data to telemetry streams:
    PREBUILT_MODULE_NAME: ClassVar[str] = 'WatchdogHeartbeat'

    # Empty __slots__ allows super's __slots__ to not turn into __dict__:
    __slots__: List[str] = []

    class CustomPayload():
        """
        Core custom WatchdogHeartbeatPacket payload.
        Members must have same names as corresponding telemetry channels in the
        `WatchdogHeartbeat` prebuilt module.
        Note: an error will get thrown by `WatchdogHeartbeatPacket.__init__`
        when building `payloads` from the `custom_payload` if all the channels
        in the `WatchdogHeartbeat` prebuilt module don't have a corresponding
        attr in this `CustomPayload`.

        *NOTE:* For this to work effectively, all fields, including computed
        properties, must match their names from the prebuilt module
        AND the order of the args in `__init__` must match the order of the
        bytes in the packet. [SEE THE NOTE IN `CustomPayloadPacket` FOR MORE
        DETAILS]
        """

        __slots__: List[str] = [
            '_ChargeRaw',
            '_HeaterStatus',
            '_CurrentRaw',
            '_BatteryVoltageOk',
            '_BattAdcTempRaw'
        ]

        _ChargeRaw: int
        _HeaterStatus: bool
        _CurrentRaw: int
        _BatteryVoltageOk: bool
        _BattAdcTempRaw: int

        @ staticmethod
        def despan(span: int,
                   num_bits: int,
                   min_val: float,
                   max_val: float,
                   span_min: int = 0,
                   span_max: Optional[int] = None
                   ) -> float:
            """
            Converts a given `span` value can span from 0 to 2**`num_bits`
            back to its original value in its original range from `min_val` to
            `max_val`.
            This is just basic linear interpolation over a given bit range.
            Data is transmitted as a `span` value to maximize the number of
            useful values which can occupy a given number of bits.

            Optionally, `span_min` and `span_max` can be given which will limit
            the lower and upper bound value for `span` used in the conversion
            process. `span` values below `span_min` will plateau and correspond to
            `min_val` and `span` values above `span_max` will plateau and
            correspond to `max_val`. This is necessary since, if floating point
            arithmetic is to be avoided in the FSW, it's not always possible to
            map a value from its original range to an exact power of 2.

            **Note:** `max_val` is the value which makes `span` take its highest
            value (2^n-1) and `min_val` is the value which makes `span` 0.
            If the range is inverted (say, 0xFF corresponds to 0 and 0x00
            corresponds to 100), then `max_val` will be smaller than `min_val`.
            """
            span_ub = (2 << (num_bits-1)) - 1  # upper bound of `span` (2^n-1)
            if span_max is None:
                span_max = span_ub
            elif span_max > span_ub:
                raise ValueError(
                    f"Given `span_max` cannot exceed 2**`num_bits`-1={span_ub}. "
                    f"{span_max} was given."
                )

            if span_min < 0:
                raise ValueError(
                    f"Given `span_min` cannot be <0. "
                    f"{span_min} was given."
                )

            # Keep `span` in bounds (and cause it to plateau if out of bounds):
            span = span if span < span_max else span_max
            span = span if span > span_min else span_min

            return (span-span_min)/(span_max-span_min) * (max_val-min_val) + min_val

        @ staticmethod
        def span(val: float,
                 num_bits: int,
                 min_val: float,
                 max_val: float,
                 span_min: int = 0,
                 span_max: Optional[int] = None
                 ) -> float:
            """
            Converts a given value, `val`, which ranges from `min_val` to
            `max_val`, to a `span` value which ranges from 0 to 2**`num_bits`.
            This is just basic linear interpolation over a given bit range.
            Data is transmitted as a `span` value to maximize the number of
            useful values which can occupy a given number of bits.

            Optionally, `span_min` and `span_max` can be given which will limit
            the lower and upper bound value for `span` used in the conversion
            process. `span` values below `span_min` will plateau and correspond to
            `min_val` and `span` values above `span_max` will plateau and
            correspond to `max_val`. This is necessary since, if floating point
            arithmetic is to be avoided in the FSW, it's not always possible to
            map a value from its original range to an exact power of 2.

            **Note:** `max_val` is the value which makes `span` take its highest
            value (2^n-1) and `min_val` is the value which makes `span` 0.
            If the range is inverted (say, 0xFF corresponds to 0 and 0x00
            corresponds to 100), then `max_val` will be smaller than `min_val`.
            """
            span_ub = (2 << (num_bits-1)) - 1  # upper bound of `span` (2^n-1)
            if span_max is None:
                span_max = span_ub
            elif span_max > span_ub:
                raise ValueError(
                    f"Given `span_max` cannot exceed 2**`num_bits`-1={span_ub}. "
                    f"{span_max} was given."
                )

            if span_min < 0:
                raise ValueError(
                    f"Given `span_min` cannot be <0. "
                    f"{span_min} was given."
                )

            # Keep `val` in bounds (and cause it to plateau if out of bounds):
            val = val if val < max_val else max_val
            val = val if val > min_val else min_val

            return (val-min_val)/(max_val-min_val) * (span_max-span_min) + span_min

        @property
        def _module(self) -> Module:
            return WatchdogHeartbeatPacketInterface.get_ds_module()

        @property
        def ChargeMah(self) -> float:
            return self.despan(self._ChargeRaw, 7, 29.1, 3500, span_max=120)

        @property
        def ChargePercent(self) -> float:
            return self.ChargeMah / 3500 * 100

        @property
        def CurrentMilliamps(self) -> float:
            return self.despan(self._CurrentRaw, 7, 0, 600, span_max=120)

        @property
        def BattAdcTempKelvin(self) -> float:
            return self.despan(self._BattAdcTempRaw, 8, 75, -12.31, span_max=233)

        # Make public get, private set to signal that you can freely use these values
        # but modifying them directly can yield undefined behavior (specifically
        # `raw` not syncing up with whatever other data is in the container)

        @property
        def ChargeRaw(self) -> int: return self._ChargeRaw
        @property
        def HeaterStatus(self) -> bool: return self._HeaterStatus
        @property
        def CurrentRaw(self) -> int: return self._CurrentRaw
        @property
        def BatteryVoltageOk(self) -> bool: return self._BatteryVoltageOk
        @property
        def BattAdcTempRaw(self) -> int: return self._BattAdcTempRaw

        def __init__(self,
                     ChargeRaw: int,
                     HeaterStatus: int,
                     CurrentRaw: int,
                     BatteryVoltageOk: int,
                     BattAdcTempRaw: int
                     ) -> None:
            self._ChargeRaw = ChargeRaw
            self._HeaterStatus = bool(HeaterStatus)
            self._CurrentRaw = CurrentRaw
            self._BatteryVoltageOk = bool(BatteryVoltageOk)
            self._BattAdcTempRaw = BattAdcTempRaw

        def __repr__(self) -> str:
            return (
                f"[HEAT]: {' ON' if self.HeaterStatus else 'OFF'} \t\t "
                f"[BATT]: Voltage: {'GOOD' if self.BatteryVoltageOk else 'BAD'}, "
                f"Charge: {self.ChargeMah:4.0f}mAh = {self.ChargePercent:5.1f}%, "
                f"Temp: {self.BattAdcTempKelvin:5.1f}°K]"
            )


# Some useful type alias that make the subsequent class more readable:
WHB_PI = WatchdogHeartbeatPacketInterface
WHB_CP = WHB_PI.CustomPayload


class WatchdogHeartbeatPacket(WHB_PI[WHB_PI, WHB_CP]):
    # Properties (r-only class variables):
    START_FLAG: bytes = b'\xFF'  # Required start flag
    # Specify the `CUSTOM_PAYLOAD_CLASS` used by `CustomPayloadPacket` superclass (weird type signature is used to match the `Optional[Type[CPCT]]` used in the superclass):
    CUSTOM_PAYLOAD_CLASS: Optional[Type[WHB_CP]] = WHB_CP

    # Empty __slots__ allows super's __slots__ to not turn into __dict__:
    __slots__: List[str] = []

    def __repr__(self) -> str:
        return self.custom_payload.__repr__()

    @ classmethod
    def decode(cls,
               data: bytes,
               endianness_code: str = ENDIANNESS_CODE
               ) -> WatchdogHeartbeatPacket:
        flag, core_data = data[:1], data[1:]
        if cls.START_FLAG != flag:
            raise PacketDecodingException(
                data,
                "Start flag for `WatchdogTvacHeartbeatPacket` was invalid. "  # type: ignore
                f"Expected {cls.START_FLAG}, Got: {flag} ."
            )
        # TODO: use `bitstruct.compile` on first run to speed processing time.
        custom_payload = WatchdogHeartbeatPacket.CustomPayload(
            *bitstruct.unpack('u7u1u7u1u8', core_data)
        )
        return WatchdogHeartbeatPacket(
            custom_payload=custom_payload,
            raw=data,
            endianness_code=endianness_code
        )

    def encode(self, **kwargs: Any) -> bytes:
        #! TODO (not really a typical use case so not super necessary besides for completeness)
        #!! TODO: IS NECESSARY FOR IPC (OR JUST ENCODE THAT STUFF IN A STATE) <- Not with new `Packet`-specific `__reduce__` strategy
        raise NotImplementedError()

    @ classmethod
    def is_valid(cls, data: bytes, endianness_code: str = ENDIANNESS_CODE) -> bool:
        """
        Determines whether the given bytes constitute a valid packet of this type.
        """
        right_start = len(data) > 0 and data[:1] == cls.START_FLAG
        right_length = len(data) == 4  # Bytes

        return right_start and right_length

    @classmethod
    def build_minimum_packet(
        cls,
        payloads: PayloadCollection,
        raw: Optional[bytes],
        endianness_code: str
    ) -> WatchdogHeartbeatPacket:
        """ Builds a minimum representation of this Packet (before any
        additional elements from `__getstate__` are added back). Used by
        `Packet.__reduce__` for unpacking serialized data.
        """
        return cls(
            payloads=payloads,
            raw=raw,
            endianness_code=endianness_code
        )
