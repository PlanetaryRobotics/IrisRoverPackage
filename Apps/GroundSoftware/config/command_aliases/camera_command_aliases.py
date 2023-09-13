"""
Aliases for Iris camera commands.

Last Update: 06/23/2023
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
            DataPathway.WIRELESS,
            Magic.COMMAND,
            comment="Select camera 0 (tell WD to set CAM_SEL LOW)."
        ),
        'cam-sel-1': PreparedCommand(
            'WatchDogInterface_ResetSpecific',
            OrderedDict(reset_value='FPGA_CAM_1'),
            DataPathway.WIRELESS,
            Magic.COMMAND,
            comment="Select camera 1 (tell WD to set CAM_SEL HIGH)."
        ),
        'take-image-0': PreparedCommand(
            'Camera_TakeImage',
            OrderedDict(camera_num=0, callback_id=0),
            DataPathway.WIRELESS,
            Magic.COMMAND,
            comment="Take an image with Camera 0."
        ),
        'take-image-1': PreparedCommand(
            'Camera_TakeImage',
            OrderedDict(camera_num=1, callback_id=0),
            DataPathway.WIRELESS,
            Magic.COMMAND,
            comment="Take an image with Camera 1."
        ),
        'take-draft-image-0': PreparedCommand(
            'Camera_TakeImageSection',
            OrderedDict(camera_num=0, start_line=13,
                        end_line=350, callback_id=0),
            DataPathway.WIRELESS,
            Magic.COMMAND,
            comment="Take a quick draft-quality image with Camera 0."
        ),
        'take-draft-image-1': PreparedCommand(
            'Camera_TakeImageSection',
            OrderedDict(camera_num=1, start_line=13,
                        end_line=350, callback_id=0),
            DataPathway.WIRELESS,
            Magic.COMMAND,
            comment="Take a quick draft-quality image with Camera 1."
        ),
        'image-grid-flash': PreparedCommand(
            'Camera_DownlinkGrid',
            OrderedDict(via_flash=True),
            DataPathway.WIRELESS,
            Magic.COMMAND,
            comment="Generates and writes a 5x5 gradient grid to camera flash then reads and downlinks it back."
        )
    }
