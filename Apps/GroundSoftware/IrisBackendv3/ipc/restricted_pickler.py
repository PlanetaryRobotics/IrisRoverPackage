"""
Tools for 'safely' pickling data to be sent over IPC. See notes in
`_IrisRestrictedUnpickler` for restrictions on where this is safe to use (this
package, like pickle itself, should still be used with caution).

@author: Connor W. Colombo (CMU)
@last-updated: 03/05/2023
"""

import pkgutil
import io
import builtins
import pickle
import datetime

from typing import Any, ClassVar, Dict, Set
from types import ModuleType

from IrisBackendv3.utils.imports import import_package_modules

import IrisBackendv3.codec.magic
import IrisBackendv3.codec.metadata
import IrisBackendv3.codec.payload
import IrisBackendv3.codec.payload_collection
import IrisBackendv3.codec.packet
import IrisBackendv3.codec.packet_classes


def create_module_entry(module: ModuleType) -> Dict[str, ModuleType]:
    """Creates a {module_name: module} dictionary as an entry to
        `approved_iris_modules`.
        e.g.: `{'IrisBackendv3.codec.magic': IrisBackendv3.codec.magic}`
        """
    return {module.__name__: module}


def create_package_entries(package: ModuleType) -> Dict[str, ModuleType]:
    """Creates a number of {module_name: module} entries in a dictionary for
    every module in the given package, including importing all the necessary
    modules from the package.

    NOTE: By design, this only grabs one layer deep and **will not** import
    packages within packages. This is to make it so that such actions need to
    be intentional and one line of code can't inadvertently add a bunch of new
    modules to the approved list.
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
    modules = import_package_modules(package)
    return modules


# Shorthand:
_m = create_module_entry
_p = create_package_entries


class _IrisRestrictedUnpickler(pickle.Unpickler):
    """
    Safe Unpickling implementation which controls what gets unpickled with a
    custom `Unpickler.find_class()`, which is called whenever a global
    (i.e., a class or a function) is requested. This only allows safe builtins
    and a selection of pre-approved and pre-imported Iris modules to be loaded.

    This is used as a security layer for any pickles sent over the internal
    Iris Backend IPC network. Pickles are only sent over IPC in the first place
    to allow any classes which are already set up for being cached as pickles
    (think `DataStandards`) to also be sent over IPC.

    Note: even though this should offer a secure way to transfer pickles over a
    network, as a matter of principle, **pickles should still never be sent**
    **over an open network**.

    Note: careful consideration should be given when adding any iris module to
    `approved_iris_modules` to ensure there are no ways for it to be used to
    allow remote code execution (i.e. modules should only contain basic
    containers which don't have any functions, lambdas, etc as attributes).
    """

    safe_builtins: ClassVar[Set[str]] = {
        'range',
        'complex',
        'set',
        'frozenset',
        'slice',
        'getattr'
    }

    approved_iris_modules: ClassVar[Dict[str, ModuleType]] = {
        'datetime': datetime,
        **_m(IrisBackendv3.codec.magic),
        **_m(IrisBackendv3.codec.metadata),
        **_m(IrisBackendv3.codec.payload),
        **_m(IrisBackendv3.codec.payload_collection),
        **_m(IrisBackendv3.codec.packet),
        **_p(IrisBackendv3.codec.packet_classes)
    }

    def find_class(self, module, name):
        # Only allow safe classes from builtins.

        # Allow safe built-ins:
        if module == "builtins" and name in self.__class__.safe_builtins:
            return getattr(builtins, name)

        # Allow approved Iris Modules:
        if module in self.__class__.approved_iris_modules:
            # Return global with the given name in the given module, in such a
            # way that handles nested classes
            # (e.g. if `name = 'IrisCommonPacket.CommonPacketHeader'`, clearly
            # `IrisCommonPacket` is in the root of the namespace of whatever
            # `module` is and `CommonPacketHeader` is within `IrisCommonPacket`
            # - as a nested class in this case.
            # The following recursive strategy handles this edge case):

            import_result = self.__class__.approved_iris_modules[module]
            for piece in name.split('.'):
                import_result = getattr(import_result, piece)

            return import_result

        # Forbid everything else:
        raise pickle.UnpicklingError(
            f"global '{name}' in module '{module}' is forbidden"
        )

    @classmethod
    def restricted_loads(cls, s):
        """Helper function analogous to pickle.loads()."""
        return cls(io.BytesIO(s)).load()


def restricted_loads(s: bytes) -> Any:
    """ Functionally analogous to pickle.loads() but uses a restricted
    unpickler."""
    return _IrisRestrictedUnpickler.restricted_loads(s)


def restricted_dumps(o: Any) -> bytes:
    """ Functionally analogous to pickle.dumps(), but wrapped to do any prep
    work necessary to use the restricted unpickler."""
    return pickle.dumps(o)
