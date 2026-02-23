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
@last-updated: 11/12/2022
"""

# Activate postponed annotations (for using classes as return type in their own methods)
from __future__ import annotations

import typing
from typing import List, Optional, Dict, Any, TypeVar, Generic, Union, Tuple, Mapping, MutableMapping, Iterator, cast
from collections import ChainMap
import warnings

import os.path
from lxml import etree  # type: ignore # doesn't have type hints
import re
import jsonpickle  # type: ignore # doesn't have type hints
import ulid

from termcolor import cprint

from .logs import logger
from .exceptions import StandardsFormattingException

from IrisBackendv3.utils import NameIdDict
from IrisBackendv3.utils.basic import name_split_and_format, name_split_and_format_by_term

from .module import Module, BitfieldStruct, EnumItem, Argument, Command, TelemetryChannel, Event
from .fsw_data_type import FswDataType
from . import gsw_metadata_tools as GswMetadataTools

# Settings:
# Default directory to search for files in:
_SEARCH_DIR: str = '../FlightSoftware/fprime'
# Default directory to store cached files in:
_CACHE_DIR: str = './out/DataStandardsCache'
# Default filename base for stored cached files:
_FILENAME_BASE_DEFAULT: str = 'IBv3_datastandards'
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
    pieces = name_split_and_format_by_term(original_name)
    # Build the name with no joiner between pieces.
    # *but* Make sure to preserve boundaries between adjacent digits.
    # Since digits can't be capitalized and thus the boundaries will be erased.
    name = ""
    for i in range(len(pieces)-1):
        first, *rest = list(pieces[i])
        name += first.upper() + "".join(rest).lower()
        if pieces[i][-1].isdigit() and pieces[i+1][0].isdigit():
            # If this piece ends w/a digit and next one starts with one,
            # add a boundary to preserve the boundary that was once there (so it
            # doesn't just look like one big number):
            name += '.'

    first, *rest = list(pieces[-1])
    name += first.upper() + "".join(rest).lower()

    return name


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
        type_name: str = node.attrib[attr_name]
    except KeyError as e:
        raise StandardsFormattingException(
            node.base,
            f"[KeyError]: At sourceline {node.sourceline}, given attribute name "
            f"'{attr_name}' cannot be found."
            f"\n Original KeyError: {e}"
        )

    type_size: int
    if type_name.lower() == 'string':
        try:
            # Extract the string size:
            type_size = parse_int(node.attrib['size'])
            # Craft the appropriate fixed-size type name used in FswDataType:
            # NOTE: This is just the max. Actual encoded/decoded size is
            # variable and will be determined based on the data):
            type_name = 'STRING' + str(type_size)
        except KeyError as e:
            raise StandardsFormattingException(
                node.base,
                f"[KeyError]: At sourceline {node.sourceline}, given attribute name "
                f"'{attr_name}' has value {type_name}. As such, it needs a `size` "
                f"attribute to work as a fixed-size type but a `size` attribute "
                f"cannot be found."
                f"\n Original KeyError: {e}"
            )

    if 'IrisCmdByteStringArg'.lower() in type_name.lower():
        try:
            # Grab the maximum size.
            # NOTE: This must always be < `FW_COM_BUFFER_MAX_SIZE` in
            # `fprime/Fw/Cfg/Config.hpp`).
            type_size = parse_int(node.attrib['size'])
            # Craft the appropriate fixed-size type name used in FswDataType:
            # NOTE: This is just the max. Actual encoded/decoded size is
            # variable and will be determined based on the data):
            type_name = 'IRISBYTESTRING' + str(type_size)
        except KeyError as e:
            raise StandardsFormattingException(
                node.base,
                f"[KeyError]: At sourceline {node.sourceline}, given attribute name "
                f"'{attr_name}' has value {type_name}. As such, it needs a `size` "
                f"attribute to work as a fixed-size type but a `size` attribute "
                f"cannot be found."
                f"\n Original KeyError: {e}"
            )

    try:
        datatype = FswDataType.get(type_name)
    except KeyError:
        # Note: both unknown enum name and unknown attribute -> KeyError
        logger.error(
            f"[FSWTypeError] in {node.base} at sourceline {node.sourceline}: "
            f"given type in attribute '{attr_name}' with value "
            f"`{type_name}` is not a known or supported "
            f"`FSWDataType`. So, this field is being given type `INVALID`."
        )
        return FswDataType.INVALID

    return datatype


def build_bitfield_structs(path: str, nid: NameIdDict) -> None:
    #! TODO: Load bitfield structs from file, parse file, build Bitfield Structs and load into appropriate arg/channel
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

        # Make sure command ID has appropriate formatting:
        # (1B or less):
        if (command.ID >> 8) != 0:
            raise StandardsFormattingException(
                node.base,
                (
                    f"Command {command.name} defined at {command_src.sourceline} "
                    f"in {command_src.base} has an invalid ID."
                    f"\nFound ID {command.ID} = {hex(command.ID)}."
                    f"\nCommand IDs should have the format 0xXX, at most 1B. "
                    f"This can be expressed in any radix but the binary must "
                    f"conform to this spec regardless."
                )
            )

        # Grab Metadata:
        command.metadata_json_str = GswMetadataTools.extract_from_xml(
            command_src
        )

        # Grab Args:
        command.args = extract_arguments(command_src)

        # Store Command:
        if command.ID in commands.ids:
            # If command with same ID is already stored, flag it.
            logger.warning(
                f"On line {node.sourceline} in {node.base}, "
                f"DUPLICATE of existing command ID {command.ID} "
                f"with {'the same' if command == commands[command.ID] else 'DIFFERENT'} content "
                f"for NameIdDict index [{command.ID, command.name}]."
            )

        # If command contains an argument with an invalid type, toss it:
        if FswDataType.INVALID in [a.datatype for a in command.args]:
            logger.error(
                f"Command {command} is being ignored since it contains an "
                f"argument with an `INVALID` `FswDataType`."
            )
        else:
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

        # Make sure channel ID has appropriate formatting:
        # (1B or less):
        if (channel.ID >> 8) != 0:
            raise StandardsFormattingException(
                node.base,
                (
                    f"Telemetry Channel {channel} defined at "
                    f"{channel_src.sourceline} in {channel_src.base} has an "
                    f"invalid ID."
                    f"\nFound ID {channel.ID} = {hex(channel.ID)}."
                    f"\nChannel IDs should have the format 0xXX, at most 1B. "
                    f"This can be expressed in any radix but the binary must "
                    f"conform to this spec regardless."
                )
            )

        # If channel has invalid type, we won't be able to read it:
        if channel.datatype == FswDataType.INVALID:
            raise StandardsFormattingException(
                channel_src.base,
                f"Telemetry channel {channel} has `FswDataType.INVALID` type. "
                f"As a result, this telemetry channel will be unintelligible. "
                f"Please fix the type."
            )

        # Store Channel:
        if (channel.ID, channel.name) in channels:
            # If channel with same index is already stored, flag it.
            logger.warning(
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

        # Make sure event ID has appropriate formatting:
        # (1B or less):
        if (event.ID >> 8) != 0:
            raise StandardsFormattingException(
                node.base,
                (
                    f"Event {event} defined at "
                    f"{event_src.sourceline} in {event_src.base} has an "
                    f"invalid ID."
                    f"\nFound ID {event.ID} = {hex(event.ID)}."
                    f"\nEvent IDs should have the format 0xXX, at most 1B. "
                    f"This can be expressed in any radix but the binary must "
                    f"conform to this spec regardless."
                )
            )

        # If event has an arg with invalid type, we won't be able to read it:
        if FswDataType.INVALID in [a.datatype for a in event.args]:
            raise StandardsFormattingException(
                event_src.base,
                f"Event {event} has an argument with `FswDataType.INVALID` type. "
                f"As a result, this event will be unintelligible. "
                f"Please fix the type."
            )

        # Store Event:
        if (event.ID, event.name) in events:
            # If event with same index is already stored, flag it.
            logger.warning(
                f"On line {node.sourceline} in {node.base}, "
                f"DUPLICATE of existing event ID {event.ID} "
                f"with {'the same' if event == events[event.ID] else 'DIFFERENT'} content "
                f"for NameIdDict index [{event.ID, event.name}]."
            )

        events[event.ID, event.name] = event

    return events


def build_modules_from_component(node: etree.Element, tree_topology: etree.ElementTree) -> List[Module]:
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

    src_name = node.attrib['name']
    name = standardize_name(src_name)

    ####
    # Grab corresponding ID from topology tree:
    ####
    # Select all instances of the Module's <component>:
    try:
        instance_selector = f"/assembly/instance[@type='{src_name}']"
        instances = tree_topology.xpath(instance_selector)
    except IndexError:
        raise StandardsFormattingException(
            node.base,
            f"Failed to find any <instance>s in FPrime Topology for the module named {src_name}."
        )

    # Make sure there's only one instance of the Module's <component>:
    if len(instances) == 0:
        raise StandardsFormattingException(
            node.base,
            (
                f"No <instance> of <component> {src_name} found in FPrime "
                f"Topology for module defined at {node.sourceline}."
            )
        )
    elif len(instances) > 1:
        num_instances: int = len(instances)
        if len({i.attrib['name'] for i in instances}) == num_instances \
                and len({parse_int(i.attrib['base_id']) for i in instances}) == num_instances:

            # If they all have unique IDs and unique names, then make new names
            # for each instance.
            logger.notice(  # type: ignore # mypy complains that `logger` doesn't have a member `notice` but it does
                f"Multiple <instance>s of <component> {src_name} found in FPrime "
                f"Topology for module defined at {node.sourceline}. "
                f"Since all instance names and base_ids are unique, new "
                f"standards objects will be created for each instance. "
                f"Instances were found at lines "
                f"{[i.sourceline for i in instances]} in Topology file "
                f"{tree_topology.getroot().base}"
            )
            IDs = [parse_int(i.attrib['base_id']) for i in instances]
            instance_names = [
                name + "-" + standardize_name(i.attrib['name']) for i in instances
            ]

        else:
            # If not, this is a mistake and should be flagged
            raise StandardsFormattingException(
                node.base,
                (
                    f"Multiple <instance>s of <component> {src_name} with non-unique "
                    f"names and IDs were found in FPrime Topology for module "
                    f"defined at {node.sourceline}. Instances were found at lines "
                    f"{[i.sourceline for i in instances]} in Topology file "
                    f"{tree_topology.getroot().base}"
                )
            )
    else:
        IDs = [parse_int(instances[0].attrib['base_id'])]
        instance_names = [name]

    ####
    # Extract Data per Module:
    ###
    modules: List[Module] = []
    for instance_name, ID in zip(instance_names, IDs):
        # Note: Commands depend on module name:
        commands = extract_commands(node, instance_name)
        # Note: Even though Telemetry and Events can be extracted independently
        # from the module name, redoing it on each iteration ensure unique
        # objects for each module:
        telemetry = extract_telemetry(node)
        events = extract_events(node)

        if len(commands) == 0 and len(telemetry) == 0 and len(events) == 0:
            logger.info(
                f"Module {src_name} (ID = {ID}) has no commands, telemetry, or events."
            )

        # Make sure module ID has appropriate formatting:
        # 1B elevated by 1B 0xXX00:
        if (ID & 0x00FF) != 0x0000:
            raise StandardsFormattingException(
                node.base,
                (
                    f"Module {instance_name} from <component> {src_name} "
                    f"defined at {node.sourceline} in Topology file "
                    f"{tree_topology.getroot().base} has an invalid ID ."
                    f"\nFound ID {ID} = {hex(ID)}."
                    f"\nModule IDs should have the format 0xXX00, just 1B followed "
                    f"by 0x00. This can be expressed in any radix but the binary must "
                    f"conform to this spec regardless."
                )
            )

        modules.append(Module(
            name=instance_name,
            ID=ID,
            commands=commands,
            events=events,
            telemetry=telemetry
        ))

    return modules


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
            imported_xml = etree.XML(text)

            # Replace the import_* element:
            parent = im.getparent()
            parent.replace(im, imported_xml)

            # Add a breadcrumb comment before the node in question:
            breadcrumb = etree.XML(
                f"""<comment> <![CDATA[[Next node imported via {im.tag} from: {path}]]]> </comment>"""
            )
            parent.insert(parent.index(imported_xml), breadcrumb)

        # Recurse (in case imports imported more import elements):
        return import_all_fprime_xml(node, search_dir, max_depth=max_depth-1)

    return node


class DataStandards(object):
    """Loads and Stores the Standard Definitions for All Uplinked and Downlinked Data."""

    EMPTY: Union[DataStandards, None] = None

    __slots__: List[str] = [
        # Modules, indexed by component ID (highest 1B of Iris Common Packet (ICP) opcode)
        '_modules',
        # (updatable) ChainMap of All Commands by ICP opcode (module ID | command ID):
        'commands_chainmap_opcode',
        # (updatable) ChainMap of All Commands by Command Name:
        'commands_chainmap_name'
    ]

    _modules: NameIdDict[Module]

    # Chain Maps for globally looking up commands by name.
    # Maps command opcodes or full_names to a tuple containing the ID of the
    # command's module and the command itself.
    commands_chainmap_opcode: typing.ChainMap[int, Tuple[int, Command]]
    commands_chainmap_name: typing.ChainMap[str, Tuple[int, Command]]

    def __init__(self,  # DataStandards
                 modules: NameIdDict[Module]
                 ) -> None:
        self.commands_chainmap_opcode = ChainMap()
        self.commands_chainmap_name = ChainMap()
        self.modules = modules

    def rebuild_command_chainmap(self) -> None:
        """Rebuilds the ChainMap used for command lookup."""
        # !TODO: Unit test ChainMap updates and linking.
        self.commands_chainmap_opcode = ChainMap()
        self.commands_chainmap_name = ChainMap()
        # NB: ChainMap will update if module's contents update but not if it's replaced entirely
        for m in self._modules.vals:
            cs = m.commands.collect()
            # Dict of opcode -> Command:
            opcode_dict = dict(
                ((m.ID | id), (m.ID, cmd))
                for ((names, id), cmd) in cs
            )
            # List of (names, Command):
            names_list = [(names, (m.ID, cmd)) for ((names, id), cmd) in cs]
            # Flatten list of names into dict where each name has its own mapping to a command:
            names_dict = dict()
            for names, data in names_list:
                for name in names:
                    names_dict[name] = data
            # Update ChainMaps with links to this Module:
            self.commands_chainmap_opcode = self.commands_chainmap_opcode.new_child(
                opcode_dict
            )
            self.commands_chainmap_name = self.commands_chainmap_name.new_child(
                names_dict
            )

    def add_new_modules(self, modules: List[Module]) -> None:
        """Adds new modules to the datastandards.
        NOTE: Any modules whose names or IDs are already present aren't added.
        """
        # Add modules:
        for module in modules:
            if module.ID in self._modules:
                logger.warning(
                    f"Unable to add module {module} since its ID ({module.ID})"
                    f"={hex(module.ID)}) is already in the DataStandard."
                )
            elif module.name in self._modules:
                logger.warning(
                    f"Unable to add module {module} since its name "
                    f"({module.name}) is already in the DataStandard."
                )
            else:
                self._modules[module.ID, module.name] = module

        # Rebuild the chain map in case any new commands were added:
        self.rebuild_command_chainmap()

    @property
    def modules(self) -> NameIdDict[Module]:
        return self._modules

    @modules.setter
    def modules(self, new_val) -> None:
        # Update Value:
        self._modules = new_val
        self.rebuild_command_chainmap()

    def global_command_lookup(self, key: Union[str, int, NameIdDict.KeyTuple]) -> Tuple[Module, Command]:
        """
        Finds a command from any module which has the given name string `key`
        (standardized and includes the module name), id `key` as a full opcode, 
        or KeyTuple `key` which includes the name alongside its corresponding module ID.

        Returns a tuple of the command's parent module and the command itself.
        """
        # TODO: Unit test ChainMap lookup and linking.
        if isinstance(key, str):
            mID, command = self.commands_chainmap_name[key]
            return self.modules[mID], command
        if isinstance(key, int):
            mID, command = self.commands_chainmap_opcode[key]
            return self.modules[mID], command
        if NameIdDict.is_valid_key_tuple(key):
            key = cast(NameIdDict.KeyTuple, key)
            mID, command = self.commands_chainmap_opcode[key[0]]
            return self.modules[mID], command
        else:
            raise TypeError(
                "Argument to `global_command_lookup` of `DataStandards` must be "
                "a string name (not FPrime mnemonic), or an int Iris Common "
                "Packet (ICP) opcode (not FPrime opcode), or a Tuple[int,str] "
                "of the two."
            )

    def __eq__(self, other) -> bool:
        if not isinstance(other, DataStandards):
            return False

        return self.modules == other.modules

    def overview(self) -> str:
        """Returns a string which provides an overview of the standards."""
        overview = "["
        for m in self.modules.vals:
            overview += f'\n\t{m}'
            overview += '\n\t\t Commands:'
            for i, c in enumerate(m.commands.vals):
                overview += f'\n\t\t\t{i}.\t{c}'
            overview += '\n\t\t Telemetry:'
            for i, t in enumerate(m.telemetry.vals):
                overview += f'\n\t\t\t{i}.\t{t}'
            overview += '\n\t\t Events:'
            for i, e in enumerate(m.events.vals):
                overview += f'\n\t\t\t{i}.\t{e}'
        overview += '\n]'

        return overview

    def print_overview(self) -> None:
        """Prints a colored overview of the standards."""
        def module(x):
            return cprint(f"\n\t{x}", 'magenta', 'on_grey', attrs=['bold'])

        def header(x): return cprint(f"\n\t\t{x}", 'grey', 'on_white')
        def command(i, x): return cprint(f"\n\t\t\t{i}.\t{x}", 'green')
        def telemetry(i, x): return cprint(f"\n\t\t\t{i}.\t{x}", 'red')
        def event(i, x): return cprint(f"\n\t\t\t{i}.\t{x}", 'blue')

        def p_arg(a): return cprint(
            f"\n\t\t\t\t\tAvailable values for `{a.name}`:", 'cyan'
        )

        def p_enum(x): return cprint(
            f"\n\t\t\t\t\t\t'{x.name}' or {x.value} or {hex(x.value)}", 'magenta'
        )

        print("Data Standards Overview: [")
        for m in self.modules.vals:
            module(m)
            header('Commands:')
            for i, c in enumerate(m.commands.vals):
                command(i, c)
                for arg in c.args:
                    if len(arg.enum) > 0:
                        p_arg(arg)
                        for e in arg.enum:
                            p_enum(e)
            header('Telemetry:')
            for i, t in enumerate(m.telemetry.vals):
                telemetry(i, t)
                if t.is_enum:
                    for e in t.enum:
                        p_enum(e)
            header('Events:')
            for i, ev in enumerate(m.events.vals):
                event(i, ev)
                for arg in ev.args:
                    if arg.is_enum:
                        p_arg(arg)
                        for e in arg.enum:
                            p_enum(e)
        print('\n]')

    def __str__(self) -> str:
        n_cmd = sum(len(m.commands) for m in self.modules.vals)
        n_telem = sum(len(m.telemetry) for m in self.modules.vals)
        n_event = sum(len(m.events) for m in self.modules.vals)
        return (
            f"{len(self.modules)} Modules with {n_cmd} Commands, {n_telem} Channels, and {n_event} Events"
        )

    def cache(self,
              cache_dir: str = _CACHE_DIR,
              filename_base: str = _FILENAME_BASE_DEFAULT,
              ext: str = "dsc",
              indent: int = 0,
              include_ulid_suffix: bool = True
              ) -> str:
        """
        Cache this DataStandards instance in a unique file in `cache_dir`.

        Returns the unique filename.
        """
        # Make sure directory exists:
        if not os.path.exists(cache_dir):
            os.makedirs(cache_dir)
        # Encode data:
        json = jsonpickle.encode(self.modules, keys=True, indent=indent)
        # Save:
        if include_ulid_suffix:
            filename = f'{filename_base}_{ulid.new()}.{ext}'
        else:
            filename = f'{filename_base}.{ext}'
        with open(os.path.join(cache_dir, filename), 'w', encoding='utf-8') as file:
            print(json, file=file)

        return filename

    @classmethod
    def load_cache(cls,
                   cache_dir: str = _CACHE_DIR,
                   filename_base: str = _FILENAME_BASE_DEFAULT,
                   ext: str = "dsc",
                   cache_filename: Optional[str] = None,
                   ) -> DataStandards:
        """
        Create a new DataStandards instance by loading a cached file from `cache_dir`.

        The loaded file will be the latest cache in `cache_dir` unless the
        name of specific cache file is supplied with `cache_filename`.

        Any file in the given `cache_dir` whose name starts with the given
        `filename_base` and has the given `ext` will considered a candidate
        cache file. The candidate cache file with the most recent creation date
        will be selected.

        To abuse this slighly and load a cache based on an absolute path,
        supply `cache_dir=''` and `cache_filename='/abolute/path/to_cache.dsc'`
        """
        # Find the latest file if a specific file isn't requested:
        if cache_filename is None:
            # Grab all files in dir with extension:
            files = os.listdir(cache_dir)
            files = [f for f in files if f.endswith('.'+ext)]
            if len(files) == 0:
                raise FileNotFoundError(
                    f"No DataStandards files with given extension {ext} found "
                    f"in given directory {cache_dir}."
                )
            # Filter for those with desired prefix:
            files = [f for f in files if f.startswith(filename_base)]
            if len(files) == 0:
                raise FileNotFoundError(
                    f"No DataStandards files with given extension {ext} which "
                    f"start with the given `filename_base` '{filename_base}' found "
                    f"in given directory {cache_dir}."
                )

            # Grab file with the latest ulid:
            latest_file: Tuple[str, ulid.ULID]  # name, ulid
            for i, f in enumerate(files):
                # Extract the ulid:
                ulid_str = f.replace('.'+ext, '').split('_')[-1]
                try:
                    u = ulid.from_str(ulid_str)
                except ValueError as e:
                    raise ValueError(
                        f"Unable to rebuild the ULID for DataStandards cache "
                        f"file '{f}'. Extracted ULID string was '{ulid_str}' "
                        f"but appears to be an invalid ulid. Is this really a "
                        f"DS cache file? Is it from an old version? Has the file "
                        f"been renamed?"
                        f"\n Original ValueError from `ulid-py`: {e}"
                    )

                # Store this as the latest file if it's the first file or its
                # ulid is chronologically more recent that the previous `latest_file`:
                if i == 0:
                    latest_file = f, u
                elif u > latest_file[1]:
                    latest_file = f, u

            cache_filename = latest_file[0]

        # Open the file:
        with open(os.path.join(cache_dir, cache_filename), 'r', encoding='utf-8') as file:
            data = file.read()

        # Decode the file:
        modules = jsonpickle.decode(data)

        # Build the DataStandards Object:
        DS = cls(modules)

        logger.info(
            f"Successfully Loaded {DS} from {os.path.join(cache_dir, cache_filename)} ."
        )

        return cls(modules)

    @classmethod
    def build_standards(cls,
                        search_dir: str = _SEARCH_DIR,
                        uri_topology: str = _URI_TOPOLOGY
                        ) -> DataStandards:
        """
        Create a new DataStandards instance by indexing the relevant standard definitions.

        Standard definition files are held in `search_dir` (should point to the
        FSW's `fprime /` directory).

        Once built, the finalized DataStandards instance is cached in `cache_dir` without overriding previous caches.
        """

        # Grab Topology File:
        tree_topology = etree.parse(os.path.join(search_dir, uri_topology))

        # Grab all XML files referenced in Topology:
        topology_imports = tree_topology.xpath(
            "/assembly/import_component_type/text()"
        )

        # Create empty modules container:
        modules: NameIdDict[Module] = NameIdDict()

        # TODO: Add way of parsing Watchdog Module (since it's NOT an FPrime component)
        # ... make it a local FPrime XML? ... it is an FPrime component
        # ... not necessary? (Watchdog Component in FPrime should mirror commands?...)
        # ... and Heartbeat Telemetry (maybe just hardcode heartbeat since it's so small...)

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

            expanded_tree = import_all_fprime_xml(
                node=tree,
                search_dir=search_dir
            )

            # Grab all <component>s:
            found_components = expanded_tree.xpath('//component')

            # Make sure exactly one component was found (one component per definition file):
            if len(found_components) == 0:
                raise StandardsFormattingException(
                    uri,
                    f"No <component> found for module at {expanded_tree.sourceline}."
                )
            elif len(found_components) > 1:
                raise StandardsFormattingException(
                    uri,
                    f"Somehow found multiple components ({found_components}) for module at {expanded_tree.sourceline}."
                )

            try:
                component_modules = build_modules_from_component(
                    found_components[0],
                    tree_topology
                )
                for module in component_modules:
                    modules[module.ID, module.name] = module
                    logger.verbose(  # type: ignore # mypy doesn't recognize the `verboselogs` levels
                        f"Successfully Built Module #{len(modules)-1}: {module}"
                    )
            except StandardsFormattingException as e:
                logger.error(
                    f"Standards Formatting Exception while compiling component "
                    f"at '{found_components[0].base}'."
                    f"\n This module is being skipped and will not be in GSW."
                    f"\n Original `StandardsFormattingException`: {e}"
                )

        # Build the DataStandards Object:
        DS = cls(modules)

        logger.info(
            f"Successfully Built {DS} from {os.path.join(search_dir, uri_topology)} ."
        )

        return DS


# Empty DataStandards Object used as a Placeholder:
EMPTY_DATASTANDARD = DataStandards(NameIdDict[Module]())
DataStandards.EMPTY = EMPTY_DATASTANDARD
