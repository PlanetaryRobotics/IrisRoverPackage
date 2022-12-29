"""
Various exceptions used throughout the IPC package.

@author: Connor W. Colombo (CMU)
@last-updated: 11/30/2022
"""

from .wrapper import IpcPayload


class IpcException(Exception):
    """ Parent for all IPC exceptions. """
    pass


class MessageIntegrityError(IpcException):
    """ Error to raise when a received message fails integrity check. """

    def __init__(self, info: str = "") -> None:
        """Create a MessageIntegrityError.

        Args:
            info (str, optional): Info about the specific formatting issue. Defaults to "".
        """
        self.info = info
        self.message = f"MessageIntegrityError: {info}"
        super().__init__(self.message)


class UnhandledTopicException(Exception):
    """ Exception to raise when a `SocketTopicHandler` doesn't have a
    handler for this payload's topic."""

    payload: IpcPayload

    def __init__(self, payload: IpcPayload, info: str = "") -> None:
        """Create a MessageIntegrityError.

        Args:
            info (str, optional): Info about the specific formatting issue. Defaults to "".
        """
        self.payload = payload
        self.info = info
        self.message = f"UnhandledTopicException: {info}"
        super().__init__(self.message)
