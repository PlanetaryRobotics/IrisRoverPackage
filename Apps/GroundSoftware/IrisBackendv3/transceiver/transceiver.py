# -*- coding: utf-8 -*-

"""
Abstract Base Class encapsulating commong behaviors and interfaces for all
transceiver classes.

@author: Connor W. Colombo (CMU)
@last-updated: 05/18/2021
"""
from __future__ import annotations  # Activate postponed annotations (for using classes as return type in their own methods)

from typing import List
from abc import ABC, abstractmethod, abstractclassmethod, abstractproperty

from IrisBackendv3.codec.packet import Packet
from IrisBackendv3.codec.payload import Payload


class Transceiver(ABC):

    @abstractmethod
    def begin(self) -> None:
        """Initialize any special registers, etc. for this transceiver."""
        raise NotImplementedError()

    def read_update(self) -> List[Packet]:
        """Reads all available packets on the transceiver input."""
        raise NotImplementedError()

    def send(self, payloads: List[Payload]) -> None:
        """Sends the given payloads in as few packets as possible."""
        raise NotImplementedError()
