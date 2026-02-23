#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""
Defines convenient containers for `Payload`s of multiple classes.

@author: Connor W. Colombo (CMU)
@last-updated: 05/06/2022
"""
# Activate postponed annotations (for using classes as return type in their own methods)
from __future__ import annotations

from typing import Any, Optional, Iterable, Final, Iterator, List, Tuple, Type, Dict, Union, NamedTuple, cast

from IrisBackendv3.codec.payload import *

import attr
from itertools import chain
from inspect import isabstract

from pprint import pformat
from collections import OrderedDict

# Ordered Map all subclasses of `Payload` (so they can be referenced by string
# OR position index if necessary):
# (a small amount of space saves lots of repeated computation later on):
PAYLOAD_SUBCLASS_MAP_NAME_TO_CLASS: Final[OrderedDict[str, Type[Payload]]] = \
    OrderedDict()


def catalog_payload_subclass(c: Type[Payload]):
    PAYLOAD_SUBCLASS_MAP_NAME_TO_CLASS[c.__name__] = c
    for sc in c.__subclasses__():
        catalog_payload_subclass(sc)


# Note on ignore: mypy has an unfixed bug where it doesn't allow abstract
# classes as arguments to functions using `Type` like this:
catalog_payload_subclass(Payload)  # type: ignore


@attr.define(slots=True)
class PayloadTreeNode:
    """
    Helper class for `EnhancedPayloadCollection` for mapping the hierarchy of
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
    """
    # which Payload (sub)class this node is for:
    payload_class: Type[Payload] = attr.field()
    # list of the `PayloadTreeNode`s corresponding to all subclasses of this class:
    subtree: List[PayloadTreeNode] = attr.field(factory=list)

    @payload_class.validator  # type: ignore
    def _check_payload_class(self, attr, payload_class) -> None:
        if not issubclass(payload_class, Payload) and payload_class != Payload:
            raise ValueError(
                "`payload_class` must be a subclass of `Payload` but it's not. "
                f"`{payload_class}` was given."
            )

    @subtree.validator  # type: ignore
    def _check_subtree(self, attr, subtree) -> None:
        if not isinstance(subtree, list):
            raise ValueError(
                "Payloads must be a list. "
                f"Instead `{subtree}` was given."
            )
        if any(not isinstance(node, PayloadTreeNode) for node in subtree):
            raise ValueError(
                "Payloads must be a list where all entries are a "
                "`PayloadTreeNode`. "
                f"Instead `{subtree}` was given."
            )

    @classmethod
    def build_class_tree(cls, c: Type[Payload]) -> PayloadTreeNode:
        """ Builds an empty `PayloadTreeNode` for the given
            `Payload` (sub)class.
        """
        return PayloadTreeNode(  # type: ignore
            c, [cls.build_class_tree(sc) for sc in c.__subclasses__()]
        )

    @classmethod
    def make_tree(cls) -> PayloadTreeNode:
        """ Builds an empty `PayloadTreeNode` for all `Payload`
            (sub)classes (root of tree is `Payload`).

            NOTE: It's actually faster (about 3x) to just run this every time
            you need a new empty tree than to deepcopy a precomputed and cached
            instance (46us vs 147us on the dev machine).
        """
        # Note on ignore: mypy has an unfixed bug where it doesn't allow
        # abstract classes as arguments to functions using `Type` like this:
        return cls.build_class_tree(Payload)  # type: ignore

    def __repr__(self) -> str:
        s = f"\n{self.payload_class.__name__} -> ["
        for node in self.subtree:
            s += "\n\t" + str(node).replace('\n', '\n\t')
        s += "\n]"
        return s

    def __getstate__(self) -> Tuple[str, List[PayloadTreeNode]]:
        return (
            self.payload_class.__name__,
            self.subtree
        )

    def __setstate__(self, state: Tuple[str, List[PayloadTreeNode]]) -> None:
        # using the map w/out KeyError impl. proves `state[0]` is a subclass of `Payload`:
        self.payload_class = PAYLOAD_SUBCLASS_MAP_NAME_TO_CLASS[state[0]]

        self._check_subtree(None, state[1])
        self.subtree = state[1]


# Build a tree of all `Payload` subclasses once:
PAYLOAD_CLASS_TREE: Final[PayloadTreeNode] = PayloadTreeNode.make_tree()
# Immediately map the tree (for quicker access to a node using a name).
# Only need to do this once per run since the program topology
# won't change (no Payload subclasses are being introduced during
# execution).
# This map is ordered so they Payload classes can be referenced by string
# OR position index if necessary, though only the former should be used for
# serialization since the addition of a new Payload subclass between
# serialization time and deserialization time could break the mapping between
# `Payload` class and index.
# (a small amount of space saves lots of repeated computation later on):
PAYLOAD_CLASS_TREE_MAP: Final[OrderedDict[str, PayloadTreeNode]] = \
    OrderedDict()


def map_tree_node(root: PayloadTreeNode) -> None:
    # Create a map to each node in the tree starting at `root` (for faster
    # access to each node, using a key).
    PAYLOAD_CLASS_TREE_MAP[root.payload_class.__name__] = root
    for node in root.subtree:
        map_tree_node(node)


map_tree_node(PAYLOAD_CLASS_TREE)


# Preconstruct a list of concrete (instantiatable) Payload classes:
# These are the only ones it makes sense to maintain a list for.
# These will serve as the keys for `_payloads_dict` in `EnhancedPayloadCollection`:
CONCRETE_PAYLOAD_CLASSES: Final[List[str]] = []


def populate_cpc_list(root: PayloadTreeNode) -> None:
    # Add to a list the name of every concrete class in the tree starting at
    # `root`:
    if not isabstract(root.payload_class):
        # only maintain lists for instantiatable `Payload` classes:
        CONCRETE_PAYLOAD_CLASSES.append(root.payload_class.__name__)
    for node in root.subtree:
        populate_cpc_list(node)


populate_cpc_list(PAYLOAD_CLASS_TREE)


# Allowable types used to index an EnhancedPayloadCollection.
# See EnhancedPayloadCollection.key_to_name for conversion details.
EnhancedPayloadCollection_KeyType = Union[str, Type[Payload], int]
EPC_KT = EnhancedPayloadCollection_KeyType  # alias for the above


class BadEnhancedPayloadCollectionKeyError(AttributeError):
    func_name: str
    key: EPC_KT
    name: str
    tree_map: Dict[str, PayloadTreeNode]

    def __init__(
        self,
        func_name: str,
        key: EPC_KT,
        name: Optional[str]
    ) -> None:
        """
        Creates an `Error` for a bad `key` given to the function `func_name` of
        `EnhancedPayloadCollection`.
        """
        self.func_name = func_name
        self.key = key
        self.name = str(name)
        super().__init__(
            f"`EnhancedPayloadCollection.{func_name}` must be given a "
            "`Payload` subclass, or the name of a `Payload` subclass, or "
            "the index of the a `Payload` subclass in `CONCRETE_PAYLOAD_CLASSES="
            f"{[(i,k) for i,k in enumerate(CONCRETE_PAYLOAD_CLASSES)]}`. "
            "The corresponding `Payload` class `name` recovered "
            f"(`name={name}`) must be in "
            f"`PAYLOAD_CLASS_TREE_MAP={[*PAYLOAD_CLASS_TREE_MAP.keys()]}`. "
            f"Instead `key={key}` was given, corresponding to `name={name}`."
        )


class EnhancedPayloadCollection(Iterable[List[Payload]]):
    """
    A container for `Payload`s that allows subsets of it to be queried by
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
    """

    __slots__: List[str] = ['_payloads_dict']

    _payloads_dict: Dict[str, List[Payload]]

    def __init__(self, **kwargs) -> None:
        # Create a new dict for all `CONCRETE_PAYLOAD_CLASSES`
        # In testing, this was the fastest option between:
        # - building a new one with (6.6us avg.)
        # - deepcopying a prebuilt dict (9.8us avg.)
        # - list comprehensioning just the keys of a prebuilt dict (1.0us avg.)
        # * building a dict using prebuilt list of valid keys (0.93us avg.)
        self._payloads_dict = dict((c, []) for c in CONCRETE_PAYLOAD_CLASSES)

        # Go through each kwarg and add it to the tree:
        for payload_class, payload_list in kwargs.items():
            # Treat every kwarg as an indexing operation (dispatch to
            # `__setitem__`):
            self[payload_class] = payload_list

    @property
    def all_payloads(self) -> Iterable[Payload]:
        """ Returns an iterable of all payloads in the payload collection,
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
        """
        all_payloads_lists = [pl for pl in self._payloads_dict.values()]
        return chain.from_iterable(all_payloads_lists)

    @property
    def num_payloads(self) -> int:
        """The total number of `Payload`s in the `EnhancedPayloadCollection`."""
        return len([*self.all_payloads])

    def is_empty(self) -> bool:
        """ Returns whether all Payloads in the Payload Collection are empty.
        Included for backwards compatibility with `PayloadCollection`.
        """
        return self.num_payloads == 0

    @classmethod
    def make_empty(cls) -> EnhancedPayloadCollection:
        """ Makes a new empty `EnhancedPayloadCollection` (just
        `EnhancedPayloadCollection()`).

        This function only exists to preserve backwards-compatibility with code
        that used the `namedtuple`-based predecessor `PayloadCollection`.
        """
        return EnhancedPayloadCollection()

    @staticmethod
    def key_to_name(key: EPC_KT) -> Optional[str]:
        """ Converts a given `key` used to identify a `Payload` subclass by
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
        """
        # Convert any valid input types to a string of the `Payload` subclass
        # name:
        name: Optional[str] = None
        if isinstance(key, type):
            name = key.__name__
        elif isinstance(key, str):
            name = key
        elif isinstance(key, int):
            name = CONCRETE_PAYLOAD_CLASSES[key]
        elif isinstance(key, float):
            name = CONCRETE_PAYLOAD_CLASSES[int(key)]

        return name

    def extend(self, val: Union[EnhancedPayloadCollection, Iterable[Payload]]) -> None:
        """
        Appends each value in the given iterable to the appropriate
        list in `_payloads_dict` (the one corresponding to its `__class__`).

        If `val` is another `EnhancedPayloadCollection`, then type checking and
        sorting steps will be skipped and all core lists in `val` will be
        appended to the lists in this collection.

        See `append` for more details.
        """
        if isinstance(val, EnhancedPayloadCollection):
            # Just append the entries in all lists in `val` to the corresponding
            # lists in `self` if `val` is also an `EnhancedPayloadCollection`:
            for key in val._payloads_dict.keys():
                self._payloads_dict[key].extend(val._payloads_dict[key])
        else:
            # Make sure `val` is iterable:
            if not isinstance(val, Iterable):
                raise ValueError(
                    "In `EnhancedPayloadCollection.extend`, `val` needs to be an "
                    f"`Iterable` but got: `val = {val}`."
                )

            for payload in val:
                # cast b/c mypy doesn't realize type is Payload not Union[List[Payload], Payload] here
                # actual `isinstance(..., Payload)` checking happens in `append`
                self.append(cast(Payload, payload))

    def append(self, payload: Payload):
        """
        Appends the given `Payload` to the appropriate list in `_payloads_dict`,
        (the one corresponding to its `__class__`).
        """
        # Make sure `Payload` belongs to a subclass of **this** `Payload` (and
        # not just some class from another namespace with the same name as a
        # concrete `Payload` subclass):
        if not isinstance(payload, Payload):
            raise ValueError(
                "In `EnhancedPayloadCollection.append`, a payload is being "
                "appended. `payload` must be an instance of a `Payload` subclass "
                f"but got: `payload={payload}` with type `{type(payload)}`."
            )

        # Make sure `payload` is an instance of a non-abstract `Payload`
        # subclass (i.e. has a key in _payloads_dict):
        if payload.__class__.__name__ not in CONCRETE_PAYLOAD_CLASSES:
            raise ValueError(
                "In `EnhancedPayloadCollection.append`, a payload is being "
                "appended. `payload` must be an instance of an instantiatable "
                f"`Payload` subclass (any of {CONCRETE_PAYLOAD_CLASSES}) "
                f"but got: `payload={payload}` with type `{type(payload)}`."
            )

        # Add `payload` to the appropriate list in `_payloads_dict`:
        self._payloads_dict[payload.__class__.__name__].append(payload)

    def remove(self, payload: Payload):
        """
        Removes the given `Payload` to the appropriate list in `_payloads_dict`,
        (the one corresponding to its `__class__`).

        NOTE: This will remove at most one instance. So, if there are multiple
        identical payloads, only one will have been removed.
        """
        # Make sure `payload` is an instance of a non-abstract `Payload`
        # subclass (i.e. has a key in _payloads_dict):
        if payload.__class__.__name__ not in CONCRETE_PAYLOAD_CLASSES:
            raise ValueError(
                "In `EnhancedPayloadCollection.remove`, a payload is being "
                "removed. Payload must be an instance of an instantiatable "
                f"`Payload` subclass (any of {CONCRETE_PAYLOAD_CLASSES}) "
                f"but got: `payload={payload}` with type `{type(payload)}`."
            )

        # Add `payload` to the appropriate PayloadTreeNode in the tree:
        self._payloads_dict[payload.__class__.__name__].remove(payload)

    def erase(self, key: EPC_KT) -> None:
        """
        Removes all payloads associated with the `Payload` subclass of the
        given `key` (removes everything in the list in `_payloads_dict`
        corresponding to the class indicated by `key` (if it's a concrete class)
        as well as the lists of all of its concrete subclasses).
        """
        # Clear values in all down-tree `direct_payloads`:
        name: Optional[str] = self.key_to_name(key)
        if name is not None and name in PAYLOAD_CLASS_TREE_MAP:
            # if name is a valid Payload class
            if name in self._payloads_dict:
                # if class is concrete and has instances, erase its list:
                self._payloads_dict[name] = []

            # Erase the lists corresponding to all subclasses of this class:
            for subnode in PAYLOAD_CLASS_TREE_MAP[name].subtree:
                self.erase(subnode.payload_class)
        else:
            raise BadEnhancedPayloadCollectionKeyError('erase', key, name)

    def clear(self) -> None:
        """Removes all `Payload`s from every list in `_payloads_dict`."""
        for key in self._payloads_dict.keys():
            self._payloads_dict[key] = []

    def _collect_all_payload_lists(
        self,
        accumulator: List[List[Payload]],
        node: PayloadTreeNode
    ) -> List[List[Payload]]:
        """ Helper function for `__getattr__` that builds a list containing the
        `Payload` lists in `_payloads_dict` for the `Payload` class
        corresponding to the given `node` (if its class is concrete and thus
        has a list) and the lists of all of its concrete subclasses."""

        if node.payload_class.__name__ in self._payloads_dict:
            # if class is concrete and has instances, add it to the list:
            accumulator.append(
                self._payloads_dict[node.payload_class.__name__])

        for subnode in node.subtree:
            self._collect_all_payload_lists(accumulator, subnode)

        return accumulator

    def __getattr__(self, key: EPC_KT) -> Iterable[Payload]:
        """
        Returns an iterable of all payloads with the `Payload` class indicated
        by `key` or any of its subclasses. 

        NOTE: the return value is just an iterable (chain) so it can't be
        modified like a list (append, extend, etc.) **BUT** modifying the
        `Payloads` themselves (adding metadata, etc). will behave as expected.

        For example, calling with `key=CommandPayload` would give all instances
        of `CommandPayload` and `WatchdogCommandPayload` that are in the tree;
        whereas, calling with `key=WatchdogCommandPayload` would just return
        instances of `WatchdogCommandPayload`.

        This is being done as an attribute get and not just subscripting in
        order to support (at least) the same interface as the predecessor
        `PayloadCollection`.

        The given `key` must be a valid `EnhancedPayloadCollection_KeyType`.

        See `key_to_name` for what counts as a `EnhancedPayloadCollection_KeyType`.
        In addition, to be valid, the `name` must be in `PAYLOAD_CLASS_TREE_MAP`.
        If it's not valid, `AttributeError` is raised.
        """
        name: Optional[str] = self.key_to_name(key)
        if name is not None and name in PAYLOAD_CLASS_TREE_MAP:
            # if name is a valid Payload class

            # build a list of all payloads lists for this class (if concrete)
            # and any of its concrete subclasses (i.e. those w/ payloads lists):
            all_payloads_lists = self._collect_all_payload_lists(
                [],
                PAYLOAD_CLASS_TREE_MAP[name]
            )

            return chain.from_iterable(all_payloads_lists)

        # If we're here that means we haven't been given a valid input that's
        # in the map of Payload subclasses.
        raise BadEnhancedPayloadCollectionKeyError('__getattr__', key, name)

    def __getitem__(self, key: EPC_KT) -> Iterable[Payload]:
        """ Returns all payloads with the given `Payload` class indicated
        by `key` or any of its subclasses.

        Makes subscripting identical to addressing an attribute.
        See `__getattr__` for more details.
        """
        try:
            return self.__getattr__(key)
        except AttributeError as e:
            # re-cast AttributeError of __getattr__ as an IndexError
            # (expected from __getitem__)
            raise IndexError(
                "The following error occurred while calling `__getattr__` "
                f"from `__getitem__`: {e}"
            )

    def __setattr__(self, key: EPC_KT, val: Any) -> None:
        """
        Sets an attribute.

        1. If the `key` is a valid `EnhancedPayloadCollection_KeyType` (it
        identifies a `Payload` subclass) and `val` is an `Iterable`, the list
        for the `key` `Payload` class and the lists for all its concrete
        subclasses will get cleared and then all entries in `val` will get 
        added to the appropriate list in `_payloads_dict` (the list
        corresponding to each entry's `__class__`).

        See `key_to_name` for what counts as a `EnhancedPayloadCollection_KeyType`.
        In addition, to be valid, the `name` must be in `PAYLOAD_CLASS_TREE_MAP`.

        NOTE: To clarify the above, normally when you call
        `object.attribute = x`, the effect is that whatever was previously in
        `object.attribute` is thrown out and replaced by `x`.
        To be consistent with this expectation, a subtle feature has to be
        implemented:
        When setting a field like `epc.CommandPayload` to a new list (e.g. 
        `epc.CommandPayload = [p]`), not only will the list for `CommandPayload`
        in the `_payloads_dict` be cleared but so will the list corresponding
        to every subclass of the given class (in this case, the list of
        `WatchdogCommandPayload` will be set too).

        NOTE: Before setting anything, this will make sure `val` is a list and
        that each value in the list is an instance of the given type `key`
        **BUT** still lookup the appropriate `_payloads_dict` list to add to.
        For example, if you do:
        ```
        epc.CommandPayload = [CommandPayload, WatchdogCommandPayload, ...]
        ```
        Each entry will be checked to make sure it's a `CommandPayload` (as
        requested) **BUT** only the first entry will get added to the list for 
        `CommandPayload`; the second entry will get added to the the (more
        specific) `WatchdogCommandPayload` subclass list.
        A consequence of this is that the following **wouldn't** work:
        ```
        epc.CommandPayload = [TelemetryPayload, CommandPayload, ...]
        ```
        b/c even though all entries in `val` are valid `Payloads`, they don't
        all belong to subclasses of `key`.
        If you want to do something like the above, just do:
        ```
        epc.extend([TelemetryPayload, CommandPayload, ...])
        ```

        2. Since this function gets called for **ALL** attr lookups (unlike
        `__getattr__` which is only called for unknown attr lookups), if the
        attr is **NOT** a valid `EnhancedPayloadCollection_KeyType`, the
        superclass's `__setattr__` (normal attr setting) will get called to do
        the job.
        """
        # Convert any valid input types to a string of the `Payload` subclass
        # name:
        name: Optional[str] = self.key_to_name(key)

        if name is not None and name in PAYLOAD_CLASS_TREE_MAP:
            # If name is a valid `Payload` subclass, set it in `_payloads_dict`:

            # Make sure `val` is iterable:
            if not isinstance(val, Iterable):
                raise ValueError(
                    "In `EnhancedPayloadCollection`, a `Payload` subclass "
                    f"collection is being set for `key={key}` -> `name={name}`. "
                    "In this case, `val` must be an `Iterable` but got: "
                    f"`val={val}` with `type={type(val)}`."
                )

            # Grab the class being referenced:
            payload_subclass = PAYLOAD_CLASS_TREE_MAP[name].payload_class

            # Erase all payloads associated with this `key` (so that it can be
            # replaced):
            self.erase(name)

            # Add each `payload` in `val` to the tree:
            contains_nonconforming_payload: bool = False
            for payload in val:
                # Make sure `payload` is an instance of (subclass of) the given
                # `payload_subclass`:
                if isinstance(payload, payload_subclass):
                    # Add the payload to the appropriate list in
                    # `_payloads_dict`:
                    self.append(payload)
                else:
                    # if not, keep adding payloads to the collection (don't
                    # raise error yet) but set a flag so we can raise an error
                    # after all conforming payloads have safely been added:
                    contains_nonconforming_payload = True

            if contains_nonconforming_payload:
                raise ValueError(
                    "In `EnhancedPayloadCollection`, a `Payload` subclass "
                    f"collection is being set for `key={key}` -> `name={name}`. "
                    "In this case, `val` must be an `Iterable` where every entry "
                    f"is an instance of `{payload_subclass}` (or a subclass) "
                    f"but got a `val` containing (entry's type, entry): "
                    f"`{[(f'type: {type(p)}', str(p)) for p in val]}`. "
                    "NOTE: All conforming payloads in `val` were added to the "
                    "collection before this error was raised."
                )
        else:
            # Otherwise, it's not meant for setting a `Payload` class attr;
            # so, just call a normal setter:
            super().__setattr__(str(key), val)

    def __setitem__(self, key: EPC_KT, val: Any) -> None:
        """ Sets an attribute. Makes subscripting identical to addressing an
        attribute (that is `epc['x']=y` is the same as `epc.x = y`).
        See `__setattr__` for more details.
        """
        try:
            self.__setattr__(key, val)
        except AttributeError as e:
            # re-cast AttributeError of __setattr__ as an IndexError
            # (expected from __getitem__)
            raise IndexError(
                "The following error occurred while calling `__setattr__` "
                f"from `__setitem__`: {e}"
            )

    def __delattr__(self, item: EPC_KT) -> None:
        """ Performs `erase` if `item` is a valid "
        EnhancedPayloadCollection_KeyType (it identifies a `Payload` subclass).
        If not, nothing happens.

        See `erase` for more details.
        """
        try:
            self.erase(item)
        except AttributeError:
            # just do nothing. `AttributeError` just means it wasn't a valid
            # key and isn't erasable:
            pass

    def __delitem__(self, item: EPC_KT) -> None:
        """ Makes `__delitem__` equivalent to `__delattr__` (performs `erase`
        if valid). That is, makes `del epc['x']` equivalent to `del epc.x`.

        See `__delattr__` for more details.
        """
        self.__delattr__(item)

    def __getstate__(self) -> Dict[str, List[Payload]]:
        return self._payloads_dict

    def __setstate__(self, state: Dict[str, List[Payload]]) -> None:
        if not isinstance(state, dict):
            raise ValueError(
                "In `EnhancedPayloadCollection.__setstate__`, `state` must be "
                f"a `dict` but `state={state}` was given "
                f"with type `{type(state)}`."
            )
        # Make sure every key is a valid `CONCRETE_PAYLOAD_CLASSES`:
        if any(c not in CONCRETE_PAYLOAD_CLASSES for c in state.keys()):
            raise ValueError(
                "In `EnhancedPayloadCollection.__setstate__`, `state` must be "
                "a `dict` with keys that are the names of concrete "
                "(instantiatable) `Payload` classes "
                f"(`{CONCRETE_PAYLOAD_CLASSES}`) "
                f"but `state={pformat(state, indent=4)}` was given "
                f"with type `{type(state)}`."
                "\nHas a `Payload` class been deprecated, removed, or renamed "
                "since this data was serialized? Or, was this data serialized "
                "with a newer version of the codebase which has more or "
                "differently named `Payload` classes?"
            )

        self._payloads_dict = state

        # Make sure every entry in `CONCRETE_PAYLOAD_CLASSES` is in the given
        # `state`. Any missing entries are added. This covers the case where new
        # `Payload` subclasses have been added since this data was serialized:
        for c in CONCRETE_PAYLOAD_CLASSES:
            if c not in self._payloads_dict:
                self._payloads_dict[c] = []

    def __eq__(self, other: object) -> bool:
        if not isinstance(other, EnhancedPayloadCollection):
            return False

        return self.__getstate__() == other.__getstate__()

    def __contains__(self, item: Union[EPC_KT, Payload]) -> bool:
        """
        Checks if this `EnhancedPayloadCollection` contains the given `item`
        (used with the `in` operator as a containment check).

        This works in two ways:

        1. If `item` is a `Payload`, this checks if it's contained in any of the
        lists in `_payloads_dict` (if the `item` is contained in this
        collection).

        2. If the `key` is a valid EnhancedPayloadCollection_KeyType (it
        identifies a `Payload` subclass), this will return whether the given 
        `item` can be used as a `key` for `__getattr__` to get a `Payload` list.
        That is, is it in `PAYLOAD_CLASS_TREE_MAP`. NOTE: This tests whether
        the given `item` is a mapped `Payload` subclass and not just that it's
        a concrete `Payload` subclass with an entry in `_payloads_dict`.
        """
        # 1. Check if `item` is contained in this collection:
        if isinstance(item, Payload):
            return item in self.all_payloads

        # 2. Check if `item` can be used as a `key` for this collection (the
        # `dict` sense of `in`):
        # Convert any valid input types to a string of the `Payload` subclass
        # name:
        name: Optional[str] = self.key_to_name(item)
        # Check if key is valid:
        return name is not None and name in PAYLOAD_CLASS_TREE_MAP

    def __str__(self) -> str: return pformat(self._payloads_dict, indent=4)
    def __repr__(self) -> str: return str(self._payloads_dict)

    def __len__(self) -> int:
        """ Returns the number of elements traversed when using `__iter__`
        (`len(CONCRETE_PAYLOAD_CLASSES)`).

        This also must be equivalent to the maximum index you can give to
        `__getattr__` + 1 in order to support the following scheme which was
        used in the namedtuple-based predecessor `PayloadCollection`:
        ```
        for i in range(len(epc)):
            for payload in epc[i]:
                ...
        ```
        in which iterating over `PayloadCollection` would iterate over all lists
        in the collection.

        If you want the number of unique `payloads`, use `num_payloads` instead.
        """
        return len(CONCRETE_PAYLOAD_CLASSES)

    def __iter__(self) -> Iterator[List[Payload]]:
        """ Iterates over the lists of `Payloads` for each concrete `Payload`
        subclass.

        That is, each `__next__` call on the iterator returns a list of payloads
        with homogeneous type. All unique concrete `Payload` types will be
        iterated over exactly once.

        NOTE: To be backwards compatible with the namedtuple-based predecessor
        `PayloadCollection`, this must iterate over unique addressable
        collections and **NOT** the payloads themselves.

        If you want to iterate over all the payloads themselves, use:
        ```
        for payload in epc.all_payloads:
            ...
        ```
        """
        return (pl for pl in self._payloads_dict.values())


class PayloadCollection(NamedTuple):
    """
    [[DEPRECATED]] Use `EnhancedPayloadCollection` instead.
    Dictionary Collection of Lists of Payloads, separated by Payload Type.
    *NOTE:* All keys are strings and should accessed using class.__name__

    This means that, if class names are changed, these strings will have to be
    MANUALLY updated. This should be relatively easy since, although the linter
    won't detect the member name as outdated, it will detect the class name in
    the `List[]` type hint as outdated and all the developer has to do is make
    sure the two are the same.
    """
    CommandPayload: List[CommandPayload]
    TelemetryPayload: List[TelemetryPayload]
    EventPayload: List[EventPayload]
    FileBlockPayload: List[FileBlockPayload]

    def is_empty(self) -> bool:
        """Returns whether all Payloads in the Payload Collection are empty."""
        return all(len(cast(list, x)) == 0 for x in self)

    @classmethod
    def make_empty(cls) -> PayloadCollection:
        """Makes a new empty payload collection."""
        return cls(
            CommandPayload=[],
            TelemetryPayload=[],
            EventPayload=[],
            FileBlockPayload=[]
        )


def extract_downlinked_payloads(
        VLP: bytes,
        endianness_code: str = ENDIANNESS_CODE
) -> EnhancedPayloadCollection:
    """Extract all Payloads from a Variable Length Payload to produce a
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
    """
    # Dispatch table to map extracted magics to the corresponding payloads classes:
    payload_magic_dispatch_table: Dict[Magic, Type[Payload]] = {
        Magic.COMMAND: CommandPayload,
        Magic.RADIO_COMMAND: CommandPayload,
        Magic.WATCHDOG_COMMAND: WatchdogCommandPayload,
        Magic.TELEMETRY: TelemetryPayload,
        Magic.EVENT: EventPayload,
        Magic.FILE: FileBlockPayload
    }

    # Collection of Payload Lists:
    payloads: EnhancedPayloadCollection = EnhancedPayloadCollection()

    # Unpack Variable Length Payload:
    while len(VLP) > 0:
        # Strip off the magic:
        magic_bytes, VLP = VLP[:MAGIC_SIZE], VLP[MAGIC_SIZE:]
        magic = Magic.decode(magic_bytes, byte_order=endianness_code)

        if magic == Magic.MISSING:
            raise PacketDecodingException(
                magic_bytes,
                "This sequence of bytes corresponds to a missing magic value."
            )

        # Extract the payload and strip its bytes off the VLP:
        payload_type = payload_magic_dispatch_table[magic]

        payload, VLP = payload_type.process(VLP, endianness_code)

        # Make sure there's data there:
        if len(payload.raw) == 0:
            raise PacketDecodingException(
                VLP,
                "Failed to extract any data from this VLP with magic type "
                f"{magic}. Was the wrong magic used?"
            )

        # Add on metadata:
        payload.magic = magic

        # Note: mypy complains that `logger` doesn't have a member `spam` but it does:
        logger.spam(f"Successfully extracted {payload}")  # type: ignore

        # Add payload to collection:
        payloads.append(payload)

    return payloads
