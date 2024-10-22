Module IrisBackendv3.transceiver.xcvr_enum
==========================================
Simple enumeration / registry of all available Transceiver classes (allows
specifying which transceiver should handle something).

@author: Connor W. Colombo (CMU)
@last-updated: 03/06/2023

Classes
-------

`TransceiverEnum(value, names=None, *, module=None, qualname=None, type=None, start=1)`
:   Enum for defines all ports used in the IPC.
    
    This enum should serve as the single source of truth for the topic 
    information.

    ### Ancestors (in MRO)

    * enum.Enum

    ### Class variables

    `ALL`
    :

    `PCAP`
    :

    `SERIAL_SLIP`
    :

    `WIFI`
    :

    `YAMCS`
    :

    `transceiver_class: Optional[Type[IrisBackendv3.transceiver.transceiver.Transceiver]]`
    :

    ### Methods

    `matches(self, xcvr: IrisBackendv3.transceiver.transceiver.Transceiver) ‑> bool`
    :   Checks if the given transceiver matches the one specified by enum
        instance.