#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Enum for En/Decoding FPrime Datatypes using Python Struct Strings.

@author: Connor W. Colombo (CMU)
@last-updated: 04/16/2021
"""
# Activate postponed annotations (for using classes as return type in their own methods):
from __future__ import annotations
from typing import cast, Optional, Any, Tuple, Union, Dict, Generic, TypeVar
from enum import Enum
from abc import ABC, abstractmethod
import struct

from .exceptions import PacketDecodingException, PacketEncodingException
from .settings import ENDIANNESS_CODE

from IrisBackendv3.data_standards.fsw_data_type import FswDataType
from IrisBackendv3.data_standards.fsw_data_type import Category as FswDataCategory


def category_to_codec(datatype: FswDataType) -> Codec:
    """
    Returns the appropriate Codec for the given FswDataType.
    """
    if type == FswDataCategory.EMPTY:
        raise TypeError(
            "Data with FswDataType Category `EMPTY` Cannot be Encoded/Decoded."
        )

    return cast(Dict[FswDataCategory, Codec], {
        FswDataCategory.NUMBER: NumberPacker,
        FswDataCategory.BOOLEAN: BooleanPacker,
        FswDataCategory.ENUM: NumberPacker,
        FswDataCategory.STRING: StringPacker
    })[datatype.category]


def fstr(datatype: FswDataType) -> str:
    """
    Creates the appropriate format string for the given `FswDataType`.
    """
    if datatype.category == FswDataCategory.STRING:
        endianness = '>'  # FPrime encodes all strings as big endian
    else:
        endianness = ENDIANNESS_CODE
    return endianness + datatype.struct_sym


def encode(datatype: FswDataType, data: Any) -> bytes:
    """
    Encodes the given data which has the given FswDataType.
    (A simple external access method.)
    """
    codec = category_to_codec(datatype)
    fstr = fstr(datatype)
    if not codec.check(fstr, data):
        raise PacketEncodingException(
            data,
            (
                f"Data failed check with `{codec}` Codec for `{datatype}` data."
            )
        )
    if codec == StringPacker:
        en = codec.encode(fstr, ((datatype.num_octets-2), data))
    else:
        en = codec.encode(fstr, data)
    return en


def decode(datatype: FswDataType, buffer: bytes) -> Any:
    """
    Decodes the given data which has the given FswDataType.
    (A simple external access method.)
    """
    codec = category_to_codec(datatype)
    if codec == StringPacker:
        dec = codec.decode(format_string=fstr(datatype), buffer=buffer)[1]
    else:
        dec = codec.decode(format_string=fstr(datatype), buffer=buffer)
    return dec


CT = TypeVar('CT')


class Codec(Generic[CT], ABC):
    """
    Generic ABC detailing the methods which all type codecs for `FSWDataType`s
    should have.
    """

    @classmethod
    @ abstractmethod
    def check(cls, format_string: str, data: CT) -> bool:
        """
        Checks whether the given value(s) is able to be encoded as this type.
        """
        pass

    @classmethod
    @ abstractmethod
    def encode(cls, format_string: str, data: CT) -> bytes:
        """
        Encodes the given value(s) as the given type and returns the bytes.
        """
        pass

    @classmethod
    @ abstractmethod
    def decode(cls, format_string: str, buffer: bytes) -> CT:
        """
        Decodes the given bytes buffer as the given type and returns a tuple
        containing the extracted values.
        """
        pass


class GenericCodec(Codec[Tuple[Any, ...]]):
    """
    Defines how to code a generic object which should un/packed exactly as
    `struct` would un/pack it.
    """

    @classmethod
    def check(cls, format_string: str, *args: Any) -> bool: return True

    @classmethod
    def encode(cls, format_string: str, *args: Any) -> bytes:
        return struct.pack(format_string, *args)

    @classmethod
    def decode(cls, format_string: str, buffer: bytes) -> Tuple[Any, ...]:
        return struct.unpack(format_string, buffer)


class NumberPacker(Codec[Union[int, float]]):
    """
    Defines how to en/decode a Numeric Type.
    """
    @classmethod
    def check(cls, format_string: str, data: Union[int, float]) -> bool:
        return type(data) == float or type(data) == int

    @classmethod
    def encode(cls, format_string: str, data: Union[int, float]) -> bytes:
        valid = cls.check(format_string, data)
        if not valid:
            raise PacketEncodingException(
                data,
                f"Data `{data}` not able to be encoded as Number."
            )
        return struct.pack(format_string, data)

    @classmethod
    def decode(cls, format_string: str, buffer: bytes) -> Union[float, int]:
        return cast(Union[int, float], struct.unpack(format_string, buffer)[0])


class BooleanPacker(Codec[bool]):
    """
    Defines how to en/decode a Boolean.
    """

    @classmethod
    def check(cls, format_string: str, data: bool) -> bool:
        return isinstance(data, bool)

    @classmethod
    def encode(cls, format_string: str, data: bool) -> bytes:
        valid = cls.check(format_string, data)
        if not valid:
            raise PacketEncodingException(
                data,
                f"Data `{data}` not able to be encoded as Boolean."
            )
        # need bool() cast in case types are ignored (force bits into right place):
        return struct.pack(format_string, 0xFF * bool(data))

    @classmethod
    def decode(cls, format_string: str, buffer: bytes) -> bool:
        return bool(struct.unpack(format_string, buffer)[0])


class StringPacker(Codec[Tuple[int, str]]):
    """
    Defines how to en/decode a Fixed-Length String.
    *Note:* Here the *args must be [fixed_length, string]
    """

    @classmethod
    def check(cls, format_string: str, args: Tuple[int, str]) -> bool:
        """
        Checks the if the given vals can be used to encode as a Fixed Length String.
        where *args are [fixed_length, string]
        """
        return (
            len(args) == 2
            and isinstance(args[0], int)
            and isinstance(args[1], str)
            and len(args[1]) == args[0]
        )

    @classmethod
    def encode(cls, format_string: str, *args: Tuple[int, str]) -> bytes:
        """
        Encodes the given val as a Fixed Length String.
        where *args are [fixed_length, string]
        """
        valid = cls.check(format_string, *args)
        if not valid:
            raise PacketEncodingException(
                args,
                f"Data `{args}` not able to be encoded as Fixed-Length String."
                f"Where data should contain [target_fixed_length, string]."
            )
        return struct.pack(format_string, *args)

    @classmethod
    def decode(cls, format_string: str, buffer: bytes) -> Tuple[int, str]:
        """
        Decodes the given val as a Fixed Length String.
        """
        fixed_length, data = struct.unpack(format_string, buffer)
        if fixed_length != len(data):
            raise PacketDecodingException(
                buffer,
                f"The expected length of the fixed-length string ({fixed_length}B) "
                f"does not match its actual length ({len(data)}B)."
            )
        return (fixed_length, data)
