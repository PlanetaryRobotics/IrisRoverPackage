import ctypes

from . import command
from . import constants


class InitDataBlobCommand(command.Command):
    """The class representing the command to initialize the transfer of a data blob from rover to ground.

    The serialized data field of this command has the following structure:
    bytes  0- 1: Data blob total size in bytes (16-bit unsigned integer)
    bytes  2- 3: CRC16 (CCITT) of entire data blob (16-bit unsigned integer)
    byte      4: Number of fragments that will be sent for this data blob (8-bit unsigned integer)
    """

    class CommandDataStruct(command.CommandCtypeStruct):
        _pack_ = 1
        _fields_ = [("distance", ctypes.c_uint8),
                    ("speed", ctypes.c_uint8),
                    ("num_images", ctypes.c_uint8),
                    ("data_crc", ctypes.c_uint16)]

    def __init__(self, command_type_id, command_id, timestamp, command_parameters, lookup_id=-1):
        super().__init__(command_type_id, command_id, timestamp, command_parameters, lookup_id)

    @classmethod
    def get_command_data_struct(cls):
        return InitDataBlobCommand.CommandDataStruct


DATA_BLOB_COMMAND_ID_TO_CLASS_MAP = {

}