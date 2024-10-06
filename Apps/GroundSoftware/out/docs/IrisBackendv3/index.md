Module IrisBackendv3
====================
Useful helper functions when interacting with the IrisBackendv3 GDS as a
package.
Mostly this makes really important functions accessible directly from the
top-level of the package.

Most basic use-case for a simple app.

At one point, someone runs:
```
import IrisBackendv3 as IB3
IB3.data_standards.build_and_cache()
```

Then, to get started later, all that has to be run is:
```
import IrisBackendv3 as IB3
IB3.init_from_latest()
```
If the standards source gets updated, just re-run
`IB3.data_standards.build_and_cache()`.

Sub-modules
-----------
* IrisBackendv3.codec
* IrisBackendv3.config
* IrisBackendv3.data_standards
* IrisBackendv3.environ
* IrisBackendv3.ipc
* IrisBackendv3.logs
* IrisBackendv3.main
* IrisBackendv3.meta
* IrisBackendv3.storage
* IrisBackendv3.transceiver
* IrisBackendv3.utils

Functions
---------

    
`get_codec_standards() ‑> IrisBackendv3.data_standards.data_standards.DataStandards`
:   Returns the DataStandards currently in use by the GDS' codec (the
    subsystem that actually uses the standards).

    
`init_from_cache(path_to_cache: str) ‑> None`
:   Initializes the backend GDS using the given `DataStandards` cache stored
    at the given location.

    
`init_from_latest(cache_dir: str = './out/DataStandardsCache', cache_name_base: str = 'IBv3_datastandards') ‑> None`
:   Initializes the backend GDS using the most recent cache (based on ULID)
    from the given cache directory with the given base name (see docs of
    `data_standards.DataStandards.load_cache` for more details).
    For simplest use, just leave these fields blank and the default cache
    directory and filename base will be used.
    NOTE: `data_standards.build_and_cache()` should have been run at least
    once using these settings in order for there to be a DataStandards cache to
    load.

    
`init_from_standards(standards: IrisBackendv3.data_standards.data_standards.DataStandards) ‑> None`
:   Initializes the backend GDS using the given `DataStandards` object.