Module IrisBackendv3.transceiver.prebuilts
==========================================
Registry of pre-"built" (pre-specified) Transceivers that can be built given
just a name (useful for referring to a particular Transceiver build by name in
a CLI argument).

@author: Connor W. Colombo (CMU)
@last-updated: 06/01/2023

Functions
---------

    
`build_xcvr(xcvr_spec: IrisBackendv3.transceiver.prebuilts.PrebuiltTransceiver, **kwargs) ‑> IrisBackendv3.transceiver.transceiver.Transceiver | None`
:   

    
`build_xcvr_by_name(name: str, **kwargs) ‑> IrisBackendv3.transceiver.transceiver.Transceiver`
:   Builds a pre-specified Transceiver instance based on the name given.
    Optionally, `kwargs` can be given to override certain default settings of
    the prebuilt transceiver.

Classes
-------

`PrebuiltTransceiver(value, names=None, *, module=None, qualname=None, type=None, start=1)`
:   An enumeration.

    ### Ancestors (in MRO)

    * enum.Enum

    ### Class variables

    `IMG_GRID`
    :

    `PCAP_18H`
    :

    `PCAP_GENERIC`
    :

    `PCAP__RC9_5_3__2MIN_IMU`
    :

    `WIFI`
    :