#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""
Configurable settings for the Iris Downlink Processor.

@author: Connor W. Colombo (CMU)
@last-updated: 12/27/2023
"""
import os
from typing import Final, TypedDict

# Make sure environment variables file has been loaded:
from IrisBackendv3.environ import ensure_env_loaded
ensure_env_loaded()

_KEY_ADDR_BASE: Final[str] = 'IBv3_app__dl_processor__'


class DownlinkProcessorSettings(TypedDict):
    INIT_EST_DL_DELAY_MS: float


# Base defaults:
settings: DownlinkProcessorSettings = {
    # Initial estimate for the *full-pipeline* downlink delay:
    # Don't add in delay est. by default b/c all of mission 1 was conducted at
    # a variable distance (cislunar orbit):
    'INIT_EST_DL_DELAY_MS': 0  # (4/2 + 40/2)*1000 + 100
}

# Override settings with OS Environment variables if applicable:
for key in settings.keys():
    if (k := _KEY_ADDR_BASE+key) in os.environ:
        settings[key] = (  # type: ignore[literal-required]
            os.getenv(k) or settings[key]  # type: ignore[literal-required]
        )

# Post-process to enforce type-adherence:
settings['INIT_EST_DL_DELAY_MS'] = float(settings['INIT_EST_DL_DELAY_MS'])
