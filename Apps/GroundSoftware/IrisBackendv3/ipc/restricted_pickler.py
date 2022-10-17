"""
Tools for 'safely' pickling data to be sent over IPC. See notes in
`_IrisRestrictedUnpickler` for restrictions on where this is safe to use (this
package, like pickle itself, should still be used with caution).

@author: Connor W. Colombo (CMU)
@last-updated: 02/27/2022
"""

import io
import builtins
import pickle
import datetime

from typing import Any, ClassVar, Set

from .signature import add_digest, extract_message_and_check_integrity

import IrisBackendv3.codec.magic
import IrisBackendv3.codec.metadata
import IrisBackendv3.codec.payload
import IrisBackendv3.codec.packet


class _IrisRestrictedUnpickler(pickle.Unpickler):
    """
    Safe Unpickling implementation which controls what gets unpickled with a
    custom `Unpickler.find_class()`, which is called whenever a global
    (i.e., a class or a function) is requested. This only allows safe builtins
    and a selection of pre-approved and pre-imported Iris modules to be loaded.

    This is used as a security layer for any pickles sent over the internal Iris
    Backend IPC network. Pickles are only sent over IPC in the first place to
    allow any classes which are already set up for being cached as pickles
    (think `DataStandards`) to also be sent over IPC.

    Note: even though this should offer a secure way to transfer pickles over a
    network, as a matter of principle, **pickles should still never be sent over
    an open network**.

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

    approved_iris_modules = {
        'datetime': datetime,
        'IrisBackendv3.codec.magic': IrisBackendv3.codec.magic,
        'IrisBackendv3.codec.metadata': IrisBackendv3.codec.metadata,
        'IrisBackendv3.codec.payload': IrisBackendv3.codec.payload,
        'IrisBackendv3.codec.packet': IrisBackendv3.codec.packet
    }

    def find_class(self, module, name):
        # Only allow safe classes from builtins.

        # Allow safe built-ins:
        if module == "builtins" and name in _IrisRestrictedUnpickler.safe_builtins:
            return getattr(builtins, name)

        # Allow approved Iris Modules:
        if module in _IrisRestrictedUnpickler.approved_iris_modules:
            # Return global with the given name in the given module, in such a
            # way that handles nested classes
            # (e.g. if `name = 'IrisCommonPacket.CommonPacketHeader'`, clearly
            # `IrisCommonPacket` is in the root of the namespace of whatever
            # `module` is and `CommonPacketHeader` is within `IrisCommonPacket`
            # - as a nested class in this case.
            # The following recursive strategy handles this edge case):

            import_result = _IrisRestrictedUnpickler.approved_iris_modules[module]
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
    """ Functionally analogous to pickle.loads() but is used for a 
    double-signed message made using `restricted_dumps`."""
    layer1 = extract_message_and_check_integrity(s)
    layer0 = extract_message_and_check_integrity(layer1)
    o = _IrisRestrictedUnpickler.restricted_loads(layer0)
    return o


def restricted_dumps(o: Any) -> bytes:
    """ Functionally analogous to pickle.dumps() but double signs the message."""
    layer0 = pickle.dumps(o)
    layer1 = add_digest(layer0)
    layer2 = add_digest(layer1)
    return layer2
