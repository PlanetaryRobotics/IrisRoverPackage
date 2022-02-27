"""
Various exceptions used throughout the IPC package.

@author: Connor W. Colombo (CMU)
@last-updated: 05/27/2022
"""


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
