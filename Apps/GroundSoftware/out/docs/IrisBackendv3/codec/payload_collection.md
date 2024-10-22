Module IrisBackendv3.codec.payload_collection
=============================================
Defines convenient containers for `Payload`s of multiple classes.

@author: Connor W. Colombo (CMU)
@last-updated: 05/06/2022

Functions
---------

    
`catalog_payload_subclass(c: Type[Payload])`
:   

    
`extract_downlinked_payloads(VLP: bytes, endianness_code: str = '<') ‑> IrisBackendv3.codec.payload_collection.EnhancedPayloadCollection`
:   Extract all Payloads from a Variable Length Payload to produce a
    `EnhancedPayloadCollection`.
    
    Will be used for both downlinked and uplinked packets since both are
    encoded when being passed around IPC.
    
    Per the Iris C&TL, the VLP should be structured as:
    [magic-0][payload-0][magic-1][payload-1]...[magic-N][payload-N]
    
    Args:
        VLP (bytes): Variable Length Payload
    
    Returns:
        EnhancedPayloadCollection:
        Collection of Lists of Payloads, separated by Payload Type.
        All payload types are included but some lists will be empty if no
        payload of that type was found in the given VLP.

    
`map_tree_node(root: PayloadTreeNode) ‑> None`
:   

    
`populate_cpc_list(root: PayloadTreeNode) ‑> None`
:   

Classes
-------

`BadEnhancedPayloadCollectionKeyError(func_name: str, key: EPC_KT, name: Optional[str])`
:   Attribute not found.
    
    Creates an `Error` for a bad `key` given to the function `func_name` of
    `EnhancedPayloadCollection`.

    ### Ancestors (in MRO)

    * builtins.AttributeError
    * builtins.Exception
    * builtins.BaseException

    ### Class variables

    `func_name: str`
    :

    `key: Union[str, Type[IrisBackendv3.codec.payload.Payload], int]`
    :

    `tree_map: Dict[str, IrisBackendv3.codec.payload_collection.PayloadTreeNode]`
    :

    ### Instance variables

    `name: str`
    :   attribute name

`EnhancedPayloadCollection(**kwargs)`
:   A container for `Payload`s that allows subsets of it to be queried by
    payload class using either concrete classes (like `CommandPayload`) or
    abstract classes (like `DownlinkedPayload`). Querying with a class will get
    all `Payloads` that belong to that class either as a direct instance or as
    an instance of a subclass.
    
    **For example:**
    ```
    epc = EnhancedPayloadCollection(...)
    # ...
    epc[WatchdogCommandPayload]
    ```
    will get all `Payload`s in the collection that are instances of
    `WatchdogCommandPayload`.
    
    ```
    epc[CommandPayload]
    ```
    will get all `Payload`s in the collection that are instances of
    `CommandPayload` (incl. all instances of `WatchdogCommandPayload` which is
    a subclass of `CommandPayload`).
    
    ```
    epc[DownlinkedPayload]
    ```
    will get all `Payload`s in the collection that are instances of
    a class that's a descendant of `DownlinkedPayload` (such as
    `FileBlockPayload`, `EventPayload`, `TelemetryPayload`, any subclasses of
    `TelemetryPayload`, etc.).
    
    Each instance of this class maintains a private `_payloads_dict` which maps
    the names of all concrete (instantiatable/non-abstract) `Payload` subclasses
    to lists which contain the `Payload`s that directly instantiate those
    classes.
    
    This class makes use of the following prebuilt collections:
    - `PAYLOAD_CLASS_TREE` which contains the hierarchy of all `Payload`
    subclasses.
    - `PAYLOAD_CLASS_TREE_MAP` which maps class names to `PayloadTreeNode`s in
    the `PAYLOAD_CLASS_TREE`, allowing quick extraction of the subtree
    corresponding to a given class.
    - `CONCRETE_PAYLOAD_CLASSES`, a list of the names of all concrete (instantiatable) `Payload` classes.
    
    
    This is a successor to the now deprecated `namedtuple`-based
    `PayloadCollection` class.

    ### Ancestors (in MRO)

    * collections.abc.Iterable
    * typing.Generic

    ### Static methods

    `key_to_name(key: EPC_KT) ‑> str | None`
    :   Converts a given `key` used to identify a `Payload` subclass by
        several different means into the name of that `Payload` subclass.
        If the given `key` is not valid, `None` is returned.
        
        There are several supported key schemes:
        
        - `type`: a `Payload` subclass itself
        - `str`: the `__name__` of a `Payload` subclass
        - `int`: the numerical index which is the position of a concrete
            `Payload` subclass in the `CONCRETE_PAYLOAD_CLASSES` list.
            NOTE: **This functionality is not necessarily consistent between**
            **runs of the program and, as such, is NOT recommended for use**
            **use in serialization. It only exists to allow **
            **`EnhancedPayloadCollection` to be backwards-compatible with code**
            **that used its `namedtuple`-based predecessor `PayloadCollection`.**

    `make_empty() ‑> IrisBackendv3.codec.payload_collection.EnhancedPayloadCollection`
    :   Makes a new empty `EnhancedPayloadCollection` (just
        `EnhancedPayloadCollection()`).
        
        This function only exists to preserve backwards-compatibility with code
        that used the `namedtuple`-based predecessor `PayloadCollection`.

    ### Instance variables

    `all_payloads: Iterable[IrisBackendv3.codec.payload.Payload]`
    :   Returns an iterable of all payloads in the payload collection,
        no matter the `Payload` type.
        
        NOTE: the return value is just an iterable (chain) so it can't be
        modified like a list (append, extend, etc.) **BUT** modifying the
        `Payloads` themselves (adding metadata, etc). will behave as expected.
        
        NOTE: b/c the return value is an iterable (chain) it can only be
        traversed once (every time you call __next__ element, you pop an element
        (it gets shorter)). If you need this to behave like a list, call
        `list(epc.all_payloads)` or `[*epc.all_payloads]` for short.
        
        If you want to modify the `EnhancedPayloadCollection`, use `append`,
        `extend`, or `remove`.

    `num_payloads: int`
    :   The total number of `Payload`s in the `EnhancedPayloadCollection`.

    ### Methods

    `append(self, payload: Payload)`
    :   Appends the given `Payload` to the appropriate list in `_payloads_dict`,
        (the one corresponding to its `__class__`).

    `clear(self) ‑> None`
    :   Removes all `Payload`s from every list in `_payloads_dict`.

    `erase(self, key: EPC_KT) ‑> None`
    :   Removes all payloads associated with the `Payload` subclass of the
        given `key` (removes everything in the list in `_payloads_dict`
        corresponding to the class indicated by `key` (if it's a concrete class)
        as well as the lists of all of its concrete subclasses).

    `extend(self, val: Union[EnhancedPayloadCollection, Iterable[Payload]]) ‑> None`
    :   Appends each value in the given iterable to the appropriate
        list in `_payloads_dict` (the one corresponding to its `__class__`).
        
        If `val` is another `EnhancedPayloadCollection`, then type checking and
        sorting steps will be skipped and all core lists in `val` will be
        appended to the lists in this collection.
        
        See `append` for more details.

    `is_empty(self) ‑> bool`
    :   Returns whether all Payloads in the Payload Collection are empty.
        Included for backwards compatibility with `PayloadCollection`.

    `remove(self, payload: Payload)`
    :   Removes the given `Payload` to the appropriate list in `_payloads_dict`,
        (the one corresponding to its `__class__`).
        
        NOTE: This will remove at most one instance. So, if there are multiple
        identical payloads, only one will have been removed.

`PayloadCollection(CommandPayload: ForwardRef('List[CommandPayload]'), TelemetryPayload: ForwardRef('List[TelemetryPayload]'), EventPayload: ForwardRef('List[EventPayload]'), FileBlockPayload: ForwardRef('List[FileBlockPayload]'))`
:   [[DEPRECATED]] Use `EnhancedPayloadCollection` instead.
    Dictionary Collection of Lists of Payloads, separated by Payload Type.
    *NOTE:* All keys are strings and should accessed using class.__name__
    
    This means that, if class names are changed, these strings will have to be
    MANUALLY updated. This should be relatively easy since, although the linter
    won't detect the member name as outdated, it will detect the class name in
    the `List[]` type hint as outdated and all the developer has to do is make
    sure the two are the same.

    ### Ancestors (in MRO)

    * builtins.tuple

    ### Static methods

    `make_empty() ‑> IrisBackendv3.codec.payload_collection.PayloadCollection`
    :   Makes a new empty payload collection.

    ### Instance variables

    `CommandPayload: List[IrisBackendv3.codec.payload.CommandPayload]`
    :   Alias for field number 0

    `EventPayload: List[IrisBackendv3.codec.payload.EventPayload]`
    :   Alias for field number 2

    `FileBlockPayload: List[IrisBackendv3.codec.payload.FileBlockPayload]`
    :   Alias for field number 3

    `TelemetryPayload: List[IrisBackendv3.codec.payload.TelemetryPayload]`
    :   Alias for field number 1

    ### Methods

    `is_empty(self) ‑> bool`
    :   Returns whether all Payloads in the Payload Collection are empty.

`PayloadTreeNode(payload_class: Type[Payload], subtree: List[PayloadTreeNode] = _Nothing.NOTHING)`
:   Helper class for `EnhancedPayloadCollection` for mapping the hierarchy of
    all `Payload` subclasses.
    
    Notably, the `Payload` classes themselves can serve as this tree (using
    `__subclasses__()` as `subtree`). However, creating a serializable wrapper
    class like this allows for storing and preserving what the hierarchy looked
    like at a particular point in time when a collection of data was captured.
    
    Also, although likely trivial, it's about 12% faster to use this to access
    hierarchy information than `__subclasses__` (400ns vs 450ns to grab all
    subclass names for `Payload` on the dev machine). Since this will be called
    **A LOT** and response times on the order of `ms` will matter for this
    application, that might be (very) marginally significant.
    
    Method generated by attrs for class PayloadTreeNode.

    ### Static methods

    `build_class_tree(c: Type[Payload]) ‑> IrisBackendv3.codec.payload_collection.PayloadTreeNode`
    :   Builds an empty `PayloadTreeNode` for the given
        `Payload` (sub)class.

    `make_tree() ‑> IrisBackendv3.codec.payload_collection.PayloadTreeNode`
    :   Builds an empty `PayloadTreeNode` for all `Payload`
        (sub)classes (root of tree is `Payload`).
        
        NOTE: It's actually faster (about 3x) to just run this every time
        you need a new empty tree than to deepcopy a precomputed and cached
        instance (46us vs 147us on the dev machine).

    ### Instance variables

    `payload_class: Type[IrisBackendv3.codec.payload.Payload]`
    :   Return an attribute of instance, which is of type owner.

    `subtree: List[IrisBackendv3.codec.payload_collection.PayloadTreeNode]`
    :   Return an attribute of instance, which is of type owner.