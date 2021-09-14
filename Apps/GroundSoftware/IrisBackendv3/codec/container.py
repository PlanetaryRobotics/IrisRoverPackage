#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""
Core interface definition for all container classes whose data should be
encoded/decoded to be passed in/out of the Transceiver layer.

@author: Connor W. Colombo (CMU)
@last-updated: 04/14/2020
"""
from typing import List, Tuple, Any, Dict, Optional, Callable, Generic, TypeVar
from abc import ABC, abstractmethod
from enum import Enum
import struct

from .settings import ENDIANNESS_CODE

CT = TypeVar('CT')


class ContainerCodec(Generic[CT], ABC):
    """
    Core interface definition for all container classes whose data should be
    encoded/decoded to be passed in/out of the Transceiver layer.

    Also serializable (pickleable) as raw data.

    Note: often [CT] is the implementing subclass.
    """

    __slots__: List[str] = [
        # Python Struct Endianness Code (used for en/decoding):
        '_endianness_code',
        # Raw bytes from the Transceiver which were parsed:
        # Kept for forensics in case something goes wrong.
        # `None` if this Container is constructed and hasn't yet been encoded.
        # Also used for efficient serialization since all the data in this
        # object should be able to be reconstructed by just knowing the raw data
        # and the endianness.
        '_raw'
    ]

    _endianness_code: str
    _raw: Optional[bytes]

    def __init__(self,
                 raw: Optional[bytes] = None,
                 endianness_code: str = ENDIANNESS_CODE
                 ) -> None:
        self._raw = raw
        self._endianness_code = endianness_code

    # Make public get, private set to signal that you can freely use these values
    # but modifying them directly can yield undefined behavior (specifically
    # `raw` not syncing up with whatever other data is in the container)
    @property
    def endianness_code(self) -> str: return self._endianness_code
    @property
    def raw(self) -> Optional[bytes]: return self._raw

    @classmethod
    @abstractmethod
    def decode(self, data: bytes, endianness_code: str = ENDIANNESS_CODE) -> CT:
        """
        Decodes the given bytes buffer.
        """
        raise NotImplementedError()

    @abstractmethod
    def encode(self, **kwargs: Any) -> bytes:
        """
        Encodes the data contained in this instance using this class' formatting.
        (normally should just use instance's contained data and not require any
        kwargs). Just make sure all overriding methods still work if kwargs
        aren't supplied.
        """
        raise NotImplementedError()

    def __reduce__(self) -> Tuple[Callable, Tuple[bytes, str], Optional[Dict]]:
        # *Don't* automatically re-encode to update fields in case some part of
        # algorithm is broken and received raw data is lost.
        # (The first point of storing the raw is to be able to forensically
        # reconstruct what was seen during mission, so use same inputs to get
        # the same outputs).
        # This also allows for easy re-evaluation of all saved telemetry by
        # simply adjusting the decode / parsers and deserializing again.
        if self._raw is None:
            self._raw = self.encode()

        # If the subclass is setup to encode a state (metadata not stored in
        # `raw`), grab it:
        if hasattr(self, '__getstate__'):
            state = self.__getattribute__('__getstate__')()
        else:
            state = None

        # "Callable object" returned will be the decoding function:
        # If a subclassed object is reduced, it will call that subclass' `decode`
        # function (assuming it's been implemented).
        return (self.__class__.decode, (self._raw, self._endianness_code), state)

    def __eq__(self, other) -> bool:
        """
        Performs an equality test based on all attributes deemed essential to 
        the container's content (that is, encoded in `__reduce__`).
        """
        if not isinstance(other, ContainerCodec):
            return False

        _, self_data, self_state = self.__reduce__()
        _, other_data, other_state = other.__reduce__()

        #! TODO: WORKING HERE
        return (
            self_data == other_data
            and self_state == other_state
        )
