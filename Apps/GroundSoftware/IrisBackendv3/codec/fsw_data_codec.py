#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Enum for En/Decoding FPrime Datatypes using Python Struct Strings.

@author: Connor W. Colombo (CMU)
@last-updated: 04/08/2021
"""
# Activate postponed annotations (for using classes as return type in their own methods):
from __future__ import annotations
from typing import cast, Optional, Any, Tuple
from enum import Enum
from abc import ABC, abstractmethod
import struct


class Codec(ABC):
    """
    Generic ABC detailing the methods which all type codecs for `FSWDataType`s 
    should have.
    """

    format_string: str

    def __init__(self, format_string: str):
        self.format_string = format_string

    @abstractmethod
    def check(self, *args: Any) -> bool:
        """
        Checks whether the given value(s) is able to be encoded as this type.
        """
        pass

    @abstractmethod
    def encode(self, *args: Any) -> bytes:
        """
        Encodes the given value(s) as the given type and returns the bytes.
        """
        pass

    @abstractmethod
    def decode(self, buffer: bytes) -> Tuple[Any, ...]:
        """
        Decodes the given bytes buffer as the given type and returns a tuple 
        containing the extracted values.
        """
        pass


class GenericCodec(Codec):
    """
    Defines how to code a generic object which should un/packed exactly as 
    `struct` would un/pack it.
    """

    def check(self, *args: Any) -> bool: return True

    def pack(self, *args: Any) -> bytes:
        return struct.pack(self.format_string, *args)

    def unpack(self, buffer: bytes) -> Tuple[Any, ...]:
        return struct.unpack(self.format_string, buffer)


class BooleanPacker(Codec):
    """
    Defines how to en/decode a boolean.
    """

    def check(self, *args: Any) -> bool:
        return len(args) == 1 and isinstance(args[0], bool)

    def pack(self, *args: bool) -> bytes:
        valid = self.check(args)
        if not valid:
            raise TypeMismatchException(bool, type(val))
        return struct.pack(self.format_string, *args)

    def unpack(self, buffer: bytes) -> Tuple[Any, ...]:
        return struct.unpack(self.format_string, buffer)
