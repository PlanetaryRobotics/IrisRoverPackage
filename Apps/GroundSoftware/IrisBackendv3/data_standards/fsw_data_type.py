#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Enum for Representing FPrime Datatypes as Python Struct Strings.

@author: Connor W. Colombo (CMU)
@last-updated: 11/12/2022
"""
# Activate postponed annotations (for using classes as return type in their own methods):
from __future__ import annotations
from typing import Optional, Tuple, Union, cast
from enum import Enum
import struct

#! TODO: Add unittests for this type now that it's not just a basic enum


class Category(Enum):
    """
    Various categories for serialization of FSW Types, which define how they
    will be en/decoded in the codec layer. Each category has its own way of
    serializing values.
    """
    EMPTY = -1
    NUMBER = 0x00
    BOOLEAN = 0x01
    ENUM = 0x02
    STRING = 0x03  # Fixed-length string
    VARSTRING = 0x04  # Variable length string
    # Special Iris Serializable Type for uplinking arbitrary byte sequences
    # (made forBGAPI Passthrough):
    # Equivalent to custom `Fw::IrisCmdByteStringArg` in FSW.
    IRISBYTESTRING = 0x05


class FswDataType(Enum):
    """
    Enum for Representing Native FPrime Datatypes.

    Each instance contains four explicit auxiliary data fields: `struct_sym`,
    `type_str`, and `category`.
    - `struct_sym` is the symbol used for representing this datatype when using
    `struct.pack`
    - `type_str` is the standard embedded (C-style) notation of writing this
    data type (sans the `_t` at the end).
    - `category` is the FSW serialization category which is used to determine
    how to en/decode a value with this type.
    -  `python_type` is the corresponding type or tuple of types in python (eg. 
    `str`, `(int,str)`) which a value must have in order to be able to be 
    accurately encoded as this FswDataType.

    `type_str` is used as the default `_value_` for each instance. Thus,
    `FswDataType('uint8') == FswDataType['U8']`. This makes fetching the right
    instance easier since one can go from either useful direction (FPrime unit
    type or standard type name).

    Additionally, there's one optional field `num_octets` which gives the number
    of 8b bytes (octets) in this type's size. This is normally straightforward
    and can be computed using `struct.calcsize(struct_sym)` but if for some
    reason it's more complex or needs to be explicit, this field can be
    specified too.
    """

    # NOTE: As a standard, all member names should be UPPERCASE.
    # NOTE: As a standard, C-style type strings should omit any '_t'
    BOOL = 'B', 'bool', Category.BOOLEAN, bool  # packed as 0xFF=True, 00=False
    BOOLEAN = 'B', 'bool', Category.BOOLEAN, bool  # alias
    I8 = 'b', 'int8', Category.NUMBER, int
    I16 = 'h', 'int16', Category.NUMBER, int
    I32 = 'l', 'int32', Category.NUMBER, int
    I64 = 'q', 'int64', Category.NUMBER, int
    U8 = 'B', 'uint8', Category.NUMBER, int
    U16 = 'H', 'uint16', Category.NUMBER, int
    U32 = 'L', 'uint32', Category.NUMBER, int
    U64 = 'Q', 'uint64', Category.NUMBER, int
    F32 = 'f', 'float', Category.NUMBER, float  # IEEE 754 binary32 float
    F64 = 'd', 'double', Category.NUMBER, float  # IEEE 754 binary64 double
    # FPrime enums all map to an int (default `int` type is `int32_t`):
    ENUM = 'l', 'enum/*int32*/', Category.ENUM, (str, int)
    # Fixed Length Strings (only expected / pre-approved sizes allowed):
    # This is a halfword (2B, as ">H") indicating length followed by a (utf-8) encoded char[]
    # Per [FPrime docs](https://nasa.github.io/fprime/v1.5/UsersGuide/api/python/fprime/html/modules/fprime/common/models/serialize/string_type.html)
    STRING5 = 'H5s', 'char[5]', Category.STRING, str
    STRING6 = 'H6s', 'char[6]', Category.STRING, str
    STRING8 = 'H8s', 'char[8]', Category.STRING, str
    STRING10 = 'H10s', 'char[10]', Category.STRING, str
    STRING15 = 'H15s', 'char[15]', Category.STRING, str
    STRING24 = 'H24s', 'char[24]', Category.STRING, str
    STRING39 = 'H39s', 'char[39]', Category.STRING, str
    STRING40 = 'H40s', 'char[40]', Category.STRING, str
    STRING50 = 'H50s', 'char[50]', Category.STRING, str
    STRING240 = 'H240s', 'char[240]', Category.STRING, str
    # Variable length string with max length of 255:
    VARSTRING_255 = 'H255s', 'char[/*up to*/255]', Category.VARSTRING, str

    # Special Iris Serializable Type for uplinking arbitrary byte sequences
    # (made forBGAPI Passthrough):
    # Equivalent to custom `Fw::IrisCmdByteStringArg` in FSW.
    #
    # NOTE: Size here is just the max. Actual encoded/decoded size is
    # variable and will be determined based on the data):
    # NOTE: Size must always be << `FW_COM_BUFFER_MAX_SIZE` in
    # `fprime/Fw/Cfg/Config.hpp`).
    IRISBYTESTRING134 = 'H134s', 'char[/*up to*/134]', Category.IRISBYTESTRING, bytes

    # Invalid / Unsupported Type:
    INVALID = '', 'invalid', Category.EMPTY, type(None), 0

    # Instance attributes for type-checker:
    struct_sym: str
    type_str: str
    category: Category
    num_octets: int
    python_type: Union[type, Tuple[type]]

    @property
    def num_bits(self) -> int:
        """Number of bits in the datatype (or max number in the case of a
        variable-length string)."""
        return self.num_octets * 8

    def get_actual_num_bytes(self, data: Optional[bytes] = None) -> int:
        """Returns the actual number of bytes in this datatype.
        For most types, this is a fixed length determined just by the type;
        however, for a variable-length string it's given by the first two bytes
        in the data.

        @param data: The data that has this type (only needed for variable
        length strings).
        """
        if self.category not in [Category.VARSTRING, Category.STRING, Category.IRISBYTESTRING]:
            return self.num_octets
        else:
            # FPrime packs all strings (variable length or not) as just the
            # data given (so a STRING50 that only uses 10B will only be sent as
            # 10B with a length of 0x000A).
            # Note: `Fw::IrisCmdByteStringArg` is derived from strings in FPrime.
            # Serialization is the same, it just also allows 0x00 anywhere in its contents.

            # Make sure data was given:
            if data is None:
                raise ValueError(
                    "Attempting to determine true length of a variable-length "
                    "string but no data was given, which is necessary for "
                    "determining its length."
                )
            # Make sure at least two bytes are present for the length:
            if len(data) < 2:
                # Not enough bytes left.
                raise Exception(
                    f"Attempting to decode a variable-length string but there "
                    f"aren't even two bytes in the given data, which would be "
                    f"needed to decode the length of the variable-length "
                    f"string."
                )
            (str_len,) = struct.unpack('>H', data[:2])
            # Make sure number of bytes indicated is not greater than the
            # max-length for the type:
            if str_len > self.num_octets:
                raise Exception(
                    f"Attempting to decode a variable-length string but "
                    f"the length indicated (`{str_len}`) is greater than the "
                    f"max length for this variable-length string "
                    f"(`{self.num_octets}`). Data given: {data!r}."
                )
            # Make sure there's enough data left in the given data stream:
            if str_len > (len(data)-2):
                raise Exception(
                    f"Attempting to decode a variable-length string but "
                    f"the length indicated (`{str_len}`) is greater than the "
                    f"number of bytes left in the available data stream "
                    f"(`{len(data)-2}`). Data given: {data!r}."
                )
            # +2 for the two length bytes, which are part of the type:
            return cast(int, str_len) + 2

    def __new__(cls, struct_sym: str, type_str: str, cat: Category, python_type: type, num_octets: Optional[int] = None):
        """Constructs a new instance of the Enum."""
        obj = object.__new__(cls)
        obj._value_ = type_str
        obj.type_str = type_str
        obj.category = cat
        obj.python_type = python_type

        # Make sure struct_sym doesn't contain any byte-order characters:
        byte_order_chars = ['@', '=', '<', '>', '!']
        BOCs = [c for c in byte_order_chars if c in struct_sym]
        if len(BOCs) > 0:
            raise ValueError(
                f"The `struct_sym` for `FswDataType` with given "
                f"`type_str`=\"{type_str}\" has the following byte order "
                f"characters (BOCs) in it: {BOCs}. These BOCs must be removed "
                f"so they don't conflict with BOCs where these types are used."
            )

        obj.struct_sym = struct_sym

        # Calculate the number of octets indicated by the `struct_sym`
        # (this is a fast way to check if `struct_sym` formatting correct and
        # result might be used later if `num_octets` not given.)
        # add network byte order ('!') to prevent any native alignment issues:
        struct_octets = struct.calcsize('!' + obj.struct_sym)

        if num_octets is None:
            obj.num_octets = struct_octets
            #! TODO: add a unit test for this which makes sure all calculated types have the expected size.
        else:
            obj.num_octets = num_octets

        return obj

    @classmethod
    def is_valid_FPrime_name(cls, name: str) -> bool:
        """
        Tests if the given name is a valid FswDataType name(FPrime type name).
        If it is, you can get the associated enum instance with `FswDataType[name]`.
        """
        return name in {t.name for t in FswDataType}

    @classmethod
    def is_valid_C_name(cls, name: str) -> bool:
        """
        Tests if the given name is a valid FswDataType value(C-style type name).
        If it is, you can get the associated enum instance with `FswDataType(name)`.
        """
        return name in {t.value for t in FswDataType}

    @classmethod
    def get(cls, name: str) -> FswDataType:
        """
        Gets the `FswDataType` enum member which matches to the given name if 
        one exists. Matching is done by:
        - Enum member name (the FPrime name), once UPPERCASED.
        - Enum member value (the C-style embedded name), once any '_t' are removed.

        Returns:
            FswDataType: Matched member.

        Raises:
            KeyError: if the name is not valid / doesn't match to any members.
        """
        if isinstance(name, str):
            if '_t' in name:
                # ? TODO: why is this `if` here? Seems like this will ONLY accept names with '_t' in it rather than ALSO if '_t' is in there...
                # ? Check which is desired behavior and unit test this.
                # All `type_str`s omit the '_t' BUT we should accept one if it's given.
                stripped_down = name.replace('_t', '')
                if cls.is_valid_C_name(stripped_down):
                    # Type ignore note: mypy doesn't recognize that Enum() doesn't call __new__ but is instead an Enum indexing operation and, thus, doesn't need __new__'s arguments.
                    return cls(stripped_down)  # type: ignore

            # support one method of indexing (so we can grab by name as well):
            elif cls.is_valid_FPrime_name(name.upper()):
                return cls[name.upper()]  # all names are UPPER by standard

        raise KeyError(name)
