Module IrisBackendv3.data_standards.data_standards
==================================================
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

Functions
---------

    
`build_bitfield_structs(path: str, nid: NameIdDict) ‑> None`
:   

    
`build_modules_from_component(node: etree.Element, tree_topology: etree.ElementTree) ‑> List[IrisBackendv3.data_standards.module.Module]`
:   Build `Module` from the given XML node which should be a <component> Element.
    
    Args:
        node (etree.Element): XML <component> Node representing the Module in FPrime.
        tree_topology (etree.ElementTree): ElementTree of the FPrime XML Topology.
    
    Raises:
        StandardsFormattingException: when `node` tag is not correct
    
    Returns:
        Module: Built Module

    
`extract_arguments(node: etree.Element) ‑> List[IrisBackendv3.data_standards.module.Argument]`
:   Extract all arguments from the given XML node as a List of Command Arguments.
    
    XML Node should be a `<command>` or `<event>` containing an <args> element
    containing <arg> elements.

    
`extract_commands(node: etree.Element, module_name: str) ‑> IrisBackendv3.utils.nameiddict.NameIdDict[IrisBackendv3.data_standards.module.Command]`
:   Extract all commands from the given XML node as a NameIdDict of Commands.
    
    XML Node should contain a `<commands>` element containing <command> elements.
    Eg. a <component> element.

    
`extract_enum(node: etree.Element) ‑> List[IrisBackendv3.data_standards.module.EnumItem]`
:   Extracts a list of all EnumItems from the given XML node.
    
    Node should contain an <enum> element as direct child (eg. <arg> or
    <channel>) and EnumItems should be <item> children of that <enum>.

    
`extract_events(node: etree.Element) ‑> IrisBackendv3.utils.nameiddict.NameIdDict[IrisBackendv3.data_standards.module.Event]`
:   Extract all events from the given XML node as a NameIdDict of Events.
    
    XML Node should contain a `<events>` element containing <event> elements.
    Eg. a <component> element.

    
`extract_telemetry(node: etree.Element) ‑> IrisBackendv3.utils.nameiddict.NameIdDict[IrisBackendv3.data_standards.module.TelemetryChannel]`
:   Extract all telemetry channels from the given XML node as a NameIdDict of TelemetryChannels.
    
    XML Node should contain a `<telemetry>` element containing <channel> elements.
    Eg. a <component> element.

    
`extract_type(node: etree.Element, attr_name: str) ‑> IrisBackendv3.data_standards.fsw_data_type.FswDataType`
:   Extracts the FSWDataType from the attribute `attr_name` inside the XML node `node`.
    
    Throws relevant error if attribute not found or invalid.

    
`import_all_fprime_xml(node: etree.Element, search_dir: str = '../FlightSoftware/fprime', max_depth: int = 100) ‑> <cyfunction Element at 0x11083a260>`
:   Recursively import all XML files listed in fprime-style <import_*> elements.
    
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

    
`parse_int(number: Union[int, str]) ‑> int`
:   Parses the string representation of an int from any base (likely either 10 or 16).
    
    Written with a wrapper in case extra-parsing conditions or warnings want to
    be supported later.
    
    This works so long as it's given appropriate base markers (eg. "0b", "0o",
    "0x"). Supports taking integer arguments as well in case the field was
    pre-parsed earlier in a pipeline.

    
`standardize_constant_name(original_name: str) ‑> str`
:   Standardize the capitalization and spacing of the given name string of any
    constant-like name(eg. Enums).

    
`standardize_field_name(original_name: str) ‑> str`
:   Standardize the capitalization and spacing of the given name string of any
    low-level field(Argument, Bitfield) of a highlevel struct-like container
    (Module, Command, Event, TelemetryChannel).

    
`standardize_name(original_name: str) ‑> str`
:   Standardize the capitalization and spacing of the given name string.
    
    Used for all names except for a few special cases(fields and
    constants/enums), one example being all highlevel struct-like containers
    (Module, Command, Event, TelemetryChannel).

Classes
-------

`DataStandards(modules: NameIdDict[Module])`
:   Loads and Stores the Standard Definitions for All Uplinked and Downlinked Data.

    ### Class variables

    `EMPTY: Optional[IrisBackendv3.data_standards.data_standards.DataStandards]`
    :

    ### Static methods

    `build_standards(search_dir: str = '../FlightSoftware/fprime', uri_topology: str = './CubeRover/Top/CubeRoverTopologyAppAi.xml') ‑> IrisBackendv3.data_standards.data_standards.DataStandards`
    :   Create a new DataStandards instance by indexing the relevant standard definitions.
        
        Standard definition files are held in `search_dir` (should point to the
        FSW's `fprime /` directory).
        
        Once built, the finalized DataStandards instance is cached in `cache_dir` without overriding previous caches.

    `load_cache(cache_dir: str = './out/DataStandardsCache', filename_base: str = 'IBv3_datastandards', ext: str = 'dsc', cache_filename: Optional[str] = None) ‑> IrisBackendv3.data_standards.data_standards.DataStandards`
    :   Create a new DataStandards instance by loading a cached file from `cache_dir`.
        
        The loaded file will be the latest cache in `cache_dir` unless the
        name of specific cache file is supplied with `cache_filename`.
        
        Any file in the given `cache_dir` whose name starts with the given
        `filename_base` and has the given `ext` will considered a candidate
        cache file. The candidate cache file with the most recent creation date
        will be selected.
        
        To abuse this slighly and load a cache based on an absolute path,
        supply `cache_dir=''` and `cache_filename='/abolute/path/to_cache.dsc'`

    ### Instance variables

    `commands_chainmap_name: ChainMap[str, Tuple[int, IrisBackendv3.data_standards.module.Command]]`
    :   Return an attribute of instance, which is of type owner.

    `commands_chainmap_opcode: ChainMap[int, Tuple[int, IrisBackendv3.data_standards.module.Command]]`
    :   Return an attribute of instance, which is of type owner.

    `modules: IrisBackendv3.utils.nameiddict.NameIdDict[IrisBackendv3.data_standards.module.Module]`
    :

    ### Methods

    `add_new_modules(self, modules: List[Module]) ‑> None`
    :   Adds new modules to the datastandards.
        NOTE: Any modules whose names or IDs are already present aren't added.

    `cache(self, cache_dir: str = './out/DataStandardsCache', filename_base: str = 'IBv3_datastandards', ext: str = 'dsc', indent: int = 0, include_ulid_suffix: bool = True) ‑> str`
    :   Cache this DataStandards instance in a unique file in `cache_dir`.
        
        Returns the unique filename.

    `global_command_lookup(self, key: Union[str, int, NameIdDict.KeyTuple]) ‑> Tuple[IrisBackendv3.data_standards.module.Module, IrisBackendv3.data_standards.module.Command]`
    :   Finds a command from any module which has the given name string `key`
        (standardized and includes the module name), id `key` as a full opcode, 
        or KeyTuple `key` which includes the name alongside its corresponding module ID.
        
        Returns a tuple of the command's parent module and the command itself.

    `overview(self) ‑> str`
    :   Returns a string which provides an overview of the standards.

    `print_overview(self) ‑> None`
    :   Prints a colored overview of the standards.

    `rebuild_command_chainmap(self) ‑> None`
    :   Rebuilds the ChainMap used for command lookup.