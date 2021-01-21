#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""
A dictionary where each value is keyed by both a string name and numeric id.

Used for looking up modules, commands, etc. by either name or opcode/id.

@author: Connor W. Colombo (CMU)
@last-updated: 01/17/2021
"""

# Activate postponed annotations (for using classes as return type in their own methods):
from __future__ import annotations

from typing import cast, List, Dict, Any, TypeVar, Union, Tuple, MutableMapping, Iterator
from collections import abc

import pprint

VT = TypeVar('VT')


class NameIdDict(MutableMapping[Tuple[int, str], VT]):
    """
    A dictionary where each value is keyed by *both* a string name and integer ID.

    This is handled by a map from IDs to [VT] data (values) and a separate map
    from names to corresponding IDs. As such, multiple names can map to the
    same ID, though that should be done with some careful consideration since
    the meaning of the map might become less useful in some contexts.
    """

    # Standard indexing tuple of integer ID and string name. Used for adding new
    # entries:
    KeyTuple = Tuple[int, str]
    # Key Enumerators Need to Support 2 Item Lists if being Rebuilt from JSON:
    # Looser form used for accepting the appropriate function arguments:
    KeyEnumerator = Union[Tuple[List[str], int], List[Union[List[str], int]]]
    # Stricter Version of Key Enumerator for better type checking performance.
    # If `is_valid_key_enumerator` is True, one can safely cast to this form,
    # even if the key is actually a list. Cast a key to this once
    # `is_valid_key_enumerator` is passed:
    KeyEnumeratorTuple = Tuple[List[str], int]

    id_dict: Dict[int, VT]
    name_id_mapping: Dict[str, int]

    _printer: pprint.PrettyPrinter = pprint.PrettyPrinter(
        indent=4,
        sort_dicts=False
    )

    @staticmethod
    def is_valid_key_tuple(key: Any) -> bool:
        """Check whether the given key is a valid key tuple (of type Tuple[int,str].)"""
        return isinstance(key, tuple) and len(key) == 2 and isinstance(key[0], int) and isinstance(key[1], str)

    @staticmethod
    def is_valid_key_enumerator(key: Any) -> bool:
        """
        Check whether the given key is a valid enumerator of all names which map to a given ID (of type List|Tuple[List[str],int]).

        Key enumerators allow for completely caching and rebuilding a NameIdDict 
        with the output of `collect()`.
        """
        return (
            # Is a 2-Tuple or a 2 Item List:
            (isinstance(key, tuple) or isinstance(key, list))
            and len(key) == 2
            # First entry is a non-empty str list (of names):
            and isinstance(key[0], list)
            and len(key[0]) > 0
            and isinstance(key[0][0], str)
            # Second entry is int (ID):
            and isinstance(key[1], int)
        )

    def __init__(self, dictionary=None) -> None:
        self.id_dict = {}
        self.name_id_mapping = {}

        if dictionary is not None:
            self.update(dictionary)

    def __getitem__(self, key: Union[int, str, NameIdDict.KeyTuple, NameIdDict.KeyEnumerator]) -> VT:
        """Return the item with the given key."""
        if isinstance(key, int):
            return self.id_dict[key]
        if isinstance(key, str):
            if key not in self.name_id_mapping:
                raise KeyError(f"Given name {key} does not map to any ID.")
            return self.id_dict[self.name_id_mapping[key]]
        elif self.is_valid_key_tuple(key):
            key = cast(NameIdDict.KeyTuple, key)
            if key[1] not in self.name_id_mapping:
                raise KeyError(
                    f"Given name {key[1]} does not map to any ID.")
            if self.name_id_mapping[key[1]] != key[0]:
                raise KeyError(
                    f"Given name {key[1]} maps to ID {self.name_id_mapping[key[1]]} and not to the given ID {key[0]}.")
            return self.id_dict[key[0]]
        elif self.is_valid_key_enumerator(key):
            key = cast(NameIdDict.KeyEnumeratorTuple, key)
            names, ID = key
            for name in names:
                if name not in self.name_id_mapping:
                    raise KeyError(
                        f"Given name '{name}' in the Key Enumerator {key} does not map to *any* ID in this NameIdDict."
                    )
                if self.name_id_mapping[name] != ID:
                    raise KeyError(
                        f"Given name '{key[1]}' in the Key Enumerator {key} maps to ID {self.name_id_mapping[name]} and not to the given ID {ID}."
                    )
            return self.id_dict[ID]
        else:
            raise TypeError(
                "Argument to __getitem__ of NameIdDict must be integer ID, string name, or a Tuple[int,str] or Tuple[List[str], int] of the two."
            )

    def __setitem__(self, key: Union[int, str, NameIdDict.KeyTuple, NameIdDict.KeyEnumerator], value: VT) -> None:
        """
        Set an item in the NameIdDict with the given key.

        If the given index isn't already in the NameIdDict, both an integer ID and string name must be given.

        *Notes for indexing with tuples:*
        If the name already exists but a new or different ID is being used, the name will be remapped to the value associated with the given ID.
        (Each unique name can only map to one unique ID).

        If an entry with this ID already exists, its [VT] data will be updated.
        The previous string which mapped to this ID will be preserved but it
        will now map to this new updated value.
        """
        if isinstance(key, int):
            if key in self.ids:
                self.id_dict[key] = value
            else:
                raise KeyError(
                    f"Index ID {key} isn't already in the NameIdDict. NameIdDict must have both an integer ID and string name supplied as an [int,str] tuple when the given index doesn't already exist.")

        elif isinstance(key, str):
            if key in self.name_id_mapping:
                self.id_dict[self.name_id_mapping[key]] = value
            else:
                raise KeyError(
                    f"Index name {key} isn't already in the NameIdDict. NameIdDict must have both an integer ID and string name supplied as an [int,str] tuple when the given index doesn't already exist.")

        elif self.is_valid_key_enumerator(key):
            key = cast(NameIdDict.KeyEnumeratorTuple, key)
            names, ID = key
            self.id_dict[ID] = value
            for name in names:
                self.name_id_mapping[name] = ID

        elif self.is_valid_key_tuple(key):
            key = cast(NameIdDict.KeyTuple, key)
            self.id_dict[key[0]] = value
            self.name_id_mapping[key[1]] = key[0]

        else:
            raise TypeError(
                "NameIdDict should only have values set with a Tuple[int,str], List|Tuple[List[str],int], or the names or IDs of existing entries"
            )

    def __delitem__(self, key: Union[int, str, NameIdDict.KeyTuple, NameIdDict.KeyEnumerator]) -> None:
        """Delete all entries with that map through given integer ID or map from the given string name."""
        if self.is_valid_key_tuple(key):
            key = cast(NameIdDict.KeyTuple, key)

            if key[1] not in self.name_id_mapping:
                raise KeyError(
                    f"Index {key} can't be deleted from this NameIdDict since it's name doesn't exist in this NameIdDict."
                )

            if self.name_id_mapping[key[1]] != key[0]:
                raise ValueError(
                    f"Given name {key[1]} maps to ID {self.name_id_mapping[key[1]]} and not to the given ID {key[0]}."
                )

            # Just delete based on the ID (most thorough option):
            del self[key[0]]

        elif self.is_valid_key_enumerator(key):
            key = cast(NameIdDict.KeyEnumeratorTuple, key)
            names, ID = key

            for name in names:
                if name not in self.name_id_mapping:
                    raise KeyError(
                        f"Index {key} can't be deleted from this NameIdDict since at least one of it's names ({name}) doesn't exist in this NameIdDict."
                    )

                if self.name_id_mapping[name] != ID:
                    raise ValueError(
                        f"At least one of the given names ({name}) maps to ID {self.name_id_mapping[name]} and not to the given ID {ID}."
                    )

            # Just delete based on the ID (most thorough option):
            del self[ID]

        elif isinstance(key, int):
            # Ensure key is in dictionary and remove it:
            if key in self.id_dict.keys():
                del self.id_dict[key]
                # Find all names which map to this ID key (should only be 1 name
                # except maybe in a weird edge case where setter checks are
                # intentionally circumvented) and remove them:
                orig_items = list(self.name_id_mapping.items())
                for k, v in orig_items:
                    if v == key:
                        del self.name_id_mapping[k]
            else:
                raise KeyError(
                    f"ID {key} can't be deleted from this NameIdDict since it's not a member."
                )
        elif isinstance(key, str):
            # Ensure key is in names mapping and remove it and it's associated
            # ID (invoke __delitem__ to do so to handle any extra cleanup required).
            orig_keys = list(self.name_id_mapping.keys())
            if key in orig_keys:
                ID = self.name_id_mapping[key]
                del self.name_id_mapping[key]
                self.__delitem__(ID)
            else:
                raise KeyError(
                    f"Name {key} can't be deleted from this NameIdDict since it's not a member."
                )
        else:
            raise TypeError(
                "Can only delete an item if given a valid index "
                "(an integer ID, a string name, or a Tuple[int,str] or List|Tuple[List[str],int] of the two). "
                f"{key} is not a valid index."
            )

    def __contains__(self, key: Any) -> bool:
        """Determine whether there is data mapped under the given key.

        Note: if a tuple containing both an int ID and a string name are given,
        then the name must map to the given ID (and the given ID must map to
        data, which it should if __setitem__ and __delitem__ are working properly).
        """
        if isinstance(key, int):
            return key in self.id_dict.keys()
        if isinstance(key, str):
            return (
                # Name is registered:
                key in self.name_id_mapping.keys()
                # The name's corresponding ID maps to data (it always should):
                and self.name_id_mapping[key] in self.id_dict.keys()
            )
        if self.is_valid_key_tuple(key):
            return (
                # Name is registered:
                key[1] in self.name_id_mapping.keys()
                # The name's corresponding ID matches the given ID:
                and self.name_id_mapping[key[1]] == key[0]
                # The name's corresponding ID maps to data (it always should):
                and key[0] in self.id_dict.keys()
            )
        if self.is_valid_key_enumerator(key):
            key = cast(NameIdDict.KeyEnumeratorTuple, key)
            names, ID = key
            return (
                # All names are registered:
                all([name in self.name_id_mapping.keys() for name in names])
                # The corresponding ID of all names matches the given ID:
                and all([self.name_id_mapping[name] == ID for name in names])
                # The name's corresponding ID maps to data (it always should):
                and ID in self.id_dict.keys()
            )
        return False

    def __len__(self) -> int:
        """
        Return number of pieces of mapped [VT] data with unique IDs.

        Note: there may be more names stored than this since multiple names can
        map to the same ID *but*, in order to coincide with number of entries
        which are visited by `__iter__`, this only returns the number of pieces
        of mapped data with unique IDs (since the iterator ensures each is only
        visited once).
        """
        return len(self.id_dict)

    def fast_items(self) -> Iterator[Tuple[NameIdDict.KeyTuple, VT]]:
        """
        Create an iterator which will loop through each piece of stored data [VT] once.

        Slightly faster than items itself since fewer checks need to be
        done when getting items (we already know all keys are valid).

        Iterator's __next__ returns a tuple pairing an (id and a name) to a
        piece of data.

        Output will be sorted by ID, not in insertion order.

        If multiple names map to the same ID, only one name (likely the
        *latest* entry) will be visited to ensure that each piece of mapped
        data is only visited once.

        Use `collect()` if you want every name for each ID to be included in the output.

        """
        # Guards:
        if self.__len__() == 0:
            return iter([])

        # Sort ids first to speed up matching with names:
        sorted_id_items = sorted(self.id_dict.items(), key=lambda x: x[0])
        ids, vals = zip(*sorted_id_items)

        # Flip names list:
        flipped_names = [(y, x) for x, y in self.name_id_mapping.items()]
        # Keep only one name entry for each ID (only one name per ID):
        names_unique_ids = dict(flipped_names).items()
        # Sort names by corresponding ID:
        sorted_names = sorted(names_unique_ids, key=lambda x: x[0])
        # Extract just the names (strip off the IDs from the tuples):
        names = [n for _, n in sorted_names]

        # Build the iterator which returns entries of the form ((id,name), value)
        return iter(zip(zip(ids, names), vals))

    def __iter__(self) -> Iterator[NameIdDict.KeyTuple]:
        """
        Create an iterator which will return keys that will map to each piece of stored data [VT] once.

        Iterator's __next__ returns an (ID, name) tuple for each unique *ID*.

        Output will be sorted by ID, not in insertion order.

        If multiple names map to the same ID, only one name (likely the
        *latest* entry) will be visited to ensure that each piece of mapped
        data is only visited once if these are used to fetch data.
        """
        # Guards:
        if self.__len__() == 0:
            return iter([])

        # Sort ids first to speed up matching with names:
        ids = sorted(self.id_dict.keys())

        # Flip names list:
        flipped_names = [(y, x) for x, y in self.name_id_mapping.items()]
        # Keep only one name entry for each ID (only one name per ID):
        names_unique_ids = dict(flipped_names).items()
        # Sort names by corresponding ID:
        sorted_names = sorted(names_unique_ids, key=lambda x: x[0])
        # Extract just the names (strip off the IDs from the tuples):
        names = [n for _, n in sorted_names]

        # Build the iterator which returns entries of the form ((id,name), value)
        return iter(zip(ids, names))

    def update(self, other=(), /, **kwargs) -> None:
        """
        Include all data from the given dataset (other NameIdDict, mapping, or iterable).

        Ignore `kwargs` since the keys will be strings not Tuple[int,str].
        `kwargs` needs to stay in the signature to maintain compatability with
        MutableMapping.
        It's reasonable to consider that no user with an understanding of this
        class will try to feed in values via `kwargs` since string-keyed
        dictionaries are inherently incompatible with the fundamental idea of
        this class (where each entry has *both* string and integer keys).
        """
        if isinstance(other, NameIdDict):
            for ke, value in other.collect():
                self.__setitem__(ke, value)
        elif isinstance(other, abc.Mapping):
            for key in other:
                self.__setitem__(key, other[key])
        elif hasattr(other, 'keys'):
            for key in other.keys():
                self.__setitem__(key, other[key])
        else:
            for key, value in other:
                self.__setitem__(key, value)

    @property
    def names(self) -> List[str]:
        """Return all the unique names as a list (even if multiple names map to the same ID)."""
        return list(self.name_id_mapping.keys())

    @property
    def ids(self) -> List[int]:
        """Return all the unique IDs as a list."""
        return list(self.id_dict.keys())

    @property
    def vals(self) -> List[VT]:
        """Return all [VT] data values as a list. If multiple unique IDs have the same value, values will be duplicated."""
        return list(self.id_dict.values())

    @property
    def key_enumerators(self) -> List[NameIdDict.KeyEnumeratorTuple]:
        """
        Somewhat inefficient but consistent way of getting all KeyEnumerators.

        Really just a shorthand.
        Importantly, this method is guaranteed to be consistent with the 
        ordering (sorted) of the output of `collect()` (since it just uses that 
        output).
        """
        return [ke for ke, _ in self.collect()]

    def collect(self) -> List[Tuple[KeyEnumeratorTuple, VT]]:
        """
        Collect all names which map to each registed ID pair and return as Tuple list alongside each registered ID's corresponding [VT] data.

        This implicitly returns a list of tuples containing all KeyEnumerators 
        and their matching values. This guarantee must be ensured for the sake 
        of using this as a possible serialization method.

        As a requirement, the returned list should be sorted by ID and each of the names lists should also be sorted using standard string comparison.
        """
        # Guards:
        if self.__len__() == 0:
            return []

        # Sort ids first to speed up matching with names:
        sorted_id_items = sorted(self.id_dict.items(), key=lambda x: x[0])
        ids, vals = zip(*sorted_id_items)

        # Sort names by corresponding ID:
        sorted_name_pairings = sorted(
            self.name_id_mapping.items(),
            key=lambda x: x[1]
        )

        # Collect ([name], ID) pairs:
        name_collections = []

        # Loop through all names and IDs (note both lists are sorted by ID and ids is guaranteed to be unique):
        ID_idx = 0
        id_names: List[str] = []
        for n, nID in sorted_name_pairings:
            if nID == ids[ID_idx]:
                # If the name's ID matches the current candidate ID, collect it:
                id_names.append(n)
            elif len(id_names) == 0:
                # If no names have matched this candidate ID including the current one, none will. This is wrong:
                raise ValueError(f"No names map to ID {ids[ID_idx]}.")
            elif ID_idx < len(ids)-1:
                # Add all names which correspond with this candidate ID to the collection:
                name_collections.append(sorted(id_names))
                # Reset the collection and advance to the next candidate ID:
                id_names = []
                ID_idx += 1
                # Start new names collection. See if this name goes there before tossing:
                if nID == ids[ID_idx]:
                    # If the name's ID matches the current candidate ID, collect it:
                    id_names.append(n)
                else:
                    raise ValueError(
                        f"Name {n} doesn't map to any existing ID. "
                        f"Name {n} maps to {nID} which isn't a registered ID."
                    )
            else:
                # If we've run out of IDs before we've run out of names, there's an issue somewhere.
                raise StopIteration(
                    "Ran out of IDs before all names could be matched with an ID. "
                    f"Failed at name {n} -> ID {nID} (this name could not be matched). "
                    "Check that *all* names map to IDs that exist (this is the likely culprit)."
                )

        # Append names collection for final ID:
        name_collections.append(sorted(id_names))

        return list(zip(zip(name_collections, ids), vals))

    def __str__(self) -> str:
        return cast(str, self._printer.pformat(self.collect()))

    def __repr__(self) -> str:
        return str(self.collect())

    def __eq__(self, other) -> bool:
        """ Check if both of the given NameIdDict objects have all the same names and IDs mapping to equal values."""
        if not (isinstance(self, NameIdDict) and isinstance(other, NameIdDict)):
            return False

        return (
            self.id_dict == other.id_dict
            and self.name_id_mapping == other.name_id_mapping
        )

    def __getstate__(self) -> List[Dict[str, Union[List[str], int, VT]]]:
        """
        Return serializable data for `pickle` (or `jsonpickle`).

        Simply a dict containing the two core dicts `id_dict` and 
        `name_id_mapping` could work but the output from `collect()` is used 
        instead so that this data is more ordered and intelligible when it's 
        loaded in another language which doesn't have these classes or read by a 
        human.
        """
        data = self.collect()
        serializable_data: List[Dict[str, Union[List[str], int, VT]]] = [
            {'names': names, 'ID': ID, 'val': val} for (names, ID), val in data
        ]
        return serializable_data

    def __setstate__(self, state: List[Dict[str, Union[List[str], int, VT]]]) -> None:
        # Check that the given state has the correct type:
        if not isinstance(state, list):
            raise TypeError(
                f"State given to deserializer `__setstate__` should be a list. Instead it is a {type(state)}."
            )

        # Clean out and setup this object:
        self.id_dict = {}
        self.name_id_mapping = {}

        for entry in state:
            # Check the formatting and type of each dict entry:
            if not isinstance(entry, dict):
                raise TypeError(
                    "An entry in state list given to deserializer `__setstate__` "
                    "isn't a dict. `state` should be a list containing only "
                    f"dicts but instead one entry was found to be: {entry}"
                )
            if not entry.keys() >= {'names', 'ID', 'val'}:
                raise ValueError(
                    "An entry in state list given to deserializer `__setstate__` "
                    "doesn't have the proper dictionary keys. "
                    f"'names', 'ID', and 'value' are required but {entry.keys()} were found."
                )
            if not (
                isinstance(entry['names'], list)
                and all([isinstance(n, str) for n in entry['names']])
            ):
                raise TypeError(
                    "An entry in state list given to deserializer `__setstate__` "
                    "has an improper 'names' entry. 'names' should be a list "
                    f"containing only strings but instead is: {entry['names']}"
                )
            if not isinstance(entry['ID'], int):
                raise TypeError(
                    "An entry in state list given to deserializer `__setstate__` "
                    "has an improper 'names' entry. 'names' should be an int "
                    f"but instead is: {entry['ID']}"
                )

            # Load entry:
            self[(entry['names'], entry['ID'])] = cast(VT, entry['val'])
