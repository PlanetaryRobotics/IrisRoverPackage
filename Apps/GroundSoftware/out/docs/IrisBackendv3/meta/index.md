Module IrisBackendv3.meta
=========================
This package handles all MetaModules, computed datastandards modules.
Has to be its own layer b/c it's dependent on both the `codec` and
`data_standards` modules (avoiding circ. imports).

@author: Connor W. Colombo (CMU)
@last-updated: 01/10/2024

Sub-modules
-----------
* IrisBackendv3.meta.metafield

Functions
---------

    
`build_standards_with_metamodules(include_all_prebuilts: bool = True, include_all_metamodules: bool = True) ‑> IrisBackendv3.data_standards.data_standards.DataStandards`
:   Helper to build DataStandards, optionally including all prebuilt
    modules and/or metamodules. Returns the DataStandards.

    
`build_standards_with_metamodules_and_cache(cache_dir: str = './out/DataStandardsCache', cache_name_base: str = 'IBv3_datastandards', include_ulid_suffix: bool = True, include_all_prebuilts: bool = True, include_all_metamodules: bool = True) ‑> str`
:   Builds the datastandards with MetaModules included and caches them.
    Cache file is stored in `cache_dir` and uses `cache_name_base` as the base
    of the file name (a ULID suffix is added if `include_ulid_suffix` is left
    on).
    Optionally including all prebuilt modules.
    Returns absolute path to cache.