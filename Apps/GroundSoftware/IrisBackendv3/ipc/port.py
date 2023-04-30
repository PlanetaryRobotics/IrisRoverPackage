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
    TRANSCEIVER = 0xF00D  # legacy, use _DL and _UL in main apps now
    TRANSCEIVER_PUB = 0xF00B  # Transceiver Proxy inbound port (for Pubs)
    TRANSCEIVER_SUB = 0xF00C  # Transceiver Proxy outbound port (for Subs)
    TRANSCEIVER_DL = 0xF00E
    TRANSCEIVER_UL = 0xF00F
    CODEC = 0xC0DE
    STORAGE = 0x5709
    REMOTEDB = 0xDA7A
    CLI_WINDOW = 0xF001
    TELEM_WINDOW = 0xFEED
    GUI = 0x15EE
    ATLAS = 0x06DE
