"""
Controls Environment Variables for the Application.

Placed at root level so it can be called directly from `__init__` without
invoking subpackage `__init__`s (which can lead to a circular import chain).

To allow free importing anywhere (incl. before the restricted pickler imports
in `__init__`), **THIS MUST NOT IMPORT ANY OTHER MODULES FROM `IrisBackendv3`**

@author: Connor W. Colombo (CMU)
@last-updated: 11/30/2023
"""
from dotenv import load_dotenv
import os.path

_ENV_LOADED: bool = False


def ensure_env_loaded(force_load: bool = False):
    """Loads the env file if it hasn't been loaded already in this process.
    (Re)loads it no matter what if `force_load` is `True`.
    """
    global _ENV_LOADED
    if not _ENV_LOADED or force_load:
        # Make sure env file is loaded:
        load_dotenv(
            os.path.join(os.path.dirname(__file__), '../', '.env'),
            verbose=True,  # True = warns if missing
            override=False,  # False = *don't* override values already there
            interpolate=True  # interpolate (expand variables) if necessary
        )
        _ENV_LOADED = True


# Run once on module load to ensure env gets loaded immediately:
ensure_env_loaded()
