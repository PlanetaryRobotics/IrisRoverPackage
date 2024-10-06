Module IrisBackendv3.storage.telemetry
======================================
Methods for handling & archiving telemetry (and events).
Processing of downlinked data.

@author: Colombo, CMU
@last-updated: 10/06/2024

Functions
---------

    
`extract_iris_packets(yamcs_dataset: IrisBackendv3.storage.dataset.DataSet, rx_is_generation_time: bool = True) ‑> Tuple[List[datetime.datetime], List[IrisBackendv3.codec.packet_classes.packet.Packet]]`
:   Extracts all the Iris packets from the DataSet.
    
    NOTE: These packets are likely but NOT necessarily chronological.
    
    Returns a tuple of timestamps for all packets and the corresponding packets.

    
`extract_packets(dataset: IrisBackendv3.storage.dataset.DataSet, packet_cols: List[str], rx_is_generation_time: bool = False) ‑> Tuple[List[datetime.datetime], List[IrisBackendv3.codec.packet_classes.packet.Packet]]`
:   Extracts packets that are contained in the columns `packet_cols` of the
    given dataset. Returns a tuple containing a time stamp list and a
    corresponding Packet list.

    
`generate_all_packets(yamcs_dataset: IrisBackendv3.storage.dataset.DataSet, rx_is_generation_time: bool = True) ‑> Tuple[List[datetime.datetime], List[IrisBackendv3.codec.packet_classes.packet.Packet]]`
:   Processes all raw Iris bytes and Peregrine data from YAMCS to generate
    all Iris packets and PeregrineDummyPackets for each time stamp.
    
    NOTE: This is NOT sorted by time. All iris packets will appear, likely but
    not necessarily chronologically, FOLLOWED BY all Peregrine packets, likely
    but not necessarily chronologically.
    
    Returns a tuple of timestamps for all packets and the corresponding packets.

    
`generate_peregrine_packets(yamcs_dataset: IrisBackendv3.storage.dataset.DataSet, rx_is_generation_time: bool = True) ‑> Tuple[List[datetime.datetime], List[IrisBackendv3.codec.packet_classes.packet.Packet]]`
:   Processes all Peregrine data in the given YAMCS DataSet to generate a
    vector of PeregrineDummyPackets for each time stamp.
    
    NOTE: These packets are likely but NOT necessarily chronological.
    
    Returns a tuple of timestamps for all packets and the corresponding packets.

    
`iris_packets_to_telem(yamcs_dataset: IrisBackendv3.storage.dataset.DataSet, rx_is_generation_time: bool = True) ‑> IrisBackendv3.storage.dataset.DataSet`
:   Processes all raw Iris bytes in the given YAMCS DataSet to generate a
    DataSet of all telemetry that would be generated from each packet,
    including metafields.
    All data is sorted chronologically.

    
`packet_to_telem_rows(time: datetime.datetime, packet: IrisBackendv3.codec.packet_classes.packet.Packet, time_resolution: str = 'milliseconds') ‑> pandas.core.frame.DataFrame`
:   Extracts telemetry (and events) from the given timestamped packet and
    creates a DataFrame representing one row of "telemetry" for the telemetry
    archive (in quotes here because non-telem items are also contained).
    NOTE: Each packet contains telem for multiple SCET times downlinked
    together, so multiple rows will be generated.
    
    Two payloads are considered to have happened simultaneously if they are
    within the same `time_resolution`. Typ. this should be `ms`.

    
`packets_to_full_telem(times: List[datetime.datetime], packets: List[IrisBackendv3.codec.packet_classes.packet.Packet]) ‑> IrisBackendv3.storage.dataset.DataSet`
:   Creates a standardized DataSet of telemetry (and events) from a vector
    of packet timestamps and corresponding vector of timestamps.
    Telemetry (and events) is extracted from each packet and any GDS-generated
    meta-telemetry and meta-events is generated and also added.

    
`peregrine_data_to_telem(yamcs_dataset: IrisBackendv3.storage.dataset.DataSet, rx_is_generation_time: bool = True) ‑> IrisBackendv3.storage.dataset.DataSet`
:   Processes all Peregrine data in the given YAMCS DataSet to create a
    standardized DataSet of all Peregrine telemetry (adhering to DataStandards)
    that would have been generated, including metafields.
    All data is sorted chronologically.

    
`yamcs_dataset_to_telem(yamcs_dataset: IrisBackendv3.storage.dataset.DataSet, rx_is_generation_time: bool = True) ‑> IrisBackendv3.storage.dataset.DataSet`
:   Produces a DataSet of all telemetry (and events) that would have been
    produced in the GDS from the given YAMCS input, including meta-telemetry
    (and meta-events) from both Iris data and Peregrine data.
    All data is sorted chronologically.