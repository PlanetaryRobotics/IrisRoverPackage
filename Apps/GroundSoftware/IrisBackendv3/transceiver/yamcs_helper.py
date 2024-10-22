"""
Helper for interfacing with data from YAMCS.

Specifically, for inter-operating between Iris' data formatting and YAMCS's
formatting.

@author: Connor W. Colombo (CMU)
@last-updated: 01/05/2024
"""
from typing import Any, Final, Dict, List, cast

from datetime import datetime, timezone
import IrisBackendv3 as IB3
from IrisBackendv3.data_standards.module import Module
from IrisBackendv3.codec.packet import parse_packet
from IrisBackendv3.codec.packet_classes.packet import Packet
from IrisBackendv3.codec.packet_classes.peregrine_dummy_packet import PeregrineDummyPacket
from IrisBackendv3.codec.payload import TelemetryPayload, DownlinkedPayload
from IrisBackendv3.codec.payload_collection import EnhancedPayloadCollection
from IrisBackendv3.codec.metadata import DataSource, DataPathway, DownlinkTimes
from IrisBackendv3.codec.fsw_data_codec import FswDataType
from IrisBackendv3.transceiver.logs import logger

from IrisBackendv3.data_standards.data_standards import standardize_constant_name

import yamcs.protobuf.yamcs_pb2 as yamcs_pb2  # type: ignore
from yamcs.tmtc.model import ParameterValue  # type: ignore


def get_peregrine_module() -> Module:
    """Gets the DataStandards Module for Peregrine telemetry from the live
    datastandards (which is why this can't just be evaluated as a constant).
    """
    return IB3.get_codec_standards().modules['Peregrine']


# Map of all params Lander PMCC can access to their GDS channel names:
LANDER_PARAMS_YAMCS_GDS_MAP: Final[Dict[str, str]] = {
    '/Peregrine/PL1/LSS1_Derived/LSS1_HK_Derived/Iris_Operational_EnabledFet': 'IrisOperationalEnabledFet',
    '/Peregrine/PL1/LSS1_Derived/LSS1_HK_Derived/Iris_Release_EnabledFet': 'IrisReleaseEnabledFet',
    '/Peregrine/PL1/LSS1_Derived/LSS1_HK_Derived/Iris_Operational_avgCurrent': 'IrisOperationalAvgCurrent',
    '/Peregrine/PL1/LSS1_Derived/LSS1_HK_Derived/Iris_Release_avgCurrent': 'IrisReleaseAvgCurrent',
    '/Peregrine/PL1/LSS1_Derived/LSS1_HK_Derived/Iris_Operational_maxCurrent': 'IrisOperationalMaxCurrent',
    '/Peregrine/PL1/LSS1_Derived/LSS1_HK_Derived/Iris_Release_maxCurrent': 'IrisReleaseMaxCurrent',
    '/Peregrine/PL1/SLIPsvc@3Status': 'SlipServiceStatus',
    '/Peregrine/PL1/WIFIsvc@3Status': 'WifiServiceStatus',
    # deprecated (not flown)
    '/Peregrine/FCPU/TMGR/TCS_DECKD_1/TCS_DECKD_1': 'DeckD1TempCelsius',
    '/Peregrine/FCPU/TMGR/TCS_DECKD_2/TCS_DECKD_2': 'DeckD2TempCelsius',
    # deprecated (not flown)
    '/Peregrine/FCPU/TMGR/TCS_DECKD_3/TCS_DECKD_3': 'DeckD3TempCelsius',
    '/Peregrine/MO/missionPhase': 'PeregrineMissionPhase'
}
# Parameter(s) that contain Iris packets:
IRIS_TELEM_PARAMS: Final[List[str]] = [
    '/Peregrine/payloads/iris/iris-payload-tm-rs422',
    '/Peregrine/payloads/iris/iris-payload-tm-wlan'
]
# All params Iris PMCC has access to:
ALL_YAMCS_PARAMS: Final[List[str]] = [
    *LANDER_PARAMS_YAMCS_GDS_MAP.keys(),
    *IRIS_TELEM_PARAMS
]
# Parameter that indicates when Iris has power:
IRIS_OPERATIONAL_POWER_PARAM: Final[str] = \
    '/Peregrine/PL1/LSS1_Derived/LSS1_HK_Derived/Iris_Operational_EnabledFet'


def iris_telem_param_to_packet(param: ParameterValue) -> Packet:
    """Converts the given YAMCS param of Iris Telemetry into the Iris Packet it
    represents."""
    # Build packet:
    packet = parse_packet(param.eng_value)
    # Add metadata:
    packet.source = DataSource.YAMCS
    packet.pathway = {
        '/Peregrine/payloads/iris/iris-payload-tm-rs422': DataPathway.WIRED,
        '/Peregrine/payloads/iris/iris-payload-tm-wlan': DataPathway.WIRELESS
    }[param.name]
    # Add metadata to all the payloads in Packet:
    for payload in packet.payloads.all_payloads:
        if isinstance(payload, DownlinkedPayload):
            # Add Downlink metadata:
            if payload.downlink_times is None:
                payload.downlink_times = DownlinkTimes()
            payload.downlink_times.pmcc_rx = datetime.now(timezone.utc)
            payload.downlink_times.amcc_rx = param.reception_time
            payload.downlink_times.lander_rx = param.generation_time

    return packet


def peregrine_telem_params_to_packet(
    params: List[ParameterValue]
) -> PeregrineDummyPacket:
    """Extract relevant data and metadata from the given YAMCS parameters of
    Peregrine telemetry and wraps the extracted data in a PeregrineDummyPacket.
    """
    PEREGRINE_MODULE = get_peregrine_module()
    # Build each payload:
    telem_payloads = []
    for param in params:
        if param.name not in [*LANDER_PARAMS_YAMCS_GDS_MAP.keys()]:
            logger.warning(
                f"{param.name=} is not a known Peregrine telem name in: "
                f"{LANDER_PARAMS_YAMCS_GDS_MAP=}"
            )
            continue
        # Look up appropriate DataStandards entry and use it:
        channel_name = LANDER_PARAMS_YAMCS_GDS_MAP[param.name]
        channel = PEREGRINE_MODULE.telemetry[channel_name]
        val = param.eng_value
        if (
            isinstance(val, str)
            and channel.datatype == FswDataType.ENUM
        ):
            # Enum string from Peregrine. Format the string to match our enum
            # formatting standards:
            val = standardize_constant_name(val)

        telem = TelemetryPayload(
            module_id=PEREGRINE_MODULE.ID,
            channel_id=channel.ID,
            data=val,
            timestamp=0,  # U32 ms timestamp is not applicable here
            source=DataSource.YAMCS,
            pathway=DataPathway.NONE
        )
        # Add metadata to telem:
        if telem.downlink_times is None:
            telem.downlink_times = DownlinkTimes()
        telem.downlink_times = DownlinkTimes(
            scet_est=param.generation_time,
            lander_rx=param.generation_time,
            amcc_rx=param.reception_time,
            pmcc_rx=datetime.now(timezone.utc)
        )
        telem_payloads.append(telem)

    # Build Packet and add metadata:
    payloads = EnhancedPayloadCollection(TelemetryPayload=telem_payloads)
    packet = PeregrineDummyPacket(payloads=payloads)
    packet.source = DataSource.YAMCS
    packet.pathway = DataPathway.NONE
    return packet
