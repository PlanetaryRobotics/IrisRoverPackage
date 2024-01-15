"""
Metafields pertaining to rover power systems.

Last Update: 01/14/2024
"""
from typing import Final, List, Tuple, Type, Dict, TypedDict

import numpy as np

from IrisBackendv3.data_standards.module import TelemetryChannel, Event
from IrisBackendv3.data_standards.fsw_data_type import FswDataType
from IrisBackendv3.codec.payload import (
    DownlinkedPayload, TelemetryPayload, EventPayload
)
from IrisBackendv3.meta.metafield import (
    MetaModule, MetaChannel, MetaChannelUpdateBehavior
)


def _adc_to_mA(R_sense_ohm: float, adc_reading: float) -> float:
    return (adc_reading/4095*3.3-0.000015)/500/(R_sense_ohm)*1000


# class CurrentReading(MetaChannel):
#     """Builds a Current Reading, read from the currents event."""
#     _PROTO = TelemetryChannel("_mA", 0, FswDataType.F64)
#     _EVENT_ARG = ""
#     _R_SENSE_OHM = 0


class CurrentReading3V3F(MetaChannel):
    _R_SENSE_OHM = 10/1000
    _EVENT_ARG = "current_3_v_3_fpga"
    _PROTO = TelemetryChannel("Fpga3V3Current_mA", 0, FswDataType.F64)
    _UPDATE_BEHAVIOR = MetaChannelUpdateBehavior.ANY
    _WATCHING = ['WatchDogInterface_AdcCurrentSensorReadingsReport']

    def _calculate(self) -> Tuple[TelemetryPayload, List[DownlinkedPayload]]:
        currents_evt = self._get_e(
            'WatchDogInterface_AdcCurrentSensorReadingsReport'
        )
        raw_val = currents_evt.args.get(self._EVENT_ARG, 0)
        curr_mA = _adc_to_mA(self._R_SENSE_OHM, raw_val)

        return curr_mA, [currents_evt]


class CurrentReading3V3R(MetaChannel):
    _R_SENSE_OHM = 10/1000
    _EVENT_ARG = "current_3_v_3_radio"
    _PROTO = TelemetryChannel("Radio3V3Current_mA", 0, FswDataType.F64)
    _UPDATE_BEHAVIOR = MetaChannelUpdateBehavior.ANY
    _WATCHING = ['WatchDogInterface_AdcCurrentSensorReadingsReport']

    def _calculate(self) -> Tuple[TelemetryPayload, List[DownlinkedPayload]]:
        currents_evt = self._get_e(
            'WatchDogInterface_AdcCurrentSensorReadingsReport'
        )
        raw_val = currents_evt.args.get(self._EVENT_ARG, 0)
        curr_mA = _adc_to_mA(self._R_SENSE_OHM, raw_val)

        return curr_mA, [currents_evt]


class CurrentReading3V3(MetaChannel):
    _R_SENSE_OHM = 3/1000
    _EVENT_ARG = "current_3_v_3"
    _PROTO = TelemetryChannel("Full3V3Current_mA", 0, FswDataType.F64)
    _UPDATE_BEHAVIOR = MetaChannelUpdateBehavior.ANY
    _WATCHING = ['WatchDogInterface_AdcCurrentSensorReadingsReport']

    def _calculate(self) -> Tuple[TelemetryPayload, List[DownlinkedPayload]]:
        currents_evt = self._get_e(
            'WatchDogInterface_AdcCurrentSensorReadingsReport'
        )
        raw_val = currents_evt.args.get(self._EVENT_ARG, 0)
        curr_mA = _adc_to_mA(self._R_SENSE_OHM, raw_val)

        return curr_mA, [currents_evt]


class CurrentReading3V3H(MetaChannel):
    _R_SENSE_OHM = 50/1000
    _EVENT_ARG = "current_3_v_3_hercules"
    _PROTO = TelemetryChannel("Hercules3V3Current_mA", 0, FswDataType.F64)
    _UPDATE_BEHAVIOR = MetaChannelUpdateBehavior.ANY
    _WATCHING = ['WatchDogInterface_AdcCurrentSensorReadingsReport']

    def _calculate(self) -> Tuple[TelemetryPayload, List[DownlinkedPayload]]:
        currents_evt = self._get_e(
            'WatchDogInterface_AdcCurrentSensorReadingsReport'
        )
        raw_val = currents_evt.args.get(self._EVENT_ARG, 0)
        curr_mA = _adc_to_mA(self._R_SENSE_OHM, raw_val)

        return curr_mA, [currents_evt]


class CurrentReading1V2H(MetaChannel):
    _R_SENSE_OHM = 10/1000
    _EVENT_ARG = "current_1_v_2_hercules"
    _PROTO = TelemetryChannel("Hercules1V2Current_mA", 0, FswDataType.F64)
    _UPDATE_BEHAVIOR = MetaChannelUpdateBehavior.ANY
    _WATCHING = ['WatchDogInterface_AdcCurrentSensorReadingsReport']

    def _calculate(self) -> Tuple[TelemetryPayload, List[DownlinkedPayload]]:
        currents_evt = self._get_e(
            'WatchDogInterface_AdcCurrentSensorReadingsReport'
        )
        raw_val = currents_evt.args.get(self._EVENT_ARG, 0)
        curr_mA = _adc_to_mA(self._R_SENSE_OHM, raw_val)

        return curr_mA, [currents_evt]


class CurrentReading1V2F(MetaChannel):
    _R_SENSE_OHM = 10/1000
    _EVENT_ARG = "current_1_v_2_fpga"
    _PROTO = TelemetryChannel("Fpga1V2Current_mA", 0, FswDataType.F64)
    _UPDATE_BEHAVIOR = MetaChannelUpdateBehavior.ANY
    _WATCHING = ['WatchDogInterface_AdcCurrentSensorReadingsReport']

    def _calculate(self) -> Tuple[TelemetryPayload, List[DownlinkedPayload]]:
        currents_evt = self._get_e(
            'WatchDogInterface_AdcCurrentSensorReadingsReport'
        )
        raw_val = currents_evt.args.get(self._EVENT_ARG, 0)
        curr_mA = _adc_to_mA(self._R_SENSE_OHM, raw_val)

        return curr_mA, [currents_evt]


class CurrentReadingMotors(MetaChannel):
    _R_SENSE_OHM = 50/1000
    _EVENT_ARG = "current_24_v"
    _PROTO = TelemetryChannel("MotorBusCurrent_mA", 0, FswDataType.F64)
    _UPDATE_BEHAVIOR = MetaChannelUpdateBehavior.ANY
    _WATCHING = ['WatchDogInterface_AdcCurrentSensorReadingsReport']

    def _calculate(self) -> Tuple[TelemetryPayload, List[DownlinkedPayload]]:
        currents_evt = self._get_e(
            'WatchDogInterface_AdcCurrentSensorReadingsReport'
        )
        raw_val = currents_evt.args.get(self._EVENT_ARG, 0)
        curr_mA = _adc_to_mA(self._R_SENSE_OHM, raw_val)

        return curr_mA, [currents_evt]


MOD_ROVER_POWER = MetaModule(
    # Note: this gets prefixed by `MetaModule.UNIVERSAL_PREFIX` b/c it's a MetaModule:
    name="RoverPower",
    ID=0xA500,
    meta_channels=[
        # NOTE: Order Matters
        # To preserve backwards compatibility, don't delete, just deprecate.
        CurrentReading3V3F(),
        CurrentReading3V3R(),
        CurrentReading3V3(),
        CurrentReading3V3H(),
        CurrentReading1V2H(),
        CurrentReading1V2F(),
        CurrentReadingMotors()
    ],
    meta_events=[]
)

print([m._PROTO for m in MOD_ROVER_POWER.meta_channels])

ALL_META_MODULES: Final[List[MetaModule]] = [
    MOD_ROVER_POWER
]
