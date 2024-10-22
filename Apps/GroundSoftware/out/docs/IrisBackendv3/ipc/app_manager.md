Module IrisBackendv3.ipc.app_manager
====================================
High-level IPC manager classes to simplify developing IPC Apps.

At this level, all IPC interface operations have been abstracted. For the
low-level IPC interface and implementation, see `wrapper.py`.

@author: Connor W. Colombo (CMU)
@last-updated: 04/23/2023

#! NOTE: Many of the example docstrings in here are out of date. Updating soon.

Classes
-------

`IpcAppHelper(name: str, set_window_title: bool = True)`
:   Basic class to manage the configuration surrounding an IpcApp.
    Essentially just a collection of convenience features to streamline the
    setup that occurs outside of an `IpcAppManager`.
    
    Sets up the IPC App.
    
    Args:
        name (str): Name of the IPC App.
        set_window_title (bool, optional): Whether or not to retitle the
         terminal window. Defaults to `True`.

    ### Class variables

    `logger: verboselogs.VerboseLogger`
    :

    `name: str`
    :

    ### Methods

    `setFileLogLevel(self, level: str) ‑> None`
    :

    `setLogLevel(self, level: str) ‑> None`
    :

    `set_window_title(self) ‑> None`
    :   Helper function to set the title of the terminal window:

`IpcAppManager(socket_specs: Dict[str, SocketSpec[_HT]], context_io_threads: int = 2, context: Optional[_CT] = None)`
:   ABC Manager for an App that uses IPC to communicate with other apps.
    
    Args:
        socket_specs (Dict[str, SocketSpec[_HT]): Dict mapping socket names to `SocketSpec`s for each socket.
        context_io_threads (int, optional): Number of IO threads to use if a new context needs to be constructed. Defaults to 2 (one for RX, one for TX). Generally speaking this is overkill and 1 thread per GBps of throughput is typically sufficient (so, `1` should also suffice).
        context (Optional[_CT], optional): Optionally, pass in an external context to use instead of building one internally. Defaults to `None`.

    ### Ancestors (in MRO)

    * abc.ABC
    * typing.Generic

    ### Descendants

    * IrisBackendv3.ipc.app_manager.IpcAppManagerAsync
    * IrisBackendv3.ipc.app_manager.IpcAppManagerSync

    ### Class variables

    `context: +_CT`
    :

    `socket_specs: Dict[str, IrisBackendv3.ipc.app_manager.SocketSpec[+_HT]]`
    :

    `sockets: Dict[str, +_ST]`
    :

    ### Methods

    `create_context(*args, **kwargs) ‑> +_CT`
    :   Creates an IPC context of type `_CT`.
        To be implemented in concrete subclasses.

    `create_socket(self, spec: SocketSpec[_HT]) ‑> +_ST`
    :   Creates a socket of type `_ST`.
        To be implemented in concrete subclasses.

    `setup_socket(self, name: str, spec: SocketSpec[_HT])`
    :   Creates and sets up (as necessary) a socket with the given spec and
        stores it under the given name.

`IpcAppManagerAsync(socket_specs: Dict[str, SocketSpec[SocketHandlerAsync_T]], context_io_threads: int = 2, context: Optional[AsyncContext] = None)`
:   ABC Manager for an App that uses IPC to communicate with other apps.
    
    Args:
        socket_specs (Dict[str, SocketSpec[_HT]): Dict mapping socket names to `SocketSpec`s for each socket.
        context_io_threads (int, optional): Number of IO threads to use if a new context needs to be constructed. Defaults to 2 (one for RX, one for TX). Generally speaking this is overkill and 1 thread per GBps of throughput is typically sufficient (so, `1` should also suffice).
        context (Optional[_CT], optional): Optionally, pass in an external context to use instead of building one internally. Defaults to `None`.

    ### Ancestors (in MRO)

    * IrisBackendv3.ipc.app_manager.IpcAppManager
    * abc.ABC
    * typing.Generic

    ### Static methods

    `create_context(*args, **kwargs) ‑> zmq.asyncio.Context`
    :   Creates an async IPC context.

    ### Methods

    `add_coros(self, other_coros: List[Generator | Coroutine]) ‑> None`
    :   Converts the given coroutines into `asyncio.Task`s and adds them to
        the list of managed tasks to be run alongside this IPC App's core
        tasks.

    `add_tasks(self, other_tasks: List[asyncio.Task]) ‑> None`
    :   Adds the given `asyncio.Task`s to the list of managed tasks to be
        run alongside this IPC App's core tasks.

    `create_socket(self, spec: SocketSpec[SocketHandlerAsync_T]) ‑> zmq.asyncio.Socket`
    :   Creates an async IPC socket with the given spec.

    `read(self, sock_name: str) ‑> IrisBackendv3.ipc.ipc_payload.IpcPayload`
    :   Asynchronously reads from the socket with the given name.

    `read_as(self, sock_name: str, message_type: Type[_IPMT]) ‑> Tuple[IrisBackendv3.ipc.ipc_payload.IpcPayload, ~_IPMT]`
    :   Same as `read`, but instead of returning raw binary, it
        parses the data read as the given IPC `InterProcessMessage`.
        Returns a tuple containing the raw `IpcPayload` alongside the
        interpreted `InterProcessMessage`.

    `read_msg(self, sock_name: str) ‑> Tuple[IrisBackendv3.ipc.ipc_payload.IpcPayload, IrisBackendv3.ipc.inter_process_message.InterProcessMessage]`
    :   Asynchronously reads from the socket with the given name and decodes
        the message (based on the `Topic` of the `payload`).

    `run(self, return_when: str = 'FIRST_COMPLETED', timeout: Optional[float] = None) ‑> None`
    :   Runs all tasks for this `IpcAppManager` alongside any other tasks
        passed in. Manages the life-cycle of all these concurrent tasks and
        based on the given `asyncio` `return_when` flag (see `asyncio.wait`
        for more details on `return_when`).
        
        Args:
            return_when (str, optional): When to end app execution based on
                status of internal tasks. From `asyncio.wait`.
            timeout (Optional[float], optional): Max runtime (as a failsafe).
                From `asyncio.wait`.

    `send_to(self, sock_name: str, msg: InterProcessMessage, subtopic_bytes: bytes = b'') ‑> None`
    :   Asynchronously sends to the socket with the given name using its
        assigned topic (or all if no topic is assigned), optionally tagging the
        message with the given subtopic.

    `socket_rx_coro(self, sock_name: str) ‑> None`
    :   Coroutine for receiving data from a socket with the given name.

    `spawn_core_tasks(self) ‑> None`
    :   Spawns all core internal tasks/coroutines for this app and adds
        them to the list of managed tasks.

`IpcAppManagerSync(socket_specs: Dict[str, SocketSpec[_HT]], context_io_threads: int = 2, context: Optional[_CT] = None)`
:   ABC Manager for an App that uses IPC to communicate with other apps.
    
    Args:
        socket_specs (Dict[str, SocketSpec[_HT]): Dict mapping socket names to `SocketSpec`s for each socket.
        context_io_threads (int, optional): Number of IO threads to use if a new context needs to be constructed. Defaults to 2 (one for RX, one for TX). Generally speaking this is overkill and 1 thread per GBps of throughput is typically sufficient (so, `1` should also suffice).
        context (Optional[_CT], optional): Optionally, pass in an external context to use instead of building one internally. Defaults to `None`.

    ### Ancestors (in MRO)

    * IrisBackendv3.ipc.app_manager.IpcAppManager
    * abc.ABC
    * typing.Generic

    ### Static methods

    `create_context(*args, **kwargs) ‑> zmq.sugar.context.Context`
    :   Creates an async IPC context.

    ### Methods

    `create_socket(self, spec: SocketSpec[SocketHandlerSync_T]) ‑> zmq.sugar.socket.Socket`
    :   Creates an async IPC socket with the given spec.

    `read(self, sock_name: str) ‑> IrisBackendv3.ipc.ipc_payload.IpcPayload`
    :   Synchronously reads from the socket with the given name.

    `read_as(self, sock_name: str, message_type: Type[_IPMT]) ‑> Tuple[IrisBackendv3.ipc.ipc_payload.IpcPayload, ~_IPMT]`
    :   Same as `read`, but instead of returning raw binary, it
        parses the data read as the given IPC `InterProcessMessage`.
        Returns a tuple containing the raw `IpcPayload` alongside the
        interpreted `InterProcessMessage`.

    `read_msg(self, sock_name: str) ‑> Tuple[IrisBackendv3.ipc.ipc_payload.IpcPayload, IrisBackendv3.ipc.inter_process_message.InterProcessMessage]`
    :   Synchronously reads from the socket with the given name and decodes
        the message (based on the `Topic` of the `payload`).

    `send_to(self, sock_name: str, msg: InterProcessMessage, topics: Topic | List[Topic] | None = None, subtopic_bytes: bytes = b'') ‑> None`
    :   Synchronously sends to the socket with the given name using its
        assigned topic(s) (or all topics on the Port if no topic is assigned),
        unless a specific topic(s) is given, then the message is sent to that
        topic.
        The message is optionally tagged with the given subtopic.

`SocketHandler()`
:   Abstract base class for a functor to handle new payloads sent to a
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

    ### Ancestors (in MRO)

    * typing.Generic

    ### Descendants

    * IrisBackendv3.ipc.app_manager.SocketHandlerAsync
    * IrisBackendv3.ipc.app_manager.SocketHandlerSync
    * IrisBackendv3.ipc.app_manager.SocketTopicHandler

`SocketHandlerAsync()`
:   `SocketHandler` with Asynchronous (`Awaitable`) response type
     with abstracted inner async function. Simplifies work of making an async
    `SocketHandler` impl.

    ### Ancestors (in MRO)

    * IrisBackendv3.ipc.app_manager.SocketHandler
    * typing.Generic

    ### Methods

    `async_call(self, manager: IpcAppManagerAsync, payload: IpcPayload) ‑> None`
    :   The actual async handler.
        Must be overridden in subclasses.
        Override this instead of `__call__` for async versions.

`SocketHandlerSync()`
:   `SocketHandler` with Synchronous response type
     with abstracted inner async function. To be used as a parent for any
     Synchronous impl.s of basic `SocketHandler`s.
     Might include class-specific helper functions later.

    ### Ancestors (in MRO)

    * IrisBackendv3.ipc.app_manager.SocketHandler
    * typing.Generic

`SocketSpec(sock_type: SocketType, port: Port | List[Port], topics: Optional[Topic | List[Topic]] = None, rx_handler: Optional[_HT] = None, bind: Optional[bool] = None, blind_consumer: bool = False, publish_only: bool = False)`
:   Specifications needed to create a socket.

    ### Ancestors (in MRO)

    * typing.Generic

    ### Class variables

    `bind: Optional[bool]`
    :

    `blind_consumer: bool`
    :

    `port: Union[IrisBackendv3.ipc.port.Port, List[IrisBackendv3.ipc.port.Port]]`
    :

    `publish_only: bool`
    :

    `rx_handler: Optional[+_HT]`
    :

    `sock_type: IrisBackendv3.ipc.wrapper.SocketType`
    :

    `topics: Union[IrisBackendv3.ipc.topics_registry.Topic, List[IrisBackendv3.ipc.topics_registry.Topic], ForwardRef(None)]`
    :

    ### Instance variables

    `topics_list: List[IrisBackendv3.ipc.topics_registry.Topic]`
    :

`SocketTopicHandler()`
:   Abstract base class for a functor to handle new payloads sent to a
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

    ### Ancestors (in MRO)

    * IrisBackendv3.ipc.app_manager.SocketHandler
    * typing.Generic

    ### Descendants

    * IrisBackendv3.ipc.app_manager.SocketTopicHandlerAsync
    * IrisBackendv3.ipc.app_manager.SocketTopicHandlerSync

    ### Class variables

    `TopicHandlerFactory`
    :   Decorator that makes sure the wrapped function complies with this
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

    `TopicHandlerRegistry: ClassVar[TypeAlias]`
    :

    ### Static methods

    `TOPICS() ‑> List[IrisBackendv3.ipc.topics_registry.Topic]`
    :   Returns a list of all `Topic`s this SocketTopicHandler interacts
        with (non-None keys of `_topic_handlers`).

    ### Methods

    `handle_unhandled_topic(self, manager: _AMT, payload: IpcPayload) ‑> Optional[~_HRT]`
    :   Handler called for a payload whose topic has no registered handler
        (and only iff `cls._raise_on_unhandled_topics==False`).
        
        Default behavior raises a `NotImplementedError`.
        **Should** be overridden in subclass if
        `cls._raise_on_unhandled_topics==False`, unless
        `cls._require_unhandled_topic_handler==False`.

`SocketTopicHandlerAsync()`
:   `SocketTopicHandler` with Asynchronous (`Awaitable`) response type
     with abstracted inner async function. To be used as a parent for any
     Synchronous impl.s of `SocketTopicHandler`s.
     Might include class-specific helper functions later.

    ### Ancestors (in MRO)

    * IrisBackendv3.ipc.app_manager.SocketTopicHandler
    * IrisBackendv3.ipc.app_manager.SocketHandler
    * typing.Generic

`SocketTopicHandlerSync()`
:   `SocketTopicHandler` with Synchronous response type
     with abstracted inner async function. To be used as a parent for any
     Synchronous impl.s of `SocketTopicHandler`s.
     Might include class-specific helper functions later.

    ### Ancestors (in MRO)

    * IrisBackendv3.ipc.app_manager.SocketTopicHandler
    * IrisBackendv3.ipc.app_manager.SocketHandler
    * typing.Generic