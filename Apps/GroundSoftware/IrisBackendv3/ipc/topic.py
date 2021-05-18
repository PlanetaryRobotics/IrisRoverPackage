#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
This module supplies an Enum which defines all topics used in the IPC on
pub/sub ports.

@author: Connor W. Colombo (CMU)
@last-updated: 05/18/2021
"""
# Activate postponed annotations (for using classes as return type in their own methods):
from __future__ import annotations
from typeguard import check_type
from typing import _GenericAlias, _VariadicGenericAlias, _SpecialForm  # type: ignore
from typing import List, Any, Type, Union
from enum import Enum

from IrisBackendv3.codec.packet import Packet
from IrisBackendv3.codec.payload import Payload

AnyType = Union[Type, _GenericAlias, _VariadicGenericAlias, _SpecialForm]


class Topic(Enum):
    """
    Enum for defines all ports used in the IPC.

    Try to make sure these ports aren't occupied on `localhost` the hosting
    device but feel free to change them here if needed. This enum should serve
    as the single source of truth for the port numbers.
    """

    # Try to keep topics to two bytes. All topic IDs must unique and not an
    # initial subset of any other topic (e.g. you could not have `0xDEADBEEF`
    # and `0xDEAD` but you could have `0xDEADBEEF` and `0xBEEF`).
    RX_PACKETS = b'\xFE\xED', Packet  # Packets received from any transceiver
    TX_PAYLOADS = b'\xF0\x0D', List[Payload]  # Payloads to send in a packet

    # Instance attributes for type-checker:
    # Type of data written to the topic (can be any valid mypy type):
    topic_type: AnyType

    def __new__(cls, val: bytes, topic_type: AnyType):
        """Constructs a new instance of the Enum."""
        obj = object.__new__(cls)
        obj._value_ = val

        check_type('topic_type', topic_type, AnyType)
        obj.topic_type = topic_type

        return obj

    def allows(self, data: Any) -> bool:
        """
        Checks whether the given data is allowed on the given topic based on
        its type.
        """
        try:
            check_type('data', data, self.topic_type)
            return True
        except TypeError:
            return False
