#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Various exceptions used throughout this package.

@author: Connor W. Colombo (CMU)
@last-updated: 01/18/2021
"""


class StandardsFormattingException(Exception):
    """Formatting of Some Standards File doesn't Conform to Expectations."""

    def __init__(self, file_uri: str, info: str = "") -> None:
        """Create a StandardsFormattingException.

        Args:
            file_uri (str): URI of the standards file containing the formatting exception.
            info (str, optional): Info about the specific formatting issue. Defaults to "".
        """
        self.file_uri = file_uri
        self.info = info
        self.message = f"Formatting of standards file '{file_uri}' does not conform to expectations: {info}."
        super().__init__(self.message)
