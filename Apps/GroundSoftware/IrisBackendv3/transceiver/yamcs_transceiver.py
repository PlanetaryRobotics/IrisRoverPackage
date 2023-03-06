"""
Defines the Transceiver class for reading telemetry and sending commands over
a YAMCS. This is the only `Transceiver` used during mission to talk with the
flight rover.

Includes any supporting functions necessary for maintaining YAMCS connection.

TODO: Consult script used for iTVAC...

@author: Connor W. Colombo (CMU)
@last-updated: 07/03/2022
"""
from typing import Any, Final, Optional, Callable, ClassVar, Dict, Deque, List, Type, cast

from collections import deque
import re

# from yamcs.client import YamcsClient, YamcsProcessor  # type: ignore
# from yamcs.core.auth import Credentials as YamcsCredentials  # type: ignore

from IrisBackendv3.transceiver.transceiver import Transceiver
from IrisBackendv3.transceiver.endec import Endec, SlipEndec
from IrisBackendv3.transceiver.logging import logger
from IrisBackendv3.transceiver.exceptions import TransceiverConnectionException, TransceiverDecodingException
from IrisBackendv3.transceiver.settings import settings

from IrisBackendv3.codec.packet_classes.packet import Packet
from IrisBackendv3.codec.packet_classes.iris_common import IrisCommonPacket
from IrisBackendv3.codec.payload_collection import EnhancedPayloadCollection
from IrisBackendv3.codec.metadata import DataPathway, DataSource
from IrisBackendv3.utils.basic import type_guard_argument


class YamcsTransceiver(Transceiver):
    # IP of host (YAMCS server IP, e.g. '172.17.0.37'):
    _host: Final[str]
    # Port the server is on (e.g. 11234):
    _port: Final[int]
    # # YAMCS client which has a connection with the AMCC YAMCS server:
    # _client: Optional[YamcsClient]
    # # Processor used to interface with YAMCS using the `_client`:
    # _processor: Optional[YamcsProcessor]

    # Where to get data from in YAMCS:
    YAMCS_INSTANCE: ClassVar[str] = 'Astrobotic-M1'
    YAMCS_PROCESSOR: ClassVar[str] = 'realtime'

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
        super().__init__(
            endecs=endecs,
            pathway=pathway,
            source=source
        )

        # Validate input:
        self._host = cast(str, type_guard_argument(str, 'host', host,
                                                   calling_function_name='YamcsTransceiver.__init__'))
        self._port = cast(int, type_guard_argument(int, 'port', port,
                                                   calling_function_name='YamcsTransceiver.__init__'))

        # Start all YAMCS variables as None to signify that we're not connected:
        self._client = None
        self._processor = None

    # ! TODO: Pull in connection methods from iTVAC scripts.
    # def begin(self) -> None:
    #     """ Initialize any special registers, etc. for this transceiver.
    #     Can also be used to reset the state of the transceiver.
    #     """
    #     super().begin()

    #     if self._client is None:
    #         # if it was connected successfully before during __init__,
    #         # attempt to reconnect:
    #         self._connect()

    # def _connect(self) -> bool:
    #     """
    #     Attempts to connect to YAMCS. Returns whether this was successful.
    #     """
    #     success = False

    #     try:
    #         details_msg: Final[str] = (
    #             f"instance: {self.YAMCS_INSTANCE} "
    #             f"\tusing processor: {self.YAMCS_PROCESSOR}"
    #             f"\ton a server at {self._host}:{self._port} "
    #             f"\twith user {settings['yamcs_username']}"
    #         )
    #         logger.info(
    #             "Attempting to connect `YamcsTransceiver` to "
    #             f"{details_msg} . . ."
    #         )

    #         credentials = YamcsCredentials(
    #             username=settings['yamcs_username'],
    #             password=settings['yamcs_password']
    #         )

    #         # Create a yamcs client that connects to the YAMCS server in AMCC:
    #         self._client = YamcsClient(
    #             f"{self._host}:{self._port}", credentials=credentials
    #         )

    #         self._processor = self._client.get_processor(
    #             instance=self.YAMCS_INSTANCE,
    #             processor=self.YAMCS_PROCESSOR
    #         )

    #         success = True
    #         logger.success(  # type: ignore
    #             f"`YamcsTransceiver` connected successfully to {details_msg}."
    #         )
    #     except Exception as e:
    #         logger.error(
    #             f"Failed to connect `YamcsTransceiver` to {details_msg}."
    #         )
    #         # Nullify all YAMCS variables to signify that we're not connected:
    #         self._client = None
    #         self._processor = None

    #     return success

    # @property
    # def has_connected(self) -> bool:
    #     """
    #     Returns whether this Transceiver has successfully connected to YAMCS
    #     during its most recent connection attempt. If it has since experienced
    #     a connection failure, this becomes `False`.

    #     NOTE: This just checks the history. It's not an active test and does not
    #     poll the network to determine if it's currently connected (which is
    #     often unnecessary).
    #     """
    #     return self._client is not None and self._processor is not None

    # def _downlink_byte_packets(self) -> List[bytes]:
    #     """ Reads all **complete** packets of bytes from the connected serial
    #     interface using SLIP (Serial Line Internet Protocol).
    #     This SLIP implementation is designed according to RFC-1055 and based on
    #     the implementation in `sliplib 0.6.2` by Ruud de Jong.

    #     NOTE: No decoding occurs with the `endecs` here, just transmits the
    #     packet of bytes given.

    #     Returns a list of all packets received, as bytes. If no packets were
    #     received, this list is empty.
    #     """
    #     raise NotImplementedError()

    # #! TODO: TX: set suffix by pathway meta, RX: set pathway by suffix
    # # (have to override super's send and receive)

    # def send(self, packet: Packet, **uplink_metadata) -> bool:
    #     """ Sends the given packet over this `Transceiver`'s transmission line,
    #     encoding it as necessary.

    #     `**uplink_metadata` contains any special data needed by methods further
    #     down the uplink pipeline, particularly `_uplink_byte_packet`.

    #     NOTE: AB's YAMCS setup requires that:
    #     - Each payload uplink packet contain only one command.
    #     - Each payload uplink packet be labelled with the desired interface
    #       (rs422 or wifi).

    #     Returns whether the send was successful.
    #     """
    #     # Verify AB YAMCS requirements:

    #     # Add necessary metadata:
    #     uplink_metadata = {
    #         **uplink_metadata,
    #         'command_name': command.name,
    #         'yamcs_suffix': pathway.yamcs_suffix
    #     }

    #     # Dispatch to the superclass' `send` method, now equipped with the
    #     # proper metadata:
    #     return super().send(packet, **uplink_metadata)

    # def _uplink_byte_packet(
    #     self,
    #     packet_bytes: bytes,
    #     **uplink_metadata
    # ) -> bool:
    #     """ Transmits the given packet of bytes on this `Transceiver`'s uplink
    #     transmission line.

    #     NOTE: No encoding occurs with the `endecs` here, just transmits the
    #     bytes as given.

    #     NOTE: `**uplink_metadata` contains any special data needed by methods further
    #     down the uplink pipeline, particularly `_uplink_byte_packet`.

    #     Returns whether the uplink was successful.
    #     """

    #     if self.has_connected:

    #         yamcs_cmd_name = (
    #             f"/{self.YAMCS_INSTANCE}"
    #             f"/{uplink_metadata['command_name']}"
    #             f"{uplink_metadata['yamcs_suffix']}"
    #         )

    #         # Build the YAMCS payload:
    #         yamcs_payload = {
    #             'tc-binary': ''.join(['{:02x}'.format(x) for x in packet_bytes])
    #         }
    #         status = self._processor.issue_command(
    #             yamcs_cmd_name, args=yamcs_payload)

    #         # TODO: Check the status dict for the queue it was placed in (maybe
    #         # if it was automatically sent, we want to capture that here).

    #         # TODO: Monitor the queue to make sure it actually goes through.
    #         print("Command status: ", status.__dict__)
    #         return True

    #     else:
    #         return False
