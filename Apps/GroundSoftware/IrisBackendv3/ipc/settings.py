#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""
Configurable settings for the IPC layer.

@author: Connor W. Colombo (CMU)
@last-updated: 02/27/2022
"""
import os
from typing import Final
_KEY_ADDR_BASE: Final[str] = 'IBv3-ipc--'

# Base defaults:
settings = {
    'IP': '127.0.0.1',  # localhost
    # Key used to sign all pickled IPC messages.
    # All process running locally should be given the same session key.
    'SESSION_KEY': 'test-session-key',
    # App Name used in Logs:
    'app_name': 'IpcApp'
}

# Override settings with OS Environment variables if applicable:
for key in settings.keys():
    if _KEY_ADDR_BASE+key in os.environ:
        settings[key] = os.getenv(_KEY_ADDR_BASE+key)


def set_ip(ip: str):
    """
    Sets the IP for all IPC.
    """
    settings['IP'] = ip
