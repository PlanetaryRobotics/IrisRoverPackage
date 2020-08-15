import sys
assert sys.version_info >= (3, 5), "Python 3.5 or newer is required"

# Make sure we have the root directory "IDL" in the python path
import inspect
import os
THIS_FILE_DIR = os.path.realpath(os.path.dirname(inspect.getfile(inspect.currentframe())))
IDL_DIR = os.path.dirname(THIS_FILE_DIR)
sys.path.insert(0, IDL_DIR)

import argparse
import pathlib

from idl.compiler import idl_compiler
from idl.compiler import python_compiler
from idl.compiler import javascript_compiler
from idl.compiler import c_compiler

def add_all_compiler_subparsers(subparsers: argparse._SubParsersAction):
    for specific_compiler_cls in idl_compiler.IdlCompiler.__subclasses__():
        specific_compiler_cls.add_compiler_subparser(subparsers)


def path_resolve_strict(path: pathlib.Path) -> pathlib.Path:
    if sys.version_info >= (3, 6):
        resolved_path = pathlib.Path(path).resolve(strict=True)
    else:
        resolved_path = pathlib.Path(path).resolve()
    return resolved_path


def get_path(path_string: str, *, require_exists: bool=False) -> pathlib.Path:
    expanded_path_str = os.path.abspath(os.path.expanduser(os.path.expandvars(path_string)))

    path = pathlib.Path(expanded_path_str)

    if require_exists:
        path = path_resolve_strict(pathlib.Path(path))

    return path


def main():
    parser = argparse.ArgumentParser(description="Compile a directory of JSON message definitions into output source "
                                                 "files")
    parser.add_argument("-i", "--input_directory", type=str, required=True,
                        help="The directory containing all messages to be compiled.")
    parser.add_argument("-o", "--output_directory", type=str, required=True,
                        help="The directory to which the generated python package will be output. This directory "
                             "must already exist.")
    parser.add_argument("-f", "--force", action="store_true", required=False, default=False,
                        help="If this option is given, this compiler will overwrite any existing output files")

    subparsers = parser.add_subparsers(title="Language Compilers", description="Valid language compilers")
    add_all_compiler_subparsers(subparsers)

    args = parser.parse_args()

    # Grab the input and output directories and make sure that they exist
    input_directory_path = get_path(args.input_directory, require_exists=True)
    output_directory_path = get_path(args.output_directory, require_exists=True)
    assert input_directory_path.is_dir(), "{} is not a directory".format(str(input_directory_path))
    assert output_directory_path.is_dir(), "{} is not a directory".format(str(output_directory_path))

    force_overwrite = args.force

    args.func(input_directory_path, output_directory_path, force_overwrite, args)


if __name__ == "__main__":
    main()