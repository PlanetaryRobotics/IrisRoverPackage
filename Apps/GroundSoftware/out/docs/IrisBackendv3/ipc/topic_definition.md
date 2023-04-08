Module IrisBackendv3.ipc.topic_definition
=========================================
Class whose instances define an IPC topic, including what messages are passed
on the topic.

@author: Connor W. Colombo (CMU)
@last-updated: 03/03/2023

Classes
-------

`TopicDefinition(message_def: Type[IrisBackendv3.ipc.inter_process_message.InterProcessMessage])`
:   TopicDefinition(message_def: Type[IrisBackendv3.ipc.inter_process_message.InterProcessMessage])

    ### Class variables

    `message_def: Type[IrisBackendv3.ipc.inter_process_message.InterProcessMessage]`
    :

    ### Instance variables

    `topic_type: Union[Type, _BaseGenericAlias, Tuple, Callable, _SpecialForm]`
    :   Instance attributes for type-checker:
        Type of data written to the topic (can be any valid mypy type):

    ### Methods

    `allows(self, data: Any) ‑> bool`
    :   Checks whether the given data is allowed on the given topic based on
        its type.