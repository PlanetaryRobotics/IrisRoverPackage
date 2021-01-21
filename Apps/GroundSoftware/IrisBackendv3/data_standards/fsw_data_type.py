#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Enum for Respresenting FPrime Datatypes as Python Struct Strings.

@author: Connor W. Colombo (CMU)
@last-updated: 01/18/2021
"""
# Activate postponed annotations (for using classes as return type in their own methods):
from __future__ import annotations
from typing import cast
from enum import Enum


class FswDataType(Enum):
    """
    Enum for Respresenting Native FPrime Datatypes.

    Each instance contains two auxiliary data fields: `struct_sym` and `type_str`.
    `struct_sym` is the symbol used for representing this datatype when using
    `struct.pack` and `type_str` is the standard embedded notation of writing
    this data type (sans the `_t` at the end).

    The latter of these fields (`type_str`) is used as the default `_value_` for
    each instance. Thus, `FswDataType('uint8') == FswDataType['U8']`. This makes
    fetching the right instance easier since one can go from either useful
    direction (FPrime unit type or standard type name).
    """

    BOOL = '?', 'bool'
    I8 = 'b', 'int8'
    I16 = 'h', 'int8'
    I32 = 'l', 'int32'
    I64 = 'q', 'int64'
    U8 = 'B', 'uint8'
    U16 = 'H', 'uint16'
    U32 = 'L', 'uint32'
    U64 = 'Q', 'uint64'
    F32 = 'f', 'float'  # IEEE 754 binary32 float
    F64 = 'd', 'double'  # IEEE 754 binary64 double
    # FPrime enums all map to an int (default `int` type is `int32_t`):
    ENUM = 'l', 'enum/*int32*/'

    def __new__(cls, struct_sym, type_str):
        obj = object.__new__(cls)
        obj._value_ = type_str
        obj.struct_sym = struct_sym
        obj.type_str = type_str
        return obj
