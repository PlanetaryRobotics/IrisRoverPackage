Module IrisBackendv3.transceiver.yamcs_transceiver
==================================================
Defines the Transceiver class for reading telemetry and sending commands over
a YAMCS. This is the only `Transceiver` used during mission to talk with the
flight rover.

Includes any supporting functions necessary for maintaining YAMCS connection.

TODO: Consult script used for iTVAC...

@author: Connor W. Colombo (CMU)
@last-updated: 07/03/2022

Classes
-------

`YamcsTransceiver(host: str, port: int, endecs: Optional[List[IrisBackendv3.transceiver.endecs.endec.Endec]] = None, pathway: IrisBackendv3.codec.metadata.DataPathway = DataPathway.NONE, source: IrisBackendv3.codec.metadata.DataSource = DataSource.YAMCS)`
:   Abstract base class for all `Transceivers`. This class handles all
    logic interfacing with `Packet` classes and `Endec`s so the implementation
    of the subclasses can be as simple as possible (just handling how to get
    and send packet bytes.)
    
    Initializes a `YamcsTransceiver`.
    
    NOTE: By default, no `endecs` should be required.

    ### Ancestors (in MRO)

    * IrisBackendv3.transceiver.transceiver.Transceiver
    * abc.ABC

    ### Class variables

    `YAMCS_INSTANCE: ClassVar[str]`
    :

    `YAMCS_PROCESSOR: ClassVar[str]`
    :