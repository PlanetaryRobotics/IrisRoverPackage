#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Various exceptions used throughout this package.

@author: Connor W. Colombo (CMU)
@last-updated: 05/10/2022
"""

from typing import Any
from IrisBackendv3.utils.basic import bytearray_to_spaced_hex as hexstr


class TransceiverConnectionException(Exception):
    """A `Transceiver` failed to connect to an uplink/downlink channel."""

    def __init__(self, info: str = "") -> None:
        """Create a TransceiverConnectionException.

        Args:
            info (str, optional): Info about the specific formatting issue. Defaults to "".
        """
        self.info = info
        self.message = f"Transceiver failed to connect to an uplink/downlink channel: {info}"
        super().__init__(self.message)


class TransceiverDecodingException(Exception):
    """An exception occurred while decoding a packet."""

    def __init__(self, data: bytes, info: str = "") -> None:
        """Create a TransceiverDecodingException.

        Args:
            data (bytes): Data which couldn't be decoded properly and caused the exception.
            info (str, optional): Info about the specific formatting issue. Defaults to "".
        """
        self.data = data
        self.info = info
        self.message = f"Transceiver unable to properly decode `0x {hexstr(data)}` because: {info}"
        super().__init__(self.message)


class TransceiverEncodingException(Exception):
    """An exception occurred while encoding a packet."""

    def __init__(self, data: Any, info: str = "") -> None:
        """Create a TransceiverEncodingException.

        Args:
            data (bytes): Data which couldn't be encoded properly and caused the exception.
            info (str, optional): Info about the specific formatting issue. Defaults to "".
        """
        self.data = data
        self.info = info
        self.message = f"Transceiver unable to properly encode `{data}` because: {info}"
        super().__init__(self.message)
