Module IrisBackendv3.codec.ipc_messages
=======================================
Defines the Structure, Content, and Serialization for all incoming and outgoing 
Messages for the Codec layer over IPC.

@author: Connor W. Colombo (CMU)
@last-updated: 05/20/2021

Classes
-------

`PayloadsToPacketRequestContent(payloads: EnhancedPayloadCollection, packet_type: Type[Packet])`
:   Defines the Message Content for a Request to pack an
    `EnhancedPayloadCollection` to into a `Packet` of a given type.
    
    Method generated by attrs for class PayloadsToPacketRequestContent.

    ### Instance variables

    `packet_type: Type[IrisBackendv3.codec.packet_classes.packet.Packet]`
    :   Return an attribute of instance, which is of type owner.

    `payloads: IrisBackendv3.codec.payload_collection.EnhancedPayloadCollection`
    :   Return an attribute of instance, which is of type owner.

`PayloadsToPacketRequestMessage(content: IPMC)`
:   Defines the Message Structure and Serialization Scheme for a Request to 
    pack an `EnhancedPayloadCollection` to into a `Packet` of a given type.

    ### Ancestors (in MRO)

    * IrisBackendv3.ipc.inter_process_message.InterProcessMessage
    * typing.Generic
    * abc.ABC