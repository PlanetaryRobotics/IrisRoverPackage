"""
Defines the Transceiver class for reading telemetry and sending commands over
a YAMCS. This is the only `Transceiver` used during mission to talk with the
flight rover.

Includes any supporting functions necessary for maintaining YAMCS connection.

@author: Connor W. Colombo (CMU)
@last-updated: 05/13/2022
"""
from typing import Any, Optional, Callable, ClassVar, Dict, Deque, List, Type, cast

from collections import deque
import re

# type: ignore (no typehints)
from yamcs.client import YamcsClient, YamcsProcessor
from yamcs.core.auth import Credentials as YamcsCredentials  # type: ignore

from .transceiver import Transceiver
from .endec import Endec, SlipEndec
from .logging import logger
from .exceptions import TransceiverConnectionException, TransceiverDecodingException

from IrisBackendv3.codec.packet import Packet, IrisCommonPacket
from IrisBackendv3.codec.payload_collection import EnhancedPayloadCollection
from IrisBackendv3.codec.metadata import DataPathway, DataSource
from IrisBackendv3.utils.basic import type_guard_argument


class YamcsTransceiver(Transceiver):
    # IP of host (YAMCS server IP, e.g. '172.17.0.37'):
    _host: str
    # Port the server is on (e.g. 11234):
    _port: int
    # YAMCS client which has a connection with the AMCC YAMCS server:
    _client: Optional[YamcsClient]
    # Processor used to interface with YAMCS using the `_client`:
    _processor: Optional[YamcsProcessor]

    # Where to get data from in YAMCS:
    YAMCS_INSTANCE: ClassVar[str] = 'Astrobotic-M1'
    YAMCS_PROCESSOR: ClassVar[str] = 'realtime'

    def _connect(self) -> bool:
        """
        Attempts to connect to YAMCS. Returns whether this was successful.
        """
        success = False

        try:
            # Sign in with username and password defined at top of file
            credentials = YamcsCredentials(
                username='iris',
                password=")S5FG.8c>'LnKcm*"
            )

            logger.info(
                "Attempting to connect `YamcsTransceiver` to "
                f"a YAMCS server at {self._host}:{self._port} . . ."
            )
            # Create a yamcs client that connects to the YAMCS server in AMCC:
            self._client = YamcsClient(
                f"{self._host}:{self._port}", credentials=credentials
            )

            self._processor = self._client.get_processor(
                instance=self.YAMCS_INSTANCE,
                processor=self.YAMCS_PROCESSOR
            )

            #! TODO: (WORKING-HERE)

            success = True
            logger.verbose(  # type: ignore
                "`YamcsTransceiver` connected successfully to "
                f"a YAMCS server at {self._host}:{self._port} . . ."
            )
        except Exception as e:
            logger.info(
                "Failed to connect `YamcsTransceiver` to "
                f"a YAMCS server at {self._host}:{self._port} . . ."
            )
            # Nullify all YAMCS variables to signify that we're not connected:
            self._client = None
            self._processor = None

        return success

    def __init__(
        self,
        host: str,  # ip of host (e.g. '172.17.0.37')
        port: int,  # port number (e.g. 11234)
        endecs: Optional[List[Endec]] = None,
        pathway: DataPathway = DataPathway.NONE,  # is variable
        source: DataSource = DataSource.YAMCS
    ) -> None:
        """ Initializes a `YamcsTransceiver`.

        NOTE: By default, no `endecs` should be required.
        """

        # Validate input:
        self._host = cast(str, type_guard_argument(str, 'host', host,
                                                   calling_function_name='YamcsTransceiver.__init__'))
        self._port = cast(int, type_guard_argument(int, 'port', port,
                                                   calling_function_name='YamcsTransceiver.__init__'))

        self.connect(host=host, port=port)

        super().__init__(
            endecs=endecs,
            pathway=pathway,
            source=source
        )

    def begin(self) -> None:
        """ Initialize any special registers, etc. for this transceiver.
        Can also be used to reset the state of the transceiver.
        """
        if self._client is None:
            # if it was connected successfully before during __init__,
            # attempt to reconnect:
            self.connect()

        super().begin()

    def _downlink_byte_packets(self) -> List[bytes]:
        """ Reads all **complete** packets of bytes from the connected serial
        interface using SLIP (Serial Line Internet Protocol).
        This SLIP implementation is designed according to RFC-1055 and based on
        the implementation in `sliplib 0.6.2` by Ruud de Jong.

        NOTE: No decoding occurs with the `endecs` here, just transmits the
        packet of bytes given.

        Returns a list of all packets received, as bytes. If no packets were
        received, this list is empty.
        """
        raise NotImplementedError()

    #! TODO: TX: set suffix by pathway meta, RX: set pathway by suffix
    # (have to override super's send and receive)

    def _uplink_byte_packets(self, packet_bytes: bytes) -> bool:
        """ Transmits the given packet of bytes on this `Transceiver`'s uplink
        transmission line.

        NOTE: No encoding occurs with the `endecs` here, just transmits the
        bytes as given.

        NOTE: This expects the given `packet_bytes` to have **ALREADY** been
        SLIP encoded by `Transceiver` since `SlipEndec` was added to `endecs` in
        `SlipTransceiver.__init__`.)

        Returns whether the uplink was successful.
        """

        # Send a command:
        yamcs_cmd_name = f"/{self.YAMCS_INSTANCE}/{yamcs_name_root}{pathway.yamcs_suffix}"
        status = processor.issue_command(yamcs_cmd_name, args=yamcs_payload)
        print("Command status: ", status.__dict__)

        return self._send_raw_bytes_over_serial(packet_bytes)
