ENUM_FILE_TEMPLATE = \
    r"""import enum


@enum.unique
class {enum_name}(enum.Enum):
{enum_members}
"""

ENUM_MEMBER_TEMPLATE = r"    {member_name} = {member_value}"

MESSAGE_FILE_TEMPLATE = \
    r"""import collections
import struct
import time
{additional_imports}

class {class_name}:
    STRUCT_SERIALIZATION_FORMAT_STRING = "{struct_serialization_format_string}"
{fixed_value_constant_definitions}
    
    def __init__(self, {constructor_parameters_and_defaults}, lookup_id=-1):
{constructor_initializations}
        self.lookup_id = lookup_id
    
    def serialize(self, use_big_endian: bool) -> bytes:
        byte_order_char = '>' if use_big_endian else '<'
        full_format_string = byte_order_char + {class_name}.STRUCT_SERIALIZATION_FORMAT_STRING
        return struct.pack(full_format_string, 
                           {ordered_list_of_fields_to_serialize})
    
    @classmethod
    def get_serialized_size(cls) -> int:
        return struct.calcsize('>' + cls.STRUCT_SERIALIZATION_FORMAT_STRING)
    
    @staticmethod
    def parse_from_serialized_bytes(serialized_msg: bytes, serialized_msg_is_big_endian: bool) -> "{class_name}":
        byte_order_char = '>' if serialized_msg_is_big_endian else '<'
        full_format_string = byte_order_char + {class_name}.STRUCT_SERIALIZATION_FORMAT_STRING
        unpacked_tuple = struct.unpack(full_format_string, bytes(serialized_msg))
        return {class_name}({constructor_arguments_from_unpacked_tuple})
        
    @staticmethod
    def from_database_object(database_command_dict: dict) -> "{class_name}":
        return {class_name}(
            lookup_id=database_command_dict["lookupID"],
            timestamp=int(round(time.time() * 1000)),
            **database_command_dict["args"]
        )
    
    def __str__(self):
        params = collections.OrderedDict({{
{parameter_pairs}
        }})
        params_str = ",\n        ".join(["{{}}: {{}}".format(name, value) for name, value in params.items()])
        desc = ("Command: {{{{\n"
                "    class: {{}},\n"
                "    lookup_id: {{}},\n"
                "    command type id: {{}},\n"
                "    command id: {{}},\n"
                "    timestamp: {{}},\n"
                "    parameters: {{{{\n"
                "        {{}}\n"
                "    }}}}\n"
                "}}}}").format(self.__class__.__name__,
                             self.lookup_id,
                             self.COMMAND_TYPE_ID_FIXED_VALUE,
                             self.COMMAND_ID_FIXED_VALUE,
                             self.timestamp,
                             params_str)
        return desc
"""

FIXED_VALUE_TEMPLATE = r"    {fixed_value_name}_FIXED_VALUE = {fixed_value}"
CONSTRUCTOR_INSTANCE_VAR_INIT = r"        self.{name} = {name}"
FIXED_VALUE_PARAM_TEMPLATE = r"{class_name}.{fixed_value_name}_FIXED_VALUE"
INSTANCE_PARAMETER_TEMPLATE = r"self.{name}"
DESERIALIZED_TUPLE_INDEX_TEMPLATE = r"unpacked_tuple[{index}]"
PARAMETER_PAIR_TEMPLATE = """            "{name}": self.{name}"""

MESSAGE_UTILS_FILE_TEMPLATE = \
    """import inspect
import os
import struct
import sys
import typing

# This is one way to essentially allow a relative import in Python3. I add the the parent directory of this file's
# package directory to the python path, because then I know that this file's package is always globally accessible.
THIS_FILE_DIR = os.path.realpath(os.path.dirname(inspect.getfile(inspect.currentframe())))
THIS_PACKAGE_PARENT_DIR = os.path.dirname(THIS_FILE_DIR)
if THIS_PACKAGE_PARENT_DIR not in sys.path:
    sys.path.insert(0, THIS_PACKAGE_PARENT_DIR)

{message_file_imports}

# A map from message name to the message class
NAME_TO_CLASS_MAP = {{
{name_to_class_items}
}}

# A map from message class to the (command type id, command id) tuple corresponding to that message class.
CLASS_TO_ID_MAP = {{
{class_to_id_items}
}}

# The format string used by the struct module for the header of a message
HEADER_STRUCT_FORMAT_STRING = "BBHQ"
HEADER_SIZE_IN_BYTES = struct.calcsize(">" + HEADER_STRUCT_FORMAT_STRING)

def get_msg_info_from_header(serialized_msg: bytes, serialized_msg_is_big_endian: bool) -> typing.Tuple[type, int]:
    byte_order_char = '>' if serialized_msg_is_big_endian else '<'
    full_format_string = byte_order_char + HEADER_STRUCT_FORMAT_STRING

    assert len(serialized_msg) >= HEADER_SIZE_IN_BYTES, \\
        "Length of serialized message ({{}}) is not large enough to contain" \\
        " a header (required: {{}})".format(len(serialized_msg), struct.calcsize(full_format_string))
    
    header_contents_tuple = struct.unpack_from(full_format_string, bytes(serialized_msg))
    
    for msg_class, (msg_command_type_id, msg_command_id) in CLASS_TO_ID_MAP.items():
        if header_contents_tuple[0] == msg_command_type_id and header_contents_tuple[1] == msg_command_id:
            return msg_class, header_contents_tuple[2]
    
    raise ValueError("No known message type with command type id {{}} and command id {{}}".format(
                     header_contents_tuple[0], header_contents_tuple[1]))

"""

MESSAGE_FILE_IMPORT_TEMPLATE = r"from {package_name} import {module_name}"

NAME_TO_CLASS_MAP_ITEM_TEMPLATE = \
    """    "{class_name}": {module_name}.{class_name}"""

CLASS_TO_ID_PAIR_MAP_ITEM_TEMPLATE = \
    """    {module_name}.{class_name}: ({module_name}.{class_name}.COMMAND_TYPE_ID_FIXED_VALUE,
                                    {module_name}.{class_name}.COMMAND_ID_FIXED_VALUE)"""
