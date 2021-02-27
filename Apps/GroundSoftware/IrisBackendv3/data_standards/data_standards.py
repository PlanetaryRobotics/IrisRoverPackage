#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Loads and Stores Collection of Standard Definitions for All Uplinked and
Downlinked Data in accordance with the Command & Telemetry List (C&TL)

The sole source of truth for which files to index and how to process
them lies in this python module.

Files are defined purely in the C&TL. Since they are just binary packed
files and are all packed the same way, the only relevant part of the
standard is the magic and the associated file extension.

The following standards are defined in XML files imported from the topology file:
- Commands, with GSW-specific metadata defined in <![CDATA[]]> tag in <comment>
- Telemetry, with GSW-specific metadata defined in <![CDATA[]]> tag in <comment>
- Events(aka Logs), with GSW strings being parsed using the `format_string property`.

Note: Bitfields are defined in [ComponentName]_Bitfields.hpp within each
Component's directory (directory containing the component's XML file).
If a component directory doesn't contain a Bitfields file, it is assumed
to not have a bitfield.

@author: Connor W. Colombo (CMU)
@last-updated: 02/01/2021
"""

# Activate postponed annotations (for using classes as return type in their own methods)
from __future__ import annotations

from typing import List, Optional, Dict, Any, TypeVar, Generic, Union, Tuple, Mapping, MutableMapping, Iterator
from collections import abc
import warnings

import os.path
from lxml import etree
import re

from .exceptions import StandardsFormattingException

from IrisBackendv3.utils import NameIdDict
from IrisBackendv3.utils.basic import name_split_and_format, name_split_and_format_by_term

from .module import Module, BitfieldStruct, EnumItem, Argument, Command, TelemetryChannel, Event
from .fsw_data_type import FswDataType
from . import gsw_metadata_tools as GswMetadataTools

# Settings:
# Default directory to search for files in:
_SEARCH_DIR: str = '../../../FlightSoftware/fprime'
# Default directory to store cached files in:
_CACHE_DIR: str = './DataStandardsCache'
# Relative Locations of FPrime XML Topology File w.r.t. `_SEARCH_DIR`:
_URI_TOPOLOGY: str = './CubeRover/Top/CubeRoverTopologyAppAi.xml'


def parse_int(number: Union[int, str]) -> int:
    """
    Parses the string representation of an int from any base (likely either 10 or 16).

    Written with a wrapper in case extra-parsing conditions or warnings want to
    be supported later.

    This works so long as it's given appropriate base markers (eg. "0b", "0o",
    "0x"). Supports taking integer arguments as well in case the field was
    pre-parsed earlier in a pipeline.
    """
    # (use `base=0` to automatically recognize base):
    return int(str(number), base=0)


def standardize_name(original_name: str) -> str:
    """
    Standardize the capitalization and spacing of the given name string.

    Used for all names except for a few special cases(fields and
    constants/enums), one example being all highlevel struct-like containers
    (Module, Command, Event, TelemetryChannel).
    """
    return "".join(name_split_and_format(original_name))


def standardize_field_name(original_name: str) -> str:
    """
    Standardize the capitalization and spacing of the given name string of any
    low-level field(Argument, Bitfield) of a highlevel struct-like container
    (Module, Command, Event, TelemetryChannel).
    """
    # Split into terms:
    terms = name_split_and_format_by_term(original_name)
    # Make all lowercase:
    terms = [t.lower() for t in terms]
    # Make snake-case:
    return "_".join(terms)


def standardize_constant_name(original_name: str) -> str:
    """
    Standardize the capitalization and spacing of the given name string of any
    constant-like name(eg. Enums).
    """
    # Split into terms:
    terms = name_split_and_format_by_term(original_name)
    # Make all uppercase:
    terms = [t.upper() for t in terms]
    # Make (constant) snake-case:
    return "_".join(terms)


def extract_type(node: etree.Element, attr_name: str) -> FswDataType:
    """
    Extracts the FSWDataType from the attribute `attr_name` inside the XML node `node`.

    Throws relevant error if attribute not found or invalid.
    """
    try:
        datatype = FswDataType[node.attrib[attr_name]]
    except KeyError:
        # Note: both unknown enum name and unknown attribute -> KeyError
        try:
            raise StandardsFormattingException(
                node.base,
                f"[TypeError]: At sourceline {node.sourceline}, given type in attribute "
                f"'{attr_name}' with value `{node.attrib[attr_name]}` is not "
                "a known `FSWDataType`."
            )
        except KeyError as e:
            raise StandardsFormattingException(
                node.base,
                f"[KeyError]: At sourceline {node.sourceline}, given attribute name "
                f"'{attr_name}' cannot be found."
                f"\n Original KeyError: {e}"
            )

    return datatype


def build_bitfield_structs(path: str, nid: NameIdDict) -> None:
    # TODO: Load bitfield structs from file, parse file, build Bitfield Structs and load into appropriate arg/channel
    # TODO: More advanced bitfield checks (than in `BitfieldStruct` ctor)
    # TODO: Raise flag if any bitfield structs are unmatched?
    # Check for spanning across storage-units
    # Check for incomplete storage unit padding
    # Check struct packing efficiency (incl. forgetting to put in padding
    # where it should have been added or using uint32 when you shouldn't have)
    # ... incl. sub-storage unit not properly filled.
    raise NotImplementedError()


def extract_enum(node: etree.Element) -> List[EnumItem]:
    """
    Extracts a list of all EnumItems from the given XML node.

    Node should contain an <enum> element as direct child (eg. <arg> or
    <channel>) and EnumItems should be <item> children of that <enum>.
    """
    # Extract and Build Enum if Present:
    enum: List[EnumItem] = []
    if len(node.xpath("enum")) > 1:
        raise StandardsFormattingException(
            node.base,
            f"At {node.sourceline}, <{node.tag}> node has multiple <enum> children "
            " but only one is allowed."
        )

    enum_items = node.xpath("enum/item")
    for i, item_src in enumerate(enum_items):
        # Get value:
        val = parse_int(item_src.attrib.get('value', i))
        # Build EnumItem:
        enum.append(EnumItem(
            name=standardize_constant_name(item_src.attrib['name']),
            value=val,
            comment=item_src.attrib.get('comment', "")
        ))

    return enum


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
        args.append(Argument(
            name=standardize_field_name(arg_src.attrib['name']),
            datatype=extract_type(arg_src, 'type'),
            enum=extract_enum(arg_src),
            metadata_json_str=GswMetadataTools.extract_from_xml(arg_src)
        ))

    return args


def extract_commands(node: etree.Element, module_name: str) -> NameIdDict[Command]:
    """
    Extract all commands from the given XML node as a NameIdDict of Commands.

    XML Node should contain a `<commands>` element containing <command> elements.
    Eg. a <component> element.
    """
    command_tree = node.xpath("//commands/command")

    commands: NameIdDict[Command] = NameIdDict()
    for command_src in command_tree:
        command = Command()

        # Grab Mnemonic:
        command.mnemonic = command_src.attrib['mnemonic']

        # Create UI Name:
        # Format: ModuleName_FPrimeCommandMnemonic
        formatted_module_name = standardize_name(module_name)
        formatted_mnemonic = standardize_name(command.mnemonic)
        command.name = f"{formatted_module_name}_{formatted_mnemonic}"

        # Grab ID ("opcode" in FPrime):
        command.ID = parse_int(command_src.attrib['opcode'])

        # Grab Metadata:
        command.metadata_json_str = GswMetadataTools.extract_from_xml(
            command_src
        )

        # Grab Args:
        command.args = extract_arguments(command_src)

        # Store Command:
        if command.ID in commands.ids:
            # If command with same ID is already stored, flag it.
            warnings.warn(
                f"On line {node.sourceline} in {node.base}, "
                f"DUPLICATE of existing command ID {command.ID} "
                f"with {'the same' if command == commands[command.ID] else 'DIFFERENT'} content "
                f"for NameIdDict index [{command.ID, command.name}]."
            )

        commands[command.ID, command.name] = command

    return commands


def extract_telemetry(node: etree.Element) -> NameIdDict[TelemetryChannel]:
    """
    Extract all telemetry channels from the given XML node as a NameIdDict of TelemetryChannels.

    XML Node should contain a `<telemetry>` element containing <channel> elements.
    Eg. a <component> element.
    """
    # Grab component telemetry channels:
    telem_tree = node.xpath("//telemetry/channel")

    channels: NameIdDict[TelemetryChannel] = NameIdDict()
    for channel_src in telem_tree:
        # Build Channel:
        channel = TelemetryChannel(
            name=standardize_name(channel_src.attrib['name']),
            ID=parse_int(channel_src.attrib['id']),
            datatype=extract_type(channel_src, 'data_type'),
            enum=extract_enum(channel_src),
            metadata_json_str=GswMetadataTools.extract_from_xml(channel_src)
        )

        # Store Channel:
        if (channel.ID, channel.name) in channels:
            # If channel with same index is already stored, flag it.
            warnings.warn(
                f"On line {node.sourceline} in {node.base}, "
                f"DUPLICATE of existing telemetry channel ID {channel.ID} "
                f"with {'the same' if channel == channels[channel.ID] else 'DIFFERENT'} content "
                f"for NameIdDict index [{channel.ID, channel.name}]."
            )

        channels[channel.ID, channel.name] = channel

    return channels


def extract_events(node: etree.Element) -> NameIdDict[Event]:
    """
    Extract all events from the given XML node as a NameIdDict of Events.

    XML Node should contain a `<events>` element containing <event> elements.
    Eg. a <component> element.
    """
    # Grab component events:
    event_tree = node.xpath("//events/event")

    events: NameIdDict[Event] = NameIdDict()
    for event_src in event_tree:
        event = Event(
            name=standardize_name(event_src.attrib['name']),
            ID=parse_int(event_src.attrib['id']),
            severity_str=event_src.attrib['severity'],
            format_string=event_src.attrib['format_string'],
            metadata_json_str=GswMetadataTools.extract_from_xml(event_src),
            args=extract_arguments(event_src)
        )

        # Store Event:
        if (event.ID, event.name) in events:
            # If event with same index is already stored, flag it.
            warnings.warn(
                f"On line {node.sourceline} in {node.base}, "
                f"DUPLICATE of existing event ID {event.ID} "
                f"with {'the same' if event == events[event.ID] else 'DIFFERENT'} content "
                f"for NameIdDict index [{event.ID, event.name}]."
            )

        events[event.ID, event.name] = event

    return events


def build_module(node: etree.Element, tree_topology: etree.ElementTree) -> Module:
    """
    Build `Module` from the given XML node which should be a <component> Element.

    Args:
        node (etree.Element): XML <component> Node representing the Module in FPrime.
        tree_topology (etree.ElementTree): ElementTree of the FPrime XML Topology.

    Raises:
        StandardsFormattingException: when `node` tag is not correct

    Returns:
        Module: Built Module
    """
    if node.tag != "component":
        raise StandardsFormattingException(
            node.base,
            (
                f"On line {node.sourceline}, `Module` should have a <component> "
                f"tag. Instead, it has a <{node.tag}> tag."
            )
        )

    name = standardize_name(node.attrib['name'])

    ####
    # Grab corresponding ID from topology tree:
    ####
    # Select all instances of the Module's <component>:
    try:
        instance_selector = f"/assembly/instance[@type='{name}']"
        instances = tree_topology.xpath(instance_selector)
    except IndexError:
        raise StandardsFormattingException(
            node.base,
            f"Failed to find any <instance>s in FPrime Topology for the module named {name}."
        )

    # Make sure there's only one instance of the Module's <component>:
    if len(instances) == 0:
        raise StandardsFormattingException(
            node.base,
            (
                f"No <instance> of <component> {name} found in FPrime "
                f"Topology for module defined at {node.sourceline}."
            )
        )
    elif len(instances) > 1:
        raise StandardsFormattingException(
            node.base,
            (
                f"Multiple <instance>s of <component> {name} found in FPrime "
                f"Topology for module defined at {node.sourceline}. "
                "Instances were found at lines "
                f"{[i.sourceline for i in instances]} in Topology file "
                f"{tree_topology.base}"
            )
        )

    ID = parse_int(instances[0].attrib['base_id'])

    ####
    # Extract Data:
    ###
    commands = extract_commands(node, name)
    telemetry = extract_telemetry(node)
    events = extract_events(node)

    if len(commands) > 0 or len(telemetry) > 0 or len(events) > 0:
        warnings.warn(
            f"Module {name} (ID = {ID}) has no commands, telemetry, or events."
        )

    return Module(
        name=name,
        ID=ID,
        commands=extract_commands(node, name),
        events=extract_events(node),
        telemetry=extract_telemetry(node)
    )


def import_all_fprime_xml(
    node: etree.Element,
    search_dir: str = _SEARCH_DIR,
    max_depth: int = 100
) -> etree.Element:
    """
    Recursively import all XML files listed in fprime-style <import_*> elements.

    # Basic Notes:
    - All file paths listed inside each <include_*> element are processed relative 
    to the given `search_dir`.

    - Each <import_*> element is replaced by the root node of the xml tree found 
    at the specified file location as well as a <comment> indicating what was 
    processed.

    - If the file imported includes more import statements, those will be imported 
    recursively up to `max_depth`.

    # Key Notes and Exceptions:
    - If the imported file does not end in `*.xml`, it will be ignored.
    - If the XML file is invalid XML, an `XMLSyntaxError` will be raised.

    - If the file contains <?xml ?> tags or any other <? ?> tags, they will be 
    ignored. The document will be processed with same standards as the base 
    tree (the XML file into which this file is being imported).

    - If the file imported does not contain a solitary root node (eg. 
    `<event /><event />` instead of `<events><event /><event /></events>`), an 
    `XMLSyntaxError` will be raised.
    """
    # Grab all elements starting with Fprime `import_*` identifier:
    imports = node.xpath('//*[starts-with(local-name(), "import_")]')

    # Check if there any imports to import:
    if len(imports) > 0 and max_depth > 0:
        # Import all sub-trees into the current tree (in-place):
        for im in imports:
            path = im.text  # path to imported file (w.r.t. project root)

            # Load the replacement (imported) element:
            _, ext = os.path.splitext(path)
            if ext.lower() == '.xml':
                with open(os.path.join(search_dir, path), 'r') as f:
                    text = f.read()
            # Remove all info tags from the file (eg.'<?xml ?>')
            # (since we should be reading XML with specs of base tree)
            text = re.sub(r'<\?.*\?>', '', text)
            # Load text as XML:
            imported_xml = etree.XML(path)
            # Extract root (on separate line so that `XMLSyntaxError`s can be pinpointed):
            imported_root = imported_xml.getroot()

            # Replace the import_* element:
            parent = im.getparent()
            parent.replace(im, imported_root)

            # Add a breadcrumb comment before the node in question:
            breadcrumb = etree.XML(
                f"""<comment> <![CDATA[[Next node imported via {im.tag} from: {path}]]]> </comment>"""
            )
            parent.insert(parent.index(imported_root), breadcrumb)

        # Recurse (in case imports imported more import elements):
        return import_all_fprime_xml(node, search_dir, max_depth=max_depth-1)

    return node


class DataStandards(object):
    """Loads and Stores the Standard Definitions for All Uplinked and Downlinked Data."""

    __slots__: List[str] = [
        # Modules, indexed by opcode (highest 1B)
        'modules'
    ]

    def __init__(self,  # DataStandards
                 modules: NameIdDict[Module]
                 ) -> None:
        self.modules = modules
        pass

    def __getitem__(self, key):
        # TODO: Get from any module (collection,indexer)
        # ... or just getCommand(), getTelem(), getEvent()
        # use collections.ChainMap?
        # ... shouldn't need to except for commands. Otherwise just index by module then ID.
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

    @ classmethod
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

    @ classmethod
    def build_standards(cls,
                        search_dir: str = _SEARCH_DIR,
                        cache_dir: str = _CACHE_DIR,
                        uri_topology: str = _URI_TOPOLOGY
                        ) -> DataStandards:
        """
        Create a new DataStandards instance by indexing the relevant standard definitions.

        Standard definition files are held in `search_dir` (should point to the
        FSW's `fprime/` directory).

        Once built, the finalized DataStandards instance is cached in `cache_dir` without overriding previous caches.
        """

        # Grab Topology File:
        tree_topology = etree.parse(os.path.join(search_dir, uri_topology))

        # Create empty modules container:
        modules: NameIdDict[Module] = NameIdDict()

        # Grab all XML files referenced in Topology:
        topology_imports = tree_topology.xpath(
            "/assembly/import_component_type/text()"
        )

        # TODO: Add way of parsing Watchdog Module (since it's NOT an FPrime component)
        # ... make it a local FPrime XML? ... it is an FPrime component
        # ... and Heartbeat Telemetry (maybe just hardcode heartbeat since it's so small...)

        #! TODO: Clean up and finish this section then unit test module building

        # Build a `Module` from imported XML component file:
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

            # Grab all <component>s:
            found_components = tree.xpath('//component')

            # Make sure exactly one component was found (one component per definition file):
            if len(found_components) == 0:
                raise StandardsFormattingException(
                    uri,
                    f"No <component> found for module at {tree.sourceline}."
                )
            elif len(found_components) > 1:
                raise StandardsFormattingException(
                    uri,
                    f"Somehow found multiple components ({found_components}) for module at {tree.sourceline}."
                )

            module = build_module(found_components[0], tree_topology)

        # Build and Return the DataStandards Object:
        return cls(modules)
