# -*- coding: utf-8 -*-
"""
Test the critical functions of the classes of the `module` module.

@author: Connor W. Colombo (CMU)
@last-updated: 01/31/2021
"""
import pytest
from typing import Any, List, Union, Tuple
import inspect
from copy import deepcopy

from collections import OrderedDict
import jsonpickle  # type: ignore # mypy doesn't see type hints

from .module import *
from .fsw_data_type import FswDataType as FDT


class EqualsSuite(object):
    """
    Verify `__eq__` works correctly for all data classes.

    These are used in all tests higher up in the pipeline so need to work 
    correctly.
    """

    @staticmethod
    def _equality_test(cls: type, source: dict, unique: dict, permute=True, ignore_none=False, flag_duplicates=True):
        """
        Perform a comprehensive equality test for the given (container) class.

        # Operation:
        The test will always make sure that objects constructed from equivalent  
        data are viewed as equivalent. Then:

        If `permute` is set to `True`, the test is performed by iterating over 
        each key in `source` swapping only that key's value with the 
        corresponding value found in `unique` and then ensuring that `__eq__` 
        can tell these two objects are distinct.

        The main purpose of this is to test container classes and make sure no
        data requested by the constructor isn't tested for (directly or
        indirectly) by the equality function.

        If `permute` is set to `False`, the test will simply check to make sure 
        `cls(**source)==cls(**source) and cls(**source)!=cls(**unique)`.
        Use this when the constructor requires agreement between various 
        arguments and certain permutations may result in an invalid set of 
        constructor arguments.

        If `ignore_none` is `True`, the test will ignore any keys which have a 
        value of `None` in **both** `source` and `unique`. It's still necessary 
        to include these keys to signal that they are being ignored 
        intentionally and aren't just ctor arguments which were accidentally 
        missed. Note: the ignored keys with value `None` will be removed and 
        will not be passed to the constructor in any subtests.

        If `flag_duplicates` is `False`, the test won't raise any errors for 
        keys in `source` and `unique` which map to equivalent values. It will 
        still use these keys in the constructors, though (except where told to 
        do otherwise by `ignore_none`).

        # Requirements:
        `source` and `unique` should contain the same keys, one for every
        possible argument for the constructor of the given class `cls`.

        The values of these two dictionaries should be different for every key
        in such a way that the when an object is constructed with any permutation of these two
        parameter sets, it will be fundamentally different (i.e. it won't result
        in an equivalent object.

        # Examples of bad `source` and `unique` pairs:

        - `int('8', base=10)` and `int('0o10', base=8)` have distinct constructor
        arguments but both result in equivalent objects.

        - `int('7', base=10)` and `int('0o10', base=8)` have distinct constructor
        arguments **and** both result in distinct objects **but** not all
        combinations of these sets of arguments produce distinct objects,
        for example: `int('7', base=10)` and `int('7', base=8)` are equivalent.

        In practice, for the basic data container classes this method is
        designed to test, this caution doesn't come into play much and, when it
        does, the cases are much simpler.

        **Note:** This method's formatting checking abilities only works for
        classes where all arguments are enumerated in the constructor (i.e.
        no *args or **kwargs). For classes with such constructors, this method
        will not be able to automatically verify that the given `source` and
        `unique` arguments are formatted correctly.
        """
        ####
        # Perform basic setup check:
        ####
        # `unique` and `source` should share same keys:
        src_keys = set(source.keys())
        if src_keys != set(unique.keys()):
            raise ValueError(
                "TEST SET UP IMPROPERLY. Dictionaries for `_equality_test` ("
                "`source` and `unique`) should have the same keys but don't. "
                f"`source` has {src_keys} whereas `unique` has "
                f"{set(unique.keys())}."
            )

        # And the keys of `unique` and `source` should exactly cover all
        # constructor arguments of the given class:
        ctor_params = set(inspect.signature(
            cls.__init__  # type: ignore # direct __init__ access
        )._parameters.keys())
        ctor_params = ctor_params - {'self'}  # remove `self` argument
        if ctor_params != src_keys:
            raise ValueError(
                "TEST SET UP IMPROPERLY. Keys to dictionary arguments of "
                "`_equality_test` (`source` and `unique`) should exactly match "
                "the parameters taken by the constructor of the given class. "
                f"The constructor of the given class {cls.__name__} takes "
                f"{ctor_params} but the keys of given dictionaries are {src_keys}."
            )

        # ... but `unique` and `source` should have distinct values:
        if ignore_none or flag_duplicates:  # if neither True, this loop won't do anything
            for k in src_keys:
                if source[k] == unique[k]:
                    # Remove duplicate `None` keys if `ignore_none`:
                    if ignore_none and source[k] is None:
                        del source[k]
                        del unique[k]

                    elif flag_duplicates:
                        raise ValueError(
                            "TEST SET UP IMPROPERLY. Dictionaries for `_equality_test` ("
                            "`source` and `unique`) should have different values for "
                            f"each key but have equivalent values for key {k}."
                        )

        ####
        # Check Clone Equality:
        ####
        a = cls(**source)
        b = cls(**deepcopy(source))
        assert a == b, (
            f"Failed to recognize two equivalently instantiated {cls.__name__} "
            f"objects as being equal."
        )
        ####
        # Permute Parameters:
        ####
        ref = cls(**source)
        if permute:
            # For each keyword argument...
            for k in source.keys():
                # only run if we know the val being swapped is unique:
                if flag_duplicates or source[k] != unique[k]:
                    # Mutate just one keyword argument to a distinct value:
                    kwargs = deepcopy(source)
                    kwargs[k] = unique[k]
                    # Then build an altered object:
                    altered = cls(**kwargs)
                    # Ensure it's distinct:
                    assert ref != altered, (
                        f"The {cls.__name__} class didn't recognize two objects with "
                        "distinct constructor arguments as non-equal objects. "
                        "\n The reference object's arguments were: "
                        f"{cls.__name__}({', '.join([f'{k}={v}' for k,v in source.items()])})"
                        "\n The altered object's arguments were: "
                        f"{cls.__name__}({', '.join([f'{k}={v}' for k,v in kwargs.items()])})."
                        "\n First check that these two different sets of arguments should "
                        "have produced non-equal objects, then debug the equality test."
                    )
            else:
                altered = cls(**unique)
                # Ensure it's distinct:
                assert ref != altered, (
                    f"The {cls.__name__} class didn't recognize two objects with "
                    "distinct constructor arguments as non-equal objects. "
                    "\n The reference object's arguments were: "
                    f"{cls.__name__}({', '.join([f'{k}={v}' for k,v in source.items()])})"
                    "\n The altered object's arguments were: "
                    f"{cls.__name__}({', '.join([f'{k}={v}' for k,v in kwargs.items()])})."
                    "\n First check that these two different sets of arguments should "
                    "have produced non-equal objects, then debug the equality test."
                )

    @pytest.fixture(autouse=True)
    def _setup(self):
        """Common setup. Performed once per test."""

        # Sample Common Datastructures to be used for tests:
        self.sample_enum_1 = [
            EnumItem(
                name='FIELD_0',
                value=0,
                comment=""
            ),
            EnumItem(
                name='FIELD_1',
                value=1,
                comment="I am alive."
            )
        ]
        self.sample_enum_2 = [
            EnumItem(
                name='FIELD_FOUR',
                value=4,
                comment=""
            ),
            EnumItem(
                name='FIELD_5',
                value=5,
                comment="I am a five."
            )
        ]

        self.sample_args_1 = [
            Argument(
                name='arg_1',
                datatype=FDT.U8,
                metadata_json_str='{"fieldA1": "A1", "fieldB1": 21}'
            ),
            Argument(
                name='arg_2',
                datatype=FDT.U16,
                metadata_json_str='{"fieldA2": "A2", "fieldB2": 22}'
            )
        ]
        self.sample_args_2 = [
            Argument(
                name='arg_2',
                datatype=FDT.U16,
                metadata_json_str='{"fieldA2": "A2", "fieldB2": 22}'
            ),
            Argument(
                name='arg_3',
                datatype=FDT.ENUM,
                enum=self.sample_enum_2,
                metadata_json_str='{"fieldA3": "A3", "fieldB3": 23}'
            )
        ]

    def test_equals_bitfield_struct(self):
        self._equality_test(
            cls=BitfieldStruct,
            source={
                'fields': OrderedDict([('a', 1), ('b', 2)]),
                'total_padding': 29,
            },
            unique={
                'fields': OrderedDict([('a', 1), ('b', 3)]),
                'total_padding': 28,
            },
            permute=False
        )

    def test_equals_enum_item(self):
        self._equality_test(
            cls=EnumItem,
            source={
                'name': 'SETTING_ONE',
                'value': 1,
                'comment': 'hye'
            },
            unique={
                'name': 'SETTING_2',
                'value': 0x00,
                'comment': 'bye'
            }
        )

    def test_equals_arg(self):
        # Test for basic args:
        self._equality_test(
            cls=Argument,
            source={
                'name': 'arg_one',
                'datatype': FDT.U32,
                'enum': None,
                'bitfields':  None,
                'metadata_json_str': '{"fieldA1": "A1", "fieldB1": 21}'
            },
            unique={
                'name': 'arg_2',
                'datatype': FDT.I32,
                'enum': None,
                'bitfields':  None,
                'metadata_json_str': '{"fieldA2": "A2", "fieldB2": 22}'
            },
            ignore_none=True
        )
        # Test for enum args:
        self._equality_test(
            cls=Argument,
            source={
                'name': 'arg_one',
                'datatype': FDT.ENUM,
                'enum': self.sample_enum_1,
                'bitfields':  None,
                'metadata_json_str': '{"fieldA1": "A1", "fieldB1": 21}'
            },
            unique={
                'name': 'arg_2',
                'datatype': FDT.ENUM,
                'enum': self.sample_enum_2,
                'bitfields':  None,
                'metadata_json_str': '{"fieldA2": "A2", "fieldB2": 22}'
            },
            ignore_none=True,
            flag_duplicates=False
        )

        # Test for bitfield args:
        self._equality_test(
            cls=Argument,
            source={
                'name': 'arg_one',
                'datatype': FDT.U32,
                'enum': None,
                'bitfields': BitfieldStruct(
                    fields=OrderedDict([('a', 1), ('b', 2)]),
                    total_padding=29
                ),
                'metadata_json_str': '{"fieldA1": "A1", "fieldB1": 21}'
            },
            unique={
                'name': 'arg_2',
                'datatype': FDT.I32,
                'enum': None,
                'bitfields': BitfieldStruct(
                    fields=OrderedDict([('a', 4), ('b', 1)]),
                    total_padding=27
                ),
                'metadata_json_str': '{"fieldA2": "A2", "fieldB2": 22}'
            },
            ignore_none=True
        )

    def test_equals_command(self):
        self._equality_test(
            cls=Command,
            source={
                'name': 'SampleModule_CommandA',
                'mnemonic': 'command_a',
                'ID': 1,
                'args': self.sample_args_1,
                'metadata_json_str': '{"fieldA1": "A1", "fieldB1": 21}'
            },
            unique={
                'name': 'SampleModule_CommandB',
                'mnemonic': 'command_b',
                'ID': 0x02,
                'args': self.sample_args_2,
                'metadata_json_str': '{"fieldA2": "A2", "fieldB2": 22}'
            }
        )

    def test_equals_telemetry_channel(self):
        # Test for basic channel:
        self._equality_test(
            cls=TelemetryChannel,
            source={
                'name': 'chan_one',
                'ID': 0x01,
                'datatype': FDT.U32,
                'enum': None,
                'bitfields':  None,
                'metadata_json_str': '{"fieldA1": "A1", "fieldB1": 21}'
            },
            unique={
                'name': 'chan_2',
                'ID': 2,
                'datatype': FDT.I32,
                'enum': None,
                'bitfields':  None,
                'metadata_json_str': '{"fieldA2": "A2", "fieldB2": 22}'
            },
            ignore_none=True
        )
        # Test for enum channel:
        self._equality_test(
            cls=TelemetryChannel,
            source={
                'name': 'chan_one',
                'ID': 0x01,
                'datatype': FDT.ENUM,
                'enum': self.sample_enum_1,
                'bitfields':  None,
                'metadata_json_str': '{"fieldA1": "A1", "fieldB1": 21}'
            },
            unique={
                'name': 'chan_2',
                'ID': 2,
                'datatype': FDT.ENUM,
                'enum': self.sample_enum_2,
                'bitfields':  None,
                'metadata_json_str': '{"fieldA2": "A2", "fieldB2": 22}'
            },
            ignore_none=True,
            flag_duplicates=False
        )

        # Test for bitfield args:
        self._equality_test(
            cls=TelemetryChannel,
            source={
                'name': 'chan_one',
                'ID': 0x01,
                'datatype': FDT.U32,
                'enum': None,
                'bitfields': BitfieldStruct(
                    fields=OrderedDict([('a', 1), ('b', 2)]),
                    total_padding=29
                ),
                'metadata_json_str': '{"fieldA1": "A1", "fieldB1": 21}'
            },
            unique={
                'name': 'chan_2',
                'ID': 2,
                'datatype': FDT.I32,
                'enum': None,
                'bitfields': BitfieldStruct(
                    fields=OrderedDict([('a', 4), ('b', 1)]),
                    total_padding=27
                ),
                'metadata_json_str': '{"fieldA2": "A2", "fieldB2": 22}'
            },
            ignore_none=True
        )

    def test_equals_event(self):
        self._equality_test(
            cls=Event,
            source={
                'name': 'EventA',
                'ID': 1,
                'severity_str': 'warning_hi',
                'format_string': r"Event a occurred at %d and %d!",
                'args': self.sample_args_1,
                'metadata_json_str': '{"fieldA1": "A1", "fieldB1": 21}'
            },
            unique={
                'name': 'Event2',
                'ID': 0x02,
                'format_string': r"Event 2 occurred at %d and %d!",
                'severity_str': 'ACTIVITY_LO',
                'args': self.sample_args_2,
                'metadata_json_str': '{"fieldA2": "A2", "fieldB2": 22}'
            }
        )

    def test_equals_module(self):
        self._equality_test(
            cls=Module,
            source={
                'name': 'SampleModule',
                'ID': 0xBE,
                'commands': NameIdDict({
                    (0x01, "SampleModule_CommandA"): Command(
                        name="SampleModule_CommandA",
                        mnemonic="command_a",
                        ID=1,
                        args=self.sample_args_1,
                        metadata_json_str='{"fieldA": "A", "fieldB": 2}'
                    ),
                    (0x02, "SampleModule_CommandB"): Command(
                        name="SampleModule_CommandB",
                        mnemonic="command_b",
                        ID=2,
                        args=self.sample_args_2,
                        metadata_json_str='{"fieldA": "A", "fieldB": 2}'
                    )
                }),
                'telemetry': NameIdDict({
                    (0x01, "ChannelA"): TelemetryChannel(
                        name="ChannelA",
                        ID=1,
                        datatype=FDT.I16,
                        metadata_json_str='{"fieldA1": "A1", "fieldB1": 21, "fieldNone": null}'
                    ),
                    (0x02, "ChannelB"): TelemetryChannel(
                        name="ChannelB",
                        ID=2,
                        datatype=FDT.ENUM,
                        enum=self.sample_enum_1,
                        metadata_json_str='{"fieldA2": "A2", "fieldB2": 22, "fieldNone": null}'
                    )
                }),
                'events': NameIdDict({
                    (0x00, "EventA"): Event(
                        name="EventA",
                        ID=0,
                        severity_str="ACTIVITY_HI",
                        format_string=r"Event a occurred at %d, %d, and %d!",
                        args=self.sample_args_1,
                        metadata_json_str='{"fieldA": "A", "fieldB": 2}'
                    ),
                    (0x01, "EventB"): Event(
                        name="EventB",
                        ID=1,
                        severity_str="ACTIVITY_HI",
                        format_string=r"Event b occurred at %d, %d, and %d!",
                        args=self.sample_args_2,
                        metadata_json_str='{"fieldA": "A", "fieldB": 2}'
                    ),
                })
            },
            unique={
                'name': 'AnotherModule',
                'ID': 0xEF,
                'commands': NameIdDict({
                    (0x02, "AnotherModule_CommandB"): Command(
                        name="AnotherModule_CommandB",
                        mnemonic="command_b",
                        ID=2,
                        args=self.sample_args_1,
                        metadata_json_str='{"fieldA": "A", "fieldB": 2}'
                    )
                }),
                'telemetry': NameIdDict({
                    (0x01, "ChannelA"): TelemetryChannel(
                        name="ChannelA",
                        ID=1,
                        datatype=FDT.ENUM,
                        enum=self.sample_enum_2,
                        metadata_json_str='{"fieldA1": "A1", "fieldB1": 21, "fieldNone": null}'
                    )
                }),
                'events': NameIdDict({
                    (0x00, "EventA"): Event(
                        name="EventA",
                        ID=0,
                        severity_str="ACTIVITY_HI",
                        format_string=r"Event a occurred at %d, %d, and %d!",
                        args=self.sample_args_2,
                        metadata_json_str='{"fieldA": "A", "fieldB": 2}'
                    )
                })
            }
        )


class SerializationSuite(object):
    """Verify serialization works as expected for all data classes."""

    @staticmethod
    def _serialization_test(obj: object):
        """Perform a basic serialize-deserialize test on the given object."""
        serialized = jsonpickle.dumps(obj)
        reconstructed = jsonpickle.loads(serialized)
        assert reconstructed == obj, (
            f"Object {obj} failed to be reconstructed from the jsonpickle "
            f"serialization of itself '{serialized}'. Instead, it was "
            f"reconstructed into {reconstructed}"
        )

    @pytest.fixture(autouse=True)
    def _setup(self):
        """Common setup. Performed once per test."""
        self.sample_module_name = "SampleModule"

        self.expected_enum = [
            EnumItem(
                name='FIELD_0',
                value=0,
                comment=""
            ),
            EnumItem(
                name='FIELD_1',
                value=1,
                comment="I am alive."
            ),
            EnumItem(
                name='FIELD_FOUR',
                value=4,
                comment=""
            ),
            EnumItem(
                name='FIELD_5',
                value=5,
                comment="I am a five."
            )
        ]

        self.expected_args = [
            Argument(
                name='arg_1', datatype=FDT.U8, metadata_json_str='{"fieldA1": "A1", "fieldB1": 21}'
            ),
            Argument(
                name='arg_2', datatype=FDT.U16, metadata_json_str='{"fieldA2": "A2", "fieldB2": 22}'
            ),
            Argument(
                name='arg_3',
                datatype=FDT.ENUM,
                enum=self.expected_enum,
                metadata_json_str='{"fieldA3": "A3", "fieldB3": 23}'
            )
        ]

        self.expected_commands: NameIdDict[Command] = NameIdDict({
            (0x01, f"{self.sample_module_name}_CommandA"): Command(
                name=f"{self.sample_module_name}_CommandA",
                mnemonic="command_a",
                ID=1,
                args=self.expected_args,
                metadata_json_str='{"fieldA": "A", "fieldB": 2}'
            ),
            (0x02, f"{self.sample_module_name}_CommandB"): Command(
                name=f"{self.sample_module_name}_CommandB",
                mnemonic="command_b",
                ID=2,
                args=self.expected_args,
                metadata_json_str='{"fieldA": "A", "fieldB": 2}'
            )
        })

        self.expected_telem: NameIdDict[TelemetryChannel] = NameIdDict({
            (0x01, "ChannelA"): TelemetryChannel(
                name="ChannelA",
                ID=1,
                datatype=FDT.I16,
                metadata_json_str='{"fieldA1": "A1", "fieldB1": 21, "fieldNone": null}'
            ),
            (0x02, "ChannelB"): TelemetryChannel(
                name="ChannelB",
                ID=2,
                datatype=FDT.ENUM,
                enum=self.expected_enum,
                metadata_json_str='{"fieldA2": "A2", "fieldB2": 22, "fieldNone": null}'
            )
        })

        self.expected_events: NameIdDict[Event] = NameIdDict({
            (0x00, "EventA"): Event(
                name="EventA",
                ID=0,
                severity_str="ACTIVITY_HI",
                format_string=r"Event a occurred at %d, %d, and %d!",
                args=self.expected_args,
                metadata_json_str='{"fieldA": "A", "fieldB": 2}'
            ),
            (0x01, "EventB"): Event(
                name="EventB",
                ID=1,
                severity_str="ACTIVITY_HI",
                format_string=r"Event b occurred at %d, %d, and %d!",
                args=self.expected_args,
                metadata_json_str='{"fieldA": "A", "fieldB": 2}'
            ),
        })

        self.expected_module: Module = Module(
            name=self.sample_module_name,
            ID=0xBE,
            commands=self.expected_commands,
            telemetry=self.expected_telem,
            events=self.expected_events
        )

    def test_reconstruct_bitfield_struct(self):
        self._serialization_test(
            BitfieldStruct(
                fields=OrderedDict([('a', 4), ('b', 2), ('c', 28)]),
                total_padding=30
            )
        )

    def test_reconstruct_enum_item(self):
        for ei in self.expected_enum:
            self._serialization_test(ei)

    def test_reconstruct_enum(self):
        self._serialization_test(self.expected_enum)

    def test_reconstruct_arg(self):
        for arg in self.expected_args:
            self._serialization_test(arg)

    def test_reconstruct_args(self):
        self._serialization_test(self.expected_args)

    def test_reconstruct_command(self):
        for command in self.expected_commands.vals:
            self._serialization_test(command)

    def test_reconstruct_telemetry_channel(self):
        for channel in self.expected_telem.vals:
            self._serialization_test(channel)

    def test_reconstruct_event(self):
        for event in self.expected_events.vals:
            self._serialization_test(event)

    def test_reconstruct_module(self):
        self._serialization_test(self.expected_module)


class ConstructorExceptionsSuite(object):
    """Verify constructor exceptions work correctly for all data classes."""

    def test_bitfield_struct_exceptions(self):
        # Bad number of bits:
        bad_size = BitfieldStruct.STORAGE_UNIT_SIZE + 1 + 2
        with pytest.raises(ValueError, match=rf"[\s\S]*storage unit size[\s\S]*{bad_size}b[\s\S]*"):
            BitfieldStruct(
                fields=OrderedDict([('a', 1), ('b', 2)]),
                total_padding=BitfieldStruct.STORAGE_UNIT_SIZE
            )
            pytest.fail(
                "`BitfieldStruct` ctor failed to flag that total number of bits "
                "wasn't a multiple of the storage unit size."
            )

    def test_arg_exceptions(self):
        # Enum type mismatch:
        with pytest.raises(ValueError, match=r"[\s\S]*[Dd]atatype[\s\S]*enum[\s\S]*"):
            Argument(
                name='arg_one',
                datatype=FDT.U16,
                enum=[
                    EnumItem(
                        name='FIELD_FOUR',
                        value=4,
                        comment=""
                    ),
                    EnumItem(
                        name='FIELD_5',
                        value=5,
                        comment="I am a five."
                    )
                ]
            )
            pytest.fail(
                "`Argument` ctor failed to flag an enum argument which isn't "
                "using the `ENUM` FswDataType."
            )

        # Bitfield size mismatch:
        with pytest.raises(ValueError, match=r"[\s\S]*bitfield[\s\S]*"):
            Argument(
                name='arg_one',
                datatype=FDT.U64,
                bitfields=BitfieldStruct(
                    fields=OrderedDict([('a', 1), ('b', 3)]),
                    total_padding=28
                )
            )
            pytest.fail(
                "`Argument` ctor failed to flag an bitfield argument where the "
                "`datatype` size doesn't match the `bitfield` size."
            )

    def test_telemetry_channel_exceptions(self):
        # Enum type mismatch:
        with pytest.raises(ValueError, match=r"[\s\S]*[Dd]atatype[\s\S]*enum[\s\S]*"):
            TelemetryChannel(
                name='chan_one',
                ID=0x01,
                datatype=FDT.U16,
                enum=[
                    EnumItem(
                        name='FIELD_FOUR',
                        value=4,
                        comment=""
                    ),
                    EnumItem(
                        name='FIELD_5',
                        value=5,
                        comment="I am a five."
                    )
                ]
            )
            pytest.fail(
                "`TelemetryChannel` ctor failed to flag an enum argument which "
                "isn't using the `ENUM` FswDataType."
            )

        # Bitfield size mismatch:
        with pytest.raises(ValueError, match=r"[\s\S]*bitfield[\s\S]*"):
            TelemetryChannel(
                name='chan_one',
                ID=0x01,
                datatype=FDT.U64,
                bitfields=BitfieldStruct(
                    fields=OrderedDict([('a', 1), ('b', 3)]),
                    total_padding=28
                )
            )
            pytest.fail(
                "`TelemetryChannel` ctor failed to flag an bitfield argument "
                "where the `datatype` size doesn't match the `bitfield` size."
            )

    def test_event_exceptions(self):
        # Bad Severity String:
        with pytest.raises(ValueError, match=r"[\s\S]*SeverityLevel[\s\S]*"):
            Event(
                name='event_a',
                ID=0x60,
                severity_str="".join([sl.name for sl in Event.SeverityLevel])
            )
            pytest.fail(
                "`Event` ctor failed to flag an invalid severity string, "
                "which doesn't correspond to any `SeverityLevel`, as being "
                "invalid.")
