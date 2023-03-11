"""
Aliases for basic Iris commands.

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
        'ReportStatus': PreparedCommand(
            'WatchDogInterface_RequestStatusReport',
            OrderedDict(confirm='CONFIRM_REQUEST'),
            DataPathway.WIRED,
            Magic.WATCHDOG_COMMAND,
            comment="Tell WD to downlink a detailed status & a flight heartbeat."
        ),
        'transit': PreparedCommand(
            'WatchDogInterface_SwitchToKeepAliveMode',
            OrderedDict(confirm='CONFIRM_ALIVE'),
            DataPathway.WIRED,
            Magic.WATCHDOG_COMMAND,
            comment="Switch back to KeepAlive Mode."
        ),
        'setup': PreparedCommand(
            'WatchDogInterface_SwitchToServiceMode',
            OrderedDict(confirm='CONFIRM_SERVICE'),
            DataPathway.WIRED,
            Magic.WATCHDOG_COMMAND,
            comment="Switch to Service Mode."
        ),
        'power-on': PreparedCommand(
            'WatchDogInterface_PrepareForDeployment',
            OrderedDict(confirm='CONFIRM_PREP'),
            DataPathway.WIRED,
            Magic.WATCHDOG_COMMAND,
            comment="Switch to Mission Mode."
        ),
        'wifi-mode': PreparedCommand(
            'GroundInterface_SetPrimaryInterface',
            OrderedDict(primary_interface='WF_121'),
            DataPathway.WIRELESS,
            Magic.COMMAND,
            comment="Tell Herc to downlink over WiFi (via a WiFi Command)."
        ),
        'wifi-mode-wired-send': PreparedCommand(
            'GroundInterface_SetPrimaryInterface',
            OrderedDict(primary_interface='WF_121'),
            DataPathway.WIRED,
            Magic.COMMAND,
            comment="Tell Herc to downlink over WiFi (via a wired Command)."
        ),
        'wired-mode': PreparedCommand(
            'GroundInterface_SetPrimaryInterface',
            OrderedDict(primary_interface='WATCHDOG'),
            DataPathway.WIRED,
            Magic.COMMAND,
            comment="Tell Herc to downlink over RS422."
        ),
        'monitor-hercules-on': PreparedCommand(
            'WatchDogInterface_ResetSpecific',
            OrderedDict(reset_value='HERCULES_WATCHDOG_ENABLE'),
            DataPathway.WIRED,
            Magic.WATCHDOG_COMMAND,
            comment="Tell WD to monitor Herc and reset it if unresponsive (default)."
        ),
        'monitor-hercules-off': PreparedCommand(
            'WatchDogInterface_ResetSpecific',
            OrderedDict(reset_value='HERCULES_WATCHDOG_DISABLE'),
            DataPathway.WIRED,
            Magic.WATCHDOG_COMMAND,
            comment="Tell WD to STOP monitoring Herc to see if it's unresponsive."
        )
    }
