Module IrisBackendv3.transceiver.utils.asyncio_udp
==================================================
High-level UDP endpoints for asyncio with typing and support for external
loggers.

Adapted from `aioudp.py` by `vxgmichel` from 
`https://gist.github.com/vxgmichel/e47bff34b68adb3cf6bd4845c4bed448`,
accessed on 06/22/2023.

Example:

async def main():

    # Create a local UDP endpoint
    local = await open_local_endpoint('localhost', 8888)

    # Create a remote UDP endpoint, pointing to the first one
    remote = await open_remote_endpoint(*local.address)

    # The remote endpoint sends a datagram
    remote.send(b'Hey Hey, My My')

    # The local endpoint receives the datagram, along with the address
    data, address = await local.receive()

    # This prints: Got 'Hey Hey, My My' from 127.0.0.1 port 8888
    print(f"Got {data!r} from {address[0]} port {address[1]}")
    
Last Modified: 06/01/2023

Functions
---------

    
`open_datagram_endpoint(host: str, port: int, *, logger: logging.Logger, endpoint_factory: Type[Endpoint] | EndpointFactory[_EndpointType] = IrisBackendv3.transceiver.utils.asyncio_udp.Endpoint, remote=False, sock: socket.socket | None = None, **kwargs) ‑> Union[IrisBackendv3.transceiver.utils.asyncio_udp.Endpoint, ~_EndpointType]`
:   Open and return a datagram endpoint.
    
    The default endpoint factory is the Endpoint class.
    The endpoint can be made local or remote using the remote argument.
    Extra keyword arguments are forwarded to `loop.create_datagram_endpoint`.
    
    Will use a premade socket `sock` if specified.

    
`open_local_endpoint(host: str = '0.0.0.0', port: int = 0, *, logger: logging.Logger, queue_size: int | None = None, sock: socket.socket | None = None, **kwargs) ‑> IrisBackendv3.transceiver.utils.asyncio_udp.LocalEndpoint`
:   Open and return a local datagram endpoint.
    
    An optional queue size argument can be provided.
    Extra keyword arguments are forwarded to `loop.create_datagram_endpoint`.
    
    Will use a premade socket `sock` if specified.

    
`open_remote_endpoint(host: str, port: int, *, logger: logging.Logger, queue_size: int | None = None, sock: socket.socket | None = None, **kwargs) ‑> IrisBackendv3.transceiver.utils.asyncio_udp.RemoteEndpoint`
:   Open and return a remote datagram endpoint.
    
    An optional queue size argument can be provided.
    Extra keyword arguments are forwarded to `loop.create_datagram_endpoint`.
    
    Will use a premade socket `sock` if specified.

Classes
-------

`DatagramEndpointProtocol(endpoint: Endpoint, logger: logging.Logger)`
:   Datagram protocol for the endpoint high-level interface.

    ### Ancestors (in MRO)

    * asyncio.protocols.DatagramProtocol
    * asyncio.protocols.BaseProtocol

    ### Class variables

    `logger: logging.Logger`
    :

    ### Methods

    `connection_lost(self, exc: Exception | None) ‑> None`
    :   Called when the connection is lost or closed.
        
        The argument is an exception object or None (the latter
        meaning a regular EOF is received or the connection was
        aborted or closed).

    `connection_made(self, transport: DatagramTransport) ‑> None`
    :   Type signature (and non-kosher override) taken from
        `asyncio/protocols.pyi`.

    `datagram_received(self, data: bytes, addr: _AddrType) ‑> None`
    :   Called when some datagram is received.

    `error_received(self, exc: Exception) ‑> None`
    :   Called when a send or receive operation raises an OSError.
        
        (Other than BlockingIOError or InterruptedError.)

    `pause_writing(self) ‑> None`
    :   Called when the transport's buffer goes over the high-water mark.
        
        Pause and resume calls are paired -- pause_writing() is called
        once when the buffer goes strictly over the high-water mark
        (even if subsequent writes increases the buffer size even
        more), and eventually resume_writing() is called once when the
        buffer size reaches the low-water mark.
        
        Note that if the buffer size equals the high-water mark,
        pause_writing() is not called -- it must go strictly over.
        Conversely, resume_writing() is called when the buffer size is
        equal or lower than the low-water mark.  These end conditions
        are important to ensure that things go as expected when either
        mark is zero.
        
        NOTE: This is the only Protocol callback that is not called
        through EventLoop.call_soon() -- if it were, it would have no
        effect when it's most needed (when the app keeps writing
        without yielding until pause_writing() is called).

    `resume_writing(self) ‑> None`
    :   Called when the transport's buffer drains below the low-water mark.
        
        See pause_writing() for details.

`Endpoint(logger: logging.Logger, queue_size=None)`
:   High-level interface for UDP endpoints.
    
    Can either be local or remote.
    It is initialized with an optional queue size for the incoming datagrams.

    ### Descendants

    * IrisBackendv3.transceiver.utils.asyncio_udp.LocalEndpoint
    * IrisBackendv3.transceiver.utils.asyncio_udp.RemoteEndpoint

    ### Class variables

    `logger: logging.Logger`
    :

    ### Instance variables

    `address`
    :   The endpoint address as a (host, port) tuple.

    `closed`
    :   Indicates whether the endpoint is closed or not.

    `transport: asyncio.transports.DatagramTransport`
    :   Getter for Transport that makes sure Transport has been
        established.

    ### Methods

    `abort(self)`
    :   Close the transport immediately.

    `close(self) ‑> None`
    :

    `drain(self)`
    :   Drain the transport buffer below the low-water mark.

    `feed_datagram(self, data: bytes | None, addr: _AddrType | None) ‑> None`
    :

    `receive(self) ‑> Tuple[bytes, Tuple[Union[str, Any], int]]`
    :   Wait for an incoming datagram and return it with
        the corresponding address.
        
        This method is a coroutine.

    `send(self, data: bytes, addr: _AddrType)`
    :   Send a datagram to the given address.

`LocalEndpoint(logger: logging.Logger, queue_size=None)`
:   High-level interface for UDP local endpoints.
    
    It is initialized with an optional queue size for the incoming datagrams.

    ### Ancestors (in MRO)

    * IrisBackendv3.transceiver.utils.asyncio_udp.Endpoint

`RemoteEndpoint(logger: logging.Logger, queue_size=None)`
:   High-level interface for UDP remote endpoints.
    
    It is initialized with an optional queue size for the incoming datagrams.

    ### Ancestors (in MRO)

    * IrisBackendv3.transceiver.utils.asyncio_udp.Endpoint

    ### Methods

    `receive(self)`
    :   Wait for an incoming datagram from the remote host.
        
        This method is a coroutine.

    `send(self, data)`
    :   Send a datagram to the remote host.