"""
Defines the Transceiver class for reading telemetry and sending commands over
a serial port. Main use-case is using an RS-422 to Serial dongle to
communicate with the rover over RS-422.

Includes any supporting functions necessary for maintaining serial connection.

@author: Connor W. Colombo (CMU)
@last-updated: 02/26/2022
"""
from typing import Optional, List

import serial  # type: ignore # no type hints

from .ipc_messages import SendPayloadsRequestContent
from .transceiver import Transceiver
from .logging import logger

from IrisBackendv3.codec.packet import Packet, IrisCommonPacket
from IrisBackendv3.codec.payload import EnhancedPayloadCollection
from IrisBackendv3.codec.metadata import DataPathway, DataSource
from IrisBackendv3.utils.basic import type_guard_argument


class SerialTransceiver(Transceiver):
    seq_num: int = 0
    device: str
    baud: int
    serial_port: Optional[serial.Serial]

    def __init__(
        self,
        device: str = '/dev/ttyUSB0',
        baud: int = 9600
    ) -> None:
        # Validate inputs:
        self.device = type_guard_argument(str, 'device', device,
                                          calling_function_name='SerialTransceiver.init')
        self.baud = type_guard_argument(int, 'baud', baud,
                                        calling_function_name='SerialTransceiver.init')

        # Initialize sequence number counter:
        self.seq_num = 0

        # Initialize serial object:
        self.serial_port = None

    def connect_serial(self) -> None:
        """
        Open a serial port connection.
        """
        try:
            self.serial_port = serial.Serial(
                self.device, self.baud)  # open serial port

            logger.success(  # type: ignore # mypy complains that `logger` doesn't have a member `success` but it does
                "Connection Success!")
        except serial.SerialException as e:
            logger.error(
                f"Failed to connect to serial device `{self.device}`. "
                "Is the device name right? "
                f"Check the connection? Original error: {e}",
            )

    def begin(self) -> None:
        """ Initialize any special registers, etc. for this transceiver.
        Can also be used to reset the state of the transceiver.
        """
        # (Re)Initialize serial connection:
        self.connect_serial()

        # (Re)Initialize sequence number counter:
        self.seq_num = 0

    def read_update(self) -> List[Packet]:
        """Reads all available packets on the transceiver input."""
        raise NotImplementedError()

    def send_packet(self, packet: Packet) -> None:
        """Sends one packet over serial."""
        packet.

        # Successfully sent!
        logging.info(  # type: ignore
            f"[SerialTransceiver] Just sent packet over serial: {packet}")
        self.seq_num += 1

    def send(self, send_request: SendPayloadsRequestContent) -> None:
        """ Sends the given payloads in as few packets as possible.

        TODO: Use `IrisUplinkPacket` (and create a method for passing the
        whole packet incl. that data to storage.)

        TODO: Mind the MTUs and actually handle splitting up the `Payload`s
        into multiple `Packet`s as necessary.
        """
        # Log the payloads received:
        payload_strs = [
            str(payload)
            for payloads_of_type in send_request.payloads
            for payload in payloads_of_type
        ]
        logger.info(  # type: ignore
            "[SerialTransceiver] Attempting to packetize and send payloads "
            f"{','.join(payload_strs)} ."
        )

        # Build packet:
        packet = IrisCommonPacket(
            seq_num=self.seq_num,
            payloads=send_request.payloads
        )

        # Send!:
        self.send_packet(packet)
