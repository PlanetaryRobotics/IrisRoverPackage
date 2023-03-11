"""
Aliases for Iris deployment commands.

Last Update: 03/10/2023
"""
from collections import OrderedDict
from IrisBackendv3.codec.magic import Magic
from IrisBackendv3.codec.metadata import DataPathway
from IrisBackendv3.data_standards.data_standards import DataStandards
from IrisBackendv3.config.command_aliases import PreparedCommand, CommandAliases, CommandAliasesBuilder


@CommandAliasesBuilder
def ALIASES(standards: DataStandards) -> CommandAliases:
    return {
        'deploy-pretest--disable-heater-control-via-herc': PreparedCommand(
            'WatchDogInterface_ResetSpecific',
            OrderedDict(reset_value='AUTO_HEATER_CONTROLLER_DISABLE'),
            DataPathway.WIRED,
            Magic.COMMAND,
            comment="Test deployment cmd path by telling Herc to tell WD to enable heater control."
        ),
        'deploy': PreparedCommand(
            'WatchDogInterface_DisengageFromLander',
            OrderedDict(confirm='CONFIRM_DEPLOY'),
            DataPathway.WIRED,
            Magic.COMMAND,
            comment="Tell Herc to deploy (via RS422)."
        ),  # Note: there's no way to do just the hercules (you can tell just the hercules but then it'll immediately make watch)
        'deploy-wifi': PreparedCommand(
            'WatchDogInterface_DisengageFromLander',
            OrderedDict(confirm='CONFIRM_DEPLOY'),
            DataPathway.WIRELESS,
            Magic.COMMAND,
            comment="Tell Herc to deploy (via WiFi)."
        ),
        'deploy-wd-only': PreparedCommand(
            'WatchDogInterface_DisengageFromLander',
            OrderedDict(confirm='CONFIRM_DEPLOY'),
            DataPathway.WIRED,
            Magic.WATCHDOG_COMMAND,
            # (in case Herc-WD comms are broken)
            comment="Special command. Tell only WD to release its interlock."
        ),
        'undeploy': PreparedCommand(
            'WatchDogInterface_ResetSpecific',
            OrderedDict(reset_value='HDRM_OFF'),
            DataPathway.WIRED,
            Magic.COMMAND,
            comment="Tell WD to reset its deployment signal."
        ),  # Note: Deploy2 (Herc deploy pin) does not undeploy, but WD does.
        'undeploy-2': PreparedCommand(
            'WatchDogInterface_EngageFromLander',
            OrderedDict(),
            DataPathway.WIRED,
            Magic.COMMAND,
            comment="Tell Herc to reset its deployment signal."
        )  # Note: Also does not turn off Deploy2
    }
