Module IrisBackendv3.ipc.inter_process_message
==============================================
Defines the container structure for messages that pass through `Topic`s
between IPC Apps.

NOTE: This code used to live inside `wrapper.py`

@author: Connor W. Colombo (CMU)
@last-updated: 03/06/2023

Functions
---------

    
`IpmSubclassFactory(ipmc_t: Type[IPMC]) ‑> Type[IrisBackendv3.ipc.inter_process_message.InterProcessMessage[~IPMC]]`
:   Factory that handles the boiler plate of creating a basic subclass of
    `InterProcessMessage` (basically removes the repetition).
    
    Subclasses that require special functionality can still be made normally,
    just make sure to include any of the necessary boilerplate included here.

    
`IsIPMHandler(func: IPMHandler) ‑> IrisBackendv3.ipc.inter_process_message.IPMHandler`
:   Decorator to use before a function to have `mypy` make sure it complies
    with the `IPMHandler` callback protocol.
    Example:
    ```py
    @IsIPMHandler
    def good_ipmh(msg_bytes: bytes) -> InterProcessMessage:
        pass
    ```
    creates no errors.
    
    ```py
    @IsIPMHandler
    def bad_ipmh(x: int, y: Dict) -> float:
        pass
    ```
    gives a mypy error b/c it's argument and return type signatures are wrong.
    
    but

    
`check_msg(msg: _IPMT, msg_type: Type[_IPMT]) ‑> TypeGuard[~_IPMT]`
:   Narrowing `TypeGuard` to check if the given message is an instance of
    the `InterProcessMessage` given.
    If it is, returns `True` and type of `msg` is now `_IPMT`.

    
`guard_msg(msg: _IPMT, msg_type: Type[_IPMT]) ‑> ~_IPMT`
:   Narrowing type guard to make sure the given message is an instance of
    the `InterProcessMessage` given.

Classes
-------

`IPMHandler(*args, **kwargs)`
:   Structural typing `Protocol` defining a handler that decodes raw
    IPC message bytes, performs some operation, and optionally returns an
    `InterProcessMessage` if needed by the process type (i.e. server/client).
    
    NOTE:
    Can't do something as simple as `Callable[[bytes], InterProcessMessage])`
    b/c of this still open (as of 2/26/22) issue:
    https://github.com/python/mypy/issues/5485
    which prevents `subtopic.handler(ipc_raw.msg)` or even
    `subtopic.handler.__call__(ipc_raw.msg)` from working.

    ### Ancestors (in MRO)

    * typing.Protocol
    * typing.Generic

`InterProcessMessage(content: IPMC)`
:   Interface for any message data which supports being sent between processes.

    ### Ancestors (in MRO)

    * typing.Generic
    * abc.ABC

    ### Descendants

    * IrisBackendv3.ipc.inter_process_message.IpmSubclassFactory.<locals>.SubIpm
    * IrisBackendv3.ipc.inter_process_message.IpmSubclassFactory.<locals>.SubIpm
    * IrisBackendv3.ipc.inter_process_message.IpmSubclassFactory.<locals>.SubIpm
    * IrisBackendv3.ipc.inter_process_message.IpmSubclassFactory.<locals>.SubIpm

    ### Static methods

    `CONTENT_TYPE() ‑> Type[~IPMC]`
    :   The type/class that `content` is checked against **at runtime**.
        This should be the same as `IPMC`.
        This has to be handled as a separate variable b/c it's difficult to
        reliably check types against a type-hint (esp. a `TypeVar`) at runtime,
        particularly in a way that doesn't depend on private python
        implementations and is supported across many python versions.
        NOTE: `mypy` should be able to automatically check to make sure a
        compatible type is returned here. Again, this should be **the same** as
        what is supplied for `IPMC`. `IpmSubclassFactory` enforces this.
        NOTE: This effectively needs to be an abstract static class variable,
        but since that's not supported by python, an abstract staticmethod
        which returns a fixed result is functionally equivalent.

    `from_ipc_bytes(data: bytes) ‑> ~_IPMT`
    :   Unpack bytes sent over IPC to reconstruct the sent object.
        Default behavior just uses `ipc.serializer` to de/serialize
        `content` but this can be overridden in a subclass.
        Since the data unpacked could hypothetically have any type allowed by
        the restricted unpickler, this will validate that the unpacked data has
        `cls.CONTENT_TYPE`.

    ### Instance variables

    `content: ~IPMC`
    :   Return an attribute of instance, which is of type owner.

    ### Methods

    `to_ipc_bytes(self) ‑> bytes`
    :   Pack this object into bytes to be sent over the IPC network
        (in a safe way, unlike pickle).
        Default behavior just uses `ipc.serializer` to de/serialize
        `content` but this can be overridden in a subclass.

`MessageContentAttrMixin()`
:   Mixin that adds `to_tuple` functionality to any `MessageContent` classes
    made using `attr`.

    ### Ancestors (in MRO)

    * IrisBackendv3.ipc.inter_process_message.MessageContentInterface
    * abc.ABC

    ### Descendants

    * IrisBackendv3.ipc.messages.m_downlinked_packets.DownlinkedPacketsContent
    * IrisBackendv3.ipc.messages.m_downlinked_payloads.DownlinkedPayloadsContent
    * IrisBackendv3.ipc.messages.m_uplink_packet_request.UplinkPacketRequestContent

    ### Static methods

    `from_dict(d: Dict) ‑> ~IPMC`
    :

    ### Methods

    `to_dict(self: attr.AttrsInstance) ‑> Dict`
    :

`MessageContentInterface()`
:   Basic interface specifying that all `content` classes must be able to be
    decomposed to a dict of objects that can be serialized using
    `IrisGenericSerializer` (i.e. they are on the approved modules list) using
    a method called `to_dict` and rebuilt using a method called
    `from_dict`.

    ### Ancestors (in MRO)

    * abc.ABC

    ### Descendants

    * IrisBackendv3.ipc.inter_process_message.MessageContentAttrMixin
    * IrisBackendv3.ipc.messages.m_uplink_payloads_request.UplinkPayloadsRequestContent

    ### Static methods

    `decode(d: Dict) ‑> ~IPMC`
    :   Basically just `from_dict` but checks a field that includes the
        class name to make sure the class rebuilt on the other end is the
        correct class, not just a class with the same fields.
        Also checks the restricted pickler and serializer versions,
        to act as a compatibility check.

    `from_dict(d: Dict) ‑> ~IPMC`
    :

    ### Methods

    `encode(self) ‑> Dict`
    :   Basically just `to_dict` but adds a field that includes the class
        name to make sure the class rebuilt on the other end is the correct
        class, not just a class with the same fields.
        Also adds a check for the restricted pickler and serializer versions,
        to act as a compatibility check.

    `to_dict(self) ‑> Dict`
    :