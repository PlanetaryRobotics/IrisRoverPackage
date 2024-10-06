"""
Metafields pertaining to thermals.

Last Update: 10/03/2024
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

from datetime import timedelta


class ThermistorLookupTable(TypedDict):
    degC: np.ndarray
    R: np.ndarray
    adc: np.ndarray


_SBC_10K_THERMISTOR_LOOKUP_TABLE: ThermistorLookupTable = {  # for 10k on-SBC thermistors per Rev-I BOM, based on 3V3 12b ADC with 10kR high-side resistor and: https://octopart.com/datasheet/ntcg163jx103dt1s-tdk-71703662
    'degC': np.asarray([-40, -35, -30, -25, -20, -15, -10, -5, 0, 5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 65, 70, 75, 80, 85, 90, 95, 100, 105, 110, 115, 120, 125, 130, 135, 140, 145, 150]),
    'R': np.asarray([235830.7559, 173946.0536, 129916.7384, 98180.08736, 75021.6899, 57926.35333, 45168.27118, 35548.39008, 28223.72509, 22594.94578, 18231.39918, 14820.49884, 12133.17001, 10000, 8294.606244, 6921.923035, 5809.874448, 4903.40116, 4160.138877, 3501.875591, 2994.538893, 2572.582316, 2219.88672, 1923.67517, 1671.142015, 1459.525019, 1258.139345, 1105.71858, 975.1321202, 862.8308999, 765.9047863, 681.9572457, 609.0064814, 545.4070781, 489.7876121, 441.0007504, 398.0831659, 360.2232028, 326.734689]),
    'adc': np.asarray([3928, 3872, 3802, 3716, 3613, 3492, 3353, 3196, 3024, 2839, 2644, 2445, 2245, 2048, 1857, 1675, 1505, 1347, 1203, 1062, 944, 838, 744, 661, 586, 522, 458, 408, 364, 325, 291, 261, 235, 212, 191, 173, 157, 142, 130])
}

_ROVER_10K_THERMISTOR_LOOKUP_TABLE: ThermistorLookupTable = {  # for 10k on-SBC thermistors per Rev-I BOM, based on 3V3 12b ADC with 10kR high-side resistor and: https://octopart.com/datasheet/ntcg163jx103dt1s-tdk-71703662
    'degC': np.asarray([-55, -50, -45, -40, -35, -30, -25, -20, -15, -10, -5, 0, 5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 65, 70, 75, 80, 85, 90, 95, 100, 105, 110, 115, 120, 125, 130, 135, 140, 145, 150, 155]),
    'R': np.asarray([963000, 670100, 471700, 336500, 242600, 177000, 130400, 97070, 72930, 55330, 42320, 32650, 25390, 19900, 15710, 12490, 10000, 8057, 6531, 5327, 4369, 3603, 2986, 2488, 2083, 1752, 1481, 1258, 1072, 917.7, 788.5, 680, 588.6, 511.2, 445.4, 389.3, 341.7, 300.9, 265.4, 234.8, 208.3, 185.3, 165.3]),
    'adc': np.asarray([4053, 4035, 4010, 3977, 3933, 3876, 3803, 3713, 3601, 3468, 3312, 3135, 2938, 2725, 2502, 2274, 2048, 1827, 1618, 1423, 1245, 1085, 942, 816, 706, 610, 528, 458, 396, 344, 299, 261, 228, 199, 175, 153, 135, 120, 106, 94, 84, 74, 67])
}


def _adc_to_kelvin(LOOKUP_TABLE: ThermistorLookupTable, adc: int) -> float:
    return float(np.interp(
        float(adc),
        LOOKUP_TABLE['adc'][::-1],  # ::-1 b/c NTC
        LOOKUP_TABLE['degC'][::-1]  # ::-1 b/c NTC
    )) + 273.15


def BoardThermBuilder(N: int) -> Type[MetaChannel]:
    N = int(N)

    class BoardTherm(MetaChannel):
        """Builds a Board Thermistor MetaChannel for channel N."""
        _PROTO = TelemetryChannel(
            f'BoardTherm{N:d}_RT{N+1:d}Kelvin', 0, FswDataType.F64
        )
        _UPDATE_BEHAVIOR = MetaChannelUpdateBehavior.ANY
        _WATCHING = [f'WatchDogInterface_Therm{N:d}']

        def _calculate(self) -> Tuple[float, List[DownlinkedPayload]]:
            adc_payload = self._get_t(f'WatchDogInterface_Therm{N:d}')
            adc_val: int = adc_payload.data
            temp_K = _adc_to_kelvin(_SBC_10K_THERMISTOR_LOOKUP_TABLE, adc_val)

            return temp_K, [adc_payload]

    return BoardTherm


def RoverThermBuilder(N: int) -> Type[MetaChannel]:
    N = int(N)

    class BoardTherm(MetaChannel):
        """Builds a Rover Internal Thermistor MetaChannel for channel N."""
        _PROTO = TelemetryChannel(
            f'RoverTherm{N:d}_TMR{N-5:d}Kelvin', 0, FswDataType.F64
        )
        _UPDATE_BEHAVIOR = MetaChannelUpdateBehavior.ANY
        _WATCHING = [f'WatchDogInterface_Therm{N:d}']

        def _calculate(self) -> Tuple[float, List[DownlinkedPayload]]:
            adc_payload = self._get_t(f'WatchDogInterface_Therm{N:d}')
            adc_val: int = adc_payload.data
            temp_K = _adc_to_kelvin(
                _ROVER_10K_THERMISTOR_LOOKUP_TABLE, adc_val)

            return temp_K, [adc_payload]

    return BoardTherm


class BatteryTempAvgKelvin(MetaChannel):
    """Latest battery temp in Kelvin from any source, averaging multiple if
    they come in together (or updated within `_TIME_WINDOW`.)"""
    _PROTO = TelemetryChannel('BatteryTempAvgKelvin', 0, FswDataType.F64)
    _UPDATE_BEHAVIOR = MetaChannelUpdateBehavior.ANY
    _WATCHING = [
        'WatchdogHeartbeat_BattAdcTempKelvin',
        'WatchdogHeartbeatTvac_AdcTempKelvin',
        'MetaModTemps_RoverTherm8_TMR3Kelvin'
    ]
    _TIME_WINDOW = timedelta(minutes=2)

    def _calculate(self) -> Tuple[float, List[DownlinkedPayload]]:
        # Average all acceptable telem sources that have telemetered within the
        # last `_TIME_WINDOW`:
        payloads = [
            self._get_t(n) for n in self._WATCHING
        ]
        times = [
            p.downlink_times.scet_est for p in payloads
            if p.downlink_times is not None and p.downlink_times.scet_est is not None
        ]
        latest_time = max(times)
        payloads = [
            p for p, t in zip(payloads, times)
            if t > latest_time-self._TIME_WINDOW
        ]
        T_avg: float = sum(p.data for p in payloads) / len(payloads)

        return T_avg, payloads  # type: ignore


MOD_TEMPS = MetaModule(
    # Note: this gets prefixed by `MetaModule.UNIVERSAL_PREFIX` b/c it's a MetaModule:
    name="Temps",
    ID=0xA400,
    meta_channels=[
        # NOTE: Order Matters
        # To preserve backwards compatibility, don't delete, just deprecate.
        # On-SBC Thermistors:
        *[BoardThermBuilder(n)() for n in range(6)],
        # Inside-Rover Thermistors:
        *[RoverThermBuilder(n)() for n in range(6, 16)],
        BatteryTempAvgKelvin()
    ],
    meta_events=[]
)

ALL_META_MODULES: Final[List[MetaModule]] = [
    MOD_TEMPS
]
