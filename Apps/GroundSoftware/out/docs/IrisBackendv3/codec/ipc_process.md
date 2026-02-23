Module IrisBackendv3.codec.ipc_process
======================================
Standalone codec process which converts bytes->packet or payloads->packet 
(so long as any network headers have already been stripped off).

TODO: Redirect logs to an independent logging process.

@author: Connor W. Colombo (CMU)
@last-updated: 02/26/2022

Functions
---------

    
`bytes_to_packet(incoming_msg_data: bytes) ‑> IrisBackendv3.ipc.inter_process_message.InterProcessMessage`
:   

    
`payloads_to_packet(incoming_msg_data: bytes) ‑> IrisBackendv3.ipc.inter_process_message.InterProcessMessage`
:   

    
`run() ‑> None`
:   

Classes
-------

`SubTopic(value, names=None, *, module=None, qualname=None, type=None, start=1)`
:   An enumeration.

    ### Ancestors (in MRO)

    * enum.Enum

    ### Class variables

    `BYTES_TO_PACKET`
    :

    `PAYLOADS_TO_PACKET`
    :

    `handler: IrisBackendv3.ipc.inter_process_message.IPMHandler`
    :