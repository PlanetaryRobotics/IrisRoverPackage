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

_KEY_ADDR_BASE: Final[str] = 'IBv3_xcvr__'

# Base defaults:
settings = {
    'yamcs_username': 'iris',
    'yamcs_password': 'notapassword'  # set the creds during each session.
}

# Override settings with OS Environment variables if applicable:
for key in settings.keys():
    if _KEY_ADDR_BASE+key in os.environ:
        settings[key] = os.getenv(_KEY_ADDR_BASE+key)


def set_yamcs_credentials(user, password):
    """
    Sets the YAMCS login credentials for this session.
    """
    settings['yamcs_username'] = user
    settings['yamcs_password'] = password
