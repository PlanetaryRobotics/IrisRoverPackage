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
@last-updated: 04/29/2023
"""
# Activate postponed annotations (for using classes as return type in their own methods)
from __future__ import annotations
from typing import Optional, Union, List, cast, Tuple, Type, TypeVar, TypeGuard
import dataclasses
from enum import Enum

import zmq
import zmq.asyncio

from IrisBackendv3.ipc.port import Port
from IrisBackendv3.ipc.topics_registry import Topic
from IrisBackendv3.ipc.settings import settings
from IrisBackendv3.ipc.logs import logger

from IrisBackendv3.ipc.ipc_payload import IpcPayload
from IrisBackendv3.ipc.inter_process_message import InterProcessMessage


class SocketType(Enum):
    """
    Enumerates all possible socket types.
    """
    SERVER = 0x00, zmq.REP, False, False
    CLIENT = 0x01, zmq.REQ, True, False
    PUBLISHER = 0x02, zmq.PUB, False, False
    SUBSCRIBER = 0x03, zmq.SUB, True, True
    XPUBLISHER = 0x12, zmq.XPUB, False, False
    XSUBSCRIBER = 0x13, zmq.XSUB, True, True

    # Instance attributes for type-checker:
    _zmq_type: int
    _multiple_ports_allowed: bool
    _can_subscribe: bool

    def __new__(
        cls,
        val: int,
        zmq_type: int,
        multiple_ports_allowed: bool,
        can_subscribe: bool
    ):
        """Constructs a new instance of the Enum."""
        obj = object.__new__(cls)
        obj._value_ = val
        obj._zmq_type = zmq_type
        obj._multiple_ports_allowed = multiple_ports_allowed
        obj._can_subscribe = can_subscribe

        return obj


Context = zmq.sugar.context.Context  # convenient type alias
AsyncContext = zmq.asyncio.Context  # convenient type alias


def create_context(*args, **kwargs) -> Context:
    return zmq.Context(*args, **kwargs)


def create_async_context(*args, **kwargs) -> AsyncContext:
    return zmq.asyncio.Context(*args, **kwargs)


Socket = zmq.sugar.socket.Socket  # convenient type alias
AsyncSocket = zmq.asyncio.Socket  # convenient type alias


def _create_socket(
    context: Context | AsyncContext,
    socket_type: SocketType,
    ports: Port | List[Port],
    bind: Optional[bool] = None
) -> Union[Socket, AsyncSocket]:
    """
    Creates a socket of the given `socket_type` on the given `context` and
    binds or connects it to the given port/ports. Binding occurs as needed.

    If `bind=True`, binding will be attempted no matter the `socket_type` (can
    be useful for inverse topologies with many pubs and one sub which binds).
    If `bind` is not given (`bind is None`), binding will be chosen by default
    behavior: servers & pubs bind, clients & subs don't.

    NOTE: Bound sockets can only take one port. Unbound sockets that are not
    server or publisher can take multiple.
    """
    if isinstance(ports, Port):
        ports = [ports]
    elif not (isinstance(ports, list) and all(isinstance(p, Port) for p in ports)):
        logger.error(
            "TypeError: "
            "`ports` must be a `Port` or a `list` containing only `Port`s. "
            f"instead got: `{ports}` of type `{type(ports)}`."
        )

    # Make sure socket type is on whitelist of socket types we support
    # (types we've tested this setup with):
    if socket_type not in (whitelist := [
            SocketType.SERVER,
            SocketType.PUBLISHER,
            SocketType.XPUBLISHER,
            SocketType.CLIENT,
            SocketType.SUBSCRIBER,
            SocketType.XSUBSCRIBER
    ]):
        logger.error(
            "ValueError: "
            f"Unsupported/unimplemented `{socket_type=}`."
        )

    # Create socket:
    socket = context.socket(socket_type._zmq_type)

    # Make XPUB sockets verbose so un/subscription notification forwarding
    # occurs for use in proxies (the only place we use XPUBs):
    if socket_type == SocketType.XPUBLISHER:
        socket.setsockopt(zmq.XPUB_VERBOSER, True)

    # Determine appropriate binding approach:
    if bind is None:
        # Use default binding (look it up):
        bind = {
            SocketType.SERVER: True,
            SocketType.PUBLISHER: True,
            SocketType.XPUBLISHER: True,
            SocketType.CLIENT: False,
            SocketType.SUBSCRIBER: False,
            SocketType.XSUBSCRIBER: False
        }[socket_type]
        logger.verbose(
            f"Using default binding scheme (`{bind=}`) for `{socket_type=}`."
        )
    else:
        # Overriding the default. Make note of it:
        logger.info(
            f"Using forced binding scheme (`{bind=}`) for `{socket_type=}`."
        )

    # Start the socket and either bind or connect:
    if bind:
        # Bind Servers and Publishers:
        if len(ports) > 1:
            logger.warn(
                f"Attempting to bind with `{socket_type=}` in `_create_socket`"
                f"but multiple `{ports=}` were given. Need exactly 1."
                f"Proceeding with the first port in the list: `{ports[0]=}`."
            )
        port = ports[0]
        addr = f"tcp://{settings['IP']}:{port.value}"
        socket.bind(addr)
        logger.info(  # type: ignore
            f"Bound `{socket_type}` at `{addr}` "
            f"(port {port!s}: 0x{port.value:04X})."
        )
    else:
        # If we're not binding, then we're connecting:
        # Make sure that we're not connecting to multiple ports if this is a
        # server, publisher, etc where that's not allowed on a single socket:
        if len(ports) > 1 and not socket_type._multiple_ports_allowed:
            logger.warn(
                f"Socket with `{socket_type=}` given multiple ports in "
                f"`_create_socket` (`{ports=}`) but exactly 1 should be given."
                f"Proceeding with the first port in the list: `{ports[0]=}`."
            )
            ports = ports[:1]

        # Connect to all ports given:
        addr_base = f"tcp://{settings['IP']}"
        for port in ports:
            socket.connect(f"{addr_base}:{port.value}")

        logger.info(  # type: ignore
            f"Created a `{socket_type}` connected to `{addr_base}` "
            f"on ports `{ports}` "
            f"(`{', '.join([f'0x{p.value:04X}' for p in ports])}`)."
        )

    # Indicate that the return type (sync vs async) is unknown since the
    # context type (sync vs async) is also unknown:
    return cast(Socket | AsyncSocket, socket)


def create_socket(
    context: Context,
    socket_type: SocketType,
    ports: Port | List[Port],
    bind: Optional[bool] = None
) -> Socket:
    """
    Creates a `Socket` of the given `socket_type` on the given `context` and
    binds or connects it to the given port/ports. Binding occurs as needed.

    If `bind=True`, binding will be attempted no matter the `socket_type` (can
    be useful for inverse topologies with many pubs and one sub which binds).
    If `bind` is not given (`bind is None`), binding will be chosen by default
    behavior: servers & pubs bind, clients & subs don't.

    NOTE: Bound sockets can only take one port. Unbound sockets that are not
    server or publisher can take multiple.
    """
    return cast(Socket, _create_socket(context, socket_type, ports, bind))


def create_async_socket(
    context: AsyncContext,
    socket_type: SocketType,
    ports: Port | List[Port],
    bind: Optional[bool] = None
) -> AsyncSocket:
    """
    Creates an `AsyncSocket` of the given `socket_type` on the given `context`
    and binds or connects it to the given port/ports. Binding occurs as needed.

    If `bind=True`, binding will be attempted no matter the `socket_type` (can
    be useful for inverse topologies with many pubs and one sub which binds).
    If `bind` is not given (`bind is None`), binding will be chosen by default
    behavior: servers & pubs bind, clients & subs don't.

    NOTE: Bound sockets can only take one port. Unbound sockets that are not
    server or publisher can take multiple.
    """
    return cast(AsyncSocket, _create_socket(context, socket_type, ports, bind))


def subscribe(
    socket: Socket | AsyncSocket,
    topics: Topic | List[Topic] | None,
) -> None:
    """
    Subscribes the given socket to the given topic(s).
    Subscribes to all topics on the port (removes any filters) if
    `topics is None`.
    """
    sub_whitelist = [st for st in SocketType if st._can_subscribe]
    if socket.socket_type not in [st._zmq_type for st in sub_whitelist]:
        logger.error(
            "ValueError: "
            f"Attempted to subscribe to topic(s) `{topics}` using a socket "
            f"with invalid type ({socket.socket_type}). "
            f"Only sockets with types `{sub_whitelist}` can subscribe to a "
            f"topic."
        )

    if topics is None:
        # Subscribe to all topics (remove any topic filters):
        socket.setsockopt(zmq.SUBSCRIBE, b'')
        socket.subscribe(b'')
    else:
        if isinstance(topics, Topic):
            topics = [topics]
        elif not (isinstance(topics, list) and all(isinstance(t, Topic) for t in topics)):
            logger.error(
                "TypeError: "
                "`topics` must be a `Topic` or a `list` containing only `Topic`s. "
                f"instead got: `{topics}` of type `{type(topics)}`."
            )

        for topic in topics:
            socket.setsockopt(zmq.SUBSCRIBE, topic.value)
            socket.subscribe(topic.value)


def start_proxy(inbound: Socket, outbound: Socket) -> None:
    """Starts a blocking proxy between the inbound and outbound sockets.
    Simple wrapper for ZMQ proxy."""
    zmq.proxy(inbound, outbound)


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
                f"This topic allows: {topic.definition.topic_type}."
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


def read_from(socket: Socket) -> IpcPayload:
    raw: List[bytes] = socket.recv_multipart()
    return _process_multipart(raw)


async def async_read_from(socket: AsyncSocket) -> IpcPayload:
    raw: List[bytes] = await socket.recv_multipart()
    return _process_multipart(raw)


def read_msg(socket: Socket) -> Tuple[IpcPayload, InterProcessMessage]:
    """Reads a message from this given socket.
    Only works when the payload has a valid `Topic`.
    Returns a tuple of the raw `IpcPayload` and the decoded
    `InterProcessMessage`."""
    payload = read_from(socket)
    return payload, payload.message


async def async_read_msg(
    socket: AsyncSocket
) -> Tuple[IpcPayload, InterProcessMessage]:
    """Asynchronously reads a message from this given socket.
    Only works when the payload has a valid `Topic`.
    Returns a tuple of the raw `IpcPayload` and the decoded
    `InterProcessMessage`."""
    payload = await async_read_from(socket)
    return payload, payload.message


_IPMT = TypeVar('_IPMT', bound=InterProcessMessage)


def read_from_as(
    socket: Socket,
    message_type: Type[_IPMT]
) -> Tuple[IpcPayload, _IPMT]:
    """Same as `read_from`, but instead of returning raw binary, it
    parses the data read as the given IPC `InterProcessMessage`.
    Returns a tuple containing the raw `IpcPayload` alongside the interpreted
    `InterProcessMessage`.
    """
    payload = read_from(socket)
    message = message_type.from_ipc_bytes(payload.msg_bytes)
    return (payload, message)


async def async_read_from_as(
    socket: AsyncSocket,
    message_type: Type[_IPMT]
) -> Tuple[IpcPayload, _IPMT]:
    """Same as `async_read_from`, but instead of returning raw binary, it
    parses the data read as the given IPC `InterProcessMessage`.
    Returns a tuple containing the raw `IpcPayload` alongside the interpreted
    `InterProcessMessage`.
    """
    payload = await async_read_from(socket)
    message = message_type.from_ipc_bytes(payload.msg_bytes)
    return (payload, message)
