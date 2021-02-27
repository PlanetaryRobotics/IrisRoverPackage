#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""
Defines Common Data Required for Payloads. Support for Building and Parsing 
Payloads as part of a Variable Length Payload.

@author: Connor W. Colombo (CMU)
@last-updated: 12/27/2020
"""
from __future__ import annotations  # Activate postponed annotations (for using classes as return type in their own methods)

from typing import List
from abc import ABC, abstractmethod, abstractclassmethod, abstractproperty

import struct


class Payload(ABC):
    """
    Generic Interface for Handling and Processing Payloads.

    All data is stored in the unpacked form since that's how it will most 
    often be handled and transformed and only packed into bytes when needed.
    """

    __slots__: List[str] = [
        'endianness_code',  # - Python Struct Endianness Code
    ]

    def __init__(self, endianness_code: str) -> None:
        self.endianness_code = endianness_code

    @abstractclassmethod
    def from_bytes(cls, endianness_code: str, data: bytes) -> Payload:
        """Construct a CPH Object from Bytes. Alias for unpack."""
        pass

    @abstractmethod
    def pack(self) -> bytes:
        """Pack data into a bytes object."""
        pass

    @property
    def raw_data(self) -> bytes:
        """Return the raw bytes. Property alias for pack."""
        return self.pack()


class DownlinkTimes():
    __slots__: ['rover_tx', 'lander_rx', 'lander_tx', 'amcc_rx', 'pmcc_rx']


class TelemetryPayload(Payload):
    """Implementation of Payload Interface for IRIS Telemetry Payloads."""

    __slots__: List[str] = [
        'yamcs_source',  # Source in YAMCS (eg. wifi or wired)
        'module_id',  # - ID of the Target Module in FPrime
        'channel_id',  # - ID of the Telemetry Channel in the FPrime Module
        'size',  # - Size of the Core Telemetry Data being Transmitted
        'data',  # - Core Raw Telemetry Data being Transmitted
        'raw_value',  # - Raw value
        'converted_value',  # - Converted value, if applicable
        'timestamp'  # - Time this Telemetry Payload was Generated on the Rover
    ]

    # raw_value: Union[float, str]
    # converted_value: Union[float, str]

    def __init__(self,
                 endianness_code: str,
                 module_id: int,

                 )

    @property
    def size(self) -> int:
        # Lookup from table based on module id | channel id

        # TODO:
        # Dict should be prebuilt elsewhere (?) possibly in a staticmethod which runs and caches on first call? (...or as part of a larger external lookup process involving commands etc.?)
        # ...
        # Point to FPrimeXMLData class instance... constructor can either reindex/rebuild or load from saved cache file?
        pass

    @property
    def ground_metadata(self) -> dict:
        # Lookup from table based on module id | channel id
        pass

    @classmethod
    def from_bytes(cls, endianness_code: str, data: bytes) -> Payload:
        """Construct a CPH Object from Bytes. Alias for unpack."""
        pass

    def pack(self) -> bytes:
        """Pack data into a bytes object."""
        pass
