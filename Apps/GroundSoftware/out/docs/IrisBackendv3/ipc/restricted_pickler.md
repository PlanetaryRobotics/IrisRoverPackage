Module IrisBackendv3.ipc.restricted_pickler
===========================================
Tools for 'safely' pickling data to be sent over IPC. See notes in
`_IrisRestrictedUnpickler` for restrictions on where this is safe to use (this
package, like pickle itself, should still be used with caution).

NOTE: All modules should be absolute imports, starting from the
IrisBackendv3 root, and not imported via script (importing via script
creates a duplicate import that's in the wrong place in `sys.modules`).

@author: Connor W. Colombo (CMU)
@last-updated: 03/06/2023

Functions
---------

    
`create_module_entry(module: module) ‑> Dict[str, module]`
:   Creates a {module_name: module} dictionary as an entry to
    `approved_iris_modules`.
    e.g.: `{'IrisBackendv3.codec.magic': IrisBackendv3.codec.magic}`

    
`restricted_dumps(o: Any) ‑> bytes`
:   Functionally analogous to pickle.dumps(), but wrapped to do any prep
    work necessary to use the restricted unpickler.

    
`restricted_loads(s: bytes) ‑> Any`
:   Functionally analogous to pickle.loads() but uses a restricted
    unpickler.