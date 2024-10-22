Module IrisBackendv3.codec.packet_classes.iris_common
=====================================================
Defines `IrisCommonPacket` and `Legacy2020IrisCommonPacket`, implementations of
codec standards for all data sent over DSN for Iris, except data which is
parsed by the Watchdog. `IrisCommonPacket` is the current and
`Legacy2020IrisCommonPacket` represents a deprecated standard which is still
required for backwards compatibility (reading old archived pcaps).

@author: Connor W. Colombo (CMU)
@last-updated: 06/22/2023

Classes
-------

`IrisCommonPacket(payloads: EnhancedPayloadCollection, seq_num: int = 0, common_packet_header: Optional[CommonPacketHeader] = None, ignore_state_members: bool = False, raw: Optional[bytes] = None, endianness_code: str = '<')`
:   Defines Common Data Required for All Packets following the Iris Common
    Packet standard.
    
    Packets follow the standard defined in the IRIS Command & Telemetry List.
    
    All data is stored in the unpacked form since that's how it will most often
    be handled and transformed and only packed into bytes when needed.
    
    @author: Connor W. Colombo (CMU)
    @last-updated: 06/22/2023

    ### Ancestors (in MRO)

    * IrisBackendv3.codec.packet_classes.iris_common.IrisCommonPacketInterface
    * IrisBackendv3.codec.packet_classes.packet.Packet
    * IrisBackendv3.codec.container.ContainerCodec
    * typing.Generic
    * abc.ABC

    ### Class variables

    `CommonPacketHeader`
    :   Container for Handling and Processing CommonPacketHeaders.
        
        All data is stored in the unpacked form since that's how it will most
        often be handled and transformed and only packed into bytes when needed.

    `CommonPacketHeaderInterface`
    :   Core interface definition for all container classes whose data should be
        encoded/decoded to be passed in/out of the Transceiver layer.
        
        Also serializable (pickleable) as raw data.
        
        Note: often [CT] is the implementing subclass.

    `MTU_HERCULES: int`
    :

    `MTU_WATCHDOG: int`
    :

    ### Static methods

    `decode(data: bytes, endianness_code: str = '<') ‑> IrisBackendv3.codec.packet_classes.iris_common.IrisCommonPacket`
    :   Construct a Iris Packet Object from Bytes.
        NOTE: This does not add metadata to the Payloads like `Pathway`,
        `Source`, or `DownlinkTimes`. That needs to be added to the
        Payloads at the `Transceiver` layer.

    ### Instance variables

    `common_packet_header: IrisBackendv3.codec.packet_classes.iris_common.IrisCommonPacket.CommonPacketHeader`
    :

`IrisCommonPacketInterface(payloads: EnhancedPayloadCollection, raw: Optional[bytes] = None, endianness_code: str = '<')`
:   A special `CodecContainer` used for Packet data which combines the
    standardized data-encoding features of `ContainerCodec` with a standard
    `EnhancedPayloadCollection`. All packets are essentially 
    `EnhancedPayloadCollection`s with specialized encoding and decoding schemes
    applied. Each `Packet` en/decodes raw bytes sent over a `Transceiver` layer
    with any network headers (IP etc.) already stripped off.

    ### Ancestors (in MRO)

    * IrisBackendv3.codec.packet_classes.packet.Packet
    * IrisBackendv3.codec.container.ContainerCodec
    * typing.Generic
    * abc.ABC

    ### Descendants

    * IrisBackendv3.codec.packet_classes.iris_common.IrisCommonPacket
    * IrisBackendv3.codec.packet_classes.iris_common.Legacy2020IrisCommonPacket

`Legacy2020IrisCommonPacket(payloads: EnhancedPayloadCollection, seq_num: int = 0, common_packet_header: Optional[Legacy2020CommonPacketHeader] = None, raw: Optional[bytes] = None, endianness_code: str = '<')`
:   Legacy Version of `IrisCommonPacket` to support parsing old FSW pcaps from
    2020 and early 2021 which use a 2B checksum (which often wasn't populated).
    
    @author: Connor W. Colombo (CMU)
    @last-updated: 10/02/2021

    ### Ancestors (in MRO)

    * IrisBackendv3.codec.packet_classes.iris_common.IrisCommonPacketInterface
    * IrisBackendv3.codec.packet_classes.packet.Packet
    * IrisBackendv3.codec.container.ContainerCodec
    * typing.Generic
    * abc.ABC

    ### Class variables

    `LEGACY2020_CPH_SIZE: int`
    :

    `Legacy2020CommonPacketHeader`
    :   Legacy Version of `CommonPacketHeader` to support parsing old FSW pcaps from
        2020 and early 2021 which use a 2B checksum (which often wasn't populated)
        and certain standards were adhered to more laxly.
        
        SHOULD NOT MAKE IT INTO MISSION SOFTWARE.

    `Legacy2020CommonPacketHeaderInterface`
    :   Core interface definition for all container classes whose data should be
        encoded/decoded to be passed in/out of the Transceiver layer.
        
        Also serializable (pickleable) as raw data.
        
        Note: often [CT] is the implementing subclass.

    ### Static methods

    `decode(data: bytes, endianness_code: str = '<') ‑> IrisBackendv3.codec.packet_classes.iris_common.Legacy2020IrisCommonPacket`
    :   Construct a Iris Packet Object from Bytes.

    ### Instance variables

    `common_packet_header: IrisBackendv3.codec.packet_classes.iris_common.Legacy2020IrisCommonPacket.Legacy2020CommonPacketHeader`
    :