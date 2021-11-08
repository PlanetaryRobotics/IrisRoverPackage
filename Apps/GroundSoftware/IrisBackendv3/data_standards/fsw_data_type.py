#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Enum for Respresenting FPrime Datatypes as Python Struct Strings.

@author: Connor W. Colombo (CMU)
@last-updated: 04/16/2021
"""
# Activate postponed annotations (for using classes as return type in their own methods):
from __future__ import annotations
from typing import Optional, Tuple, Union
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
    STRING = 0x03


class FswDataType(Enum):
    """
    Enum for Respresenting Native FPrime Datatypes.

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
    # Per [FPrime docs](https://nasa.github.io/fprime/UsersGuide/api/python/fprime/html/modules/fprime/common/models/serialize/string_type.html)
    STRING10 = 'H10s', 'char[10]', Category.STRING, str
    STRING15 = 'H15s', 'char[15]', Category.STRING, str
    STRING24 = 'H24s', 'char[24]', Category.STRING, str
    STRING40 = 'H40s', 'char[40]', Category.STRING, str
    STRING50 = 'H50s', 'char[50]', Category.STRING, str
    STRING240 = 'H240s', 'char[240]', Category.STRING, str
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
        """Number of bits in the datatype."""
        return self.num_octets * 8

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