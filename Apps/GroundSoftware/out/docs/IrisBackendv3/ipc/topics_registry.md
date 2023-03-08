Module IrisBackendv3.ipc.topics_registry
========================================
This module supplies an Enum which defines all topics used in the IPC on
pub/sub ports.

This registry serves two purposes:
  1. Assign `TopicDefinitions` to ports.
  2. Provide an enumeration of all available `Topics`.

@author: Connor W. Colombo (CMU)
@last-updated: 03/06/2023

Classes
-------

`Topic(value, names=None, *, module=None, qualname=None, type=None, start=1)`
:   Enum for defines all ports used in the IPC.
    
    This enum should serve as the single source of truth for the topic 
    information.

    ### Ancestors (in MRO)

    * enum.Enum

    ### Class variables

    `DL_PACKETS`
    :

    `DL_PAYLOADS`
    :

    `UL_PACKET`
    :

    `UL_PAYLOADS`
    :

    `definition: IrisBackendv3.ipc.topic_definition.TopicDefinition`
    :

    ### Methods

    `allows(self, data: Any) ‑> bool`
    :   Checks whether the given data is allowed on the given topic based on
        its type.