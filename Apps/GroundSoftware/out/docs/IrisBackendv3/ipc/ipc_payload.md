Module IrisBackendv3.ipc.ipc_payload
====================================
Defines a simple container class for data sent over IPC (i.e. a container for
the raw message data alongside routing information (topic, subtopic)).

NOTE: This code used to live inside `wrapper.py`

@author: Connor W. Colombo (CMU)
@last-updated: 03/04/2023

Classes
-------

`IpcPayload(topic_bytes: bytes = b'', subtopic_bytes: bytes = b'', msg_bytes: bytes = b'')`
:   Data sent over IPC.

    ### Class variables

    `msg_bytes: bytes`
    :

    `subtopic_bytes: bytes`
    :

    `topic_bytes: bytes`
    :

    ### Instance variables

    `message`
    :   Builds a message of the type that corresponds to this payload's
        topic.
        NOTE: This value is cached, so it's only computed once per payload
        object.

    `topic: IrisBackendv3.ipc.topics_registry.Topic`
    :