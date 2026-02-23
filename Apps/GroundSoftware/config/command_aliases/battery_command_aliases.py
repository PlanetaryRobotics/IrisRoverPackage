"""
Aliases for Iris battery commands.

Last Update: 03/10/2023
"""
from typing import Final
from collections import OrderedDict
from IrisBackendv3.codec.magic import Magic
from IrisBackendv3.codec.metadata import DataPathway
from IrisBackendv3.data_standards.data_standards import DataStandards
from IrisBackendv3.config.command_aliases import PreparedCommand, CommandAliases, CommandAliasesBuilder


@CommandAliasesBuilder
def ALIASES(standards: DataStandards) -> CommandAliases:
    return {
        'batt-on': PreparedCommand(
            'WatchDogInterface_ResetSpecific',
            OrderedDict(reset_value='ENABLE_BATTERIES'),
            DataPathway.WIRED,
            Magic.WATCHDOG_COMMAND,
            comment="Switch ON the batteries (persistently)."
        ),
        'batt-off': PreparedCommand(
            'WatchDogInterface_ResetSpecific',
            OrderedDict(reset_value='DISABLE_BATTERIES'),
            DataPathway.WIRED,
            Magic.WATCHDOG_COMMAND,
            comment="Switch OFF the batteries (persistently)."
        ),
        'charge-start': PreparedCommand(
            'WatchDogInterface_ResetSpecific',
            OrderedDict(reset_value='BATTERY_START_CHARGE'),
            DataPathway.WIRED,
            Magic.WATCHDOG_COMMAND,
            comment="START charging batteries (turning on BE & REGE if necessary)."
        ),
        'charge-stop': PreparedCommand(
            'WatchDogInterface_ResetSpecific',
            OrderedDict(reset_value='BATTERY_STOP_CHARGE'),
            DataPathway.WIRED,
            Magic.WATCHDOG_COMMAND,
            comment="STOP charging batteries & turn off REGE."
        ),
        'SetChargerEn': PreparedCommand(
            'WatchDogInterface_SetChargerEnable',
            OrderedDict(charge_en='CHARGE_EN_OFF'),
            DataPathway.WIRED,
            Magic.WATCHDOG_COMMAND,
            comment="Set charger enable (CE). Default=OFF."
        ),
        'SetChargerPowerConnection': PreparedCommand(
            'WatchDogInterface_SetChargerPowerConnection',
            OrderedDict(v_lander_reg_en=False),
            DataPathway.WIRED,
            Magic.WATCHDOG_COMMAND,
            comment="Set charger power connection (REGE). Default=OFF."
        ),
        'batt-en-on': PreparedCommand(
            'WatchDogInterface_SetBatteryConnection',
            OrderedDict(batt_en=True),
            DataPathway.WIRED,
            Magic.WATCHDOG_COMMAND,
            comment="Set non-persistent battery enable (BE) to ON."
        ),
        'batt-en-off': PreparedCommand(
            'WatchDogInterface_SetBatteryConnection',
            OrderedDict(batt_en=False),
            DataPathway.WIRED,
            Magic.WATCHDOG_COMMAND,
            comment="Set non-persistent battery enable (BE) to OFF."
        ),
        'SetBatteryControlEnable': PreparedCommand(
            'WatchDogInterface_SetBatteryControlEnable',
            OrderedDict(batt_ctrl_en='BATT_CTRL_EN_OFF'),
            DataPathway.WIRED,
            Magic.WATCHDOG_COMMAND,
            comment="Set state for battery controller circuitry (BCTRLE). Should be no-op. Default=OFF."
        ),
        'SetBatteryLatch': PreparedCommand(
            'WatchDogInterface_SetBatteryLatch',
            OrderedDict(latch_batt='LATCH_BATT_OFF'),
            DataPathway.WIRED,
            Magic.WATCHDOG_COMMAND,
            comment="Set persisting battery latch state (LB). Default=LOW."
        ),
        'SetLatchSet': PreparedCommand(
            'WatchDogInterface_SetLatchSet',
            OrderedDict(latch_set='LATCH_SET_OFF'),
            DataPathway.WIRED,
            Magic.WATCHDOG_COMMAND,
            comment="Set state for battery latch set (LS). Should be no-op. Default=OFF."
        ),
        'SetLatchReset': PreparedCommand(
            'WatchDogInterface_SetLatchReset',
            OrderedDict(latch_reset='LATCH_RESET_OFF'),
            DataPathway.WIRED,
            Magic.WATCHDOG_COMMAND,
            comment="Set state for battery latch reset (LR). Should be no-op. Default=OFF."
        )
    }
