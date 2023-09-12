"""Useful helper functions when interacting with the IrisBackendv3 GDS as a
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
"""
# Import `restricted_pickler` before **ANYTHING ELSE** so it has first dibs on
# how the modules it cares about are imported:
import IrisBackendv3.ipc.restricted_pickler

# Import all top-level modules so they're dot-accessible if just
# `IrisBackendv3` is imported:
from IrisBackendv3 import codec
from IrisBackendv3 import data_standards
from IrisBackendv3 import ipc
from IrisBackendv3 import transceiver
from IrisBackendv3 import logs
from IrisBackendv3 import utils
from IrisBackendv3 import config


# Private imports for use by helper functions:
from IrisBackendv3.codec.settings import get_codec_standards as _get_codec_standards
from IrisBackendv3.codec.settings import set_codec_standards as _set_codec_standards
from IrisBackendv3.data_standards import DataStandards as _DataStandards
from IrisBackendv3.data_standards.data_standards import (
    _CACHE_DIR as _DS_CACHE_DIR,
    _FILENAME_BASE_DEFAULT as _DS_FILENAME_BASE_DEFAULT
)


# Helper functions for managing the lifecycle of the backend:
def init_from_standards(standards: _DataStandards) -> None:
    """Initializes the backend GDS using the given `DataStandards` object."""
    _set_codec_standards(standards)


def init_from_cache(path_to_cache: str) -> None:
    """Initializes the backend GDS using the given `DataStandards` cache stored
    at the given location."""
    standards = _DataStandards.load_cache(
        cache_dir='',
        cache_filename=path_to_cache
    )
    init_from_standards(standards)


def init_from_latest(
    cache_dir: str = _DS_CACHE_DIR,
    cache_name_base: str = _DS_FILENAME_BASE_DEFAULT
) -> None:
    """Initializes the backend GDS using the most recent cache (based on ULID)
    from the given cache directory with the given base name (see docs of
    `data_standards.DataStandards.load_cache` for more details).
    For simplest use, just leave these fields blank and the default cache
    directory and filename base will be used.
    NOTE: `data_standards.build_and_cache()` should have been run at least
    once using these settings in order for there to be a DataStandards cache to
    load.
    """
    standards = _DataStandards.load_cache(
        cache_dir=cache_dir,
        filename_base=cache_name_base
    )
    init_from_standards(standards)


def get_codec_standards() -> _DataStandards:
    """Returns the DataStandards currently in use by the GDS' codec (the
    subsystem that actually uses the standards)."""
    return _get_codec_standards()
