"""
Command Aliases for the Iris Lunar Rover FM1 Mission (c.2024)
"""
from typing import Final, List

from IrisBackendv3.meta.metafield import MetaModule

from config.metafields import latency
from config.metafields import peregrine
from config.metafields import transit_power
from config.metafields import transit_messages
from config.metafields import transit_alarms
from config.metafields import temps
from config.metafields import rover_power
from config.metafields import imu

ALL_META_MODULES: Final[List[MetaModule]] = [
    *latency.ALL_META_MODULES,
    *peregrine.ALL_META_MODULES,
    *transit_power.ALL_META_MODULES,
    *transit_messages.ALL_META_MODULES,
    *transit_alarms.ALL_META_MODULES,
    *temps.ALL_META_MODULES,
    *rover_power.ALL_META_MODULES,
    *imu.ALL_META_MODULES
]
