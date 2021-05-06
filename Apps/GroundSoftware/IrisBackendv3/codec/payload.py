#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""
Defines Common Data Required for Payloads. Support for Building and Parsing
Payloads as part of a Variable Length Payload.

@author: Connor W. Colombo (CMU)
@last-updated: 05/06/2020
"""
from __future__ import annotations  # Activate postponed annotations (for using classes as return type in their own methods)

from typing import List, Any, Dict, Tuple, Optional, TypeVar, Type, NamedTuple, cast
from abc import ABC, abstractmethod, abstractclassmethod, abstractproperty
from enum import Enum
from collections import OrderedDict

import struct
from datetime import datetime

from .magic import Magic, MAGIC_SIZE
from .metadata import DataPathway, DataSource, DownlinkTimes, UplinkTimes
from .container import ContainerCodec
from .fsw_data_codec import encode as fsw_data_encode
from .fsw_data_codec import decode as fsw_data_decode

from .logging import logger

from .settings import ENDIANNESS_CODE, settings

from IrisBackendv3.data_standards import DataStandards
from IrisBackendv3.data_standards.module import Module, Command, TelemetryChannel, Event
from IrisBackendv3.data_standards import FswDataType
from IrisBackendv3.utils.basic import full_dict_spec_check

from .exceptions import PacketDecodingException


class PayloadCollection(NamedTuple):
    """
    Dictionary Collection of Lists of Payloads, separated by Payload Type.
    *NOTE:* All keys are strings and should accessed using class.__name__

    This means that, if class names are changed, these strings will have to be
    MANUALLY updated. This should be relatively easy since, although the linter
    won't detect the member name as outdated, it will detect the class name in
    the List[] type hint as outdated and all the developer has to do is make
    sure the two are the same.
    """
    CommandPayload: List[CommandPayload]
    TelemetryPayload: List[TelemetryPayload]
    EventPayload: List[EventPayload]
    FileBlockPayload: List[FileBlockPayload]

    @classmethod
    def make_empty(cls) -> PayloadCollection:
        """Makes a new empty payload collection."""
        return cls(
            CommandPayload=[],
            TelemetryPayload=[],
            EventPayload=[],
            FileBlockPayload=[]
        )


def extract_downlinked_payloads(
        VLP: bytes,
        pathway: DataPathway,
        source: DataSource,
        downlink_times: Optional[DownlinkTimes] = None,
        endianness_code: str = ENDIANNESS_CODE
) -> PayloadCollection:
    """Extract all Payloads from a Variable Length Payload. Should only be used
    for downlinked packets (nominal use does not involve deconstructing uplinked
    packets from bytes since they're packed here.)

    Per the Iris C&TL, the VLP should be structured as:
    [magic-0][payload-0][magic-1][payload-1]...[magic-N][payload-N]

    Args:
        VLP (bytes): Variable Length Payload
        pathway (DataPathway): How the VLP data got here (from the rover)
        source (DataSource): Where the Packet entered the backend

    Returns:
        PayloadCollection:
        Dictionary Collection of Lists of Payloads, separated by Payload Type.
        All payload types are included but some lists will be empty if no
        payload of that type was found in the given VLP.
    """
    # Dispatch table to map extracted magics to the corresponding payloads classes:
    payload_magic_dispatch_table: Dict[Magic, Type[Payload]] = {
        Magic.COMMAND: CommandPayload,
        Magic.WATCHDOG_COMMAND: WatchdogCommandPayload,
        Magic.TELEMETRY: TelemetryPayload,
        Magic.EVENT: EventPayload,
        Magic.FILE: FileBlockPayload
    }

    # Collection of Payload Lists:
    payloads: PayloadCollection = PayloadCollection(
        CommandPayload=[],
        TelemetryPayload=[],
        EventPayload=[],
        FileBlockPayload=[]
    )

    # Unpack Variable Length Payload:
    while len(VLP) > 0:
        # Strip off the magic:
        magic_bytes, VLP = VLP[:MAGIC_SIZE], VLP[MAGIC_SIZE:]
        magic = Magic.decode(magic_bytes, byte_order=endianness_code)

        if magic == Magic.MISSING:
            raise PacketDecodingException(
                magic_bytes,
                "This set of bytes corresponds to a missing magic value."
            )

        # Extract the payload and strip its bytes off the VLP:
        payload_type = payload_magic_dispatch_table[magic]

        if issubclass(payload_type, UplinkedPayload):
            raise PacketDecodingException(
                magic_bytes,
                "This payload magic was found in a packet being processed as a "
                "downlinked packet but its magic indicates it should be uplinked. "
                "Was the wrong magic used? Are you trying to unpack a packet to "
                "be uplinked?"
            )

        payload, VLP = payload_type.process(VLP, endianness_code)

        # Make sure there's data there:
        if len(payload.raw) == 0:
            raise PacketDecodingException(
                VLP,
                "Failed to extract any data from this VLP with magic type "
                f"{magic}. Was the wrong magic used?"
            )

        # Add on metadata:
        payload.magic = magic
        payload.pathway = pathway
        payload.source = source
        payload.downlink_times = downlink_times

        # Note: mypy complains that `logger` doesn't have a member `spam` but it does:
        logger.spam(f"Successfully extracted {payload}")  # type: ignore

        # Add payload to collection:
        getattr(payloads, payload.__class__.__name__).append(payload)

    return payloads


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

    # Empty __slots__ allows keeping __slots__ (and not switching to __dict__):
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
        VLP = VLP[payload_size:]

        return payload, VLP

    def __getstate__(self) -> Dict[str, Any]:
        return {
            'magic': self.magic,
            'pathway': self.pathway,
            'source': self.source
        }

    def __setstate__(self, data: Dict[str, Any]) -> None:
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
                 endianness_code: str = ENDIANNESS_CODE
                 ) -> None:
        # Pass through data to parent and initialize metadata values.
        self.amcc_ack = False
        self.rover_ack = False
        self.uplink_times = None
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

        # Then exract the state data managed by this class:
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
            if arg.datatype == FswDataType.ENUM:
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
                 endianness_code: str = ENDIANNESS_CODE
                 ) -> None:
        self._module_id = module_id
        self._command_id = command_id
        self._args = args
        self.check_args()
        super().__init__(
            magic=magic, pathway=pathway, source=source,
            raw=raw, endianness_code=endianness_code
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
        total_arg_len = 0
        for arg in command.args:
            #! TODO: Maybe move this checker into the appropriate Module class?
            arg_len = arg.datatype.num_octets
            total_arg_len += arg_len
            arg_bytes = remaining[:arg_len]
            args[arg.name] = fsw_data_decode(
                arg.datatype, arg_bytes
            )
            remaining = remaining[arg_len:]

            # Convert to enum name string if type is enum:
            if arg.datatype == FswDataType.ENUM:
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

        data_used = data[:(2+total_arg_len)]

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
            if arg.datatype == FswDataType.ENUM and isinstance(arg_val, str):
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
        total_arg_len = 0
        for arg in command.args:
            #! TODO: Maybe move this checker into the appropriate Module class?
            arg_len = arg.datatype.num_octets
            total_arg_len += arg_len
            arg_bytes = remaining[:arg_len]
            # Watchdog takes U8 Enums instead of U32:
            eff_datatype = arg.datatype if arg.datatype != FswDataType.ENUM else FswDataType.U8
            args[arg.name] = fsw_data_decode(
                eff_datatype, arg_bytes
            )
            remaining = remaining[arg_len:]

            # Convert to enum name string if type is enum:
            if arg.datatype == FswDataType.ENUM:
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

        data_used = data[:(2+total_arg_len)]

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
            if arg.datatype == FswDataType.ENUM and isinstance(arg_val, str):
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
                 endianness_code: str = ENDIANNESS_CODE
                 ) -> None:
        # Pass through data to parent and initialize metadata values.
        self.downlink_times = None
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

        # Then exract the state data managed by this class:
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
        if self.channel.datatype == FswDataType.ENUM:
            if isinstance(self.data, str):
                val = f"{self.data}[{self.channel.get_enum_value(self.data)}]"
            else:
                val = f"{self.channel.get_enum_name(self.data)}[{self.data}]"
        else:
            val = self.data

        return f"{self.module.name}{{{self.channel.name}}}@{self.timestamp} = {self.data}"

    def __repr__(self) -> str:
        # Enums could be int or str, so convert to what's not given and show both:
        if self.channel.datatype == FswDataType.ENUM:
            if isinstance(self.data, str):
                val = f"{self.data}[{self.channel.get_enum_value(self.data)}]"
            else:
                val = f"{self.channel.get_enum_name(self.data)}[{self.data}]"
        else:
            val = self.data

        return f"{self.module}{{{self.channel}}}@{self.timestamp} = {self.data}"

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

        core_data_len = channel.datatype.num_octets
        core_data = fsw_data_decode(
            channel.datatype, remaining[:core_data_len]
        )

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
        if self.channel.datatype == FswDataType.ENUM and isinstance(self.data, str):
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
        raise NotImplementedError()


class EventPayload(EventPayloadInterface[EventPayloadInterface]):
    """Implementation of Payload Interface for IRIS Event Payloads."""

    # Empty __slots__ allows keeping __slots__ (and not switching to __dict__):
    __slots__: List[str] = []

    def __str__(self) -> str:
        return self.formatted_string

    def __repr__(self) -> str:
        return f"{self.event.name}{self.args} -> '{self.formatted_string}'"

    def __init__(self,
                 module_id: int,
                 event_id: int,
                 args: OrderedDict[str, Any],
                 timestamp: int,
                 magic: Magic = Magic.MISSING,
                 pathway: DataPathway = DataPathway.NONE,
                 source: DataSource = DataSource.NONE,
                 raw: Optional[bytes] = None,
                 endianness_code: str = ENDIANNESS_CODE
                 ) -> None:
        self._module_id = module_id
        self._event_id = event_id
        self._args = args
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
        raise NotImplementedError()

    def encode(self, **kwargs: Any) -> bytes:
        raise NotImplementedError()


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
        raise NotImplementedError()

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
        # Number of bytes in this block:
        '_length',
        # Data this block contains:
        '_data',
        # If this is block 0, this contains the decoded file metadata:
        '_file_metadata'
    ]
    _hashed_id: bytes
    _total_blocks: int
    _block_number: int
    _length: int
    _data: bytes
    _file_metadata: Optional[FileMetadata]

    # Make public get, private set to signal that you can freely use these values
    # but modifying them directly can yield undefined behavior (specifically
    # `raw` not syncing up with whatever other data is in the container)
    @property
    def hashed_id(self) -> bytes: return self._hashed_id
    @property
    def total_blocks(self) -> int: return self._total_blocks
    @property
    def block_number(self) -> int: return self._block_number
    @property
    def length(self) -> int: return self._length
    @property
    def data(self) -> bytes: return self._data

    @property
    def file_metadata(self) -> Optional[FileMetadata]:
        return self._file_metadata


class FileBlockPayload(FileBlockPayloadInterface[FileBlockPayloadInterface]):
    """Implementation of Payload Interface for IRIS File Blocks."""

    # Empty __slots__ allows keeping __slots__ (and not switching to __dict__):
    __slots__: List[str] = []

    def __init__(self,
                 hashed_id: bytes,
                 total_blocks: int,
                 block_number: int,
                 length: int,
                 data: bytes,
                 file_metadata: Optional[FileMetadata],
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
        raise NotImplementedError()

    def encode(self, **kwargs: Any) -> bytes:
        raise NotImplementedError()
