"""
Defines `Packet` class, a special `CodecContainer` used for Packet data.

@author: Connor W. Colombo (CMU)
@last-updated: 09/15/2022
"""
from __future__ import annotations  # Activate postponed annotations (for using classes as return type in their own methods)

from typing import Any, List, Optional, Callable, Tuple, TypeVar, Dict, cast
from abc import ABC, abstractmethod

from ..container import ContainerCodec
from ..payload_collection import EnhancedPayloadCollection
from ..metadata import DataPathway, DataSource

from ..settings import ENDIANNESS_CODE
from ..logging import logger


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
        '_payloads',  # collection of all Payloads, separated by type
        '_pathway',  # optionally, how this packet was transmitted from the rover
        '_source'  # optionally, where we got this packet
    ]

    _payloads: EnhancedPayloadCollection
    _pathway: Optional[DataPathway]
    _source: Optional[DataSource]

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

    @property
    def pathway(self) -> DataPathway:
        """Returns the pathway for the packet."""
        if self._pathway is None:
            # If no pathway has been set, attempt to set it based on payloads:
            if self.payloads.num_payloads > 0:
                payloads = [*self.payloads.all_payloads]
                payload0_pathway = payloads[0].pathway
                if any(p.pathway != payload0_pathway for p in payloads):
                    # Warn if all payloads don't share the same pathway:
                    logger.warning(
                        f"Packet {self} contains non-homogeneous payload "
                        f"pathways (they're not all the same). "
                        f"Payload pathways are: {[p.pathway for p in payloads]}."
                    )
                # Set pathway based on payload 0:
                self._pathway = payload0_pathway
            else:
                # nothing's been set yet and there aren't any payloads to set
                # it based on. So just *return* (but don't set) NONE:
                return DataPathway.NONE

        return self._pathway

    @pathway.setter
    def pathway(self, pathway: DataPathway) -> None:
        """Sets the pathway for this packet, in its own metadata
        **as well as the metadata of all payloads in the packet**."""
        self._pathway = pathway
        for payload in self.payloads.all_payloads:
            payload.pathway = pathway

    @property
    def source(self) -> DataSource:
        """Returns the source for the packet."""
        if self._source is None:
            # If no source has been set, attempt to set it based on payloads:
            if self.payloads.num_payloads > 0:
                payloads = [*self.payloads.all_payloads]
                payload0_source = payloads[0].source
                if any(p.source != payload0_source for p in payloads):
                    # Warn if all payloads don't share the same pathway:
                    logger.warning(
                        f"Packet {self} contains non-homogeneous payload "
                        f"sources (they're not all the same). "
                        f"Payload sources are: {[p.source for p in payloads]}."
                    )
                # Set source based on payload 0:
                self._source = payload0_source
            else:
                # nothing's been set yet and there aren't any payloads to set
                # it based on. So just *return* (but don't set) NONE:
                return DataSource.NONE

        return self._source

    @source.setter
    def source(self, source: DataSource) -> None:
        """Sets the source for this packet, in its own metadata
        **as well as the metadata of all payloads in the packet**."""
        self._source = source
        for payload in self.payloads.all_payloads:
            payload.source = source

    def __init__(self,
                 payloads: EnhancedPayloadCollection,
                 raw: Optional[bytes] = None,
                 endianness_code: str = ENDIANNESS_CODE
                 ) -> None:
        # Set payloads using `.payloads` and not `._payloads` so any special
        # `@payloads.setter` added in a subclass will be automatically called.
        self.payloads = payloads
        # Set pathway and source to a default value of none to signal this
        # metadata hasn't been added yet (these are optional and added separately):
        self._pathway = None
        self._source = None
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

    def __getstate__(self) -> Dict[str, Any]:
        """Encode metadata which is not stored in `payloads`.
        """
        state: Dict[str, Any] = dict()
        # Only bother encoding pathway and source if not None:
        # (using the properties here, not the private variables since that
        # gives us the opportunity to grab values from the payloads
        # )
        if self.pathway != DataPathway.NONE:
            state['pkt_pwy'] = self.pathway
        if self.source != DataSource.NONE:
            state['pkt_src'] = self.source
        return state

    def __setstate__(self, data: Dict[str, Any]) -> None:
        """Retrieve metadata which is not stored in `payloads`."""
        if not isinstance(data, dict):
            raise TypeError(f"`data` should be dict. Got {data}.")

        if 'pkt_pwy' in data:
            # if wasn't included in data, this value will stay at its default:
            # None
            if not isinstance(data['pkt_pwy'], DataPathway):
                raise TypeError(
                    f"`data['pkt_pwy']` should be DataPathway. "
                    f"Got `{data['pkt_pwy']}`."
                )
            self._pathway = data['pkt_pwy']
        if 'pkt_src' in data:
            # if wasn't included in data, this value will stay at its default:
            # None
            if not isinstance(data['pkt_src'], DataSource):
                raise TypeError(
                    f"`data['pkt_src']` should be DataSource. "
                    f"`Got {data['pkt_src']}`.")
            self._source = data['pkt_src']

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
