#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Loads and Stores Collection of Standard Definitions for All Uplinked and 
Downlinked Data in accordance with the Command & Telemetry List (C&TL)

TODO: Break up into small testable units (you idiot <-me).

@author: Connor W. Colombo (CMU)
@last-updated: 01/20/2021
"""

# Activate postponed annotations (for using classes as return type in their own methods)
from __future__ import annotations

from typing import List, Optional, Dict, Any, TypeVar, Generic, Union, Tuple, Mapping, MutableMapping, Iterator
from collections import abc
import warnings

import os.path
from lxml import etree

from .exceptions import StandardsFormattingException

from IrisBackendv3.utils import NameIdDict
from IrisBackendv3.utils.basic import name_split_and_format

from .module import Module, EnumItem, Argument, Command, Telemetry, Event
from .fsw_data_type import FswDataType
from . import gsw_metadata_tools as GswMetadataTools


def extract_arguments(node: etree.Element) -> List[Argument]:
    """
    Extract all arguments from the given XML node as a List of Command Arguments.

    XML Node should be a `<command>` or `<event>` containing an <args> element 
    containing <arg> elements.
    """
    arg_tree = node.xpath("args/arg")

    # Build Arguments:
    args: List[Argument] = []
    for arg_src in arg_tree:
        name = arg_src.attrib['name']
        try:
            datatype = FswDataType[arg_src.attrib['type']]
        except:
            raise TypeError(
                f"At sourceline {arg_src.sourceline}, given type "
                f"`{arg_src.attrib['type']}` is not a known FSW type."
            )

        # Extract and Build Enum if Present:
        enum: List[EnumItem] = []
        if len(arg_src.xpath("enum")) > 1:
            raise StandardsFormattingException(
                arg_src.base,
                f"At {arg_src.sourceline}, argument has multiple <enum> children "
                " but only one is allowed."
            )

        enum_items = arg_src.xpath("enum/item")
        if len(enum_items) > 0:
            # If there are enum items, make sure datatype is correct.
            if datatype != FswDataType.ENUM:
                raise StandardsFormattingException(
                    arg_src.base,
                    f"At {arg_src.sourceline}, argument has an <enum> child but "
                    f"its type is listed as {datatype} not {FswDataType.ENUM} "
                    f"as is required."
                )

            for i, item_src in enumerate(enum_items):
                # Get value:
                # (use `base=0` to automatically recognize base (likely either 10 or 16))
                val = int(str(item_src.attrib.get('value', i)), base=0)
                # Build EnumItem:
                enum.append(EnumItem(
                    name=item_src.attrib['name'],
                    value=val,
                    comment=item_src.attrib.get('comment', "")
                ))

        metadata = GswMetadataTools.extract_from_xml(arg_src)
        args.append(Argument(
            name=name,
            datatype=datatype,
            enum=enum,
            metadata_json_str=metadata
        ))

    return args


def extract_commands(node: etree.Element, module_name: str) -> NameIdDict[Command]:
    """
    Extract all commands from the given XML node as a NaN of Command Arguments.

    XML Node should contain a `<commands>` element containing <command> elements.
    Eg. would be a <component> element.
    """
    command_tree = node.xpath("//commands/command")

    commands: NameIdDict[Command] = NameIdDict()
    for command_src in command_tree:
        command = Command()

        # Grab Mnemonic:
        command.mnemonic = command_src.attrib['mnemonic']

        # Create UI Name:
        # Format: ModuleName_FPrimeCommandMnemonic
        formatted_module_name = "".join(
            name_split_and_format(module_name)
        )
        formatted_mnemonic = "".join(
            name_split_and_format(command.mnemonic)
        )
        command.name = f"{formatted_module_name}_{formatted_mnemonic}"

        # Grab ID ("opcode" in FPrime):
        # (use `base=0` to automatically recognize base (likely either 10 or 16))
        command.ID = int(str(command_src.attrib['opcode']), base=0)

        # Grab Metadata:
        command.metadata_json_str = GswMetadataTools.extract_from_xml(
            command_src
        )

        # Grab Args:
        command.args = extract_arguments(command_src)

        # Store Command:
        if (command.ID, command.name) in commands:
            # If command with same index is already stored, flag it.
            warnings.warn(
                f"Duplicate of existing command {command} "
                "with NameIdDict index [{command.ID, command.name}]"
            )

        commands[command.ID, command.name] = command

    return commands


def extract_telemetry(node: etree.Element, module_name: str) -> NameIdDict[Telemetry]:
    """
    Extract all telemetry channels from the given XML node as a NameIdDict of Command Arguments.

    XML Node should contain a `<telemetry>` element containing <channel> elements.
    Eg. would be a <component> element.
    """
    # Grab component telemetry channels:
    xpath_telemetry = """/component/telemetry/channel"""
    raise NotImplementedError()


def extract_events(node: etree.Element, module_name: str) -> NameIdDict[Event]:
    """
    Extract all events from the given XML node as a NameIdDict of Command Arguments.

    XML Node should contain a `<events>` element containing <event> elements.
    Eg. would be a <component> element.
    """
    # Grab component events:
    xpath_events = """/component/events/event"""
    raise NotImplementedError()


class DataStandards(object):
    """Loads and Stores the Standard Definitions for All Uplinked and Downlinked Data."""

    __slots__: List[str] = [
        'modules'  # - Modules indexed by opcode (highest 1B)
    ]

    def __init__(self,  # DataStandards
                 modules: NameIdDict[Module]
                 ) -> None:
        self.modules = modules
        pass

    def __getitem__(self, key):
        # TODO: Get in any module (collection,indexer)
        # ... or just getCommand(), getTelem(), getEvent()
        pass

    def cache(self,
              cache_dir: str = './DataStandardsCache'
              ) -> str:
        """
        Cache this DataStandards instance in a unique file in `cache_dir`.

        Returns the unique filename.
        """
        # TODO
        pass

    @classmethod
    def load_cache(cls,
                   cache_dir: str = './DataStandardsCache',
                   cache_name: Optional[str] = None
                   ) -> DataStandards:
        """
        Create a new DataStandards instance by loading a cached file from `cache_dir`.

        The loaded file will be the latest cache in `cache_dir` unless the 
        name of specific cache file is supplied with `cache_name`.
        """
        # TODO
        pass

    @classmethod
    def build_standards(cls,
                        search_dir: str = '../../../FlightSoftware/fprime',
                        cache_dir: str = './DataStandardsCache'
                        ) -> DataStandards:
        """
        Create a new DataStandards instance by indexing the relevant standard definitions.

        Standard definition files are held in `search_dir` (should point to the 
        FSW's `fprime/` directory). 
        The sole source of truth for which files to index and how to process 
        them lies in this function and its subroutines.

        Files are defined purely in the C&TL. Since they are just binary packed 
        files and are all packed the same way, the only relevant part of the 
        standard is the magic and the associated file extension.

        The following standards are defined in XML files imported from the topology file:
        - Commands, with GSW-specific metadata defined in <![CDATA[]]> tag in <comment>
        - Telemetry, with GSW-specific metadata defined in <![CDATA[]]> tag in <comment>
        - Events (aka Logs), with GSW strings being parsed using the `format_string property`.

        Note: Bitfields are defined in [ComponentName]_Bitfields.hpp within each
        Component's directory (directory containing the component's XML file).
        If a component directory doesn't contain a Bitfields file, it is assumed 
        to not have a bitfield.

        Once built, the finalized DataStandards instance is cached in 
        `cache_dir` without overriding previous caches.
        """
        ###
        # Settings:
        ###
        # Relative Locations of Import Definition Files and Directories w.r.t. `search_dir`:
        uri_topology = './CubeRover/Top/CubeRoverTopologyAppAi.xml'

        ###
        # Important Tree XPath Selectors:
        ###
        # Select all file imports:
        xpath_imports = """/assembly/import_component_type/text()"""

        # Grab a component's name from file:
        xpath_component_name = """/component/@name"""
        # Grab component's corresponding ID from topology:
        xpath_component_id = """/assembly/instance[@type='{name}']/@base_id"""

        # Load Important Trees:
        tree_topology = etree.parse(os.path.join(search_dir, uri_topology))

        ###
        # Setup:
        ###
        # Create empty modules container:
        modules: NameIdDict[Module] = NameIdDict()

        ###
        # Load:
        ###
        # Grab all XML files referenced in Topology:
        topology_imports = tree_topology.xpath(xpath_imports)

        # Examine each XML import and include it in the standards if it has
        # *any* <commands>, <telemetry>, or <events>:
        for uri in topology_imports:
            try:
                tree = etree.parse(os.path.join(search_dir, uri))
            except OSError as e:
                raise StandardsFormattingException(
                    uri,
                    (
                        f"This file was included in topology but failed to load. Does it exist?\n"
                        f"Raised error: {e}."
                    )
                )

            # TODO: Deep import all XML referenced in each file (and append where it was included)
            # ... loop through all nodes, if import, try just replacing node with root?

            # Grab component name:
            name_list = tree.xpath(xpath_component_name)
            # Make sure exactly one name was found:
            if len(name_list) == 0:
                raise StandardsFormattingException(
                    uri,
                    f"No name found for component at {tree.sourceline}."
                )
            elif len(name_list) > 1:
                raise StandardsFormattingException(
                    uri,
                    f"Somehow multiple names found ({name_list}) for component at {tree.sourceline}."
                )
            else:
                module_name = name_list[0]

            commands = extract_commands(tree, module_name)
            telemetry = extract_telemetry(tree, module_name)
            events = extract_events(tree, module_name)

            if len(commands) > 0 or len(telemetry) > 0 or len(events) > 0:
                # Proceed if there are any valid <command>s, <channel>s, or <event>s:

                # Grab corresponding ID from topology tree:
                try:
                    ID_selector = xpath_component_id.format(name=module_name)
                    module_id = tree_topology.xpath(ID_selector)[0]
                except IndexError:
                    raise StandardsFormattingException(
                        uri,
                        f"Failed to find an ID in topology for the module named {module_name}."
                    )

                # Build Module:
                modules[module_id, module_name] = Module(
                    name=module_name,
                    ID=module_id,
                    commands=commands,
                    telemetry=telemetry,
                    events=events
                )

        # Build and Return the DataStandards Object:
        return cls(modules)
