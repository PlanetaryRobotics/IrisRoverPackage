import ctypes

from teleop_backend.commands import command
from teleop_backend.commands import constants

dir(command)

class StraightDriveCommand(command.Command):
    class DriveCommandDataStruct(command.CommandCtypeStruct):
        _pack_ = 1
        _fields_ = [("Distance", ctypes.c_uint8),
                    ("Speed", ctypes.c_uint8),
                    ("Images", ctypes.c_uint8)]

    def __init__(self, command_type_id, command_id, timestamp, command_parameters, lookup_id=-1):
        super().__init__(command_type_id, command_id, timestamp, command_parameters, lookup_id)

    @classmethod
    def get_command_data_struct(cls):
        return StraightDriveCommand.DriveCommandDataStruct


class AngledDriveCommand(command.Command):
    class DriveCommandDataStruct(command.CommandCtypeStruct):
        _pack_ = 1
        _fields_ = [("Angle", ctypes.c_uint8),
                    ("Speed", ctypes.c_uint8),
                    ("Images", ctypes.c_uint8)]

    def __init__(self, command_type_id, command_id, timestamp, command_parameters, lookup_id=-1):
        super().__init__(command_type_id, command_id, timestamp, command_parameters, lookup_id)

    @classmethod
    def get_command_data_struct(cls):
        return AngledDriveCommand.DriveCommandDataStruct



DRIVING_COMMAND_ID_TO_CLASS_MAP = {
    constants.DrivingCommandId.FORWARD: StraightDriveCommand,
    constants.DrivingCommandId.REVERSE: StraightDriveCommand,
    constants.DrivingCommandId.LEFT: AngledDriveCommand,
    constants.DrivingCommandId.RIGHT: AngledDriveCommand,
}