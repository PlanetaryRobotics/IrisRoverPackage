import argparse
import pathlib
import typing

from idl.compiler import idl_compiler
from idl.compiler import javascript_template
from idl.parser import parser
from idl.parser import idl_type


class JavascriptCompiler(idl_compiler.IdlCompiler):

    @classmethod
    def get_compiler_name(cls):
        return "javascript"

    @classmethod
    def get_compiler_description(cls):
        return "A compiler that generates javascript source files"

    @classmethod
    def add_compiler_specific_options(cls, parser: argparse.ArgumentParser):
        pass

    @classmethod
    def run(cls,
            input_dir_path: pathlib.Path,
            output_dir_path: pathlib.Path,
            force_overwrite: bool,
            args) -> getattr(typing, 'NoReturn', typing.Any):
        force_overwrite = args.force

        # Iterate over all of the input files, building a list command option strings
        all_command_option_strings = []
        for input_file in input_dir_path.iterdir():
            print(input_file)
            file_parser = parser.Parser(file_to_parse=input_file)
            file_type = file_parser.get_message_type()

            if isinstance(file_type, idl_type.MessageType):
                # Get all parameter strings
                parameter_strings = []
                for data_field_name, data_field_obj in file_type.data_fields_dict.items():
                    parameter_strings.append(javascript_template.COMMAND_OPTION_PARAM_TEMPLATE.format(
                        name=data_field_name,
                        units=data_field_obj.get_units(),
                        type=data_field_obj.get_type_string(),
                        default_value=data_field_obj.get_default_value()))

                all_parameters_string = ",\n".join(parameter_strings)

                # Create the command option string
                all_command_option_strings.append(javascript_template.COMMAND_OPTION_TEMPLATE.format(
                    class_name=file_type.get_name(),
                    class_description=file_type.get_description(),
                    command_type_id_value=file_type.get_command_type_id_value(),
                    command_id_value=file_type.get_command_id_value(),
                    command_parameters=all_parameters_string))

        all_command_options_str = ",\n".join(all_command_option_strings)

        # Create the full string for the "CommandOptions.js" file that will be created
        file_str = javascript_template.COMMAND_OPTIONS_FILE_TEMPLATE.format(command_options=all_command_options_str)

        # If overwrite is true and the output file already exists, then delete the existing file
        command_options_file_path = output_dir_path.joinpath("CommandOptions.js")
        if force_overwrite and command_options_file_path.exists():
            command_options_file_path.unlink()

        # Finally, write the output file
        command_options_file_path.write_text(file_str)
