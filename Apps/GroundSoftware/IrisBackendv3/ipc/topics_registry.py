"""
This module supplies an Enum which defines all topics used in the IPC on
pub/sub ports.

This registry serves two purposes:
  1. Assign `TopicDefinitions` to ports.
  2. Provide an enumeration of all available `Topics`.

@author: Connor W. Colombo (CMU)
@last-updated: 03/06/2023
"""
# Activate postponed annotations (for using classes as return type in their own methods):
from __future__ import annotations
from typeguard import check_type
from typing import Any

from enum import Enum

from IrisBackendv3.ipc.topic_definition import TopicDefinition
from IrisBackendv3.ipc.topics import *


class Topic(Enum):
    """
    Enum for defines all ports used in the IPC.

    This enum should serve as the single source of truth for the topic 
    information.
    """

    # Try to keep topics to two bytes. All topic IDs must unique and not an
    # initial subset of any other topic (e.g. you could not have `0xDEADBEEF`
    # and `0xDEAD` but you could have `0xDEADBEEF` and `0xBEEF`).

    # Topic Assignments:
    # (see `TopicDefinition`s (`TD``) and their messages for more details on
    # the contents of the specific topics):
    DL_PACKETS = b'\xFE\xED', TD_DL_PACKETS
    DL_PAYLOADS = b'\xFE\xEE', TD_DL_PAYLOADS
    UL_PACKET = b'\xF0\x01', TD_UL_PACKET
    UL_PAYLOADS = b'\xF0\x0D', TD_UL_PAYLOADS

    # Definition of the topic:
    definition: TopicDefinition

    def __new__(cls, val: bytes, topic_def: TopicDefinition):
        """Constructs a new instance of the Enum."""
        obj = object.__new__(cls)
        obj._value_ = val

        check_type('topic_def', topic_def, TopicDefinition)
        obj.definition = topic_def

        return obj

    def allows(self, data: Any) -> bool:
        """
        Checks whether the given data is allowed on the given topic based on
        its type.
        """
        return self.definition.allows(data)
