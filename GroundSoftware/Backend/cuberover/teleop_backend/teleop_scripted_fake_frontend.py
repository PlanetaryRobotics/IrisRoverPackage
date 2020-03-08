#!/usr/bin/env python3

# Make sure we have the root directory "teleop_backend" in the python path
import inspect
import os
import sys

__THIS_FILE_DIR = os.path.realpath(os.path.dirname(inspect.getfile(inspect.currentframe())))
__TELEOP_BACKEND_DIR = os.path.dirname(__THIS_FILE_DIR)
if __TELEOP_BACKEND_DIR not in sys.path:
    sys.path.insert(0, __TELEOP_BACKEND_DIR)

import argparse
import copy
import json
import pathlib
import time

from teleop_backend.database import mongo_db_collection


TYPE_KEY = "type"
OPCODE_KEY = "opcode"
SLEEP_NAME_VALUE = "sleep"
SLEEP_DURATION_KEY = "duration"
COMMAND_ARGS_KEY = "args"


def path_resolve_strict(path: pathlib.Path) -> pathlib.Path:
    if sys.version_info >= (3, 6):
        resolved_path = pathlib.Path(path).resolve(strict=True)
    else:
        resolved_path = pathlib.Path(path).resolve()
    return resolved_path


def get_path(path_string: str, *, require_exists: bool = False) -> pathlib.Path:
    expanded_path_str = os.path.abspath(os.path.expanduser(os.path.expandvars(path_string)))

    path = pathlib.Path(expanded_path_str)

    if require_exists:
        path = path_resolve_strict(pathlib.Path(path))

    return path


def main():
    parser = argparse.ArgumentParser(description="Reads a sequence of commands and sleeps, encoded in json, "
                                                 "from a file, then push the commands to the database")
    parser.add_argument("command_file", type=str,
                        help="The path of the command sequence json file. This json file is expected "
                             "to contain an array of objects, where each object specifies either a command or a "
                             "sleep. For a command, the object must have a 'type' key with the string value 'command',"
                             " an 'opcode' key with an integer value, and an "
                             "'args' key with a object value. The 'args' value should contain the argument names (as "
                             "the keys) and argument values (as the values) of all arguments for that command. The "
                             "name of the command and name of the arguments much exactly match (including "
                             "capitalization) the corresponding names in the IDL message definition. For a sleep, "
                             "the object must have a 'type' key with the value 'sleep', and a 'duration' key with a "
                             "floating point or integer number value. The value is the sleep duration in seconds.")

    args = parser.parse_args()
    command_file_path = get_path(args.command_file, require_exists=True)
    command_file_json_list = json.loads(command_file_path.read_text())

    assert isinstance(command_file_json_list, list), \
        "The top-level structure in the command file is not a list"

    db = mongo_db_collection.MongoDbCollection()

    for index, command_or_sleep_dict in enumerate(command_file_json_list):
        assert TYPE_KEY in command_or_sleep_dict, \
            "Object {} in the command file does not contain a {} key".format(index + 1, TYPE_KEY)

        if command_or_sleep_dict[TYPE_KEY] == SLEEP_NAME_VALUE:
            # This is a sleep directive dictionary
            assert SLEEP_DURATION_KEY in command_or_sleep_dict, \
                "Object {} in the command file is a sleep object but does not contain a {} key".format(
                    index + 1, SLEEP_DURATION_KEY)

            sleep_duration = command_or_sleep_dict[SLEEP_DURATION_KEY]

            assert isinstance(sleep_duration, float) or isinstance(sleep_duration, int), \
                "Object {} in the command file is a sleep object with a non-numeric duration value ({})".format(
                    index + 1, command_or_sleep_dict[SLEEP_DURATION_KEY])

            print("[{}]: Now sleeping for {} seconds".format(time.time(), sleep_duration))
            time.sleep(sleep_duration)

        else:
            # This is a command directive
            assert command_or_sleep_dict[TYPE_KEY] == "command", \
                "Object {} in the command file has a type (\"{}\") other than \"sleep\" or \"command\"".format(
                    index + 1, command_or_sleep_dict[TYPE_KEY])

            # Make a copy since we may be modifying it
            command_dict = copy.deepcopy(command_or_sleep_dict)

            assert COMMAND_ARGS_KEY in command_dict, \
                "Object {} in the command file is a command object but does not contain an {} key".format(
                    index + 1, COMMAND_ARGS_KEY)

            args_value = command_dict[COMMAND_ARGS_KEY]

            assert isinstance(args_value, dict), \
                "Object {} in the command file is a command object with a non-object args value ({})".format(
                    index + 1, args_value)

            assert OPCODE_KEY in command_dict, \
                "Object {} in the command file is a command object but does not contain an {} key".format(
                    index + 1, OPCODE_KEY)

            opcode_value = command_dict[OPCODE_KEY]

            assert isinstance(opcode_value, int), \
                "Object {} in the command file is a command object with a non-integer opcode value ({})".format(
                    index + 1, args_value)

            # Make sure we have a lookup ID k/v pair. The value doesn't matter since it will be overwritten, but I make
            # it accurate (i.e. db.count() + 1 is what it will be overwritten to) so that it is correct when we print
            # out the command we wrote to the database
            command_dict['lookupID'] = db.count() + 1

            # Write the command to the database
            db.write(command_dict)
            print("[{}]: Just wrote cmd to db: {}".format(time.time(), command_dict))


if __name__ == "__main__":
    main()
