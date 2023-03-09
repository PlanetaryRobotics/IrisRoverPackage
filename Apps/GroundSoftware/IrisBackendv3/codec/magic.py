#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
This module supplies an Enum which defines all the latest 4B 'magic' identifiers 
used in the IRIS Common Packet Headers (CPH) to determine the type of a payload 
within the Variable Length Payload (VLP) section of the CPH.

**Note:** Proper standards are followed and **Enum values should never be 
deleted from Magic**, only marked as deprecated. Once a magic value is used, it 
should remain persistently and never be later removed or repurposed.

@author: Connor W. Colombo (CMU)
@last-updated: 09/13/2022
"""
# Activate postponed annotations (for using classes as return type in their own methods):
from __future__ import annotations
from typing import cast
from enum import Enum

from IrisBackendv3.codec.settings import ENDIANNESS_CODE

from IrisBackendv3.codec.logging import logger
from IrisBackendv3.codec.exceptions import PacketDecodingException

import struct

# Number of Bytes in each Magic (as a `struct` sym):
SIZE_SYM = 'L'  # 4B unsigned
MAGIC_SIZE = struct.calcsize(ENDIANNESS_CODE+SIZE_SYM)


class Magic(Enum):
    """
    Enum for Representing Iris Common Packet Header (CPH) Magic identifiers 
    used in the Variable Length Payload (VLP).
    """

    # *No Palindromes!*
    MISSING = 0xBEEFEEEE, 'Missing', True
    COMMAND = 0x00BADA55, 'Commands', False
    WATCHDOG_COMMAND = 0xC000FFEE, 'Commands Destined for Watchdog Hardware', False
    # 0xD09AD10 when encoded as little-endian:
    RADIO_COMMAND = 0x10AD09D0, 'Commands Destined for Radio\'s internal MCU', False
    TELEMETRY = 0xC00010FF, 'Telemetry', False
    EVENT = 0x0DEADBAD, 'Events (Logs)', False
    FILE = 0xDABAD000, 'Files (Images, UWB, etc.)', False

    # Instance attributes for type-checker:
    description: str
    # whether this magic has been deprecated (and should no longer be used intentionally):
    deprecated: bool

    def __new__(cls, val: int, desc: str, deprecated: bool):
        """Constructs a new instance of the Enum."""
        obj = object.__new__(cls)
        obj._value_ = val
        obj.description = desc
        obj.deprecated = deprecated
        return obj

    def encode(self, byte_order='<') -> bytes:
        """
        Returns an encoded interpretation of this Magic, using the supplied 
        `byte_order` symbol if given or `!` (network order) if not.
        """
        if self.deprecated:
            logger.warning(
                f"Encoding data using a deprecated Common Packet Header Magic: "
                f"{self.name} = `0x{self._value_:08X}`."
            )
        return struct.pack(byte_order+SIZE_SYM, self.value)

    @classmethod
    def decode(cls, data: bytes, byte_order='<') -> Magic:
        """
        Given a data chunk as a `bytes` object, this converts it to the 
        appropriate Magic is given.
        """
        try:
            contents = struct.unpack(byte_order+SIZE_SYM, data)
            unpack_error = False
        except struct.error:
            unpack_error = True

        if unpack_error or len(contents) != 1:
            raise PacketDecodingException(
                data,
                (
                    f"This set of magic bytes doesn't conform to the Magic "
                    f"standard. Magics should be decodeable using a "
                    f"'{byte_order+SIZE_SYM}' `struct` string. Are there too "
                    f"many/few bytes here?"
                )
            )

        try:
            # Type ignore note: mypy doesn't recognize that Enum() doesn't call __new__ but is instead an Enum indexing operation and, thus, doesn't need __new__'s arguments.
            magic = cls(contents[0])  # type: ignore
        except ValueError:
            raise PacketDecodingException(
                data,
                (
                    f"This set of magic bytes is correctly formatted but can't be "
                    f"mapped to any known Magic bytes. Check the list of known "
                    f"Magics in the C&TL or `magic.py`. Are the bytes backwards "
                    f"/ using the wrong endianness?"
                )
            )

        if magic.deprecated:
            logger.warning(
                f"Decoded data and found a deprecated Common Packet Header Magic: "
                f"{magic.name} = `0x{magic._value_:08X}`."
            )

        return magic
