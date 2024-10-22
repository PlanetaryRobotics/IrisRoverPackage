"""High-level UDP endpoints for asyncio with typing and support for external
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
"""
from __future__ import annotations

from typing import Any, Callable, Awaitable, Type, Tuple, TypeAlias, TypeVar, overload

import asyncio
from asyncio.transports import DatagramTransport
import socket
import logging

_AddrType: TypeAlias = Tuple[str | Any, int]  # based on asyncio.protocols

# Datagram protocol


class DatagramEndpointProtocol(asyncio.DatagramProtocol):
    """Datagram protocol for the endpoint high-level interface."""

    _endpoint: Endpoint
    logger: logging.Logger

    def __init__(
        self,
        endpoint: Endpoint,
        logger: logging.Logger
    ) -> None:
        self._endpoint = endpoint
        self.logger = logger

    # Protocol methods

    def connection_made(  # type: ignore[override]
        self,
        transport: DatagramTransport
    ) -> None:
        """Type signature (and non-kosher override) taken from
        `asyncio/protocols.pyi`."""
        self._endpoint.transport = transport

    def connection_lost(self, exc: Exception | None) -> None:
        assert exc is None
        if self._endpoint._write_ready_future is not None:
            self._endpoint._write_ready_future.set_result(None)
        self._endpoint.close()

    # Datagram protocol methods

    def datagram_received(
        self,
        data: bytes,
        addr: _AddrType
    ) -> None:
        self.logger.debug(f"Got dgram for {addr}: {data!r}")
        self._endpoint.feed_datagram(data, addr)

    def error_received(self, exc: Exception) -> None:
        msg = 'Endpoint received an error: {!r}'
        self.logger.warning(msg.format(exc))

    # Workflow control

    def pause_writing(self) -> None:
        assert self._endpoint._write_ready_future is None
        if not hasattr(self._endpoint.transport, '_loop'):
            raise IOError(
                "Transport doesn't have FlowControlMixin but "
                "`pause_writing` called."
            )
        loop = self._endpoint.transport._loop
        self._endpoint._write_ready_future = loop.create_future()

    def resume_writing(self) -> None:
        assert self._endpoint._write_ready_future is not None
        self._endpoint._write_ready_future.set_result(None)
        self._endpoint._write_ready_future = None


# Endpoint classes

class Endpoint:
    """High-level interface for UDP endpoints.

    Can either be local or remote.
    It is initialized with an optional queue size for the incoming datagrams.
    """

    logger: logging.Logger
    _transport: DatagramTransport | None
    _queue: asyncio.Queue[Tuple[bytes | None, _AddrType | None]]
    _closed: bool
    _write_ready_future: asyncio.Future | None

    def __init__(self, logger: logging.Logger, queue_size=None):
        self.logger = logger
        if queue_size is None:
            queue_size = 0
        self._queue = asyncio.Queue(queue_size)
        self._closed = False
        self._transport = None
        self._write_ready_future = None

    # Protocol callbacks

    def feed_datagram(
        self,
        data: bytes | None,
        addr: _AddrType | None
    ) -> None:
        try:
            self._queue.put_nowait((data, addr))
        except asyncio.QueueFull:
            self.logger.error('Endpoint queue is full')

    def close(self) -> None:
        # Manage flag
        if self._closed:
            return
        self._closed = True
        # Wake up
        if self._queue.empty():
            self.feed_datagram(None, None)
        # Close transport
        if self.transport:
            self.transport.close()

    # User methods

    def send(self, data: bytes, addr: _AddrType):
        """Send a datagram to the given address."""
        if self._closed:
            raise IOError("Endpoint is closed.")
        if self.transport is None:
            raise IOError("Endpoint transport not established.")
        self.transport.sendto(data, addr)

    async def receive(self) -> Tuple[bytes, _AddrType]:
        """Wait for an incoming datagram and return it with
        the corresponding address.

        This method is a coroutine.
        """
        if self._queue.empty() and self._closed:
            raise IOError("Endpoint is closed")
        data, addr = await self._queue.get()
        if data is None:
            raise IOError("Endpoint is closed")
        if addr is None:
            raise IOError("Bad address received")
        return data, addr

    def abort(self):
        """Close the transport immediately."""
        if self._closed:
            raise IOError("Endpoint is closed")
        self.transport.abort()
        self.close()

    async def drain(self):
        """Drain the transport buffer below the low-water mark."""
        if self._write_ready_future is not None:
            await self._write_ready_future

    # Properties

    @property
    def transport(self) -> DatagramTransport:
        """Getter for Transport that makes sure Transport has been
        established."""
        if self._transport is None:
            raise IOError("Endpoint transport not established.")
        return self._transport

    @transport.setter
    def transport(self, transport: DatagramTransport) -> None:
        self._transport = transport

    @property
    def address(self):
        """The endpoint address as a (host, port) tuple."""
        return self.transport.get_extra_info("socket").getsockname()

    @property
    def closed(self):
        """Indicates whether the endpoint is closed or not."""
        return self._closed


class LocalEndpoint(Endpoint):
    """High-level interface for UDP local endpoints.

    It is initialized with an optional queue size for the incoming datagrams.
    """
    pass


class RemoteEndpoint(Endpoint):
    """High-level interface for UDP remote endpoints.

    It is initialized with an optional queue size for the incoming datagrams.
    """

    def send(self, data):
        """Send a datagram to the remote host."""
        super().send(data, None)

    async def receive(self):
        """ Wait for an incoming datagram from the remote host.

        This method is a coroutine.
        """
        data, addr = await super().receive()
        return data


_EndpointType = TypeVar('_EndpointType', bound=Endpoint)

EndpointFactory: TypeAlias = Callable[[logging.Logger], _EndpointType]

# High-level coroutines


@overload
async def open_datagram_endpoint(
    host: str,
    port: int,
    *,
    logger: logging.Logger,
    endpoint_factory: Type[Endpoint] = Endpoint,
    remote=False,
    sock: socket.socket | None = None,
    **kwargs
) -> Endpoint:
    ...


@overload
async def open_datagram_endpoint(
    host: str,
    port: int,
    *,
    logger: logging.Logger,
    endpoint_factory: EndpointFactory[_EndpointType],
    remote=False,
    sock: socket.socket | None = None,
    **kwargs
) -> _EndpointType:
    ...


async def open_datagram_endpoint(
    host: str,
    port: int,
    *,
    logger: logging.Logger,
    endpoint_factory: Type[Endpoint] | EndpointFactory[_EndpointType] = Endpoint,
    remote=False,
    sock: socket.socket | None = None,
    **kwargs
) -> Endpoint | _EndpointType:
    """Open and return a datagram endpoint.

    The default endpoint factory is the Endpoint class.
    The endpoint can be made local or remote using the remote argument.
    Extra keyword arguments are forwarded to `loop.create_datagram_endpoint`.

    Will use a premade socket `sock` if specified.
    """
    loop = asyncio.get_event_loop()
    endpoint = endpoint_factory(logger)
    kwargs['protocol_factory'] = lambda: DatagramEndpointProtocol(
        endpoint=endpoint,
        logger=logger
    )
    if sock is not None:
        kwargs['sock'] = sock
    else:
        kwargs['remote_addr' if remote else 'local_addr'] = host, port
    await loop.create_datagram_endpoint(**kwargs)
    return endpoint


async def open_local_endpoint(
    host: str = '0.0.0.0',
    port: int = 0,
    *,
    logger: logging.Logger,
    queue_size: int | None = None,
    sock: socket.socket | None = None,
    **kwargs
) -> LocalEndpoint:
    """Open and return a local datagram endpoint.

    An optional queue size argument can be provided.
    Extra keyword arguments are forwarded to `loop.create_datagram_endpoint`.

    Will use a premade socket `sock` if specified.
    """
    return await open_datagram_endpoint(
        host, port,
        remote=False,
        logger=logger,
        endpoint_factory=lambda logger: LocalEndpoint(
            logger,
            queue_size=queue_size
        ),
        sock=sock,
        **kwargs
    )


async def open_remote_endpoint(
    host: str,
    port: int,
    *,
    logger: logging.Logger,
    queue_size: int | None = None,
    sock: socket.socket | None = None,
    **kwargs
) -> RemoteEndpoint:
    """Open and return a remote datagram endpoint.

    An optional queue size argument can be provided.
    Extra keyword arguments are forwarded to `loop.create_datagram_endpoint`.

    Will use a premade socket `sock` if specified.
    """
    return await open_datagram_endpoint(
        host, port,
        remote=True,
        logger=logger,
        endpoint_factory=lambda logger: RemoteEndpoint(
            logger=logger,
            queue_size=queue_size
        ),
        sock=sock,
        **kwargs
    )


# Testing

if __name__ == "__main__":  # pragma: no cover
    try:
        import pytest
        pytestmark = pytest.mark.asyncio
    except ImportError:  # pragma: no cover
        pass

    async def test_standard_behavior():
        local = await open_local_endpoint()
        remote = await open_remote_endpoint(*local.address)

        remote.send(b'Hey Hey')
        data, address = await local.receive()

        assert data == b'Hey Hey'
        assert address == remote.address

        local.send(b'My My', address)
        data = await remote.receive()
        assert data == b'My My'

        local.abort()
        assert local.closed

        with pytest.warns(UserWarning):
            await asyncio.sleep(1e-3)
            remote.send(b'U there?')
            await asyncio.sleep(1e-3)

        remote.abort()
        assert remote.closed

    async def test_closed_endpoint():
        local = await open_local_endpoint()
        future = asyncio.ensure_future(local.receive())
        local.abort()
        assert local.closed

        with pytest.raises(IOError):
            await future

        with pytest.raises(IOError):
            await local.receive()

        with pytest.raises(IOError):
            await local.send(b'test', ('localhost', 8888))

        with pytest.raises(IOError):
            local.abort()

    async def test_queue_size():
        local = await open_local_endpoint(queue_size=1)
        remote = await open_remote_endpoint(*local.address)

        remote.send(b'1')
        remote.send(b'2')
        with pytest.warns(UserWarning):
            await asyncio.sleep(1e-3)
            assert await local.receive() == (b'1', remote.address)
        remote.send(b'3')
        assert await local.receive() == (b'3', remote.address)

        remote.send(b'4')
        await asyncio.sleep(1e-3)
        local.abort()
        assert local.closed
        assert await local.receive() == (b'4', remote.address)

        remote.abort()
        assert remote.closed

    async def test_flow_control():
        m = n = 1024
        remote = await open_remote_endpoint("8.8.8.8", 12345)

        for _ in range(m):
            remote.send(b"a" * n)

        await remote.drain()

        for _ in range(m):
            remote.send(b"a" * n)

        remote.abort()
        await remote.drain()

    pytest.main([__file__])  # pragma: no cover
