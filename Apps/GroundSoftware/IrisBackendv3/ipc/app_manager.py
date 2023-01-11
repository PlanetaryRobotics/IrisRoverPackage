# -*- coding: utf-8 -*-
"""
High-level IPC manager classes to simplify developing IPC Apps.

At this level, all IPC interface operations have been abstracted. For the
low-level IPC interface and implementation, see `wrapper.py`.

@author: Connor W. Colombo (CMU)
@last-updated: 12/29/2022
"""
# Activate postponed annotations (for using classes as return type in their own methods)
from __future__ import annotations
from abc import ABC, abstractmethod
from typing import Any, Awaitable, Callable, Generic, Optional, ClassVar, Protocol, Dict, List, cast, TypeVar, Tuple
from typing_extensions import TypeAlias
from types import NoneType
import dataclasses

from .wrapper import (
    Context, AsyncContext,
    create_context, create_async_context,
    Socket, AsyncSocket,
    create_socket, create_async_socket,
    subscribe,
    SocketType,
    IpcPayload,
    InterProcessMessage, IPMHandler,
    read_from, async_read_from,
    send_to, async_send_to,
)

from .port import Port
from .topic import Topic
from .settings import settings
from .logging import logger
from .exceptions import UnhandledTopicException


def _subscriber_dispatch_handler(
    payload: IpcPayload,
    topic_dispatch_table: Dict[Topic, IPMHandler]
) -> None:
    """Helper function that passes the `raw_data` of the given `IpcPayload`
    into the handler function found by looking up `payload.topic` in the given
    `topic_dispatch_table`.

    Args:
        payload (IpcPayload): Payload to handle.
        topic_dispatch_table (Dict[Topic, IPMHandler]): Maps topic to handlers.
    """
    topic = payload.topic

    # Make sure a handler exists for this topic:
    if topic not in topic_dispatch_table:
        logger.error(
            "ValueError: "
            f"Bad topic found. "
            f"Got `{topic}` "
            f"but dispatch table only has handlers for: "
            f"{[n for n in topic_dispatch_table.keys()]}"
        )


# Context Type (sync or async):
_CT = TypeVar('_CT', bound=(Context | AsyncContext), covariant=True)
# Socket Type (sync or async):
_ST = TypeVar('_ST', bound=(Socket | AsyncSocket), covariant=True)
# Handler Type:
# NOTE: as of Python 3.10 'SocketHandler' has to be a string b/c
# forward declaration. Don't worry, mypy still works correctly with this.
_HT = TypeVar('_HT', bound='SocketHandler', covariant=True)
# SocketHandlerType: same thing as `_HT`, but no variance specifiers (for self-reference)
_SHT = TypeVar('_SHT', bound='SocketHandler')


# Handler return type (options for sync and async versions):
# NOTE: PayloadHandler here refers to both SocketHandlers and TopicHandlers
# since they both handle payloads and have the same return type.
# NOTE: Currently we return nothing. Handler just does whatever it does then
# returns. BUT, this is repeated in a lot of places so it's abstracted here for
# easy changeability if necessary.
PayloadHandlerResponseType: TypeAlias = None
PayloadHandlerResponseTypeAsync: TypeAlias = Awaitable[PayloadHandlerResponseType]
_HRT = TypeVar('_HRT', bound=(
    PayloadHandlerResponseType |
    PayloadHandlerResponseTypeAsync
))


# App Manager Type:
# NOTE: as of Python 3.10 'IpcAppManager' has to be a string b/c forward
# declaration. Don't worry, mypy still works correctly with this.
_AMT = TypeVar('_AMT', bound='IpcAppManager')


class SocketHandler(Generic[_HRT, _AMT]):
    """
    Abstract base class for a functor to handle new payloads sent to a
    socket, managed by an `IpcAppManager`.

    Example usage:
    ```py
        class SomeSocketHandler(SocketHandler):
            def __call__(
                self,
                manager: IpcAppManager,
                payload: IpcPayload
            ) -> Optional[InterProcessMessage]:
                # do something with payload. maybe use manager to send a response.


        manager = IpcAppManagerSync(
            socket_specs={
                'sub_sock': IpcAppManager.SocketSpec(
                    sock_type=ipc.SocketType.SUBSCRIBER,
                    port=1234,
                    topics=[ipc.Topic.SOME_TOPIC, ipc.Topic.ANOTHER_TOPIC],
                    rx_handler=SomeSocketHandler()
                )
            }
        )
    ```

    It's done this way with inheritance, not just with generic callables, so
    that the handlers can act as functors which can manage an internal state if
    desired. Example:

    ```py
        class AStatefulSocketHandler(SocketHandler):
            call_count: int
            def __init__(self, init_count: int = 0) -> None:
                self.call_count = init_count

            def __call__(
                self,
                manager: IpcAppManager,
                payload: IpcPayload
            ) -> Optional[InterProcessMessage]:
                self.call_count += 1
                # do something with payload. maybe use manager to send a
                # response containing `self.call_count`.

        manager = IpcAppManagerSync(
            socket_specs={
                'sub_sock': IpcAppManager.SocketSpec(
                    sock_type=ipc.SocketType.SUBSCRIBER,
                    port=1234,
                    topics=[ipc.Topic.SOME_TOPIC, ipc.Topic.ANOTHER_TOPIC],
                    rx_handler=AStatefulSocketHandler()
                )
            }
        )
    ```
    """

    @abstractmethod
    def __call__(
        self,
        manager: _AMT,
        payload: IpcPayload
    ) -> _HRT:
        """
        The actual handler.
        Must be overridden in subclasses.
        """
        raise NotImplementedError()


# Type Aliases for sync and async `SocketHandler`s.
# NOTE: Since these are just `TypeAlias`es for populated `Generic`s,
# these **will** still act as superclasses for populated subclasses
# (e.g. `SocketTopicHandlerAsync`).
SocketHandlerSync_T: TypeAlias = SocketHandler[PayloadHandlerResponseType,
                                               'IpcAppManagerSync']
SocketHandlerAsync_T: TypeAlias = SocketHandler[PayloadHandlerResponseTypeAsync,
                                                'IpcAppManagerAsync']


class SocketHandlerSync(SocketHandlerSync_T):
    """
    `SocketHandler` with Synchronous response type
     with abstracted inner async function. To be used as a parent for any
     Synchronous impl.s of basic `SocketHandler`s.
     Might include class-specific helper functions later.
    """
    pass


class SocketHandlerAsync(SocketHandlerAsync_T):
    """
    `SocketHandler` with Asynchronous (`Awaitable`) response type
     with abstracted inner async function. Simplifies work of making an async
    `SocketHandler` impl.
    """
    @abstractmethod
    async def async_call(
        self,
        manager: IpcAppManagerAsync,
        payload: IpcPayload
    ) -> PayloadHandlerResponseType:
        """
        The actual async handler.
        Must be overridden in subclasses.
        Override this instead of `__call__` for async versions.
        """
        raise NotImplementedError()

    def __call__(
        self,
        manager: IpcAppManagerAsync,
        payload: IpcPayload
    ) -> PayloadHandlerResponseTypeAsync:
        return self.async_call(manager, payload)


TopicHandlerType: TypeAlias[_SHT, _AMT, _HRT] = \
    Callable[[_SHT, _AMT, IpcPayload], _HRT]

_THT = TypeVar('_THT', bound=TopicHandlerType)


class SocketTopicHandler(SocketHandler[_HRT, _AMT], Generic[_SHT, _HRT, _AMT]):
    """
    Abstract base class for a functor to handle new payloads sent to a
    socket, where each payload topic has a different handler.

    `_topic_handlers` must be overridden in each subclass. Example:

    ```py
        class SomeHandler(SocketTopicHandlerAsync['SomeHandler']):
            _raise_on_unhandled_topics: ClassVar[bool] = False
            # Decorator shorthand (also plays more nicely with syntax highlighting):
            topic_handler = SocketTopicHandlerAsync['SomeHandler'].TopicHandlerFactory()

            @topic_handler
            async def some_handler(self, manager: IpcAppManagerAsync, payload: IpcPayload) -> None:
                # do some stuff
                pass

            @topic_handler
            async def some_other_handler(self, manager: IpcAppManagerAsync, payload: IpcPayload) -> None:
                # do some stuff
                pass

            @topic_handler
            async def do_nothing(self, *_) -> None:
                # Do nothing. Just toss this data out.
                return None

            def handle_unhandled_topic(
                self,
                manager: IpcAppManagerAsync,
                payload: IpcPayload
            ) -> Awaitable[None]:
                # Handler called for a payload whose topic has no registered handler
                return self.do_nothing(*[])

            _topic_handlers: ClassVar[IpcAppManager.SocketTopicHandler.TopicHandlerRegistry] = {
                Topic.SomeTopic: some_handler,
                Topic.AnotherTopic: some_other_handler,
                # the same handler can even be used for multiple topics:
                Topic.SomeTopicWeWantToHandleGenerically: some_handler,
                Topic.AnotherTopicWeWantToHandleGenerically: some_handler,
                # You can also add a handler to use if the payload has no topic:
                None: do_nothing
            }
    ```

    Additionally, consider overriding default settings and behaviors:
    `_raise_on_unhandled_topics` and `handle_unhandled_topic`.
    """

    # Convenience Type Aliases:
    TopicHandlerRegistry: ClassVar[TypeAlias] = Dict[Optional[Topic],
                                                     TopicHandlerType[_SHT, _AMT, _HRT]]

    # Dictionary of all topic handlers in the class.
    # MUST be overridden in subclass.
    # Can include a `None` key to catch payloads with no (recognized) topic.
    # See docstring above for an example.
    _topic_handlers: ClassVar[TopicHandlerRegistry] = dict()

    # Whether or not to ignore payloads where we don't have a topic handler
    # matching their topic.
    # (if `False`, a `UnhandledTopicException` is raised. if `True`, the
    # payload is just passed to `handle_unhandled_topic`)
    # **Can** be overridden in subclass.
    _raise_on_unhandled_topics: ClassVar[bool] = False

    class TopicHandlerFactory:
        """
        Decorator that makes sure the wrapped function complies with this
        class's topic handler specs with 0 runtime overhead (mypy will throw
        errors at compile time if not compliant).

        Example:
        ```py
        class SomeHandler(SocketTopicHandlerAsync['SomeHandler']):
            # Decorator shorthand (also plays more nicely with syntax highlighting):
            topic_handler = SocketTopicHandlerAsync['SomeHandler'].TopicHandlerFactory()

            # Throws no errors:
            @topic_handler
            async def full_handler(self, manager: IpcAppManagerAsync, payload: IpcPayload) -> None:
                # do some stuff
                pass

            # Also throws no errors:
            @topic_handler
            async def do_nothing(self, *_) -> None:
                # Do nothing. Just toss this data out.
                return None

            # Throws a mypy error b/c bad return type:
            @topic_handler
            async def full_handler(self, manager: IpcAppManagerAsync, payload: IpcPayload) -> str:
                # do some stuff
                pass

            # Throws a mypy error b/c bad arg type:
            @topic_handler
            async def full_handler(self, manager: str, payload: IpcPayload) -> str:
                # do some stuff
                pass
        ```
        """

        def __init__(self) -> None: pass

        def __call__(self, func: TopicHandlerType[_SHT, _AMT, _HRT]) -> TopicHandlerType[_SHT, _AMT, _HRT]:
            return func

    def handle_unhandled_topic(
        self,
        manager: _AMT,
        payload: IpcPayload
    ) -> _HRT:
        """ Handler called for a payload whose topic has no registered handler
        (and only iff `cls._raise_on_unhandled_topics==False`).

        Default behavior raises a `NotImplementedError`.
        **Should** be overridden in subclass if
        `cls._raise_on_unhandled_topics==False`.
        """
        raise NotImplementedError(
            f"`handle_unhandled_topic` called but unimplemented in "
            f"{self.__class__.__name__}."
        )

    def __call__(
        self,
        manager: _AMT,
        payload: IpcPayload
    ) -> _HRT:
        try:
            topic = payload.topic
        except Exception:
            # Topic not recognized / bad bytes. Treat as `None`
            topic = None

        if topic not in self._topic_handlers:
            # We don't have a handler for this topic...
            if self._raise_on_unhandled_topics:
                raise UnhandledTopicException(payload, (
                    f"`{self.__class__.__name__}` {self} received `{payload=}` "
                    f"with `{topic=}`. The `SocketTopicHandler` does not "
                    f"have a topic handler for this topic "
                    f"and has `{self._raise_on_unhandled_topics=}`. "
                    f"Registered topic handlers are: `{self._topic_handlers=}`."
                ))
            else:
                # Handle it with a default handler:
                return self.handle_unhandled_topic(manager=manager, payload=payload)

        # Everything checks out. Handle it:
        res: _HRT = self._topic_handlers[topic](manager, payload)
        return res


# Type Aliases for sync and async `SocketHandler`s.
# NOTE: Since these are just `TypeAlias`es for populated `Generic`s,
# these **will** still act as superclasses for any future populated subclasses
# of `SocketTopicHandler`.
SocketTopicHandlerSync_T: TypeAlias[_SHT] = SocketTopicHandler[_SHT, PayloadHandlerResponseType,
                                                               'IpcAppManagerSync']
SocketTopicHandlerAsync_T: TypeAlias[_SHT] = SocketTopicHandler[_SHT, PayloadHandlerResponseTypeAsync,
                                                                'IpcAppManagerAsync']


class SocketTopicHandlerSync(SocketTopicHandlerSync_T[_SHT]):
    """
    `SocketTopicHandler` with Synchronous response type
     with abstracted inner async function. To be used as a parent for any
     Synchronous impl.s of `SocketTopicHandler`s.
     Might include class-specific helper functions later.
    """
    pass


class SocketTopicHandlerAsync(SocketTopicHandlerAsync_T[_SHT]):
    """
    `SocketTopicHandler` with Asynchronous (`Awaitable`) response type
     with abstracted inner async function. To be used as a parent for any
     Synchronous impl.s of `SocketTopicHandler`s.
     Might include class-specific helper functions later.
    """
    pass

# ! TODO: (WORKING-HERE):
# - Finish `SocketTopicHandler` sync and async versions (like in SocketHandler).
#   -- Double check how logic for async __call__ works (might have to consult SocketHandler solution)
# - Integrate into `IpcAppManager` and subclasses.
#   -- + finish impl.s
# - At end, double check if _CT, _ST, and _HT need to be marked as covariant.
#   -- Does having them as covariant break anything?
#   -- Does changing them back to invariant break anything new?
#   -- Particularly, be careful with TopicHandler Protocol (not sure why
#       this stopped being a problem).
# - Go through and update all docs + examples
# - Build + Test XCVR demo
#   -- Consider bringing any generic(able) messaging / queue impl.s used for
#      XCVR demo into here
#
# !! How does IpcAppManager work with multiple SocketHandler types? Seems like
# we'd need multiple (if we want reg. and topic handler versions but have to
# enforce one...) <- might have to restructure when async is enforced.


@dataclasses.dataclass
class SocketSpec(Generic[_HT]):
    """Specifications needed to create a socket."""
    # Type of the socket:
    sock_type: SocketType
    # Which ports the socket is on:
    port: Port | List[Port]
    # Which topics the socket subscribes to (if this SocketType allows
    # subscription):
    topics: Optional[Topic | List[Topic]] = None
    # Receive handler for payloads received on this socket (only
    # applicable if this socket is used to process incoming payloads - i.e.
    # running a `socket_rx_task`):
    rx_handler: Optional[_HT] = None

    def __str__(self) -> str:
        if self.topics is None:
            return f"{self.sock_type.name} on {self.port}"
        else:
            ts: List[Topic]
            if isinstance(self.topics, list):
                ts = self.topics
            else:
                ts = [self.topics]
            s = (
                f"{self.sock_type.name} on {self.port} "
                f"for [{', '.join(t.name for t in ts)}]"
            )
            if self.rx_handler is not None:
                s += (
                    f" with RX handler: {self.rx_handler.__class__.__name__}"
                )
            return s


class IpcAppManager(ABC, Generic[_CT, _ST, _HT]):
    """
    ABC Manager for an App that uses IPC to communicate with other apps.
    """
    # Context used to communicate with IPC:
    context: _CT
    # Map from the name of sockets used by this app to the specs of each socket:
    socket_specs: Dict[str, SocketSpec[_HT]]
    # Map from the name of sockets used by this app to the socket objects:
    sockets: Dict[str, _ST]

    def __init__(
        self,
        socket_specs: Dict[str, SocketSpec[_HT]],
        context_io_threads: int = 2,
        context: Optional[_CT] = None,
    ) -> None:
        """
        Args:
            socket_specs (Dict[str, SocketSpec[_HT]): Dict mapping socket names to `SocketSpec`s for each socket.
            context_io_threads (int, optional): Number of IO threads to use if a new context needs to be constructed. Defaults to 2 (one for RX, one for TX). Generally speaking this is overkill and 1 thread per GBps of throughput is typically sufficient (so, `1` should also suffice).
            context (Optional[_CT], optional): Optionally, pass in an external context to use instead of building one internally. Defaults to `None`.
        """

        # Load or create context:
        if context is None:
            # 2 IO threads, one for RX, one for TX:
            self.context = self.create_context(io_threads=context_io_threads)
        else:
            self.context = context

        # Grab socket specs:
        self.socket_specs = socket_specs

        # Create all sockets:
        self.sockets = dict()
        for sock_name, spec in self.socket_specs.items():
            self.setup_socket(sock_name, spec)

    def __del__(self):
        """Close everything on destruction."""
        # Close sockets:
        for socket in self.sockets.values():
            socket.close()
        # Close context (waiting for all sockets to close first):
        self.context.term()

    def __str__(self) -> str:
        return (
            f"{self.__class__.__name__} with "
            f"{',  '.join(f'{n}: {s!s}' for n,s in self.socket_specs.items())}"
        )

    def setup_socket(self, name: str, spec: SocketSpec[_HT]):
        """ Creates and sets up (as necessary) a socket with the given spec and
        stores it under the given name.
        """
        self.sockets[name] = self.create_socket(spec)

        # Subscribe to topics if the spec details topics:
        if spec.topics is not None:
            subscribe(self.sockets[name], spec.topics)

    @ abstractmethod
    def create_context(*args, **kwargs) -> _CT:
        """Creates an IPC context of type `_CT`.
        To be implemented in concrete subclasses."""
        # static too (is implicit here since it doesn't take `self` or `cls`,
        # thus subclass will necessarily have to declare as @staticmethod.
        # @abstractmethod doesn't always play nicely with @staticmethod at runtime.)
        raise NotImplementedError()

    @ abstractmethod
    def create_socket(self, spec: SocketSpec[_HT]) -> _ST:
        """Creates a socket of type `_ST`.
        To be implemented in concrete subclasses."""
        raise NotImplementedError()


class IpcAppManagerAsync(IpcAppManager[AsyncContext, AsyncSocket, SocketHandlerAsync_T]):
    @ staticmethod
    def create_context(*args, **kwargs) -> AsyncContext:
        """Creates an async IPC context."""
        return create_async_context(*args, **kwargs)

    def create_socket(self, spec: SocketSpec[SocketHandlerAsync_T]) -> AsyncSocket:
        """Creates an async IPC socket with the given spec."""
        return create_async_socket(
            context=self.context,
            socket_type=spec.sock_type,
            ports=spec.port
        )

    async def read(self, sock_name: str) -> IpcPayload:
        """Asynchronously reads from the socket with the given name."""
        return await async_read_from(self.sockets[sock_name])

    async def socket_rx_task(
        self,
        sock_name: str,
        consume_only: bool = False
    ) -> None:
        """
        Task for receiving data from a socket with the given name.

        Args:
            sock_name (str): Name of socket this task will read data from.
            consume_only (bool): Whether this task should just consume data and
                **NOT** do anything with incoming data. Note: this will also
                happen, by necessity, if the socket doesn't have an `rx_handler`.
                Setting this to `True` in that case will flag that as not being
                a problem. Default: `False`.
        """
        # Validate inputs;
        if sock_name not in self.socket_specs:
            raise KeyError(f"Socket with {sock_name=} not found.")

        if not consume_only and self.socket_specs[sock_name].rx_handler is None:
            logger.warning(
                f"Starting a `socket_rx_task` for a socket ({sock_name=}) "
                f"that doesn't have an `rx_handler` and `{consume_only}=False`. "
                f"This will consume data on the socket interface but won't do "
                f"anything with that data. This is likely undesirable behavior "
                f"(if this were desirable, `consume_only` would be `True`.)"
            )

        if consume_only and self.socket_specs[sock_name].rx_handler is not None:
            logger.notice(
                f"Starting a `socket_rx_task` for a socket ({sock_name=}) "
                f"that has an `rx_handler` and `{consume_only}=True`. "
                f"This will consume data on the socket interface but won't do "
                f"anything with that data. `{consume_only}=True` indicates "
                f"that this is likely **desired** behavior but it is strange."
            )

        while True:
            payload = await self.read(sock_name)
            # Dispatch payload to socket handler (if there is one):
            rx_handler = self.socket_specs[sock_name].rx_handler
            if not consume_only and rx_handler is not None:
                await rx_handler(self, payload)


class IpcAppManagerSync(IpcAppManager[Context, Socket, SocketHandlerSync_T]):
    @ staticmethod
    def create_context(*args, **kwargs) -> Context:
        """Creates an async IPC context."""
        return create_context(*args, **kwargs)

    def create_socket(self, spec: SocketSpec[SocketHandlerSync_T]) -> Socket:
        """Creates an async IPC socket with the given spec."""
        return create_socket(
            context=self.context,
            socket_type=spec.sock_type,
            ports=spec.port
        )

    def read(self, sock_name: str) -> IpcPayload:
        """Synchronously reads from the socket with the given name."""
        return read_from(self.sockets[sock_name])