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
    TRANSCEIVER_PUB = 0xF00D  # Transceiver Proxy inbound port (for Pubs)
    TRANSCEIVER_SUB = 0xF00E  # Transceiver Proxy outbound port (for Subs)
    CODEC_PUB = 0xC0DE
    CODEC_SUB = 0xC0DF
    STORAGE_PUB = 0x5709
    STORAGE_SUB = 0x570A
    REMOTEDB_PUB = 0xDA7A
    REMOTEDB_SUB = 0xDA7B
    # CLI_WINDOW = 0xF001
    # TELEM_WINDOW = 0xFEED
    GUI_PUB = 0x15EE
    GUI_SUB = 0x15EF
    ATLAS_PUB = 0x06DE
    ATLAS_SUB = 0x06DF
