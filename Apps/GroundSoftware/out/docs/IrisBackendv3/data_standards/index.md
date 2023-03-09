Module IrisBackendv3.data_standards
===================================
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

Sub-modules
-----------
* IrisBackendv3.data_standards.data_standards
* IrisBackendv3.data_standards.exceptions
* IrisBackendv3.data_standards.fsw_data_type
* IrisBackendv3.data_standards.gsw_metadata_tools
* IrisBackendv3.data_standards.logging
* IrisBackendv3.data_standards.module
* IrisBackendv3.data_standards.prebuilt

Functions
---------

    
`build(include_all_prebuilts: bool = True) ‑> IrisBackendv3.data_standards.data_standards.DataStandards`
:   Helper to build DataStandards, optionally including all prebuilt
    modules. Returns the DataStandards.

    
`build_and_cache(cache_dir: str = './out/DataStandardsCache', cache_name_base: str = 'IBv3_datastandards', include_ulid_suffix: bool = True, include_all_prebuilts: bool = True) ‑> str`
:   Builds the datastandards and caches them.
    Cache file is stored in `cache_dir` and uses `cache_name_base` as the base
    of the file name (a ULID suffix is added if `include_ulid_suffix` is left
    on).
    Optionally including all prebuilt modules.
    Returns absolute path to cache.