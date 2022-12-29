# -*- coding: utf-8 -*-
"""
Wraps ZeroMQ (ZMQ/0MQ) for IPC connections. All ZMQ functions are wrapped so
that this interface can be changed later if needed to support new requirements
without disrupting the rest of the
system.

Only the interior of this module should have any idea that zmq is being used.
Anything outside of this wrapper should not be affected by the fact that ZMQ is
being used for IPC. That is, if we ever decide to migrate away from ZMQ for IPC,
the only area that should **need** to be changed would be this file.

@author: Connor W. Colombo (CMU)
@last-updated: 11/22/2022
"""
# Activate postponed annotations (for using classes as return type in their own methods)
from __future__ import annotations
from abc import ABC, abstractmethod, abstractclassmethod
from typing import Any, Awaitable, Callable, Generic, Optional, ClassVar, Protocol, Union, Dict, List, cast, TypeVar, Tuple
from typing_extensions import TypeAlias
import dataclasses
from enum import Enum
from typeguard import check_type

import zmq
import zmq.asyncio

from .port import Port
from .topic import Topic
from .settings import settings
from .logging import logger


IPMC = TypeVar('IPMC')  # Inter-Process Message Content


class InterProcessMessage(Generic[IPMC], ABC):
    """
    Interface for any message data which supports being sent between processes.
    """

    __slots__: List[str] = ['content']

    content: IPMC

    def __init__(self, content: IPMC) -> None:
        self.content = content

    @abstractmethod
    def to_ipc_bytes(self) -> bytes:
        """
        Pack this object into bytes to be sent over the IPC network
        (in a safe way, unlike pickle).
        """
        # ... use restricted_pickler
        raise NotImplementedError()

    @abstractclassmethod
    def from_ipc_bytes(cls, data: bytes) -> IPMC:
        """
        Unpack bytes sent over IPC to reconstruct the sent object.
        """
        raise NotImplementedError()


class IPMHandler(Protocol):
    """ Structural typing `Protocol` defining a handler that decodes raw
    IPC message bytes, performs some operation, and optionally returns an
    `InterProcessMessage` if needed by the process type (i.e. server/client).

    NOTE:
    Can't do something as simple as `Callable[[bytes], InterProcessMessage])`
    b/c of this still open (as of 2/26/22) issue:
    https://github.com/python/mypy/issues/5485
    which prevents `subtopic.handler(ipc_raw.msg)` or even
    `subtopic.handler.__call__(ipc_raw.msg)` from working.
    """

    def __call__(self, __raw_msg: bytes) -> Optional[InterProcessMessage]: ...


def IsIPMHandler(func: IPMHandler) -> IPMHandler:
    """ Decorator to use before a function to have `mypy` make sure it complies
    with the `IPMHandler` callback protocol.
    Example:
    ```py
    @IsIPMHandler
    def good_ipmh(msg_bytes: bytes) -> InterProcessMessage:
        pass
    ```
    creates no errors.

    ```py
    @IsIPMHandler
    def bad_ipmh(x: int, y: Dict) -> float:
        pass
    ```
    gives a mypy error b/c it's argument and return type signatures are wrong.

    but
    """
    return func


@dataclasses.dataclass(order=True)
class IpcPayload:
    """
    Data sent over IPC.
    """
    topic_bytes: bytes = b''  # if applicable
    subtopic_bytes: bytes = b''  # if applicable
    msg_bytes: bytes = b''  # message

    @property
    def topic(self) -> Topic:
        # Type ignore note: mypy doesn't recognize that Enum() doesn't call
        # __new__ but is instead an Enum indexing operation and, thus,
        # doesn't need __new__'s arguments.
        return Topic(self.topic_bytes)  # type: ignore


class SocketType(Enum):
    """
    Enumerates all possible socket types.
    """
    SERVER = 0x00, zmq.REP
    CLIENT = 0x01, zmq.REQ
    PUBLISHER = 0x02, zmq.PUB
    SUBSCRIBER = 0x03, zmq.SUB

    # Instance attributes for type-checker:
    _zmq_type: int

    def __new__(cls, val: int, zmq_type: int):
        """Constructs a new instance of the Enum."""
        obj = object.__new__(cls)
        obj._value_ = val
        obj._zmq_type = zmq_type

        return obj


Context = zmq.sugar.context.Context  # convenient type alias
AsyncContext = zmq.asyncio.Context  # convenient type alias


def create_context(*args, **kwargs) -> Context:
    return zmq.Context(*args, **kwargs)


def create_async_context(*args, **kwargs) -> AsyncContext:
    return zmq.asyncio.Context(*args, **kwargs)


Socket = zmq.sugar.socket.Socket  # convenient type alias
AsyncSocket = zmq.asyncio.Socket  # convenient type alias


def create_socket(
    context: Context,
    socket_type: SocketType,
    ports: Union[Port, List[Port]]
) -> Socket:
    return cast(Socket, _create_socket(context, socket_type, ports))


def create_async_socket(
    context: AsyncContext,
    socket_type: SocketType,
    ports: Union[Port, List[Port]]
) -> AsyncSocket:
    return cast(AsyncSocket, _create_socket(context, socket_type, ports))


def _create_socket(
    context: Union[Context, AsyncContext],
    socket_type: SocketType,
    ports: Union[Port, List[Port]]
) -> Union[Socket, AsyncSocket]:
    """
    Creates a socket of the given `socket_type` on the given `context` and binds
    or connects it to the given port/ports.

    Note: servers and publishers are to be bound and so they can only take
    one port. Clients and subscribers can connect to multiple ports.
    """

    if isinstance(ports, Port):
        ports = [ports]
    elif not (isinstance(ports, list) and all(isinstance(p, Port) for p in ports)):
        logger.error(
            "TypeError: "
            "`ports` must be a `Port` or a `list` containing only `Port`s. "
            f"instead got: `{ports}` of type `{type(ports)}`."
        )

    # Create socket:
    socket = context.socket(socket_type._zmq_type)

    if socket_type in [SocketType.SERVER, socket_type.PUBLISHER]:
        # Bind Servers and Publishers:
        if len(ports) > 1:
            logger.error(
                "ValueError: "
                f"Sockets with type `{socket_type}` are bound and can only take "
                f"one port but were given `{ports}`."
            )

        port = ports[0]
        addr = f"tcp://{settings['IP']}:{port.value}"
        socket.bind(addr)

        logger.info(  # type: ignore
            f"Created a `{socket_type}` at `{addr}` "
            f"(port {port!s}: 0x{port.value:04X})."
        )

    elif socket_type in [SocketType.CLIENT, socket_type.SUBSCRIBER]:
        # Connect Clients and Subscribers:
        for port in ports:
            socket.connect(f"tcp://{settings['IP']}:{port}")

        logger.info(  # type: ignore
            f"Created a `{socket_type}` connected to `tcp://{settings['IP']}` on ports `{ports}`."
        )

    else:
        logger.error(
            "ValueError: "
            f"Unsupported/unimplemented socket type. Got {socket_type}."
        )

    return cast(Socket, socket)


def subscribe(socket: Union[Socket, AsyncSocket], topics: Union[Topic, List[Topic]]) -> None:
    """
    Subscribes the given socket to the given topic(s).
    """
    if socket.socket_type != SocketType.SUBSCRIBER._zmq_type:
        logger.error(
            "ValueError: "
            f"Attempted to subscribe to topic(s) `{topics}` using a socket "
            f"with invalid type ({socket.socket_type}). "
            f"Only sockets with type {SocketType.SUBSCRIBER} can subscribe to a topic."
        )

    if isinstance(topics, Topic):
        topics = [topics]
    elif not (isinstance(topics, list) and all(isinstance(t, Topic) for t in topics)):
        logger.error(
            "TypeError: "
            "`ports` must be a `Topic` or a `list` containing only `Topic`s. "
            f"instead got: `{topics}` of type `{type(topics)}`."
        )

    for topic in topics:
        socket.setsockopt(zmq.SUBSCRIBE, topic.value)
        socket.subscribe(topic.value)


def _prep_send_payload(
    data: Union[bytes, InterProcessMessage],
    subtopic: Optional[bytes],
    topic: Optional[Topic]
) -> IpcPayload:
    """Prepares for a send operation by creating an `IpcPayload` from the given
    data. This is a helper for the various `send` functions.

    If a `topic` is given, the data will be published to that `topic` iff the
    `data` is allowed on the given `topic`.

    If `subtopic` bytes are given, they will be prepended to the data.
    This can be done even if `topic` isn't being used.
    """
    payload = IpcPayload()

    # Add topic if applicable:
    if topic is not None:
        # NOTE: Publishing to a topic just entails leading with the topic bytes.
        # So, anyone (any socket) can do it (not necessary to limit just to `PUBLISHER`s).
        if not topic.allows(data):
            logger.error(
                "ValueError: "
                f"Given data `{data!r}` is not allowed on topic `{topic}` "
                f"because its type `{type(data)}` is not a supported type. "
                f"This topic allows: {topic.topic_type}."
            )
        payload.topic_bytes = topic.value

    # Add any subtopic bytes if applicable:
    if subtopic is not None:
        payload.subtopic_bytes = subtopic

    # Extract the core data and add it to the message
    if isinstance(data, bytes):
        payload.msg_bytes = data
    elif isinstance(data, InterProcessMessage):
        payload.msg_bytes = data.to_ipc_bytes()
    else:
        logger.error(
            "ValueError: "
            "`data` needs to be `bytes` or an instance of a class which "
            "implements `InterProcessMessage`. "
            f"Instead got `{data}` of type `{type(data)}`."
        )

    return payload


def send_to(
    socket: Socket,
    data: Union[bytes, InterProcessMessage],
    subtopic: Optional[bytes] = None,
    topic: Optional[Topic] = None
) -> None:
    """
    Sends the given `data` using the given `socket`.
    If a `topic` is given, the data will be published to that `topic` iff the
    `data` is allowed on the given `topic`.

    If `subtopic` bytes are given, they will be prepended to the data.
    This can be done even if `topic` isn't being used.
    """
    payload = _prep_send_payload(data, subtopic, topic)
    socket.send_multipart(dataclasses.astuple(payload))


async def async_send_to(
    socket: AsyncSocket,
    data: Union[bytes, InterProcessMessage],
    subtopic: Optional[bytes] = None,
    topic: Optional[Topic] = None
) -> None:
    """
    Sends the given `data` using the given `socket`.
    If a `topic` is given, the data will be published to that `topic` iff the
    `data` is allowed on the given `topic`.

    If `subtopic` bytes are given, they will be prepended to the data.
    This can be done even if `topic` isn't being used.
    """
    payload = _prep_send_payload(data, subtopic, topic)
    await socket.send_multipart(dataclasses.astuple(payload))


def validate_ipc_payload(ipc_payload: IpcPayload) -> None:
    """Checks the contents of an `IpcPayload`."""
    # Make sure the topic is a topic that exists at all:
    try:
        # Attempt to extract a valid `Topic` from `topic_bytes`:
        _ = ipc_payload.topic
    except ValueError as e:
        logger.error(
            "ValueError: "
            f"Bad topic bytes received. "
            f"Got `0x{ipc_payload.topic_bytes!r:04X}`. "
            f"Valid topics are: {[t for t in Topic]} ."
        )


def _process_multipart(raw: List[bytes]) -> IpcPayload:
    """Helper for various `read` functions that processes a multipart message."""
    payload: IpcPayload

    # Make sure multipart message is valid:
    if len(raw) != 3:
        logger.warning(
            "Invalid multipart message received. "
            f"Should have three parts but instead it has {len(raw)} part(s). "
            f"Received: {raw}. "
        )
        return IpcPayload()

    # Extract data:
    payload = IpcPayload(*raw)

    # Make sure data is valid:
    validate_ipc_payload(payload)

    return payload


def read_from(
    socket: Socket
) -> IpcPayload:
    raw: List[bytes] = socket.recv_multipart()
    return _process_multipart(raw)


async def async_read_from(
    socket: AsyncSocket
) -> IpcPayload:
    raw: List[bytes] = await socket.recv_multipart()
    return _process_multipart(raw)
