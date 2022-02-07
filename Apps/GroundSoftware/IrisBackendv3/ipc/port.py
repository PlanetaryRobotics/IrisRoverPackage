#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
This module supplies an Enum which defines all ports used in the IPC.

@author: Connor W. Colombo (CMU)
@last-updated: 05/17/2021
"""
# Activate postponed annotations (for using classes as return type in their own methods):
from __future__ import annotations
from enum import Enum


class Port(Enum):
    """
    Enum for defines all ports used in the IPC.

    Try to make sure these ports aren't occupied on `localhost` the hosting 
    device but feel free to change them here if needed. This enum should serve 
    as the single source of truth for the port numbers.
    """

    # (Remember: max port number is 0xFF49).
    TRANSCEIVER = 0xF00D
    CODEC = 0xC0DE
    STORAGE = 0x5709
    REMOTEDB = 0xDA7A
    CLI_WINDOW = 0xF001
    TELEM_WINDOW = 0xFEED
    GUI = 0x15EE
    ATLAS = 0x06DE
