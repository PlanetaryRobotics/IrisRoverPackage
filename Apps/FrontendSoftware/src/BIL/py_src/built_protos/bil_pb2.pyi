import time_pb2 as _time_pb2
import db_pb2 as _db_pb2
from google.protobuf.internal import containers as _containers
from google.protobuf.internal import enum_type_wrapper as _enum_type_wrapper
from google.protobuf import descriptor as _descriptor
from google.protobuf import message as _message
from typing import ClassVar as _ClassVar, Iterable as _Iterable, Mapping as _Mapping, Optional as _Optional, Union as _Union

CONNECTED_TO_DB: BilStatus
CONNECTING_DB: BilStatus
CRASHED: BilStatus
DESCRIPTOR: _descriptor.FileDescriptor

class BilData(_message.Message):
    __slots__ = ["bil_status", "cmd_ack", "cmd_update", "event", "image", "telem"]
    BIL_STATUS_FIELD_NUMBER: _ClassVar[int]
    CMD_ACK_FIELD_NUMBER: _ClassVar[int]
    CMD_UPDATE_FIELD_NUMBER: _ClassVar[int]
    EVENT_FIELD_NUMBER: _ClassVar[int]
    IMAGE_FIELD_NUMBER: _ClassVar[int]
    TELEM_FIELD_NUMBER: _ClassVar[int]
    bil_status: BilStatus
    cmd_ack: InternalCommandAck
    cmd_update: CommandUpdate
    event: _db_pb2.Event
    image: Image
    telem: _db_pb2.TelemItem
    def __init__(self, bil_status: _Optional[_Union[BilStatus, str]] = ..., cmd_ack: _Optional[_Union[InternalCommandAck, _Mapping]] = ..., cmd_update: _Optional[_Union[CommandUpdate, _Mapping]] = ..., telem: _Optional[_Union[_db_pb2.TelemItem, _Mapping]] = ..., event: _Optional[_Union[_db_pb2.Event, _Mapping]] = ..., image: _Optional[_Union[Image, _Mapping]] = ...) -> None: ...

class BilDataRequest(_message.Message):
    __slots__ = ["all", "dl_time_span", "latest", "lookupId_span", "stream_name_filter", "type", "ul_time_span", "ulid_span"]
    class Type(int, metaclass=_enum_type_wrapper.EnumTypeWrapper):
        __slots__ = []
    class All(_message.Message):
        __slots__ = ["all"]
        ALL_FIELD_NUMBER: _ClassVar[int]
        all: bool
        def __init__(self, all: bool = ...) -> None: ...
    class DownlinkTimeSpan(_message.Message):
        __slots__ = ["end", "start"]
        END_FIELD_NUMBER: _ClassVar[int]
        START_FIELD_NUMBER: _ClassVar[int]
        end: _time_pb2.IrisDownlinkTimestamp
        start: _time_pb2.IrisDownlinkTimestamp
        def __init__(self, start: _Optional[_Union[_time_pb2.IrisDownlinkTimestamp, _Mapping]] = ..., end: _Optional[_Union[_time_pb2.IrisDownlinkTimestamp, _Mapping]] = ...) -> None: ...
    class Latest(_message.Message):
        __slots__ = ["num"]
        NUM_FIELD_NUMBER: _ClassVar[int]
        num: int
        def __init__(self, num: _Optional[int] = ...) -> None: ...
    class LookupIdSpan(_message.Message):
        __slots__ = ["end", "start"]
        END_FIELD_NUMBER: _ClassVar[int]
        START_FIELD_NUMBER: _ClassVar[int]
        end: int
        start: int
        def __init__(self, start: _Optional[int] = ..., end: _Optional[int] = ...) -> None: ...
    class UlidSpan(_message.Message):
        __slots__ = ["end", "start"]
        END_FIELD_NUMBER: _ClassVar[int]
        START_FIELD_NUMBER: _ClassVar[int]
        end: str
        start: str
        def __init__(self, start: _Optional[str] = ..., end: _Optional[str] = ...) -> None: ...
    class UplinkTimeSpan(_message.Message):
        __slots__ = ["end", "start"]
        END_FIELD_NUMBER: _ClassVar[int]
        START_FIELD_NUMBER: _ClassVar[int]
        end: _time_pb2.IrisUplinkTimestamp
        start: _time_pb2.IrisUplinkTimestamp
        def __init__(self, start: _Optional[_Union[_time_pb2.IrisUplinkTimestamp, _Mapping]] = ..., end: _Optional[_Union[_time_pb2.IrisUplinkTimestamp, _Mapping]] = ...) -> None: ...
    ALL_FIELD_NUMBER: _ClassVar[int]
    CommandRecord: BilDataRequest.Type
    DL_TIME_SPAN_FIELD_NUMBER: _ClassVar[int]
    Events: BilDataRequest.Type
    Images: BilDataRequest.Type
    LATEST_FIELD_NUMBER: _ClassVar[int]
    LOOKUPID_SPAN_FIELD_NUMBER: _ClassVar[int]
    STREAM_NAME_FILTER_FIELD_NUMBER: _ClassVar[int]
    TYPE_FIELD_NUMBER: _ClassVar[int]
    Telemetry: BilDataRequest.Type
    ULID_SPAN_FIELD_NUMBER: _ClassVar[int]
    UL_TIME_SPAN_FIELD_NUMBER: _ClassVar[int]
    all: BilDataRequest.All
    dl_time_span: BilDataRequest.DownlinkTimeSpan
    latest: BilDataRequest.Latest
    lookupId_span: BilDataRequest.LookupIdSpan
    stream_name_filter: str
    type: BilDataRequest.Type
    ul_time_span: BilDataRequest.UplinkTimeSpan
    ulid_span: BilDataRequest.UlidSpan
    def __init__(self, type: _Optional[_Union[BilDataRequest.Type, str]] = ..., stream_name_filter: _Optional[str] = ..., all: _Optional[_Union[BilDataRequest.All, _Mapping]] = ..., ul_time_span: _Optional[_Union[BilDataRequest.UplinkTimeSpan, _Mapping]] = ..., dl_time_span: _Optional[_Union[BilDataRequest.DownlinkTimeSpan, _Mapping]] = ..., ulid_span: _Optional[_Union[BilDataRequest.UlidSpan, _Mapping]] = ..., lookupId_span: _Optional[_Union[BilDataRequest.LookupIdSpan, _Mapping]] = ..., latest: _Optional[_Union[BilDataRequest.Latest, _Mapping]] = ...) -> None: ...

class BilDataRequestResponse(_message.Message):
    __slots__ = ["data", "success"]
    DATA_FIELD_NUMBER: _ClassVar[int]
    SUCCESS_FIELD_NUMBER: _ClassVar[int]
    data: _containers.RepeatedCompositeFieldContainer[BilData]
    success: bool
    def __init__(self, success: bool = ..., data: _Optional[_Iterable[_Union[BilData, _Mapping]]] = ...) -> None: ...

class BilIngest(_message.Message):
    __slots__ = ["command", "data_request"]
    COMMAND_FIELD_NUMBER: _ClassVar[int]
    DATA_REQUEST_FIELD_NUMBER: _ClassVar[int]
    command: _db_pb2.Command
    data_request: BilDataRequest
    def __init__(self, data_request: _Optional[_Union[BilDataRequest, _Mapping]] = ..., command: _Optional[_Union[_db_pb2.Command, _Mapping]] = ...) -> None: ...

class CommandUpdate(_message.Message):
    __slots__ = ["dbid", "lookupId", "times"]
    DBID_FIELD_NUMBER: _ClassVar[int]
    LOOKUPID_FIELD_NUMBER: _ClassVar[int]
    TIMES_FIELD_NUMBER: _ClassVar[int]
    dbid: _db_pb2.DatabaseId
    lookupId: int
    times: _containers.RepeatedCompositeFieldContainer[_time_pb2.IrisUplinkTimestamp]
    def __init__(self, dbid: _Optional[_Union[_db_pb2.DatabaseId, _Mapping]] = ..., lookupId: _Optional[int] = ..., times: _Optional[_Iterable[_Union[_time_pb2.IrisUplinkTimestamp, _Mapping]]] = ...) -> None: ...

class Image(_message.Message):
    __slots__ = ["b64_encoding", "dbid", "description", "file_name", "timestamps"]
    B64_ENCODING_FIELD_NUMBER: _ClassVar[int]
    DBID_FIELD_NUMBER: _ClassVar[int]
    DESCRIPTION_FIELD_NUMBER: _ClassVar[int]
    FILE_NAME_FIELD_NUMBER: _ClassVar[int]
    TIMESTAMPS_FIELD_NUMBER: _ClassVar[int]
    b64_encoding: str
    dbid: _db_pb2.DatabaseId
    description: str
    file_name: str
    timestamps: _containers.RepeatedCompositeFieldContainer[_time_pb2.IrisDownlinkTimestamp]
    def __init__(self, dbid: _Optional[_Union[_db_pb2.DatabaseId, _Mapping]] = ..., b64_encoding: _Optional[str] = ..., timestamps: _Optional[_Iterable[_Union[_time_pb2.IrisDownlinkTimestamp, _Mapping]]] = ..., file_name: _Optional[str] = ..., description: _Optional[str] = ...) -> None: ...

class InternalCommandAck(_message.Message):
    __slots__ = ["dbid", "lookupId", "timestamps"]
    DBID_FIELD_NUMBER: _ClassVar[int]
    LOOKUPID_FIELD_NUMBER: _ClassVar[int]
    TIMESTAMPS_FIELD_NUMBER: _ClassVar[int]
    dbid: _db_pb2.DatabaseId
    lookupId: int
    timestamps: _containers.RepeatedCompositeFieldContainer[_time_pb2.IrisUplinkTimestamp]
    def __init__(self, dbid: _Optional[_Union[_db_pb2.DatabaseId, _Mapping]] = ..., lookupId: _Optional[int] = ..., timestamps: _Optional[_Iterable[_Union[_time_pb2.IrisUplinkTimestamp, _Mapping]]] = ...) -> None: ...

class PingMessage(_message.Message):
    __slots__ = ["payload"]
    PAYLOAD_FIELD_NUMBER: _ClassVar[int]
    payload: str
    def __init__(self, payload: _Optional[str] = ...) -> None: ...

class BilStatus(int, metaclass=_enum_type_wrapper.EnumTypeWrapper):
    __slots__ = []
