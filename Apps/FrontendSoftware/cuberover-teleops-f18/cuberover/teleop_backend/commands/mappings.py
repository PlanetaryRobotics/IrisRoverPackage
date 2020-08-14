import enum
import typing

from . import constants
from . import driving_command
from . import data_blob_command

# Some type aliases to make type annotations throughout this file a bit cleaner
CommandIdToCommandClassMap = typing.Dict[enum.IntFlag, typing.Type["Command"]]
MapFromCommandTypeToMap = typing.Dict[constants.CommandTypeId, CommandIdToCommandClassMap]
MapFromCommandTypeToIdEnum = typing.Dict[constants.CommandTypeId, typing.Type[enum.IntFlag]]


# Maps from constants.CommandTypeId enum values to dictionaries that map from the items of a command-type-specific enum
# (e.g. constants.DrivingCommandId enum for the Driving command type) to the Command subclass for that
# specific command.
COMMAND_TYPE_TO_MAP_FROM_COMMAND_ID_TO_COMMAND_CLASS: MapFromCommandTypeToMap = {
    constants.CommandTypeId.DRIVING: driving_command.DRIVING_COMMAND_ID_TO_CLASS_MAP,
    constants.CommandTypeId.DATA_BLOB: data_blob_command.DATA_BLOB_COMMAND_ID_TO_CLASS_MAP
}

# Maps from constants.CommandTypeId enum values to dictionaries that map from the items of a command-type-specific enum
# (e.g. constants.DrivingCommandId enum for the Driving command type) to the CommandId Enum class for that command
# type.
COMMAND_TYPE_TO_COMMAND_ID_ENUM_CLASS: MapFromCommandTypeToIdEnum = {
    constants.CommandTypeId.DRIVING: constants.DrivingCommandId,
}

DATABASE_COMMAND_NAME_TO_COMMAND_IDS = {
    "MoveForward": (constants.CommandTypeId.DRIVING, constants.DrivingCommandId.FORWARD),
    "MoveBackward": (constants.CommandTypeId.DRIVING, constants.DrivingCommandId.REVERSE),
    "TurnLeft": (constants.CommandTypeId.DRIVING, constants.DrivingCommandId.LEFT),
    "TurnRight": (constants.CommandTypeId.DRIVING, constants.DrivingCommandId.RIGHT),
}
