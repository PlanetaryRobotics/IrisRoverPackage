"""
Tools for 'safely' pickling data to be sent over IPC. See notes in
`_IrisRestrictedUnpickler` for restrictions on where this is safe to use (this
package, like pickle itself, should still be used with caution).

NOTE: All modules should be absolute imports, starting from the
IrisBackendv3 root, and not imported via script (importing via script
creates a duplicate import that's in the wrong place in `sys.modules`).

@author: Connor W. Colombo (CMU)
@last-updated: 03/06/2023
"""

import io
import builtins
import datetime

if _USING_DILL := False:
    # using dill instead of pickle for more robust serialization, esp. with multiprocessing
    import dill as pickle
else:
    import pickle

from typing import Any, Final, ClassVar, Dict, Set
from types import ModuleType

# from IrisBackendv3.utils.imports import import_package_modules
from IrisBackendv3.ipc.logs import logger

import IrisBackendv3.transceiver.xcvr_enum
import IrisBackendv3.codec.packet_classes
import IrisBackendv3.codec.packet
import IrisBackendv3.codec.magic
import IrisBackendv3.codec.metadata
import IrisBackendv3.codec.payload
import IrisBackendv3.codec.payload_collection

import IrisBackendv3.codec.packet_classes.packet
import IrisBackendv3.codec.packet_classes.unsupported
import IrisBackendv3.codec.packet_classes.watchdog_heartbeat
import IrisBackendv3.codec.packet_classes.watchdog_command_response
import IrisBackendv3.codec.packet_classes.iris_common
import IrisBackendv3.codec.packet_classes.watchdog_detailed_status
import IrisBackendv3.codec.packet_classes.watchdog_tvac_heartbeat
import IrisBackendv3.codec.packet_classes.radio_ground
import IrisBackendv3.codec.packet_classes.radio_hello
import IrisBackendv3.codec.packet_classes.radio_bgapi_packet
import IrisBackendv3.codec.packet_classes.radio_uart_byte
import IrisBackendv3.codec.packet_classes.radio_direct_message
import IrisBackendv3.codec.packet_classes.watchdog_reset_specific_ack
import IrisBackendv3.codec.packet_classes.watchdog_hello
import IrisBackendv3.codec.packet_classes.watchdog_radio_debug
import IrisBackendv3.codec.packet_classes.watchdog_debug_important
import IrisBackendv3.codec.packet_classes.watchdog_debug
import IrisBackendv3.codec.packet_classes.radio_downlink_flush

# Use the latest supported version of pickle (faster and more performant):
# (don't worry, pickle handles compat. checking with messages).
_PICKLE_PROTOCOL: Final[int] = 5

# Version of Iris Restricted Pickler. Inc. when there's a change that could
# break backwards compatibility. (gets checked on receiving end to ensure
# compatibility).
IRIS_RESTRICTED_PICKLER_VERSION: Final[int] = 1


def create_module_entry(module: ModuleType) -> Dict[str, ModuleType]:
    """Creates a {module_name: module} dictionary as an entry to
        `approved_iris_modules`.
        e.g.: `{'IrisBackendv3.codec.magic': IrisBackendv3.codec.magic}`
        """
    return {module.__name__: module}


# def create_package_entries(package: ModuleType) -> Dict[str, ModuleType]:
#     """Creates a number of {module_name: module} entries in a dictionary for
#     every module in the given package, including importing all the necessary
#     modules from the package.

#     ! NOTE: As written currently, DON'T USE THIS. This places modules in the
#      wrong place in `sys.modules` and **very** subtly breaks the pickler.
#     (specifically imports get put under a new `IrisBackendv3` instance
#      **inside** `IrisBackendv3.ipc.restricted_pickler`, instead of in the
#      root of `sys.modules`)

#     NOTE: By design, this only grabs one layer deep and **will not** import
#     packages within packages. This is to make it so that such actions need to
#     be intentional and one line of code can't inadvertently add a bunch of new
#     modules to the approved list.
#     E.g.:
#     ```py
#     import IrisBackendv3.codec.packet_classes
#     create_package_entries(IrisBackendv3.codec.packet_classes)
#     ```
#     will return a dictionary that looks like:
#     ```py
#     {
#         'IrisBackendv3.codec.packet_classes.custom_payload': IrisBackendv3.codec.packet_classes.custom_payload,
#         'IrisBackendv3.codec.packet_classes.iris_common': IrisBackendv3.codec.packet_classes.iris_common,
#         # etc ...
#     }
#     ```
#     """
#     modules = import_package_modules(package)
#     return modules


# Shorthand:
_m = create_module_entry
# _p = create_package_entries

_DEFAULT_APPROVED_IRIS_MODULES: Final[Dict[str, ModuleType]] = {
    'datetime': datetime,
    **_m(IrisBackendv3.transceiver.xcvr_enum),
    **_m(IrisBackendv3.codec.magic),
    **_m(IrisBackendv3.codec.metadata),
    **_m(IrisBackendv3.codec.payload),
    **_m(IrisBackendv3.codec.payload_collection),
    **_m(IrisBackendv3.codec.packet),
    **_m(IrisBackendv3.codec.packet_classes.packet),
    **_m(IrisBackendv3.codec.packet_classes.unsupported),
    **_m(IrisBackendv3.codec.packet_classes.watchdog_heartbeat),
    **_m(IrisBackendv3.codec.packet_classes.watchdog_command_response),
    **_m(IrisBackendv3.codec.packet_classes.iris_common),
    **_m(IrisBackendv3.codec.packet_classes.watchdog_detailed_status),
    **_m(IrisBackendv3.codec.packet_classes.watchdog_tvac_heartbeat),
    **_m(IrisBackendv3.codec.packet_classes.radio_ground),
    **_m(IrisBackendv3.codec.packet_classes.radio_hello),
    **_m(IrisBackendv3.codec.packet_classes.radio_bgapi_packet),
    **_m(IrisBackendv3.codec.packet_classes.radio_uart_byte),
    **_m(IrisBackendv3.codec.packet_classes.radio_direct_message),
    **_m(IrisBackendv3.codec.packet_classes.watchdog_reset_specific_ack),
    **_m(IrisBackendv3.codec.packet_classes.watchdog_hello),
    **_m(IrisBackendv3.codec.packet_classes.watchdog_radio_debug),
    **_m(IrisBackendv3.codec.packet_classes.watchdog_debug_important),
    **_m(IrisBackendv3.codec.packet_classes.watchdog_debug),
    **_m(IrisBackendv3.codec.packet_classes.radio_downlink_flush)
}


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

    approved_iris_modules: ClassVar[Dict[str, ModuleType]] = \
        _DEFAULT_APPROVED_IRIS_MODULES

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

        # Extend to also allow dill types:
        if _USING_DILL:
            if (module, name) == ('__builtin__', '__main__'):
                return self._main.__dict__  # XXX: above set w/save_module_dict
            elif (module, name) == ('__builtin__', 'NoneType'):
                return type(None)  # XXX: special case: NoneType missing
            if module == 'dill.dill':
                module = 'dill._dill'
            if module == 'dill._dill':
                return pickle.Unpickler.find_class(self, module, name)

        # Forbid everything else:
        raise pickle.UnpicklingError(
            f"global '{name}' in module '{module}' is forbidden"
        )

    def persistent_load(self, pid: Any) -> Any:
        logger.success(f"\t\tload PID: {pid}")
        if pid in self.approved_iris_modules:
            return self.approved_iris_modules[pid]
        raise pickle.UnpicklingError(
            f"`{pid=} doesn't match an approved module in "
            f"`approved_iris_modules`."
        )

    @classmethod
    def restricted_loads(cls, s):
        """Helper function analogous to pickle.loads()."""
        return cls(io.BytesIO(s)).load()


class _IrisRestrictedPickler(pickle.Pickler):
    # Presently, these aren't used for anything but this is here in case we
    # need to do map alignment between the pickler and unpickler (presently,
    # seems unnecessary):
    approved_iris_modules: ClassVar[Dict[str, ModuleType]] = \
        _DEFAULT_APPROVED_IRIS_MODULES

    def persistent_id(self, obj: Any) -> Any:
        key_matches = [
            k for k, v in self.approved_iris_modules.items()
            if obj == v
        ]
        if len(key_matches) > 0:
            logger.success(f"\t\dump PID: {obj}")
            return key_matches[0]
        return None

    @classmethod
    def restricted_dumps(cls, o: Any) -> bytes:
        """Helper function analogous to pickle.dumps()."""
        f = io.BytesIO()
        cls(f, protocol=_PICKLE_PROTOCOL).dump(o)
        return f.getvalue()


def restricted_loads(s: bytes) -> Any:
    """ Functionally analogous to pickle.loads() but uses a restricted
    unpickler."""
    return _IrisRestrictedUnpickler.restricted_loads(s)


def restricted_dumps(o: Any) -> bytes:
    """ Functionally analogous to pickle.dumps(), but wrapped to do any prep
    work necessary to use the restricted unpickler."""
    return _IrisRestrictedPickler.restricted_dumps(o)
