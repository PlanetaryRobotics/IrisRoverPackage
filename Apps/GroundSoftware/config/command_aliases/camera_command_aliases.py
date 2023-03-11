"""
Aliases for Iris camera commands.

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
        'cam-sel-0': PreparedCommand(
            'WatchDogInterface_ResetSpecific',
            OrderedDict(reset_value='FPGA_CAM_0'),
            DataPathway.WIRED,
            Magic.WATCHDOG_COMMAND,
            comment="Select camera 0 (tell WD to set CAM_SEL LOW)."
        ),
        'cam-sel-1': PreparedCommand(
            'WatchDogInterface_ResetSpecific',
            OrderedDict(reset_value='FPGA_CAM_1'),
            DataPathway.WIRED,
            Magic.WATCHDOG_COMMAND,
            comment="Select camera 1 (tell WD to set CAM_SEL HIGH)."
        ),
        'take-image-0': PreparedCommand(
            'Camera_TakeImage',
            OrderedDict(camera_num=0, callback_id=0),
            DataPathway.WIRED,
            Magic.COMMAND,
            comment="Take an image with Camera 0."
        ),
        'take-image-1': PreparedCommand(
            'Camera_TakeImage',
            OrderedDict(camera_num=1, callback_id=0),
            DataPathway.WIRED,
            Magic.COMMAND,
            comment="Take an image with Camera 1."
        )
    }
