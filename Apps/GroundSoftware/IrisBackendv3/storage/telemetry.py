"""
Methods for handling & archiving telemetry (and events).
Processing of downlinked data.

@author: Colombo, CMU
@last-updated: 10/06/2024
"""
from typing import Any, Final, Tuple, List, Dict, cast

from IrisBackendv3.codec.packet import Packet, UnsupportedPacket
from IrisBackendv3.codec.payload import Payload, DownlinkedPayload, TelemetryPayload, EventPayload

from IrisBackendv3.data_standards.fsw_data_type import (
    FswDataType,
    Category as FswDataCategory
)
from ipc_apps.dl_processor import process_dl_payloads

from datetime import datetime, timedelta, timezone

import numpy as np
import pandas as pd  # type: ignore
from rich.progress import track

from IrisBackendv3.storage.dataset import DataSet
from IrisBackendv3.storage.formatting import (
    str_to_bytes_if_bytes, bytes_to_str, payload_to_storage_format, format_time,
    remove_ansi_escape_codes, reformat_event_str
)
from IrisBackendv3.storage.settings import settings
from IrisBackendv3.storage.logs import logger

from IrisBackendv3.transceiver.yamcs_helper import (
    IRIS_TELEM_PARAMS, LANDER_PARAMS_YAMCS_GDS_MAP,
    iris_telem_param_to_packet, peregrine_telem_params_to_packet,
    build_mock_yamcs_parameter
)

# Suppress inaccurate fragmentation error (per pandas github issue):
from warnings import simplefilter
simplefilter(action="ignore", category=pd.errors.PerformanceWarning)


def extract_packets(
    dataset: DataSet,
    packet_cols: List[str],
    rx_is_generation_time: bool = False
) -> Tuple[List[datetime], List[Packet]]:
    """
    Extracts packets that are contained in the columns `packet_cols` of the
    given dataset. Returns a tuple containing a time stamp list and a
    corresponding Packet list.
    """
    times: List[datetime] = []
    packets: List[Packet] = []
    # For every packet column that's actually present in the dataset:
    for col in packet_cols:
        if col in dataset.data:
            # Grab all non-NaN data:
            valid = dataset.data[col][~dataset.data[col].isna()]
            new_times: List[datetime] = [t.to_pydatetime()
                                         for t in valid.index]
            times.extend(new_times)
            packets.extend(
                iris_telem_param_to_packet(
                    build_mock_yamcs_parameter(
                        name=col,
                        reception_time=(
                            t if rx_is_generation_time
                            else datetime.now(timezone.utc)
                        ),
                        generation_time=t,
                        eng_value=str_to_bytes_if_bytes(pb)
                    ), rx_is_generation_time
                )
                for t, pb in track(
                    zip(new_times, valid.tolist()),
                    "[blue]Recovering & Parsing Iris Packets",
                    total=len(new_times)
                )
            )
    return times, packets


def packet_to_telem_rows(
    time: datetime,
    packet: Packet,
    time_resolution: str = 'milliseconds'
) -> pd.DataFrame:
    """Extracts telemetry (and events) from the given timestamped packet and
    creates a DataFrame representing one row of "telemetry" for the telemetry
    archive (in quotes here because non-telem items are also contained).
    NOTE: Each packet contains telem for multiple SCET times downlinked
    together, so multiple rows will be generated.

    Two payloads are considered to have happened simultaneously if they are
    within the same `time_resolution`. Typ. this should be `ms`.
    """
    # Start out with just some packet metadata at the timestamp of the metadata:
    # (prefix with underscore to mirror module naming so these can be sorted as
    # a module):
    df = pd.DataFrame(
        data=[{
            '*Packet_type': packet.__class__.__name__,
            '*Packet_str': reformat_event_str(str(packet)),
            '*Packet_valid': not (isinstance(packet, UnsupportedPacket) or packet is None),
            '*Packet_source': packet.source.name if packet.source is not None else np.nan,
            '*Packet_pathway': packet.pathway.name if packet.pathway is not None else np.nan,
            '*Packet_size': len(packet._raw) if packet._raw is not None else np.nan,
            '*Packet_bytes': bytes_to_str(packet.raw)
        }],
        index=format_time([time])
    )
    df.index.name = settings['TIME_COL_NAME']
    # Collect target dtypes for columns as the table is built:
    df_dtypes: Dict[str, type] = {
        '*Packet_type': str,
        '*Packet_str': str,
        '*Packet_valid': bool,
        '*Packet_source': str,
        '*Packet_pathway': str,
        '*Packet_size': float,  # can't have nan in int column
        '*Packet_bytes': str
    }

    # Act as DownlinkProcessor and add in all Metafields that would have been
    # generated live:
    payloads_and_metapayloads = process_dl_payloads(packet.payloads)

    def _store_payload(p: DownlinkedPayload, n: str) -> None:
        """Internal helper for building dataframe."""
        dtype, val = payload_to_storage_format(p)
        if p.downlink_times is None or p.downlink_times.scet_est is None:
            t = time
        else:
            t = p.downlink_times.scet_est
        # Truncate time to neatest time resolution and apply:
        df.loc[format_time(t.isoformat(timespec=time_resolution)), n] = val
        # Store type:
        if n in df_dtypes and df_dtypes[n] != dtype:
            # Types don't agree with previous entries (somehow), so we have to
            # go with `object` to hold multiple types:
            df_dtypes[n] = object
        df_dtypes[n] = dtype

    # Add all TelemetryPayloads:
    for p in payloads_and_metapayloads[TelemetryPayload]:
        p = cast(TelemetryPayload, p)
        n = f"{p.module.name}_{p.channel.name}"
        _store_payload(p, n)

    # Add all EventPayloads as strings:
    for p in payloads_and_metapayloads[EventPayload]:
        p = cast(EventPayload, p)
        n = f"{p.module.name}_{p.event.name}"
        _store_payload(p, n)

    # Convert all columns to mono-types based on FSW datatype:
    # Don't cast for any columns that need to go to int or bool as those
    # columns can't contain nans. Leave them as they are (likely mixed type):
    # A solution here would be to use pd.NA and extension types but HDFStore
    # doesn't support extension types (as of 09/2024).
    for key in [*df_dtypes.keys()]:
        if df_dtypes[key] in [int, bool]:
            df_dtypes.pop(key)
    df = df.astype(df_dtypes)

    return df


def extract_iris_packets(
    yamcs_dataset: DataSet,
    rx_is_generation_time: bool = True
) -> Tuple[List[datetime], List[Packet]]:
    """Extracts all the Iris packets from the DataSet.

    NOTE: These packets are likely but NOT necessarily chronological.

    Returns a tuple of timestamps for all packets and the corresponding packets.
    """
    return extract_packets(
        yamcs_dataset, IRIS_TELEM_PARAMS,
        rx_is_generation_time=rx_is_generation_time
    )


def _telem_row_aggregator(col: pd.Series) -> Any:
    """Aggregation function helper to combine rows with the same index.
    NOTE: This gets fed with a column vector of all entries that have the same
    row index for a given idx,col.
    """
    col = col.dropna()
    if col.size == 0:
        return np.nan
    # Try to take mean if numeric:
    if pd.api.types.is_numeric_dtype(col.dtypes):
        try:
            return col.mean()
        except TypeError:
            # Couldn't take mean, so just proceed as if this weren't numeric:
            # (likely there's some edge case of a non-average-able "numeric"
            # type in pandas)
            pass
    # Take final value:
    col = col.sort_index(ascending=True, inplace=False)
    return col[-1]


def packets_to_full_telem(times: List[datetime], packets: List[Packet]) -> DataSet:
    """Creates a standardized DataSet of telemetry (and events) from a vector
    of packet timestamps and corresponding vector of timestamps.
    Telemetry (and events) is extracted from each packet and any GDS-generated
    meta-telemetry and meta-events is generated and also added.
    """
    # Make sure packets (and times) are sorted by time (so metafields are
    # generated correctly, esp. those that depend on both Iris and Peregrine
    # telem will be generated correctly):
    logger.debug(f"Sorting all {len(packets)} packets . . . ")
    times, packets = zip(*sorted(zip(times, packets), key=lambda x: x[0]))

    logger.debug(f"Creating telem rows from {len(packets)} packets . . . ")
    # Extract all telem from each packet and add in any generated metafields:
    telem_rows = [
        packet_to_telem_rows(t, p)
        for t, p in track(
            zip(times, packets), "[blue]Packets -> Telemetry", total=len(times)
        )
    ]

    logger.debug("Joining telem rows . . . ")
    data = pd.concat([*telem_rows], axis=0, ignore_index=False)
    # Aggregate by time, joining multiple rows with the same timestamp:
    data.index.name = settings['TIME_COL_NAME']
    logger.debug(f"Aggregating rows with common time in {data.shape} df ... ")
    data = data.groupby(settings['TIME_COL_NAME']).agg(_telem_row_aggregator)

    logger.debug(f"Building dataset from {data.shape} df . . . ")
    if len(telem_rows) > 0:
        return DataSet(data)
    else:
        return DataSet(pd.DataFrame())


def iris_packets_to_telem(
    yamcs_dataset: DataSet,
    rx_is_generation_time: bool = True
) -> DataSet:
    """Processes all raw Iris bytes in the given YAMCS DataSet to generate a
    DataSet of all telemetry that would be generated from each packet,
    including metafields.
    All data is sorted chronologically.
    """
    times, packets = extract_iris_packets(yamcs_dataset, rx_is_generation_time)
    return packets_to_full_telem(times, packets)


def generate_peregrine_packets(
    yamcs_dataset: DataSet,
    rx_is_generation_time: bool = True
) -> Tuple[List[datetime], List[Packet]]:
    """Processes all Peregrine data in the given YAMCS DataSet to generate a
    vector of PeregrineDummyPackets for each time stamp.

    NOTE: These packets are likely but NOT necessarily chronological.

    Returns a tuple of timestamps for all packets and the corresponding packets.
    """
    # Drop all columns that aren't lander params:
    peregrine_data = yamcs_dataset.data.drop(
        columns=list(
            set(yamcs_dataset.data.columns)
            - set([*LANDER_PARAMS_YAMCS_GDS_MAP.keys()])
        ),
        inplace=False
    )

    # For each timestamp, lump all non-NaN data into one Peregrine packet:
    times: List[datetime] = []
    packets: List[Packet] = []
    for idx, row in track(
        peregrine_data.iterrows(), "[blue]Building Peregrine Packets",
        total=peregrine_data.shape[0]
    ):
        t = idx.to_pydatetime()
        # Exclude any empty data in this entry:
        row_data = row.dropna()
        # Create Mock Parameters for each remaining column:
        if row_data.size > 0:
            peregrine_params = [
                build_mock_yamcs_parameter(
                    name=col,
                    reception_time=(
                        t if rx_is_generation_time
                        else datetime.now(timezone.utc)
                    ),
                    generation_time=t,
                    eng_value=str_to_bytes_if_bytes(val)
                )
                for col, val in row_data.items()
            ]
            peregrine_packet = peregrine_telem_params_to_packet(
                peregrine_params,
                rx_is_generation_time
            )
            times.append(t)
            packets.append(peregrine_packet)
    return times, packets


def peregrine_data_to_telem(
    yamcs_dataset: DataSet,
    rx_is_generation_time: bool = True
) -> DataSet:
    """Processes all Peregrine data in the given YAMCS DataSet to create a
    standardized DataSet of all Peregrine telemetry (adhering to DataStandards)
    that would have been generated, including metafields.
    All data is sorted chronologically.
    """
    times, packets = generate_peregrine_packets(
        yamcs_dataset,
        rx_is_generation_time
    )
    return packets_to_full_telem(times, packets)


def generate_all_packets(
    yamcs_dataset: DataSet,
    rx_is_generation_time: bool = True
) -> Tuple[List[datetime], List[Packet]]:
    """Processes all raw Iris bytes and Peregrine data from YAMCS to generate
    all Iris packets and PeregrineDummyPackets for each time stamp.

    NOTE: This is NOT sorted by time. All iris packets will appear, likely but
    not necessarily chronologically, FOLLOWED BY all Peregrine packets, likely
    but not necessarily chronologically.

    Returns a tuple of timestamps for all packets and the corresponding packets.
    """
    logger.verbose("Generating & Parsing Iris Packets . . . ")
    iris_times, iris_packets = extract_iris_packets(
        yamcs_dataset, rx_is_generation_time
    )
    logger.verbose("Generating Peregrine Packets . . . ")
    peregrine_times, peregrine_packets = generate_peregrine_packets(
        yamcs_dataset, rx_is_generation_time
    )
    times = [*iris_times, *peregrine_times]
    packets = [*iris_packets, *peregrine_packets]
    return times, packets


def yamcs_dataset_to_telem(
    yamcs_dataset: DataSet,
    rx_is_generation_time: bool = True
) -> DataSet:
    """Produces a DataSet of all telemetry (and events) that would have been
    produced in the GDS from the given YAMCS input, including meta-telemetry
    (and meta-events) from both Iris data and Peregrine data.
    All data is sorted chronologically.
    """
    logger.info("Generating Packets from YAMCS Archive . . . ")
    times, packets = generate_all_packets(
        yamcs_dataset, rx_is_generation_time
    )
    logger.info("Converting Packets to Telem . . . ")
    return packets_to_full_telem(times, packets)
