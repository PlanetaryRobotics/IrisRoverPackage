"""
Shorthand aliases for common ground commands used during testing.

If you want to explore the Data Standards to build new commands, run:
`make standards` from `Apps/GroundSoftware`.

Last Update: 03/10/2023
"""
from IrisBackendv3.data_standards.data_standards import DataStandards
from IrisBackendv3.config.command_aliases import CommandAliases, CommandAliasesBuilder

from config.command_aliases import basic_command_aliases
from config.command_aliases import deploy_command_aliases
from config.command_aliases import battery_command_aliases
from config.command_aliases import power_command_aliases
from config.command_aliases import heater_command_aliases
from config.command_aliases import radio_command_aliases
from config.command_aliases import driving_command_aliases
from config.command_aliases import camera_command_aliases
from config.command_aliases import bgapi_command_aliases
from config.command_aliases import diag_command_aliases


@CommandAliasesBuilder
def ALIASES(standards: DataStandards) -> CommandAliases:
    return {
        **basic_command_aliases.ALIASES(standards),
        **deploy_command_aliases.ALIASES(standards),
        **battery_command_aliases.ALIASES(standards),
        **power_command_aliases.ALIASES(standards),
        **heater_command_aliases.ALIASES(standards),
        **radio_command_aliases.ALIASES(standards),
        **driving_command_aliases.ALIASES(standards),
        **camera_command_aliases.ALIASES(standards),
        **bgapi_command_aliases.ALIASES(standards),
        **diag_command_aliases.ALIASES(standards)
    }
