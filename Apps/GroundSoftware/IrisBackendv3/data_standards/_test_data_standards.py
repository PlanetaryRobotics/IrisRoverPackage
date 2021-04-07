# -*- coding: utf-8 -*-
"""
Test all the structural functions of the `data_standards` module.

@author: Connor W. Colombo (CMU)
@last-updated: 04/05/2021
"""
import pytest
from typing import Any, List, Union, Tuple

from lxml import etree
from string import Template

from .data_standards import *
from .module import EnumItem, Argument, Command, TelemetryChannel, Event
from .fsw_data_type import FswDataType as FDT

from IrisBackendv3.utils.basic import name_split_and_format


@pytest.fixture(scope="class")
def suite_setup(request):
    # Load Test Data:
    request.cls.xml_uri = "../../test-data/SampleFPrimeXml.xml"


#! TODO: Add tests for `FswDataType.INVALID` arguments and the removal of commands / error of events/telemetry with that type.

@pytest.mark.usefixtures('suite_setup')
class ExtractionSuite(object):
    """Verify all extraction functions work as expected."""

    @pytest.fixture(autouse=True)
    def _setup(self):
        """Common setup. Performed once per test."""
        self.sample_module_name = "SampleModule"
        self.sample_module_ID = '0xbe'

        self.sample_enum = r"""
                    <enum name="enum_one">
                        <item name="field_0"/>
                        <item name="field_1" comment="I am alive."/>
                        <item name="field_four" value="0x04"/>
                        <item name="field_5" comment="I am a five." value="5"/>
                    </enum>
                    """

        self.sample_args = Template(r"""
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
                    $enum
                    <comment>
                        FPrime Description of Argument 3.
                        <![CDATA[{JSONSTART{{
                            "fieldA3": "A3",
                            "fieldB3": 23
                        }}JSONEND}]]>
                    </comment>
                </arg>
             </args>
        """).substitute(enum=self.sample_enum)

        self.sample_command = Template(r"""
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

        self.sample_telem = Template(r"""
        <telemetry>
            <channel id="1" name="channel_a" data_type="I16" update="on_change">
                <comment>
                    FPrime Description of Telemetry Channel a.
                    <![CDATA[{JSONSTART{{
                        "fieldA1": "A1",
                        "fieldB1": 21,
                        "fieldNone": null
                    }}JSONEND}]]>
                </comment>
            </channel>
            <channel id="0x02" name="channel_b" data_type="ENUM" update="on_change">
                <comment>
                    FPrime Description of Telemetry Channel b.
                    <![CDATA[{JSONSTART{{
                        "fieldA2": "A2",
                        "fieldB2": 22,
                        "fieldNone": null
                    }}JSONEND}]]>
                </comment>
                $enum
            </channel>
        </telemetry>
        """).substitute(enum=self.sample_enum)

        self.sample_event = Template(r"""
        <event id="$ID" name="event_$letter" severity="ACTIVITY_HI" format_string="Event $letter occurred at %d, %d, and %d!">
            <comment>
                FPrime Description of Event $letter.
                <![CDATA[{JSONSTART{{
                    "fieldA": "A",
                    "fieldB": 2
                }}JSONEND}]]>
            </comment>

            $args
        </event>
        """).substitute(args=self.sample_args, ID='$ID', letter='$letter')

        self.sample_module = f"""
        <component name="{self.sample_module_name}" kind="passive" namespace="CubeRover">
            <commands>
                {Template(self.sample_command).substitute(ID ='0x01', letter='a')}
                {Template(self.sample_command).substitute(ID ='2', letter='b')}
            </commands>
            {self.sample_telem}
            <events>
                {Template(self.sample_event).substitute(ID ='0x00', letter='a')}
                {Template(self.sample_event).substitute(ID ='1', letter='b')}
            </events>
        </component>
        """

        self.sample_topology = f"""
        <assembly name="CubeRover">
            <import_component_type>Not/A/Real/Address/SomeOther/SomeOtherComponentAi.xml</import_component_type>
            <import_component_type>Not/A/Real/Address/{self.sample_module_name}/{self.sample_module_name}ComponentAi.xml</import_component_type>
            <import_component_type>Not/A/Real/Address/YetAnother/YetAnotherComponentAi.xml</import_component_type>
            
            <instance namespace="CubeRover" name="some_other_component" type="SomeOtherComponent" base_id="0xc0" base_id_window="255" />
            <instance namespace="CubeRover" name="{'_'.join(name_split_and_format(self.sample_module_name)).lower()}" type="{self.sample_module_name}" base_id="{self.sample_module_ID}" base_id_window="255" />
            <instance namespace="CubeRover" name="yet_another_component" type="YetAnotherComponent" base_id="0x01" base_id_window="255" />
        </assembly>
        """

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
            ID=parse_int(self.sample_module_ID),
            commands=self.expected_commands,
            telemetry=self.expected_telem,
            events=self.expected_events
        )

    def test_extract_arguments(self):
        tree = etree.fromstring(self.sample_command)
        extracted_args = extract_arguments(tree)
        assert extracted_args == self.expected_args, f"""
            Extracted arguments don't match expected arguments.
            Expected: {[(a, a.metadata) for a in self.expected_args]}
            Extracted: {[(a, a.metadata) for a in extracted_args]}
        """

    def test_extract_commands(self):
        tree = etree.fromstring(self.sample_module)
        extracted_commands = extract_commands(tree, self.sample_module_name)
        assert extracted_commands == self.expected_commands, f"""
            Extracted commands don't match expected commands.
            \nExpected: {self.expected_commands} \n-> {[(c, c.metadata) for c in self.expected_commands.vals]}
            \nExtracted: {extracted_commands} \n-> {[(c, c.metadata) for c in extracted_commands.vals]}
        """

    def test_extract_telemetry(self):
        tree = etree.fromstring(self.sample_module)
        extracted_telem = extract_telemetry(tree)
        assert extracted_telem == self.expected_telem, f"""
            Extracted telemetry channels don't match expected telemetry channels.
            \nExpected: {self.expected_telem} \n-> {[(ch, ch.metadata) for ch in self.expected_telem.vals]}
            \nExtracted: {extracted_telem} \n-> {[(ch, ch.metadata) for ch in extracted_telem.vals]}
        """

    def test_extract_events(self):
        tree = etree.fromstring(self.sample_module)
        extracted_events = extract_events(tree)
        assert extracted_events == self.expected_events, f"""
            Extracted events don't match expected events.
            \nExpected: {self.expected_events} \n-> {[(e, e.metadata) for e in self.expected_events.vals]}
            \nExtracted: {extracted_events} \n-> {[(e, e.metadata) for e in extracted_events.vals]}
        """

    def test_build_module(self):
        topology_tree = etree.fromstring(self.sample_topology)
        module_node = etree.fromstring(self.sample_module)
        built_module = build_module(module_node, topology_tree)
        assert built_module == self.expected_module, f"""
            Built module doesn't match expected module.
            \nExpected: {self.expected_module}
            \Built: {built_module}
        """


@pytest.mark.usefixtures('suite_setup')
class ImportSuite(object):
    """Verify all file finding and importing functions work as expected."""

    @pytest.fixture(autouse=True)
    def _setup(self):
        # Default directory to search for files in:
        self._SEARCH_DIR: str = './test-data'
        # Relative Location of FPrime XML Topology File w.r.t. `_SEARCH_DIR`:
        self._URI_TOPOLOGY: str = './SampleFPrimeXmlTopology.xml'
        # Relative Location of FPrime XML Sample Component File w.r.t. `_SEARCH_DIR`:
        self._URI_COMPONENT: str = './SampleFPrimeXml.xml'

    def test_import_all_fprime_xml(self):
        # TODO: Build out Sample Test files, using imports (make sure using right names)... also, throw in some imports with <?xml?> and .hpp

        expected_commands: NameIdDict[Command] = NameIdDict({
            (0x00, "Camera_GetStatus"): Command(
                name='Camera_GetStatus',
                mnemonic='Get_Status',
                ID=0x00,
                metadata_json_str="""{
                    "description": "Get camera status.",
                    "YAMCS_queue": "dependent"
                }"""
            ),
            (0x01, "Camera_TakeImage"): Command(
                name='Camera_TakeImage',
                mnemonic='Take_Image',
                ID=0x01,
                metadata_json_str="""{
                    "description": "I am a command.",
                    "YAMCS_queue": "much_danger"
                }""",
                args=[
                    Argument(
                        name="camera_num",
                        datatype=FDT.U8,
                        metadata_json_str="""{
                            "hello": "sir",
                            "i_am": "Test Data",
                            "BEEP": "0xB009",
                            "enum": {
                                "FRONT": 0,
                                "REAR": 1
                            }
                        }"""
                    ),
                    Argument(
                        name="callback_id",
                        datatype=FDT.U16,
                        metadata_json_str="""{
                            "hello": "sir",
                            "i_am": "Test Data",
                            "BEEP": "0xB009",
                            "enum": {
                                "FRONT": 0,
                                "REAR": 1
                            }
                        }"""
                    )
                ]
            ),
            (0x02, "Camera_Error"): Command(
                name='Camera_Error',
                mnemonic='Error',
                ID=0x02,
                metadata_json_str="""{
                    "description": "I am a command.",
                    "YAMCS_queue": "much_danger"
                }""",
                args=[
                    Argument(
                        name='action',
                        datatype=FDT.U8,
                        metadata_json_str="""{
                            "hello": "sir",
                            "i_am": "Test Data",
                            "BEEP": "0xB009",
                            "enum": {
                                "CLEAR": 0,
                                "READ": 255
                            }
                        }"""
                    )
                ]
            ),
            (0x03, "Camera_ConfigureCamera0"): Command(
                name='Camera_ConfigureCamera0',
                mnemonic='Configure_Camera0',
                ID=0x03,
                metadata_json_str="""{
                    "description": "I am a command.",
                    "YAMCS_queue": "much_danger"
                }""",
                args=[
                    Argument(
                        name='config',
                        datatype=FDT.U64,
                        metadata_json_str="""{
                            "hello": "sir",
                            "i_am": "Test Data",
                            "BEEP": "0xB009",
                            "enum": {
                                "FRONT": 0,
                                "REAR": 1
                            }
                        }"""
                    )
                ]
            ),
            (0x04, "Camera_Camera0Crop"): Command(
                name='Camera_Camera0Crop',
                mnemonic='Camera0_Crop',
                ID=0x04,
                metadata_json_str="""{
                    "description": "I am a command.",
                    "YAMCS_queue": "much_danger"
                }""",
                args=[
                    Argument(
                        name='config',
                        datatype=FDT.U64,
                        metadata_json_str="""{
                            "hello": "sir",
                            "i_am": "Test Data",
                            "BEEP": "0xB009",
                            "enum": {
                                "FRONT": 0,
                                "REAR": 1
                            }
                        }"""
                    )
                ]
            ),
            (0x05, "Camera_ConfigureCamera1"): Command(
                name='Camera_ConfigureCamera1',
                mnemonic='Configure_Camera1',
                ID=0x05,
                metadata_json_str="""{
                    "description": "I am a command.",
                    "YAMCS_queue": "much_danger"
                }""",
                args=[
                    Argument(
                        name='config',
                        datatype=FDT.U64,
                        metadata_json_str="""{
                            "hello": "sir",
                            "i_am": "Test Data",
                            "BEEP": "0xB009",
                            "enum": {
                                "FRONT": 0,
                                "REAR": 1
                            }
                        }"""
                    )
                ]
            ),
            (0x06, "Camera_Camera1Crop"): Command(
                name='Camera_Camera1Crop',
                mnemonic='Camera1_Crop',
                ID=0x06,
                metadata_json_str="""{
                    "description": "I am a command.",
                    "YAMCS_queue": "much_danger"
                }""",
                args=[
                    Argument(
                        name='config',
                        datatype=FDT.U64,
                        metadata_json_str="""{
                            "hello": "sir",
                            "i_am": "Test Data",
                            "BEEP": "0xB009",
                            "enum": {
                                "FRONT": 0,
                                "REAR": 1
                            }
                        }"""
                    )
                ]
            ),
            (0x07, "Camera_ReadImage"): Command(
                name='Camera_ReadImage',
                mnemonic='Read_Image',
                ID=0x07,
                metadata_json_str="""{
                    "description": "I am a command.",
                    "YAMCS_queue": "much_danger"
                }""",
                args=[
                    Argument(
                        name='callback_id',
                        datatype=FDT.U16,
                        metadata_json_str="""{
                            "hello": "sir",
                            "i_am": "Test Data",
                            "BEEP": "0xB009",
                            "enum": {
                                "FRONT": 0,
                                "REAR": 1
                            }
                        }"""
                    )
                ]
            ),
            (0x08, "Camera_EraseImage"): Command(
                name='Camera_EraseImage',
                mnemonic='Erase_Image',
                ID=0x08,
                metadata_json_str="""{
                    "description": "I am a command.",
                    "YAMCS_queue": "much_danger"
                }"""
            ),
            (0x09, "Camera_SoftCameraReset"): Command(
                name='Camera_SoftCameraReset',
                mnemonic='Soft_Camera_Reset',
                ID=0x09,
                metadata_json_str="""{
                    "description": "I am a command.",
                    "YAMCS_queue": "much_danger"
                }""",
                args=[
                    Argument(
                        name='camera_num',
                        datatype=FDT.U8,
                        metadata_json_str="""{
                            "hello": "sir",
                            "i_am": "Test Data",
                            "BEEP": "0xB009",
                            "enum": {
                                "FRONT": 0,
                                "REAR": 1
                            }
                        }"""
                    )
                ]
            ),
            (0x0A, "Camera_ImageDump"): Command(
                name='Camera_ImageDump',
                mnemonic='Image_Dump',
                ID=0x0A,
                metadata_json_str="""{
                    "description": "I am a command.",
                    "YAMCS_queue": "much_danger"
                }"""
            )
        })

        # Grab Topology File:
        tree_topology = etree.parse(os.path.join(
            self._SEARCH_DIR, self._URI_TOPOLOGY))

        # Grab all XML files referenced in Topology:
        topology_imports = tree_topology.xpath(
            "/assembly/import_component_type/text()"
        )

        tree_root: etree.ElementTree = etree.parse(
            os.path.join(self._SEARCH_DIR, self._URI_COMPONENT))

        expanded_tree = import_all_fprime_xml(
            node=tree_root,
            search_dir=self._SEARCH_DIR
        )

        built_module = build_module(
            expanded_tree.xpath('//component')[0], tree_topology)

        assert built_module.commands == expected_commands, f"""
            The commands of the module built with imported XML don't match expected module's commands.
            \nExpected: {expected_commands}
            \Built: {built_module.commands}
        """

        #! TODO: Actually checkout the rest of the module (not just commands)

        #! TODO: Add more import checks (namely, the loop through components from topo part)
