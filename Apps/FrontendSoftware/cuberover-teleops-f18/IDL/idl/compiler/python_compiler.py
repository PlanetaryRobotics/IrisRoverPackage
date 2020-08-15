import argparse
import collections
import copy
import pathlib
import shutil
import typing

from idl.compiler import idl_compiler
from idl.compiler import compiler_utils
from idl.compiler import python_template
from idl.parser import parser
from idl.parser import idl_type
from idl.parser import idl_field

class PythonCompiler(idl_compiler.IdlCompiler):

    @classmethod
    def get_compiler_name(cls):
        return "python"

    @classmethod
    def get_compiler_description(cls):
        return "A compiler that generates python source files"

    @classmethod
    def add_compiler_specific_options(cls, parser: argparse.ArgumentParser):
        parser.add_argument("-p", "--package_name", type=str, required=False, default="idl_msgs",
                            help="The name of the package into which all generated files will be placed. In the output"
                                 "directory, a new subdirectory will be created with the specified package name. All "
                                 "generated python files will be placed inside of this new package directory, "
                                 "along with an empty __init__.py file. This argument controls the name of the "
                                 "package directory into which all generated message files will be placed. The "
                                 "default package name is 'idl_msgs'.")

    @classmethod
    def run(cls,
            input_dir_path: pathlib.Path,
            output_dir_path: pathlib.Path,
            force_overwrite: bool,
            args) -> typing.NoReturn:
        package_name = args.package_name
        force_overwrite = args.force

        # If overwrite is true and the package directory exists, then delete the already existing package directory
        package_directory_path = output_dir_path.joinpath(package_name)
        if force_overwrite and package_directory_path.exists():
            shutil.rmtree(str(package_directory_path))

        # Re-create the package directory
        package_directory_path.mkdir()

        # Iterate over all of the input files, generating a python source file for each
        all_message_types = {}
        for input_file in input_dir_path.iterdir():
            print(input_file)
            file_parser = parser.Parser(file_to_parse=input_file)
            file_type = file_parser.get_message_type()

            output_file_basename = compiler_utils.pascal_to_snake_case(file_type.get_name()) + ".py"
            output_file_path = package_directory_path.joinpath(output_file_basename)

            if isinstance(file_type, idl_type.EnumType):
                enum_members = '\n'.join([python_template.ENUM_MEMBER_TEMPLATE.format(member_name=name,
                                                                                      member_value=value.underlying_value)
                                          for name, value in file_type.member_dict.items()])
                enum_file_contents = python_template.ENUM_FILE_TEMPLATE.format(enum_name=file_type.get_name(),
                                                                               enum_members=enum_members)
                output_file_path.write_text(enum_file_contents)
            else:  # File has a message type
                all_message_types[file_type.get_name()] = file_type

                struct_all_fields, struct_fixed_fields = PythonCompiler.__gather_msg_fields(file_type)
                unfixed_fields = copy.deepcopy(struct_all_fields)
                for field_name in struct_fixed_fields.keys():
                    del unfixed_fields[field_name]

                # Create fixed value constant definition lines
                fixed_value_contant_lines = []
                for field_name, fixed_field_value in struct_fixed_fields.items():
                    fixed_value_contant_lines.append(python_template.FIXED_VALUE_TEMPLATE.format(
                        fixed_value_name=compiler_utils.pascal_to_screaming_snake_case(field_name),
                        fixed_value=fixed_field_value))

                fixed_value_constants = '\n'.join(fixed_value_contant_lines)

                # Create the constructor parameters (TODO: and defaults?)
                constructor_args = ', '.join(unfixed_fields.keys())

                # Create the initialization lines for all of the instance variables in the constructor
                constructor_inits = "\n".join([python_template.CONSTRUCTOR_INSTANCE_VAR_INIT.format(name=field_name)
                                               for field_name in unfixed_fields.keys()])

                # Create the list of arguments to the serialization call
                argument_string_list = []
                for field_name in struct_all_fields.keys():
                    if field_name in struct_fixed_fields:
                        argument_string_list.append(python_template.FIXED_VALUE_PARAM_TEMPLATE.format(
                            class_name=file_type.get_name(),
                            fixed_value_name=compiler_utils.pascal_to_screaming_snake_case(field_name)))
                    else:
                        argument_string_list.append(python_template.INSTANCE_PARAMETER_TEMPLATE.format(
                            name=field_name))

                arguments_string = ', '.join(argument_string_list)

                # Create the list of tuple indices to pass to the constructor after deserializing
                tuple_index_string_list = []
                for index, field_name in enumerate(struct_all_fields.keys()):
                    if field_name in unfixed_fields:
                        tuple_index_string_list.append(python_template.DESERIALIZED_TUPLE_INDEX_TEMPLATE.format(
                            index=index
                        ))

                construction_arg_string = ", ".join(tuple_index_string_list)

                struct_file_contents = python_template.MESSAGE_FILE_TEMPLATE.format(
                    additional_imports='',
                    class_name=file_type.get_name(),
                    struct_serialization_format_string=file_type.get_struct_format_string(),
                    fixed_value_constant_definitions=fixed_value_constants,
                    constructor_parameters_and_defaults=constructor_args,
                    constructor_initializations=constructor_inits,
                    ordered_list_of_fields_to_serialize=arguments_string,
                    constructor_arguments_from_unpacked_tuple=construction_arg_string)

                output_file_path.write_text(struct_file_contents)

        # Also add a "msg_utilities.py" file with utility functions that operate over all messages
        msg_file_import_strings = []
        msg_dict_item_strings = []
        for msg_type_name, msg_type in all_message_types.items():
            msg_module_name = compiler_utils.pascal_to_snake_case(msg_type_name)
            msg_file_import_strings.append(python_template.MESSAGE_FILE_IMPORT_TEMPLATE.format(
                package_name=package_name,
                module_name=msg_module_name))
            msg_dict_item_strings.append(python_template.CLASS_TO_ID_PAIR_MAP_ITEM_TEMPLATE.format(
                module_name=msg_module_name,
                class_name=msg_type_name))

        imports_string = "\n".join(msg_file_import_strings)
        dict_items_string = ",\n".join(msg_dict_item_strings)
        utils_file_contents = python_template.MESSAGE_UTILS_FILE_TEMPLATE.format(message_file_imports=imports_string,
                                                                                 class_to_id_items=dict_items_string)

        utils_file_path = package_directory_path.joinpath("msg_utilities.py")
        utils_file_path.write_text(utils_file_contents)

        # Lastly, add the __init__.py file to the package directory
        output_file_path = package_directory_path.joinpath("__init__.py")
        output_file_path.touch()

    @staticmethod
    def __gather_msg_fields(struct_type_object: idl_type.MessageType) -> typing.Tuple[collections.OrderedDict,
                                                                                      collections.OrderedDict]:
        all_msg_contents = copy.deepcopy(struct_type_object.all_message_contents_dict)

        all_fields = collections.OrderedDict()
        fixed_fields = collections.OrderedDict()
        for field_name, field_object in all_msg_contents.items():
            all_fields[field_name] = field_object
            all_fields.move_to_end(field_name)

            if field_object.has_fixed_value():
                fixed_fields[field_name] = field_object.get_fixed_value()
                fixed_fields.move_to_end(field_name)

        return all_fields, fixed_fields


