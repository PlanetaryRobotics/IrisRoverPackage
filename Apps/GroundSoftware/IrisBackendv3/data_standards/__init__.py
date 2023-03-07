# -*- coding: utf-8 -*-
"""
This package interprets the definitions for all of the Data Standards defined in 
the Command & Telemetry List (C&TL). Essentially, this package contains various 
containers which describe the *specifications* (herein referred to as 
'data standards') for how various pieces of data (Commands, Telemetry, etc.) are 
structured. These specifications are then populated using the building tools in 
`data_standards.py` which either parses FPrime XML (which *should* be built 
according to C&TL specifications) or various supporting pieces of Flight 
Software (FSW) definition code (eg. cpp headers). In addition, there are parsing 
tools which support finding discrepancies between the FSW FPrime and the C&TL.

The FSW is treated as the source of truth (rather than the C&TL or GSW) so that 
the FSW team can have the freedom to readily change their standards with the 
assurance that GSW will be able to accommodate immediately. This also allows GSW 
to save compact `data_standards` caches alongside data logs from the rover to 
ensure the formatting of logged data streams is always known.

@author: Connor W. Colombo (CMU)
@last-updated: 03/01/2023
"""

# Expose internal modules (allow them to be accessed directly):
from .data_standards import *

from .fsw_data_type import *

from .exceptions import *

from IrisBackendv3.data_standards import logging

# Private imports for use by helper functions:
from .logging import logger as _logger
from .prebuilt import (
    add_to_standards as _add_to_standards,
    ALL_PREBUILT_MODULES as _ALL_PREBUILT_MODULES
)
from .data_standards import _CACHE_DIR, _FILENAME_BASE_DEFAULT


# Helper to build and manage datastandards:
def build(include_all_prebuilts: bool = True) -> DataStandards:
    """Helper to build DataStandards, optionally including all prebuilt
    modules. Returns the DataStandards."""
    standards = DataStandards.build_standards()
    if include_all_prebuilts:
        _logger.notice(
            f'Adding {len(_ALL_PREBUILT_MODULES)} Prebuilt Modules . . .'
        )
        _add_to_standards(standards, _ALL_PREBUILT_MODULES)
    return standards


def build_and_cache(
    cache_dir: str = _CACHE_DIR,
    cache_name_base: str = _FILENAME_BASE_DEFAULT,
    include_ulid_suffix: bool = True,
    include_all_prebuilts: bool = True
) -> str:
    """Builds the datastandards and caches them.
    Cache file is stored in `cache_dir` and uses `cache_name_base` as the base
    of the file name (a ULID suffix is added if `include_ulid_suffix` is left
    on).
    Optionally including all prebuilt modules.
    Returns absolute path to cache."""
    _logger.notice('Building DataStandards . . .')
    standards = build(include_all_prebuilts)

    _logger.notice('Caching DataStandards . . .')
    filename = standards.cache(
        cache_dir=cache_dir,
        filename_base=cache_name_base,
        include_ulid_suffix=include_ulid_suffix
    )

    filepath = os.path.join(cache_dir, filename)
    filepath = os.path.abspath(filepath)
    _logger.success(f"DataStandards Cached to `{filepath}`.")
    return filepath
