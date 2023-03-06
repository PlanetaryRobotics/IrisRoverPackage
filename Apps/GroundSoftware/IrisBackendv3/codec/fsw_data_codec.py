#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Enum for En/Decoding FPrime Datatypes using Python Struct Strings.

@author: Connor W. Colombo (CMU)
@last-updated: 11/12/2022
"""
# Activate postponed annotations (for using classes as return type in their own methods):
from __future__ import annotations
from typing import cast, Optional, Any, Tuple, Union, Dict, Generic, TypeVar
from enum import Enum
from abc import ABC, abstractmethod
import struct

from IrisBackendv3.codec.exceptions import PacketDecodingException, PacketEncodingException
from IrisBackendv3.codec.settings import ENDIANNESS_CODE

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
        FswDataCategory.VARSTRING: StringPacker,
        FswDataCategory.IRISBYTESTRING: BytesPacker
    })[datatype.category]


def format_string(datatype: FswDataType, data: Optional[bytes] = None) -> str:
    """
    Creates the appropriate format string for the given `FswDataType`.

    This format string only depends on the data in the case of a Variable
    length string, in which case the data needs to be supplied.
    """
    if datatype.category in [FswDataCategory.STRING, FswDataCategory.VARSTRING, FswDataCategory.IRISBYTESTRING]:
        # Strings get a special format_string since their length
        # depends on the length of the data:
        # Note: `Fw::IrisCmdByteStringArg` is derived from strings in FPrime.
        # Serialization is the same, it just also allows 0x00 anywhere in its contents.
        endianness = '>'  # FPrime encodes all strings as big endian
        if isinstance(data, bytes):
            struct_sym = f"H{len(data)}s"
        else:
            # Bad datatype given. Just use the default:
            struct_sym = datatype.struct_sym
    else:
        endianness = ENDIANNESS_CODE
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

        fstr = format_string(datatype, raw_data)
        en = codec.encode(fstr, (datatype.get_max_num_bytes(), data))

    elif codec == BytesPacker:
        if not isinstance(data, bytes):
            raise PacketEncodingException(
                data,
                (
                    f"Failed to encode {data!r} as a FSW BytesString because the type "
                    f"of the given data is {type(data)} and it needs to be bytes."
                )
            )

        fstr = format_string(datatype, data)
        en = codec.encode(fstr, (datatype.get_max_num_bytes(), data))

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
    if codec in [StringPacker, BytesPacker]:
        dec_len, dec = codec.decode(
            # [2:] to skip length bytes
            format_string=format_string(datatype, buffer[2:]),
            buffer=buffer
        )

        if dec_len > datatype.get_max_num_bytes():
            raise PacketDecodingException(
                buffer,
                (f"The length of the string/bytestring `{dec!r}` ({dec_len-2}B) "
                 f"is greater than the max capacity of the datatype "
                 f"{datatype} ({datatype.get_max_num_bytes()-2}B).")
                # -2 to ignore length bytes in the print (easier to understand for the reader)
            )
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
    Defines how to en/decode a String with a Variable-Length up to a fixed
    limit. Think of this length limit as the amount of buffer space allocated
    in the FSW.
    *Note:* Here the *args must be [max_encoded_length, string]
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
            len(args) == 2  # (max_length, data)
            and isinstance(args[0], int)  # max length
            and isinstance(args[1], (str, bytes))  # data
            # AT LEAST enough room for all the data:
            and len(args[1]) <= (args[0]-2)  # -2 to ignore the length bytes
        )

    @classmethod
    def encode(cls, format_string: str, args: Tuple[int, str]) -> bytes:
        """
        Encodes the given val as a String.
        where *args are [max_encoded_length, string]
        NOTE: Format string will contain the ACTUAL number of bytes to use for
        the encoding (and it will clip if more data is supplied).
        """
        max_enc_len = args[0]
        max_data_len = max_enc_len-2  # -2 to ignore the length bytes
        proc_core_data = cls.encode_str(args[1])
        proc_data_len = len(proc_core_data)

        valid = cls.check(format_string, (max_enc_len, proc_core_data))
        if not valid:
            raise PacketEncodingException(
                args,
                f"Data `{args}` not able to be encoded as a String. "
                f"Where data should contain [max_encoded_length, string]. "
                f"Processed string is: {proc_core_data!r} w/ "
                f"len={proc_data_len}B (max processed length: "
                f"{max_enc_len}-2B = {max_data_len}B)."
            )

        enc_out = struct.pack(format_string, proc_data_len, proc_core_data)

        # NOTE: Native Strings in FSW don't allow for 0x00 (NULL) anywhere
        # in the string since they use null termination for length checking.
        # If this is important to you, use `Fw::IrisCmdByteStringArg`
        # (IRISBYTESTRING in GSW) for transmitting arbitrary bytes sequences.
        # NOTE: It is, however, okay if this appears in the terminal position
        # (hence, why we're only checking up to but not including the last byte):
        # Also, first 2B are length so we skip those:
        if b'\0' in enc_out[2:-1]:
            raise PacketEncodingException(
                args,
                f"Data `{args}` not able to be encoded as a String, "
                f"since it encodes to: `{enc_out!r}` which contains b'\\0' "
                f"(a null byte) in a non-terminal position. FSW doesn't allow "
                f"this. "
                f"If this is important to you, use `Fw::IrisCmdByteStringArg` "
                f"(IRISBYTESTRING in GSW) for transmitting arbitrary bytes "
                f"sequences."
            )

        return enc_out

    @classmethod
    def decode(cls, format_string: str, buffer: bytes) -> Tuple[int, str]:
        """
        Decodes the given val as a String.
        """
        data_len, raw_data = struct.unpack(format_string, buffer)
        if data_len > len(raw_data):
            raise PacketDecodingException(
                buffer,
                f"The expected length of the string ({data_len}B) "
                f"is greater than the length of available raw data ({len(raw_data)}B)."
            )

        return (data_len+2, cls.decode_str(raw_data[:data_len]))


class BytesPacker(Codec[Tuple[int, bytes]]):
    """
    Defines how to en/decode a string of arbitrary raw bytes for FSW (i.e.
    impl. of `Fw::IrisCmdByteStringArg` from FSW).
    *Note:* Here the *args must be [max_length, bytes]
    """

    @classmethod
    def check(cls, format_string: str, args: Tuple[int, Union[str, bytes]]) -> bool:
        """
        Checks the if the given vals can be used to encode as a Byte String.
        where *args are [max_length, string]
        """
        return (
            len(args) == 2  # (max_length, data)
            and isinstance(args[0], int)  # max length
            and isinstance(args[1], bytes)  # data
            # AT LEAST enough room for all the data:
            and len(args[1]) <= (args[0]-2)  # -2 to ignore the length bytes
        )

    @classmethod
    def encode(cls, format_string: str, args: Tuple[int, bytes]) -> bytes:
        """
        Encodes the given val as a Byte String.
        where *args are [max_string_length, string].
        NOTE: Format string will contain the ACTUAL number of bytes to use for
        the encoding (and it will clip if more data is supplied).
        """
        max_enc_len = args[0]
        max_data_len = args[0]-2  # -2 to ignore the length bytes
        raw_data = args[1]
        raw_data_len = len(raw_data)

        valid = cls.check(format_string, args)
        if not valid:
            raise PacketEncodingException(
                args,
                f"Data `{args}` not able to be encoded as an IRISBYTESTRING. "
                f"Where data should contain [max_encoded_length, bytes]. "
                f"Raw data is: {raw_data!r} w/ len={raw_data_len}B (max raw "
                f"length: {max_enc_len}-2B = {max_data_len}B)."
            )

        enc_out = struct.pack(format_string, raw_data_len, raw_data)

        return enc_out

    @classmethod
    def decode(cls, format_string: str, buffer: bytes) -> Tuple[int, bytes]:
        """
        Decodes the given val as `bytes`.
        """
        data_len, raw_data = struct.unpack(format_string, buffer)
        if data_len > len(raw_data):
            raise PacketDecodingException(
                buffer,
                f"The expected length of the string ({data_len}B) "
                f"is greater than the length of available raw data ({len(raw_data)}B)."
            )

        # +2 b/c total size include 2 len bytes
        return (data_len+2, raw_data[:data_len])
