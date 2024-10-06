#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""
Configurable settings for the IPC layer.

@author: Connor W. Colombo (CMU)
@last-updated: 11/30/2023
"""
import os
from typing import Final

# Make sure environment variables file has been loaded:
from IrisBackendv3.environ import ensure_env_loaded
ensure_env_loaded()

_KEY_ADDR_BASE: Final[str] = 'IBv3_ipc__'

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
    if (k := _KEY_ADDR_BASE+key) in os.environ:
        settings[key] = os.getenv(k) or settings[key]


def set_ip(ip: str):
    """
    Sets the IP for all IPC.
    """
    settings['IP'] = ip
