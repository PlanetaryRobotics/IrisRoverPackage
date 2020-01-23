import ctypes

from . import command
from . import constants


class DriveCommand(command.Command):
    class DriveCommandDataStruct(command.CommandCtypeStruct):
        _pack_ = 1
        _fields_ = [("distance", ctypes.c_uint8),
                    ("speed", ctypes.c_uint8),
                    ("num_images", ctypes.c_uint8),
                    ("data_crc", ctypes.c_uint16)]

    def __init__(self, command_type_id, command_id, timestamp, command_parameters):
        super().__init__(command_type_id, command_id, timestamp, command_parameters)

    @classmethod
    def get_command_data_struct(cls):
        return DriveCommand.DriveCommandDataStruct


DRIVING_COMMAND_ID_TO_CLASS_MAP = {
    constants.DrivingCommandId.FORWARD: DriveCommand,
    constants.DrivingCommandId.REVERSE: DriveCommand,
    constants.DrivingCommandId.LEFT: DriveCommand,
    constants.DrivingCommandId.RIGHT: DriveCommand,
}