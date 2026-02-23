Module IrisBackendv3.transceiver.endecs.endec
=============================================

Classes
-------

`Endec()`
:   Abstract Base Class for encoder/decoder to convert bytes for `Transceiver`.
    messages.

    ### Ancestors (in MRO)

    * abc.ABC

    ### Descendants

    * IrisBackendv3.transceiver.endecs.ip_udp_endec.IpUdpEndec
    * IrisBackendv3.transceiver.endecs.slip_endec.SlipEndec
    * IrisBackendv3.transceiver.endecs.unity_endec.UnityEndec

    ### Methods

    `decode(self, data: bytes) ‑> bytes`
    :

    `encode(self, data: bytes) ‑> bytes`
    :