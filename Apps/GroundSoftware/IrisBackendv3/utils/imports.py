# -*- coding: utf-8 -*-
"""
A Collection of utility functions for handling imports:

@author: Connor W. Colombo (CMU)
@last-updated: 03/05/2023
"""
from typing import Dict
from types import ModuleType

import pkgutil
from typing import Dict


def import_package_modules(package: ModuleType) -> Dict[str, ModuleType]:
    """Imports all the packages in the given module.
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
    """
    package_name = package.__name__
    package_path = package.__path__
    # Grab info on all modules in the package:
    module_infos = [*pkgutil.iter_modules(
        package_path,
        prefix=f"{package_name}."
    )]

    # Import the modules:
    modules = dict()
    for module_info in module_infos:
        finder = module_info.module_finder
        loader = finder.find_module(module_info.name)  # type: ignore
        if loader is not None:
            m = loader.load_module(module_info.name)
            modules[module_info.name] = m

    return modules
