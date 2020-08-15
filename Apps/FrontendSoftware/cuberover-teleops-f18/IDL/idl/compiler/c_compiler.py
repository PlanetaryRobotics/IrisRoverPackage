import argparse
import pathlib
import typing

from idl.compiler import compiler_utils
from idl.compiler import idl_compiler
from idl.compiler import c_template
from idl.parser import parser
from idl.parser import idl_type

PRIMITIVE_TYPE_TO_C_TYPE_STRING_MAP = {
    idl_type.PrimitiveTypeCtype.UINT8: "uint8_t",
    idl_type.PrimitiveTypeCtype.UINT16: "uint16_t",
    idl_type.PrimitiveTypeCtype.UINT32: "uint32_t",
    idl_type.PrimitiveTypeCtype.UINT64: "uint64_t",
    idl_type.PrimitiveTypeCtype.INT8: "int8_t",
    idl_type.PrimitiveTypeCtype.INT16: "int16_t",
    idl_type.PrimitiveTypeCtype.INT32: "int32_t",
    idl_type.PrimitiveTypeCtype.INT64: "int64_t",
    idl_type.PrimitiveTypeCtype.FLOAT: "float",
    idl_type.PrimitiveTypeCtype.DOUBLE: "double",
    idl_type.PrimitiveTypeCtype.BOOL: "int8_t"
}

PRIMITIVE_TYPE_TO_FIELD_TEMPLATES_MAP = {
    idl_type.PrimitiveTypeCtype.UINT8: (c_template.SERIALIZE_UINT8_OR_INT8_FIELD_TEMPLATE,
                                        c_template.DESERIALIZE_UINT8_OR_INT8_FIELD_TEMPLATE),
    idl_type.PrimitiveTypeCtype.UINT16: (c_template.SERIALIZE_UINT16_OR_INT16_FIELD_TEMPLATE,
                                         c_template.DESERIALIZE_UINT16_OR_INT16_FIELD_TEMPLATE),
    idl_type.PrimitiveTypeCtype.UINT32: (c_template.SERIALIZE_UINT32_OR_INT32_FIELD_TEMPLATE,
                                         c_template.DESERIALIZE_UINT32_OR_INT32_FIELD_TEMPLATE),
    idl_type.PrimitiveTypeCtype.UINT64: (c_template.SERIALIZE_UINT64_OR_INT64_FIELD_TEMPLATE,
                                         c_template.DESERIALIZE_UINT64_OR_INT64_FIELD_TEMPLATE),
    idl_type.PrimitiveTypeCtype.INT8: (c_template.SERIALIZE_UINT8_OR_INT8_FIELD_TEMPLATE,
                                       c_template.DESERIALIZE_UINT8_OR_INT8_FIELD_TEMPLATE),
    idl_type.PrimitiveTypeCtype.INT16: (c_template.SERIALIZE_UINT16_OR_INT16_FIELD_TEMPLATE,
                                        c_template.DESERIALIZE_UINT16_OR_INT16_FIELD_TEMPLATE),
    idl_type.PrimitiveTypeCtype.INT32: (c_template.SERIALIZE_UINT32_OR_INT32_FIELD_TEMPLATE,
                                        c_template.DESERIALIZE_UINT32_OR_INT32_FIELD_TEMPLATE),
    idl_type.PrimitiveTypeCtype.INT64: (c_template.SERIALIZE_UINT64_OR_INT64_FIELD_TEMPLATE,
                                        c_template.DESERIALIZE_UINT64_OR_INT64_FIELD_TEMPLATE),
    idl_type.PrimitiveTypeCtype.FLOAT: (c_template.SERIALIZE_FLOAT_FIELD_TEMPLATE,
                                        c_template.DESERIALIZE_FLOAT_FIELD_TEMPLATE),
    idl_type.PrimitiveTypeCtype.DOUBLE: (c_template.SERIALIZE_DOUBLE_FIELD_TEMPLATE,
                                         c_template.DESERIALIZE_DOUBLE_FIELD_TEMPLATE),
    idl_type.PrimitiveTypeCtype.BOOL: (c_template.SERIALIZE_UINT8_OR_INT8_FIELD_TEMPLATE,
                                       c_template.DESERIALIZE_UINT8_OR_INT8_FIELD_TEMPLATE)
}


class _MessageData(typing.NamedTuple):
    # For the source file
    serialize_body_function_declaration: str
    deserialize_body_function_declaration: str
    valid_msg_id_case: str
    get_packed_msg_size_case: str
    serialize_body_case: str
    deserialize_body_case: str
    serialize_body_function_definition: str
    deserialize_body_function_definition: str

    # For the header file
    message_id_member: str
    message_body_declaration: str
    message_body_member: str
    body_packed_size_enum_member: str
    msg_packed_size_enum_member: str
    msg_name_screaming_snake: str
    body_packed_size: int


class CCompiler(idl_compiler.IdlCompiler):

    @classmethod
    def get_compiler_name(cls):
        return "c"

    @classmethod
    def get_compiler_description(cls):
        return "A compiler that generates C files"

    @classmethod
    def add_compiler_specific_options(cls, parser: argparse.ArgumentParser):
        pass

    @classmethod
    def run(cls,
            input_dir_path: pathlib.Path,
            output_dir_path: pathlib.Path,
            force_overwrite: bool,
            args) -> typing.NoReturn:
        force_overwrite = args.force

        # Iterate over all of the input files, building a list the message types for all message files
        all_message_file_types = []

        for input_file in input_dir_path.iterdir():
            print(input_file)
            file_parser = parser.Parser(file_to_parse=input_file)
            file_type = file_parser.get_message_type()

            if isinstance(file_type, idl_type.MessageType):
                all_message_file_types.append(file_type)

        # Sort the messages by command type id and command id
        all_message_file_types.sort(key=lambda msg_type: (msg_type.get_command_type_id_value(),
                                                          msg_type.get_command_id_value()))

        # Build a list of the filled-in template strings for all of the messages
        all_message_data = []
        largest_message_packed_size = 0
        largest_message_name_screaming_snake = "; UNKNOWN"

        for file_type in all_message_file_types:
            this_msg_data = CCompiler.__build_message_data(file_type)
            all_message_data.append(this_msg_data)

            if this_msg_data.body_packed_size > largest_message_packed_size:
                largest_message_packed_size = this_msg_data.body_packed_size
                largest_message_name_screaming_snake = this_msg_data.msg_name_screaming_snake

        # Iterate over all of the input files, building a

        source_file_string = CCompiler.__build_source_file_string(all_message_data)
        header_file_string = CCompiler.__build_header_file_string(all_message_data,
                                                                  largest_message_name_screaming_snake)

        # If overwrite is true and the output file already exists, then delete the existing file
        teleop_message_source_file_path = output_dir_path.joinpath("TeleopMessages.c")
        teleop_message_header_file_path = output_dir_path.joinpath("TeleopMessages.h")
        if force_overwrite:
            if teleop_message_source_file_path.exists():
                teleop_message_source_file_path.unlink()

            if teleop_message_header_file_path.exists():
                teleop_message_header_file_path.unlink()

        # Finally, write the output file
        teleop_message_source_file_path.write_text(source_file_string)
        teleop_message_header_file_path.write_text(header_file_string)

    @staticmethod
    def __build_message_data(file_type: idl_type.MessageType):
        message_name_pascal_case = file_type.get_name()
        message_name_screaming_snake_case = compiler_utils.pascal_to_screaming_snake_case(message_name_pascal_case)
        message_name_camel_case = compiler_utils.pascal_to_camel_case(message_name_pascal_case)
        message_name_pretty = compiler_utils.pascal_to_pretty_case(message_name_pascal_case)

        # Get all of the strings needed for the source file
        serialize_body_function_declaration = c_template.SERIALIZE_BODY_FUNCTION_DECLARATION_TEMPLATE.format(
            message_name_pretty=message_name_pretty,
            message_name_screaming_snake=message_name_screaming_snake_case,
            message_name_pascal_case=message_name_pascal_case
        )

        deserialize_body_function_declaration = c_template.DESERIALIZE_BODY_FUNCTION_DECLARATION_TEMPLATE.format(
            message_name_pretty=message_name_pretty,
            message_name_screaming_snake=message_name_screaming_snake_case,
            message_name_pascal_case=message_name_pascal_case
        )

        valid_msg_id_case = c_template.VALID_MSG_ID_CASE_TEMPLATE.format(
            message_name_screaming_snake=message_name_screaming_snake_case
        )

        get_packed_msg_size_case = c_template.GET_PACKED_MSG_SIZE_CASE_TEMPLATE.format(
            message_name_screaming_snake=message_name_screaming_snake_case
        )

        serialize_body_case = c_template.SERIALIZE_BODY_CASE_TEMPLATE.format(
            message_name_screaming_snake=message_name_screaming_snake_case,
            message_name_pascal_case=message_name_pascal_case,
            message_name_camel_case=message_name_camel_case
        )

        deserialize_body_case = c_template.DESERIALIZE_BODY_CASE_TEMPLATE.format(
            message_name_screaming_snake=message_name_screaming_snake_case,
            message_name_pascal_case=message_name_pascal_case,
            message_name_camel_case=message_name_camel_case
        )

        serialize_body_function_definition, deserialize_body_function_definition = \
            CCompiler.__build_ser_and_deser_function_definitions(file_type,
                                                                 message_name_pascal_case,
                                                                 message_name_screaming_snake_case)

        # Get all of the strings needed for the header file
        message_id_member = c_template.MESSAGE_ID_MEMBER_TEMPLATE.format(
            message_name_screaming_snake=message_name_screaming_snake_case,
            command_type_id_int=file_type.get_command_type_id_value(),
            command_id_int=file_type.get_command_id_value()
        )

        message_body_declaration, body_packed_size = \
            CCompiler.__build_message_body_declaration_and_packed_size(file_type, message_name_pascal_case)

        message_body_member = c_template.MESSAGE_BODY_MEMBER_TEMPLATE.format(
            message_name_pascal_case=message_name_pascal_case,
            message_name_camel_case=message_name_camel_case
        )

        body_packed_size_enum_member = c_template.BODY_PACKED_SIZE_ENUM_MEMBER_TEMPLATE.format(
            message_name_pascal_case=message_name_pascal_case,
            message_name_screaming_snake=message_name_screaming_snake_case,
            body_packed_size=body_packed_size
        )

        msg_packed_size_enum_member = c_template.MSG_PACKED_SIZE_ENUM_MEMBER_TEMPLATE.format(
            message_name_pretty=message_name_pretty,
            message_name_screaming_snake=message_name_screaming_snake_case
        )

        return _MessageData(serialize_body_function_declaration=serialize_body_function_declaration,
                            deserialize_body_function_declaration=deserialize_body_function_declaration,
                            valid_msg_id_case=valid_msg_id_case,
                            get_packed_msg_size_case=get_packed_msg_size_case,
                            serialize_body_case=serialize_body_case,
                            deserialize_body_case=deserialize_body_case,
                            serialize_body_function_definition=serialize_body_function_definition,
                            deserialize_body_function_definition=deserialize_body_function_definition,
                            message_id_member=message_id_member,
                            message_body_declaration=message_body_declaration,
                            message_body_member=message_body_member,
                            body_packed_size_enum_member=body_packed_size_enum_member,
                            msg_packed_size_enum_member=msg_packed_size_enum_member,
                            msg_name_screaming_snake=message_name_screaming_snake_case,
                            body_packed_size=body_packed_size)

    @staticmethod
    def __build_ser_and_deser_function_definitions(file_type: idl_type.MessageType,
                                                   message_name_pascal_case: str,
                                                   message_name_screaming_snake_case: str) -> typing.Tuple[str, str]:
        all_parameters_ser_strings = []
        all_parameters_deser_strings = []

        if len(file_type.data_fields_dict) > 0:
            for data_field_name, data_field_obj in file_type.data_fields_dict.items():
                field_type = data_field_obj.get_type()  # type: idl_type.IdlType

                if isinstance(field_type, idl_type.EnumType):
                    ser_template, deser_template = PRIMITIVE_TYPE_TO_FIELD_TEMPLATES_MAP[field_type.underlying_type]
                else:
                    assert isinstance(field_type, idl_type.PrimitiveType), \
                        "A message member ({} of {}) is neither an enum type or a primitive type".format(
                            file_type.get_name(), data_field_name)

                    ser_template, deser_template = PRIMITIVE_TYPE_TO_FIELD_TEMPLATES_MAP[field_type.primitive_ctype]

                field_name_camel_case = compiler_utils.pascal_to_camel_case(data_field_name)
                all_parameters_ser_strings.append(ser_template.format(field_name_camel_case=field_name_camel_case))
                all_parameters_deser_strings.append(deser_template.format(field_name_camel_case=field_name_camel_case))

            code_to_serialize_fields = "\n\n".join(all_parameters_ser_strings)
            code_to_deserialize_fields = "\n\n".join(all_parameters_deser_strings)

            serialize_body_function_definition = c_template.SERIALIZE_BODY_FUNCTION_DEFINITION_TEMPLATE.format(
                message_name_pascal_case=message_name_pascal_case,
                message_name_screaming_snake=message_name_screaming_snake_case,
                code_to_serialize_fields=code_to_serialize_fields
            )

            deserialize_body_function_definition = c_template.DESERIALIZE_BODY_FUNCTION_DEFINITION_TEMPLATE.format(
                message_name_pascal_case=message_name_pascal_case,
                message_name_screaming_snake=message_name_screaming_snake_case,
                code_to_deserialize_fields=code_to_deserialize_fields
            )
        else:
            serialize_body_function_definition = c_template.SERIALIZE_EMPTY_BODY_FUNCTION_DEFINITION_TEMPLATE.format(
                message_name_pascal_case=message_name_pascal_case
            )

            deserialize_body_function_definition = \
                c_template.DESERIALIZE_EMPTY_BODY_FUNCTION_DEFINITION_TEMPLATE.format(
                    message_name_pascal_case=message_name_pascal_case
                )

        return serialize_body_function_definition, deserialize_body_function_definition

    @staticmethod
    def __build_message_body_declaration_and_packed_size(file_type: idl_type.MessageType,
                                                         message_name_pascal_case: str) -> typing.Tuple[str, int]:
        all_body_field_strings = []
        body_packed_size = 0
        for data_field_name, data_field_obj in file_type.data_fields_dict.items():
            field_type = data_field_obj.get_type()  # type: idl_type.IdlType

            if isinstance(field_type, idl_type.EnumType):
                c_type_string = PRIMITIVE_TYPE_TO_C_TYPE_STRING_MAP[field_type.underlying_type]
            else:
                assert isinstance(field_type, idl_type.PrimitiveType), \
                    "A message member ({} of {}) is neither an enum type or a primitive type".format(
                        file_type.get_name(), data_field_name)

                c_type_string = PRIMITIVE_TYPE_TO_C_TYPE_STRING_MAP[field_type.primitive_ctype]

            field_name_camel_case = compiler_utils.pascal_to_camel_case(data_field_name)
            all_body_field_strings.append(c_template.MESSAGE_BODY_FIELD_TEMPLATE.format(
                type=c_type_string,
                name=field_name_camel_case
            ))

            body_packed_size += data_field_obj.size_in_bytes()

        message_body_fields = "\n".join(all_body_field_strings)

        message_body_declaration = c_template.MESSAGE_BODY_DECLARATION_TEMPLATE.format(
            message_name_pascal_case=message_name_pascal_case,
            message_body_fields=message_body_fields
        )

        return message_body_declaration, body_packed_size

    @staticmethod
    def __build_source_file_string(all_message_data: typing.Sequence[_MessageData]) -> str:
        serialize_body_function_declarations = "\n\n".join((data.serialize_body_function_declaration
                                                            for data in all_message_data))

        deserialize_body_function_declarations = "\n\n".join((data.deserialize_body_function_declaration
                                                              for data in all_message_data))

        valid_msg_id_cases = "\n".join((data.valid_msg_id_case
                                        for data in all_message_data))

        get_packed_msg_size_cases = "\n\n".join((data.get_packed_msg_size_case
                                                 for data in all_message_data))

        serialize_body_cases = "\n\n".join((data.serialize_body_case
                                            for data in all_message_data))

        deserialize_body_cases = "\n\n".join((data.deserialize_body_case
                                              for data in all_message_data))

        serialize_body_function_definitions = "\n\n\n".join((data.serialize_body_function_definition
                                                             for data in all_message_data))

        deserialize_body_function_definitions = "\n\n\n".join((data.deserialize_body_function_definition
                                                               for data in all_message_data))

        return c_template.TELEOP_MESSAGES_SOURCE_FILE_TEMPLATE.format(
            serialize_body_function_declarations=serialize_body_function_declarations,
            deserialize_body_function_declarations=deserialize_body_function_declarations,
            valid_msg_id_cases=valid_msg_id_cases,
            get_packed_msg_size_cases=get_packed_msg_size_cases,
            serialize_body_cases=serialize_body_cases,
            deserialize_body_cases=deserialize_body_cases,
            serialize_body_function_definitions=serialize_body_function_definitions,
            deserialize_body_function_definitions=deserialize_body_function_definitions,
        )

    @staticmethod
    def __build_header_file_string(all_message_data: typing.Sequence[_MessageData],
                                   largest_message_name_screaming_snake: str) -> str:
        message_id_members = ",\n".join((data.message_id_member
                                         for data in all_message_data))

        message_body_declarations = "\n\n".join((data.message_body_declaration
                                                 for data in all_message_data))

        message_body_members = "\n".join((data.message_body_member
                                          for data in all_message_data))

        body_packed_size_members = ",\n\n".join((data.body_packed_size_enum_member
                                                 for data in all_message_data))

        msg_packed_size_members = ",\n\n".join((data.msg_packed_size_enum_member
                                                for data in all_message_data))

        return c_template.TELEOP_MESSAGES_HEADER_FILE_TEMPLATE.format(
            message_id_members=message_id_members,
            message_body_declarations=message_body_declarations,
            message_body_members=message_body_members,
            body_packed_size_members=body_packed_size_members,
            msg_packed_size_members=msg_packed_size_members,
            largest_msg_screaming_snake=largest_message_name_screaming_snake
        )


__all__ = [CCompiler]
