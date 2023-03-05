from abc import ABC, abstractmethod

from ..logging import logger
from ..exceptions import TransceiverDecodingException


class Endec(ABC):
    """
    Abstract Base Class for encoder/decoder to convert bytes for `Transceiver`.
    messages.
    """
    @abstractmethod
    def encode(self, data: bytes) -> bytes:
        raise NotImplementedError()

    @abstractmethod
    def decode(self, data: bytes) -> bytes:
        raise NotImplementedError()
