#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Containers for Flight Software Modules/Components and their Relevant Fields
(Commands, Telemetry, Events), alongside relevant ground-software metadata
(where applicable).

@author: Connor W. Colombo (CMU)
@last-updated: 01/18/2021
"""
# Activate postponed annotations (for using classes as return type in their own methods):
from __future__ import annotations

from typing import List, Dict, Any, Union, cast
from dataclasses import dataclass

import json

from IrisBackendv3.utils import NameIdDict
from IrisBackendv3.utils.basic import full_dict_spec_check
from .fsw_data_type import FswDataType


class GswMetadataContainer(object):
    """
    Base class for any data container which will include GSW-specific JSON metadata parsed from XML CDATA.

    Supports automatic parsing and caching of JSON strings to make loading data
    from a string easier.
    """

    __slots__: List[str] = [
        # Internal GSW-specific Metadata JSON String (from FPrime XML Comment):
        '_metadata_json_str',
        # Internal GSW-specific Metadata Dictionary (from JSON String):
        '_metadata_dict'
    ]

    _metadata_json_str: str
    _metadata_dict: Dict[str, Any]

    def __init__(self, metadata_json_str: str) -> None:
        self.metadata_json_str = metadata_json_str

    @property
    def metadata(self) -> Dict[str, Any]:
        """Return cached conversion of metadata json string to a dictionary."""
        return self._metadata_dict

    @metadata.setter
    def metadata(self, metadata_dict: Dict[str, Any]) -> None:
        """Useful if the data has already been deserialized externally."""
        # Store the data:
        self._metadata_dict = metadata_dict
        # Convert back to json string for external consistency's sake:
        self._metadata_json_str = json.dumps(self._metadata_dict)

    @property
    def metadata_json_str(self) -> str:
        return self._metadata_json_str

    @metadata_json_str.setter
    def metadata_json_str(self, metadata_json_str: str) -> None:
        # Store the value:
        self._metadata_json_str = metadata_json_str
        # Format string to reflect appropriate empty JSON:
        if self._metadata_json_str == "":
            self._metadata_json_str = r"{}"
        # Reconvert metadata json string to a dictionary and cache:
        self._metadata_dict = json.loads(self._metadata_json_str)


class Module(object):
    """Container for Data Standards for a Module in the FSW."""

    __slots__: List[str] = [
        'name',  # - Name of the Module
        'ID',  # - Highest 1B of the Command Opcodes for this Module (its ID)
        'commands',  # - NameIdDict mapping command names and opcodes to Command data
        'telemetry',  # - NameIdDict mapping telemetry names and IDs to Telemetry data
        'events'  # - NameIdDict mapping event names and IDs to Event data
    ]

    def __init__(self,
                 name: str,
                 ID: int,
                 commands: NameIdDict[Command],
                 telemetry: NameIdDict[Telemetry],
                 events: NameIdDict[Event],
                 ) -> None:
        self.name = name
        self.ID = ID
        self.commands = commands
        self.telemetry = telemetry
        self.events = events

    def __eq__(self, other) -> bool:
        if not isinstance(other, Module):
            return False

        return (
            self.name == other.name
            and self.ID == other.ID
            and self.commands == other.commands
            and self.telemetry == other.telemetry
            and self.events == other.events
        )

    def __repr__(self) -> str:
        return f"Module[{self.ID}]::{self.name}"

    def __getstate__(self) -> Dict[str, Union[str, int, NameIdDict]]:
        return {
            'name': self.name,
            'ID': self.ID,
            'commands': self.commands,
            'telemetry': self.telemetry,
            'events': self.events
        }

    def __setstate__(self, data: Dict[str, Union[str, int, NameIdDict]]) -> None:
        full_dict_spec_check(
            data,
            {
                'name': str,
                'ID': int,
                'commands': NameIdDict[Command],
                'telemetry': NameIdDict[Telemetry],
                'events': NameIdDict[Event]
            },
            name='data'
        )
        self.name = cast(str, data['name'])
        self.ID = cast(int, data['ID'])
        self.commands = cast(NameIdDict[Command], data['commands'])
        self.telemetry = cast(NameIdDict[Telemetry], data['telemetry'])
        self.events = cast(NameIdDict[Event], data['events'])


class EnumItem(object):
    """Container for FPrime Enumeration Item (inside an Argument) and associated."""

    __slots__: List[str] = ['name', 'value', 'comment']
    name: str
    value: int
    comment: str

    def __init__(self,
                 name: str,
                 value: int,
                 comment: str = ""
                 ):
        self.name = name
        self.value = value
        self.comment = comment

    def __eq__(self, other) -> bool:
        if not isinstance(other, EnumItem):
            return False

        return (
            self.name == other.name
            and self.value == other.value
            and self.comment == other.comment
        )

    def __repr__(self) -> str:
        return f"{self.name}={self.value}"

    def __getstate__(self) -> Dict[str, Union[str, int]]:
        return {
            'name': self.name,
            'value': self.value,
            'comment': self.comment
        }

    def __setstate__(self, data: Dict[str, Union[str, int]]) -> None:
        full_dict_spec_check(
            data,
            {
                'name': str,
                'value': int,
                'comment': str
            },
            name='data'
        )
        self.name = cast(str, data['name'])
        self.value = cast(int, data['value'])
        self.comment = cast(str, data['comment'])


class Argument(GswMetadataContainer):
    """Container for FPrime (Command or Event) Argument and associated Ground Software Metadata"""

    __slots__: List[str] = ['fprime_name', 'datatype', 'enum']
    fprime_name: str
    datatype: FswDataType
    enum: List[EnumItem]

    def __init__(self,
                 name: str,
                 datatype: FswDataType,
                 enum: List[EnumItem] = [],
                 metadata_json_str: str = ""
                 ):
        self.fprime_name = name
        self.datatype = datatype

        if enum != [] and datatype != FswDataType.ENUM:
            raise ValueError(
                f"Datatype is given as {datatype} but an enum list is also provided. "
                f"An Argument must have type {FswDataType.ENUM} to have an enum list."
            )
        self.enum = enum

        super().__init__(metadata_json_str)

    def __eq__(self, other) -> bool:
        if not isinstance(other, Argument):
            return False

        return (
            self.fprime_name == other.fprime_name
            and self.datatype == other.datatype
            and self.enum == other.enum
            and self.metadata == other.metadata
        )

    def __repr__(self) -> str:
        return f"{self.fprime_name}: {self.datatype.value}"

    def __getstate__(self) -> Dict[str, Union[str, FswDataType, List[EnumItem], Dict[str, Any]]]:
        return {
            'name': self.fprime_name,
            'type': self.datatype,
            'enum': self.enum,
            'metadata': self._metadata_dict
        }

    def __setstate__(self, data: Dict[str, Union[str, FswDataType, list, dict]]) -> None:
        full_dict_spec_check(
            data,
            {
                'name': str,
                'type': FswDataType,
                'enum': list,
                'metadata': dict
            },
            name='data'
        )
        if not all([isinstance(e, EnumItem) for e in cast(list, data['enum'])]):
            raise TypeError(
                f"`data['enum']` should be a list of `EnumItems`s. Got {data['enum']}."
            )
        self.fprime_name = cast(str, data['name'])
        self.datatype = cast(FswDataType, data['type'])
        self.enum = cast(List[EnumItem], data['enum'])
        self.metadata = cast(Dict[str, Any], data['metadata'])


class Command(GswMetadataContainer):
    """Container for Data Standards for an FPrime Command and associated Ground Software Metadata."""

    __slots__: List[str] = [
        # Full (UI) Name of the Command:
        'name',
        # FPrime Command Mnemonic:
        'mnemonic',
        # Lowest 1B of Command Opcode:
        'ID',
        # List of FSW Command Arguments:
        'args'
    ]

    _metadata_dict: Dict[Any, Any]

    def __init__(self,
                 name: str = "",
                 mnemonic: str = "",
                 ID: int = -1,
                 metadata_json_str: str = "",
                 args: List[Argument] = None
                 ) -> None:
        self.name = name
        self.mnemonic = mnemonic
        self.ID = ID
        super().__init__(metadata_json_str)

        if args is None:
            self.args: List[Argument] = []
        else:
            self.args = args

    def __eq__(self, other) -> bool:
        if not isinstance(other, Command):
            return False

        return (
            self.name == other.name
            and self.mnemonic == other.mnemonic
            and self.ID == other.ID
            and self.args == other.args
            and self.metadata == other.metadata
        )

    def __str__(self) -> str:
        return f"Command[{self.ID}]::{self.name}{self.args}"

    def __repr__(self) -> str:
        return f"Command[{self.ID}]::{self.mnemonic}{self.args}"

    def __getstate__(self) -> Dict[str, Any]:
        return {
            'name': self.name,
            'mnemonic': self.mnemonic,
            'ID': self.ID,
            'args': self.args,
            'metadata': self._metadata_dict
        }

    def __setstate__(self, data: Dict[str, Any]) -> None:
        full_dict_spec_check(
            data,
            {
                'name': str,
                'mnemonic': str,
                'ID': int,
                'args': list,
                'metadata': dict
            },
            name='data'
        )

        if not all([isinstance(a, Argument) for a in data['args']]):
            raise TypeError(
                f"`data['args']` should be a list of `Argument`s. Got {data['args']}."
            )
        self.name = data['name']
        self.mnemonic = data['mnemonic']
        self.ID = data['ID']
        self.args = data['args']
        self.metadata = data['metadata']


class Telemetry(object):
    """Container for Data Standards for an FPrime Telemetry Channel and associated Ground Software Metadata."""
    raw_value: Union[float, str]
    converted_value: Union[float, str]
    # TODO: Build Telemetry Class.
    pass


class Event(object):
    """Container for Data Standards for an FPrime Event (aka Log)."""
    __slots__: List[str] = [
        # FPrime Name of the Event:
        'name',
        # Lowest 1B of Command Opcode:
        'ID',
        # Severity Level of the Event:
        'severity',
        # Format String used to Display the Event on the Frontend:
        'format',
        # List of FSW Event Arguments:
        'args'
    ]
    # TODO: Build Events Class.
    pass
