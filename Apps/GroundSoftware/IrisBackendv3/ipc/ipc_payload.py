"""
Defines a simple container class for data sent over IPC (i.e. a container for
the raw message data alongside routing information (topic, subtopic)).

NOTE: This code used to live inside `wrapper.py`

@author: Connor W. Colombo (CMU)
@last-updated: 03/04/2023
"""
# Activate postponed annotations (for using classes as return type in their own methods)
from __future__ import annotations
import dataclasses

from .topics_registry import Topic


@dataclasses.dataclass(order=True)
class IpcPayload:
    """
    Data sent over IPC.
    """
    topic_bytes: bytes = b''  # if applicable
    subtopic_bytes: bytes = b''  # if applicable
    msg_bytes: bytes = b''  # message

    @property
    def topic(self) -> Topic:
        # Type ignore note: mypy doesn't recognize that Enum() doesn't call
        # __new__ but is instead an Enum indexing operation and, thus,
        # doesn't need __new__'s arguments.
        return Topic(self.topic_bytes)  # type: ignore
