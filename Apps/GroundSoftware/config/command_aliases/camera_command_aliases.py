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

FRAME_SIZE: Final[int] = 243


@CommandAliasesBuilder
def ALIASES(standards: DataStandards) -> CommandAliases:
    def quick_frame(cam_num: int, frame_num: int) -> PreparedCommand:
        return PreparedCommand(
            'Camera_TakeImageSectionAdv',
            OrderedDict(
                camera_num=cam_num,
                start_line=int(FRAME_SIZE*(frame_num-1)),
                # grab a little extra jic
                end_line=int(FRAME_SIZE*(frame_num+0.25)),
                callback_id=0, erase_first=True,
                n_bin=7, compress_line=True
            ),
            DataPathway.WIRELESS,
            Magic.COMMAND,
            comment=f"Using camera {cam_num}, Quickly downlink a binned & compressed copy of frame_idx={frame_num-1} frame."
        )

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
        ),
        'take-quick-frame-3-cam-0': quick_frame(cam_num=0, frame_num=3),
        'take-quick-frame-3-cam-1': quick_frame(cam_num=1, frame_num=3),
        'read-quick-frame-3': PreparedCommand(
            'Camera_ReadImageSectionAdv',
            OrderedDict(
                start_line=int(FRAME_SIZE*2),
                end_line=int(FRAME_SIZE*(2+1.25)),  # grab a little extra jic
                n_bin=7, compress_line=True
            ),
            DataPathway.WIRELESS,
            Magic.COMMAND,
            comment=f"Quickly Read & downlink from Flash a binned & compressed copy of frame_idx=2 frame."
        ),
        'deployment-image': PreparedCommand(
            'Camera_CaptureDeploymentImage',
            OrderedDict(
                # NOTE: This is presently set up to be triggered by a tip not a
                # drop but the IMU-triggered-image principle is the same:
                camera_num=1, callback_id=10, erase_first=True,
                timeout_ms=120000, trigger_on_timeout=True,
                condition_mode_all=True,  # all = AND
                #
                imu_x_acc_on=False,
                imu_x_acc_trigger_mode=False,
                imu_x_acc_min=0,
                imu_x_acc_max=1,
                #
                imu_y_acc_on=True,  # True = YAcc is a trigger
                imu_y_acc_trigger_mode=False,  # trigger when out of window
                imu_y_acc_min=-50,
                imu_y_acc_max=50,
                #
                imu_z_acc_on=False,
                imu_z_acc_trigger_mode=False,
                imu_z_acc_min=0,
                imu_z_acc_max=1,
                #
                wdi_28_v_on=False,
                wdi_28_v_trigger_mode=False,
                wdi_28_v_min=0,
                wdi_28_v_max=1
            ),
            DataPathway.WIRELESS,
            Magic.COMMAND,
            comment=f"Triggers an image capture with the given settings once the given sensor triggers occur."
        )
    }
