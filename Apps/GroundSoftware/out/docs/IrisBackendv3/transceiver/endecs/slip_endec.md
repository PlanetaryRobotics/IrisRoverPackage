Module IrisBackendv3.transceiver.endecs.slip_endec
==================================================

Classes
-------

`SlipEndec()`
:   Implementation of SLIP (Serial Line Internet Protocol) per RFC-1055,
    based on the implementation in `sliplib 0.6.2` by Ruud de Jong.
    
    NOTE: Although this class is used internally in the `SlipTransceiver` class,
    you **don't**  need to (and shouldn't) include `SlipEndec` in the `endecs`
    list.

    ### Ancestors (in MRO)

    * IrisBackendv3.transceiver.endecs.endec.Endec
    * abc.ABC

    ### Class variables

    `B_END: ClassVar[bytes]`
    :

    `B_ESC: ClassVar[bytes]`
    :

    `B_ESC_END: ClassVar[bytes]`
    :

    `B_ESC_ESC: ClassVar[bytes]`
    :

    ### Methods

    `decode(self, data: bytes) ‑> bytes`
    :   Decodes data sent over a `Transceiver`.

    `encode(self, data: bytes) ‑> bytes`
    :   Encodes data to send over a `Transceiver`.
        
        Wraps the given data in SLIP.

    `is_valid(self, data: bytes) ‑> bool`
    :   Returns whether the packet's contents conform to the SLIP spec.
        A packet is valid if:
        * It contains no `B_END` bytes other than leading and/or trailing `B_END` bytes
        * Each `B_ESC` byte is followed by either an `B_ESC_END` or an `B_ESC_ESC` byte.