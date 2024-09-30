# -*- coding: utf-8 -*-
"""
This package handles all MetaModules, computed datastandards modules.
Has to be its own layer b/c it's dependent on both the `codec` and
`data_standards` modules (avoiding circ. imports).

@author: Connor W. Colombo (CMU)
@last-updated: 01/10/2024
"""
import os

from IrisBackendv3.data_standards import (
    build as build_base_standards,
    DataStandards,
    _CACHE_DIR, _FILENAME_BASE_DEFAULT
)
from IrisBackendv3.data_standards.logs import logger as _logger

from IrisBackendv3.meta.metafield import (
    add_metamodules_to_standards as _add_metamodules_to_standards
)

from config.metafields import ALL_META_MODULES as _ALL_META_MODULES

# Helper to build and manage datastandards that incl. meta_modules


def build_standards_with_metamodules(
    include_all_prebuilts: bool = True,
    include_all_metamodules: bool = True
) -> DataStandards:
    """Helper to build DataStandards, optionally including all prebuilt
    modules and/or metamodules. Returns the DataStandards."""
    standards = build_base_standards(
        include_all_prebuilts=include_all_prebuilts)
    if include_all_metamodules:
        _logger.notice(
            f'Adding {len(_ALL_META_MODULES)} MetaModules . . .'
        )
        _add_metamodules_to_standards(standards, _ALL_META_MODULES)
    return standards


def build_standards_with_metamodules_and_cache(
    cache_dir: str = _CACHE_DIR,
    cache_name_base: str = _FILENAME_BASE_DEFAULT,
    include_ulid_suffix: bool = True,
    include_all_prebuilts: bool = True,
    include_all_metamodules: bool = True
) -> str:
    """Builds the datastandards with MetaModules included and caches them.
    Cache file is stored in `cache_dir` and uses `cache_name_base` as the base
    of the file name (a ULID suffix is added if `include_ulid_suffix` is left
    on).
    Optionally including all prebuilt modules.
    Returns absolute path to cache."""
    _logger.notice('Building DataStandards w/ MetaModules . . .')
    standards = build_standards_with_metamodules(
        include_all_prebuilts=include_all_prebuilts,
        include_all_metamodules=include_all_metamodules
    )

    _logger.notice('Caching DataStandards w/ MetaModules . . .')
    filename = standards.cache(
        cache_dir=cache_dir,
        filename_base=cache_name_base,
        include_ulid_suffix=include_ulid_suffix
    )

    filepath = os.path.join(cache_dir, filename)
    filepath = os.path.abspath(filepath)
    _logger.success(f"DataStandards w/MetaModules Cached to `{filepath}`.")
    return filepath
