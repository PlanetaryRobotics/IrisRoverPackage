Module IrisBackendv3.data_standards.module
==========================================
Containers for Flight Software Modules/Components and their Relevant Fields
(Commands, Telemetry, Events), alongside relevant ground-software metadata
(where applicable).

@author: Connor W. Colombo (CMU)
@last-updated: 09/19/2022

Classes
-------

`Argument(name: str, datatype: FswDataType, enum: List[EnumItem] = None, bitfields: BitfieldStruct = None, metadata_json_str: str = '')`
:   Container for FPrime (Command or Event) Argument and associated Ground Software Metadata

    ### Ancestors (in MRO)

    * IrisBackendv3.data_standards.module.DataUnit
    * IrisBackendv3.data_standards.module.GswMetadataContainer

    ### Instance variables

    `name: str`
    :   Return an attribute of instance, which is of type owner.

`BitfieldStruct(fields: OrderedDict[str, int], total_padding: int)`
:   

    ### Class variables

    `STORAGE_UNIT_SIZE`
    :

    ### Instance variables

    `fields: collections.OrderedDict[str, int]`
    :

    `names: List[str]`
    :   Names of all the fields.

    `total_bits: int`
    :   Total number of bits in this struct.

    `total_padding: int`
    :

    ### Methods

    `pack(self, data: OrderedDict[str, int]) ‑> bytes`
    :   Packs the data given in the `data` dictionary according to the fields 
        defined in this bitfield struct.

    `unpack(self, data: bytes) ‑> collections.OrderedDict[str, int]`
    :   Unpacks the data given in the `data` bytes according to the fields 
        defined in this bitfield struct.

`Command(name: str = '', mnemonic: str = '', ID: int = -1, metadata_json_str: str = '', args: List[Argument] = None)`
:   Container for Data Standards for an FPrime Command and associated Ground Software Metadata.

    ### Ancestors (in MRO)

    * IrisBackendv3.data_standards.module.GswMetadataContainer

    ### Instance variables

    `ID`
    :   Return an attribute of instance, which is of type owner.

    `args`
    :   Return an attribute of instance, which is of type owner.

    `mnemonic`
    :   Return an attribute of instance, which is of type owner.

    `name`
    :   Return an attribute of instance, which is of type owner.

`DataUnit(datatype: FswDataType, enum: List[EnumItem] = None, bitfields: BitfieldStruct = None, metadata_json_str: str = '')`
:   Base class for any data container which carries a single unit of data.
    
    That is, containers that have a single `datatype`, namely `Argument`s and 
    `TelemetryChannel`s.
    
    Implicitly, all `DataUnit`s are also `GswMetadataContainer`s because every 
    piece of data being sent to/from Ground will likely be displayed or written 
    by Ground and thus should have the option of having associated GSW metadata 
    to assist the human operator interpreting/writing the data.
    
    Supports either representing the datatype as an enum value, or dissolving 
    the datatype into a `BitfieldStruct`.

    ### Ancestors (in MRO)

    * IrisBackendv3.data_standards.module.GswMetadataContainer

    ### Descendants

    * IrisBackendv3.data_standards.module.Argument
    * IrisBackendv3.data_standards.module.TelemetryChannel

    ### Instance variables

    `bitfields: Optional[IrisBackendv3.data_standards.module.BitfieldStruct]`
    :

    `datatype: IrisBackendv3.data_standards.fsw_data_type.FswDataType`
    :

    `enum: List[IrisBackendv3.data_standards.module.EnumItem]`
    :

    `is_enum: bool`
    :

    ### Methods

    `get_enum_formatted_str(self, data: Union[str, int]) ‑> Optional[str]`
    :   Returns a nicely formatted string containing the name and value of
        the enum item matching the given data (either name or value).

    `get_enum_item(self, idx: Union[int, str]) ‑> Optional[IrisBackendv3.data_standards.module.EnumItem]`
    :   Returns the EnumItem that matches the given `str` name or `int` value in the enum (if there is one).

    `get_enum_name(self, val: int) ‑> Optional[str]`
    :   Returns the name that matches the given value in the enum (if there is one).

    `get_enum_value(self, name: str) ‑> Optional[int]`
    :   Returns the value that matches the given name in the enum (if there is one).

`EnumItem(name: str, value: int, comment: str = '')`
:   Container for FPrime Enumeration Item (inside an Argument).

    ### Instance variables

    `comment: str`
    :   Return an attribute of instance, which is of type owner.

    `name: str`
    :   Return an attribute of instance, which is of type owner.

    `value: int`
    :   Return an attribute of instance, which is of type owner.

`Event(name: str = '', ID: int = -1, severity_str: str = '', format_string: str = '', metadata_json_str: str = '', args: List[Argument] = None)`
:   Container for Data Standards for an FPrime Event (aka Log) and associated Ground Software Metadata.

    ### Ancestors (in MRO)

    * IrisBackendv3.data_standards.module.GswMetadataContainer

    ### Class variables

    `SeverityLevel`
    :   Allowed Severity Levels for an FPrime Event.

    ### Instance variables

    `ID`
    :   Return an attribute of instance, which is of type owner.

    `args`
    :   Return an attribute of instance, which is of type owner.

    `format_string`
    :   Return an attribute of instance, which is of type owner.

    `name`
    :   Return an attribute of instance, which is of type owner.

    `severity: IrisBackendv3.data_standards.module.Event.SeverityLevel`
    :

    `severity_str: str`
    :

    ### Methods

    `check_and_correct_format_string(self) ‑> None`
    :   Make sure the format string's argument specifiers match the event
        args. Raise an exception if there's a problem.
        If there are correctable problems, they will be fixed.

    `grab_format_string_arg_specs(self) ‑> List[str]`
    :   Grabs all format specifiers from the format string.

`GswMetadataContainer(metadata_json_str: str)`
:   Base class for any data container which will include GSW-specific JSON metadata parsed from XML CDATA.
    
    Supports automatic parsing and caching of JSON strings to make loading data
    from a string easier.

    ### Descendants

    * IrisBackendv3.data_standards.module.Command
    * IrisBackendv3.data_standards.module.DataUnit
    * IrisBackendv3.data_standards.module.Event

    ### Instance variables

    `metadata: Dict[str, Any]`
    :   Return cached conversion of metadata json string to a dictionary.

    `metadata_json_str: str`
    :

`Module(name: str, ID: int, commands: NameIdDict[Command], telemetry: NameIdDict[TelemetryChannel], events: NameIdDict[Event])`
:   Container for Data Standards for a Module in the FSW.

    ### Instance variables

    `ID`
    :   Return an attribute of instance, which is of type owner.

    `commands`
    :   Return an attribute of instance, which is of type owner.

    `events`
    :   Return an attribute of instance, which is of type owner.

    `name`
    :   Return an attribute of instance, which is of type owner.

    `telemetry`
    :   Return an attribute of instance, which is of type owner.

`TelemetryChannel(name: str, ID: int, datatype: FswDataType, enum: List[EnumItem] = None, bitfields: BitfieldStruct = None, metadata_json_str: str = '')`
:   Container for Data Standards for an FPrime Telemetry Channel and associated Ground Software Metadata.

    ### Ancestors (in MRO)

    * IrisBackendv3.data_standards.module.DataUnit
    * IrisBackendv3.data_standards.module.GswMetadataContainer

    ### Instance variables

    `ID`
    :   Return an attribute of instance, which is of type owner.

    `name`
    :   Return an attribute of instance, which is of type owner.