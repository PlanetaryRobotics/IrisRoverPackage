# -*- coding: utf-8 -*-
"""
Test all possible NameIdDict use cases.

It's important for these tests to be rigorous since this class forms the 
backbone of the Backend DataStandards.

@author: Connor W. Colombo (CMU)
@last-updated: 01/18/2021
"""
import pytest
from typing import Any, List, Union, Tuple

from IrisBackendv3.utils import NameIdDict


@pytest.fixture(scope="class")
def suite_setup(request):
    # Seed values for testing (datasets which tests can start with):
    # This setup must be performed in such a way that (by default)
    # the list of ids, names, and values exactly represent what's returned by
    # `self.commands.ids`, `self.commands.names`, and `self.commands.vals`
    # (So there shouldn't be multiple names mapping to any id by default. The
    # individual test suites can change that as needed.)
    request.cls.ids: List[int] = [0x03, 0x01, 0x02]
    request.cls.names: List[str] = ['reset', 'mover', 'image']
    request.cls.vals: List[str] = [
        'Reset Rover.', 'Do Drive.', 'Incite Image.']
    data = zip(zip(request.cls.ids, request.cls.names), request.cls.vals)
    request.cls.commands: NameIdDict[str] = NameIdDict(data)


@pytest.mark.usefixtures('suite_setup')
class ConstructorSuite(object):
    """Verify all construction techniques work."""

    @pytest.fixture(autouse=True)
    def _setup(self):
        """Common setup. Performed once per test."""
        # Make a copy:
        self.copy_commands: NameIdDict[str] = NameIdDict(self.commands)

        # Ensure there is at least one ID with multiple unique names
        # pointing to it (to ensure all names are removed when deleting
        # any reference to the data):
        new_name = "".join(self.copy_commands.names)
        first_val = self.copy_commands[self.ids[0]]
        self.copy_commands[self.ids[0], new_name] = first_val
        self.copy_commands[self.ids[0], new_name + "2"] = first_val

    @staticmethod
    def _verify_content(nid: NameIdDict[str], ids: List[int], names: List[str], vals: List[str]) -> None:
        """
        Ensure the given NameIdDict (nid) has the given ids, names, and values in the given order.

        Also performs basic content sanity checks (proper name-ID mapping).
        """

        assert list(nid.id_dict.keys()) == ids, \
            "NameIdDict IDs set improperly."
        assert list(nid.id_dict.values()) == vals, \
            "NameIdDict values set improperly."
        assert list(nid.name_id_mapping.keys()) == names, \
            "NameIdDict names set improperly."

        name_ids = set(nid.name_id_mapping.values())
        core_ids = set(nid.id_dict.keys())

        assert len(name_ids - core_ids) == 0, \
            "NameIdDict contains names which map to IDs that don't exist."
        assert len(core_ids - name_ids) == 0, \
            "NameIdDict contains IDs which aren't mapped to by any names."

    def test_empty(self):
        """Construct new empty dict."""
        commands: NameIdDict[str] = NameIdDict()
        self._verify_content(commands, [], [], [])

    def test_list(self):
        """Construct new dict from tuple list."""
        data = zip(zip(self.ids, self.names), self.vals)
        commands: NameIdDict[str] = NameIdDict(data)
        self._verify_content(commands, self.ids, self.names, self.vals)

    def test_dict(self):
        """Construct new dict from dict."""
        data = zip(zip(self.ids, self.names), self.vals)
        data = dict(data)
        commands: NameIdDict[str] = NameIdDict(data)
        self._verify_content(commands, self.ids, self.names, self.vals)

    def test_copy(self):
        """Construct new NameIdDict by *copying* an existing one."""
        commands2: NameIdDict[str] = NameIdDict(self.copy_commands)
        # Should contain same data:
        assert commands2 == self.copy_commands, \
            "The second NameIdDict doesn't contain the same content as the first."

        # Should be a copy (not pointing to the same object):
        assert id(commands2) != id(self.copy_commands), \
            "NameIdDict constructor doesn't make a *copy*."

        # Same deal with the internal data structures:
        assert id(commands2.id_dict) != id(self.copy_commands.id_dict), \
            "NameIdDict constructor doesn't make a *copy* of `id_dict`."
        assert id(commands2.name_id_mapping) != id(self.copy_commands.name_id_mapping), \
            "NameIdDict constructor doesn't make a *copy* of `name_id_mapping`."

    def test_reconstruct_from_collect(self):
        """Construct a new NameIdDict using the output of collect (for serialization)."""
        collected = self.copy_commands.collect()
        rebuilt = NameIdDict(collected)

        # Should be contain same data:
        assert rebuilt == self.copy_commands, \
            "The second NameIdDict doesn't contain the same content as the first."


@pytest.mark.usefixtures('suite_setup')
class GetItemSuite(object):
    """Verify all __getitem__ techniques work."""

    def test_index_by_id(self):
        for idx, ID in enumerate(self.ids):
            assert self.commands[ID] == self.vals[idx], \
                f"Data value retrieved for the {idx}th ID ({ID}) doesn't match the {idx}th data value ({self.vals[idx]})."

    def test_index_by_name(self):
        for idx, name in enumerate(self.names):
            assert self.commands[name] == self.vals[idx], \
                f"Data value retrieved for the {idx}th name ({name}) doesn't match the {idx}th data value ({self.vals[idx]})."

    def test_index_by_tuple(self):
        for idx, (ID, name) in enumerate(zip(self.ids, self.names)):
            assert self.commands[(ID, name)] == self.vals[idx], \
                f"Data value retrieved for the {idx}th indexing tuple (({ID},{name})) doesn't match the {idx}th data value ({self.vals[idx]})."

    def test_index_by_tuple_no_paren(self):
        for idx, (ID, name) in enumerate(zip(self.ids, self.names)):
            assert self.commands[ID, name] == self.vals[idx], \
                f"Data value retrieved for the {idx}th indexing tuple (({ID},{name})) doesn't match the {idx}th data value ({self.vals[idx]})."

    def test_index_with_flipped_tuple(self):
        for idx, (ID, name) in enumerate(zip(self.ids, self.names)):
            with pytest.raises(TypeError):
                # misordered tuple (should be [int,str]) should raise TypeError:
                self.commands[name, ID]

    def test_index_with_invalid_type(self):
        with pytest.raises(TypeError):
            self.commands[3.4]

    def test_index_with_invalid_id(self):
        # Construct an ID not in object:
        bad_id = max(self.commands.ids) + 1
        with pytest.raises(KeyError):
            self.commands[bad_id]

    def test_index_with_invalid_name(self):
        # Construct a name not in object:
        # (need to add a char in case there's only one item and thus concat of all name strings is that solitary name):
        bad_name = ''.join(self.commands.names) + '_'
        with pytest.raises(KeyError):
            self.commands[bad_name]

    def test_index_with_invalid_tuple(self):
        # Construct a name and id which aren't in object:
        bad_id = max(self.commands.ids) + 1
        bad_name = ''.join(self.commands.names) + '_'

        # name and ID are registered but don't pair with each other:
        all_ids = set(self.commands.ids)
        all_names = set(self.commands.names)
        # Iterate over all registered names and IDs:
        for (names, ID), _ in self.commands.collect():
            # All registered names which don't map to this ID:
            invalid_names = list(all_names - set(names))
            for bn in invalid_names:
                with pytest.raises(KeyError):
                    self.commands[ID, bn]

            # All IDs which aren't mapped to by these names:
            invalid_ids = list(all_ids - {ID})
            for n in names:
                for bi in invalid_ids:
                    with pytest.raises(KeyError):
                        self.commands[bi, n]

        # ID doesn't exist:
        for name in all_names:
            with pytest.raises(KeyError):
                self.commands[bad_id, name]
        # name doesn't exist:
        for ID in all_ids:
            with pytest.raises(KeyError):
                self.commands[ID, bad_name]
        # name and ID don't exist:
        with pytest.raises(KeyError):
            self.commands[bad_id, bad_name]

    def test_key_enumerator(self):
        """Verify indexing by a KeyEnumerator."""
        # Get a bunch of Key Enumerators from `collect()`:
        collected = self.commands.collect()
        kes = [ke for ke, _ in collected]

        for ke in kes:
            _, ID = ke
            # Key Enumerator should return same thing as indexing by just ID
            # (and shouldn't raise any errors):
            assert self.commands[ke] == self.commands[ID], \
                f"Indexing by KeyEnumerator {ke} gave a different value than indexing by its ID {ID}."

    def test_key_enumerator_invalid_name(self):
        """Ensure this raises an error if a KeyEnumerator contains an invalid name."""
        # Make a copy:
        copy_commands = NameIdDict(self.commands)
        # Ensure there is at least one ID with multiple unique names
        # pointing to it (to ensure all names are removed when deleting
        # any reference to the data):
        new_name = "".join(copy_commands.names)
        last_val = copy_commands[self.ids[-1]]
        copy_commands[self.ids[-1], new_name] = last_val
        copy_commands[self.ids[-1], new_name + "2"] = last_val

        # Get a bunch of KeyEnumerators from `collect()`:
        collected = copy_commands.collect()
        kes = [ke for ke, _ in collected]
        # Modify the first KeyEnumerators to contain an invalid name:
        invalid_name = "".join(copy_commands.names)
        kes[0][0].append(invalid_name)

        # Ensure an error is raised for the modified KeyEnumerator:
        with pytest.raises(KeyError):
            copy_commands[kes[0]]

        # ... and only the modified KeyEnumerator
        for ke in kes[1:]:
            _, ID = ke
            # Key Enumerator should return same thing as indexing by just ID
            # (and shouldn't raise any errors):
            assert copy_commands[ke] == copy_commands[ID], \
                f"Indexing by KeyEnumerator {ke} gave a different value than indexing by its ID {ID}."

    def test_key_enumerator_invalid_id(self):
        """Ensure this raises an error if a KeyEnumerator contains an invalid ID."""
        # Make a copy:
        copy_commands = NameIdDict(self.commands)
        # Ensure there is at least one ID with multiple unique names
        # pointing to it (to ensure all names are removed when deleting
        # any reference to the data):
        new_name = "".join(copy_commands.names)
        last_val = copy_commands[self.ids[-1]]
        copy_commands[self.ids[-1], new_name] = last_val
        copy_commands[self.ids[-1], new_name + "2"] = last_val

        # Get a bunch of KeyEnumerators from `collect()`:
        collected = copy_commands.collect()
        kes = [ke for ke, _ in collected]
        # Modify the first KeyEnumerators to contain an invalid ID:
        first_names = kes[0][0]
        invalid_id = max(copy_commands.ids) + 1
        kes[0] = first_names, invalid_id

        # Ensure an error is raised for the modified KeyEnumerator:
        with pytest.raises(KeyError):
            copy_commands[kes[0]]

        # ... and only the modified KeyEnumerator
        for ke in kes[1:]:
            _, ID = ke
            # Key Enumerator should return same thing as indexing by just ID
            # (and shouldn't raise any errors):
            assert copy_commands[ke] == copy_commands[ID], \
                f"Indexing by KeyEnumerator {ke} gave a different value than indexing by its ID {ID}."

    def test_key_enumerator_incorrect_id(self):
        """Ensure this raises an error if a KeyEnumerator contains an incorrect ID."""
        # Make a copy:
        copy_commands = NameIdDict(self.commands)
        # Ensure there is at least one ID with multiple unique names
        # pointing to it (to ensure all names are removed when deleting
        # any reference to the data):
        new_name = "".join(copy_commands.names)
        last_val = copy_commands[self.ids[-1]]
        copy_commands[self.ids[-1], new_name] = last_val
        copy_commands[self.ids[-1], new_name + "2"] = last_val

        # Get a bunch of KeyEnumerators from `collect()`:
        collected = copy_commands.collect()
        kes = [ke for ke, _ in collected]
        # Swap the (valid) IDs corresponding to the first two entries:
        names_0, id_0 = kes[0]
        names_1, id_1 = kes[1]
        kes[0] = names_0, id_1
        kes[1] = names_1, id_0

        # Ensure an error is raised for the modified KeyEnumerator:
        with pytest.raises(KeyError):
            copy_commands[kes[0]]
            copy_commands[kes[1]]

        # ... and only the modified KeyEnumerator
        for ke in kes[2:]:
            _, ID = ke
            # Key Enumerator should return same thing as indexing by just ID
            # (and shouldn't raise any errors):
            assert copy_commands[ke] == copy_commands[ID], \
                f"Indexing by KeyEnumerator {ke} gave a different value than indexing by its ID {ID}."


@ pytest.mark.usefixtures('suite_setup')
class SetItemSuite(object):
    """Verify all __setitem__ techniques work."""

    @pytest.fixture(autouse=True)
    def _setup(self):
        """Common setup. Performed once per test."""
        # Make a copy:
        self.copy_commands: NameIdDict[str] = NameIdDict(self.commands)

        # Construct a name, id, and value which aren't currently anywhere in object:
        self.new_id = max(self.copy_commands.ids) + 1
        self.new_name = ''.join(self.copy_commands.names) + '_'
        self.new_val = ''.join(self.copy_commands.vals) + '_'

    def test_index_by_id(self):
        # Update a value:
        for ID in self.copy_commands.ids:
            self.copy_commands[ID] = self.new_val
            assert self.copy_commands[ID] == self.new_val, \
                f"Didn't correctly set the value for existing ID {ID} to {self.new_val}."

        # Add a new value (can't do it with just an ID index, need both ID and name,
        # so this should throw an error):
        even_newer_val = self.new_val + 'sOmEeXtRaLeTtErS'
        with pytest.raises(KeyError):
            self.copy_commands[self.new_id] = even_newer_val

    def test_index_by_name(self):
        # Update a value:
        for name in self.copy_commands.names:
            unique_val = self.new_val + name
            self.copy_commands[name] = unique_val
            assert self.copy_commands[name] == unique_val, \
                f"Didn't correctly set the value for existing name {name} to {unique_val}."

        # Add a new value (can't do it with just a name index, need both ID and name,
        # so this should throw an error):
        even_newer_val = self.new_val + 'sOmEeXtRaLeTtErS'
        with pytest.raises(KeyError):
            self.copy_commands[self.new_name] = even_newer_val

    def test_index_by_tuple_no_paren(self):
        # Update value:
        for idx, (ID, name) in enumerate(zip(self.ids, self.names)):
            unique_val = self.new_val + name + str(ID)
            self.copy_commands[ID, name] = unique_val
            assert self.copy_commands[ID, name] == unique_val, \
                f"Didn't correctly set the value for the {idx}th indexing tuple (({ID},{name})) to {unique_val}."

        # Add a new entry:
        even_newer_val = self.new_val + 'sOmEeXtRaLeTtErS'
        self.copy_commands[self.new_id, self.new_name] = even_newer_val
        assert self.copy_commands[self.new_id, self.new_name] == even_newer_val, \
            f"Didn't correctly set the value for the new index (({self.new_id},{self.new_name})) to {even_newer_val}."

    def test_new_id_for_existing_name(self):
        """
        If the name already exists but a new or different ID is being used, the name must be remapped to the value associated with the given ID.
        (Each unique name can only map to one unique ID).

        Note: for the purposes of this test, it's fine that `self.new_id` won't be new for all but the first test. In fact, this is desired since
        we also get to test what happens when mapping to a different but already extant ID.
        """
        for (names, ID), _ in self.copy_commands.collect():
            for name in names:
                unique_val = self.new_val + name + str(ID)
                self.copy_commands[self.new_id, name] = unique_val
                # Should now be mapped to new val:
                assert self.copy_commands[self.new_id, name] == unique_val, \
                    f"Didn't correctly set the value for the new index pairing (({self.new_id},{name})) to {unique_val}."
                assert self.copy_commands[self.new_id] == unique_val, \
                    f"Didn't correctly set the value for the new index pairing (({self.new_id},{name})) to {unique_val}."
                assert self.copy_commands[name] == unique_val, \
                    f"Didn't correctly set the value for the new index pairing (({self.new_id},{name})) to {unique_val}."

                # Old pairing should now be unlinked:
                assert self.copy_commands[ID] != unique_val, \
                    f"Somehow didn't correctly unlink the old index pairing (({ID},{name})) when setting the new index pairing (({self.new_id},{name}))."
                # The old pairing should no longer exist and should raise a KeyError:
                with pytest.raises(KeyError):
                    self.copy_commands[ID, name]

    def test_new_name_for_existing_id(self):
        """
        If an entry with this ID already exists, its[VT] data will be updated.
        The previous string which mapped to this ID will be preserved but it
        will now map to this new updated value.
        """
        for ID in self.copy_commands.ids:
            # Don't update the value, just add the new name:
            old_val = self.copy_commands[ID]
            self.copy_commands[ID, self.new_name] = old_val
            # Should now be mapped to new val:
            assert self.copy_commands[ID, self.new_name] == old_val, \
                f"Didn't correctly add the new name ({self.new_name}) to map to the ID ({ID}) without modifying the ID's data value."
            assert self.copy_commands[ID] == old_val, \
                f"Didn't correctly add the new name ({self.new_name}) to map to the ID ({ID}) without modifying the ID's data value."
            assert self.copy_commands[self.new_name] == old_val, \
                f"Didn't correctly add the new name ({self.new_name}) to map to the ID ({ID}) without modifying the ID's data value."

            # Update the value:
            newer_name = self.new_name + 'sOmEeXtRaLeTtErS'
            unique_val = self.new_val + newer_name + str(ID)
            self.copy_commands[ID, newer_name] = unique_val
            # Should now be mapped to new val:
            assert self.copy_commands[ID, newer_name] == unique_val, \
                f"Didn't correctly set the value for the new index pairing (({ID},{newer_name})) to {unique_val}."
            assert self.copy_commands[ID] == unique_val, \
                f"Didn't correctly set the value for the new index pairing (({ID},{newer_name})) to {unique_val}."
            assert self.copy_commands[newer_name] == unique_val, \
                f"Didn't correctly set the value for the new index pairing (({ID},{newer_name})) to {unique_val}."

    def test_index_with_flipped_tuple(self):
        # For existing values:
        for ID, name in zip(self.ids, self.names):
            # misordered tuple (should be [int,str]) should raise TypeError:
            with pytest.raises(TypeError):
                self.copy_commands[name, ID] = self.new_val

        # And for new values:
        # misordered tuple (should be [int,str]) should raise TypeError:
        with pytest.raises(TypeError):
            self.copy_commands[self.new_name, self.new_id] = self.new_val

    def test_index_with_invalid_type(self):
        with pytest.raises(TypeError):
            self.copy_commands[3.4] = self.new_val

    def test_key_enumerator(self):
        """Ensure items can be set with a key enumerator."""
        # First, ensure there is at least one ID with multiple unique names
        # pointing to it (to ensure all names are removed when deleting
        # any reference to the data):
        copy_commands = NameIdDict(self.commands)
        first_val = copy_commands[self.ids[0]]
        copy_commands[self.ids[0], self.new_name] = first_val
        copy_commands[self.ids[0], self.new_name + "2"] = first_val

        # Attempt to clone a NameIdDict using only KeyEnumerators and set item.
        # Create an empty NameIdDict:
        clone = NameIdDict()
        # Get a bunch of KeyEnumerators from `collect()`:
        collected = copy_commands.collect()
        # Clone the object (should clone all names too since using KeyEnumerators):
        for ke, val in collected:
            clone[ke] = val

        # Check that it was actually cloned:
        assert clone == copy_commands, (
            f"Failed to accurately clone NameIdDict {copy_commands} using only KeyEnumerators."
            f"Clone looks like: {clone}."
        )


@ pytest.mark.usefixtures('suite_setup')
class DeleteItemSuite(object):
    """Verify all __delitem__ techniques work."""

    @pytest.fixture(autouse=True)
    def _setup(self):
        """Common setup. Performed once per test."""
        # Make a copy:
        self.copy_commands: NameIdDict[str] = NameIdDict(self.commands)

        # Ensure there is at least one ID with multiple unique names
        # pointing to it (to ensure all names are removed when deleting
        # any reference to the data):
        new_name = "".join(self.copy_commands.names)
        first_val = self.copy_commands[self.ids[0]]
        self.copy_commands[self.ids[0], new_name] = first_val
        self.copy_commands[self.ids[0], new_name + "2"] = first_val

    def _check_delete(self, remaining: List[Union[int, str, Tuple[int, str]]], deleted: List[Union[int, str, Tuple[int, str]]]) -> None:
        """Check that all items indexed by the elements of `remaining` are still in the list and all items indexed by the elements of `deleted` are gone from the list."""
        for r in remaining:
            try:
                self.copy_commands[r]
            except KeyError:
                pytest.fail(
                    f"Index {r} appears to have been deleted from the NameIdDict and it should still be there."
                )

        for d in deleted:
            with pytest.raises(KeyError):
                self.copy_commands[d]

    def test_index_by_id(self):
        # Setup:
        orig_data = self.copy_commands.collect()
        remaining_ids = self.copy_commands.ids
        deleted_ids: List[int] = []
        remaining_names = self.copy_commands.names
        deleted_names: List[int] = []

        # Loop through all data by ID:
        for (id_names, ID), _ in orig_data:
            # Delete by ID:
            del self.copy_commands[ID]

            # Check that the ID mapping no longer works:
            with pytest.raises(KeyError):
                self.copy_commands[ID]

            # Ensure all associated names have also been removed:
            for name in id_names:
                with pytest.raises(KeyError):
                    self.copy_commands[name]

            # Log the intended removals:
            remaining_names = [
                rn for rn in remaining_names
                if rn not in id_names
            ]
            deleted_names.extend(id_names)

            remaining_ids.remove(ID)
            deleted_ids.append(ID)

            # Check that only the desired ID has been removed:
            self._check_delete(remaining_ids, deleted_ids)
            # Check that all and only the desired names have been removed:
            self._check_delete(remaining_names, deleted_names)

    def test_index_by_name(self):
        # Setup:
        orig_data = self.copy_commands.collect()
        remaining_ids = self.copy_commands.ids
        deleted_ids: List[int] = []
        remaining_names = self.copy_commands.names
        deleted_names: List[int] = []

        # Loop through all data by ID:
        for (id_names, ID), _ in orig_data:
            # Loop through all names associated with this ID:
            for i, name in enumerate(id_names):
                if i == 0:
                    # Delete the first one:
                    del self.copy_commands[name]
                else:
                    # All other names also mapping to this ID should also be gone
                    # once one name is removed:
                    with pytest.raises(KeyError):
                        self.copy_commands[name]

            # Check that the ID mapping also no longer works:
            with pytest.raises(KeyError):
                self.copy_commands[ID]

            # Log the intended removals:
            remaining_names = [
                rn for rn in remaining_names
                if rn not in id_names
            ]
            deleted_names.extend(id_names)

            remaining_ids.remove(ID)
            deleted_ids.append(ID)

            # Check that only the desired ID has been removed:
            self._check_delete(remaining_ids, deleted_ids)
            # Check that all and only the desired names have been removed:
            self._check_delete(remaining_names, deleted_names)

    def test_index_by_tuple(self):
        # Setup:
        orig_data = self.copy_commands.collect()
        remaining_ids = self.copy_commands.ids
        deleted_ids: List[int] = []
        remaining_names = self.copy_commands.names
        deleted_names: List[int] = []

        # Loop through all data by ID:
        for (id_names, ID), _ in orig_data:
            # Loop through all names associated with this ID:
            for i, name in enumerate(id_names):
                if i == 0:
                    # Delete the first one:
                    del self.copy_commands[ID, name]
                else:
                    # All other names also mapping to this ID should also be gone
                    # once one name is removed:
                    with pytest.raises(KeyError):
                        self.copy_commands[ID, name]

            # Check that the ID mapping also no longer works:
            with pytest.raises(KeyError):
                self.copy_commands[ID, id_names[0]]

            # Log the intended removals:
            remaining_names = [
                rn for rn in remaining_names
                if rn not in id_names
            ]
            deleted_names.extend(id_names)

            remaining_ids.remove(ID)
            deleted_ids.append(ID)

            # Check that only the desired ID has been removed:
            self._check_delete(remaining_ids, deleted_ids)
            # Check that all and only the desired names have been removed:
            self._check_delete(remaining_names, deleted_names)

    def test_index_by_key_enumerator(self):
        # Setup:
        orig_data = self.copy_commands.collect()
        remaining_ids = self.copy_commands.ids
        deleted_ids: List[int] = []
        remaining_names = self.copy_commands.names
        deleted_names: List[int] = []

        # Loop through all data by ID:
        for ke, _ in orig_data:
            id_names, ID = ke

            # Delete using the KeyEnumerator:
            del self.copy_commands[ke]

            # Loop through all names associated with this ID:
            for i, name in enumerate(id_names):
                # Ensure all names mapping to this ID are gone:
                with pytest.raises(KeyError):
                    self.copy_commands[ID, name]

            # Check that the ID mapping also no longer works:
            with pytest.raises(KeyError):
                self.copy_commands[ID, id_names[0]]

            # Log the intended removals:
            remaining_names = [
                rn for rn in remaining_names
                if rn not in id_names
            ]
            deleted_names.extend(id_names)

            remaining_ids.remove(ID)
            deleted_ids.append(ID)

            # Check that only the desired ID has been removed:
            self._check_delete(remaining_ids, deleted_ids)
            # Check that all and only the desired names have been removed:
            self._check_delete(remaining_names, deleted_names)

    def test_invalid_type(self):
        with pytest.raises(TypeError):
            del self.copy_commands[3.14]

    def test_index_invalid_id(self):
        # Store data from before deletion attempt:
        orig_ids = self.copy_commands.ids
        orig_names = self.copy_commands.names

        # Construct an id which isn't currently anywhere in object:
        self.bad_id = max(self.copy_commands.ids) + 1
        # ... and try to delete it:
        with pytest.raises(KeyError):
            del self.copy_commands[self.bad_id]

        # Make sure nothing was (somehow) spuriously deleted:
        self._check_delete(remaining=orig_ids, deleted=[])
        self._check_delete(remaining=orig_names, deleted=[])

    def test_index_invalid_name(self):
        # Store data from before deletion attempt:
        orig_ids = self.copy_commands.ids
        orig_names = self.copy_commands.names

        # Construct a name which isn't currently anywhere in object:
        self.bad_name = ''.join(self.copy_commands.names) + '_'
        # ... and try to delete it:
        with pytest.raises(KeyError):
            del self.copy_commands[self.bad_name]

        # Make sure nothing was (somehow) spuriously deleted:
        self._check_delete(remaining=orig_ids, deleted=[])
        self._check_delete(remaining=orig_names, deleted=[])

    def test_index_invalid_tuple(self):
        # Store data from before deletion attempt:
        orig_ids = self.copy_commands.ids
        orig_names = self.copy_commands.names

        # Construct a name and id which aren't currently anywhere in object:
        self.bad_id = max(self.copy_commands.ids) + 1
        self.bad_name = ''.join(self.copy_commands.names) + '_'

        # try to delete the invalid pair:
        with pytest.raises(KeyError):
            del self.copy_commands[self.bad_id, self.bad_name]

        # use a valid name but invalid ID:
        # expect either KeyError because ID is bad or
        # ValueError because name doesn't map to this ID:
        with pytest.raises((KeyError, ValueError)):
            del self.copy_commands[self.bad_id, orig_names[0]]

        # use a valid ID but invalid name:
        with pytest.raises(KeyError):
            del self.copy_commands[orig_ids[0], self.bad_name]

        # use a valid name with valid but wrong ID:
        # (given name doesn't pair with given ID)
        with pytest.raises(ValueError):
            data = self.copy_commands.collect()
            ([first_name, *_], _), _ = data[0]  # ([names], id), value
            (_, second_id), _ = data[1]  # ([names], id), value
            del self.copy_commands[second_id, first_name]

        # Make sure nothing was (somehow) spuriously deleted:
        self._check_delete(remaining=orig_ids, deleted=[])
        self._check_delete(remaining=orig_names, deleted=[])

    def test_invalid_key_enumerator(self):
        # Store data from before deletion attempt:
        orig_data = self.copy_commands.collect()
        orig_ids = self.copy_commands.ids
        orig_names = self.copy_commands.names

        # Construct names and an id which aren't currently anywhere in object:
        self.bad_id = max(self.copy_commands.ids) + 1
        bad_name = ''.join(self.copy_commands.names)
        self.bad_names = [bad_name + '1', bad_name + '2']
        ([good_name, *_], _), _ = orig_data[0]
        self.semi_bad_names = [good_name, bad_name]

        # try to delete the invalid pair:
        with pytest.raises(KeyError):
            del self.copy_commands[[self.bad_names, self.bad_id]]

        # use all valid names but invalid ID:
        # expect either KeyError because ID is bad or
        # ValueError because names don't map to this ID:
        with pytest.raises((KeyError, ValueError)):
            del self.copy_commands[[orig_data[0][0][0], self.bad_id]]

        # use one valid name but invalid ID:
        # expect either KeyError because ID is bad or
        # ValueError because names don't map to this ID:
        with pytest.raises((KeyError, ValueError)):
            del self.copy_commands[[self.semi_bad_names, self.bad_id]]

        # use a valid ID but all invalid names:
        with pytest.raises(KeyError):
            del self.copy_commands[[self.bad_names, orig_data[0][0][1]]]

        # use valid names with valid but wrong ID:
        # (given name doesn't pair with given ID)
        with pytest.raises(ValueError):
            (first_names, _), _ = orig_data[0]  # ([names], id), value
            (_, second_id), _ = orig_data[1]  # ([names], id), value
            del self.copy_commands[first_names, second_id]

        # Make sure nothing was (somehow) spuriously deleted:
        self._check_delete(remaining=orig_ids, deleted=[])
        self._check_delete(remaining=orig_names, deleted=[])


@ pytest.mark.usefixtures('suite_setup')
class ContainsItemSuite(object):
    """Verify all __contains__ uses work."""

    @pytest.fixture(autouse=True)
    def _setup(self):
        """Common setup. Performed once per test."""
        # Make a copy:
        self.copy_commands: NameIdDict[str] = NameIdDict(self.commands)

        # Ensure there is at least one ID with multiple unique names
        # pointing to it (to ensure all names are removed when deleting
        # any reference to the data):
        new_name = "".join(self.copy_commands.names)
        first_val = self.copy_commands[self.ids[0]]
        self.copy_commands[self.ids[0], new_name] = first_val
        self.copy_commands[self.ids[0], new_name + "2"] = first_val

    def test_known_ids(self):
        for ID in self.copy_commands.ids:
            assert ID in self.copy_commands, f"NameIdDict failed to recognize known ID {ID}."

    def test_known_names(self):
        for name in self.copy_commands.names:
            assert name in self.copy_commands, f"NameIdDict failed to recognize known name {name}."

    def test_known_tuples(self):
        for (names, ID), _ in self.copy_commands.collect():
            for name in names:
                assert (ID, name) in self.copy_commands, \
                    f"NameIdDict failed to recognize known indexing tuple ({ID},{name})."

    def test_known_key_enumerators(self):
        for ke, _ in self.copy_commands.collect():
            assert ke in self.copy_commands, \
                f"NameIdDict failed to recognize known KeyEnumerator index ({ke})."

    def test_unknown_id(self):
        # Construct an ID that's definitely not in the NameIdDict:
        unknown_id = max(self.copy_commands.ids) + 1
        # Make sure the NameIdDict knows it's not contained by it:
        assert (unknown_id in self.copy_commands) == False, \
            f"NameIdDict failed to state that unknown ID {ID} is not contained in it."

    def test_unknown_name(self):
        # Construct a name that's definitely not in the NameIdDict:
        unknown_name = "".join(self.copy_commands.names)
        # Make sure the NameIdDict knows it's not contained by it:
        assert (unknown_name in self.copy_commands) == False, \
            f"NameIdDict failed to state that unknown name {name} is not contained in it."

    def test_unknown_tuples(self):
        # Construct an ID that's definitely not in the NameIdDict:
        unknown_id = max(self.copy_commands.ids) + 1
        # Construct a name that's definitely not in the NameIdDict:
        unknown_name = "".join(self.copy_commands.names)

        # Grab a known name and ID pair:
        ([known_name, *_], known_id), _ = self.copy_commands.collect()[0]
        # Grab a second known name and ID pair:
        ([known_name2, *_], known_id2), _ = self.copy_commands.collect()[1]

        # Known ID, unknown name:
        assert ((known_id, unknown_name) in self.copy_commands) == False, \
            f"NameIdDict failed to state that bad tuple containing a known id {known_id} but unknown name {unknown_name} is not contained in it."

        # Unknown ID, known name:
        assert ((unknown_id, known_name) in self.copy_commands) == False, \
            f"NameIdDict failed to state that bad tuple containing an unknown id {unknown_id} but known name {known_name} is not contained in it."

        # Unknown ID, unknown name:
        assert ((unknown_id, unknown_name) in self.copy_commands) == False, \
            f"NameIdDict failed to state that bad tuple containing an unknown id {unknown_id} and unknown name {unknown_name} is not contained in it."

        # Mispaired ID and name:
        assert ((known_id, known_name2) in self.copy_commands) == False, \
            f"NameIdDict failed to state that bad tuple containing a known id {known_id} and known but mismatched name {known_name2} is not contained in it."
        assert ((known_id2, known_name) in self.copy_commands) == False, \
            f"NameIdDict failed to state that bad tuple containing a known id {known_id2} and known but mismatched name {known_name} is not contained in it."

    def test_unknown_key_enumerators(self):
        # Construct an ID that's definitely not in the NameIdDict:
        unknown_id = max(self.copy_commands.ids) + 1
        # Construct a name that's definitely not in the NameIdDict:
        unknown_name = "".join(self.copy_commands.names)
        unknown_name2 = unknown_name + '2'
        unknown_names = [unknown_name, unknown_name2]
        semi_unknown_names = [self.copy_commands.names[0], unknown_name]

        data = self.copy_commands.collect()
        # Grab a pair of a known ID and a set of known names:
        (known_names, known_id), _ = data[0]
        # Grab a second pair of a known ID and a set of known names:
        (known_names2, known_id2), _ = data[1]

        # Known ID, unknown names:
        assert ([unknown_names, known_id] in self.copy_commands) == False, \
            f"NameIdDict failed to state that bad KeyEnumerator containing a known id {known_id} but unknown names {unknown_names} is not contained in it."

        # Known ID, mix of known and unknown names:
        assert ([semi_unknown_names, known_id] in self.copy_commands) == False, \
            f"NameIdDict failed to state that bad KeyEnumerator containing a known id {known_id} but mix of known and unknown names {semi_unknown_names} is not contained in it."

        # Unknown ID, known names:
        assert ([known_names, unknown_id] in self.copy_commands) == False, \
            f"NameIdDict failed to state that bad KeyEnumerator containing a unknown id {unknown_id} but known names {known_names} is not contained in it."

        # Unknown ID, unknown names:
        assert ([unknown_names, unknown_id] in self.copy_commands) == False, \
            f"NameIdDict failed to state that bad KeyEnumerator containing an unknown id {unknown_id} and unknown names {unknown_names} is not contained in it."

        # Unknown ID, mix of known and unknown names:
        assert ([semi_unknown_names, unknown_id] in self.copy_commands) == False, \
            f"NameIdDict failed to state that bad KeyEnumerator containing an unknown id {unknown_id} and mix of known and unknown names {semi_unknown_names} is not contained in it."

        # Mispaired ID and name:
        assert ([known_names2, known_id] in self.copy_commands) == False, \
            f"NameIdDict failed to state that bad tuple containing a known id {known_id} and known but mismatched names {known_names2} is not contained in it."
        assert ([known_names, known_id2] in self.copy_commands) == False, \
            f"NameIdDict failed to state that bad tuple containing a known id {known_id2} and known but mismatched names {known_names} is not contained in it."

    def test_invalid_type(self):
        assert (3.14 in self.copy_commands) == False, \
            f"NameIdDict failed to state that an index with invalid type (3.14) is not contained in it."


@ pytest.mark.usefixtures('suite_setup')
class LengthSuite(object):
    """Verify all __len__ uses work."""

    def test_empty(self):
        """Ensure empty NameIdDict gives zero length (and doesn't error out)."""
        empty_commands: NameIdDict[str] = NameIdDict()
        assert len(empty_commands) == 0, "Empty NameIdDict gave non-zero length."

    def test_basic(self):
        assert len(self.commands) == len(self.ids), \
            "Length of NameIdDict doesn't match number of unique IDs."

    def test_with_duplicate_names(self):
        """Ensure length gives number of ID entries and not number of names entries in the condition where multiple IDs map to the same name."""
        # Make at least one ID have multiple names mapping to it:
        new_name = "".join(self.commands.names)
        first_val = self.commands[self.ids[0]]
        self.commands[self.ids[0], new_name] = first_val
        self.commands[self.ids[0], new_name + "2"] = first_val

        assert len(self.commands) == len(self.ids), \
            "Length of NameIdDict doesn't match number of unique IDs when multiple names map to same ID."


@ pytest.mark.usefixtures('suite_setup')
class FastItemsSuite(object):
    """Verify all fast_items uses work."""

    def test_empty(self):
        """Ensure this returns an empty iterator is returned for an empty NameIdDict (and doesn't error out)."""
        empty_commands: NameIdDict[str] = NameIdDict()
        it: Iterator[Tuple[Tuple[int, str], int]] = empty_commands.fast_items()
        it_len: int = sum([1 for i in it])
        assert it_len == 0, "`fast_items` should return an empty iterator if the NameIdDict is empty."

    def test_basic(self):
        # What the data should look like:
        # (this can be built simply since we know the correspondence and that there's only one name per ID):
        data = list(zip(zip(self.ids, self.names), self.vals))
        # Sort by ID (match `fast_items` output formatting):
        data_sorted = sorted(data, key=lambda x: x[0][0])

        # Collect `fast_items` output:
        output = list(self.commands.fast_items())

        assert len(output) == len(data_sorted), \
            "`fast_items` output formatting is incorrect and produces the wrong length."

        # Note: deep list assertion like this just works in pytest:
        assert output == data_sorted, \
            "`fast_items` output pairing or formatting is incorrect."

    def test_with_duplicate_names(self):
        """Check correctness when multiple names map to an ID."""
        # Make at least one ID have multiple names mapping to it:
        new_name = "".join(self.commands.names)
        first_val = self.commands[self.ids[0]]
        self.commands[self.ids[0], new_name] = first_val
        self.commands[self.ids[0], new_name + "2"] = first_val

        # What the data should look like:
        # (this can be built simply since we know the correspondence and that there's only one name per ID):
        data = list(zip(zip(self.ids, self.names), self.vals))
        # Sort by ID (match `fast_items` output)
        data_sorted = sorted(data, key=lambda x: x[0][0])

        # Collect `fast_items` output:
        output = list(self.commands.fast_items())

        assert len(output) == len(data_sorted), \
            "`fast_items` output formatting is incorrect and produces the wrong length."

        for i, (((data_id, _), data_val), ((out_id, out_name), out_val)) in enumerate(zip(data_sorted, output)):
            # Make sure basic ID and value are correct:
            assert data_id == out_id, f"Correct ID and the ID from output of `fast_items` mismatch at index {i}"
            assert data_val == out_val, f"Correct value and the value from output of `fast_items` mismatch at index {i}"

            # Since the name chosen by `fast_items` might not be the first name given for each ID (as it is in `data`),
            # just make sure the name picked maps to the correct ID:
            assert self.commands.name_id_mapping[out_name] == out_id, \
                f"Given name ({out_name}) doesn't map to given ID ({out_id}) at index {i} in output of `fast_items`"


@ pytest.mark.usefixtures('suite_setup')
class IterSuite(object):
    """Verify all __iter__ uses work."""

    def test_empty(self):
        """Ensure this returns an empty iterator is returned for an empty NameIdDict (and doesn't error out)."""
        empty_commands: NameIdDict[str] = NameIdDict()
        it: Iterator[Tuple[int, str]] = iter(empty_commands)
        it_len: int = sum([1 for i in it])
        assert it_len == 0, "`__iter__` should return an empty iterator if the NameIdDict is empty."

    def test_basic(self):
        # What the data should look like:
        # (this can be built simply since we know the correspondence and that there's only one name per ID):
        data = list(zip(self.ids, self.names))
        # Sort by ID (match `__iter__` output formatting):
        data_sorted = sorted(data, key=lambda x: x[0])

        # Collect `__iter__` output:
        output = [x for x in self.commands]

        assert len(output) == len(data_sorted), \
            "`__iter__` output formatting is incorrect and produces the wrong length."

        # Note: deep list assertion like this just works in pytest:
        assert output == data_sorted, \
            "`__iter__` output pairing or formatting is incorrect."

    def test_with_duplicate_names(self):
        """Check correctness when multiple names map to an ID."""
        # Make at least one ID have multiple names mapping to it:
        new_name = "".join(self.commands.names)
        first_val = self.commands[self.ids[0]]
        self.commands[self.ids[0], new_name] = first_val
        self.commands[self.ids[0], new_name + "2"] = first_val

        # What the data should look like:
        # (this can be built simply since we know the correspondence and that there's only one name per ID):
        data = list(zip(self.ids, self.names))
        # Sort by ID (match `__iter__` output)
        data_sorted = sorted(data, key=lambda x: x[0])

        # Collect `fast_items` output:
        output = [x for x in self.commands]

        assert len(output) == len(data_sorted), \
            "`__iter__` output formatting is incorrect and produces the wrong length."

        for i, ((data_id, _), (out_id, out_name)) in enumerate(zip(data_sorted, output)):
            # Make sure basic ID and value are correct:
            assert data_id == out_id, f"Correct ID and the ID from output of `__iter__` mismatch at index {i}"

            # Since the name chosen by `fast_items` might not be the first name given for each ID (as it is in `data`),
            # just make sure the name picked maps to the correct ID:
            assert self.commands.name_id_mapping[out_name] == out_id, \
                f"Given name ({out_name}) doesn't map to given ID ({out_id}) at index {i} in output of `__iter__`"


@ pytest.mark.usefixtures('suite_setup')
class CollectSuite(object):
    """Verify all `collect` uses work."""

    def test_empty(self):
        """Ensure this returns an empty list is returned for an empty NameIdDict (and doesn't error out)."""
        empty_commands: NameIdDict[str] = NameIdDict()
        li = empty_commands.collect()
        assert len(li) == 0, \
            "`collect` should return an empty list if the NameIdDict is empty."

    def test_basic(self):
        # What the data should look like:
        # (this can be built simply since we know the correspondence and that there's only one name per ID):
        wrapped_names = [[n]
                         for n in self.names]  # wrap each name up in a list
        data = list(zip(zip(wrapped_names, self.ids), self.vals))
        # Sort by ID (match `collect` output formatting):
        data_sorted = sorted(data, key=lambda x: x[0][1])

        # Collect `collect` output:
        output = self.commands.collect()

        assert len(output) == len(data_sorted), \
            "`collect` output formatting is incorrect and produces the wrong length."

        # Note: deep list assertion like this just works in pytest:
        assert output == data_sorted, \
            "`collect` output pairing or formatting is incorrect."

    def test_with_duplicate_names(self):
        """Check correctness when multiple names map to an ID."""
        # Make at least one ID have multiple names mapping to it:
        new_name = "".join(self.commands.names)
        new_name_2 = new_name + "2"
        first_val = self.commands[self.ids[0]]
        self.commands[self.ids[0], new_name] = first_val
        self.commands[self.ids[0], new_name_2] = first_val

        # What the data should look like:
        # (this can be built simply since we know the correspondence and that there's only one name per ID):
        wrapped_names = [[n]
                         for n in self.names]  # wrap each name up in a list
        wrapped_names[0].append(new_name)
        wrapped_names[0].append(new_name_2)
        wrapped_names[0].sort()
        data = list(zip(zip(wrapped_names, self.ids), self.vals))
        # Sort by ID (match `collect` output formatting):
        data_sorted = sorted(data, key=lambda x: x[0][1])

        # Collect `collect` output:
        output = self.commands.collect()

        assert len(output) == len(data_sorted), \
            "`__iter__` output formatting is incorrect and produces the wrong length."

        # Note: deep list assertion like this just works in pytest:
        assert output == data_sorted, \
            "`collect` output pairing or formatting is incorrect."


@ pytest.mark.usefixtures('suite_setup')
class UpdateSuite(object):
    """
    Verify all update uses work.

    Essentially the same tests as in ConstructorSuite.
    """
    @staticmethod
    def _verify_content(nid: NameIdDict[str], ids: List[int], names: List[str], vals: List[str]) -> None:
        """
        Ensure the given NameIdDict(nid) has the given ids, names, and values in the given order.

        Also performs basic content sanity checks(proper name-ID mapping).
        """

        assert list(nid.id_dict.keys()) == ids, \
            "NameIdDict IDs set improperly."
        assert list(nid.id_dict.values()) == vals, \
            "NameIdDict values set improperly."
        assert list(nid.name_id_mapping.keys()) == names, \
            "NameIdDict names set improperly."

        name_ids = set(nid.name_id_mapping.values())
        core_ids = set(nid.id_dict.keys())

        assert len(name_ids - core_ids) == 0, \
            "NameIdDict contains names which map to IDs that don't exist."
        assert len(core_ids - name_ids) == 0, \
            "NameIdDict contains IDs which aren't mapped to by any names."

    def test_empty(self):
        """Construct new empty dict."""
        commands: NameIdDict[str] = NameIdDict()
        commands.update()
        self._verify_content(commands, [], [], [])

    def test_list(self):
        """Construct new dict from tuple list."""
        data = zip(zip(self.ids, self.names), self.vals)
        commands: NameIdDict[str] = NameIdDict()
        commands.update(data)
        self._verify_content(commands, self.ids, self.names, self.vals)

    def test_dict(self):
        """Construct new dict from dict."""
        data = zip(zip(self.ids, self.names), self.vals)
        data = dict(data)
        commands: NameIdDict[str] = NameIdDict()
        commands.update(data)
        self._verify_content(commands, self.ids, self.names, self.vals)

    def test_copy(self):
        """Construct new NameIdDict by * copying * an existing one."""
        commands2: NameIdDict[str] = NameIdDict()
        commands2.update(self.commands)
        # Should be correct:
        assert commands2 == self.commands, \
            "The second NameIdDict doesn't contain the same content as the first."

        # Should be a copy (not pointing to the same object):
        assert id(commands2) != id(self.commands), \
            "NameIdDict constructor doesn't make a *copy*."

        # Same deal with the internal data structures:
        assert id(commands2.id_dict) != id(self.commands.id_dict), \
            "NameIdDict constructor doesn't make a *copy* of `id_dict`."
        assert id(commands2.name_id_mapping) != id(self.commands.name_id_mapping), \
            "NameIdDict constructor doesn't make a *copy* of `name_id_mapping`."


@ pytest.mark.usefixtures('suite_setup')
class DirectAccessorsSuite(object):
    """Verify all direct data accessors work."""

    def test_ids(self):
        assert self.ids == self.commands.ids, "IDs were not properly fetched in property accessor."

    def test_names(self):
        """Test the simple condition when each ID only has one corresponding name."""
        assert self.names == self.commands.names, "Names were not properly fetched in property accessor."

    def test_names_multiple(self):
        """Make sure all names are given when multiple names map to the same ID."""
        # Make at least one ID have multiple names mapping to it:
        new_name = "".join(self.commands.names)
        new_name_2 = new_name + "2"
        first_val = self.commands[self.ids[0]]
        self.commands[self.ids[0], new_name] = first_val
        self.commands[self.ids[0], new_name_2] = first_val

        self.names.append(new_name)
        self.names.append(new_name_2)

        assert sorted(self.names) == sorted(self.commands.names), \
            "Names were not properly fetched in property accessor."

    def test_key_enumerators(self):
        """
        Make sure all KeyEnumerators are returned correctly.

        This test is done by simply ensuring that this gives the same output as 
        what `collect()` gives and lean very heavily on the more comprehensive 
        tests in `collect()`. At the time of writing, this test is sort of 
        pointless since the definition of `key_enumerators` is to just return 
        the KeyEnumerators from the output of `collect()` BUT having this test 
        ensures that consistency will remain, even if the implementation of one 
        or both of them changes in the future.
        """
        # Make at least one ID have multiple names mapping to it:
        new_name = "".join(self.commands.names)
        new_name_2 = new_name + "2"
        first_val = self.commands[self.ids[0]]
        self.commands[self.ids[0], new_name] = first_val
        self.commands[self.ids[0], new_name_2] = first_val

        assert self.commands.key_enumerators == [ke for ke, _ in self.commands.collect()], (
            "Output of `key_enumerators` does not match output of first "
            "elements from `collect()` even though it should."
        )

    def test_vals(self):
        assert self.vals == self.commands.vals, "Values were not properly fetched in property accessor."


@ pytest.mark.usefixtures('suite_setup')
class EqualsSuite(object):
    """Verify all __eq__ uses work."""

    def test_copy(self):
        copy: NameIdDict[str] = NameIdDict(self.commands)
        assert copy == self.commands, "Copy of object was not recognized as being equal to object."

    def test_expanded_copy(self):
        copy: NameIdDict[str] = NameIdDict(self.commands)
        new_name = "".join(copy.names)
        new_id = max(copy.ids) + 1
        copy[new_id, new_name] = "I'm different."
        assert copy != self.commands, "Expanded copy of object with extra ID was falsely recognized as being equal to original object."

    def test_mutated_copy(self):
        copy: NameIdDict[str] = NameIdDict(self.commands)
        new_val = "".join(copy.vals)  # create a unique value
        copy[copy.ids[0]] = new_val
        assert copy != self.commands, "Mutated copy of object with changed data was falsely recognized as being equal to original object."

    def test_against_empty(self):
        empty: NameIdDict[str] = NameIdDict()
        assert self.commands != empty, "Populated NameIdDict was falsely recognized as being equal to an empty NameIdDict."

    def test_empty_and_empty(self):
        empty1: NameIdDict[str] = NameIdDict()
        empty2: NameIdDict[str] = NameIdDict()
        assert empty1 == empty2, "Failed to recognize two empty NameIdDicts as being equal to each other."


@ pytest.mark.usefixtures('suite_setup')
class SerializationSuite(object):
    """Verify all required serialization techniques work."""

    @pytest.fixture(autouse=True)
    def _setup(self):
        """Common setup. Performed once per test."""
        # Make a copy:
        self.copy_commands: NameIdDict[str] = NameIdDict(self.commands)

        # Ensure there is at least one ID with multiple unique names
        # pointing to it (to ensure all names are removed when deleting
        # any reference to the data):
        new_name = "".join(self.copy_commands.names)
        first_val = self.copy_commands[self.ids[0]]
        self.copy_commands[self.ids[0], new_name] = first_val
        self.copy_commands[self.ids[0], new_name + "2"] = first_val

    def test_jsonpickle(self):
        import jsonpickle
        jsp = jsonpickle.encode(self.copy_commands, keys=True)
        reconstruction = jsonpickle.decode(jsp)
        assert reconstruction == self.copy_commands, (
            "Reconstruction of jsonpickled NameIdDict doesn't match original, pre-jsonpickling, NameIdDict."
            f"\n Original: {self.copy_commands}"
            f"\n Reconstruction: {reconstruction}"
        )
