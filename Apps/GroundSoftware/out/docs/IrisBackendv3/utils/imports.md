Module IrisBackendv3.utils.imports
==================================
A Collection of utility functions for handling imports:

@author: Connor W. Colombo (CMU)
@last-updated: 03/05/2023

Functions
---------

    
`import_package_modules(package: module) ‑> Dict[str, module]`
:   Imports all the packages in the given module.
    Returns a dictionary with keys that are the full module name and the values
    are the imported modules themselves.
    
    NOTE: This only grabs one layer deep and **will not** import
    packages within packages.
    
    E.g.:
    ```py
    import IrisBackendv3.codec.packet_classes
    create_package_entries(IrisBackendv3.codec.packet_classes)
    ```
    will return a dictionary that looks like:
    ```py
    {
        'IrisBackendv3.codec.packet_classes.custom_payload': IrisBackendv3.codec.packet_classes.custom_payload,
        'IrisBackendv3.codec.packet_classes.iris_common': IrisBackendv3.codec.packet_classes.iris_common,
        # etc ...
    }
    ```