import copy
import typing

from idl.parser import idl_type
from idl.parser import context


def make_field_from_json(included_context: context.Context, field_name: str, field_details_dict: typing.Dict):
    """Parses an IdlField out of JSON data.

    Args:
        included_context: The included context, needed to determine the included enum types.
        field_name: The name of the field being parsed.
        field_details_dict: The dictionary of JSON data that was read as the value of the field being parsed from the
            message schema file.

    Returns:
        IdlField: The field object corresponding to the parsed field.
    """
    # Make a copy of the field details dict because we'll be popping values from it
    field_details_dict_copy = copy.deepcopy(field_details_dict)

    # Ensure we have "type", "units", and "default_value" keys
    required_keys = [IdlField.TYPE_KEY, IdlField.UNITS_KEY, IdlField.DEFAULT_VALUE_KEY]
    for required_key in required_keys:
        assert required_key in field_details_dict_copy, \
            "The field {} is missing a {} key".format(field_name, required_key)

    # Get the type
    type_value_str = field_details_dict_copy.pop(IdlField.TYPE_KEY)
    if idl_type.PrimitiveTypeCtype.has_member(type_value_str):
        field_type = idl_type.PrimitiveType(field_name, idl_type.PrimitiveTypeCtype.from_str(type_value_str))
    elif included_context.is_known_type(type_value_str):
        field_type = included_context.get_type(type_value_str)
    else:
        raise ValueError("{} is not a known type".format(type_value_str))

    # Get the units
    units = field_details_dict_copy.pop(IdlField.UNITS_KEY)
    if not isinstance(units, str):
        raise ValueError("The value of the {} key of the {} field (which is {}) is not a string".format(
            IdlField.UNITS_KEY, field_name, units))

    # Get the default value
    default_value = field_details_dict_copy.pop(IdlField.DEFAULT_VALUE_KEY)

    # If this field is a primitive or an enum, get the fixed value (if one is set)
    if isinstance(field_type, idl_type.PrimitiveType) or isinstance(field_type, idl_type.EnumType):
        fixed_value = None
        if IdlField.FIXED_VALUE_KEY in field_details_dict_copy:
            fixed_value_str = field_details_dict_copy.pop(IdlField.FIXED_VALUE_KEY)
            fixed_value = parse_fixed_value(included_context, field_name, fixed_value_str, field_type)

        if len(field_details_dict_copy) > 0:
            raise ValueError("The field {} has unhandled keys".format(field_name))

        return IdlField(name=field_name, field_type=field_type, type_string=type_value_str,
                        units=units, default_val=default_value, fixed_value=fixed_value)

    # This should never happen
    else:
        raise RuntimeError("field_type has a type other than PrimitiveType or EnumType")


def parse_fixed_value(included_context: context.Context,
                      field_name: str,
                      value,
                      field_type: "idl_type.IdlType"):
    """Parses a base value from from the JSON message schema.

    A "base value" may be an enum member specified as a string in either the "<EnumClass>.<EnumMember>" or the
    "<EnumMember>"  formats, or it may be a primitive value: an integer, float, or bool.

    If the base value is an enum member, the underlying value is returned.

    Args:
        included_context: The included context, needed to determine the included enum types.
        field_name: The name of the field whose value is being parsed (used only for logging purposes if an error
            occurs).
        value: The value from the JSON message schema that is being parsed.
        field_type: The type of the field being parsed. If the field is an enum type, this function will return the
            idl_type.EnumType.EnumMember object for the enum member specified as the value. Otherwise (i.e. when the
            field is a primitive type) this function will return a primitve value.

    Returns:
        idl_type.EnumType.EnumMember | int | float | bool: If the field type is an enum type, then the EnumMember object
            for the enum member specified as the value; otherwise, the parsed primitive value.
    """
    if isinstance(field_type, idl_type.PrimitiveType):
        # A primitive-type field could have a fixed value that is an enum member, or it could be a primitive. I
        # assume it is a primitive and try to act as such, and if that cast fails then I assume it is an enum
        # and attempt to get the underlying value of the specified enum member.
        if field_type.is_valid_value_string(value):
            return field_type.cast_to_type(value)
        else:
            # Check if the type is an enum value
            for type_in_context in included_context.get_all_types():
                if isinstance(type_in_context, idl_type.EnumType) and \
                        type_in_context.is_valid_value_string(value):
                    fixed_value_enum_member = type_in_context.cast_to_type(value)  # type: idl_type.EnumType.EnumMember
                    return field_type.cast_to_type(fixed_value_enum_member.underlying_value)

            # We didn't find the type we're looking for in our context, so raise an error
            raise ValueError("{} is not a valid fixed value for the {} field".format(value,
                                                                                     field_name))
    elif isinstance(field_type, idl_type.EnumType):
        if field_type.is_valid_value_string(value):
            return field_type.cast_to_type(value)  # type: idl_type.EnumType.EnumMember
        else:
            raise ValueError("{} is not a valid fixed value for the {} field".format(value,
                                                                                     field_name))

    else:
        raise ValueError("Field {} has a type ({}) that doesn't support a fixed value".format(field_name,
                                                                                              str(type(field_type))))


class IdlField:
    """A class representing a field in an IDL message.

    In a message schema, fields must have a "type" key/value pair, and can optionally have a "fixed_value" key/value
    pair.
    """

    TYPE_KEY = "type"
    UNITS_KEY = "units"
    DEFAULT_VALUE_KEY = "default_value"
    FIXED_VALUE_KEY = "fixed_value"

    def __init__(self,
                 name: str,
                 field_type: "idl_type.IdlType",
                 type_string=None,
                 units: str = None,
                 default_val: typing.Union[int, float, bool, str] = None,
                 fixed_value=None):
        """Constructor.

        Args:
            name: The name of this field.
            field_type: The type of this field.
            units
            fixed_value: If not None, then this is the value to which this field is fixed. If None, then this field is
                not a fixed field.
        """
        self.name = name
        self.field_type = field_type
        self.type_string = type_string
        self.units = units
        self.default_value = default_val
        self.fixed_value = fixed_value

    def size_in_bytes(self) -> int:
        """Returns the size of this field when serialized, in bytes.

        Returns:
            int: The field size in bytes.
        """
        return self.field_type.get_value_size_in_bytes()

    def get_type(self) -> "idl_type.IdlType":
        """Gets the type of this field.

        Returns:
            idl_type.IdlType: The type of this field.
        """
        return self.field_type

    def get_type_string(self) -> str:
        """Gets the type string of this field.

        Returns:
            str: The type string
        """
        return self.type_string

    def get_units(self) -> str:
        """Gets the units string of this field.

        Returns:
            str: The units string
        """
        return self.units

    def get_default_value(self) -> typing.Union[int, float, bool, str]:
        """Gets the default value of this field.

        Returns:
            typing.Union[int, float, bool, str]: The default value for this field
        """
        return self.default_value

    def get_fixed_value(self):
        """Returns the fixed value of this field. If this field is not fixed, this value will be None.

        Returns:
            idl_type.EnumType.EnumMember | int | float | bool | None: The fixed value for this field, or None if this
                field is not fixed.
        """
        return self.fixed_value

    def has_fixed_value(self):
        """Gets whether or not this field is fixed.

        Returns:
            bool: Whether or not this field is fixed.
        """
        return self.fixed_value is not None
