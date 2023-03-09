# -*- coding: utf-8 -*-
"""
High-level IPC manager classes to simplify developing IPC Apps.

At this level, all IPC interface operations have been abstracted. For the
low-level IPC interface and implementation, see `wrapper.py`.

@author: Connor W. Colombo (CMU)
@last-updated: 03/07/2023

#! NOTE: Many of the example docstrings in here are out of date. Updating soon.

# ! TODO: async queue the packets received from IPC so nothing is missed.
# ^- low priority rn b/c we can keep from sending multiple messages in rapid
# succession by just sending lists.
# - will need this though.
# -- seems like ZMQ already does this. Might not need to double-queue it.
#    Determine how slow is too slow...

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
"""
# Activate postponed annotations (for using classes as return type in their own methods)
from __future__ import annotations
from abc import ABC, abstractmethod
from typing import Awaitable, Callable, Generic, Optional, ClassVar, Dict, List, TypeVar, Tuple, Coroutine, Generator, Type
from typing_extensions import TypeAlias
import dataclasses
import asyncio
import traceback
from verboselogs import VerboseLogger

from IrisBackendv3.ipc.wrapper import (
    Context, AsyncContext,
    create_context, create_async_context,
    Socket, AsyncSocket,
    create_socket, create_async_socket,
    subscribe,
    SocketType,
    read_from, read_msg, read_from_as,
    async_read_from, async_read_msg, async_read_from_as,
    send_to, async_send_to,
)

from IrisBackendv3.ipc.ipc_payload import IpcPayload
from IrisBackendv3.ipc.inter_process_message import InterProcessMessage

from IrisBackendv3.ipc.port import Port
from IrisBackendv3.ipc.topics_registry import Topic
from IrisBackendv3.ipc.settings import settings
from IrisBackendv3.ipc.logging import logger, create_app_logger
from IrisBackendv3.ipc.exceptions import UnhandledTopicException, IpcEndAppRequest

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


_IPMT = TypeVar('_IPMT', bound=InterProcessMessage)


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
    ) -> _HRT | None:
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
            topic_handler = SocketTopicHandlerAsync['SomeHandler'].TopicHandlerFactory(
            )

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

    # Whether or not to require `handle_unhandled_topic` to be implemented in
    # the subclass if `_raise_on_handled_topics = False`.
    # That is, if `_raise_on_handled_topics = False`, then
    # `handle_unhandled_topic` would normally be called if this socket
    # encounters a topic it doesn't have a handler for.
    # If `handle_unhandled_topic` isn't implemented in the subclass, then this
    # would cause an exception, unless
    # `_require_unhandled_topic_handler = False`, in which case nothing will
    # happen and the data will simply be ignored.
    # **Can** be overridden in subclass.
    _require_unhandled_topic_handler: ClassVar[bool] = False

    class TopicHandlerFactory:
        """
        Decorator that makes sure the wrapped function complies with this
        class's topic handler specs with 0 runtime overhead (mypy will throw
        errors at compile time if not compliant).

        Example:
        ```py
        class SomeHandler(SocketTopicHandlerAsync['SomeHandler']):
            # Decorator shorthand (also plays more nicely with syntax highlighting):
            topic_handler = SocketTopicHandlerAsync['SomeHandler'].TopicHandlerFactory(
            )

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

    @classmethod
    def TOPICS(cls) -> List[Topic]:
        """Returns a list of all `Topic`s this SocketTopicHandler interacts
        with (non-None keys of `_topic_handlers`)."""
        return [k for k in cls._topic_handlers.keys() if k is not None]

    def handle_unhandled_topic(
        self,
        manager: _AMT,
        payload: IpcPayload
    ) -> _HRT | None:
        """ Handler called for a payload whose topic has no registered handler
        (and only iff `cls._raise_on_unhandled_topics==False`).

        Default behavior raises a `NotImplementedError`.
        **Should** be overridden in subclass if
        `cls._raise_on_unhandled_topics==False`, unless
        `cls._require_unhandled_topic_handler==False`.
        """
        if self._require_unhandled_topic_handler:
            raise NotImplementedError(
                f"`handle_unhandled_topic` called but unimplemented in "
                f"{self.__class__.__name__} but "
                f"`{self._require_unhandled_topic_handler=}`."
            )

        return None

    def __call__(
        self,
        manager: _AMT,
        payload: IpcPayload
    ) -> _HRT | None:
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
        res: _HRT = self._topic_handlers[topic](self, manager, payload)
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
    # running a `socket_rx_coro`):
    # NOTE: This currently is only used in IpcAppManagerAsync.
    rx_handler: Optional[_HT] = None
    # Whether or not this socket should be binding the port:
    # (if not given, AppManager chooses
    # - servers & pubs bind, clients & subs don't)
    bind: Optional[bool] = None
    # Whether this should just be treated as a consumer (i.e. ignore any
    # rx_handler and just consume (rx & throw away) data on this
    # socket/topics):
    # Note: this will also happen, by necessity, if the socket doesn't have an
    # `rx_handler`. Setting this to `True` in that case will flag that as not
    # being a problem. Default: `False`.
    blind_consumer: bool = False

    def __str__(self) -> str:
        s = ""
        if self.topics is None:
            s = f"{self.sock_type.name} on {self.port}"
        else:
            ts: List[Topic]
            if isinstance(self.topics, list):
                ts = self.topics
            else:
                ts = [self.topics]
            s += (
                f"{self.sock_type.name} on {self.port} "
                f"for [{', '.join(t.name for t in ts)}]"
            )
            if self.rx_handler is not None:
                s += (
                    f" with RX handler: {self.rx_handler.__class__.__name__}"
                )
        s += (
            f" with settings: "
            f"bind={self.bind}, "
            f"blind_consumer={self.blind_consumer}"
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
        context_io_threads: int = 2,  # default: 2 IO threads (RX and TX)
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
        self.context.destroy()  # closes any remaining ctx sockets then `term`

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

        # Subscribe to topics if the spec details topics
        # (and we can subscribe):
        if spec.topics is not None and spec.sock_type == SocketType.SUBSCRIBER:
            subscribe(self.sockets[name], spec.topics)

    @abstractmethod
    def create_context(*args, **kwargs) -> _CT:
        """Creates an IPC context of type `_CT`.
        To be implemented in concrete subclasses."""
        # static too (is implicit here since it doesn't take `self` or `cls`,
        # thus subclass will necessarily have to declare as @staticmethod.
        # @abstractmethod doesn't always play nicely with @staticmethod at runtime.)
        raise NotImplementedError()

    @abstractmethod
    def create_socket(self, spec: SocketSpec[_HT]) -> _ST:
        """Creates a socket of type `_ST`.
        To be implemented in concrete subclasses."""
        raise NotImplementedError()


class IpcAppManagerAsync(IpcAppManager[AsyncContext, AsyncSocket, SocketHandlerAsync_T]):
    # `asyncio.Task`s this Manager is in charge of:
    # (these get built progressively)
    _tasks: List[asyncio.Task]
    # Whether this app manager's core tasks have already been spawned
    # (and are already in `_tasks`:
    _core_tasks_spawned: bool

    def __init__(
        self,
        socket_specs: Dict[str, SocketSpec[SocketHandlerAsync_T]],
        context_io_threads: int = 2,  # default: 2 IO threads (RX and TX)
        context: Optional[AsyncContext] = None,
    ) -> None:
        super().__init__(
            socket_specs=socket_specs,
            context_io_threads=context_io_threads,
            context=context
        )
        # Start with no managed tasks (gets built progressively):
        self._tasks = []
        self._core_tasks_spawned = False

    @staticmethod
    def create_context(*args, **kwargs) -> AsyncContext:
        """Creates an async IPC context."""
        return create_async_context(*args, **kwargs)

    def create_socket(self, spec: SocketSpec[SocketHandlerAsync_T]) -> AsyncSocket:
        """Creates an async IPC socket with the given spec."""
        return create_async_socket(
            context=self.context,
            socket_type=spec.sock_type,
            ports=spec.port,
            bind=spec.bind
        )

    async def send_to(
        self,
        sock_name: str,
        msg: InterProcessMessage,
        subtopic_bytes: bytes = b''
    ) -> None:
        """Asynchronously sends to the socket with the given name using its
        assigned topic (or all if no topic is assigned), optionally tagging the
        message with the given subtopic."""
        socket = self.sockets[sock_name]
        specs = self.socket_specs[sock_name]
        if len(specs.topics) == 0:
            await async_send_to(socket, msg, subtopic_bytes, None)
        else:
            for topic in specs.topics:
                await async_send_to(socket, msg, subtopic_bytes, topic)

    async def read(self, sock_name: str) -> IpcPayload:
        """Asynchronously reads from the socket with the given name."""
        return await async_read_from(self.sockets[sock_name])

    async def read_msg(
        self,
        sock_name: str
    ) -> Tuple[IpcPayload, InterProcessMessage]:
        """Asynchronously reads from the socket with the given name and decodes
        the message (based on the `Topic` of the `payload`).
        """
        return await async_read_msg(self.sockets[sock_name])

    async def read_as(
        self,
        sock_name: str,
        message_type: Type[_IPMT]
    ) -> Tuple[IpcPayload, _IPMT]:
        """Same as `read`, but instead of returning raw binary, it
        parses the data read as the given IPC `InterProcessMessage`.
        Returns a tuple containing the raw `IpcPayload` alongside the
        interpreted `InterProcessMessage`.
        """
        return await async_read_from_as(self.sockets[sock_name], message_type)

    async def socket_rx_coro(
        self,
        sock_name: str
    ) -> None:
        """
        Coroutine for receiving data from a socket with the given name.
        """
        # Validate inputs;
        if sock_name not in self.socket_specs:
            raise KeyError(f"Socket with {sock_name=} not found.")

        blind_consumer = self.socket_specs[sock_name].blind_consumer
        if (
            not blind_consumer
            and self.socket_specs[sock_name].rx_handler is None
        ):
            logger.warning(
                f"Starting a `socket_rx_coro` for a socket ({sock_name=}) "
                f"that doesn't have an `rx_handler` and `{blind_consumer}=False`. "
                f"This will consume data on the socket interface but won't do "
                f"anything with that data. This is likely undesirable behavior "
                f"(if this were desirable, `blind_consumer` would be `True`.)"
            )

        if (
            blind_consumer
            and self.socket_specs[sock_name].rx_handler is not None
        ):
            logger.notice(
                f"Starting a `socket_rx_coro` for a socket ({sock_name=}) "
                f"that has an `rx_handler` and `{blind_consumer}=True`. "
                f"This will consume data on the socket interface but won't do "
                f"anything with that data. `{blind_consumer}=True` indicates "
                f"that this is likely **desired** behavior but it is strange."
            )

        while True:
            payload = await self.read(sock_name)
            # Dispatch payload to socket handler (if there is one):
            rx_handler = self.socket_specs[sock_name].rx_handler
            if not blind_consumer and rx_handler is not None:
                awaitable = rx_handler(self, payload)
                if awaitable is not None:
                    await awaitable

    def spawn_core_tasks(self) -> None:
        """Spawns all core internal tasks/coroutines for this app and adds
        them to the list of managed tasks.
        """
        # Create Tasks for inner coros:
        self._tasks.extend(
            asyncio.create_task(self.socket_rx_coro(sock_name), name=sock_name)
            for sock_name in self.socket_specs.keys()
        )
        self._core_tasks_spawned = True

    def add_coros(self, other_coros: List[Generator | Coroutine]) -> None:
        """Converts the given coroutines into `asyncio.Task`s and adds them to
        the list of managed tasks to be run alongside this IPC App's core
        tasks."""
        self._tasks.extend(asyncio.create_task(c) for c in other_coros)

    def add_tasks(self, other_tasks: List[asyncio.Task]) -> None:
        """Adds the given `asyncio.Task`s to the list of managed tasks to be
        run alongside this IPC App's core tasks."""
        self._tasks.extend(other_tasks)

    async def run(
        self,
        return_when: str = asyncio.FIRST_COMPLETED,
        timeout: Optional[float] = None
    ) -> None:
        """Runs all tasks for this `IpcAppManager` alongside any other tasks
        passed in. Manages the life-cycle of all these concurrent tasks and
        based on the given `asyncio` `return_when` flag (see `asyncio.wait`
        for more details on `return_when`).

        Args:
            return_when (str, optional): When to end app execution based on
                status of internal tasks. From `asyncio.wait`.
            timeout (Optional[float], optional): Max runtime (as a failsafe).
                From `asyncio.wait`.
        """
        logger.notice(f"Starting IPC processes for: {self!s}.")

        # Spawn core tasks if not already spawned:
        if not self._core_tasks_spawned:
            self.spawn_core_tasks()

        # Make sure there are tasks to run:
        if len(self._tasks) == 0:
            # There are no tasks to run.
            # Log this since it's possibly weird but could be intentional:
            logger.notice(
                f"`IpcAppManagerAsync.run` was called for `{self}` "
                f"but this manager isn't in charge of any tasks currently. "
                f"`spawn_core_tasks()` should have automatically been called "
                f"if you didn't call it manually, so there should be tasks. "
                f"Does this manager have no core tasks?"
            )
            # and just return:
            return

        # Clean up if the above closes for some reason:
        try:
            done, pending = await asyncio.wait(
                self._tasks,
                timeout=timeout,
                return_when=return_when
            )

            # We're done. Log results:
            for task in done:
                result, exception, trace = None, None, None
                end_request_reason: Optional[str] = None
                try:
                    result = task.result()
                except IpcEndAppRequest as ear:
                    end_request_reason = ear.why
                except Exception as e:
                    exception = e
                    trace = '\n'.join(traceback.format_tb(e.__traceback__))
                if end_request_reason is not None:
                    # This Task ended due to a request:
                    logger.notice(
                        f"Task {task.get_name()} requested end of app "
                        f"because: `{end_request_reason}`."
                    )
                else:
                    logger.notice(
                        f"Task {task.get_name()} ended "
                        f"with `{result=}`, `{exception=}`,\n"
                        f"`trace={trace}`."
                    )
            for task in pending:
                task.cancel()
                logger.notice(
                    f"Task {task.get_name()} didn't end. Now cancelled."
                )
        except Exception as e:
            logger.error(f"Exception occurred while handling: \n\t{e}.")


class IpcAppManagerSync(IpcAppManager[Context, Socket, SocketHandlerSync_T]):
    @staticmethod
    def create_context(*args, **kwargs) -> Context:
        """Creates an async IPC context."""
        return create_context(*args, **kwargs)

    def create_socket(self, spec: SocketSpec[SocketHandlerSync_T]) -> Socket:
        """Creates an async IPC socket with the given spec."""
        return create_socket(
            context=self.context,
            socket_type=spec.sock_type,
            ports=spec.port,
            bind=spec.bind
        )

    def send_to(
        self,
        sock_name: str,
        msg: InterProcessMessage,
        subtopic_bytes: bytes = b''
    ) -> None:
        """Synchronously sends to the socket with the given name using its
        assigned topic (or all if no topic is assigned), optionally tagging the
        message with the given subtopic."""
        socket = self.sockets[sock_name]
        specs = self.socket_specs[sock_name]
        if len(specs.topics) == 0:
            send_to(socket, msg, subtopic_bytes, None)
        else:
            for topic in specs.topics:
                send_to(socket, msg, subtopic_bytes, topic)

    def read(self, sock_name: str) -> IpcPayload:
        """Synchronously reads from the socket with the given name."""
        return read_from(self.sockets[sock_name])

    def read_msg(
        self,
        sock_name: str
    ) -> Tuple[IpcPayload, InterProcessMessage]:
        """Synchronously reads from the socket with the given name and decodes
        the message (based on the `Topic` of the `payload`).
        """
        return read_msg(self.sockets[sock_name])

    def read_as(
        self,
        sock_name: str,
        message_type: Type[_IPMT]
    ) -> Tuple[IpcPayload, _IPMT]:
        """Same as `read`, but instead of returning raw binary, it
        parses the data read as the given IPC `InterProcessMessage`.
        Returns a tuple containing the raw `IpcPayload` alongside the
        interpreted `InterProcessMessage`.
        """
        return read_from_as(self.sockets[sock_name], message_type)


class IpcAppHelper:
    """Basic class to manage the configuration surrounding an IpcApp.
    Essentially just a collection of convenience features to streamline the
    setup that occurs outside of an `IpcAppManager`.
    """
    # Name of this IPC App:
    name: str
    # Logger instance:
    logger: VerboseLogger
    # Function Handle to set console logging level:
    _setLogLevel: Callable[[str], None]
    # Function Handle to set file logging level:
    _setFileLogLevel: Callable[[str], None]

    def __init__(
        self,
        name: str,
        set_window_title: bool = True
    ) -> None:
        """Sets up the IPC App.

        Args:
            name (str): Name of the IPC App.
            set_window_title (bool, optional): Whether or not to retitle the
             terminal window. Defaults to `True`.
        """
        self.name = name
        # Set the name in IPC settings:
        settings['app_name'] = "PacketPrinter"
        # Build the logger:
        logger_build = create_app_logger()
        self.logger, self._setLogLevel, self._setFileLogLevel = logger_build
        # Set the window title:
        if set_window_title:
            self.set_window_title()
        # Log that this started:
        self.logger.notice(
            f"IRIS Lunar Rover - {name} - IPC App Started"
        )

    def setLogLevel(self, level: str) -> None:
        # Set logging level for logs printed to the console:
        self._setLogLevel(level)

    def setFileLogLevel(self, level: str) -> None:
        # Set logging level for logs saved to file:
        self._setFileLogLevel(level)

    def set_window_title(self) -> None:
        """Helper function to set the title of the terminal window:"""
        print(f"\033]0;{self.name}\a")
