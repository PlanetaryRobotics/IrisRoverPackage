#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Various exceptions used throughout this package.

@author: Connor W. Colombo (CMU)
@last-updated: 04/09/2021
"""

from typing import Any

from IrisBackendv3.utils.basic import bytearray_to_spaced_hex as hexstr


class PacketDecodingException(Exception):
    """An exception occurred while decoding a packet."""

    def __init__(self, data: bytes, info: str = "") -> None:
        """Create a PacketDecodingException.

        Args:
            data (bytes): Data which couldn't be decoded properly and caused the exception.
            info (str, optional): Info about the specific formatting issue. Defaults to "".
        """
        self.data = data
        self.info = info
        self.message = f"Unable to properly decode `0x{hexstr(data)}` because: {info}"
        super().__init__(self.message)


class PacketEncodingException(Exception):
    """An exception occurred while encoding a packet."""

    def __init__(self, data: Any, info: str = "") -> None:
        """Create a PacketEncodingException.

        Args:
            data (bytes): Data which couldn't be encoded properly and caused the exception.
            info (str, optional): Info about the specific formatting issue. Defaults to "".
        """
        self.data = data
        self.info = info
        self.message = f"Unable to properly encode `{data}` because: {info}"
        super().__init__(self.message)
