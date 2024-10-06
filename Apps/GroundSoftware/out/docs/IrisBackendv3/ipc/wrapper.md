Module IrisBackendv3.ipc.wrapper
================================
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

Functions
---------

    
`async_read_from(socket: AsyncSocket) ‑> IrisBackendv3.ipc.ipc_payload.IpcPayload`
:   

    
`async_read_from_as(socket: AsyncSocket, message_type: Type[_IPMT]) ‑> Tuple[IrisBackendv3.ipc.ipc_payload.IpcPayload, ~_IPMT]`
:   Same as `async_read_from`, but instead of returning raw binary, it
    parses the data read as the given IPC `InterProcessMessage`.
    Returns a tuple containing the raw `IpcPayload` alongside the interpreted
    `InterProcessMessage`.

    
`async_read_msg(socket: AsyncSocket) ‑> Tuple[IrisBackendv3.ipc.ipc_payload.IpcPayload, IrisBackendv3.ipc.inter_process_message.InterProcessMessage]`
:   Asynchronously reads a message from this given socket.
    Only works when the payload has a valid `Topic`.
    Returns a tuple of the raw `IpcPayload` and the decoded
    `InterProcessMessage`.

    
`async_send_to(socket: AsyncSocket, data: Union[bytes, InterProcessMessage], subtopic: Optional[bytes] = None, topic: Optional[Topic] = None) ‑> None`
:   Sends the given `data` using the given `socket`.
    If a `topic` is given, the data will be published to that `topic` iff the
    `data` is allowed on the given `topic`.
    
    If `subtopic` bytes are given, they will be prepended to the data.
    This can be done even if `topic` isn't being used.

    
`create_async_context(*args, **kwargs) ‑> zmq.asyncio.Context`
:   

    
`create_async_socket(context: AsyncContext, socket_type: SocketType, ports: Port | List[Port], bind: Optional[bool] = None) ‑> zmq.asyncio.Socket`
:   Creates an `AsyncSocket` of the given `socket_type` on the given `context`
    and binds or connects it to the given port/ports. Binding occurs as needed.
    
    If `bind=True`, binding will be attempted no matter the `socket_type` (can
    be useful for inverse topologies with many pubs and one sub which binds).
    If `bind` is not given (`bind is None`), binding will be chosen by default
    behavior: server, xpub, and xsub bind, everything else just connects.
    
    NOTE: Bound sockets can only take one port. Unbound sockets that are not
    server or publisher can take multiple.

    
`create_context(*args, **kwargs) ‑> zmq.sugar.context.Context`
:   

    
`create_socket(context: Context, socket_type: SocketType, ports: Port | List[Port], bind: Optional[bool] = None) ‑> zmq.sugar.socket.Socket`
:   Creates a `Socket` of the given `socket_type` on the given `context` and
    binds or connects it to the given port/ports. Binding occurs as needed.
    
    If `bind=True`, binding will be attempted no matter the `socket_type` (can
    be useful for inverse topologies with many pubs and one sub which binds).
    If `bind` is not given (`bind is None`), binding will be chosen by default
    behavior: server, xpub, and xsub bind, everything else just connects.
    
    NOTE: Bound sockets can only take one port. Unbound sockets that are not
    server or publisher can take multiple.

    
`read_from(socket: Socket) ‑> IrisBackendv3.ipc.ipc_payload.IpcPayload`
:   

    
`read_from_as(socket: Socket, message_type: Type[_IPMT]) ‑> Tuple[IrisBackendv3.ipc.ipc_payload.IpcPayload, ~_IPMT]`
:   Same as `read_from`, but instead of returning raw binary, it
    parses the data read as the given IPC `InterProcessMessage`.
    Returns a tuple containing the raw `IpcPayload` alongside the interpreted
    `InterProcessMessage`.

    
`read_msg(socket: Socket) ‑> Tuple[IrisBackendv3.ipc.ipc_payload.IpcPayload, IrisBackendv3.ipc.inter_process_message.InterProcessMessage]`
:   Reads a message from this given socket.
    Only works when the payload has a valid `Topic`.
    Returns a tuple of the raw `IpcPayload` and the decoded
    `InterProcessMessage`.

    
`send_to(socket: Socket, data: Union[bytes, InterProcessMessage], subtopic: Optional[bytes] = None, topic: Optional[Topic] = None) ‑> None`
:   Sends the given `data` using the given `socket`.
    If a `topic` is given, the data will be published to that `topic` iff the
    `data` is allowed on the given `topic`.
    
    If `subtopic` bytes are given, they will be prepended to the data.
    This can be done even if `topic` isn't being used.

    
`start_proxy(inbound: Socket, outbound: Socket) ‑> None`
:   Starts a blocking proxy between the inbound and outbound sockets.
    Simple wrapper for ZMQ proxy.

    
`subscribe(socket: Socket | AsyncSocket, topics: Topic | List[Topic] | None) ‑> None`
:   Subscribes the given socket to the given topic(s).
    Subscribes to all topics on the port (removes any filters) if
    `topics is None`.

    
`validate_ipc_payload(ipc_payload: IpcPayload) ‑> None`
:   Checks the contents of an `IpcPayload`.

Classes
-------

`SocketType(value, names=None, *, module=None, qualname=None, type=None, start=1)`
:   Enumerates all possible socket types.

    ### Ancestors (in MRO)

    * enum.Enum

    ### Class variables

    `CLIENT`
    :

    `PUBLISHER`
    :

    `SERVER`
    :

    `SUBSCRIBER`
    :

    `XPUBLISHER`
    :

    `XSUBSCRIBER`
    :