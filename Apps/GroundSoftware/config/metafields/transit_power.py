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
from IrisBackendv3.meta.metafield import (
    MetaModule, MetaChannel, MetaChannelUpdateBehavior
)


class VHeaterLive(MetaChannel):
    """Heater voltage, reflecting the actual current heater status."""
    _PROTO = TelemetryChannel('VHeaterLive', 0, FswDataType.F64)
    _UPDATE_BEHAVIOR = MetaChannelUpdateBehavior.ANY
    _WATCHING = [
        'WatchdogDetailedStatus_Heater_EffectiveVoltage',
        'WatchdogDetailedStatus_Heater_IsHeating'
    ]

    def _calculate(self) -> Tuple[float, List[DownlinkedPayload]]:
        vht = self._get_t('WatchdogDetailedStatus_Heater_EffectiveVoltage')
        v_heater: float = vht.data
        iht = self._get_t('WatchdogDetailedStatus_Heater_IsHeating')
        is_heating_val: str | int = iht.data

        is_heating: bool
        if isinstance(is_heating_val, str):
            is_heating = is_heating_val.upper() == 'HEATING'
        else:
            is_heating = is_heating_val == 0x01

        return v_heater if is_heating else 0, [vht, iht]


MOD_TRANSIT_POWER = MetaModule(
    # Note: this gets prefixed by `MetaModule.UNIVERSAL_PREFIX` b/c it's a MetaModule:
    name="TransitPower",
    ID=0xA300,
    meta_channels=[
        # NOTE: Order Matters
        # To preserve backwards compatibility, don't delete, just deprecate.
        VHeaterLive()
    ],
    meta_events=[]
)

ALL_META_MODULES: Final[List[MetaModule]] = [
    MOD_TRANSIT_POWER
]
