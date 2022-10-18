#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Enum for En/Decoding FPrime Datatypes using Python Struct Strings.

@author: Connor W. Colombo (CMU)
@last-updated: 10/17/2022
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
        FswDataCategory.STRING: StringPacker,
        FswDataCategory.VARSTRING: StringPacker
    })[datatype.category]


def format_string(datatype: FswDataType, data: Optional[bytes] = None) -> str:
    """
    Creates the appropriate format string for the given `FswDataType`.

    This format string only depends on the data in the case of a Variable
    length string, in which case the data needs to be supplied.
    """
    if datatype.category in [FswDataCategory.STRING, FswDataCategory.VARSTRING]:
        endianness = '>'  # FPrime encodes all strings as big endian
    else:
        endianness = ENDIANNESS_CODE

    # Variable length strings get a special format_string since their length
    # depends on the length of the data:
    if datatype.category == FswDataCategory.VARSTRING:
        if isinstance(data, bytes):
            struct_sym = f"H{len(data)}s"
        else:
            # Bad datatype given. Just use the default:
            struct_sym = datatype.struct_sym
    else:
        struct_sym = datatype.struct_sym

    return endianness + struct_sym


def encode(datatype: FswDataType, data: Any) -> bytes:
    """
    Encodes the given data which has the given FswDataType.
    (A simple external access method.)
    """
    codec = category_to_codec(datatype)
    if codec == StringPacker:
        if not isinstance(data, str):
            raise PacketEncodingException(
                data,
                (
                    f"Failed to encode {data} as a FSW String because the type "
                    f"of the given data is {type(data)} and it needs to be str."
                )
            )

        # Must perform length-based calculations using the *encoded* length
        # of the string:
        raw_data = StringPacker.encode_str(data)

        if datatype.category == FswDataCategory.VARSTRING:
            target_raw_data_len = len(raw_data)
        else:
            # -2 because first two bytes of encoding are the length:
            target_raw_data_len = datatype.get_actual_num_bytes() - 2

        fstr = format_string(datatype, raw_data)
        en = codec.encode(fstr, (target_raw_data_len, data))
    else:
        fstr = format_string(datatype)
        en = codec.encode(fstr, data)
    return en


def decode(datatype: FswDataType, buffer: bytes) -> Any:
    """
    Decodes the given data which has the given FswDataType.

    Given buffer must only contain data that should be part of this field
    (and not, for example, the entire rest of the message).

    (A simple external access method.)
    """
    codec = category_to_codec(datatype)
    if codec == StringPacker:
        dec = codec.decode(
            # [2:] to skip length bytes
            format_string=format_string(datatype, buffer[2:]),
            buffer=buffer
        )[1]  # [1] to grab just the string, we don't care about the length
    else:
        dec = codec.decode(
            format_string=format_string(datatype),
            buffer=buffer
        )
    return dec


CT = TypeVar('CT')


class Codec(Generic[CT], ABC):
    """
    Generic ABC detailing the methods which all type codecs for `FSWDataType`s
    should have.
    """

    @classmethod
    @abstractmethod
    def check(cls, format_string: str, data: CT) -> bool:
        """
        Checks whether the given value(s) is able to be encoded as this type.
        """
        pass

    @classmethod
    @abstractmethod
    def encode(cls, format_string: str, data: CT) -> bytes:
        """
        Encodes the given value(s) as the given type and returns the bytes.
        """
        pass

    @classmethod
    @abstractmethod
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
    def check(cls, format_string: str,
              args: Tuple[Any, ...]) -> bool: return True

    @classmethod
    def encode(cls, format_string: str, args: Tuple[Any, ...]) -> bytes:
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
    This is also used for Variable-Length strings since the variable length of
    the string is given as the "fixed" length before using the packer.
    *Note:* Here the *args must be [fixed_length, string]
    """

    @staticmethod
    def encode_str(data: str) -> bytes:
        # Encode the string, processing any **raw** non-ascii escape strings:
        # e.x. r'\xBE\xEF' becomes b'\xBE\xEF':
        return data.encode('ascii')\
                   .decode('unicode_escape')\
                   .encode('raw_unicode_escape')

    @staticmethod
    def decode_str(raw_data: bytes) -> str:
        # Convert raw_data (byte string) to a str, converting non-ascii hex
        # into an escaped encoding like: r'\xAA':
        return raw_data.decode('raw_unicode_escape')\
                       .encode('unicode_escape')\
                       .decode('ascii')

    @classmethod
    def check(cls, format_string: str, args: Tuple[int, Union[str, bytes]]) -> bool:
        """
        Checks the if the given vals can be used to encode as a Fixed Length String.
        where *args are [fixed_length, string]
        """
        return (
            len(args) == 2
            and isinstance(args[0], int)
            and isinstance(args[1], (str, bytes))
            and len(args[1]) >= args[0]  # AT LEAST enough data given.
        )

    @classmethod
    def encode(cls, format_string: str, args: Tuple[int, str]) -> bytes:
        """
        Encodes the given val as a String.
        where *args are [string_length, string]
        """
        raw_data_len = args[0]
        raw_data = cls.encode_str(args[1][:raw_data_len])

        valid = cls.check(format_string, (raw_data_len, raw_data))
        if not valid:
            raise PacketEncodingException(
                args,
                f"Data `{args}` not able to be encoded as a String."
                f"Where data should contain [target_length, string]"
                f"and the target_length should reflect the length of the "
                f"string after encoding, which is: {raw_data!r} w/ "
                f"len={len(raw_data)}"
            )

        return struct.pack(format_string, raw_data_len, raw_data)

    @classmethod
    def decode(cls, format_string: str, buffer: bytes) -> Tuple[int, str]:
        """
        Decodes the given val as a String.
        """
        data_len, raw_data = struct.unpack(format_string, buffer)
        # Must have enough data (when sending fixed length strings, FPrime will
        # give more data than the actual buffer being sent and set the length
        # to the length of the intended usable portion).
        if data_len > len(raw_data):
            raise PacketDecodingException(
                buffer,
                f"The expected length of the string ({data_len}B) "
                f"is greater than the length of available raw length ({len(raw_data)}B)."
            )

        return (data_len, cls.decode_str(raw_data)[:data_len])
