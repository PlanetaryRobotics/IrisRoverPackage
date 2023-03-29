#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""
Defines Common Data Required for Payloads. Support for Building and Parsing
Payloads as part of a Variable Length Payload.

@author: Connor W. Colombo (CMU)
@last-updated: 09/19/2022
"""
from __future__ import annotations  # Activate postponed annotations (for using classes as return type in their own methods)

from typing import List, Any, Union, Dict, Tuple, Optional, TypeVar, Type
from abc import ABC, abstractmethod, abstractclassmethod, abstractproperty
from enum import Enum
from collections import OrderedDict

import struct
from datetime import datetime

from IrisBackendv3.codec.magic import Magic, MAGIC_SIZE
from IrisBackendv3.codec.metadata import DataPathway, DataSource, DownlinkTimes, UplinkTimes
from IrisBackendv3.codec.container import ContainerCodec
from IrisBackendv3.codec.fsw_data_codec import encode as fsw_data_encode
from IrisBackendv3.codec.fsw_data_codec import decode as fsw_data_decode

from IrisBackendv3.codec.logging import logger

from IrisBackendv3.codec.settings import ENDIANNESS_CODE, settings

from IrisBackendv3.data_standards import DataStandards
from IrisBackendv3.data_standards.module import Module, Command, TelemetryChannel, Event
from IrisBackendv3.data_standards import FswDataType
from IrisBackendv3.utils.basic import full_dict_spec_check

from IrisBackendv3.codec.exceptions import PacketDecodingException


PIT = TypeVar('PIT')


class PayloadInterface(ContainerCodec[PIT], ABC):
    """
    Generic Payload Interface/Wrapper class to enable binding the Payload's
    generic type to something that lists all the right interfaces.
    """
    __slots__: List[str] = [
        # Magic indicating how this payload should be transmitted / where it came from:
        'magic',
        # Flag indicating how this payload should go to the rover / came from the rover:
        'pathway',
        # DataSource of this data (how it entered the GSW):
        'source'
    ]

    magic: Magic
    pathway: DataPathway
    source: DataSource

    @classmethod
    @abstractmethod
    def process(cls,
                VLP: bytes, endianness_code: str = ENDIANNESS_CODE
                ) -> Tuple[PIT, bytes]:
        raise NotImplementedError()

    @abstractmethod
    def __getstate__(self) -> Dict[str, Any]:
        raise NotImplementedError()

    @abstractmethod
    def __setstate__(self, data: Dict[str, Any]) -> None:
        raise NotImplementedError()


PT = TypeVar('PT', bound=PayloadInterface)


class Payload(PayloadInterface[PT], ABC):
    """
    Generic Interface for Handling and Processing Payloads.

    All data is stored in the unpacked form since that's how it will most
    often be handled and transformed and only packed into bytes when needed.

    *However* the raw form of the data (as downlinked) is preserved for forensic
    reasons and is used as the sole way of serializing and storing data so that
    if, for example, adjustments need to be made to the decoding script (perhaps
    to correct for a persistent data corruption mode, etc.), the raw data will
    be preserved and can simply be reprocessed with a fixed decoding script.

    *NOTE:* All Payload data is encoded and decoded with the `magic` stripped
    off.

    All additional payload-specific metadata (including the `magic`, data
    `pathway`, and `source`) will be serialized on the ground as a state
    alongside the `raw` bytes.
    """

    # Empty __slots__ allows keeping __slots__ from superclass (and not switching to __dict__):
    __slots__: List[str] = []

    def __init__(self,
                 magic: Magic = Magic.MISSING,
                 pathway: DataPathway = DataPathway.NONE,
                 source: DataSource = DataSource.NONE,
                 raw: Optional[bytes] = None,
                 endianness_code: str = ENDIANNESS_CODE
                 ) -> None:
        self.magic = magic
        self.pathway = pathway
        self.source = source
        super().__init__(raw=raw, endianness_code=endianness_code)

    @classmethod
    def process(cls,
                VLP: bytes, endianness_code: str = ENDIANNESS_CODE
                ) -> Tuple[PT, bytes]:
        """
        Processes the given VLP *with the appropriate magic already stripped off*
        as a payload of this type and returns the appropriate payload object as
        well as the VLP with the consumed bytes stripped off.
        """
        # Extract the Payload:
        payload = cls.decode(
            data=VLP,
            endianness_code=endianness_code
        )

        # Strip off the bytes used by the payload from the VLP:
        if payload.raw is None:
            payload_size = 0
        else:
            payload_size = len(payload.raw)
        # All of the VLP left over for subsequent payloads:
        VLP = VLP[payload_size:]

        return payload, VLP

    def __getstate__(self) -> Dict[str, Any]:
        # Extra metadata that's not encoded in `_raw` bytes.
        return {
            'magic': self.magic,
            'pathway': self.pathway,
            'source': self.source
        }

    def __setstate__(self, data: Dict[str, Any]) -> None:
        # Extra metadata that's not encoded in `_raw` bytes.
        full_dict_spec_check(
            data,
            {
                'magic': Magic,
                'pathway': DataPathway,
                'source': DataSource
            },
            name='data'
        )

        self.magic = data['magic']
        self.pathway = data['pathway']
        self.source = data['source']


class UplinkedPayload(Payload[PT]):
    """
    Generic superclass for all payloads which are uplinked.
    Allows for creating typed containers of just uplinked payloads.
    """
    __slots__: List[str] = [
        # Whether AMCC (Astrobotic Mission Control) has acknowledged receipt of
        # this payload:
        'amcc_ack',
        # Whether the rover has acknowledged receipt of this payload:
        'rover_ack',
        # Collection of times for the downlink pipeline
        'uplink_times'
    ]

    amcc_ack: bool
    rover_ack: bool
    uplink_times: Optional[UplinkTimes]

    def __init__(self,
                 magic: Magic = Magic.MISSING,
                 pathway: DataPathway = DataPathway.NONE,
                 source: DataSource = DataSource.NONE,
                 raw: Optional[bytes] = None,
                 endianness_code: str = ENDIANNESS_CODE,
                 amcc: bool = False,
                 rover_ack: bool = False,
                 uplink_times: Optional[UplinkTimes] = None
                 ) -> None:
        # Pass through data to parent and initialize metadata values.
        self.amcc_ack = amcc
        self.rover_ack = rover_ack
        self.uplink_times = uplink_times
        super().__init__(
            magic=magic,
            pathway=pathway,
            source=source,
            raw=raw,
            endianness_code=endianness_code
        )

    def __getstate__(self) -> Dict[str, Any]:
        """Encode metadata which is not derived from `_raw` as a state."""
        return {
            **super().__getstate__(),  # grab any relevant state managed by parent
            'amcc_ack': self.amcc_ack,
            'rover_ack': self.rover_ack,
            'uplink_times': self.uplink_times
        }

    def __setstate__(self, data: Dict[str, Any]) -> None:
        """Retrieve metadata which is not encoded in `_raw` as a state."""
        # Let the parent extract any data it cares about (manages):
        super().__setstate__(data)

        # Then extract the state data managed by this class:
        full_dict_spec_check(
            data,
            {
                'amcc_ack': bool,
                'rover_ack': bool,
                'uplink_times': [UplinkTimes, type(None)]
            },
            name='data'
        )

        self.amcc_ack = data['amcc_ack']
        self.rover_ack = data['rover_ack']
        self.uplink_times = data['uplink_times']


class CommandPayloadInterface(UplinkedPayload[PT], ABC):
    """
    Generic interface which defines all I/O for CommandPayload allowing it to be
    used as the Type Generic for CommandPayload, which in turn allows many of
    inherited functions to return CommandPayloads.

    In essence, this defines any variables, properties, and methods which are
    unique to CommandPayload (and not inherited from its superclasses).
    In addition, properties and not just their interfaces are defined here for
    convenience sake (to reduce code bulk).

    Note: this is basically required because you can't directly reference a
    class as its own generic fill, e.g.:
    `class CommandPayload(UplinkedPayload[CommandPayload])`
    """
    __slots__: List[str] = [
        # ID for the Definition for Module this Belongs:
        '_module_id',
        # ID for the Command Definition (from FPrime XML):
        '_command_id',
        # Dictionary of arguments for the command
        '_args'
    ]
    _module_id: int
    _command_id: int
    _args: Dict[str, Any]

    # Make public get, private set to signal that you can freely use these values
    # but modifying them directly can yield undefined behavior (specifically
    # `raw` not syncing up with whatever other data is in the container)
    @property
    def module_id(self) -> int: return self._module_id
    @property
    def command_id(self) -> int: return self._command_id
    @property
    def args(self) -> Dict[str, Any]: return self._args

    @property
    def module(self) -> Module:
        return settings['STANDARDS'].modules[self.module_id]

    @property
    def command(self) -> Command:
        return self.module.commands[self.command_id]

    @property
    def opcode(self) -> int:
        return self.module_id | self._command_id

    def __str__(self) -> str:
        return (
            f"{self.command.name}"
            f"[{', '.join(f'{a}={v}' for a,v in self.args.items())}]"
        )

    def __repr__(self) -> str:
        return (
            f"{self.command.name}(0x{self.opcode:04X})"
            f"[{', '.join(f'{a}={v}' for a,v in self.args.items())}]"
        )

    @abstractmethod
    def check_args(self) -> None:
        raise NotImplementedError()


class CommandPayload(CommandPayloadInterface[CommandPayloadInterface]):
    """Implementation of Payload Interface for IRIS Telemetry Payloads."""

    # Empty __slots__ allows keeping __slots__ (and not switching to __dict__):
    __slots__: List[str] = []

    def check_args(self) -> None:
        """Checks whether this command's args exactly meet the command's spec."""
        given_args = set(self.args.keys())
        required_args = {a.name for a in self.command.args}

        # Make sure all the right and only the arg names are supplied:
        if given_args != required_args:
            raise ValueError(
                f"Command Payload for command {self.command} was given wrong "
                f"set of arguments. Required: {required_args}, Given: {given_args} ."
            )

        # Check given arg's value and type conformity:
        for arg in self.command.args:
            arg_value = self.args[arg.name]
            # Check all args have the right datatype:
            if not isinstance(arg_value, arg.datatype.python_type):
                raise TypeError(
                    f"Type of given arg data `{arg_value}` is `{type(arg_value)}` "
                    f"which is not valid for argument `{arg}` with FswDataType "
                    f"`{arg.datatype}` of command `{self.command}`."
                )

            # Check to make sure any enum args have a valid value:
            if arg.is_enum:
                if type(arg_value) == str:
                    matches = [
                        arg_value == e.name for e in arg.enum
                    ]
                else:
                    matches = [
                        arg_value == e.value for e in arg.enum
                    ]
                if not any(matches):
                    raise ValueError(
                        f"Argument `{arg}` is an Enum of command `{self.command}` "
                        f"but no EnumItems whose name/value matches the supplied "
                        f"argument value `{arg_value}` were found."
                        f"Valid EnumItems are `{arg.enum}` ."
                    )

                if sum(matches) > 1:
                    raise ValueError(
                        f"Argument `{arg}` is an Enum of command `{self.command}` "
                        f"and somehow multiple EnumItems were found whose "
                        "name/value matches the supplied argument value "
                        f"`{arg_value}`. Valid EnumItems are `{arg.enum}` ."
                        "This is likely an issue with the DataStandards spec "
                        "which should have been caught when it was built."
                    )

            #! TODO: Check against bitfields

    def __init__(self,
                 module_id: int,
                 command_id: int,
                 args: Dict[str, Any],
                 magic: Magic = Magic.MISSING,
                 pathway: DataPathway = DataPathway.NONE,
                 source: DataSource = DataSource.NONE,
                 raw: Optional[bytes] = None,
                 endianness_code: str = ENDIANNESS_CODE,
                 auto_tag_generated_time: bool = True,
                 **kwargs  # kwargs for UplinkedPayload super class
                 ) -> None:
        self._module_id = module_id
        self._command_id = command_id
        self._args = args
        self.check_args()

        if auto_tag_generated_time and 'uplink_times' not in kwargs:
            # ! TODO: Shouldn't this be in UplinkedPayload?
            # automatically tag the payload with the generation time in
            # `UplinkTimes` if `uplink_times` not given.
            kwargs['uplink_times'] = UplinkTimes(generated=datetime.now())

        super().__init__(
            magic=magic, pathway=pathway, source=source,
            raw=raw, endianness_code=endianness_code,
            **kwargs
        )

    @classmethod
    def decode(cls,
               data: bytes,
               endianness_code: str = ENDIANNESS_CODE
               ) -> CommandPayload:
        opcode_bytes, remaining = data[:2], data[2:]
        opcode: int = struct.unpack(endianness_code+'H', opcode_bytes)[0]
        module_id = opcode & 0xFF00
        command_id = opcode & 0xFF

        module = settings['STANDARDS'].modules[module_id]
        command = module.commands[command_id]

        args: Dict[str, Any] = {}
        total_args_len = 0
        for arg in command.args:
            #! TODO: Maybe move this checker into the appropriate Module class?
            arg_len = arg.datatype.get_actual_num_bytes(remaining)
            total_args_len += arg_len
            arg_bytes = remaining[:arg_len]
            args[arg.name] = fsw_data_decode(
                arg.datatype, arg_bytes
            )
            remaining = remaining[arg_len:]

            # Convert to enum name string if type is enum:
            if arg.is_enum:
                # Grab names of all EnumItems whose value matches the extracted value:
                # (there should be one)
                matches = [
                    e.name for e in arg.enum if args[arg.name] == e.value
                ]
                if len(matches) == 0:
                    raise PacketDecodingException(
                        arg_bytes,
                        f"No EnumItems found in argument `{arg}` of command "
                        f"`{command}` whose value matches the value "
                        f"`{args[arg.name]}` extracted from the given data. "
                        f"Valid EnumItems are `{arg.enum}` ."
                    )

                if len(matches) > 1:
                    raise PacketDecodingException(
                        arg_bytes,
                        f"Somehow multiple EnumItems found in argument `{arg}` "
                        f"of command `{command}` whose value matches the value "
                        f"`{args[arg.name]}` extracted from the given data. "
                        f"Valid EnumItems are `{arg.enum}` ."
                        "This is likely an issue with the DataStandards spec "
                        "which should have been caught when it was built."
                    )

                args[arg.name] = matches[0]

        data_used = data[:(2+total_args_len)]

        #! TODO: Handle bitfields

        return cls(
            module_id=module.ID,
            command_id=command.ID,
            args=args,
            raw=data_used,
            endianness_code=endianness_code
        )

    def encode(self, **kwargs: Any) -> bytes:
        # Check all args meet spec:
        self.check_args()

        opcode = self.module_id | self.command_id
        header = struct.pack(self.endianness_code+'H', opcode)
        data = header

        # Successively encode and append all arguments:
        for arg in self.command.args:
            # Grab argument value:
            arg_val = self.args[arg.name]
            # If it's an enum and the name of an EnumItem was supplied (not
            # its value), convert it to its value.
            if arg.is_enum and isinstance(arg_val, str):
                arg_val = [e.value for e in arg.enum if e.name == arg_val][0]
            # Encode and append arg bytes:
            data = data + fsw_data_encode(arg.datatype, arg_val)

        return data


class WatchdogCommandPayload(CommandPayload):
    """Same as CommandPayload but with a special U8 Enum implementation."""
    @classmethod
    def decode(cls,
               data: bytes,
               endianness_code: str = ENDIANNESS_CODE
               ) -> WatchdogCommandPayload:
        opcode_bytes, remaining = data[:2], data[2:]
        opcode: int = struct.unpack(endianness_code+'H', opcode_bytes)[0]
        module_id = opcode & 0xFF00
        command_id = opcode & 0xFF

        module = settings['STANDARDS'].modules[module_id]
        command = module.commands[command_id]

        args: Dict[str, Any] = {}
        total_args_len = 0
        for arg in command.args:
            #! TODO: Maybe move this checker into the appropriate Module class?
            arg_len = arg.datatype.get_actual_num_bytes(remaining)
            total_args_len += arg_len
            arg_bytes = remaining[:arg_len]
            # Watchdog takes U8 Enums instead of U32:
            eff_datatype = arg.datatype if arg.datatype != FswDataType.ENUM else FswDataType.U8
            args[arg.name] = fsw_data_decode(
                eff_datatype, arg_bytes
            )
            remaining = remaining[arg_len:]

            # Convert to enum name string if type is enum:
            if arg.is_enum:
                # Grab names of all EnumItems whose value matches the extracted value:
                # (there should be one)
                matches = [
                    e.name for e in arg.enum if args[arg.name] == e.value
                ]
                if len(matches) == 0:
                    raise PacketDecodingException(
                        arg_bytes,
                        f"No EnumItems found in argument `{arg}` of command "
                        f"`{command}` whose value matches the value "
                        f"`{args[arg.name]}` extracted from the given data. "
                        f"Valid EnumItems are `{arg.enum}` ."
                    )

                if len(matches) > 1:
                    raise PacketDecodingException(
                        arg_bytes,
                        f"Somehow multiple EnumItems found in argument `{arg}` "
                        f"of command `{command}` whose value matches the value "
                        f"`{args[arg.name]}` extracted from the given data. "
                        f"Valid EnumItems are `{arg.enum}` ."
                        "This is likely an issue with the DataStandards spec "
                        "which should have been caught when it was built."
                    )

                args[arg.name] = matches[0]

        data_used = data[:(2+total_args_len)]

        #! TODO: Handle bitfields

        return cls(
            module_id=module.ID,
            command_id=command.ID,
            args=args,
            raw=data_used,
            endianness_code=endianness_code
        )

    def encode(self, **kwargs: Any) -> bytes:
        # Check all args meet spec:
        self.check_args()

        opcode = self.module_id | self.command_id
        header = struct.pack(self.endianness_code+'H', opcode)
        data = header

        # Successively encode and append all arguments:
        for arg in self.command.args:
            # Grab argument value:
            arg_val = self.args[arg.name]
            # If it's an enum and the name of an EnumItem was supplied (not
            # its value), convert it to its value.
            if arg.is_enum and isinstance(arg_val, str):
                arg_val = [e.value for e in arg.enum if e.name == arg_val][0]
            # Encode and append arg bytes:
            # Watchdog takes U8 Enums instead of U32:
            eff_datatype = arg.datatype if arg.datatype != FswDataType.ENUM else FswDataType.U8
            data = data + fsw_data_encode(eff_datatype, arg_val)

        return data


class DownlinkedPayload(Payload[PT]):
    """
    Generic superclass for all payloads which are downlinked.
    Allows for creating typed containers of just downlinked payloads.
    """
    __slots__: List[str] = [
        # Collection of times for the downlink pipeline (even though these times
        # are uniform across an entire packet, it's linked into each Payload
        # since payloads will eventually be severed from their Packets later in
        # the data pipeline):
        'downlink_times'
    ]

    downlink_times: Optional[DownlinkTimes]

    def __init__(self,
                 magic: Magic = Magic.MISSING,
                 pathway: DataPathway = DataPathway.NONE,
                 source: DataSource = DataSource.NONE,
                 raw: Optional[bytes] = None,
                 endianness_code: str = ENDIANNESS_CODE,
                 downlink_times: Optional[DownlinkTimes] = None
                 ) -> None:
        # Pass through data to parent and initialize metadata values.
        self.downlink_times = downlink_times
        super().__init__(
            magic=magic,
            pathway=pathway,
            source=source,
            raw=raw,
            endianness_code=endianness_code
        )

    def __getstate__(self) -> Dict[str, Any]:
        """Encode metadata which is not derived from `_raw` as a state."""
        return {
            **super().__getstate__(),  # grab any relevant state managed by parent
            'downlink_times': self.downlink_times
        }

    def __setstate__(self, data: Dict[str, Any]) -> None:
        """Retrieve metadata which is not encoded in `_raw` as a state."""
        # Let the parent extract any data it cares about (manages):
        super().__setstate__(data)

        # Then extract the state data managed by this class:
        full_dict_spec_check(
            data,
            {
                'downlink_times': [DownlinkTimes, type(None)]
            },
            name='data'
        )

        self.downlink_times = data['downlink_times']


class TelemetryPayloadInterface(DownlinkedPayload[PT], ABC):
    """
    Generic interface which defines all I/O for TelemetryPayload allowing it to be
    used as the Type Generic for TelemetryPayload, which in turn allows many of
    inherited functions to return TelemetryPayload.

    In essence, this defines any variables, properties, and methods which are
    unique to TelemetryPayload (and not inherited from its superclasses).
    In addition, properties and not just their interfaces are defined here for
    convenience sake (to reduce code bulk).

    Note: this is basically required because you can't directly reference a
    class as its own generic fill, e.g.:
    `class TelemetryPayload(DownlinkedPayload[TelemetryPayload])`
    """

    __slots__: List[str] = [
        # ID for the Definition for Module this Belongs:
        '_module_id',
        # ID for the TelemetryChannel Definition (from FPrime XML):
        '_channel_id',
        # Core Telemetry Data being Transmitted:
        '_data',
        # Time this Payload was Generated on the Rover [ms]:
        '_timestamp'
    ]
    _module_id: int
    _channel_id: int
    _data: Any
    _timestamp: int

    # Make public get, private set to signal that you can freely use these values
    # but modifying them directly can yield undefined behavior (specifically
    # `raw` not syncing up with whatever other data is in the container)
    @property
    def module_id(self) -> int: return self._module_id
    @property
    def channel_id(self) -> int: return self._channel_id
    @property
    def data(self) -> Any: return self._data
    @property
    def timestamp(self) -> int: return self._timestamp

    @property
    def module(self) -> Module:
        return settings['STANDARDS'].modules[self.module_id]

    @property
    def channel(self) -> TelemetryChannel:
        return self.module.telemetry[self.channel_id]

    @property
    def opcode(self) -> int:
        return self.module_id | self.channel_id


class TelemetryPayload(TelemetryPayloadInterface[TelemetryPayloadInterface]):
    """Implementation of Payload Interface for IRIS Telemetry Payloads."""

    # Empty __slots__ allows keeping __slots__ (and not switching to __dict__):
    __slots__: List[str] = []

    def __init__(self,
                 module_id: int,
                 channel_id: int,
                 data: Any,
                 timestamp: int,
                 magic: Magic = Magic.MISSING,
                 pathway: DataPathway = DataPathway.NONE,
                 source: DataSource = DataSource.NONE,
                 raw: Optional[bytes] = None,
                 endianness_code: str = ENDIANNESS_CODE
                 ) -> None:
        self._module_id = module_id
        self._channel_id = channel_id
        self._data = data
        self._timestamp = timestamp
        super().__init__(
            magic=magic, pathway=pathway, source=source,
            raw=raw, endianness_code=endianness_code
        )

    def __str__(self) -> str:
        # Enums could be int or str, so convert to what's not given and show both:
        if self.channel.is_enum:
            val = self.channel.get_enum_formatted_str(self.data)
        else:
            val = self.data

        return f"{self.module.name}{{{self.channel.name}}}@{self.timestamp} = {val}"

    def __repr__(self) -> str:
        # Enums could be int or str, so convert to what's not given and show both:
        if self.channel.is_enum:
            val = self.channel.get_enum_formatted_str(self.data)
        else:
            val = self.data

        return f"{self.module}{{{self.channel}}}@{self.timestamp} = {val}"

    @classmethod
    def decode(cls,
               data: bytes,
               endianness_code: str = ENDIANNESS_CODE
               ) -> TelemetryPayload:
        opcode_bytes, timestamp_bytes, remaining = data[:2], data[2:6], data[6:]
        opcode: int = struct.unpack(endianness_code+'H', opcode_bytes)[0]
        timestamp: int = struct.unpack(endianness_code+'L', timestamp_bytes)[0]
        module_id = opcode & 0xFF00
        channel_id = opcode & 0xFF

        module = settings['STANDARDS'].modules[module_id]
        channel = module.telemetry[channel_id]

        core_data_len = channel.datatype.get_actual_num_bytes(remaining)
        core_data = fsw_data_decode(
            channel.datatype, remaining[:core_data_len]
        )

        # All data used by this payload (so anything else left in `data` is part
        # of another payload in the VLP):
        data_used = data[:(6+core_data_len)]

        return cls(
            module_id=module.ID,
            channel_id=channel.ID,
            data=core_data,
            timestamp=timestamp,
            raw=data_used,
            endianness_code=endianness_code
        )

    def encode(self, **kwargs: Any) -> bytes:
        opcode = self.module_id | self.channel_id
        t = self.timestamp
        header = struct.pack(self.endianness_code+'H L', opcode, t)

        # If data is an enum and the name of an EnumItem was supplied (not
        # its value), convert it to its value.
        if self.channel.is_enum and isinstance(self.data, str):
            val = [e.value for e in self.channel.enum if e.name == self.data][0]
        else:
            val = self.data

        payload = fsw_data_encode(self.channel.datatype, val)
        return header + payload


class EventPayloadInterface(DownlinkedPayload[PT], ABC):
    """
    Generic interface which defines all I/O for EventPayload allowing it to be
    used as the Type Generic for EventPayload, which in turn allows many of
    inherited functions to return EventPayload.

    In essence, this defines any variables, properties, and methods which are
    unique to EventPayload (and not inherited from its superclasses).
    In addition, properties and not just their interfaces are defined here for
    convenience sake (to reduce code bulk).

    Note: this is basically required because you can't directly reference a
    class as its own generic fill, e.g.:
    `class EventPayload(DownlinkedPayload[EventPayload])`
    """
    __slots__: List[str] = [
        # ID for the Definition for Module this Belongs
        '_module_id',
        # ID for the Event Definition (from FPrime XML):
        '_event_id',
        # OrderedDict of all args and their extracted values:
        '_args',
        # Time this Payload was Generated on the Rover [ms]:
        '_timestamp'
    ]
    _module_id: int
    _event_id: int
    _args: OrderedDict[str, Any]
    _timestamp: int

    # Make public get, private set to signal that you can freely use these values
    # but modifying them directly can yield undefined behavior (specifically
    # `raw` not syncing up with whatever other data is in the container)
    @property
    def module_id(self) -> int: return self._module_id
    @property
    def event_id(self) -> int: return self._event_id
    @property
    def opcode(self) -> int: return self.module_id | self.event_id
    @property
    def args(self) -> OrderedDict[str, Any]: return self._args
    @property
    def timestamp(self) -> int: return self._timestamp

    @property
    def module(self) -> Module:
        return settings['STANDARDS'].modules[self.module_id]

    @property
    def event(self) -> Event:
        return self.module.events[self.event_id]

    @property
    def formatted_string(self) -> str:
        """Use the parsed args to populate the Event's format string."""
        ordered_formatted_args_list = []
        for arg, v in zip(self.event.args, self.args.values()):
            if arg.is_enum:
                val = arg.get_enum_formatted_str(v)
            else:
                val = v
            ordered_formatted_args_list.append(val)

        # Apply the args list to the Event's format string:
        return self.event.format_string % tuple(ordered_formatted_args_list)


class EventPayload(EventPayloadInterface[EventPayloadInterface]):
    """Implementation of Payload Interface for IRIS Event Payloads."""

    # Empty __slots__ allows keeping __slots__ (and not switching to __dict__):
    __slots__: List[str] = []

    def check_args(self) -> None:
        """Checks whether this events's args exactly meet the events's spec."""
        #! TODO: Maybe move this checker into the appropriate Module class?
        given_args = set(self.args.keys())
        required_args = {a.name for a in self.event.args}

        # Make sure all the right and only the arg names are supplied:
        if given_args != required_args:
            raise ValueError(
                f"Event Payload for event {self.event} was given wrong "
                f"set of arguments. Required: {required_args}, Given: {given_args} ."
            )

        # Check given arg's value and type conformity:
        for arg in self.event.args:
            arg_value = self.args[arg.name]
            # Check all args have the right datatype:
            if not isinstance(arg_value, arg.datatype.python_type):
                raise TypeError(
                    f"Type of given arg data `{arg_value}` is `{type(arg_value)}` "
                    f"which is not valid for argument `{arg}` with FswDataType "
                    f"`{arg.datatype}` of event `{self.event}`."
                )

            # Check to make sure any enum args have a valid value:
            if arg.is_enum:
                if type(arg_value) == str:
                    matches = [
                        arg_value == e.name for e in arg.enum
                    ]
                else:
                    matches = [
                        arg_value == e.value for e in arg.enum
                    ]
                if not any(matches):
                    raise ValueError(
                        f"Argument `{arg}` is an Enum of event `{self.event}` "
                        f"but no EnumItems whose name/value matches the supplied "
                        f"argument value `{arg_value}` were found."
                        f"Valid EnumItems are `{arg.enum}` ."
                    )

                if sum(matches) > 1:
                    raise ValueError(
                        f"Argument `{arg}` is an Enum of command `{self.event}` "
                        f"and somehow multiple EnumItems were found whose "
                        "name/value matches the supplied argument value "
                        f"`{arg_value}`. Valid EnumItems are `{arg.enum}` ."
                        "This is likely an issue with the DataStandards spec "
                        "which should have been caught when it was built."
                    )

            #! TODO: Check against bitfields

    def __str__(self) -> str:
        return (
            f"({self.event.severity_str}) "
            f"<0x{self.opcode:04X}>"
            f"{self.event.name}@{self.timestamp}: "
            f"'{self.formatted_string}'"
        )

    def __repr__(self) -> str:
        return (
            f"({self.event.severity_str}) "
            f"<0x{self.opcode:04X}>"
            f"{self.event.name}[{self.args}]@{self.timestamp} "
            f"-> '{self.formatted_string}'"
        )

    def __init__(self,
                 module_id: int,
                 event_id: int,
                 args: Dict[str, Any],
                 timestamp: int,
                 magic: Magic = Magic.MISSING,
                 pathway: DataPathway = DataPathway.NONE,
                 source: DataSource = DataSource.NONE,
                 raw: Optional[bytes] = None,
                 endianness_code: str = ENDIANNESS_CODE
                 ) -> None:
        self._module_id = module_id
        self._event_id = event_id
        # Convert args Dict to OrderedDict, taking order from args list in Event:
        self._args = OrderedDict([(a.name, args[a.name])
                                 for a in self.event.args])
        self.check_args()

        self._timestamp = timestamp
        super().__init__(
            magic=magic, pathway=pathway, source=source,
            raw=raw, endianness_code=endianness_code
        )

    @classmethod
    def decode(cls,
               data: bytes,
               endianness_code: str = ENDIANNESS_CODE
               ) -> EventPayload:
        opcode_bytes, timestamp_bytes, remaining = data[:2], data[2:6], data[6:]
        opcode: int = struct.unpack(endianness_code+'H', opcode_bytes)[0]
        timestamp: int = struct.unpack(endianness_code+'L', timestamp_bytes)[0]
        module_id = opcode & 0xFF00
        event_id = opcode & 0xFF

        module = settings['STANDARDS'].modules[module_id]
        event = module.events[event_id]

        args: Dict[str, Any] = {}
        total_args_len = 0
        for arg in event.args:
            arg_len = arg.datatype.get_actual_num_bytes(remaining)
            total_args_len += arg_len
            arg_bytes = remaining[:arg_len]
            args[arg.name] = fsw_data_decode(
                arg.datatype, arg_bytes
            )
            remaining = remaining[arg_len:]

            # Convert to enum name string if type is enum:
            if arg.is_enum:
                # Grab names of all EnumItems whose value matches the extracted value:
                # (there should be one)
                matches = [
                    e.name for e in arg.enum if args[arg.name] == e.value
                ]
                if len(matches) == 0:
                    raise PacketDecodingException(
                        arg_bytes,
                        f"No EnumItems found in argument `{arg}` of event "
                        f"`{event}` whose value matches the value "
                        f"`{args[arg.name]}` extracted from the given data. "
                        f"Valid EnumItems are `{arg.enum}` ."
                    )

                if len(matches) > 1:
                    raise PacketDecodingException(
                        arg_bytes,
                        f"Somehow multiple EnumItems found in argument `{arg}` "
                        f"of event `{event}` whose value matches the value "
                        f"`{args[arg.name]}` extracted from the given data. "
                        f"Valid EnumItems are `{arg.enum}` ."
                        "This is likely an issue with the DataStandards spec "
                        "which should have been caught when it was built."
                    )

                args[arg.name] = matches[0]

        # All data used by this payload (so anything else left in `data` is part
        # of another payload in the VLP):
        data_used = data[:(6+total_args_len)]

        return cls(
            module_id=module.ID,
            event_id=event.ID,
            args=args,
            timestamp=timestamp,
            raw=data_used,
            endianness_code=endianness_code
        )

    def encode(self, **kwargs: Any) -> bytes:
        # Check all args meet spec:
        self.check_args()

        # Build header:
        opcode = self.module_id | self.event_id
        t = self.timestamp
        header = struct.pack(self.endianness_code+'H L', opcode, t)
        data = header

        # Successively encode and append all arguments:
        for arg in self.event.args:
            # Grab argument value:
            arg_val = self.args[arg.name]
            # If it's an enum and the name of an EnumItem was supplied (not
            # its value), convert it to its value.
            if arg.is_enum and isinstance(arg_val, str):
                arg_val = [e.value for e in arg.enum if e.name == arg_val][0]
            # Encode and append arg bytes:
            data = data + fsw_data_encode(arg.datatype, arg_val)

        return data


FMIT = TypeVar('FMIT')


class FileMetadataInterface(ContainerCodec[FMIT], ABC):
    """
    Generic interface which defines all I/O for FileMetadata allowing it to be
    used as the Type Generic for FileMetadata, which in turn allows many of
    inherited functions to return FileMetadata.

    In essence, this defines any variables, properties, and methods which are
    unique to FileMetadata (and not inherited from its superclasses).
    In addition, properties and not just their interfaces are defined here for
    convenience sake (to reduce code bulk).

    Note: this is basically required because you can't directly reference a
    class as its own generic fill, e.g.:
    `class FileMetadata(ContainerCodec[FileMetadata])`
    """

    class FileTypeMagic(Enum):
        """
        Enumeration of all file types which could be downlinked.

        As usual, for backwards compatibility and data preservation, *never* change
        any of the enum values or delete entries, just deprecate old ones.
        """
        IMAGE = 0x01
        UWB = 0x0F
        BAD = 0xEE  # No valid Filetype magic was found.

    __slots__: List[str] = [
        # Monotonically increasing callback ID of the command that triggered
        # this file to be sent. Uniquely links this file to the command which
        # requested it. Callback IDs are unique for images and UWB files.
        # Callback IDs for commands requesting images start counting at 0.
        # Callback IDs for commands requesting UWB files start counting at 2^5:
        '_callback_id',
        # Time (in ms since Hercules power up) of when this file was generated:
        '_timestamp',
        # Magic indicating the type of file being encoded:
        '_file_type_magic'
    ]
    _callback_id: int
    _timestamp: int
    _file_type_magic: FileTypeMagic

    # Make public get, private set to signal that you can freely use these values
    # but modifying them directly can yield undefined behavior (specifically
    # `raw` not syncing up with whatever other data is in the container)
    @property
    def callback_id(self) -> int: return self._callback_id
    @property
    def timestamp(self) -> int: return self._timestamp
    @property
    def file_type_magic(self) -> FileTypeMagic: return self._file_type_magic

    @property
    def full_file_id(self) -> int:
        """ID that specifies which larger file this line came from (since
        each line of the image is being downlinked as a "file").
        For now, since all lines of an image share the same `createTime`
        timestamp, we'll just use that.
        """
        return self._timestamp

    def __str__(self) -> str:
        return (
            "FileBlockMetadata["
            f"{self.file_type_magic.name}: "
            f"file={self.full_file_id}, "
            f"call={self.callback_id}, "
            f"time={self.timestamp}"
            "]"
        )


class FileMetadata(FileMetadataInterface[FileMetadataInterface]):
    """Metadata about the file contained inside Block 0 of a `FileBlockPayload`."""

    # Empty __slots__ allows keeping __slots__ (and not switching to __dict__):
    __slots__: List[str] = []

    def __init__(self,
                 callback_id: int,
                 timestamp: int,
                 file_type_magic: FileMetadata.FileTypeMagic,
                 raw: Optional[bytes] = None,
                 endianness_code: str = ENDIANNESS_CODE
                 ) -> None:
        self._callback_id = callback_id
        self._timestamp = timestamp
        self._file_type_magic = file_type_magic
        super().__init__(raw=raw, endianness_code=endianness_code)

    @classmethod
    def decode(cls,
               data: bytes,
               endianness_code: str = ENDIANNESS_CODE
               ) -> FileMetadata:
        """
        From C&TL at time of writing (10/01/2021):
        File Metadata Format:	(data in block 0)
        Field	        Type		Description
        callbackId	    uint16_t	Monotonically increasing callback ID of the command that triggered this file to be sent. Uniquely links this file to the command which requested it. Callback IDs are unique for images and UWB files. Callback IDs for commands requesting images start counting at 0. Callback IDs for commands requesting UWB files start counting at 2^5
        timestamp	    uint32_t	Time (in ms since Hercules power up) of when this file was generated.
        fileTypeMagic	uint8_t		[See below]

        File        Type Magic (in Metadata):	(data in block 0, inside File Metadata)
        Image:	    0x01
        UWB File:	0x0F
        """

        # Extract header and count up header_size as you go:
        ptr, size = 0, 2  # `callback_id` is 2B
        callback_id_bytes = data[ptr:size]
        callback_id: int = struct.unpack(
            endianness_code+'H', callback_id_bytes
        )[0]

        ptr, size = size, size+4  # `timestamp` is 4B
        timestamp_bytes = data[ptr:size]
        timestamp: int = struct.unpack(endianness_code+'L', timestamp_bytes)[0]

        # ptr, size = size, size+1  # `file_type_magic` is 1B
        # file_type_magic_val: int = int(data[ptr:size][0])
        # try:
        #     file_type_magic = FileMetadata.FileTypeMagic(file_type_magic_val)
        # except ValueError as e:
        #     # Invalid FTM received:
        #     logger.error((
        #         f"While decoding `FileMetadata` in a `FileBlockPayload` the "
        #         f"the received `file_type_magic` value of {file_type_magic_val}` "
        #         f"is not a valid `FileTypeMagic`. Valid values are: "
        #         f"{[f'{m.name}=0x{m.value:02x}' for m in FileMetadata.FileTypeMagic if m != FileMetadata.FileTypeMagic.BAD]}"
        #         f"\n For: FileMetadata{{callback_id={callback_id}, "
        #         f"timestamp={timestamp}, "
        #         f"file_type_magic_val={file_type_magic_val}}}"
        #         f"\n Packet data supplied to `FileMetadata` extractor: {data!r}"
        #     ))
        #     file_type_magic = FileMetadata.FileTypeMagic.BAD
        # ! TODO: FSW doesn't do filetype magic rn, so we'll just assume
        # ! everything's an image.
        file_type_magic = FileMetadata.FileTypeMagic.IMAGE

        # All data used by the metadata:
        data_used = data[:size]

        return cls(
            callback_id=callback_id,
            timestamp=timestamp,
            file_type_magic=file_type_magic,
            raw=data_used,
            endianness_code=endianness_code
        )

    def encode(self, **kwargs: Any) -> bytes:
        raise NotImplementedError()


class FileBlockPayloadInterface(DownlinkedPayload[PT], ABC):
    """
    Generic interface which defines all I/O for FileBlockPayload allowing it to be
    used as the Type Generic for FileBlockPayload, which in turn allows many of
    inherited functions to return FileBlockPayload.

    In essence, this defines any variables, properties, and methods which are
    unique to FileBlockPayload (and not inherited from its superclasses).
    In addition, properties and not just their interfaces are defined here for
    convenience sake (to reduce code bulk).

    Note: this is basically required because you can't directly reference a
    class as its own generic fill, e.g.:
    `class FileBlockPayload(DownlinkedPayload[EventPayload])`
    """

    __slots__: List[str] = [
        # A hash of the timestamp when file transmission started. Used to
        # identify which blocks belong to the same file if multiple files are
        # being sent at once. *NOT* a globally unique file identifier. Two files
        # over the course of the entire mission could have the same hashedId.
        # This is only unique up to the point that no two files with overlapping
        # transmission periods should have the same hashedId:
        '_hashed_id',
        # Total number of blocks in the file:
        '_total_blocks',
        # Number of this block (**1** indexed - 0 is for the metadata)
        '_block_number',
        # Number of bytes in this block (not including metadata block), as received (might not match `len(data)` if corrupted). Appears to be U32 in FSW as of 09/30/2021. Let's stick to this.
        '_length',
        # Data this block contains (* The data of each block will be compressed using heatshrink eventually):
        '_data',
        # If any possible sources of corruption were detected during loading that don't prevent the packet from being parsable:
        '_possible_corruption',
        # If this is block 0, this contains the decoded file metadata:
        '_file_metadata'
    ]
    _hashed_id: int
    _total_blocks: int
    _block_number: int
    _length: int
    _data: bytes
    _possible_corruption: bool
    _file_metadata: Optional[FileMetadata]

    # Make public get, private set to signal that you can freely use these values
    # but modifying them directly can yield undefined behavior (specifically
    # `raw` not syncing up with whatever other data is in the container)
    @property
    def hashed_id(self) -> int: return self._hashed_id
    @property
    def total_blocks(self) -> int: return self._total_blocks
    @property
    def block_number(self) -> int: return self._block_number
    @property
    def length(self) -> int: return self._length
    @property
    def data(self) -> bytes: return self._data
    @property
    def possible_corruption(self) -> bool: return self._possible_corruption

    @property
    def file_metadata(self) -> Optional[FileMetadata]:
        return self._file_metadata
    
    @property
    def is_metadata(self) -> bool: return self.file_metadata is not None

    def __str__(self) -> str:
        return (
            "FileBlock["
            f"{self.block_number}/{self.total_blocks}: "
            f"{self.length}B, "
            f"line={self.hashed_id}, "
            f"bad={self.possible_corruption}"
            "]"
        )


class FileBlockPayload(FileBlockPayloadInterface[FileBlockPayloadInterface]):
    """Implementation of Payload Interface for IRIS File Blocks."""

    # Empty __slots__ allows keeping __slots__ (and not switching to __dict__):
    __slots__: List[str] = []

    def __init__(self,
                 hashed_id: int,
                 total_blocks: int,
                 block_number: int,
                 length: int,
                 data: bytes,
                 possible_corruption: bool = False,
                 file_metadata: Optional[FileMetadata] = None,
                 magic: Magic = Magic.MISSING,
                 pathway: DataPathway = DataPathway.NONE,
                 source: DataSource = DataSource.NONE,
                 raw: Optional[bytes] = None,
                 endianness_code: str = ENDIANNESS_CODE
                 ) -> None:
        self._hashed_id = hashed_id
        self._total_blocks = total_blocks
        self._block_number = block_number
        self._length = length
        self._data = data
        self._possible_corruption = possible_corruption
        self._file_metadata = file_metadata
        super().__init__(
            magic=magic, pathway=pathway, source=source,
            raw=raw, endianness_code=endianness_code
        )

    @classmethod
    def decode(cls,
               data: bytes,
               endianness_code: str = ENDIANNESS_CODE
               ) -> FileBlockPayload:
        """
        From C&TL at time of writing (10/01/2021):
        File Block Format:
        Field	        Type		    Description
        hashedId	    uint16_t		A hash of the timestamp when file transmission started. Used to identify which blocks belong to the same file if multiple files are being sent at once. *NOT* a globally unique file identifier. Two files over the course of the entire mission could have the same hashedId. This is only unique up to the point that no two files with overlapping transmission periods should have the same hashedId.
        totalBlocks	    uint8_t		    Total number of blocks in this file (not including metadata block)
        blockNumber	    uint8_t		    Number of this block (**1 indexed** - 0 is for the metadata)
        length	        FileLength_t    Number of bytes in this block (not including header data - appears to be uint32_t in FSW as of 09/30/2021. Let's stick to this.)
        data	        uint8_t[]		All the data bytes. * The data of each block will be compressed using heatshrink
        """
        # ! TODO: Move `possible_corruption` to `PayloadInterface` so it's more broadly useful?
        # ! NOTE: ^- But then we'd have to make sure it's being used everywhere else that could be relevant...
        possible_corruption = False  # not yet at least... updated later on detection

        # Extract header and count up header_size as you go:
        ptr, header_size = 0, 2  # `hashed_id` is 2B
        hashed_id_bytes = data[ptr:header_size]
        hashed_id: int = struct.unpack(endianness_code+'H', hashed_id_bytes)[0]

        ptr, header_size = header_size, header_size+1  # `total_blocks` is 1B
        total_blocks: int = int(data[ptr:header_size][0])

        # ! TODO: `block_number` overflows rn due to packet count. Talk to FSW about this or guarantee not an issue during flight b/c there will be <255 blocks in any file (unlikely since blocks are lines):
        ptr, header_size = header_size, header_size+1  # `block_number` is 1B
        block_number: int = int(data[ptr:header_size][0])

        ptr, header_size = header_size, header_size+2  # `length` is 2B (U16)
        length_bytes = data[ptr:header_size]
        length: int = struct.unpack(endianness_code+'H', length_bytes)[0]

        max_length = (len(data)-header_size)
        if length > max_length:
            logger.error((
                f"While decoding a `FileBlockPayload` the length was read as "
                f"{length_bytes!r} = {length}B which is longer than the remaining "
                f"data in the packet after the file block header ({max_length}B). "
                "Has `FileLength_t` changed? Was the packet corrupted? For now, "
                "all data left in this packet will be assumed to be part of this "
                "block so a file can still be reconstructed even if it's a bit "
                "corrupted; but, note that this might not be an apt assumption. "
                f"\n For: FileBlockPayload{{hashed_id={hashed_id}, "
                f"block_num={block_number}, total_blocks={total_blocks}, "
                f"length={length}}}"
                f"\n Packet data supplied to `FileBlockPayload` extractor: "
                f"{data!r}"
            ))
            possible_corruption = True

        if block_number > total_blocks:
            logger.error((
                f"While decoding a `FileBlockPayload` the `{total_blocks=}` "
                f"but `{block_number=}`. Block number should range from 0 to "
                f"`total_blocks` (with 0 being metadata) but should never be "
                f"`>total_blocks`. Possible file corruption. "
                f"\n For: FileBlockPayload{{hashed_id={hashed_id}, "
                f"block_num={block_number}, total_blocks={total_blocks}, "
                f"length={length}}}"
                f"\n Packet data supplied to `FileBlockPayload` extractor: "
                f"{data!r}"
            ))
            possible_corruption = True

        # Extract data contained by the block:
        # TODO: De-heatshrink this once FSW starts using heatshrink (if they do)
        block_data = data[header_size: (header_size+length)]

        # All data used by this payload (so anything else left in `data` is part
        # of another payload in the VLP):
        data_used = data[:(header_size+length)]

        file_metadata: Optional[FileMetadata]
        if block_number == 0:
            # This is the metadata block:
            file_metadata = FileMetadata.decode(
                block_data, endianness_code
            )
        else:
            # This is an actual file_data block; so, no metadata here.
            file_metadata = None

        return cls(hashed_id=hashed_id,
                   total_blocks=total_blocks,
                   block_number=block_number,
                   length=length,
                   data=block_data,
                   possible_corruption=possible_corruption,
                   file_metadata=file_metadata,
                   raw=data_used,
                   endianness_code=endianness_code
                   )

    def encode(self, **kwargs: Any) -> bytes:
        raise NotImplementedError()
