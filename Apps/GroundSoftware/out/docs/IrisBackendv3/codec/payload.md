Module IrisBackendv3.codec.payload
==================================
Defines Common Data Required for Payloads. Support for Building and Parsing
Payloads as part of a Variable Length Payload.

@author: Connor W. Colombo (CMU)
@last-updated: 09/19/2022

Classes
-------

`CommandPayload(module_id: int, command_id: int, args: Dict[str, Any], magic: Magic = Magic.MISSING, pathway: DataPathway = DataPathway.NONE, source: DataSource = DataSource.NONE, raw: Optional[bytes] = None, endianness_code: str = '<', auto_tag_generated_time: bool = True, **kwargs)`
:   Implementation of Payload Interface for IRIS Telemetry Payloads.

    ### Ancestors (in MRO)

    * IrisBackendv3.codec.payload.CommandPayloadInterface
    * IrisBackendv3.codec.payload.UplinkedPayload
    * IrisBackendv3.codec.payload.Payload
    * IrisBackendv3.codec.payload.PayloadInterface
    * IrisBackendv3.codec.container.ContainerCodec
    * typing.Generic
    * abc.ABC

    ### Descendants

    * IrisBackendv3.codec.payload.WatchdogCommandPayload

    ### Methods

    `check_args(self) ‑> None`
    :   Checks whether this command's args exactly meet the command's spec.

`CommandPayloadInterface(magic: Magic = Magic.MISSING, pathway: DataPathway = DataPathway.NONE, source: DataSource = DataSource.NONE, raw: Optional[bytes] = None, endianness_code: str = '<', amcc: bool = False, rover_ack: bool = False, uplink_times: Optional[UplinkTimes] = None)`
:   Generic interface which defines all I/O for CommandPayload allowing it to be
    used as the Type Generic for CommandPayload, which in turn allows many of
    inherited functions to return CommandPayloads.
    
    In essence, this defines any variables, properties, and methods which are
    unique to CommandPayload (and not inherited from its superclasses).
    In addition, properties and not just their interfaces are defined here for
    convenience sake (to reduce code bulk).
    
    Note: this is basically required because you can't directly reference a
    class as its own generic fill, e.g.:
    `class CommandPayload(UplinkedPayload[CommandPayload])`

    ### Ancestors (in MRO)

    * IrisBackendv3.codec.payload.UplinkedPayload
    * IrisBackendv3.codec.payload.Payload
    * IrisBackendv3.codec.payload.PayloadInterface
    * IrisBackendv3.codec.container.ContainerCodec
    * typing.Generic
    * abc.ABC

    ### Descendants

    * IrisBackendv3.codec.payload.CommandPayload

    ### Instance variables

    `args: Dict[str, Any]`
    :

    `command: IrisBackendv3.data_standards.module.Command`
    :

    `command_id: int`
    :

    `module: IrisBackendv3.data_standards.module.Module`
    :

    `module_id: int`
    :

    `opcode: int`
    :

    ### Methods

    `check_args(self) ‑> None`
    :

`DownlinkedPayload(magic: Magic = Magic.MISSING, pathway: DataPathway = DataPathway.NONE, source: DataSource = DataSource.NONE, raw: Optional[bytes] = None, endianness_code: str = '<', downlink_times: Optional[DownlinkTimes] = None)`
:   Generic superclass for all payloads which are downlinked.
    Allows for creating typed containers of just downlinked payloads.

    ### Ancestors (in MRO)

    * IrisBackendv3.codec.payload.Payload
    * IrisBackendv3.codec.payload.PayloadInterface
    * IrisBackendv3.codec.container.ContainerCodec
    * typing.Generic
    * abc.ABC

    ### Descendants

    * IrisBackendv3.codec.payload.EventPayloadInterface
    * IrisBackendv3.codec.payload.FileBlockPayloadInterface
    * IrisBackendv3.codec.payload.TelemetryPayloadInterface

    ### Instance variables

    `downlink_times: Optional[IrisBackendv3.codec.metadata.DownlinkTimes]`
    :   Return an attribute of instance, which is of type owner.

`EventPayload(module_id: int, event_id: int, args: Dict[str, Any], timestamp: int, magic: Magic = Magic.MISSING, pathway: DataPathway = DataPathway.NONE, source: DataSource = DataSource.NONE, raw: Optional[bytes] = None, endianness_code: str = '<')`
:   Implementation of Payload Interface for IRIS Event Payloads.

    ### Ancestors (in MRO)

    * IrisBackendv3.codec.payload.EventPayloadInterface
    * IrisBackendv3.codec.payload.DownlinkedPayload
    * IrisBackendv3.codec.payload.Payload
    * IrisBackendv3.codec.payload.PayloadInterface
    * IrisBackendv3.codec.container.ContainerCodec
    * typing.Generic
    * abc.ABC

    ### Methods

    `check_args(self) ‑> None`
    :   Checks whether this events's args exactly meet the events's spec.

`EventPayloadInterface(magic: Magic = Magic.MISSING, pathway: DataPathway = DataPathway.NONE, source: DataSource = DataSource.NONE, raw: Optional[bytes] = None, endianness_code: str = '<', downlink_times: Optional[DownlinkTimes] = None)`
:   Generic interface which defines all I/O for EventPayload allowing it to be
    used as the Type Generic for EventPayload, which in turn allows many of
    inherited functions to return EventPayload.
    
    In essence, this defines any variables, properties, and methods which are
    unique to EventPayload (and not inherited from its superclasses).
    In addition, properties and not just their interfaces are defined here for
    convenience sake (to reduce code bulk).
    
    Note: this is basically required because you can't directly reference a
    class as its own generic fill, e.g.:
    `class EventPayload(DownlinkedPayload[EventPayload])`

    ### Ancestors (in MRO)

    * IrisBackendv3.codec.payload.DownlinkedPayload
    * IrisBackendv3.codec.payload.Payload
    * IrisBackendv3.codec.payload.PayloadInterface
    * IrisBackendv3.codec.container.ContainerCodec
    * typing.Generic
    * abc.ABC

    ### Descendants

    * IrisBackendv3.codec.payload.EventPayload

    ### Instance variables

    `args: collections.OrderedDict[str, typing.Any]`
    :

    `event: IrisBackendv3.data_standards.module.Event`
    :

    `event_id: int`
    :

    `formatted_string: str`
    :   Use the parsed args to populate the Event's format string.

    `module: IrisBackendv3.data_standards.module.Module`
    :

    `module_id: int`
    :

    `opcode: int`
    :

    `timestamp: int`
    :

`FileBlockPayload(hashed_id: int, total_blocks: int, block_number: int, length: int, data: bytes, possible_corruption: bool = False, file_metadata: Optional[FileMetadata] = None, magic: Magic = Magic.MISSING, pathway: DataPathway = DataPathway.NONE, source: DataSource = DataSource.NONE, raw: Optional[bytes] = None, endianness_code: str = '<')`
:   Implementation of Payload Interface for IRIS File Blocks.

    ### Ancestors (in MRO)

    * IrisBackendv3.codec.payload.FileBlockPayloadInterface
    * IrisBackendv3.codec.payload.DownlinkedPayload
    * IrisBackendv3.codec.payload.Payload
    * IrisBackendv3.codec.payload.PayloadInterface
    * IrisBackendv3.codec.container.ContainerCodec
    * typing.Generic
    * abc.ABC

    ### Static methods

    `decode(data: bytes, endianness_code: str = '<') ‑> IrisBackendv3.codec.payload.FileBlockPayload`
    :   From C&TL at time of writing (10/01/2021):
        File Block Format:
        Field           Type                Description
        hashedId            uint16_t            A hash of the timestamp when file transmission started. Used to identify which blocks belong to the same file if multiple files are being sent at once. *NOT* a globally unique file identifier. Two files over the course of the entire mission could have the same hashedId. This is only unique up to the point that no two files with overlapping transmission periods should have the same hashedId.
        totalBlocks         uint8_t                 Total number of blocks in this file (not including metadata block)
        blockNumber         uint8_t                 Number of this block (**1 indexed** - 0 is for the metadata)
        length          FileLength_t    Number of bytes in this block (not including header data - appears to be uint32_t in FSW as of 09/30/2021. Let's stick to this.)
        data            uint8_t[]               All the data bytes. * The data of each block will be compressed using heatshrink

`FileBlockPayloadInterface(magic: Magic = Magic.MISSING, pathway: DataPathway = DataPathway.NONE, source: DataSource = DataSource.NONE, raw: Optional[bytes] = None, endianness_code: str = '<', downlink_times: Optional[DownlinkTimes] = None)`
:   Generic interface which defines all I/O for FileBlockPayload allowing it to be
    used as the Type Generic for FileBlockPayload, which in turn allows many of
    inherited functions to return FileBlockPayload.
    
    In essence, this defines any variables, properties, and methods which are
    unique to FileBlockPayload (and not inherited from its superclasses).
    In addition, properties and not just their interfaces are defined here for
    convenience sake (to reduce code bulk).
    
    Note: this is basically required because you can't directly reference a
    class as its own generic fill, e.g.:
    `class FileBlockPayload(DownlinkedPayload[EventPayload])`

    ### Ancestors (in MRO)

    * IrisBackendv3.codec.payload.DownlinkedPayload
    * IrisBackendv3.codec.payload.Payload
    * IrisBackendv3.codec.payload.PayloadInterface
    * IrisBackendv3.codec.container.ContainerCodec
    * typing.Generic
    * abc.ABC

    ### Descendants

    * IrisBackendv3.codec.payload.FileBlockPayload

    ### Instance variables

    `block_number: int`
    :

    `data: bytes`
    :

    `file_metadata: Optional[IrisBackendv3.codec.payload.FileMetadata]`
    :

    `hashed_id: int`
    :

    `length: int`
    :

    `possible_corruption: bool`
    :

    `total_blocks: int`
    :

`FileMetadata(callback_id: int, timestamp: int, file_type_magic: FileMetadata.FileTypeMagic, raw: Optional[bytes] = None, endianness_code: str = '<')`
:   Metadata about the file contained inside Block 0 of a `FileBlockPayload`.

    ### Ancestors (in MRO)

    * IrisBackendv3.codec.payload.FileMetadataInterface
    * IrisBackendv3.codec.container.ContainerCodec
    * typing.Generic
    * abc.ABC

    ### Static methods

    `decode(data: bytes, endianness_code: str = '<') ‑> IrisBackendv3.codec.payload.FileMetadata`
    :   From C&TL at time of writing (10/01/2021):
        File Metadata Format:   (data in block 0)
        Field           Type            Description
        callbackId          uint16_t    Monotonically increasing callback ID of the command that triggered this file to be sent. Uniquely links this file to the command which requested it. Callback IDs are unique for images and UWB files. Callback IDs for commands requesting images start counting at 0. Callback IDs for commands requesting UWB files start counting at 2^5
        timestamp           uint32_t    Time (in ms since Hercules power up) of when this file was generated.
        fileTypeMagic   uint8_t         [See below]
        
        File        Type Magic (in Metadata):   (data in block 0, inside File Metadata)
        Image:      0x01
        UWB File:       0x0F

`FileMetadataInterface(raw: Optional[bytes] = None, endianness_code: str = '<')`
:   Generic interface which defines all I/O for FileMetadata allowing it to be
    used as the Type Generic for FileMetadata, which in turn allows many of
    inherited functions to return FileMetadata.
    
    In essence, this defines any variables, properties, and methods which are
    unique to FileMetadata (and not inherited from its superclasses).
    In addition, properties and not just their interfaces are defined here for
    convenience sake (to reduce code bulk).
    
    Note: this is basically required because you can't directly reference a
    class as its own generic fill, e.g.:
    `class FileMetadata(ContainerCodec[FileMetadata])`

    ### Ancestors (in MRO)

    * IrisBackendv3.codec.container.ContainerCodec
    * typing.Generic
    * abc.ABC

    ### Descendants

    * IrisBackendv3.codec.payload.FileMetadata

    ### Class variables

    `FileTypeMagic`
    :   Enumeration of all file types which could be downlinked.
        
        As usual, for backwards compatibility and data preservation, *never* change
        any of the enum values or delete entries, just deprecate old ones.

    ### Instance variables

    `callback_id: int`
    :

    `file_type_magic: FileTypeMagic`
    :

    `timestamp: int`
    :

`Payload(magic: Magic = Magic.MISSING, pathway: DataPathway = DataPathway.NONE, source: DataSource = DataSource.NONE, raw: Optional[bytes] = None, endianness_code: str = '<')`
:   Generic Interface for Handling and Processing Payloads.
    
    All data is stored in the unpacked form since that's how it will most
    often be handled and transformed and only packed into bytes when needed.
    
    *However* the raw form of the data (as downlinked) is preserved for forensic
    reasons and is used as the sole way of serializing and storing data so that
    if, for example, adjustments need to be made to the decoding script (perhaps
    to correct for a persistent data corruption mode, etc.), the raw data will
    be preserved and can simply be reprocessed with a fixed decoding script.
    
    *NOTE:* All Payload data is encoded and decoded with the `magic` stripped
    off.
    
    All additional payload-specific metadata (including the `magic`, data
    `pathway`, and `source`) will be serialized on the ground as a state
    alongside the `raw` bytes.

    ### Ancestors (in MRO)

    * IrisBackendv3.codec.payload.PayloadInterface
    * IrisBackendv3.codec.container.ContainerCodec
    * typing.Generic
    * abc.ABC

    ### Descendants

    * IrisBackendv3.codec.payload.DownlinkedPayload
    * IrisBackendv3.codec.payload.UplinkedPayload

    ### Static methods

    `process(VLP: bytes, endianness_code: str = '<') ‑> Tuple[~PT, bytes]`
    :   Processes the given VLP *with the appropriate magic already stripped off*
        as a payload of this type and returns the appropriate payload object as
        well as the VLP with the consumed bytes stripped off.

`PayloadInterface(raw: Optional[bytes] = None, endianness_code: str = '<')`
:   Generic Payload Interface/Wrapper class to enable binding the Payload's
    generic type to something that lists all the right interfaces.

    ### Ancestors (in MRO)

    * IrisBackendv3.codec.container.ContainerCodec
    * typing.Generic
    * abc.ABC

    ### Descendants

    * IrisBackendv3.codec.payload.Payload

    ### Static methods

    `process(VLP: bytes, endianness_code: str = '<') ‑> Tuple[~PIT, bytes]`
    :

    ### Instance variables

    `magic: IrisBackendv3.codec.magic.Magic`
    :   Return an attribute of instance, which is of type owner.

    `pathway: IrisBackendv3.codec.metadata.DataPathway`
    :   Return an attribute of instance, which is of type owner.

    `source: IrisBackendv3.codec.metadata.DataSource`
    :   Return an attribute of instance, which is of type owner.

`TelemetryPayload(module_id: int, channel_id: int, data: Any, timestamp: int, magic: Magic = Magic.MISSING, pathway: DataPathway = DataPathway.NONE, source: DataSource = DataSource.NONE, raw: Optional[bytes] = None, endianness_code: str = '<')`
:   Implementation of Payload Interface for IRIS Telemetry Payloads.

    ### Ancestors (in MRO)

    * IrisBackendv3.codec.payload.TelemetryPayloadInterface
    * IrisBackendv3.codec.payload.DownlinkedPayload
    * IrisBackendv3.codec.payload.Payload
    * IrisBackendv3.codec.payload.PayloadInterface
    * IrisBackendv3.codec.container.ContainerCodec
    * typing.Generic
    * abc.ABC

`TelemetryPayloadInterface(magic: Magic = Magic.MISSING, pathway: DataPathway = DataPathway.NONE, source: DataSource = DataSource.NONE, raw: Optional[bytes] = None, endianness_code: str = '<', downlink_times: Optional[DownlinkTimes] = None)`
:   Generic interface which defines all I/O for TelemetryPayload allowing it to be
    used as the Type Generic for TelemetryPayload, which in turn allows many of
    inherited functions to return TelemetryPayload.
    
    In essence, this defines any variables, properties, and methods which are
    unique to TelemetryPayload (and not inherited from its superclasses).
    In addition, properties and not just their interfaces are defined here for
    convenience sake (to reduce code bulk).
    
    Note: this is basically required because you can't directly reference a
    class as its own generic fill, e.g.:
    `class TelemetryPayload(DownlinkedPayload[TelemetryPayload])`

    ### Ancestors (in MRO)

    * IrisBackendv3.codec.payload.DownlinkedPayload
    * IrisBackendv3.codec.payload.Payload
    * IrisBackendv3.codec.payload.PayloadInterface
    * IrisBackendv3.codec.container.ContainerCodec
    * typing.Generic
    * abc.ABC

    ### Descendants

    * IrisBackendv3.codec.payload.TelemetryPayload

    ### Instance variables

    `channel: IrisBackendv3.data_standards.module.TelemetryChannel`
    :

    `channel_id: int`
    :

    `data: Any`
    :

    `module: IrisBackendv3.data_standards.module.Module`
    :

    `module_id: int`
    :

    `opcode: int`
    :

    `timestamp: int`
    :

`UplinkedPayload(magic: Magic = Magic.MISSING, pathway: DataPathway = DataPathway.NONE, source: DataSource = DataSource.NONE, raw: Optional[bytes] = None, endianness_code: str = '<', amcc: bool = False, rover_ack: bool = False, uplink_times: Optional[UplinkTimes] = None)`
:   Generic superclass for all payloads which are uplinked.
    Allows for creating typed containers of just uplinked payloads.

    ### Ancestors (in MRO)

    * IrisBackendv3.codec.payload.Payload
    * IrisBackendv3.codec.payload.PayloadInterface
    * IrisBackendv3.codec.container.ContainerCodec
    * typing.Generic
    * abc.ABC

    ### Descendants

    * IrisBackendv3.codec.payload.CommandPayloadInterface

    ### Instance variables

    `amcc_ack: bool`
    :   Return an attribute of instance, which is of type owner.

    `rover_ack: bool`
    :   Return an attribute of instance, which is of type owner.

    `uplink_times: Optional[IrisBackendv3.codec.metadata.UplinkTimes]`
    :   Return an attribute of instance, which is of type owner.

`WatchdogCommandPayload(module_id: int, command_id: int, args: Dict[str, Any], magic: Magic = Magic.MISSING, pathway: DataPathway = DataPathway.NONE, source: DataSource = DataSource.NONE, raw: Optional[bytes] = None, endianness_code: str = '<', auto_tag_generated_time: bool = True, **kwargs)`
:   Same as CommandPayload but with a special U8 Enum implementation.

    ### Ancestors (in MRO)

    * IrisBackendv3.codec.payload.CommandPayload
    * IrisBackendv3.codec.payload.CommandPayloadInterface
    * IrisBackendv3.codec.payload.UplinkedPayload
    * IrisBackendv3.codec.payload.Payload
    * IrisBackendv3.codec.payload.PayloadInterface
    * IrisBackendv3.codec.container.ContainerCodec
    * typing.Generic
    * abc.ABC