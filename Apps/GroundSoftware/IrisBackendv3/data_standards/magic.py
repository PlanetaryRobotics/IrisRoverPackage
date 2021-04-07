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
@last-updated: 01/31/2021
"""
# Activate postponed annotations (for using classes as return type in their own methods):
from __future__ import annotations
from typing import cast
from enum import Enum


class Magic(Enum):
    """
    Enum for Respresenting Iris Common Packet Header (CPH) Magic identifiers 
    used in the Variable Length Payload (VLP).
    """

    COMMAND = 0x00BADA55, 'Commands', False
    WATCHDOG_COMMAND = 0xC000FFEE, 'Commands Destined for Watchdog Hardware', False
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
