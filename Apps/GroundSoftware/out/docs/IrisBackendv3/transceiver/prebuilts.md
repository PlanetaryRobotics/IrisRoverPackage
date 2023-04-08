Module IrisBackendv3.transceiver.prebuilts
==========================================
Registry of pre-"built" (pre-specified) Transceivers that can be built given
just a name (useful for referring to a particular Transceiver build by name in
a CLI argument).

Functions
---------

    
`build_xcvr_by_name(name: str, **kwargs) ‑> IrisBackendv3.transceiver.transceiver.Transceiver`
:   Builds a pre-specified Transceiver instance based on the name given.
    Optionally, `kwargs` can be given to override certain default settings of
    the prebuilt transceiver.