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


class DeckD2TempKelvin(MetaChannel):
    """Heater voltage, reflecting the actual current heater status."""
    _PROTO = TelemetryChannel('DeckD2TempKelvin', 0, FswDataType.F64)
    _UPDATE_BEHAVIOR = MetaChannelUpdateBehavior.ANY
    _WATCHING = ['DeckD2TempKelvin']

    def _calculate(self) -> Tuple[TelemetryPayload, List[DownlinkedPayload]]:
        T_D2_degC_telem = self._get_t('DeckD2TempKelvin')
        T_D2_degC: float = T_D2_degC_telem.data
        return T_D2_degC + 273.15, [T_D2_degC_telem]


MOD_PEREGRINE = MetaModule(
    # Note: this gets prefixed by `MetaModule.UNIVERSAL_PREFIX` b/c it's a MetaModule:
    name="Peregrine",
    ID=0xA1,
    meta_channels=[
        # NOTE: Order Matters
        # To preserve backwards compatibility, don't delete, just deprecate.
        DeckD2TempKelvin()
    ],
    meta_events=[]
)

ALL_META_MODULES: Final[List[MetaModule]] = [
    MOD_PEREGRINE
]
