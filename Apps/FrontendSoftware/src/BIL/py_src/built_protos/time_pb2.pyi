from google.protobuf import timestamp_pb2 as _timestamp_pb2
from google.protobuf.internal import enum_type_wrapper as _enum_type_wrapper
from google.protobuf import descriptor as _descriptor
from google.protobuf import message as _message
from typing import ClassVar as _ClassVar, Mapping as _Mapping, Optional as _Optional, Union as _Union

DESCRIPTOR: _descriptor.FileDescriptor
DLT_ARCHIVED_IN_DB: DownlinkTimeType
DLT_GENERATED: DownlinkTimeType
DLT_LANDER_RX: DownlinkTimeType
DLT_LANDER_TX: DownlinkTimeType
DLT_LOGGED_IN_DB: DownlinkTimeType
DLT_OSIRIS_RX: DownlinkTimeType
DLT_YAMCS_RX: DownlinkTimeType
ULT_CMD_EXECUTED: UplinkTimeType
ULT_CMD_FAILED: UplinkTimeType
ULT_CMD_PROCESSED: UplinkTimeType
ULT_CMD_PROCESSING: UplinkTimeType
ULT_CMD_WORKING: UplinkTimeType
ULT_GENERATED: UplinkTimeType
ULT_LANDER_RX: UplinkTimeType
ULT_LANDER_TX: UplinkTimeType
ULT_LOGGED_IN_DB: UplinkTimeType
ULT_OSIRIS_RX: UplinkTimeType
ULT_OSIRIS_TX: UplinkTimeType
ULT_ROVER_RX: UplinkTimeType
ULT_YAMCS_RX: UplinkTimeType
ULT_YAMCS_TX: UplinkTimeType

class IrisDownlinkTimestamp(_message.Message):
    __slots__ = ["time", "type"]
    TIME_FIELD_NUMBER: _ClassVar[int]
    TYPE_FIELD_NUMBER: _ClassVar[int]
    time: _timestamp_pb2.Timestamp
    type: DownlinkTimeType
    def __init__(self, time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., type: _Optional[_Union[DownlinkTimeType, str]] = ...) -> None: ...

class IrisUplinkTimestamp(_message.Message):
    __slots__ = ["time", "type"]
    TIME_FIELD_NUMBER: _ClassVar[int]
    TYPE_FIELD_NUMBER: _ClassVar[int]
    time: _timestamp_pb2.Timestamp
    type: UplinkTimeType
    def __init__(self, time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., type: _Optional[_Union[UplinkTimeType, str]] = ...) -> None: ...

class UplinkTimeType(int, metaclass=_enum_type_wrapper.EnumTypeWrapper):
    __slots__ = []

class DownlinkTimeType(int, metaclass=_enum_type_wrapper.EnumTypeWrapper):
    __slots__ = []
