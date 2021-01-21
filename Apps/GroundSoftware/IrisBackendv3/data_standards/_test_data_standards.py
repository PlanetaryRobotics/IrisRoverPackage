# -*- coding: utf-8 -*-
"""
Test all possible NameIdDict use cases.

It's important for these tests to be rigorous since this class forms the
backbone of the Backend DataStandards.

@author: Connor W. Colombo (CMU)
@last-updated: 01/20/2021
"""
import pytest
from typing import Any, List, Union, Tuple

from lxml import etree
from string import Template

from .data_standards import *
from .module import EnumItem, Argument, Command, Telemetry, Event
from .fsw_data_type import FswDataType as FDT


@pytest.fixture(scope="class")
def suite_setup(request):
    # Load Test Data:
    request.cls.file_uri = "../../test-data/SampleFPrimeXml.xml"


@pytest.mark.usefixtures('suite_setup')
class ExtractionSuite(object):
    """Verify all extraction functions work as expected."""

    @pytest.fixture(autouse=True)
    def _setup(self):
        """Common setup. Performed once per test."""
        self.sample_module_name = "SampleModule"
        self.sample_args = r"""
            <args>
                <arg name="arg1" type="U8">
                    <comment>
                        FPrime Description of Argument 1.
                        <![CDATA[{JSONSTART{{
                            "fieldA1": "A1",
                            "fieldB1": 21
                        }}JSONEND}]]>
                    </comment>
                </arg>
                <arg name="arg2" type="U16">
                    <comment>
                        FPrime Description of Argument 2.
                        <![CDATA[{JSONSTART{{
                            "fieldA2": "A2",
                            "fieldB2": 22
                        }}JSONEND}]]>
                    </comment>
                </arg>
                <arg name="arg3" type="ENUM">
                    <enum name="enum_one">
                        <item name="field_0"/>
                        <item name="field_1" comment="I am alive."/>
                        <item name="field_four" value="0x04"/>
                        <item name="field_5" comment="I am a five." value="5"/>
                    </enum>
                    <comment>
                        FPrime Description of Argument 3.
                        <![CDATA[{JSONSTART{{
                            "fieldA3": "A3",
                            "fieldB3": 23
                        }}JSONEND}]]>
                    </comment>
                </arg>
             </args>
        """

        self.sample_command = Template("""
        <command kind="sync" opcode="$ID" mnemonic="command_$letter">
            <comment>
                FPrime Description of Command $letter.
                <![CDATA[{JSONSTART{{
                    "fieldA": "A",
                    "fieldB": 2
                }}JSONEND}]]>
            </comment>

            $args
        </command>
        """).substitute(args=self.sample_args, ID='$ID', letter='$letter')

        self.sample_component = f"""
        <component name="{self.sample_module_name}" kind="passive" namespace="CubeRover">
            <commands>
             {Template(self.sample_command).substitute(ID ='0x01', letter='a')}
             {Template(self.sample_command).substitute(ID ='2', letter='b')}
            </commands>
        </component>
        """

        self.expected_args = [
            Argument(
                name='arg1', datatype=FDT.U8, metadata_json_str='{"fieldA1": "A1", "fieldB1": 21}'
            ),
            Argument(
                name='arg2', datatype=FDT.U16, metadata_json_str='{"fieldA2": "A2", "fieldB2": 22}'
            ),
            Argument(
                name='arg3',
                datatype=FDT.ENUM,
                enum=[
                    EnumItem(
                        name='field_0',
                        value=0,
                        comment=""
                    ),
                    EnumItem(
                        name='field_1',
                        value=1,
                        comment="I am alive."
                    ),
                    EnumItem(
                        name='field_four',
                        value=4,
                        comment=""
                    ),
                    EnumItem(
                        name='field_5',
                        value=5,
                        comment="I am a five."
                    )
                ],
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

    def test_extract_arguments(self):
        tree = etree.fromstring(self.sample_command)
        extracted_args = extract_arguments(tree)
        assert extracted_args == self.expected_args, f"""
            Extracted arguments don't match expected arguments.
            Expected: {[(a, a.metadata) for a in self.expected_args]}
            Extracted: {[(a, a.metadata) for a in extracted_args]}
        """

    def test_extract_commands(self):
        tree = etree.fromstring(self.sample_component)
        extracted_commands = extract_commands(tree, self.sample_module_name)
        assert extracted_commands == self.expected_commands, f"""
            Extracted arguments don't match expected arguments.
            \nExpected: {self.expected_commands} \n-> {[(c, c.metadata) for c in self.expected_commands.vals]}
            \nExtracted: {extracted_commands} \n-> {[(c, c.metadata) for c in extracted_commands.vals]}
        """
