#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""
Configurable settings for Codec layer.

@author: Connor W. Colombo (CMU)
@last-updated: 04/17/2020
"""

from IrisBackendv3.data_standards import DataStandards, EMPTY_DATASTANDARD

ENDIANNESS_CODE = "<"  # little-endian by default

settings = {
    'STANDARDS': EMPTY_DATASTANDARD
}


def set_codec_standards(standards: DataStandards):
    """
    Sets the DataStandards to use for all encoding / decoding (codec) operations.
    """
    settings["STANDARDS"] = standards

# *TODO: Make all these^ settable via cli args (opts?)
