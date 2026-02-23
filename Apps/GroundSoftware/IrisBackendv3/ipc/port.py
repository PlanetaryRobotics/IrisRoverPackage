#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
This module supplies an Enum which defines all ports used in the IPC.

Actual port numbers are defined in `ports.env`.

@author: Connor W. Colombo (CMU)
@last-updated: 11/30/2023
"""
# Activate postponed annotations (for using classes as return type in their own methods):
from __future__ import annotations
from dotenv import load_dotenv
import os.path
from typing import Final
import os
from enum import Enum, auto

# Make sure environment variables file has been loaded:
from IrisBackendv3.environ import ensure_env_loaded
ensure_env_loaded()

_ENV_VAR_PREFIX: Final[str] = 'IBv3_ipc__PORT__'


class _EnumFromEnv(Enum):
    # Enum that pulls values from `os.env` when set to auto.

    @staticmethod
    def _generate_next_value_(name, start, count, last_values):
        # Pull value from env:
        env_val = os.getenv(_ENV_VAR_PREFIX + name)

        # Make sure any comments from the env file have been stripped in case
        # they made it through:
        if isinstance(env_val, str):
            env_val = env_val.split('#')[0].strip()

        if env_val is None:
            raise ValueError(
                f"Can't build IPC Port Enum. "
                f"Port for `{name}` is not defined as an env variable. "
                f"It should be defined in `.env` "
                f"as `{_ENV_VAR_PREFIX + name}`."
            )
        return int(env_val)


class Port(_EnumFromEnv):
    """
    Enum for defines all ports used in the IPC.

    Try to make sure these ports aren't occupied on `localhost` the hosting 
    device but feel free to change them here if needed. This enum should serve 
    as the single source of truth for the port numbers.
    """

    # Transceiver Proxy inbound port (for Pubs)
    TRANSCEIVER_PUB = auto()
    # Transceiver Proxy outbound port (for Subs)
    TRANSCEIVER_SUB = auto()
    CODEC_PUB = auto()
    CODEC_SUB = auto()
    STORAGE_PUB = auto()
    STORAGE_SUB = auto()
    REMOTEDB_PUB = auto()
    REMOTEDB_SUB = auto()
    GUI_PUB = auto()
    GUI_SUB = auto()
    ATLAS_PUB = auto()
    ATLAS_SUB = auto()
