"""
Helper for interfacing with data from YAMCS.

Specifically, for inter-operating between Iris' data formatting and YAMCS's
formatting.

@author: Connor W. Colombo (CMU)
@last-updated: 09/30/2024
"""
from typing import Any, Final, Dict, List, cast, Optional, Tuple

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
from yamcs.protobuf.pvalue import pvalue_pb2  # type: ignore
from yamcs.tmtc.model import ParameterValue  # type: ignore

from yamcs.client import YamcsClient, MDBClient, ArchiveClient  # type: ignore
from yamcs.tmtc.client import ProcessorClient  # type: ignore
from yamcs.tmtc.model import ParameterValue  # type: ignore
from yamcs.core.auth import Credentials as YamcsCredentials  # type: ignore


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


def iris_telem_param_to_packet(
    param: ParameterValue,
    rx_is_generation_time: bool = False
) -> Packet:
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
            # If we're replaying this, we can no longer create a good reception
            # time estimate, so just go with generation time:
            payload.downlink_times.amcc_rx = param.reception_time
            if rx_is_generation_time:
                payload.downlink_times.pmcc_rx = param.generation_time
            else:
                payload.downlink_times.pmcc_rx = datetime.now(timezone.utc)
            payload.downlink_times.lander_rx = param.generation_time

    return packet


def peregrine_telem_params_to_packet(
    params: List[ParameterValue],
    rx_is_generation_time: bool = False
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
            pmcc_rx=(
                param.generation_time if rx_is_generation_time
                else datetime.now(timezone.utc)
            )
        )
        telem_payloads.append(telem)

    # Build Packet and add metadata:
    payloads = EnhancedPayloadCollection(TelemetryPayload=telem_payloads)
    packet = PeregrineDummyPacket(payloads=payloads)
    packet.source = DataSource.YAMCS
    packet.pathway = DataPathway.NONE
    return packet


def _val_to_proto_val(val: Any) -> yamcs_pb2.Value:
    """Wraps the given value in the appropriate protobuf Value expected by
    YAMCS. As of 1.7.5, yamcs just echos back whatever value we send it, so
    this whole typing is a charade atm but we adhere to it here in case that
    changes."""
    proto_val = yamcs_pb2.Value()
    if isinstance(val, int):
        proto_val.type = yamcs_pb2.Value.SINT64
        proto_val.sint64Value = val
    elif isinstance(val, float):
        proto_val.type = yamcs_pb2.Value.DOUBLE
        proto_val.doubleValue = val
    elif isinstance(val, bytes):
        proto_val.type = yamcs_pb2.Value.BINARY
        proto_val.binaryValue = val
    else:
        # Just attempt to pass it through as a string.
        # Even if it's not, the value just gets returned
        # to us.
        proto_val.type = yamcs_pb2.Value.STRING
        proto_val.stringValue = val
    return proto_val


def build_mock_yamcs_parameter(
    name: str,
    reception_time: datetime,
    generation_time: datetime,
    eng_value: Any,
    raw_value: Any | None = None
) -> ParameterValue:
    """Builds a mock YAMCS parameter with only the information we care about
    (for passing around internally to things that ingest YAMCS parameters.)"""
    proto_id = yamcs_pb2.NamedObjectId()
    proto_id.name = name
    proto_param = pvalue_pb2.ParameterValue()

    proto_param.generationTime.FromDatetime(generation_time)
    proto_param.acquisitionTime.FromDatetime(reception_time)

    proto_param.engValue.CopyFrom(_val_to_proto_val(eng_value))
    if raw_value is not None:
        proto_param.rawValue.CopyFrom(_val_to_proto_val(raw_value))

    return ParameterValue(proto=proto_param, id=proto_id)


class YamcsInterface:
    """Wrapper for common YAMCS interface tasks."""
    # Private read-only:
    _server: Final[str]
    _port: Final[int]
    _instance: Final[str]
    _processor_name: Final[str]
    _username: Final[Optional[str]]
    _password: Final[Optional[str]]
    _tls: Final[bool]

    # Public:
    client: Optional[YamcsClient]
    mdb: Optional[MDBClient]
    archive: Optional[ArchiveClient]
    processor: Optional[ProcessorClient]

    def __init__(
        self,
        server: str,
        port: int,
        instance: str,
        processor: str,
        username: Optional[str] = None,
        password: Optional[str] = None,
        tls: bool = True,
        auto_begin: bool = True,
        **_  # allow (and ignore) extraneous kwargs - from opts.
    ) -> None:
        self._server = server
        self._port = port
        self._instance = instance
        self._processor_name = processor
        self._username = username
        self._password = password
        self._tls = tls

        self.client = None
        self.mdb = None
        self.processor = None
        self.archive = None

        if auto_begin:
            self.begin()

    def begin(self) -> None:
        self._obtain_client()
        self._fetch_mdb()
        self._obtain_archive_client()
        self._get_processor()

    def _obtain_client(self) -> None:
        if self.client is None:
            credentials: Optional[Any] = None
            if self._username is not None or self._password is not None:
                credentials = YamcsCredentials(
                    username=self._username,
                    password=self._password
                )

            self.client = YamcsClient(
                f"{self._server}:{self._port}",
                credentials=credentials,
                tls=(self._tls),
                tls_verify=(not self._tls)
            )

    def _fetch_mdb(self) -> None:
        """Fetch the MDB (Mission Database - YAMCS version of DataStandards)"""
        if self.mdb is None and self.client is not None:
            self.mdb = self.client.get_mdb(instance=self._instance)

    def _obtain_archive_client(self) -> None:
        """Obtain a client to access the data archive."""
        if self.archive is None and self.client is not None:
            self.archive = self.client.get_archive(self._instance)

    def _get_processor(self) -> None:
        """Gets a hook to the specified processor from the client."""
        if self.processor is None and self.client is not None:
            self.processor = self.client.get_processor(
                instance=self._instance,
                processor=self._processor_name
            )

    def get_param_names(self) -> List[str]:
        """Helper function that returns a list of the qualified names of all
        parameters available in the MDB."""
        names: List[str] = []
        if self.mdb is not None:
            params = [p for p in self.mdb.list_parameters()]
            names = [p.qualified_name for p in params]
        return names

    def get_param_history(
        self,
        name: str,
        start: Optional[datetime] = None,
        stop: Optional[datetime] = None,
        descending: bool = False
    ) -> List[ParameterValue]:
        """Gets a list of values for the parameter with the given name from
        the archive between the specified start and stop time (or all time if
        not specified).

        Values are returned in descending order (most recent first).

        :param name: parameter to query.
        :type name: str
        :param start: Datetime to start looking, defaults to `None`
            (beginning of time).
        :type start: Optional[datetime], optional
        :param stop: Datetime to stop looking, defaults to `None`
            (end of time).

        :type stop: Optional[datetime], optional
        :param descending: Most recent first if `True`, most recent last if
            `False`, defaults to `False`.

        :type descending: bool, optional
        :return: List of all parameter values matching the query.
        :rtype: List[ParameterValue]
        """

        history: List[ParameterValue] = []

        if self.archive is not None:
            history = self.archive.list_parameter_values(
                parameter=name,
                start=start,
                stop=stop,
                descending=descending,
                parameter_cache=self._processor_name
            )

        return [h for h in history]  # decompose generator

    def get_param_datapoints(
        self,
        name: str,
        start: Optional[datetime] = None,
        stop: Optional[datetime] = None,
        descending: bool = False
    ) -> List[Tuple[datetime, Any]]:
        """Creates a list of (generation_time, eng_value) datapoint tuples for
        the parameter with the given name from the archive between the
        specified start and stop time (or all time if not specified).

        Values are returned in descending order (most recent first).

        :param name: parameter to query.
        :type name: str

        :param start: Datetime to start looking, defaults to `None`
            (beginning of time).

        :type start: Optional[datetime], optional
        :param stop: Datetime to stop looking, defaults to `None`
            (end of time).
        :type stop: Optional[datetime], optional
        :param descending: Most recent first if `True`, most recent last if
            `False`, defaults to `False`.
        :type descending: bool, optional
        :return: List of all parameter values matching the query.
        :rtype: List[ParameterValue]
        """

        history = self.get_param_history(name, start, stop, descending)

        return [(p.generation_time, p.eng_value) for p in history]
