"""
YAMCS!

01/08/2024
Colombo, CMU
"""
import time
import argparse
import IrisBackendv3 as IB3
import IrisBackendv3.ipc as ipc
from IrisBackendv3.transceiver.logs import logger_setConsoleLevel as xcvrLoggerLevel
from IrisBackendv3.ipc.messages import (
    DownlinkedPacketsMessage, DownlinkedPacketsContent,
    DownlinkedPayloadsMessage, DownlinkedPayloadsContent
)

import os  # noqa
os.environ["PROTOCOL_BUFFERS_PYTHON_IMPLEMENTATION"] = "python"  # noqa
from IrisBackendv3.data_standards import DataStandards  # noqa

from IrisBackendv3.codec.packet import parse_packet

from IrisBackendv3.data_standards.logs import (
    logger as DsLogger,
    logger_setConsoleLevel as DsLoggerLevel
)

from IrisBackendv3.data_standards.prebuilt import (
    add_to_standards, ALL_PREBUILT_MODULES
)

from IrisBackendv3.codec.settings import set_codec_standards

from IrisBackendv3.transceiver.yamcs_helper import *
# from IrisBackendv3.transceiver.h5_transceiver import (
#     H5Transceiver
# )

import warnings
from yamcs.client import YamcsClient, MDBClient, ArchiveClient  # type: ignore
from yamcs.tmtc.client import ProcessorClient  # type: ignore
from yamcs.tmtc.model import ParameterValue  # type: ignore
from yamcs.core.auth import Credentials as YamcsCredentials  # type: ignore

from typing import Any, Final, Tuple, List, Dict, Optional
from dataclasses import dataclass
from datetime import datetime, timedelta
import os.path

import argparse

from collections import OrderedDict
from termcolor import colored

import ulid
import numpy as np
import pandas as pd
import scapy.all as scp  # type: ignore


IB3.init_from_latest()

app = ipc.IpcAppHelper("YamcsQuickDirty")
app.setLogLevel('VERBOSE')

parser = argparse.ArgumentParser(description=(
    'IRIS Lunar Rover — YamcsQuickDirty Transceiver — CLI'
))


def get_opts():
    parser.add_argument('-i', '--yamcs-ip', type=str, default="172.17.0.51")
    parser.add_argument('-p', '--yamcs-port', type=int, default=443)
    parser.add_argument('-x', '--yamcs-pass', type=str, default="NOTTHEPASS")

    return parser.parse_args()


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


def ingest_yamcs_param(data) -> None:
    if data is not None:
        for param in data.parameters:
            app.logger.info(f"GOT {param.__str__()} . . .")


if __name__ == "__main__":
    opts = get_opts()
    app.logger.info(
        f"Boot w/ {opts.yamcs_ip}:{opts.yamcs_port}"
    )

    if 'IRIS_YAMCS_PASS' in os.environ:
        opts.yamcs_pass = os.getenv('IRIS_YAMCS_PASS')
        app.logger.notice("Loaded pass override.")

    # Load data:
    xcvrLoggerLevel(opts.log_level)
    app.setLogLevel(opts.log_level)
    kwargs = dict(
        packetgap=opts.packet_gap,  # skip first 37000 packets (of 37644)
        fixed_period_ms=opts.period_ms,
        loop=opts.loop,
        log_on_receive=False
    )
    # Setup IPC:
    manager = ipc.IpcAppManagerSync(socket_specs={
        'pub': ipc.SocketSpec(
            sock_type=ipc.SocketType.PUBLISHER,
            port=ipc.Port.TRANSCEIVER_PUB,
            topics=[ipc.Topic.DL_PACKETS],
            publish_only=True
        )
    })

    # YAMCS!:
    yi = YamcsInterface(
        server=opts.yamcs_ip,
        port=opts.yamcs_port,
        instance='Astrobotic-M1',
        processor='realtime',
        username='iris',
        password=opts.yamcs_pass,
        tls=True,
        auto_begin=True
    )
    params = [x for x in yi.mdb.list_parameters()]
    AVAIL_PARAM_QUAL_NAMES = [p.qualified_name for p in params]
    app.logger.notice(
        f"YAMCS PARAMS ACCESS: {AVAIL_PARAM_QUAL_NAMES}"
    )

    # Check setup:
    if not isinstance(yi.client, YamcsClient):
        raise Exception("Couldn't init YAMCS client . . . ")
    if not isinstance(yi.mdb, MDBClient):
        raise Exception("Couldn't init YAMCS mdb connection . . . ")
    if not isinstance(yi.archive, ArchiveClient):
        raise Exception("Couldn't init YAMCS archive connection . . . ")
    if not isinstance(yi.processor, ProcessorClient):
        raise Exception("Couldn't init YAMCS processor . . . ")
    app.logger.success("YAMCS connection success!")

    # Create YAMCS Subscription:
    app.logger.notice("Creating YAMCS subscription(s) . . .")
    # Subscribe to every data parameter we have access to:
    yi.processor.create_parameter_subscription(
        AVAIL_PARAM_QUAL_NAMES,
        on_data=ingest_yamcs_param
    )
