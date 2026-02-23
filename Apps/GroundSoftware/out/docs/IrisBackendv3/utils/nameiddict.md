Module IrisBackendv3.utils.nameiddict
=====================================
A dictionary where each value is keyed by both a string name and numeric id.

Used for looking up modules, commands, etc. by either name or opcode/id.

@author: Connor W. Colombo (CMU)
@last-updated: 01/23/2021

Classes
-------

`NameIdDict(dictionary=None)`
:   A dictionary where each value is keyed by *both* a string name and integer ID.
    
    The internal principle of operation is that there is an inner dict
    (`id_dict`) mapping IDs to [VT] values and an outer dict (`name_id_mapping`)
    mapping one or more names to each ID.
    As such, multiple names can map to the same ID, though that should be done
    with some careful consideration since the meaning of the map might become
    less useful in some contexts.
    In addition, there are a lot of supporting checks to make sure this expected
    structure stays intact.

    ### Ancestors (in MRO)

    * collections.abc.MutableMapping
    * collections.abc.Mapping
    * collections.abc.Collection
    * collections.abc.Sized
    * collections.abc.Iterable
    * collections.abc.Container
    * typing.Generic

    ### Class variables

    `KeyEnumerator`
    :

    `KeyEnumeratorTuple`
    :

    `KeyTuple`
    :

    `NamelessIDException`
    :   All names which used to map to an ID no longer map to that ID.

    `id_dict: Dict[int, ~VT]`
    :

    `name_id_mapping: Dict[str, int]`
    :

    ### Static methods

    `is_valid_key_enumerator(key: Any) ‑> bool`
    :   Check whether the given key is a valid enumerator of all names which map to a given ID(of type List | Tuple[List[str], int]).
        
        Key enumerators allow for completely caching and rebuilding a NameIdDict
        with the output of `collect()`.

    `is_valid_key_tuple(key: Any) ‑> bool`
    :   Check whether the given key is a valid key tuple(of type Tuple[int, str].)

    ### Instance variables

    `ids: List[int]`
    :   Return all the unique IDs as a list.

    `key_enumerators: List[Tuple[List[str], int]]`
    :   Somewhat inefficient but consistent way of getting all KeyEnumerators.
        
        Really just a shorthand.
        Importantly, this method is guaranteed to be consistent with the
        ordering(sorted) of the output of `collect()` (since it just uses that
        output).

    `names: List[str]`
    :   Return all the unique names as a list(even if multiple names map to the same ID).

    `vals: List[~VT]`
    :   Return all[VT] data values as a list. If multiple unique IDs have the same value, values will be duplicated.

    ### Methods

    `check_safe_to_remap(self, name: str, ID: int) ‑> None`
    :   Check whether it's safe to remap the given name to the given ID.
        
        Throws a relevant exception if there's a problem.

    `collect(self) ‑> List[Tuple[KeyEnumeratorTuple, VT]]`
    :   Collect all names which map to each registed ID pair and return as Tuple list alongside each registered ID's corresponding[VT] data.
        
        This implicitly returns a list of tuples containing all KeyEnumerators
        and their matching values. This guarantee must be ensured for the sake
        of using this as a possible serialization method.
        
        As a requirement, the returned list should be sorted by ID and each of the names lists should also be sorted using standard string comparison.

    `fast_items(self) ‑> Iterator[Tuple[Tuple[int, str], ~VT]]`
    :   Create an iterator which will loop through each piece of stored data[VT] once.
        
        Slightly faster than items itself since fewer checks need to be
        done when getting items(we already know all keys are valid).
        
        Iterator's __next__ returns a tuple pairing an(id and a name) to a
        piece of data.
        
        Output will be sorted by ID, not in insertion order.
        
        If multiple names map to the same ID, only one name(likely the
        * latest * entry) will be visited to ensure that each piece of mapped
        data is only visited once.
        
        Use `collect()` if you want every name for each ID to be included in the output.

    `update(self, other=(), /, **kwargs) ‑> None`
    :   Include all data from the given dataset(other NameIdDict, mapping, or iterable).
        
        Ignore `kwargs` since the keys will be strings not Tuple[int, str].
        `kwargs` needs to stay in the signature to maintain compatability with
        MutableMapping.
        It's reasonable to consider that no user with an understanding of this
        class will try to feed in values via `kwargs` since string-keyed
        dictionaries are inherently incompatible with the fundamental idea of
        this class (where each entry has * both * string and integer keys).