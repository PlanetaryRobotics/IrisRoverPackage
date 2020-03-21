import sys

from fprime_gds.common.data_types import ch_data
from fprime_gds.common.data_types import cmd_data
from fprime_gds.common.data_types import event_data
from fprime_gds.common.templates import ch_template
from fprime_gds.common.templates import cmd_template
from fprime_gds.common.templates import event_template

from fprime.common.models.serialize import enum_type
from fprime.common.models.serialize import serializable_type


def convert_event_from_data_type_to_database(data_type_obj: event_data.EventData) -> dict:
    """Converts an F Prime event data object to a database-format event dictionary.

    The resulting dictionary will have six top-level keys: "type", "id", "name", "component_name", "time", and
    "event_string".

    Args:
        data_type_obj: The F Prime event data object to be converted.

    Returns:
        The database-format event dictionary based on the contents of data_type_obj.
    """
    event_temp = data_type_obj.get_template()  # type: event_template.EventTemplate
    event_args = data_type_obj.get_args()

    if event_args is None:
        arg_str = "EMPTY EVENT OBJ"
    else:
        # The arguments are currently serializable objects which cannot be used to fill in a format string. This
        # converts them to values that can be.
        arg_val_list = [arg_obj.val for arg_obj in event_args]
        arg_str = event_temp.get_format_str() % tuple(arg_val_list)

    return {
        "type": "event",
        "id": event_temp.get_id(),
        "name": event_temp.get_name(),
        "component_name": event_temp.get_comp_name(),
        "time": data_type_obj.get_time().to_readable(),
        "event_string": arg_str
    }


def convert_telemetry_from_data_type_to_database(data_type_obj: ch_data.ChData) -> dict:
    """Converts an F Prime telemetry data object to a database-format telemetry dictionary.

    The resulting dictionary will have seven top-level keys: "type", "id", "name", "component_name", "time", "value",
    and "formatted_value".

    Args:
        data_type_obj: The F Prime telemetry data object to be converted.

    Returns:
        The database-format telemetry dictionary based on the contents of data_type_obj.
    """
    ch_temp = data_type_obj.get_template()  # type: ch_template.ChTemplate
    ch_val = data_type_obj.get_val()
    ch_format_str = ch_temp.get_format_str()

    if ch_val is None:
        ch_val_str = "EMPTY CH OBJ"
    elif ch_format_str:
        ch_val_str = ch_format_str % ch_val
    else:
        ch_val_str = str(ch_val)

    return {
        "type": "telemetry",
        "id": ch_temp.get_id(),
        "name": ch_temp.get_name(),
        "component_name": ch_temp.get_comp_name(),
        "time": data_type_obj.get_time().to_readable(),
        "value": ch_val,
        "formatted_value": ch_val_str
    }


def convert_command_from_data_type_to_database(data_type_obj: cmd_data.CmdData) -> dict:
    """Converts an F Prime command data object to a database-format command dictionary.

    The resulting dictionary will have four top-level keys: "type", "name", "opcode", and "args".

    Args:
        data_type_obj: The F Prime command data object to be converted.

    Returns:
        The database-format command dictionary based on the contents of data_type_obj.
    """
    cmd_temp = data_type_obj.get_template()  # type: cmd_template.CmdTemplate

    # temp_args is a list of tuples of (name, description, arg_obj). We don't want to modify any of the arg_obj's
    # because they are shared references with the template - if we change them, they will be changed in the one and
    # template object for this command type.
    temp_args = cmd_temp.get_args()

    args_dict = {}
    for temp_arg_tuple, arg_obj_with_val in zip(temp_args, data_type_obj.get_args()):
        this_arg_name = temp_arg_tuple[0]

        # We need to get the value of the argument, but enums and serializable types need to be specially handled.
        if isinstance(arg_obj_with_val, enum_type.EnumType):
            this_arg_value = str(arg_obj_with_val.val)
        elif isinstance(arg_obj_with_val, serializable_type.SerializableType):
            raise NotImplementedError("Don't know what to put for value of SerializableType for database")
        else:
            this_arg_value = arg_obj_with_val.val

        args_dict[this_arg_name] = this_arg_value

    return {
        "type": "command",
        "name": cmd_temp.get_name(),
        "opcode": cmd_temp.get_op_code(),
        "args": args_dict
    }


class DatabaseObjectFormatError(Exception):
    """An exception raised when a database-format object is malformed or invalid."""
    pass


def convert_command_from_database_to_data_type(database_obj: dict, cmd_opcode_dict: dict) -> cmd_data.CmdData:
    """Converts a database-format command dictionary into an F Prime command data object.

    This expects the database command format from the frontend to be, at a minimum, like the following:
    {
        "type": command,
        "opcode": 1,
        "args": {
            "arg_one_name": "arg_one_value",
            "arg_two_name": 2,
            "arg_three_name": 3.0
        }
    }

    The opcode and argument names must exactly match the values in the dictionaries generated by F Prime.

    Args:
        database_obj: The database-format command dictionary to be converted.
        cmd_opcode_dict: A dictionary mapping from command opcodes to the associated F Prime command template objects.

    Returns:
        The F Prime command data object that was the result of converting database_obj.

    Raises:
        DatabaseObjectFormatError: If database_obj is malformed or has an opcode not in cmd_opcode_dict.
    """
    if "type" not in database_obj:
        error_msg = "The database-format object {} is missing the required \"{}\" key-value " \
                    "pair".format(database_obj, "type")
        print(error_msg, file=sys.stderr)
        raise DatabaseObjectFormatError(error_msg)
    elif "opcode" not in database_obj:
        error_msg = "The database-format object {} is missing the required \"{}\" key-value " \
                    "pair".format(database_obj, "opcode")
        print(error_msg, file=sys.stderr)
        raise DatabaseObjectFormatError(error_msg)
    elif "args" not in database_obj:
        error_msg = "The database-format object {} is missing the required \"{}\" key-value " \
                    "pair".format(database_obj, "args")
        print(error_msg, file=sys.stderr)
        raise DatabaseObjectFormatError(error_msg)
    elif database_obj["type"] != "command":
        error_msg = "The database-format object {} is expected to be a command object (i.e. \"{}\": \"{}\") " \
                    "but it instead has the has the \"{}\" type" \
                    "pair".format(database_obj, "type", "command", database_obj["type"])
        print(error_msg, file=sys.stderr)
        raise DatabaseObjectFormatError(error_msg)

    cmd_opcode = database_obj["opcode"]

    if cmd_opcode not in cmd_opcode_dict:
        error_msg = "The database-format command object {} has an unknown opcode ({})".format(database_obj, cmd_opcode)
        print(error_msg, file=sys.stderr)
        raise DatabaseObjectFormatError(error_msg)

    the_cmd_template = cmd_opcode_dict[database_obj["opcode"]]  # type: cmd_template.CmdTemplate
    db_args_dict = database_obj["args"]
    expected_arg_names = [name for (name, _, _) in the_cmd_template.get_args()]

    args_vals_from_db = []
    for expected_arg_name in expected_arg_names:
        if expected_arg_name not in db_args_dict:
            error_msg = "The database-format command object {} with opcode {} is missing an expected argument with" \
                        " the name \"{}\"".format(database_obj, cmd_opcode, expected_arg_name)
            print(error_msg, file=sys.stderr)
            raise DatabaseObjectFormatError(error_msg)

        # Their CmdData expects all the args to be strings and converts them to the appropriate native type itself.
        args_vals_from_db.append(str(db_args_dict[expected_arg_name]))

    return cmd_data.CmdData(cmd_args=args_vals_from_db, cmd_temp=the_cmd_template)
