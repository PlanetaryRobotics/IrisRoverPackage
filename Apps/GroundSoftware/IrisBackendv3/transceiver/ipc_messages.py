"""
Defines the Structure, Content, and Serialization for all incoming and outgoing 
Messages for the Transceiver layer over IPC.

@author: Connor W. Colombo (CMU)
@last-updated: 02/26/2022
"""
from __future__ import annotations  # Activate postponed annotations (for using classes as return type in their own methods)

from typing import Type, List
import attr

from IrisBackendv3.ipc.wrapper import InterProcessMessage
from IrisBackendv3.codec.payload import PayloadCollection
from IrisBackendv3.codec.metadata import DataPathway, DataSource


@attr.s(frozen=True, cmp=True, slots=True, auto_attribs=True)
class SendPayloadsRequestContent:
    """
    Defines the Message Content for a Request to Send a `PayloadCollection`
    from `DataSource` over `DataPathway`.
    """
    payloads: PayloadCollection
    pathway: DataPathway
    source: DataSource


class SendPayloadsRequestMessage(InterProcessMessage[SendPayloadsRequestContent]):
    """
    Defines the Message Structure and Serialization Scheme for a Request to 
    Send a `PayloadCollection` from `DataSource` over `DataPathway`.
    """

    def to_ipc_bytes(self) -> bytes:
        raise NotImplementedError()  # !TODO

    def from_ipc_bytes(cls, data: bytes) -> SendPayloadsRequestContent:
        raise NotImplementedError()  # !TODO
