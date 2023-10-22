"""
Aliases for Iris driving commands.

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
        # Nav deprecated
        # 'drive-fwd-200': PreparedCommand(
        #     'Navigation_NavDriveForward',
        #     OrderedDict(distance=200, speed=100, callback_id=0xBEEF),
        #     DataPathway.WIRED,
        #     Magic.COMMAND,
        #     comment="Drive forward 200cm."
        # ),

        # These are changing:
        # 'motor-control-get-telem': PreparedCommand(
        #     'MotorControl_McUpdateTelemetry',
        #     OrderedDict(),
        #     DataPathway.WIRED,
        #     Magic.COMMAND
        # ),
        # # Navigation_NavDriveForward[distance: uint8, speed: uint8, callback_id: uint16]
        # 'motor-control-spin-all': PreparedCommand(
        #     'MotorControl_McSpin',
        #     OrderedDict(motor_id=0x00, raw_ticks=20000),
        #     DataPathway.WIRED,
        #     Magic.COMMAND,
        #     comment="Low-level command to tell all drivers to spin their motors."
        # )
    }
