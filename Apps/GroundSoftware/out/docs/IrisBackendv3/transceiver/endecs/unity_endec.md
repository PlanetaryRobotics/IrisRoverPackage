Module IrisBackendv3.transceiver.endecs.unity_endec
===================================================

Classes
-------

`UnityEndec()`
:   Abstract Base Class for encoder/decoder to convert bytes for `Transceiver`.
    messages.

    ### Ancestors (in MRO)

    * IrisBackendv3.transceiver.endecs.endec.Endec
    * abc.ABC

    ### Methods

    `decode(self, data: bytes) ‑> bytes`
    :   Decodes data sent over a `Transceiver`.
        Default is a unity decoder (just returns the bytes).

    `encode(self, data: bytes) ‑> bytes`
    :   Encodes data to send over a `Transceiver`.
        Default is a unity encoder (just returns the bytes).