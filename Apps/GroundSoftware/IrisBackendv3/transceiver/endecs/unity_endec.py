from .endec import *


class UnityEndec(Endec):
    def encode(self, data: bytes) -> bytes:
        """ Encodes data to send over a `Transceiver`.
        Default is a unity encoder (just returns the bytes).
        """
        return data

    def decode(self, data: bytes) -> bytes:
        """ Decodes data sent over a `Transceiver`.
        Default is a unity decoder (just returns the bytes).
        """
        return data
