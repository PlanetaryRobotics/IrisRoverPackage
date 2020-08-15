import json
import pathlib
import typing

from idl.parser import context
from idl.parser import idl_type


INCLUDED_FILES_KEY = "include_files"
TYPE_KEY = "type"
ENUM_TYPE_KEY = "enum"
MESSAGE_TYPE_KEY = "message"


class Parser:
    def __init__(self, file_to_parse: pathlib.Path):
        """

        Args:
            file_to_parse:
        """
        self.file_path = file_to_parse
        self.file_base_name = file_to_parse.name
        self.msg_folder_path = file_to_parse.parent

    def get_message_type(self) -> typing.Union[idl_type.EnumType, idl_type.MessageType]:
        """Parses the top-level type from this object's file.

        Each message schema file should have a top-level dictionary, and that dictionary can contain only two key/value
        pairs.

        The first key/value pair is optional, and it is the "include_files" key. The value of this key should be a list
        strings, where each string is the name of a message schema file that is assumed to be in the same directory as
        the message schema file being parsed. (Currently, having message schemas that reference each other in different
        directories is not supported.) An enum member from an enumeration defined in the included files can be used as
        the fixed value or type of a message field in the file being parsed.

        The other key/value pair in the top-level dictionary is not optional, and it defines the "top-level type" of
        the file being parsed. This top-level type must be either an enum or message definition. For this key/value
        pair, the key should be the name of the message or enum, and the value should contain the message or enum
        specification. For the details of a message or enum specification, see the docstring of the idl_type.MessageType
        or idl_type.EnumType classes, respectively.

        Returns:
            idl_type.EnumType | idl_type.MessageType: Either an enum or message type object.
        """
        full_file_json_dict = json.loads(self.file_path.read_text())
        include_files_list = full_file_json_dict.pop(INCLUDED_FILES_KEY, dict())
        included_context = self._parse_included_files(include_files_list)
        assert len(full_file_json_dict.items()) == 1, \
            "The file {} has more than one entry other than an {} " \
            "entry in its top-level object".format(self.file_base_name, INCLUDED_FILES_KEY)

        msg_type_name, msg_type_details = next(iter(full_file_json_dict.items()))

        # Ensure we don't already have a type with the same name
        assert all(msg_type_name != type_in_context.get_name() for type_in_context in included_context.get_all_types()), \
            "Cannot define a new type with the name {} because a type with that name already exists " \
            "in the included context".format(msg_type_name)

        # Ensure we have a type k/v pair
        assert TYPE_KEY in msg_type_details, \
            "The top-level object {} is missing a {} key/value pair".format(msg_type_name, TYPE_KEY)

        # Determine the type
        type_value_str = msg_type_details[TYPE_KEY]
        if type_value_str == MESSAGE_TYPE_KEY:
            msg_type = idl_type.MessageType.parse_from_json(included_context, msg_type_name, msg_type_details)
        elif type_value_str == ENUM_TYPE_KEY:
            msg_type = idl_type.EnumType.parse_from_json(msg_type_name, msg_type_details)
        else:
            raise ValueError("The top-level object in a given file can only be an enum or a message")

        return msg_type

    def _parse_included_files(self, include_files_list: typing.List[str]) -> context.Context:
        """

        Args:
            include_files_list:

        Returns:

        """
        included_context = context.Context()
        for include_file_name in include_files_list:
            include_file_path = self.msg_folder_path.joinpath(include_file_name).resolve(strict=True)
            include_file_parser = Parser(include_file_path)
            include_file_type = include_file_parser.get_message_type()
            included_context.add_type(include_file_type)

        return included_context

