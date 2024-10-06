"""
Timestamping utility for estimating SCET.

Author: Connor W. Colombo (colombo@cmu.edu)
Last Updated: 12/27/2023
"""
from typing import Any, Final, Callable, Protocol, ClassVar, List, cast, Dict, Type, TypedDict, Tuple
from datetime import datetime, timedelta, timezone
import logging

from IrisBackendv3.codec.packet_classes.packet import Packet
from IrisBackendv3.codec.payload import (
    TelemetryPayload, EventPayload, FileBlockPayload, DownlinkedPayload
)
from IrisBackendv3.codec.payload_collection import EnhancedPayloadCollection
from IrisBackendv3.utils.console_display import (
    init_telemetry_payload_log_dataframe,
    update_telemetry_payload_log_from_payloads,
    init_packet_log_dataframe,
    update_packet_log_dataframe
)

import IrisBackendv3 as IB3
import IrisBackendv3.ipc as ipc
from IrisBackendv3.ipc.messages import (
    DownlinkedPacketsMessage, DownlinkedPayloadsMessage
)

from .settings import settings


class RoverTimeEstimator:
    """Class that centralizes context and processing for estimating the time a
    payload was emitted in rover time (SCET) based on all available timestamp
    data.

    Current estimation technique is a pretty shitty approximation:
    For telemetry & Event timestamps w/out time-sync, we treat that max
    rover-timestamp as equal to PMCC_RX (of its packet) and then offset all
    other timestamps with the same PMCC_RX (same packet) in reverse.
    Then we shift all these back in time by the current downlink delay estimate.
    TODO: Use better `downlink_times` once populated and time-sync is active.
    """
    now: datetime
    pmcc_rx_to_max_timestamp: Dict[datetime, int]
    # Est. of full-pipeline downlink delay:
    _downlink_delay_est: timedelta
    # Logger to use for logging:
    _logger: logging.Logger

    def __init__(
        self,
        payloads: EnhancedPayloadCollection,
        now: datetime | None = None,
        downlink_delay_est: timedelta | None = None,
        logger: logging.Logger | None = None
    ) -> None:
        """Initializes the context for estimating rover time for any payload
        in the given collection. Performs useful pre-computations while still
        leaving the actual estimations to be computed at runtime."""
        # Grab a singular fixed value to consistently represent `now` for all
        # time operations on this collection:
        if not isinstance(now, datetime):
            self.now = datetime.now(timezone.utc)
        else:
            self.now = now
        if not isinstance(downlink_delay_est, timedelta):
            init_delay_est_ms = settings['INIT_EST_DL_DELAY_MS']
            init_delay_est = timedelta(milliseconds=init_delay_est_ms)
            self._downlink_delay_est = init_delay_est
        else:
            self._downlink_delay_est = downlink_delay_est
        if not isinstance(logger, logging.Logger):
            self._logger = logger  # type: ignore
        else:
            self._logger = logging.getLogger(__file__)

        self._store_max_rover_timestamp_for_each_pmcc_rx(payloads)

    def _store_max_rover_timestamp_for_each_pmcc_rx(
        self,
        payloads: EnhancedPayloadCollection
    ) -> Dict[datetime, int]:
        """
        For each unique PMCC_RX time in the given payload collection (likely only
        1 or 2), find the max rover timestamp of any `EventPayload` or
        `TelemetryPayload`.
        * Overwrites existing `self.pmcc_rx_to_max_timestamp`.
        * `now` must be set to a useful value before calling.
        """
        self.pmcc_rx_to_max_timestamp: Dict[datetime, int] = dict()
        for dlp_type in [TelemetryPayload, EventPayload]:
            dlp_type = cast(Type[TelemetryPayload] |
                            Type[EventPayload], dlp_type)
            for p in payloads[dlp_type]:
                p = cast(TelemetryPayload | EventPayload, p)
                timestamp: int = p.timestamp
                pmcc_rx: datetime
                if p.downlink_times is None or p.downlink_times.pmcc_rx is None:
                    pmcc_rx = self.now  # no pmcc_rx provided, so just use `now`
                else:
                    pmcc_rx = p.downlink_times.pmcc_rx
                if pmcc_rx not in self.pmcc_rx_to_max_timestamp:
                    self.pmcc_rx_to_max_timestamp[pmcc_rx] = timestamp
                elif timestamp > self.pmcc_rx_to_max_timestamp[pmcc_rx]:
                    self.pmcc_rx_to_max_timestamp[pmcc_rx] = timestamp
        return self.pmcc_rx_to_max_timestamp

    def estimate_rover_scet(
        self,
        payload: DownlinkedPayload
    ) -> Tuple[datetime, timedelta]:
        """Estimates the rover time when the given `payload` was generated,
        assuming this payload belongs to the collection used to initialize this
        `RoverTimeEstimator`.

        Performs the rover timestamp offset described above, given an 
        externally set datetime to use as `now` (as a fallback) and a 
        dictionary `pmcc_rx_to_max_timestamp` that maps each `pmcc_rx` 
        `datetime` to the latest *rover* timestamp that has that particular 
        `pmcc_rx` `datetime`.

        Returns the estimated SCET, alongside the estimated downlink delay
        timedelta baked into that SCET.
        """
        pmcc_rx: datetime
        if payload.downlink_times is not None and payload.downlink_times.pmcc_rx is not None:
            pmcc_rx = payload.downlink_times.pmcc_rx
        else:
            pmcc_rx = self.now  # use 'now' if None present

        delay_est = self._downlink_delay_est

        # Take the fast-route out for any Downlinked payloads that don't
        # include a timestamp:
        if not isinstance(payload, (TelemetryPayload, EventPayload)):
            # don't actually have rover timestamp information, use PMCC_RX:
            return pmcc_rx - delay_est, delay_est

        if payload.timestamp == 0 or payload.timestamp is None:
            # don't actually have rover timestamp information, use PMCC_RX:
            return pmcc_rx - delay_est, delay_est
        if not isinstance(payload.timestamp, (int, float)):
            # un-interpretable timestamp found, use PMCC_RX:
            self._logger.warning(
                " [TIMESTAMPING] Non-numeric non-None payload timestamp "
                f"(`{payload.timestamp=}`) encountered when processing: "
                f"`{payload=}`. Using `pmcc_rx`"
            )
            return pmcc_rx - delay_est, delay_est

        # Find the max rover timestamp associated with this PMCC_RX time
        # (assumes packet was sent the same ms this timestamp was generated and
        # that the transit time from rover to PMCC is `_downlink_delay_est`::
        offset_ms: int
        if pmcc_rx in self.pmcc_rx_to_max_timestamp:
            max_timestamp = self.pmcc_rx_to_max_timestamp[pmcc_rx]
            offset_ms = max_timestamp - payload.timestamp
        else:
            offset_ms = 0
        # Offset the PMCC_RX time and return:
        return (
            pmcc_rx - self._downlink_delay_est -
            timedelta(milliseconds=offset_ms),
            delay_est
        )
