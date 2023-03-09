Module IrisBackendv3.transceiver.endecs.ip_udp_endec
====================================================

Classes
-------

`IpUdpEndec(ip_dest: str = '127.0.0.2', ip_src: str = '222.173.190.239', port: int = 8080)`
:   Encoder applies a UDP header and then an IP header, decoder does reverse.

    ### Ancestors (in MRO)

    * IrisBackendv3.transceiver.endecs.endec.Endec
    * abc.ABC

    ### Class variables

    `ip_dest: str`
    :

    `ip_src: str`
    :

    `port: int`
    :

    ### Methods

    `decode(self, data: bytes) ‑> bytes`
    :   Decodes data sent over a `Transceiver`.

    `encode(self, data: bytes) ‑> bytes`
    :   Encodes data to send over a `Transceiver`.