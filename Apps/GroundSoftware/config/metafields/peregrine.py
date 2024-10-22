"""
Metafields pertaining to telemetry we get from Peregrine.

Last Update: 01/07/2024
"""
from typing import Final, List, Tuple

from IrisBackendv3.data_standards.module import TelemetryChannel, Event
from IrisBackendv3.data_standards.fsw_data_type import FswDataType
from IrisBackendv3.codec.payload import (
    DownlinkedPayload, TelemetryPayload, EventPayload
)
from IrisBackendv3.meta.metafield import (
    MetaModule, MetaChannel, MetaChannelUpdateBehavior
)

# Observed difference between the (anomalously high) reported loads from
# Peregrine and known measured currents of Iris for the operational power
# circuit:
_PM1_IRIS_OPERATIONAL_CURRENT_OFFSET_MA: Final[float] = 34


class DeckD2TempKelvin(MetaChannel):
    """Heater voltage, reflecting the actual current heater status."""
    _PROTO = TelemetryChannel('DeckD2TempKelvin', 0, FswDataType.F64)
    _UPDATE_BEHAVIOR = MetaChannelUpdateBehavior.ANY
    _WATCHING = ['Peregrine_DeckD2TempCelsius']

    def _calculate(self) -> Tuple[float, List[DownlinkedPayload]]:
        T_D2_degC_telem = self._get_t('Peregrine_DeckD2TempCelsius')
        T_D2_degC: float = T_D2_degC_telem.data
        return T_D2_degC + 273.15, [T_D2_degC_telem]


class OperationalCorrectedCurrentAvg_mA(MetaChannel):
    """Operational Current, with correction offset applied, in mA."""
    _PROTO = TelemetryChannel(
        'OperationalCorrectedCurrentAvg_mA', 0, FswDataType.F64)
    _UPDATE_BEHAVIOR = MetaChannelUpdateBehavior.ANY
    _WATCHING = ['Peregrine_IrisOperationalAvgCurrent']

    def _calculate(self) -> Tuple[float, List[DownlinkedPayload]]:
        reported_telem = self._get_t('Peregrine_IrisOperationalAvgCurrent')
        reported_mA: float = reported_telem.data * 1000
        current_mA = reported_mA - _PM1_IRIS_OPERATIONAL_CURRENT_OFFSET_MA
        current_mA = current_mA if current_mA > 0 else 0
        return current_mA, [reported_telem]


class OperationalCorrectedCurrentMax_mA(MetaChannel):
    """Max. Operational Current, with correction offset applied, in mA."""
    _PROTO = TelemetryChannel(
        'OperationalCorrectedCurrentMax_mA', 0, FswDataType.F64)
    _UPDATE_BEHAVIOR = MetaChannelUpdateBehavior.ANY
    _WATCHING = ['Peregrine_IrisOperationalMaxCurrent']

    def _calculate(self) -> Tuple[float, List[DownlinkedPayload]]:
        reported_telem = self._get_t('Peregrine_IrisOperationalMaxCurrent')
        reported_mA: float = reported_telem.data * 1000
        current_mA = reported_mA - _PM1_IRIS_OPERATIONAL_CURRENT_OFFSET_MA
        current_mA = current_mA if current_mA > 0 else 0
        return current_mA, [reported_telem]


class OperationalPowerAvg_W(MetaChannel):
    """Operational Power Avg. in Watts (based on Corrected Operational Current)."""
    _PROTO = TelemetryChannel('OperationalPowerAvg_W', 0, FswDataType.F64)
    _UPDATE_BEHAVIOR = MetaChannelUpdateBehavior.ANY
    _WATCHING = [
        'MetaModPeregrine_OperationalCorrectedCurrentAvg_mA',
        'MetaModRoverPower_LanderVoltage_FusedEst'
    ]

    def _calculate(self) -> Tuple[float, List[DownlinkedPayload]]:
        tlm_current = self._get_t(
            'MetaModPeregrine_OperationalCorrectedCurrentAvg_mA')
        tlm_volts = self._get_t('MetaModRoverPower_LanderVoltage_FusedEst')
        I: float = tlm_current.data / 1000
        V: float = tlm_volts.data
        return I*V, [tlm_current, tlm_volts]


class OperationalPowerMax_W(MetaChannel):
    """Operational Power Max. in Watts (based on Corrected Operational Current)."""
    _PROTO = TelemetryChannel('OperationalPowerMax_W', 0, FswDataType.F64)
    _UPDATE_BEHAVIOR = MetaChannelUpdateBehavior.ANY
    _WATCHING = [
        'MetaModPeregrine_OperationalCorrectedCurrentMax_mA',
        'MetaModRoverPower_LanderVoltage_FusedEst'
    ]

    def _calculate(self) -> Tuple[float, List[DownlinkedPayload]]:
        tlm_current = self._get_t(
            'MetaModPeregrine_OperationalCorrectedCurrentMax_mA')
        tlm_volts = self._get_t('MetaModRoverPower_LanderVoltage_FusedEst')
        I: float = tlm_current.data / 1000
        V: float = tlm_volts.data
        return I*V, [tlm_current, tlm_volts]


class ReleaseCurrentAvg_mA(MetaChannel):
    """Release Current Avg. in mA."""
    _PROTO = TelemetryChannel('ReleaseCurrentAvg_mA', 0, FswDataType.F64)
    _UPDATE_BEHAVIOR = MetaChannelUpdateBehavior.ANY
    _WATCHING = ['Peregrine_IrisReleaseAvgCurrent']

    def _calculate(self) -> Tuple[float, List[DownlinkedPayload]]:
        reported_telem = self._get_t('Peregrine_IrisReleaseAvgCurrent')
        reported_mA: float = reported_telem.data * 1000
        return reported_mA, [reported_telem]


class ReleaseCurrentMax_mA(MetaChannel):
    """Max. Release Current in mA."""
    _PROTO = TelemetryChannel('ReleaseCurrentMax_mA', 0, FswDataType.F64)
    _UPDATE_BEHAVIOR = MetaChannelUpdateBehavior.ANY
    _WATCHING = ['Peregrine_IrisReleaseMaxCurrent']

    def _calculate(self) -> Tuple[float, List[DownlinkedPayload]]:
        reported_telem = self._get_t('Peregrine_IrisReleaseMaxCurrent')
        reported_mA: float = reported_telem.data * 1000
        return reported_mA, [reported_telem]


class ReleasePowerAvg_W(MetaChannel):
    """Release Power Avg. in Watts"""
    _PROTO = TelemetryChannel('ReleasePowerAvg_W', 0, FswDataType.F64)
    _UPDATE_BEHAVIOR = MetaChannelUpdateBehavior.ANY
    _WATCHING = [
        'MetaModPeregrine_ReleaseCurrentAvg_mA',
        'MetaModRoverPower_LanderVoltage_FusedEst'
    ]

    def _calculate(self) -> Tuple[float, List[DownlinkedPayload]]:
        tlm_current = self._get_t('MetaModPeregrine_ReleaseCurrentAvg_mA')
        tlm_volts = self._get_t('MetaModRoverPower_LanderVoltage_FusedEst')
        I: float = tlm_current.data / 1000
        V: float = tlm_volts.data
        return I*V, [tlm_current, tlm_volts]


class ReleasePowerMax_W(MetaChannel):
    """Release Power Max. in Watts"""
    _PROTO = TelemetryChannel('ReleasePowerMax_W', 0, FswDataType.F64)
    _UPDATE_BEHAVIOR = MetaChannelUpdateBehavior.ANY
    _WATCHING = [
        'MetaModPeregrine_ReleaseCurrentMax_mA',
        'MetaModRoverPower_LanderVoltage_FusedEst'
    ]

    def _calculate(self) -> Tuple[float, List[DownlinkedPayload]]:
        tlm_current = self._get_t('MetaModPeregrine_ReleaseCurrentMax_mA')
        tlm_volts = self._get_t('MetaModRoverPower_LanderVoltage_FusedEst')
        I: float = tlm_current.data / 1000
        V: float = tlm_volts.data
        return I*V, [tlm_current, tlm_volts]


MOD_PEREGRINE = MetaModule(
    # Note: this gets prefixed by `MetaModule.UNIVERSAL_PREFIX` b/c it's a MetaModule:
    name="Peregrine",
    ID=0xA100,
    meta_channels=[
        # NOTE: Order Matters
        # To preserve backwards compatibility, don't delete, just deprecate.
        DeckD2TempKelvin(),
        OperationalCorrectedCurrentAvg_mA(),
        OperationalCorrectedCurrentMax_mA(),
        ReleaseCurrentAvg_mA(),
        ReleaseCurrentMax_mA(),
        OperationalPowerAvg_W(),
        OperationalPowerMax_W(),
        ReleasePowerAvg_W(),
        ReleasePowerMax_W()
    ],
    meta_events=[]
)

ALL_META_MODULES: Final[List[MetaModule]] = [
    MOD_PEREGRINE
]
