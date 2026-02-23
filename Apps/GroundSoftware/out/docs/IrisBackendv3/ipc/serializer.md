Module IrisBackendv3.ipc.serializer
===================================
Generic serializer tools for Iris IPC, which serializes and signs /
deserializes and verifies data sent over IPC.

Currently the core of the serialization is `ipc.restricted_pickler`. See notes
in `_IrisRestrictedUnpickler` for restrictions on where this is safe to use
(like pickle itself, it should still be used with caution).

@author: Connor W. Colombo (CMU)
@last-updated: 03/06/2023

Functions
---------

    
`deserialize(s: bytes) ‑> Any`
:   Functionally analogous to  `restricted_loads` but verifies a
    double-signed message made using `serialize`.

    
`serialize(o: Any) ‑> bytes`
:   Wrapped for `restricted_dumps` that double signs the
    message.

Classes
-------

`IrisGenericSerializer(extra_modules: Optional[List[module]] = None)`
:   Custom Serializer which uses the `ipc.restricted_pickler` to serialize
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

    ### Methods

    `deserialize(self, s: bytes) ‑> Any`
    :   Functionally analogous to  `restricted_loads` but uses the custom
        `_IrisRestrictedUnpickler` for this `IrisGenericSerializer` and
        verifies a double-signed message made using `serialize`.

    `serialize(self, o: Any) ‑> bytes`
    :   Wrapped for `restricted_dumps` that double signs the
        message.