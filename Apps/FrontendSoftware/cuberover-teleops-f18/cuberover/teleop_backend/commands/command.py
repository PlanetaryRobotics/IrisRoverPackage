import abc
import ctypes
import enum
import pprint
import time
import typing

from . import constants

SimpleCType = typing.Type[ctypes._SimpleCData]

class CommandParameter:
    """Contains the details of a single parameter of a command."""

    def __init__(self,
                 parameter_name: str,
                 parameter_ctype: SimpleCType,
                 parameter_value: int):
        """Initializes CommandParameter with the given data.

        # TODO: Do we need to handle non-integer parameter types?

        Args:
            parameter_name: The name of the parameter, which must match the name of the parameter in the subclass data
                struct
            parameter_ctype: The ctype of the parameter, which must match the ctype of the parameter in the subclass
                data struct
            parameter_value: The value of the parameter. Currently, only integer values are expected.
        """
        self.name = parameter_name
        self.ctype = parameter_ctype
        self.value = parameter_value


class CommandCtypeStruct(ctypes.BigEndianStructure):
    """A subclass that defines functions to convert a ctype structure to and from bytes."""

    def to_bytes(self) -> bytes:
        """Converts this ctype structure to bytes.

        Returns:
            bytes: The (big-endian) serialized structure contents.
        """
        return bytes(self)[:]

    def from_bytes(self, data_bytes: bytes):
        """Memmoves the given bytes into the ctype structure, which sets all field values from those in the given bytes.

        Args:
            data_bytes: The (big-endian) data bytes to fill this structure with.
        """
        num_bytes_to_copy = min(len(data_bytes), ctypes.sizeof(self))
        ctypes.memmove(ctypes.byref(self), bytes(data_bytes), num_bytes_to_copy)


class CommandHeaderStruct(CommandCtypeStruct):
    """A ctype struct that represents the contents of an encoded command header.

    See the docstring of the `Command` class for more details about the encoded command format.
    """
    _pack_ = 1
    _fields_ = [("command_type_id", ctypes.c_uint8),
                ("command_id", ctypes.c_uint8),
                ("data_length", ctypes.c_uint16),
                ("timestamp", ctypes.c_uint64)]


class Command(abc.ABC):
    """An abstract class that represents a command to/from the rover from/to the ground system.

    This class implements the vast majority of the functionality related to interacting with commands. Subclasses need
    only to define a subclass of `CommandCtypeStruct` that represents the expected layout of the Data section of the
    command, then provide an implemention of the `get_command_data_struct` class method that should simply return the
    class of the created data structure.

    If a given command has an empty Data section (i.e., has no parameters), then the subclass for this command should
    define `get_command_data_struct` to return None.

    An encoded command has the following structure:
    bytes 0-13: Header:
        byte      0: Command Type Id (8-bit unsigned integer)
        byte      1: Command Id (8-bit unsigned integer)
        bytes  2- 3: Data Length (16-bit unsigned integer)
        bytes  4-11: Timestamp (64-bit unsigned integer)
    bytes 14+: Data (command-specific)
    """

    def __init__(self,
                 command_type_id: constants.CommandTypeId,
                 command_id: enum.IntFlag,
                 timestamp: int,
                 command_parameters: typing.Sequence[CommandParameter],
                 lookup_id=-1):
        """Creates a Command object with the given data.

        Args:
            command_type_id: The type ID enumeration item for this command.
            command_id: The command-type-specific command ID enumeration item for this command.
            timestamp: The timestamp to be used in this command.
            command_parameters: An iterable containing the parameters of this command.
        """
        self.type_id = command_type_id
        self.command_id = command_id
        self.timestamp = timestamp
        self.parameters = command_parameters
        self.lookup_id = lookup_id

    def to_encoded_bytes(self) -> bytes:
        """Returns a bytestring containing the encoded form of this command."""

        # Get the subclass of CommandCtypeStruct that this command subclass has defined to represent the expected layout
        # of the command data
        command_cls_data_struct = self.get_command_data_struct()

        ####
        # Now create the data field, if there is one.
        ####
        if command_cls_data_struct is not None:
            # There should be a field in the data struct for each of the parameters in `self.parameters`. For each one,
            # populate the field in the data struct
            data_struct = command_cls_data_struct()
            for param in self.parameters:
                setattr(data_struct, param.name, param.value)

            # Convert the current data struct to bytes
            data_struct_bytes = data_struct.to_bytes()
        else:
            assert len(self.parameters) == 0, "No subclass data struct is defined, but this command has parameters"
            data_struct_bytes = bytes()

        ####
        # Now create the header
        ####

        # Create and fill in the header
        header_struct = CommandHeaderStruct()
        header_struct.command_type_id = self.type_id.value
        header_struct.command_id = self.command_id.value
        header_struct.data_length = len(data_struct_bytes)
        header_struct.timestamp = self.timestamp

        # Get the header bytes)
        header_struct_bytes = header_struct.to_bytes()

        return header_struct_bytes + data_struct_bytes

    @staticmethod
    def from_encoded(encoded_command: bytes) -> "Command":
        """Returns an instance of the Command subclass for the command encoded in the given bytestring.

        Args:
            encoded_command: A bytestring containing an encoded command.

        Returns:
            Command: A newly create instance of the Command subclass, populated with the values of the encoded command.
        """
        # This needs to be here instead of the top of the file in order to avoid circular imports
        from . import mappings

        assert len(encoded_command) >= ctypes.sizeof(CommandHeaderStruct), \
            "Encoded command byte array is smaller than the length of a command header"

        header_struct = Command.parse_header_from_encoded(encoded_command[:ctypes.sizeof(CommandHeaderStruct)])

        encoded_data = encoded_command[ctypes.sizeof(CommandHeaderStruct):]

        assert int(header_struct.data_length) == len(encoded_data), \
            "The length of the encoded data byte array ({}) does not agree " \
            "with the data length field from the command header ({})".format(encoded_data,
                                                                             int(header_struct.data_length))

        # Get the CommandTypeId item that corresponds to the command type ID value in the header. Then, find the
        # appropriate (for this command's type) map from command id to command subclass.
        command_type_enum_item = constants.CommandTypeId(header_struct.command_type_id)
        command_id_to_subclass_map = mappings.COMMAND_TYPE_TO_MAP_FROM_COMMAND_ID_TO_COMMAND_CLASS[
            command_type_enum_item]

        # Get the particular command ID enum class that is used for this command type
        command_id_enum_class = mappings.COMMAND_TYPE_TO_COMMAND_ID_ENUM_CLASS[command_type_enum_item]

        # Get the item from `command_id_enum_class` that corresponds to the command ID value in the header. Then, get
        # the specific command subclass for this command.
        command_id_enum_item = command_id_enum_class(header_struct.command_id)
        command_subclass = command_id_to_subclass_map[command_id_enum_item]

        # Get the subclass of CommandCtypeStruct that this command subclass has defined to represent the expected layout
        # of the command data
        command_cls_data_struct = command_subclass.get_command_data_struct()

        command_params = []
        if command_cls_data_struct is not None:
            assert len(encoded_data) >= ctypes.sizeof(command_cls_data_struct), \
                "Encoded data byte array is smaller than the expected length of" \
                " the data field for {}".format(command_subclass.__name__)

            # Construct an instance of the data structure, then fill it by parsing the encoded data
            data_struct = command_cls_data_struct()
            data_struct.from_bytes(encoded_data)

            # Assume each field in the subclass data structure is a command parameter, and create a list of these
            # parameters, their ctypes, and their values
            for param_name, param_ctype in data_struct._fields_:
                command_params.append(CommandParameter(parameter_name=param_name,
                                                       parameter_ctype=param_ctype,
                                                       parameter_value=getattr(data_struct, param_name)))
        else:
            assert len(encoded_data) == 0, \
                "No subclass data struct is defined, but this command has " \
                "{} bytes of encoded data".format(len(encoded_data))

        return command_subclass(command_type_id=command_type_enum_item,
                                command_id=command_id_enum_item,
                                timestamp=int(header_struct.timestamp),
                                command_parameters=command_params)

    @staticmethod
    def parse_header_from_encoded(encoded_header: bytes) -> CommandHeaderStruct:
        """Parses the command header structure from the given data bytes.

        Args:
            encoded_header: A bytestring containing an encoded command header.

        Returns:
            CommandHeaderStruct: The parsed header structure.
        """
        assert len(encoded_header) >= ctypes.sizeof(CommandHeaderStruct), \
            "Encoded header byte array is smaller than the expected length for a command header"

        header_struct = CommandHeaderStruct()
        header_struct.from_bytes(encoded_header)

        return header_struct

    @classmethod
    @abc.abstractmethod
    def get_command_data_struct(cls) -> CommandCtypeStruct:
        """Returns the Data section structure for this specific Command subclass.

        Returns:
            CommandCtypeStruct: the subclass of CommandCtypeStruct that represents the expected layout of the Data
                section of the particular command that is represented by the specific Command subclass.
        """
        raise NotImplementedError("get_command_data_struct() should only be called on subclasses of Command!")

    @staticmethod
    def from_database(database_command_dict: dict) -> "Command":
        """Returns an instance of the Command subclass for the command specified by the given database command object.

        Args:
            database_command_dict: A dictionary that details a single command.

        Returns:
            Command: A newly create instance of the Command subclass, populated with the values specified in the JSON
                string.
        """
        # This needs to be here instead of the top of the file in order to avoid circular imports
        from . import mappings

        command_type_id, command_id = mappings.DATABASE_COMMAND_NAME_TO_COMMAND_IDS[database_command_dict["name"]]

        # Find the appropriate (for this command's type) map from command id to command subclass.
        command_id_to_subclass_map = mappings.COMMAND_TYPE_TO_MAP_FROM_COMMAND_ID_TO_COMMAND_CLASS[command_type_id]

        # Get the specific command subclass for this command.
        command_subclass = command_id_to_subclass_map[command_id]

        # Get the subclass of CommandCtypeStruct that this command subclass has defined to represent the expected layout
        # of the command data
        command_cls_data_struct = command_subclass.get_command_data_struct()

        command_params = []
        if command_cls_data_struct is not None:
            # Assume each field in the subclass data structure is a command parameter, and create a list of these
            # parameters, their ctypes, and their values
            for param_name, param_ctype in command_cls_data_struct._fields_:
                assert param_name in database_command_dict["args"], \
                    "A command from the database of type {} is missing the argument {}\nFull command: {}".format(
                        database_command_dict["name"], param_name, pprint.pformat(database_command_dict))

                param_value = param_ctype(database_command_dict["args"][param_name])

                command_params.append(CommandParameter(parameter_name=param_name,
                                                       parameter_ctype=param_ctype,
                                                       parameter_value=param_value))
        else:
            assert "args" not in database_command_dict or len(database_command_dict["args"]) == 0, \
                "No subclass data struct is defined, but this command (of type {}) has the arguments {}".format(
                    database_command_dict["name"], database_command_dict["args"])

        return command_subclass(command_type_id=command_type_id,
                                command_id=command_id,
                                timestamp=int(round(time.time() * 1000)),
                                command_parameters=command_params,
                                lookup_id=database_command_dict["lookupID"])

    def __str__(self):
        params_str = ",\n        ".join(["{}: {}".format(param.name, param.value) for param in self.parameters])
        desc = ("Command: {{\n"
                "    class: {},\n"
                "    command type id: {},\n"
                "    command id: {},\n"
                "    timestamp: {},\n"
                "    parameters: {{\n"
                "        {}\n"
                "    }}\n"
                "}}").format(self.__class__.__name__,
                             self.type_id,
                             self.command_id,
                             self.timestamp,
                             params_str)
        return desc
