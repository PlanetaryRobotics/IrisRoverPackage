Module IrisBackendv3.codec.container
====================================
Core interface definition for all container classes whose data should be
encoded/decoded to be passed in/out of the Transceiver layer.

@author: Connor W. Colombo (CMU)
@last-updated: 03/03/2022

Classes
-------

`ContainerCodec(raw: Optional[bytes] = None, endianness_code: str = '<')`
:   Core interface definition for all container classes whose data should be
    encoded/decoded to be passed in/out of the Transceiver layer.
    
    Also serializable (pickleable) as raw data.
    
    Note: often [CT] is the implementing subclass.

    ### Ancestors (in MRO)

    * typing.Generic
    * abc.ABC

    ### Descendants

    * IrisBackendv3.codec.packet_classes.iris_common.IrisCommonPacket.CommonPacketHeaderInterface
    * IrisBackendv3.codec.packet_classes.iris_common.Legacy2020IrisCommonPacket.Legacy2020CommonPacketHeaderInterface
    * IrisBackendv3.codec.packet_classes.packet.Packet
    * IrisBackendv3.codec.payload.FileMetadataInterface
    * IrisBackendv3.codec.payload.PayloadInterface

    ### Static methods

    `decode(data: bytes, endianness_code: str = '<') ‑> ~CT`
    :   Decodes the given bytes buffer.

    ### Instance variables

    `endianness_code: str`
    :

    `raw: Optional[bytes]`
    :

    ### Methods

    `encode(self, **kwargs: Any) ‑> bytes`
    :   Encodes the data contained in this instance using this class' formatting.
        (normally should just use instance's contained data and not require any
        `kwargs`). When implementing a subclass, if you need to pass through
        `kwargs`, just make sure all overriding methods still work even if
        `kwargs` aren't supplied (that is, `kwargs` should be purely
        supplemental information)