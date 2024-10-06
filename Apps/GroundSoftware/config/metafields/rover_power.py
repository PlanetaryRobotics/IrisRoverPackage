"""
Metafields pertaining to rover power systems.

Last Update: 10/03/2024
"""
from typing import Final, List, Tuple, Dict, Type, Callable, TypedDict

import numpy as np
from scipy import integrate  # type: ignore

from IrisBackendv3.data_standards.module import TelemetryChannel, Event, EnumItem
from IrisBackendv3.data_standards.fsw_data_type import FswDataType
from IrisBackendv3.codec.payload import (
    DownlinkedPayload, TelemetryPayload, EventPayload
)
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


class LanderVoltage_FusedEst(MetaChannel):
    """Lander Voltage Estimate, Fused from Multiple Sensor Readings."""
    _PROTO = TelemetryChannel('LanderVoltage_FusedEst', 0, FswDataType.F64)
    _UPDATE_BEHAVIOR = MetaChannelUpdateBehavior.ANY
    _WATCHING = [
        'WatchdogDetailedStatus_Adc_LanderVoltage',
        'WatchdogDetailedStatus_Adc_Vcc28Voltage'
    ]
    V_LANDER_MAX = 1.10*28.0  # Maximum possible lander voltage (allowed)

    @classmethod
    def fused_est_lander_voltage(cls, VLander: float, Vcc28: float) -> float:
        """
        Fuses two sensor readings weighted based on their relative
        uncertainties to estimated the true lander voltage, weighted by
        sensor uncertainty.
        """
        dLander = 0.25  # [Volts] (uncertainty in LanderVoltage reading)
        dVcc28 = 0.5  # [Volts] (uncertainty in Vcc28Voltage reading)

        # Guard against 0:
        if max(abs(VLander), abs(Vcc28)) == 0:
            return 0

        # If a significant difference exists (i.e. one sensor is likely faulty)...:
        if (abs(Vcc28 - VLander) / max(abs(VLander), abs(Vcc28))) > 0.5:
            # If one of them is significantly greater (50%) than the max
            # possible lander voltage (i.e. way to large -- failed high),
            # take the other one:
            if (VLander > 1.5 * cls.V_LANDER_MAX) and (Vcc28 <= 1.5 * cls.V_LANDER_MAX):
                return Vcc28
            if (Vcc28 > 1.5 * cls.V_LANDER_MAX) and (VLander <= 1.5 * cls.V_LANDER_MAX):
                return VLander
            # otherwise, just use the larger of the two (since the lower
            # one likely failed low):
            return max(VLander, Vcc28)

        # Guard against poor uncertainty settings:
        if (dLander+dVcc28) == 0:
            return 0

        # Both sensors have consistent values, so fuse the results:
        return VLander * (1.0-dLander/(dLander+dVcc28)) + Vcc28 * (1.0-dVcc28/(dLander+dVcc28))

    def _calculate(self) -> Tuple[float, List[DownlinkedPayload]]:
        """
        Fuses two sensor readings weighted based on their relative
        uncertainties to estimated the true lander voltage.
        """
        tlm_VL = self._get_t('WatchdogDetailedStatus_Adc_Vcc28Voltage')
        tlm_V28 = self._get_t('WatchdogDetailedStatus_Adc_Vcc28Voltage')

        V = self.__class__.fused_est_lander_voltage(tlm_VL.data, tlm_V28.data)
        return V, [tlm_VL, tlm_V28]


# @property
# def Adc_FullSystemCurrent(self) -> float:  # [Amps]
#     # uses *bottom* 9b, so we don't have to shift it, it just saturates earlier:
#     # NOTE: , that's OFF or FAULT
#     full_adc_reading = self.Adc_FullSystemCurrentRaw
#     return float(full_adc_reading) / 4095 * 3.3 * 4600/1000


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
        FullSystemSwitch_FaultState()
    ],
    meta_events=[]


)

ALL_META_MODULES: Final[List[MetaModule]] = [
    MOD_ROVER_POWER
]
