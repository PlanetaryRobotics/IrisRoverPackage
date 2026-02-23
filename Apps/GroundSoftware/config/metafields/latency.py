"""
Metafields pertaining to latency.

Last Update: 01/07/2024
"""
from typing import Any, Final, Dict, List, Tuple

from datetime import datetime

import attrs

from IrisBackendv3.data_standards.module import TelemetryChannel, Event
from IrisBackendv3.data_standards.fsw_data_type import FswDataType
from IrisBackendv3.codec.payload import (
    DownlinkedPayload, TelemetryPayload, EventPayload
)
from IrisBackendv3.meta.metafield import (
    MetaModule, MetaChannel, MetaChannelUpdateBehavior
)


class _DownlinkTimesLatencySec(MetaChannel):
    """Latency between 'T_A' and 'T_B' (T_B-T_A) in DownlinkTimes over the past
    N payloads.

    Base class for other latency classes that define T_A and T_B.

    Averages over N payloads so we don't inherently double the payload volume.
    """
    _UPDATE_BEHAVIOR = MetaChannelUpdateBehavior.ANY
    _WATCHING = []

    # Number of entries being averaged:
    _N_AVG: int = 10
    # Names of time bounds:
    T_A: str
    T_B: str

    # Buffer for storing results to be averaged:
    _AVG_BUFFER: List[float]
    # Payloads driving the average:
    _AVG_PAYLOADS: List[DownlinkedPayload]

    def __init__(self) -> None:
        super().__init__()
        self._AVG_BUFFER = []
        self._AVG_PAYLOADS = []

    def _calculate(self) -> Tuple[Any | None, List[DownlinkedPayload]]:
        raise NotImplementedError(
            "`_calculate` is not relevant to `_DownlinkTimesLatencySec`. "
            "Whatever internal method called it should be overridden in the "
            "class."
        )

    def process(
        self,
        payload: DownlinkedPayload
    ) -> Tuple[float | None, List[DownlinkedPayload]]:
        """Unlike most other handlers, we just ingest any payload and produce a
        latency update every N payloads."""
        if (
            payload.downlink_times is None
            or payload.downlink_times.lander_rx is None
            or payload.downlink_times.amcc_rx is None
        ):
            # Don't have the right data
            return None, []

        times_dict: Dict[str, datetime] = attrs.asdict(payload.downlink_times)
        t_A, t_B = times_dict[self.T_A], times_dict[self.T_B]
        if t_A is None or t_B is None:
            # Can't calculate
            return None, []
        latency = (t_B - t_A).total_seconds()
        self._AVG_BUFFER.append(latency)
        self._AVG_PAYLOADS.append(payload)
        if len(self._AVG_BUFFER) != self._N_AVG:
            # Not enough data yet
            return None, []
        avg_latency = sum(self._AVG_BUFFER) / self._N_AVG
        self._AVG_BUFFER = []
        self._AVG_PAYLOADS = []
        return avg_latency, [payload]


class LanderToAmccLatencySec(_DownlinkTimesLatencySec):
    """Latency from Lander to AMCC over the past N payloads.

    Averages over N payloads so we don't inherently double the payload volume.
    """
    _PROTO = TelemetryChannel('LanderToAmccLatencySec', 0, FswDataType.F64)
    T_A = 'lander_rx'
    T_B = 'amcc_rx'
    _N_AVG: int = 10


class LanderToPmccLatencySec(_DownlinkTimesLatencySec):
    """Latency from Lander to PMCC over the past N payloads.

    Averages over N payloads so we don't inherently double the payload volume.
    """
    _PROTO = TelemetryChannel('LanderToPmccLatencySec', 0, FswDataType.F64)
    T_A = 'lander_rx'
    T_B = 'pmcc_rx'
    _N_AVG: int = 10


MOD_LATENCY = MetaModule(
    name="Latency",
    ID=0xA000,
    meta_channels=[
        # NOTE: Order Matters
        # To preserve backwards compatibility, don't delete, just deprecate.
        LanderToAmccLatencySec(),
        LanderToPmccLatencySec()
    ],
    meta_events=[]
)

ALL_META_MODULES: Final[List[MetaModule]] = [
    MOD_LATENCY
]
