"""
Defines `Packet` class, a special `CodecContainer` used for Packet data.

@author: Connor W. Colombo (CMU)
@last-updated: 04/15/2022
"""
from __future__ import annotations  # Activate postponed annotations (for using classes as return type in their own methods)

from typing import List, Optional, Callable, Tuple, TypeVar, Dict
from abc import ABC, abstractmethod

from ..container import ContainerCodec
from ..payload_collection import EnhancedPayloadCollection

from ..settings import ENDIANNESS_CODE

CT = TypeVar('CT')


class Packet(ContainerCodec[CT], ABC):
    """
    A special `CodecContainer` used for Packet data which combines the
    standardized data-encoding features of `ContainerCodec` with a standard
    `EnhancedPayloadCollection`. All packets are essentially 
    `EnhancedPayloadCollection`s with specialized encoding and decoding schemes
    applied. Each `Packet` en/decodes raw bytes sent over a `Transceiver` layer
    with any network headers (IP etc.) already stripped off.
    """

    __slots__: List[str] = [
        '_payloads'  # collection of all Payloads, separated by type
    ]

    _payloads: EnhancedPayloadCollection

    @property
    def payloads(self) -> EnhancedPayloadCollection:
        """Wrapper for `_payloads`. This getter can be overridden in subclasses
        for additional functionality (e.g. autocaching & updating of data used
        to generate or generated from payloads)."""
        return self._payloads

    @payloads.setter
    def payloads(self, payloads: EnhancedPayloadCollection) -> None:
        """Wrapper for `_payloads`. This getter can be overridden in subclasses
        for additional functionality (e.g. autocaching & updating of data used
        to generate or generated from payloads)."""
        self._payloads = payloads

    def __init__(self,
                 payloads: EnhancedPayloadCollection,
                 raw: Optional[bytes] = None,
                 endianness_code: str = ENDIANNESS_CODE
                 ) -> None:
        # Set payloads using `.payloads` and not `._payloads` so any special
        # `@payloads.setter` added in a subclass will be automatically called.
        self.payloads = payloads
        super().__init__(raw=raw, endianness_code=endianness_code)  # passthru

    @classmethod
    @abstractmethod
    def is_valid(cls, data: bytes, endianness_code: str = ENDIANNESS_CODE) -> bool:
        """
        Determines whether the given bytes constitute a valid packet of this type.
        """
        raise NotImplementedError()

    @classmethod
    @abstractmethod
    def build_minimum_packet(
        cls,
        payloads: EnhancedPayloadCollection,
        raw: Optional[bytes],
        endianness_code: str
    ) -> CT:
        """ Builds a minimum representation of this Packet (before any
        additional elements from `__getstate__` are added back). Used by
        `Packet.__reduce__` for unpacking serialized data.
        """
        raise NotImplementedError()

    def __reduce__(self) -> Tuple[Callable, Tuple[EnhancedPayloadCollection, bytes, str], Optional[Dict]]:
        """
        `Packet` `__reduce__` differs from the `__reduce__` of other 
        `ContainerCodec` subclasses b/c Packets are really just fancy
        `EnhancedPayloadCollection`s. Even though the payloads themselves can
        be derived from `_raw`, each of them have their own metadata, so they
        should be encoded independently through their on `__reduce__` methods
        which include any metadata which comes from their `__getstate__`.
        """
        # *Don't* automatically re-encode to update fields in case some part of
        # algorithm is broken and received raw data is lost.
        # (The first point of storing the raw is to be able to forensically
        # reconstruct what was seen during mission, so use same inputs to get
        # the same outputs).
        # This also allows for easy re-evaluation of all saved telemetry by
        # simply adjusting the decode / parsers and deserializing again.
        if self._raw is None:
            self._raw = self.encode()

        # If the subclass is set up to encode a state (metadata not stored in
        # `raw`), grab it:
        if hasattr(self, '__getstate__'):
            state = self.__getattribute__('__getstate__')()
        else:
            state = None

        # The "Callable object" returned will be the decoding function:
        # If a subclassed object is reduced, it will call that subclass' `decode`
        # function (assuming it's been implemented).
        # The output of `decode` will then have its `__setstate__` called with
        # an argument of `state` (to build back metadata).
        return (
            self.__class__.build_minimum_packet,
            (self.payloads, self._raw, self._endianness_code),
            state
        )
