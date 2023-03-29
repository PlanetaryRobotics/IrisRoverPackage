"""
Generic serializer tools for Iris IPC, which serializes and signs /
deserializes and verifies data sent over IPC.

Currently the core of the serialization is `ipc.restricted_pickler`. See notes
in `_IrisRestrictedUnpickler` for restrictions on where this is safe to use
(like pickle itself, it should still be used with caution).

@author: Connor W. Colombo (CMU)
@last-updated: 03/06/2023
"""

from typing import Any, Final, ClassVar, Dict, List, Type
from types import ModuleType

from IrisBackendv3.ipc.signature import add_digest, extract_message_and_check_integrity
from IrisBackendv3.ipc.restricted_pickler import (
    _IrisRestrictedUnpickler,
    _IrisRestrictedPickler,
    create_module_entry
)


# Version of Iris Serializer. Inc. when there's a change that could
# break backwards compatibility. (gets checked on receiving end to ensure
# compatibility).
IRIS_SERIALIZER_VERSION: Final[int] = 1


class IrisGenericSerializer:
    """Custom Serializer which uses the `ipc.restricted_pickler` to serialize
    generic data, so long as data only comes from a list of approved modules.
    Serialized data is signed and verified using the `ipc.signature` module.

    This class also adds the ability to add extra approved Iris modules for an
    application specific context.
    This is required not only for code cleanliness but also to prevent circular
    dependencies when classes such as those in `ipc` need to be pickled but
    also require use of the restricted pickler.

    **NOTE:**careful consideration should be given when adding any iris module
    to `approved_iris_modules` to ensure there are no ways for it to be used to
    allow remote code execution (i.e. modules should only contain basic
    containers which don't have any functions, lambdas, etc as attributes).
    """

    _restricted_unpickler: Type[_IrisRestrictedUnpickler]
    _restricted_pickler: Type[_IrisRestrictedPickler]

    def __init__(self, extra_modules: List[ModuleType] | None = None) -> None:
        """
        Creates a specialized instance of `IrisGenericSerializer`.
        This will support serializing all modules
        `ipc.restricted_pickler._IrisRestrictedUnpickler.approved_iris_modules`
        as well as any of the extra modules given in `extra_modules` if any are
        given.

        **NOTE:**careful consideration should be given when adding any iris module
        to `approved_iris_modules` to ensure there are no ways for it to be used to
        allow remote code execution (i.e. modules should only contain basic
        containers which don't have any functions, lambdas, etc as attributes).

        NOTE: For security, this only supports adding modules from
        `IrisBackendv3`.

        NOTE: As a matter of standardization, each entry in `extra_modules`
        should be given as a dot series from `IrisBackendv3`. Example:
        ```py
        import IrisBackendv3.ipc.messages.m_downlinked_packets
        IrisGenericSerializer([
            IrisBackendv3.ipc.messages.m_downlinked_packets
        ])
        ```
        """
        if extra_modules is None:
            # Just use the default:
            self._restricted_unpickler = _IrisRestrictedUnpickler
            self._restricted_pickler = _IrisRestrictedPickler
        else:
            self._use_extra_modules(extra_modules)

    def _use_extra_modules(
        self,
        extra_modules: List[ModuleType]
    ) -> None:
        """
        Supports the use of extra modules by creating a custom restricted
        pickler and unpickler for use by this serializer.

        NOTE: All modules should be absolute imports, starting from the
        IrisBackendv3 root, and not imported via script (importing via script
        creates a duplicate import that's in the wrong place in `sys.modules`).

        **NOTE:**careful consideration should be given when adding any iris module
        to `approved_iris_modules` to ensure there are no ways for it to be used to
        allow remote code execution (i.e. modules should only contain basic
        containers which don't have any functions, lambdas, etc as attributes).

        NOTE: For security, this only supports adding modules from
        `IrisBackendv3`.

        NOTE: As a matter of standardization, each entry in `extra_modules`
        should be given as a dot series from `IrisBackendv3`. Example:
        ```py
        import IrisBackendv3.ipc.messages.m_downlinked_packets
        IrisGenericSerializer([
            IrisBackendv3.ipc.messages.m_downlinked_packets
        ])
        ```
        """
        # Make sure all modules are modules:
        if any(not isinstance(m, ModuleType) for m in extra_modules):
            raise TypeError(
                f"In `IrisGenericSerializer._use_extra_modules`, "
                f"at least one of the given `{extra_modules=}` "
                f"isn't a python module."
            )

        # Make sure all modules are modules come from `IrisBackendv3` (or, more
        # specifically, the same module as this package - in case a rename
        # happens in the future):
        root_module = __name__.split('.')[0]
        mod_roots = [m.__name__.split('.')[0] for m in extra_modules]
        if any(r != root_module for r in mod_roots):
            raise ModuleNotFoundError(
                f"In `IrisGenericSerializer.__init__`, "
                f"at least one of the given `{extra_modules=}` "
                f"**doesn't belong to `{root_module=}`**. "
                f"Module roots are: `{mod_roots=}`."
            )

        # Create a special subclass of `_IrisRestrictedUnpickler` which adds
        # the extra modules:
        mod_entries = [create_module_entry(m) for m in extra_modules]

        class _CustomIrisRestrictedUnpicklerSubClass(_IrisRestrictedUnpickler):
            approved_iris_modules: ClassVar[Dict[str, ModuleType]] = {
                **{k: v for e in mod_entries for k, v in e.items()},
                # Apply pre-existing `approved_iris_modules` *LAST* to prevent
                # allowing overrides of anything that's already in there:
                **_IrisRestrictedUnpickler.approved_iris_modules
            }

        class _CustomIrisRestrictedPicklerSubClass(_IrisRestrictedPickler):
            approved_iris_modules: ClassVar[Dict[str, ModuleType]] = {
                **{k: v for e in mod_entries for k, v in e.items()},
                # Apply pre-existing `approved_iris_modules` *LAST* to prevent
                # allowing overrides of anything that's already in there:
                **_IrisRestrictedPickler.approved_iris_modules
            }

        # Use this new restricted unpickler:
        self._restricted_unpickler = _CustomIrisRestrictedUnpicklerSubClass
        self._restricted_pickler = _CustomIrisRestrictedPicklerSubClass

    def deserialize(self, s: bytes) -> Any:
        """ Functionally analogous to  `restricted_loads` but uses the custom
        `_IrisRestrictedUnpickler` for this `IrisGenericSerializer` and
        verifies a double-signed message made using `serialize`."""
        layer1 = extract_message_and_check_integrity(s)
        layer0 = extract_message_and_check_integrity(layer1)
        o = self._restricted_unpickler.restricted_loads(layer0)
        return o

    def serialize(self, o: Any) -> bytes:
        """ Wrapped for `restricted_dumps` that double signs the
        message."""
        layer0 = self._restricted_pickler.restricted_dumps(o)
        layer1 = add_digest(layer0)
        layer2 = add_digest(layer1)
        return layer2


# For backwards compatibility with functional syntax, create a basic restricted
# pickler with no extra classes:
_BASE_IRIS_GENERIC_SERIALIZER: Final = IrisGenericSerializer()


def deserialize(s: bytes) -> Any:
    """ Functionally analogous to  `restricted_loads` but verifies a
    double-signed message made using `serialize`."""
    return _BASE_IRIS_GENERIC_SERIALIZER.deserialize(s)


def serialize(o: Any) -> bytes:
    """ Wrapped for `restricted_dumps` that double signs the
    message."""
    return _BASE_IRIS_GENERIC_SERIALIZER.serialize(o)