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
            comment="Tell WD to downlink a detailed status, a flight heartbeat, & Safety Timer status."
        ),
        'ReportStatus-ViaHercules': PreparedCommand(
            # Use Reset-Specific pathway for this:
            'WatchDogInterface_ResetSpecific',
            OrderedDict(reset_value='REQUEST_STATUS_REPORT'),
            DataPathway.WIRED,
            Magic.COMMAND,
            comment="Tell WD to downlink a detailed status, a flight heartbeat, & Safety Timer status via Hercules."
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
        'auto-switch-on': PreparedCommand(
            'GroundInterface_SetInterfaceAutoSwitch',
            OrderedDict(on=True),
            DataPathway.WIRED,
            Magic.COMMAND,
            comment="Allow Hercules to auto-switch it's primary downlink interface."
        ),
        'auto-switch-off': PreparedCommand(
            'GroundInterface_SetInterfaceAutoSwitch',
            OrderedDict(on=False),
            DataPathway.WIRED,
            Magic.COMMAND,
            comment="DON'T allow Hercules to auto-switch it's primary downlink interface."
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
        ),

        'roll-credits': PreparedCommand(
            'GroundInterface_RollCredits',
            OrderedDict(on=True),
            DataPathway.WIRED,
            Magic.COMMAND,
            comment="Tell the Rover it's allowed to emit names and messages."
        ),
        'names-start': PreparedCommand(
            'GroundInterface_RollCredits',
            OrderedDict(on=True),
            DataPathway.WIRED,
            Magic.COMMAND,
            comment="Tell the Rover it's allowed to emit names and messages."
        ),
        'names-stop': PreparedCommand(
            'GroundInterface_RollCredits',
            OrderedDict(on=False),
            DataPathway.WIRED,
            Magic.COMMAND,
            comment="Tell the Rover it's NOT allowed to emit names and messages."
        ),
        'names-1s': PreparedCommand(
            'GroundInterface_SetNameAndMessagePeriod',
            OrderedDict(seconds=1),
            DataPathway.WIRED,
            Magic.COMMAND,
            comment="Tell the Rover to emit a new name or message NO FASTER THAN once every 1s."
        ),
        'names-5s': PreparedCommand(
            'GroundInterface_SetNameAndMessagePeriod',
            OrderedDict(seconds=5),
            DataPathway.WIRED,
            Magic.COMMAND,
            comment="Tell the Rover to emit a new name or message NO FASTER THAN once every 5s."
        ),
        'names-10s': PreparedCommand(
            'GroundInterface_SetNameAndMessagePeriod',
            OrderedDict(seconds=10),
            DataPathway.WIRED,
            Magic.COMMAND,
            comment="Tell the Rover to emit a new name or message NO FASTER THAN once every 10s."
        )
    }
