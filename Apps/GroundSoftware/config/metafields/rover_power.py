"""
Metafields pertaining to rover power systems.

Last Update: 10/06/2024
"""
from typing import Final, List, Tuple, Dict, Type, Callable, TypedDict, cast

import numpy as np
from scipy import integrate  # type: ignore
from datetime import datetime, timedelta

from IrisBackendv3.data_standards.module import TelemetryChannel, Event, EnumItem
from IrisBackendv3.data_standards.fsw_data_type import FswDataType
from IrisBackendv3.codec.payload import (
    DownlinkedPayload, TelemetryPayload, EventPayload
)
from IrisBackendv3.codec.packet_classes.watchdog_detailed_status import WatchdogDetailedStatusPacket
from IrisBackendv3.meta.metafield import (
    MetaModule, MetaChannel, MetaChannelUpdateBehavior
)


def _adc_to_mA(R_sense_ohm: float, adc_reading: float) -> float:
    """Converts Hercules' ADC readings of the INA190A5 to currents in mA."""
    return (adc_reading/4095*3.3-0.000015)/500/(R_sense_ohm)*1000


class BatteryDischargeCurve(TypedDict):
    V_pack_curve: np.ndarray
    SOC_C_curve: np.ndarray


class BatteryThermalDeratingCurve(TypedDict):
    T_curve_kelvin: np.ndarray
    SOC_C_max_curve: np.ndarray


class BatteryLookupData(TypedDict):
    discharge: BatteryDischargeCurve
    derating: BatteryThermalDeratingCurve
    max_charge_ah: float
    max_energy_wh: float


def _battery_charge_curve_ah(table: BatteryLookupData, max_c_ah: float) -> np.ndarray:
    """ Returns a curve curve in Ah for the pack with the given table, given
    the max attainable charge in the battery's present state. """
    return table['discharge']['SOC_C_curve'] * max_c_ah


def _battery_energy_curve_wh(table: BatteryLookupData, max_c_ah: float) -> np.ndarray:
    """ Returns a curve curve in Ah for the pack with the given table, given
    the max attainable charge in the battery's present state."""
    c_curve = _battery_charge_curve_ah(table, max_c_ah)
    v_curve = table['discharge']['V_pack_curve']
    return np.insert(integrate.cumtrapz(v_curve, c_curve), 0, 0)


def _battery_thermal_derating(table: BatteryLookupData, temp_K: float) -> float:
    """Calculates the expected thermal derating in the batteries available
    charge as a proportion from 0 to 1, based on the given temperature in Kelvin."""
    T_curve = table['derating']['T_curve_kelvin']
    socc_curve = table['derating']['SOC_C_max_curve']
    # saturates beyond bounds but those are the operational limits of the
    # battery anyway, so we're good:
    return float(np.interp(temp_K, T_curve, socc_curve))


# Key statistics of Iris' 6s1p LG Chem INR18650 NMC Battery Pack:
_IRIS_FM1_BATTERY_TABLE: BatteryLookupData = {
    'discharge': {
        'V_pack_curve': np.asarray([14.99754, 15.17421, 15.73481, 16.25385, 16.73383, 17.17715, 18.92002, 20.04619, 20.74767, 21.16945, 21.41776, 21.56763, 21.66959, 21.7556, 21.84427, 21.94521, 22.06262, 22.19812, 22.35277, 22.52835, 22.72776, 22.95478, 23.21291, 23.5035, 23.82308, 24.15993, 24.48981, 24.77096, 24.93828, 24.95323]),
        'SOC_C_curve': np.asarray([0.00000, 0.00247, 0.01072, 0.01896, 0.02721, 0.03545, 0.07667, 0.11789, 0.15911, 0.20033, 0.24155, 0.28277, 0.32399, 0.36521, 0.40643, 0.44765, 0.48887, 0.53009, 0.57131, 0.61253, 0.65375, 0.69497, 0.73619, 0.77741, 0.81863, 0.85985, 0.90107, 0.94229, 0.98351, 1.00000])
    },
    'derating': {
        'T_curve_kelvin': np.asarray([263.15, 273.15, 296.15, 333.15]),
        'SOC_C_max_curve': np.asarray([0.70, 0.80, 1.00, 0.95])
    },
    'max_charge_ah': 3.5,
    'max_energy_wh': 0
}

_IRIS_FM1_BATTERY_TABLE['max_energy_wh'] = _battery_energy_curve_wh(
    _IRIS_FM1_BATTERY_TABLE,
    _IRIS_FM1_BATTERY_TABLE['max_charge_ah']
).max()


class BatteryState(TypedDict):
    V_pack: float
    temp_K: float


def _battery_charge_Ah(table: BatteryLookupData, state: BatteryState) -> float:
    """Computes the remaining available charge in Ah for a battery with the
    given `table` at the given voltage `V_pack` and temperature `temp_K`."""
    derating = _battery_thermal_derating(table, state['temp_K'])
    c_max = _IRIS_FM1_BATTERY_TABLE['max_charge_ah'] * derating
    c_curve = _battery_charge_curve_ah(table, c_max)
    v_curve = table['discharge']['V_pack_curve']
    return float(np.interp(state['V_pack'], v_curve, c_curve))


def _battery_socc(table: BatteryLookupData, state: BatteryState) -> float:
    """Computes the capacity-based state of charge as a proportion from 0 to 1
    for a battery with the given `table` at the given voltage `V_pack` and
    temperature `temp_K`."""
    return _battery_charge_Ah(table, state) / table['max_charge_ah']


def _battery_energy_Wh(table: BatteryLookupData, state: BatteryState) -> float:
    """Computes the remaining available energy in Wh for a battery with the
    given `table` at the given voltage `V_pack` and temperature `temp_K`."""
    derating = _battery_thermal_derating(table, state['temp_K'])
    c_max = _IRIS_FM1_BATTERY_TABLE['max_charge_ah'] * derating
    e_curve = _battery_energy_curve_wh(table, c_max)
    v_curve = table['discharge']['V_pack_curve']
    return float(np.interp(state['V_pack'], v_curve, e_curve))


def _battery_soce(table: BatteryLookupData, state: BatteryState) -> float:
    """Computes the capacity-based state of charge as a proportion from 0 to 1
    for a battery with the given `table` at the given voltage `V_pack` and
    temperature `temp_K`."""
    return _battery_energy_Wh(table, state) / table['max_energy_wh']


class CurrentReading3V3F(MetaChannel):
    _R_SENSE_OHM = 10/1000
    _EVENT_ARG = "current_3_v_3_fpga"
    _PROTO = TelemetryChannel("Fpga3V3Current_mA", 0, FswDataType.F64)
    _UPDATE_BEHAVIOR = MetaChannelUpdateBehavior.ANY
    _WATCHING = ['WatchDogInterface_AdcCurrentSensorReadingsReport']

    def _calculate(self) -> Tuple[float, List[DownlinkedPayload]]:
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

    def _calculate(self) -> Tuple[float, List[DownlinkedPayload]]:
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

    def _calculate(self) -> Tuple[float, List[DownlinkedPayload]]:
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

    def _calculate(self) -> Tuple[float, List[DownlinkedPayload]]:
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

    def _calculate(self) -> Tuple[float, List[DownlinkedPayload]]:
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

    def _calculate(self) -> Tuple[float, List[DownlinkedPayload]]:
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

    def _calculate(self) -> Tuple[float, List[DownlinkedPayload]]:
        currents_evt = self._get_e(
            'WatchDogInterface_AdcCurrentSensorReadingsReport'
        )
        raw_val = currents_evt.args.get(self._EVENT_ARG, 0)
        curr_mA = _adc_to_mA(self._R_SENSE_OHM, raw_val)

        return curr_mA, [currents_evt]


def BatteryParameter(
    name: str,
    func: Callable[[BatteryLookupData, BatteryState], float]
) -> Type[MetaChannel]:

    class BatteryParam(MetaChannel):
        """Builds a MetaChannel for some battery parameter that depends on
        battery state."""
        _PROTO = TelemetryChannel(name, 0, FswDataType.F64)
        _UPDATE_BEHAVIOR = MetaChannelUpdateBehavior.ANY
        _WATCHING = [
            'MetaModTemps_BatteryTempAvgKelvin',
            'WatchdogDetailedStatus_Adc_SwitchedBatteryVoltage'
        ]

        def _calculate(self) -> Tuple[float | None, List[DownlinkedPayload]]:
            temp_K = self._get_t('MetaModTemps_BatteryTempAvgKelvin')
            V_pack = self._get_t(
                'WatchdogDetailedStatus_Adc_SwitchedBatteryVoltage'
            )

            # Check if the battery voltage is sensible (switch is closed) so
            # we can infer this isn't just a floating reading:
            if V_pack.data < 15 or V_pack.data > 26:
                return None, []

            val = func(
                _IRIS_FM1_BATTERY_TABLE,
                BatteryState(V_pack=V_pack.data, temp_K=temp_K.data)
            )
            return val, [temp_K, V_pack]

    return BatteryParam


class HerculesLanderVoltage(MetaChannel):
    """Lander Voltage, as measured by Watchdog and reported by Hercules."""
    _PROTO = TelemetryChannel('HerculesLanderVoltage', 0, FswDataType.F64)
    _UPDATE_BEHAVIOR = MetaChannelUpdateBehavior.ANY
    _WATCHING = ['WatchDogInterface_Voltage28V']

    def _calculate(self) -> Tuple[float, List[DownlinkedPayload]]:
        tlm = self._get_t('WatchDogInterface_Voltage28V')
        raw = tlm.data & ((1 << 12)-1)
        # This is the same data as from the WD, so just use the same converter:
        volts = WatchdogDetailedStatusPacket.CustomPayload.Adc2LanderVoltage(
            raw
        )
        return volts, [tlm]


class FullSystemPowerEst_W(MetaChannel):
    """Estimated power of VSA bus"""
    _PROTO = TelemetryChannel('FullSystemPowerEst_W', 0, FswDataType.F64)
    _UPDATE_BEHAVIOR = MetaChannelUpdateBehavior.ANY
    _CACHE_DEPTH = 10
    _WATCHING = [
        'WatchdogDetailedStatus_Adc_FullSystemVoltage',
        'MetaModRoverPower_FullSystemCurrent_mA'
    ]
    _TIME_WINDOW = timedelta(minutes=30)

    def _calculate(self) -> Tuple[float | None, List[DownlinkedPayload]]:
        tlm_Vs = self._get_t(
            'WatchdogDetailedStatus_Adc_FullSystemVoltage', 10)
        tlm_I = self._get_t('MetaModRoverPower_FullSystemCurrent_mA')

        # Voltage reports much less frequently than time in mission mode.
        # Grab the latest switched-on (>15V) voltage:
        tlm_Vs = [
            p for p in tlm_Vs
            if p.data > 15 and p.scet_est is not None
        ]
        tlm_Vs = sorted(tlm_Vs, key=lambda x: cast(datetime, x.scet_est))
        if len(tlm_Vs) == 0:
            return None, []
        tlm_V = tlm_Vs[-1]

        # Only use if the values are roughly concurrent and possibly accurate
        # (within `_TIME_WINDOW` of each other):
        if tlm_V.scet_est is None or tlm_I.scet_est is None:
            return None, []
        if abs(tlm_V.scet_est - tlm_I.scet_est) > self._TIME_WINDOW:
            return None, []

        return tlm_V.data * tlm_I.data / 1000.0, [tlm_V, tlm_I]


class MotorPowerEst_W(MetaChannel):
    """Estimated power of 24V Motor bus"""
    _PROTO = TelemetryChannel('MotorPowerEst_W', 0, FswDataType.F64)
    _UPDATE_BEHAVIOR = MetaChannelUpdateBehavior.ANY
    _CACHE_DEPTH = 10
    _WATCHING = [
        'WatchdogDetailedStatus_Adc_Vcc24Voltage',
        'MetaModRoverPower_MotorBusCurrent_mA'
    ]
    _TIME_WINDOW = timedelta(minutes=30)

    def _calculate(self) -> Tuple[float | None, List[DownlinkedPayload]]:
        tlm_Vs = self._get_t('WatchdogDetailedStatus_Adc_Vcc24Voltage', 10)
        tlm_I = self._get_t('MetaModRoverPower_MotorBusCurrent_mA')

        # Voltage reports much less frequently than time in mission mode.
        # Grab the latest switched-on (>15V) voltage:
        tlm_Vs = [
            p for p in tlm_Vs
            if p.data > 15 and p.scet_est is not None
        ]
        tlm_Vs = sorted(tlm_Vs, key=lambda x: cast(datetime, x.scet_est))
        tlm_V: DownlinkedPayload | None
        if len(tlm_Vs) == 0:
            # We'll just est. this since it's very likely that for the entirety
            # of a move (which will turn on 24V), we won't have gotten a 24V
            # report from DetailedStatus yet since that's only on request in
            # Mission Mode as of FM1:
            voltage = 24
            tlm_V = None
        else:
            tlm_V = tlm_Vs[-1]
            voltage = tlm_V.data

        if tlm_V is not None and tlm_V.scet_est is None:
            # Not usable, just est:
            voltage = 24
            tlm_V = None

        if (
            tlm_V is not None and tlm_V.scet_est is not None
            and tlm_I.scet_est is not None
            and abs(tlm_V.scet_est - tlm_I.scet_est) > self._TIME_WINDOW
        ):
            if tlm_I.scet_est < tlm_V.scet_est:
                # current data is too stale, abort:
                return None, []
            # otherwise, voltage data is too old, just est.:
            voltage = 24
            tlm_V = None

        if tlm_I.scet_est is None and (tlm_V is None or tlm_V.scet_est is None):
            # No usable timestamp on meaningful data, have to skip:
            return None, []

        if tlm_V is None:
            return voltage * tlm_I.data / 1000.0, [tlm_I]
        else:
            return voltage * tlm_I.data / 1000.0, [tlm_V, tlm_I]


class Power3V3Est_W(MetaChannel):
    """Estimated power of full 3V3 Motor bus"""
    _PROTO = TelemetryChannel('Power3V3Est_W', 0, FswDataType.F64)
    _UPDATE_BEHAVIOR = MetaChannelUpdateBehavior.ANY
    _WATCHING = ['MetaModRoverPower_Full3V3Current_mA']

    def _calculate(self) -> Tuple[float | None, List[DownlinkedPayload]]:
        tlm_I = self._get_t('MetaModRoverPower_Full3V3Current_mA')
        return 3.3 * tlm_I.data / 1000.0, [tlm_I]


class LanderVoltage_FusedEst(MetaChannel):

    """Lander Voltage Estimate, Fused from Multiple Sensor Readings, if they've
    been updated within `_TIME_WINDOW`."""
    _PROTO = TelemetryChannel('LanderVoltage_FusedEst', 0, FswDataType.F64)
    _UPDATE_BEHAVIOR = MetaChannelUpdateBehavior.ANY
    _N_MIN_VALS = 0  # don't need to wait for at least 1 for every field
    _CACHE_DEPTH = 4  # 1 more than num. watching
    _WATCHING = [
        'WatchdogDetailedStatus_Adc_LanderVoltage',
        'WatchdogDetailedStatus_Adc_Vcc28Voltage',
        'MetaModRoverPower_HerculesLanderVoltage'
    ]
    _TIME_WINDOW = timedelta(seconds=1.5*50)  # 1.5x max typ. packet interval
    V_LANDER_MAX = 1.10*28.0  # Maximum possible lander voltage (allowed)

    def _calculate(self) -> Tuple[float | None, List[DownlinkedPayload]]:
        """
        Fuses two sensor readings weighted based on their relative
        uncertainties to estimated the true lander voltage.
        """
        dVcc28 = 0.5  # [Volts] (uncertainty in WD's Vcc28 reading)
        dLander_WD = 0.25  # [Volts] (uncertainty in WD's VL reading)
        # [Volts] (uncertainty in Herc's VL reading)
        dLander_Herc = dLander_WD * 7/12

        def reasonable_value_filter(p: TelemetryPayload) -> bool:
            return (
                p.data is not None and
                float(p.data) > 0.1 and
                float(p.data) < 1.5 * self.V_LANDER_MAX
            )

        return cast(Tuple[float, List[DownlinkedPayload]], self._get_time_weighted_avg(
            last_n=self._CACHE_DEPTH,
            time_window=self._TIME_WINDOW,
            payload_filter=reasonable_value_filter
            # extra_weights={
            #     'WatchdogDetailedStatus_Adc_Vcc28Voltage': (1.0-dVcc28/(dLander_WD+dLander_Herc+dVcc28)),
            #     'WatchdogDetailedStatus_Adc_LanderVoltage': (1.0-dLander_WD/(dLander_WD+dLander_Herc+dVcc28)),
            #     'MetaModRoverPower_HerculesLanderVoltage': (1.0-dLander_Herc/(dLander_WD+dLander_Herc+dVcc28))
            # }
        ))


class FullSystemCurrent_mA(MetaChannel):
    """Full System Current (ISA = Current of VSA) in milliamps, as reported by
    the TPS27SA08-Q1"""
    _PROTO = TelemetryChannel('FullSystemCurrent_mA', 0, FswDataType.F64)
    _UPDATE_BEHAVIOR = MetaChannelUpdateBehavior.ANY
    _WATCHING = [
        'WatchdogDetailedStatus_Adc_FullSystemCurrent'
    ]

    def _calculate(self) -> Tuple[float | None, List[DownlinkedPayload]]:
        tlm_raw = self._get_t('WatchdogDetailedStatus_Adc_FullSystemCurrent')
        reported_mA = tlm_raw.data * 1000
        # Check if this is near TPS27SA08-Q1's ISNSFH = 6-7.6mA (6.9mA mean):
        # If so, this isn't a current, it's a fault report:
        if reported_mA > 6 and reported_mA < 7.6:
            # Likely, no actual current is being drawn, switch is off or in a
            # fault state:
            return None, []
        else:
            return reported_mA, [tlm_raw]


class FullSystemSwitch_FaultState(MetaChannel):
    """Fault State of the Full-System Power Switch (VSA), the TPS27SA08-Q1, as
    reported by the current sensor (ISNSFH)."""
    _PROTO = TelemetryChannel(
        'FullSystemSwitch_FaultState', 0, FswDataType.ENUM,
        enum=[
            EnumItem(
                'OFF/FAULT', 0, comment="VSA switch is either OFF or in a fault state (ISNSFH=~6.9mA at ISA sensor output)."),
            EnumItem(
                'NO-FAULT', 1, comment="VSA switch is working / not in a fault state (ISA sensor output is not ISNSFH=~6.9mA)."),
        ]
    )
    _UPDATE_BEHAVIOR = MetaChannelUpdateBehavior.ANY
    _WATCHING = [
        'WatchdogDetailedStatus_Adc_FullSystemCurrent'
    ]

    def _calculate(self) -> Tuple[int, List[DownlinkedPayload]]:
        tlm_raw = self._get_t('WatchdogDetailedStatus_Adc_FullSystemCurrent')
        reported_mA = tlm_raw.data * 1000
        # Check if this is near TPS27SA08-Q1's ISNSFH = 6-7.6mA (6.9mA mean):
        # If so, this isn't a current, it's a fault report:
        if reported_mA > 6 and reported_mA < 7.6:
            # Likely, no actual current is being drawn, switch is off or in a
            # fault state:
            return 0, [tlm_raw]
        else:
            return 1, [tlm_raw]


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
        CurrentReadingMotors(),
        BatteryParameter("BatteryChargeAh", _battery_charge_Ah)(),
        BatteryParameter("BatteryEnergyWh", _battery_energy_Wh)(),
        BatteryParameter("BatteryStateOfCharge_Capacity", _battery_socc)(),
        BatteryParameter("BatteryStateOfCharge_Energy", _battery_soce)(),
        BatteryParameter("BatteryStateOfCharge", _battery_soce)(),
        LanderVoltage_FusedEst(),
        FullSystemCurrent_mA(),
        FullSystemSwitch_FaultState(),
        HerculesLanderVoltage(),
        FullSystemPowerEst_W(),
        MotorPowerEst_W(),
        Power3V3Est_W()
    ],
    meta_events=[]


)

ALL_META_MODULES: Final[List[MetaModule]] = [
    MOD_ROVER_POWER
]
