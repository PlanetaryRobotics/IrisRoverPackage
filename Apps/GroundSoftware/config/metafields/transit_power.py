"""
Metafields pertaining to power, current, & voltages that are relevant in
transit.

Last Update: 01/07/2024
"""
from typing import Final, List, Tuple

from IrisBackendv3.data_standards.module import TelemetryChannel, Event
from IrisBackendv3.data_standards.fsw_data_type import FswDataType
from IrisBackendv3.codec.payload import (
    DownlinkedPayload, TelemetryPayload, EventPayload
)
from IrisBackendv3.data_standards.metafield import (
    MetaModule, MetaChannel, MetaChannelUpdateBehavior
)


class V_Heater_Live(MetaChannel):
    """Heater voltage, reflecting the actual current heater status."""
    _PROTO = TelemetryChannel('V_Heater_Live', 0, FswDataType.F64)
    _UPDATE_BEHAVIOR = MetaChannelUpdateBehavior.ANY
    _WATCHING = [
        'WatchdogDetailedStatus_Heater_EffectiveVoltage',
        'WatchdogDetailedStatus_Heater_IsHeating'
    ]

    def _calculate(self) -> Tuple[TelemetryPayload, List[DownlinkedPayload]]:
        vht = self._get_t('WatchdogDetailedStatus_Heater_EffectiveVoltage')
        v_heater: float = vht.data
        iht = self._get_t('WatchdogDetailedStatus_Heater_IsHeating')
        is_heating_val: str | int = iht.data

        is_heating: bool
        if isinstance(is_heating_val, str):
            is_heating = is_heating_val.upper() == 'HEATING'
        else:
            is_heating = is_heating_val == 0x00

        return v_heater if is_heating else 0, [vht, iht]


ALL_META_MODULES: Final[List[MetaModule]] = [

]
