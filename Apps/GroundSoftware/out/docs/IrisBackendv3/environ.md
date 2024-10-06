Module IrisBackendv3.environ
============================
Controls Environment Variables for the Application.

Placed at root level so it can be called directly from `__init__` without
invoking subpackage `__init__`s (which can lead to a circular import chain).

To allow free importing anywhere (incl. before the restricted pickler imports
in `__init__`), **THIS MUST NOT IMPORT ANY OTHER MODULES FROM `IrisBackendv3`**

@author: Connor W. Colombo (CMU)
@last-updated: 11/30/2023

Functions
---------

    
`ensure_env_loaded(force_load: bool = False)`
:   Loads the env file if it hasn't been loaded already in this process.
    (Re)loads it no matter what if `force_load` is `True`.