"""
Meta-Events for Alarms that could occur during the Transit phase of Mission.

Last Update: 01/07/2024
"""
from typing import Final, List

from IrisBackendv3.data_standards.module import TelemetryChannel, Event
from IrisBackendv3.data_standards.fsw_data_type import FswDataType
from IrisBackendv3.codec.payload import (
    DownlinkedPayload, TelemetryPayload, EventPayload
)
from IrisBackendv3.data_standards.metafield import (
    MetaModule, MetaChannel, MetaChannelUpdateBehavior
)

ALL_META_MODULES: Final[List[MetaModule]] = [

]
