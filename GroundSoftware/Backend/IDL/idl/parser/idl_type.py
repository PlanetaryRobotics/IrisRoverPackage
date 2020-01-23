import abc
import collections
import ctypes
import enum
import typing


from idl.parser import context
from idl.parser import idl_field


SimpleCType = typing.Type[ctypes._SimpleCData]


MAX_FLOAT_VAL = 3.4028235e+38
MAX_DOUBLE_VAL = 1.7976931348623157e+308


def is_ascii(s: str) -> bool:
    """Determines whether or not a string is entirely ASCII characters.

    Args:
        s: The string to check.

    Returns:
        bool: Whether or not the string is entirely ASCII.
    """
    return len(s) == len(s.encode('ascii'))


def check_valid_type_name(name: str) -> None:
    """Checks if a type name is valid.

    A type name is valid if it is all ASCII characters and contains only letters, numbers, and underscores.

    Args:
        name: The name to be checked.

    Returns:
        None

    Raises:
        ValueError: If the name is invalid.
    """
    if not is_ascii(name):
        raise ValueError("{} is not a valid name because it is not all ASCII characters".format(name))
    if not all([c.isalnum() or c == "_" for c in name]):
        raise ValueError("{} is not a valid name because it is contains a character other than letters, "
                         "numbers, and underscores".format(name))


def check_valid_enum_member_name(name: str) -> None:
    """Checks if an enum member name is valid.

    An enum member name is valid if it is all ASCII characters, contains only upper-case letters, numbers, and
    underscores.

    Args:
        name: The name to be checked.

    Returns:
        None

    Raises:
        ValueError: If the name is invalid.
    """
    if not is_ascii(name):
        raise ValueError("{} is not a valid name because it is not all ASCII characters".format(name))
    if not all([c.isalnum() or c == "_" for c in name]):
        raise ValueError("{} is not a valid enum member name because it is contains a character other than letters, "
                         "numbers, and underscores".format(name))
    elif any([c.islower() for c in name]):
        raise ValueError("{} is not a valid enum member name because it contains lower-case letters".format(name))


@enum.unique
class PrimitiveTypeCtype(enum.Enum):
    """An enumeration of the ctypes that are acceptable types for primitive IDL fields.

    The underlying value for each enum member consists of a tuple containing three things: the associated ctype class,
    the associated python type, and the struct format string character used to serialize/deserialize a value of the type
    represented by that enum member.
    """
    UINT8 = (ctypes.c_uint8, int, 'B')
    UINT16 = (ctypes.c_uint16, int, 'H')
    UINT32 = (ctypes.c_uint32, int, 'I')
    UINT64 = (ctypes.c_uint64, int, 'Q')
    INT8 = (ctypes.c_int8, 	int, 'b')
    INT16 = (ctypes.c_int16, int, 'h')
    INT32 = (ctypes.c_int32, int, 'i')
    INT64 = (ctypes.c_int64, int, 'q')
    FLOAT = (ctypes.c_float, float, 'f')
    DOUBLE = (ctypes.c_double, float, 'd')
    BOOL = (ctypes.c_bool, bool, '?')

    def get_ctype(self) -> ctypes._SimpleCData:
        """Gets the ctype class associated with this enum member.

        Returns:
            ctypes._SimpleCData: The ctype subclass.
        """
        return self.value[0]

    def get_python_type(self) -> typing.Type[typing.Union[int, float, bool]]:
        """Gets the python type that should be used to hold a value for this enum member.

        Returns:
            type: Either the int, float, or bool type.
        """
        return self.value[1]

    def get_struct_format_character(self) -> str:
        """Gets the character used to serialize/deserialize a value of this enum member's type using the struct module.

        Returns:
            str: The struct format string character.
        """
        return self.value[2]

    def __get_numeric_type_limits(self) -> typing.Union[typing.Tuple[int, int], typing.Tuple[float, float]]:
        """Gets the minimum and maximum limits on values of this type.

        Returns:
            (int, int) | (float, float): A tuple where the first value is the minimum limit and the second value is the
                maximum limit.

        Raises:
            RuntimeError: If this function was called on a non-numeric type.
        """
        if self.get_python_type() == int:
            c_int_type = self.get_ctype()
            signed = c_int_type(-1).value < c_int_type(0).value
            bit_size = ctypes.sizeof(c_int_type) * 8
            signed_limit = 2 ** (bit_size - 1)
            return (-signed_limit, signed_limit - 1) if signed else (0, 2 * signed_limit - 1)
        elif self.get_ctype() == ctypes.c_float:
            return -MAX_FLOAT_VAL, MAX_FLOAT_VAL
        elif self.get_ctype() == ctypes.c_double:
            return -MAX_DOUBLE_VAL, MAX_DOUBLE_VAL
        else:
            raise RuntimeError("__get_numeric_type_limits cannot be called for a non-numeric type")

    def is_in_bounds(self, value: typing.Union[int, float, bool]) -> bool:
        """Checks if the given value is in the bounds of this type.

        Args:
            value: The value to be checked.

        Returns:
            bool: Whether or not the value was in bounds.
        """
        if not isinstance(value, self.get_python_type()):
            raise ValueError("Cannot check if {} value is in bounds of a {} type".format(str(type(value)),
                                                                                         str(self.get_python_type())))

        if isinstance(value, int) or isinstance(value, float):
            type_min, type_max = self.__get_numeric_type_limits()
            return (value >= type_min) and (value <= type_max)
        elif isinstance(value, bool):
            # We've already checked that the value type matches the enum member type, and for a boolean that is enough
            # to know we're in bounds
            return True
        else:
            raise RuntimeError("value is not instance of int, float, or bool")

    def is_valid_value_string(self, value_string: str) -> bool:
        """Whether or not the given string contains the a valid value of this type.

        Args:
            value_string: The string to be checked.

        Returns:
            bool: Whether or not the string contained a value value.
        """
        try:
            value = self.get_python_type()(value_string)
            return self.is_in_bounds(value)
        except ValueError:
            return False

    def cast_string_to_type(self, value_string_to_cast: str):
        """Casts the given string to this enum member's corresponding python type.

        Args:
            value_string_to_cast: The value to be cast.

        Returns:
            int | float | bool: The result of the cast.

        Raises:
            ValueError: If the given value cannot be cast.
        """
        if not self.is_valid_value_string(value_string_to_cast):
            raise ValueError("{} is not a valid value to be cast to {}".format(value_string_to_cast,
                                                                               str(self.get_ctype())))

        python_value = self.get_python_type()(value_string_to_cast)
        return python_value

    @classmethod
    def from_str(cls, label: str) -> 'PrimitiveTypeCtype':
        """Gets the enum member corresponding to the given string (case-independent).

        Args:
            label: The string version of the enum member name.

        Returns:
            PrimitiveTypeCtype: The corresponding enum member.

        Raises:
            KeyError: If the given string doesn't have a corresponding enum member.
        """
        return cls[label.upper()]

    @classmethod
    def has_member(cls, label: str):
        """Whether or not the given string has a corresponding enum member (case-independent).

        Args:
            label: The string to use.

        Returns:
            bool: Whether or not the given string has a corresponding enum member.
        """
        return label.upper() in [name for name, member in cls.__members__.items()]


class IdlType(abc.ABC):
    """An abstract base class representing the type of an entity in an IDL schema."""

    def __init__(self, name: str):
        """Constructor.

        Args:
            name: The name of this type. Must be ASCII and contain only letters, numbers, and underscores.

        Raises:
            ValueError: If the given name is invalid.
        """
        check_valid_type_name(name)
        self._name = name

    def get_name(self):
        """Gets the name of this type.

        Returns:
            str: The type name.
        """
        return self._name

    @abc.abstractmethod
    def get_member(self, member_name: str):
        """Gets the member of this type with the given name.

        Args:
            member_name: The member name.

        Returns:
            Any: An implementation-dependent type representing the member of this type.

        Raises:
            ValueError: If this type doesn't have a member with the given name, or if this type doesn't support having
                members.
        """
        raise NotImplementedError("Concrete subclasses must implement this method")

    @abc.abstractmethod
    def has_member(self, member_name: str) -> bool:
        """Gets whether or not this type has a member with the given name.

        Args:
            member_name: The member name.

        Returns:
            bool: Whether or not this type has a member with the given name.
        """
        raise NotImplementedError("Concrete subclasses must implement this method")

    @abc.abstractmethod
    def get_value_size_in_bytes(self):
        """Gets the size of a serialized value of this type, in bytes.

        Returns:
            int: The size, in bytes.
        """
        raise NotImplementedError("Concrete subclasses must implement this method")

    @abc.abstractmethod
    def cast_to_type(self, value_string_to_cast: str):
        """Casts the given value to this type.

        Args:
            value_string_to_cast: The value to cast.

        Returns:
            A implementation-dependent value type that is the result of casting to this type.
        """
        raise NotImplementedError("Concrete subclasses must implement this method")

    @abc.abstractmethod
    def is_valid_value_string(self, value_string: str) -> bool:
        """Gets whether or not the given string can be cast to this type.

        Args:
            value_string: The string to check.

        Returns:
            Whether or not the given string can be cast to this type.
        """
        raise NotImplementedError("Concrete subclasses must implement this method")

    @abc.abstractmethod
    def get_struct_format_string(self) -> str:
        """Gets the string used to serialize/deserialize a value of this type using the struct module.

        Returns:
            str: The struct format string.
        """
        raise NotImplementedError("Concrete subclasses must implement this method")


class PrimitiveType(IdlType):
    """A class representing a primitive type in an IDL schema."""

    def __init__(self, name: str, primitive_ctype: PrimitiveTypeCtype):
        super().__init__(name)
        self.primitive_ctype = primitive_ctype

    def has_member(self, member_name: str) -> bool:
        return False

    def get_member(self, member_name: str) -> IdlType:
        raise ValueError("{} is not a member of the {} primitive value "
                         "(primitive value don't have members)".format(member_name, self.get_name()))

    def get_value_size_in_bytes(self):
        return ctypes.sizeof(self.primitive_ctype.get_ctype())

    def cast_to_type(self, value_string_to_cast: str):
        return self.primitive_ctype.cast_string_to_type(value_string_to_cast)

    def is_valid_value_string(self, value_string: str) -> bool:
        return self.primitive_ctype.is_valid_value_string(value_string)

    def get_struct_format_string(self) -> str:
        """Gets the string used to serialize/deserialize a value of this type using the struct module.

        Returns:
            str: The struct format string.
        """
        return self.primitive_ctype.get_struct_format_character()


class EnumType(IdlType):
    """A class representing an enumeration type in an IDL schema.

    In a message schema, enums must have a "type" field with the value "enum", an "underlying_type" key/value pair, and
    a "members" key/value pair. The values of the "members" key is expected to be a list of objects, where each object
    contains only one key/value pair. This one key/value pair should have a key that is the name of the enum member, and
    a value that is the underlying value of the that enum member.
    """
    UNDERLYING_TYPE_KEY = "underlying_type"
    MEMBERS_KEY = "members"

    class EnumMember:
        def __init__(self, name: str,
                     underlying_type: PrimitiveTypeCtype,
                     index: int,
                     underlying_value: typing.Union[int, float, bool]):
            check_valid_enum_member_name(name)
            self.name = name
            self.underlying_type = underlying_type
            self.index = index
            self.underlying_value = underlying_value

    def __init__(self, name: str, underlying_type: PrimitiveTypeCtype, member_dict: collections.OrderedDict):
        super().__init__(name)
        self.underlying_type = underlying_type
        self.member_dict = member_dict

    @staticmethod
    def parse_from_json(name, enum_json_object):
        underlying_type = PrimitiveTypeCtype.from_str(enum_json_object[EnumType.UNDERLYING_TYPE_KEY])
        all_members_dict = collections.OrderedDict()
        members_list = enum_json_object[EnumType.MEMBERS_KEY]
        for value_index, value_dict in enumerate(members_list):
            assert len(value_dict.items()) == 1, \
                "A single item in the values list of the {} enum has more than one key pair".format(name)

            enum_value_name, enum_value_underlying_value = next(iter(value_dict.items()))
            assert enum_value_name not in all_members_dict, \
                "There is more than one value in the {} enum with the name {}".format(name, enum_value_name)

            all_members_dict[enum_value_name] = EnumType.EnumMember(name=enum_value_name,
                                                                    underlying_type=underlying_type,
                                                                    index=value_index,
                                                                    underlying_value=enum_value_underlying_value)
            all_members_dict.move_to_end(enum_value_name)

        return EnumType(name=name,
                        underlying_type=underlying_type,
                        member_dict=all_members_dict)

    def has_member(self, member_name: str) -> bool:
        return member_name in self.member_dict

    def get_member(self, member_name: str):
        if member_name not in self.member_dict:
            raise ValueError("{} is not a member of the {} enum".format(member_name, self.get_name()))

        return self.member_dict[member_name]

    def get_value_size_in_bytes(self):
        return ctypes.sizeof(self.underlying_type.value)

    def cast_to_type(self, value_string_to_cast: str):
        if not self.is_valid_value_string(value_string_to_cast):
            raise ValueError("{} is not a valid value to be cast to {}".format(value_string_to_cast, self.get_name()))

        base_value_string = value_string_to_cast.replace("{}.".format(self.get_name()), "")
        return self.member_dict[base_value_string]

    def is_valid_value_string(self, value_string: str) -> bool:
        # Enum values have two acceptable forms: "MEMBER_NAME" or "ENUM_NAME.MEMBER_NAME". To handle both cases, I
        # create a copy of |value_string| with any occurrences of "ENUM_NAME." removed, then check if this copy is equal
        # to any of the members of this enum.
        base_value_string = value_string.replace("{}.".format(self.get_name()), "")
        return base_value_string in self.member_dict.keys()

    def get_struct_format_string(self) -> str:
        return self.underlying_type.get_struct_format_character()


class MessageType(IdlType):
    """A class representing a message type in an IDL schema.

    In a message schema, messages must have a "type" field with the value "message", a "command_type_id" key/value
    pair, a "command_id" key/value pair, and a "fields" key/value pair. The values of the "fields" key is expected to
    be a list of objects, where each object contains only one key/value pair. This one key/value pair should have a key
    that is the name of the field, and a value that is a dictionary containing the specification of that field (see
    the docstring of the idl_field.IdlField class for the details of a field specification).
    """

    FIELDS_KEY = "fields"
    COMMAND_TYPE_ID_KEY = "command_type_id"
    COMMAND_ID_KEY = "command_id"
    DATA_LEN_KEY = "data_len"
    TIMESTAMP_KEY = "timestamp"
    DESCRIPTION_KEY = "description"

    COMMAND_TYPE_ID_TYPE = PrimitiveType(COMMAND_TYPE_ID_KEY, PrimitiveTypeCtype.UINT8)
    COMMAND_ID_TYPE = PrimitiveType(COMMAND_ID_KEY, PrimitiveTypeCtype.UINT8)
    DATA_LEN_TYPE = PrimitiveType(DATA_LEN_KEY, PrimitiveTypeCtype.UINT16)
    TIMESTAMP_TYPE = PrimitiveType(TIMESTAMP_KEY, PrimitiveTypeCtype.UINT64)

    def __init__(self,
                 name: str,
                 command_type_id_value,
                 command_id_value,
                 description: str,
                 data_fields_dict: collections.OrderedDict):
        super().__init__(name)
        data_len = 0
        for field in data_fields_dict.values():
            data_len += field.size_in_bytes()

        self.all_message_contents_dict = collections.OrderedDict()

        self.description = description

        #
        # Fill in the implicit header fields
        #

        # First is the command type ID field
        self.all_message_contents_dict[MessageType.COMMAND_TYPE_ID_KEY] = \
            idl_field.IdlField(name=MessageType.COMMAND_TYPE_ID_KEY,
                               field_type=MessageType.COMMAND_TYPE_ID_TYPE,
                               fixed_value=command_type_id_value)
        self.command_type_id = command_type_id_value
        self.all_message_contents_dict.move_to_end(MessageType.COMMAND_TYPE_ID_KEY)

        # Second is the command ID field
        self.all_message_contents_dict[MessageType.COMMAND_ID_KEY] = \
            idl_field.IdlField(name=MessageType.COMMAND_ID_KEY,
                               field_type=MessageType.COMMAND_ID_TYPE,
                               fixed_value=command_id_value)
        self.command_id = command_id_value
        self.all_message_contents_dict.move_to_end(MessageType.COMMAND_ID_KEY)

        # Third is the data length field
        self.all_message_contents_dict[MessageType.DATA_LEN_KEY] = \
            idl_field.IdlField(name=MessageType.DATA_LEN_KEY,
                               field_type=MessageType.DATA_LEN_TYPE,
                               fixed_value=data_len)
        self.all_message_contents_dict.move_to_end(MessageType.DATA_LEN_KEY)

        # Fourth is the timestamp field
        self.all_message_contents_dict[MessageType.TIMESTAMP_KEY] = \
            idl_field.IdlField(name=MessageType.TIMESTAMP_KEY,
                               field_type=MessageType.TIMESTAMP_TYPE)
        self.all_message_contents_dict.move_to_end(MessageType.TIMESTAMP_KEY)

        #
        # Append all message specific fields to the message contents
        #
        for field_name, field_value in data_fields_dict.items():
            self.all_message_contents_dict[field_name] = field_value
            self.all_message_contents_dict.move_to_end(field_name)

        self.data_fields_dict = data_fields_dict
        self.total_size_in_bytes = sum([field.size_in_bytes() for field in self.all_message_contents_dict.values()])
        self.struct_format_string = ''.join([field.get_type().get_struct_format_string()
                                             for field in self.all_message_contents_dict.values()])

    @staticmethod
    def parse_from_json(included_context: context.Context, name: str, struct_json_object: typing.Dict):
        all_fields_dict = collections.OrderedDict()

        # Ensure we have command type id, command type, fields, and description keys
        required_keys = [MessageType.COMMAND_TYPE_ID_KEY, MessageType.COMMAND_ID_KEY, MessageType.FIELDS_KEY,
                         MessageType.DESCRIPTION_KEY]
        for required_key in required_keys:
            assert required_key in struct_json_object, \
                "The message object for {} is missing a {} key".format(name, required_key)

        command_type_id_value_str = struct_json_object[MessageType.COMMAND_TYPE_ID_KEY]
        command_type_id_value = idl_field.parse_fixed_value(included_context=included_context,
                                                            field_name=MessageType.COMMAND_TYPE_ID_KEY,
                                                            value=command_type_id_value_str,
                                                            field_type=MessageType.COMMAND_TYPE_ID_TYPE)

        command_id_value_str = struct_json_object[MessageType.COMMAND_ID_KEY]
        command_id_value = idl_field.parse_fixed_value(included_context=included_context,
                                                       field_name=MessageType.COMMAND_ID_KEY,
                                                       value=command_id_value_str,
                                                       field_type=MessageType.COMMAND_ID_TYPE)

        description_str = struct_json_object[MessageType.DESCRIPTION_KEY]

        fields_list = struct_json_object[MessageType.FIELDS_KEY]
        for field_index, field_dict in enumerate(fields_list):
            assert len(field_dict.items()) == 1, \
                "A single item in the fields list of the {} enum has more than one key pair".format(name)

            field_name, field_details_dict = next(iter(field_dict.items()))
            assert field_name not in all_fields_dict, \
                "There is more than one field in the {} struct with the name {}".format(name, field_name)

            all_fields_dict[field_name] = idl_field.make_field_from_json(included_context,
                                                                         field_name,
                                                                         field_details_dict)
            all_fields_dict.move_to_end(field_name)

        return MessageType(name=name,
                           command_type_id_value=command_type_id_value,
                           command_id_value=command_id_value,
                           description=description_str,
                           data_fields_dict=all_fields_dict)

    def has_member(self, member_name: str) -> bool:
        return member_name in self.all_message_contents_dict

    def get_member(self, member_name: str):
        if member_name not in self.all_message_contents_dict:
            raise ValueError("{} is not a member of the {} struct".format(member_name, self.get_name()))

        return self.all_message_contents_dict[member_name]

    def get_description(self) -> str:
        return self.description

    def get_command_type_id_value(self):
        return self.command_type_id

    def get_command_id_value(self):
        return self.command_id

    def get_value_size_in_bytes(self):
        return self.total_size_in_bytes

    def cast_to_type(self, value_string_to_cast: str):
        raise ValueError("Casting to a struct doesn't make any sense")

    def is_valid_value_string(self, value_string: str) -> bool:
        raise ValueError("A struct cannot have a value string")

    def get_struct_format_string(self) -> str:
        return self.struct_format_string