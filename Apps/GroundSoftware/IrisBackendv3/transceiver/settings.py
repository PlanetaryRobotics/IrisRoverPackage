#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""
Configurable settings for the IPC layer.

@author: Connor W. Colombo (CMU)
@last-updated: 07/03/2022
"""
import os
from typing import Final
_KEY_ADDR_BASE: Final[str] = 'IBv3-xcvr--'

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
