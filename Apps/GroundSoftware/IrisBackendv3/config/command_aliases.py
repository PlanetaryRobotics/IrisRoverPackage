"""
Tools for processing a Command Aliases file.

Last Update: 03/10/2023
"""
# Activate postponed annotations (for using classes as return type in their own methods)
from __future__ import annotations

import os.path
from importlib.machinery import SourceFileLoader
from functools import wraps
from dataclasses import dataclass

from typing import Any, Optional, Final, Dict, Tuple, TypeAlias, Type, Callable, TypeVar, cast
from types import ModuleType
from collections import OrderedDict

import typeguard

from IrisBackendv3.codec.magic import Magic
from IrisBackendv3.codec.metadata import DataPathway
from IrisBackendv3.codec.payload import CommandPayload
from IrisBackendv3.data_standards.data_standards import DataStandards

# Default directory containing all the `command_aliases` files (w.r.t.
# `GroundSoftware`):
DEFAULT_COMMAND_ALIASES_DIR: Final[str] = "./config/command_aliases"


@dataclass
class PreparedCommand:
    """Pre-prepared command."""
    name: str
    args: OrderedDict
    pathway: DataPathway
    magic: Magic
    comment: str = ""

    def __post_init__(self) -> None:
        # Check the types of all fields:
        typeguard.check_type('name', self.name, str)
        typeguard.check_type('args', self.args, OrderedDict)
        typeguard.check_type('pathway', self.pathway, DataPathway)
        typeguard.check_type('magic', self.magic, Magic)
        typeguard.check_type('comment', self.comment, str)


CommandAliases: TypeAlias = Dict[str, PreparedCommand]

CommandAliasesBuilderType: TypeAlias = \
    Callable[[DataStandards], CommandAliases]


def CommandAliasesBuilder(func: CommandAliasesBuilderType) -> CommandAliasesBuilderType:
    """Decorator that ensures the type signature of a function that returns
    `CommandAliases`. Use case:
    ```py
    @CommandAliasesBuilder
    def ALIASES(standards: DataStandards) -> CommandAliases:
        return {
            'command-1': PreparedCommand(
                # ...
            )
            # etc
        }
    ```
    """
    @wraps(func)
    def wrapper(*args: Any, **kwargs: Any) -> Any:
        return func(*args, **kwargs)
    return cast(CommandAliasesBuilderType, wrapper)


class CommandAliasesTable:
    @dataclass
    class Entry:
        prepared_cmd: PreparedCommand
        compiled_cmd: CommandPayload

    _table: Dict[str, CommandAliasesTable.Entry]

    def __init__(
        self,
        table: Dict[str, CommandAliasesTable.Entry]
    ) -> None:
        self._table = table

    @classmethod
    def from_aliases(
        cls: Type[CommandAliasesTable],
        aliases: CommandAliases
    ) -> CommandAliasesTable:
        # validate
        # build
        # init
        raise NotImplementedError()

    @classmethod
    def load_from_file(
        cls: Type[CommandAliasesTable],
        file_name: str,
        file_dir: str = "./config/command_aliases/"
    ) -> CommandAliasesTable:
        # build path to module
        # import it (without doubling up in sys.modules?...)
        # validate
        # build
        raise NotImplementedError()


def validate_and_build_commands(yaml_dict):
    """Validates and (pre)builds all commands, both to cache and to validate
    them at launch.
    """
    # ! Check types (explicitly of all PreparedCommands args)

    raise NotImplementedError()


def import_command_aliases_file(
    file_name: str,
    command_aliases_dir: str = DEFAULT_COMMAND_ALIASES_DIR
) -> ModuleType:
    """
    Imports the `command_aliases` file called `file_name` from 
    `command_aliases_dir` (w.r.t. the `GroundSoftware` directory).

    NOTE: The only requirement for `command_aliases` files is that they are
    python modules containing a method called `ALIASES` with the signature
    `CommandAliasesBuilderType`.
    """
    # Add the `py` ending if not there:
    if not file_name.endswith('.py'):
        file_name = file_name + '.py'

    file_path = os.path.join(command_aliases_dir, file_name)
    sfl = SourceFileLoader(file_name[:-3], file_path)
    return sfl.load_module()


# # input -> prepared command -> string -> input: edit string -> Command -> Send
# f"{COMMAND}: {module}.{command}[{name_1}: {arg_1}, ...] -> {WIRED} -> {XCVR:ANY}"
# # (invert cursor character)
# # place comment underneath (until edit?)

# # ... or truncate comments in table but always show comment for top command?
# #! ^^


# def get_command(alias: str, params: Optional[Any] = None) -> PreparedCommandType:
#     """
#     Grabs command package data (pathway, type, and payload) given a short-hand
#     name (`alias`) and, optionally, a `param` that gets inserted as a command
#     argument where necessary.

#     See docstring for `Parameter` for more details about how `params` pasting works.
#     """
#     pathway, magic, command_name, kwargs, telem_pathway = prepared_commands[alias]

#     # Paste the given parameters anywhere we're told:
#     if not isinstance(params, list):
#         params = [params]
#     param_idx = 0
#     for arg_name, arg_val in kwargs.items():
#         if arg_val == Parameter.PASTE:
#             kwargs[arg_name] = params[param_idx]
#             param_idx = (param_idx + 1) % len(params)

#     return (pathway, magic, command_name, kwargs, telem_pathway)
