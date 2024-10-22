Module IrisBackendv3.transceiver.transceiver
============================================
Abstract Base Class encapsulating common behaviors and interfaces for all
transceiver classes.

TODO: Handle setting `rover_ack` for `UplinkedPayload`s.

TODO: Handle auto-updating of seq-num (maybe this needs to work across
Transceivers?)

TODO: Add pcap-save function (writes every outbound bytes_packet to a pcap).

@author: Connor W. Colombo (CMU)
@last-updated: 03/03/2023

Classes
-------

`Transceiver(name: str, endecs: Optional[List[Endec]] = None, pathway: DataPathway = DataPathway.NONE, source: DataSource = DataSource.NONE, log_on_send: bool = True, log_on_receive: bool = True, **_)`
:   Abstract base class for all `Transceivers`. This class handles all
    logic interfacing with `Packet` classes and `Endec`s so the implementation
    of the subclasses can be as simple as possible (just handling how to get
    and send packet bytes.)
    
    Creates a `Transceiver` over `pathway` (wired/wireless) connecting to
    `source` (WIFI, Serial, YAMCS, PCAP, etc.).
    
    Data is en/decoded using `endecs`, a list of `Endec` instances in the
    order of **DECODING** (where the left-most endec is stripped off first
    when decoding and applied last when encoding.)

    ### Ancestors (in MRO)

    * abc.ABC

    ### Descendants

    * IrisBackendv3.transceiver.archive_transceiver.ArchiveTransceiver
    * IrisBackendv3.transceiver.pcap_transceiver.PcapTransceiver
    * IrisBackendv3.transceiver.slip_transceiver.SlipTransceiver
    * IrisBackendv3.transceiver.wifi_transceiver.WifiTransceiver
    * IrisBackendv3.transceiver.yamcs_transceiver.YamcsTransceiver

    ### Class variables

    `data_pathway: IrisBackendv3.codec.metadata.DataPathway`
    :

    `data_source: IrisBackendv3.codec.metadata.DataSource`
    :

    `endecs: List[IrisBackendv3.transceiver.endecs.endec.Endec]`
    :

    `log_on_receive: bool`
    :

    `log_on_send: bool`
    :

    `name: str`
    :

    `seq_num: int`
    :

    ### Methods

    `async_read(self) ‑> List[IrisBackendv3.codec.packet_classes.packet.Packet]`
    :   Asynchronously awaits the next available packet(s) on the transceiver input.

    `begin(self) ‑> None`
    :   Initialize any special registers, etc. for this transceiver.

    `endecs_decode(self, data: bytes) ‑> bytes`
    :   Decodes the given data using all the `endecs`.

    `endecs_encode(self, data: bytes) ‑> bytes`
    :   Encodes the given data using all the `endecs`.

    `read(self) ‑> List[IrisBackendv3.codec.packet_classes.packet.Packet]`
    :   Reads all available packets on the transceiver input.

    `send(self, packet: Packet, **uplink_metadata) ‑> bool`
    :   Sends the given packet over this `Transceiver`'s transmission line,
        encoding it as necessary.
        
        `**uplink_metadata` contains any special data needed by methods further
        down the uplink pipeline, particularly `_uplink_byte_packet`.
        
        Returns whether the send was successful.

    `send_payloads(self, payloads: EnhancedPayloadCollection, **uplink_metadata) ‑> List[IrisBackendv3.codec.packet_classes.packet.Packet]`
    :   Sends the given payloads in as few packets as possible.
        
        `**uplink_metadata` contains any special data needed by methods further
        down the uplink pipeline, particularly `_uplink_byte_packet`.
        
        TODO: Mind the MTUs and actually handle splitting up the `Payload`s
        into multiple `Packet`s as necessary.
        
        Returns: A list of all `Packet`s transmitted.