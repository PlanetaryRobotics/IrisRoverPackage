Module IrisBackendv3.codec.magic
================================
This module supplies an Enum which defines all the latest 4B 'magic' identifiers 
used in the IRIS Common Packet Headers (CPH) to determine the type of a payload 
within the Variable Length Payload (VLP) section of the CPH.

**Note:** Proper standards are followed and **Enum values should never be 
deleted from Magic**, only marked as deprecated. Once a magic value is used, it 
should remain persistently and never be later removed or repurposed.

@author: Connor W. Colombo (CMU)
@last-updated: 09/13/2022

Classes
-------

`Magic(value, names=None, *, module=None, qualname=None, type=None, start=1)`
:   Enum for Representing Iris Common Packet Header (CPH) Magic identifiers 
    used in the Variable Length Payload (VLP).

    ### Ancestors (in MRO)

    * enum.Enum

    ### Class variables

    `COMMAND`
    :

    `EVENT`
    :

    `FILE`
    :

    `MISSING`
    :

    `RADIO_COMMAND`
    :

    `TELEMETRY`
    :

    `WATCHDOG_COMMAND`
    :

    `deprecated: bool`
    :

    `description: str`
    :

    ### Static methods

    `decode(data: bytes, byte_order='<') ‑> IrisBackendv3.codec.magic.Magic`
    :   Given a data chunk as a `bytes` object, this converts it to the 
        appropriate Magic is given.

    ### Methods

    `encode(self, byte_order='<') ‑> bytes`
    :   Returns an encoded interpretation of this Magic, using the supplied 
        `byte_order` symbol if given or `!` (network order) if not.