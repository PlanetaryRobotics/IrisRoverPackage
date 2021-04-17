#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Containers for Flight Software Modules/Components and their Relevant Fields
(Commands, Telemetry, Events), alongside relevant ground-software metadata
(where applicable).

@author: Connor W. Colombo (CMU)
@last-updated: 04/17/2021
"""
# Activate postponed annotations (for using classes as return type in their own methods):
from __future__ import annotations

from typing import List, Dict, Any, Union, cast, Optional
from collections import OrderedDict
from enum import Enum as PyEnum  # just in case, room to prevent name-clash

import json

from IrisBackendv3.utils import NameIdDict
from IrisBackendv3.utils.basic import full_dict_spec_check
from .fsw_data_type import FswDataType


class Module(object):
    """Container for Data Standards for a Module in the FSW."""

    __slots__: List[str] = [
        'name',  # - (Standardized) Name of the Module
        'ID',  # - Highest 1B of the Command Opcodes for this Module (its ID)
        'commands',  # - NameIdDict mapping command names and opcodes to Command data
        'telemetry',  # - NameIdDict mapping telemetry channel names and IDs to TelemetryChannel data
        'events'  # - NameIdDict mapping event names and IDs to Event data
    ]

    def __init__(self,
                 name: str,
                 ID: int,
                 commands: NameIdDict[Command],
                 telemetry: NameIdDict[TelemetryChannel],
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
                'commands': NameIdDict,
                'telemetry': NameIdDict,
                'events': NameIdDict
            },
            name='data'
        )
        self.name = cast(str, data['name'])
        self.ID = cast(int, data['ID'])
        self.commands = cast(NameIdDict[Command], data['commands'])
        self.telemetry = cast(NameIdDict[TelemetryChannel], data['telemetry'])
        self.events = cast(NameIdDict[Event], data['events'])


class BitfieldStruct(object):
    __slots__: List[str] = [
        # OrderedDict mapping field names to number of bits in the order they fill the struct:
        '_fields',
        # Total number of bits of padding in the bitfield struct:
        '_total_padding'
    ]

    # Number of bits in the Primary MCU's storage unit (which all bitfield structs should fill):
    STORAGE_UNIT_SIZE = 32

    def __init__(self,
                 fields: OrderedDict[str, int],
                 total_padding: int
                 ) -> None:
        self._fields = fields
        self._total_padding = total_padding
        self._check_data_formatting()

    @property  # public get-only
    def fields(self) -> OrderedDict[str, int]:
        return self._fields

    @property  # public get-only
    def total_padding(self) -> int:
        return self._total_padding

    def _check_data_formatting(self) -> None:
        """
        Ensure all data given conforms to basic expectations.

        Used after setting data in the constructor or after de-pickling.
        """
        if self.total_bits % self.STORAGE_UNIT_SIZE != 0:
            raise ValueError(
                "Total number of bits in bitfield struct should be a multiple of "
                f"the Primary MCU's storage unit size ({self.STORAGE_UNIT_SIZE}b). "
                f"The total number of bits is {self.total_bits}b = "
                f"{sum(self.fields.values())}b of data + {self.total_padding}b of padding."
            )

    @property
    def names(self) -> List[str]:
        """Names of all the fields."""
        return list(self.fields.keys())

    @property
    def total_bits(self) -> int:
        """Total number of bits in this struct."""
        return sum(self.fields.values()) + self.total_padding

    def __eq__(self, other) -> bool:
        if not isinstance(other, BitfieldStruct):
            return False

        return (
            self.fields == other.fields
            and self.total_padding == other.total_padding
        )

    def __repr__(self) -> str:
        return "{" + ", ".join([f"{k}: {v}" for k, v in self.fields.items()]) + "}"

    def __getstate__(self) -> Dict[str, Union[OrderedDict[str, int], int]]:
        return {
            'fields': self.fields,
            'padding': self.total_padding
        }

    def __setstate__(self, data: Dict[str, Union[OrderedDict[str, int], int]]) -> None:
        full_dict_spec_check(
            data,
            {
                'fields': dict,
                'padding': int
            },
            name='data'
        )
        self._fields = cast(OrderedDict, data['fields'])
        self._total_padding = cast(int, data['padding'])
        self._check_data_formatting()


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


class DataUnit(GswMetadataContainer):
    """
    Base class for any data container which carries a single unit of data.

    That is, containers that have a single `datatype`, namely `Argument`s and 
    `TelemetryChannel`s.

    Implicitly, all `DataUnit`s are also `GswMetadataContainer`s because every 
    piece of data being sent to/from Ground will likely be displayed or written 
    by Ground and thus should have the option of having associated GSW metadata 
    to assist the human operator interpreting/writing the data.

    Supports either representing the datatype as an enum value, or dissolving 
    the datatype into a `BitfieldStruct`.
    """
    __slots__: List[str] = [
        # Type of the data being held:
        '_datatype',
        # List of EnumItems if this DataUnit contains an enum ([] otherwise):
        '_enum',
        # Descriptor for the BitfieldStruct if this `DataUnit` contains bitfields (`None` otherwise):
        '_bitfields'
    ]
    _datatype: FswDataType
    _enum: List[EnumItem]
    _bitfields: Optional[BitfieldStruct]

    def __init__(self,
                 datatype: FswDataType,
                 enum: List[EnumItem] = None,
                 bitfields: BitfieldStruct = None,
                 metadata_json_str: str = ""
                 ) -> None:
        self._datatype = datatype

        if enum is None:
            self._enum: List[EnumItem] = []
        else:
            self._enum = enum

        self._bitfields = bitfields

        self._check_data_formatting()

        super().__init__(metadata_json_str)

    @property  # public get-only
    def datatype(self) -> FswDataType:
        return self._datatype

    @property  # public get-only
    def enum(self) -> List[EnumItem]:
        return self._enum

    @property  # public get-only
    def bitfields(self) -> Optional[BitfieldStruct]:
        return self._bitfields

    def _check_data_formatting(self) -> None:
        """
        Ensure all data given conforms to basic expectations.

        Used after setting data either in the constructor or after de-pickling 
        (in subclasses).

        Should be called in the child class at end of `__init__` and 
        `__setstate__`.
        """
        if self.enum is not None and self.enum != [] and self.datatype != FswDataType.ENUM:
            raise ValueError(
                f"Datatype is given as {self.datatype} but an enum list is also provided. "
                f"An Argument must have type {FswDataType.ENUM} to have an enum list."
            )
            #! TODO: Check if multiple EnumItems have the same name, value, or both

        if self.bitfields is not None and self.bitfields.total_bits != self.datatype.num_bits:
            raise ValueError(
                f"Total number of bits in the Argument's bitfields ({self.bitfields.total_bits}b) "
                "needs to match the total number of bits in the Argument's "
                f"datatype ({self.datatype.num_octets}B = {self.datatype.num_bits}b)."
            )

    #! TODO: Unit test these fetchers (`get_enum_xxx`):
    def get_enum_value(self, name: str) -> Optional[int]:
        """Returns the value that matches the given name in the enum (if there is one)."""
        if len(self.enum) == 0 or self.datatype != FswDataType.ENUM:
            return None

        matches = [
            e.value for e in self.enum if name == e.name
        ]
        if len(matches) == 0:
            raise ValueError(
                f"No EnumItems were found in `{self}` whose name matches "
                f"the supplied argument value `{name}` were found."
                f"Valid EnumItems are `{self.enum}` ."
            )

        if len(matches) > 1:
            raise ValueError(
                f"Somehow multiple EnumItems in `{self}` were found whose "
                "name matches the supplied argument value "
                f"`{name}`. Valid EnumItems are `{self.enum}` ."
                "This is likely an issue with the DataStandards spec "
                "which should have been caught when it was built."
            )

        return matches[0]

    def get_enum_name(self, val: int) -> Optional[str]:
        """Returns the name that matches the given value in the enum (if there is one)."""
        if len(self.enum) == 0 or self.datatype != FswDataType.ENUM:
            return None

        matches = [
            e.name for e in self.enum if val == e.value
        ]
        if len(matches) == 0:
            raise ValueError(
                f"No EnumItems were found in `{self}` whose value matches "
                f"the supplied argument value `{val}` were found."
                f"Valid EnumItems are `{self.enum}` ."
            )

        if len(matches) > 1:
            raise ValueError(
                f"Somehow multiple EnumItems in `{self}` were found whose "
                "value matches the supplied argument value "
                f"`{val}`. Valid EnumItems are `{self.enum}` ."
                "This is likely an issue with the DataStandards spec "
                "which should have been caught when it was built."
            )

        return matches[0]


class EnumItem(object):
    """Container for FPrime Enumeration Item (inside an Argument)."""

    __slots__: List[str] = ['name', 'value', 'comment']
    name: str
    value: int
    comment: str

    def __init__(self,
                 name: str,
                 value: int,
                 comment: str = ""
                 ) -> None:
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


class Argument(DataUnit):
    """Container for FPrime (Command or Event) Argument and associated Ground Software Metadata"""

    __slots__: List[str] = [
        # (Standardized) Name:
        'name'
    ]
    name: str

    def __init__(self,
                 name: str,
                 datatype: FswDataType,
                 enum: List[EnumItem] = None,
                 bitfields: BitfieldStruct = None,
                 metadata_json_str: str = ""
                 ) -> None:
        self.name = name

        super().__init__(datatype=datatype, enum=enum,
                         bitfields=bitfields, metadata_json_str=metadata_json_str)

    def __eq__(self, other) -> bool:
        if not isinstance(other, Argument):
            return False

        return (
            self.name == other.name
            and self.datatype == other.datatype
            and self.enum == other.enum
            and self.bitfields == other.bitfields
            and self.metadata == other.metadata
        )

    def __repr__(self) -> str:
        if self.bitfields is None:
            return f"{self.name}: {self.datatype.value}"
        else:
            return f"{self.name}: {self.datatype.value}{self.bitfields}"

    def __getstate__(self) -> Dict[str, Union[str, FswDataType, List[EnumItem], Dict[str, Any], Optional[BitfieldStruct]]]:
        return {
            'name': self.name,
            'type': self.datatype,
            'enum': self.enum,
            'bitfields': self.bitfields,
            'metadata': self._metadata_dict
        }

    def __setstate__(self, data: Dict[str, Union[str, FswDataType, list, dict, Optional[BitfieldStruct]]]) -> None:
        full_dict_spec_check(
            data,
            {
                'name': str,
                'type': FswDataType,
                'enum': list,
                'bitfields': [BitfieldStruct, type(None)],
                'metadata': dict
            },
            name='data'
        )

        if not all([isinstance(e, EnumItem) for e in cast(list, data['enum'])]):
            raise TypeError(
                f"`data['enum']` should be a list of `EnumItems`s. Got {data['enum']}."
            )
        self.name = cast(str, data['name'])
        self._datatype = cast(FswDataType, data['type'])
        self._enum = cast(List[EnumItem], data['enum'])
        self._bitfields = cast(Optional[BitfieldStruct], data['bitfields'])
        self.metadata = cast(Dict[str, Any], data['metadata'])

        self._check_data_formatting()


class Command(GswMetadataContainer):
    """Container for Data Standards for an FPrime Command and associated Ground Software Metadata."""

    __slots__: List[str] = [
        # Full (Standardized) Name of the Command:
        'name',
        # FPrime Command Mnemonic:
        'mnemonic',
        # Lowest 1B of Command Opcode:
        'ID',
        # List of FSW Command Arguments:
        'args'
    ]

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


class TelemetryChannel(DataUnit):
    """Container for Data Standards for an FPrime Telemetry Channel and associated Ground Software Metadata."""

    __slots__: List[str] = [
        # (Standardized) Name of the Channel:
        'name',
        # Lowest 1B of Channel ID:
        'ID'
    ]

    def __init__(self,
                 name: str,
                 ID: int,
                 datatype: FswDataType,
                 enum: List[EnumItem] = None,
                 bitfields: BitfieldStruct = None,
                 metadata_json_str: str = ""
                 ) -> None:
        self.name = name
        self.ID = ID

        super().__init__(datatype=datatype, enum=enum,
                         bitfields=bitfields, metadata_json_str=metadata_json_str)

    def __eq__(self, other) -> bool:
        if not isinstance(other, TelemetryChannel):
            return False

        return (
            self.name == other.name
            and self.ID == other.ID
            and self.datatype == other.datatype
            and self.enum == other.enum
            and self.bitfields == other.bitfields
            and self.metadata == other.metadata
        )

    def __repr__(self) -> str:
        if self.bitfields is None:
            return f"Channel[{self.ID}]::{self.name}: {self.datatype.value}"
        else:
            return f"Channel[{self.ID}]::{self.name}: {self.datatype.value}{self.bitfields}"

    def __getstate__(self) -> Dict[str, Any]:
        return {
            'name': self.name,
            'ID': self.ID,
            'type': self.datatype,
            'enum': self.enum,
            'bitfields': self.bitfields,
            'metadata': self._metadata_dict
        }

    def __setstate__(self, data: Dict[str, Any]) -> None:
        full_dict_spec_check(
            data,
            {
                'name': str,
                'ID': int,
                'type': FswDataType,
                'enum': list,
                'bitfields': [BitfieldStruct, type(None)],
                'metadata': dict
            },
            name='data'
        )

        if not all([isinstance(e, EnumItem) for e in cast(list, data['enum'])]):
            raise TypeError(
                f"`data['enum']` should be a list of `EnumItems`s. Got {data['enum']}."
            )
        self.name = data['name']
        self.ID = data['ID']
        self._datatype = cast(FswDataType, data['type'])
        self._enum = cast(List[EnumItem], data['enum'])
        self._bitfields = cast(Optional[BitfieldStruct], data['bitfields'])
        self.metadata = data['metadata']

        self._check_data_formatting()


class Event(GswMetadataContainer):
    """Container for Data Standards for an FPrime Event (aka Log) and associated Ground Software Metadata."""

    class SeverityLevel(PyEnum):
        """Allowed Severity Levels for an FPrime Event."""
        DIAGNOSTIC = 0, "Software debugging information. Meant for development."
        ACTIVITY_LO = 1, "Low-priority events related to software execution."
        ACTIVITY_HI = 2, "Higher priority events related to software execution."
        COMMAND = 3, "Events related to command execution. Should be reserved for command dispatcher and sequencer."
        WARNING_LO = 4, "Error conditions that are of low importance."
        WARNING_HI = 5, "Error conditions that are of critical importance."
        FATAL = 6, "An error condition was encountered that the software cannot recover from."

        description: str  # Description of this severity level.

        def __new__(cls, val: int, desc: str):
            """Create new Enum instance."""
            obj = object.__new__(cls)
            obj._value_ = val
            obj.description = desc
            return obj

    __slots__: List[str] = [
        # (Standardized) Name of the Event:
        'name',
        # Lowest 1B of Event ID:
        'ID',
        # Severity Level of the Event:
        '_severity',
        # String Representation of the Severity Level (as given in XML):
        '_severity_str',
        # Format String used to Display the Event on the Frontend:
        'format_string',
        # List of FSW Event Arguments:
        'args'
    ]

    # Two interlocked ways of representing and using the severity data:
    _severity: Event.SeverityLevel
    _severity_str: str

    def __init__(self,
                 name: str = "",
                 ID: int = -1,
                 severity_str: str = "",
                 format_string: str = "",
                 metadata_json_str: str = "",
                 args: List[Argument] = None
                 ) -> None:
        self.name = name
        self.ID = ID
        self.severity_str = severity_str
        self.format_string = format_string
        super().__init__(metadata_json_str)

        if args is None:
            self.args: List[Argument] = []
        else:
            self.args = args

    @property
    def severity_str(self) -> str:
        return self._severity_str

    @severity_str.setter
    def severity_str(self, val: str) -> None:
        self._severity_str = val
        # Recheck that this is a known and valid SeverityLevel:
        try:
            self._severity = Event.SeverityLevel[val.upper()]
        except KeyError:
            raise ValueError(
                f"Given Event severity string of {val} does not match a known "
                f"SeverityLevel. Valid SeverityLevels are (caps-insensitive): "
                f"{[sl.name for sl in Event.SeverityLevel]}"
            )

    @property
    def severity(self) -> Event.SeverityLevel:
        return self._severity

    @severity.setter
    def severity(self, val: Event.SeverityLevel) -> None:
        self._severity = val
        # Grab the name string to update the severity_str (if actually distinct):
        if self.severity_str.upper() != val.name.upper():
            self._severity_str = val.name

    def __eq__(self, other) -> bool:
        if not isinstance(other, Event):
            return False

        return (
            self.name == other.name
            and self.ID == other.ID
            and self.severity == other.severity
            and self.format_string == other.format_string
            and self.args == other.args
            and self.metadata == other.metadata
        )

    def __repr__(self) -> str:
        return f"Event[{self.ID}]::{self.name}{self.args}"

    def __getstate__(self) -> Dict[str, Any]:
        return {
            'name': self.name,
            'ID': self.ID,
            'severity': self.severity_str,
            'format_string': self.format_string,
            'args': self.args,
            'metadata': self._metadata_dict
        }

    def __setstate__(self, data: Dict[str, Any]) -> None:
        full_dict_spec_check(
            data,
            {
                'name': str,
                'ID': int,
                'severity': str,
                'format_string': str,
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
        self.ID = data['ID']
        self.severity_str = data['severity']
        self.format_string = data['format_string']
        self.args = data['args']
        self.metadata = data['metadata']
