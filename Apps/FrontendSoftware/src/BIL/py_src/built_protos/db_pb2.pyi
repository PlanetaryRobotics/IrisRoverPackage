import time_pb2 as _time_pb2
from google.protobuf.internal import containers as _containers
from google.protobuf.internal import enum_type_wrapper as _enum_type_wrapper
from google.protobuf import descriptor as _descriptor
from google.protobuf import message as _message
from typing import ClassVar as _ClassVar, Iterable as _Iterable, Mapping as _Mapping, Optional as _Optional, Union as _Union

DESCRIPTOR: _descriptor.FileDescriptor

class Command(_message.Message):
    __slots__ = ["args", "name", "timestamps", "type", "ulid"]
    class CommandType(int, metaclass=_enum_type_wrapper.EnumTypeWrapper):
        __slots__ = []
    ALIAS_COMMAND: Command.CommandType
    ARGS_FIELD_NUMBER: _ClassVar[int]
    FSW_COMMAND: Command.CommandType
    META_COMMAND: Command.CommandType
    NAME_FIELD_NUMBER: _ClassVar[int]
    TIMESTAMPS_FIELD_NUMBER: _ClassVar[int]
    TYPE_FIELD_NUMBER: _ClassVar[int]
    ULID_FIELD_NUMBER: _ClassVar[int]
    args: _containers.RepeatedCompositeFieldContainer[CommandArg]
    name: str
    timestamps: _containers.RepeatedCompositeFieldContainer[_time_pb2.IrisUplinkTimestamp]
    type: Command.CommandType
    ulid: str
    def __init__(self, ulid: _Optional[str] = ..., type: _Optional[_Union[Command.CommandType, str]] = ..., name: _Optional[str] = ..., args: _Optional[_Iterable[_Union[CommandArg, _Mapping]]] = ..., timestamps: _Optional[_Iterable[_Union[_time_pb2.IrisUplinkTimestamp, _Mapping]]] = ...) -> None: ...

class CommandArg(_message.Message):
    __slots__ = ["name", "val"]
    NAME_FIELD_NUMBER: _ClassVar[int]
    VAL_FIELD_NUMBER: _ClassVar[int]
    name: str
    val: DataUnitValue
    def __init__(self, name: _Optional[str] = ..., val: _Optional[_Union[DataUnitValue, _Mapping]] = ...) -> None: ...

class DataUnitValue(_message.Message):
    __slots__ = ["float_val", "int_val", "str_val", "string_val"]
    FLOAT_VAL_FIELD_NUMBER: _ClassVar[int]
    INT_VAL_FIELD_NUMBER: _ClassVar[int]
    STRING_VAL_FIELD_NUMBER: _ClassVar[int]
    STR_VAL_FIELD_NUMBER: _ClassVar[int]
    float_val: float
    int_val: int
    str_val: str
    string_val: str
    def __init__(self, str_val: _Optional[str] = ..., int_val: _Optional[int] = ..., float_val: _Optional[float] = ..., string_val: _Optional[str] = ...) -> None: ...

class DatabaseId(_message.Message):
    __slots__ = ["lookupId", "ulid"]
    LOOKUPID_FIELD_NUMBER: _ClassVar[int]
    ULID_FIELD_NUMBER: _ClassVar[int]
    lookupId: int
    ulid: str
    def __init__(self, ulid: _Optional[str] = ..., lookupId: _Optional[int] = ...) -> None: ...

class Event(_message.Message):
    __slots__ = ["args", "dbid", "event_name", "timestamps"]
    ARGS_FIELD_NUMBER: _ClassVar[int]
    DBID_FIELD_NUMBER: _ClassVar[int]
    EVENT_NAME_FIELD_NUMBER: _ClassVar[int]
    TIMESTAMPS_FIELD_NUMBER: _ClassVar[int]
    args: _containers.RepeatedCompositeFieldContainer[EventArg]
    dbid: DatabaseId
    event_name: str
    timestamps: _containers.RepeatedCompositeFieldContainer[_time_pb2.IrisDownlinkTimestamp]
    def __init__(self, dbid: _Optional[_Union[DatabaseId, _Mapping]] = ..., event_name: _Optional[str] = ..., args: _Optional[_Iterable[_Union[EventArg, _Mapping]]] = ..., timestamps: _Optional[_Iterable[_Union[_time_pb2.IrisDownlinkTimestamp, _Mapping]]] = ...) -> None: ...

class EventArg(_message.Message):
    __slots__ = ["name", "val"]
    NAME_FIELD_NUMBER: _ClassVar[int]
    VAL_FIELD_NUMBER: _ClassVar[int]
    name: str
    val: DataUnitValue
    def __init__(self, name: _Optional[str] = ..., val: _Optional[_Union[DataUnitValue, _Mapping]] = ...) -> None: ...

class TelemItem(_message.Message):
    __slots__ = ["channel_name", "dbid", "timestamps", "val"]
    CHANNEL_NAME_FIELD_NUMBER: _ClassVar[int]
    DBID_FIELD_NUMBER: _ClassVar[int]
    TIMESTAMPS_FIELD_NUMBER: _ClassVar[int]
    VAL_FIELD_NUMBER: _ClassVar[int]
    channel_name: str
    dbid: DatabaseId
    timestamps: _containers.RepeatedCompositeFieldContainer[_time_pb2.IrisDownlinkTimestamp]
    val: DataUnitValue
    def __init__(self, dbid: _Optional[_Union[DatabaseId, _Mapping]] = ..., channel_name: _Optional[str] = ..., val: _Optional[_Union[DataUnitValue, _Mapping]] = ..., timestamps: _Optional[_Iterable[_Union[_time_pb2.IrisDownlinkTimestamp, _Mapping]]] = ...) -> None: ...
