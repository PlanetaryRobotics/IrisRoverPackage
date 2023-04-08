"""
Various exceptions used throughout the IPC package.

@author: Connor W. Colombo (CMU)
@last-updated: 03/04/2023
"""

from typing import Any, Type

import typing
if typing.TYPE_CHECKING:
    # NOTE: This import is only needed for type-hinting, so only needs to be
    # exposed `mypy` during `TYPE_CHECKING` but not runtime.
    # NOTE: Having this as a normal import causes a circular dependency issue
    # b/c `ipc_payload` depends on this that depend on exceptions, which would
    # depend on `ipc_payload`, etc... (hence the circle)
    from IrisBackendv3.ipc.ipc_payload import IpcPayload


class IpcException(Exception):
    """ Parent for all IPC exceptions. """
    pass


class IpcEndAppRequest(IpcException):
    """Exception to raise when a task / coroutine wants to end execution of the
    entire app.
    NOTE: Depending on how `return_when` of the app is configured, this may or
    may not actually end the app. The app may close when it gets an
    exception/completion from any task or it may end only when all of its
    managed tasks end.
    """

    def __init__(self, why: str = "") -> None:
        """Create a `IpcEndAppRequest`.

        Args:
            why (str, optional): Why the app is being ended. Defaults to "".
        """
        self.why = why
        self.message = f"IpcEndAppRequest: because {why}."
        super().__init__(self.why)


class MessageIntegrityError(IpcException):
    """ Error to raise when a received message fails integrity check. """

    def __init__(self, info: str = "") -> None:
        """Create a `MessageIntegrityError`.

        Args:
            info (str, optional): Info about the specific formatting issue. Defaults to "".
        """
        self.info = info
        self.message = f"MessageIntegrityError: {info}"
        super().__init__(self.message)


class DecodedInterProcessMessageContentTypeException(IpcException):
    """ The content of a decoded `InterProcessMessage` did not match the
    expected type.
    """

    def __init__(self, content: Any, expected_type: Type) -> None:
        """Create an `InterProcessMessageContentTypeException`."""
        self.content = content
        self.expected_type = expected_type
        self.message = (
            f"InterProcessMessageContentTypeException: "
            f"The content (`{content=}`) of a decoded `InterProcessMessage` "
            f"did not match the expected type (`{expected_type=}`)."
        )
        super().__init__(self.message)


class UnhandledTopicException(Exception):
    """ Exception to raise when a `SocketTopicHandler` doesn't have a
    handler for this payload's topic."""

    payload: 'IpcPayload'

    def __init__(self, payload: 'IpcPayload', info: str = "") -> None:
        """Create a `UnhandledTopicException`.

        Args:
            info (str, optional): Info about the specific formatting issue. Defaults to "".
        """
        self.payload = payload
        self.info = info
        self.message = f"UnhandledTopicException: {info}"
        super().__init__(self.message)
