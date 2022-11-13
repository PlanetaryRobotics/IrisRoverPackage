"""
Programs the Radio using DFU via Hercules BGAPI Passthrough over RS422 via the
Watchdog.

NOTE: For simplicity, this process is completely synchronous. This shouldn't
cause any issues since the only incoming data we care about are command
responses and we start looking for those immediately after sending a command.

@author: Connor W. Colombo (CMU)
@last-updated: 11/13/2022
"""
# Activate postponed annotations (for using classes as return type in their own methods):
from __future__ import annotations

from typing import cast, Any, Optional, Final, Callable, Tuple, Dict, List, Type, Generic, TypeVar

import time
import serial  # type: ignore # no type hints
from serial.tools import list_ports, list_ports_common  # type: ignore
from abc import ABC
from enum import Enum as PyEnum
from collections import OrderedDict
from dataclasses import dataclass

import ulid
import scapy.all as scp  # type: ignore
from datetime import datetime

import logging
import verboselogs  # type: ignore # mypy doesn't see type hints
import coloredlogs  # type: ignore # mypy doesn't see type hints

import IrisBackendv3.codec.bgapi as bgapi
from IrisBackendv3.utils.crc import crc32_fsw

from IrisBackendv3.data_standards.module import Command, TelemetryChannel, Event
from IrisBackendv3.codec.payload import Payload, TelemetryPayload, EventPayload, CommandPayload, WatchdogCommandPayload
from IrisBackendv3.codec.payload_collection import EnhancedPayloadCollection, extract_downlinked_payloads
from IrisBackendv3.codec.packet import Packet, IrisCommonPacket, RadioBgApiPacket, UnsupportedPacket
from IrisBackendv3.codec.packet import parse_packet as core_parse_packet
from IrisBackendv3.codec.exceptions import PacketDecodingException
from IrisBackendv3.codec.metadata import DataPathway, DataSource
from IrisBackendv3.codec.magic import Magic, MAGIC_SIZE

import trans_tools as XCVR

BGAPI_API: Final = bgapi.BGAPI_WIFI_API
BGAPI_ENCODE: Final = bgapi.BGAPI_WIFI_ENCODER
BGAPI_DECODE: Final = bgapi.BGAPI_WIFI_DECODER

# Application Context (settings, flags, etc. to be passed by reference to functions that need it):
APP_CONTEXT: Final[Dict[str, Any]] = {
    # Baud rate for the RS422 comms connection to the Rover
    'baud': 9600,
    # Serial number of RS422 transceiver:
    # 'serial_device_sn': 'A7035PDL',  # Connects to the Lander harness
    'serial_device_sn': 'AB0JRGV8',  # Connects to J36-RS422 header on the SBC
    # Path to the DFU binary file to load:
    'dfu_file_path': '../FlightSoftware/Radio/build/iris_wifi_radio__auto_connect.dfu',
    # DFU file is read in sections of size `chunkSize`. 128B is the standard
    # defined by the BGAPI docs and changing this value is not recommended.
    'dfu_chunk_size': 128,
    # Base to apply to all PacketIds derived from the DFU file (so maintenance
    # commands can have a lower ID and so all file packet IDs can be easily
    # identifiable in the passthrough dumps).
    'file_packet_id_base': int(1e7),
    # Delay in [ms] to put between each packet send to make we don't overwhelm
    # the WD:
    # NOTE: total operation time involve ~4000 throttles in addition to normal
    # data transmission time.
    # we expect the WD to have to process about 26B of return data (at minimum)
    # after each command send.
    'throttling_time_ms': 26 / (9600/10) * 1000
}

DATETIME_FORMAT_STR: Final[str] = '%m-%d %H:%M:%S.%f'
LOGGER_OUT_PATH: Final[str] = (
    f"./out/___bgapi_passthrough_programming_session__"
    f"{ulid.from_timestamp(datetime.now())}.log.ansi"
)


# SETUP LOGGER:
verboselogs.install()
# set up logging to file - see previous section for more details
logging.basicConfig(level='SPAM',
                    format='%(asctime)s %(levelname)-8s\t %(message)s',
                    datefmt=DATETIME_FORMAT_STR,
                    filename=LOGGER_OUT_PATH,
                    filemode='a'  # keep adding data to log, don't overwrite
                    )
# define a Handler which writes DEBUG messages or higher to the sys.stderr
console = logging.StreamHandler()
console.setLevel('SPAM')
# set a format which is simpler for console use
FORMAT = '%(asctime)-15s: %(levelname)-8s %(message)s'
formatter = logging.Formatter(FORMAT)
# tell the handler to use this format
console.setFormatter(formatter)
# add the handler to the root logger
logging.getLogger().addHandler(console)
# Now, we can log to the root logger, or any other logger. First the root...
progLogger: Any = logging.getLogger(__name__)
coloredlogs.install(logger=progLogger, level='VERBOSE', fmt=FORMAT)


def compile_bgapi_cmd_as_icp(packet_id: int, bgcmd: bgapi.BGCommand) -> bytes:
    """
    Compiles the given BGAPI Command inside an Iris Common Packet (ICP) as a
    BGAPI-Passthrough Command to the Hercules. This Hercules command is tagged
    with the given packet_id (uint32_t).
    Returns the raw packet bytes to be sent to the Rover.
    """
    # Serialize BGAPI command (max 134B - checked by ICP encoder automatically):
    bg_data = bgapi.encode_command(BGAPI_ENCODE, bgcmd)

    # Compute args:
    command_args = OrderedDict(
        crc_32=crc32_fsw(bg_data),
        packet_id=packet_id,
        expect_response='NM_BGAPI_CMD_DONTEXPECTRESPONSE' if bgcmd.no_response else 'NM_BGAPI_CMD_EXPECTRESPONSE',
        bgapi_packet=bg_data
    )

    # Pack inside an Iris Common Packet:
    command_name = 'NetworkManager_SendBgApiCommand'
    module, command = XCVR.standards.global_command_lookup(command_name)

    # Pack command:
    # Make ICP payload:
    command_payload = CommandPayload(
        pathway=DataPathway.WIRED,
        source=DataSource.GENERATED,
        magic=Magic.COMMAND,
        module_id=module.ID,
        command_id=command.ID,
        args=command_args
    )
    # Load payload into ICP packet:
    payloads = EnhancedPayloadCollection(CommandPayload=[command_payload])
    packet = IrisCommonPacket(
        seq_num=0x00,
        payloads=payloads
    )
    packet_bytes = packet.encode()

    return packet_bytes


@dataclass
class DfuChunk:
    """
    Collection of all data about a DFU chunk.
    A bit verbose and memory inefficient since technically of this can be
    derived from the compiled packet bytes but it'll help ease diagnostics and
    since we have to get this right, this is acceptable (laptop memory isn't
    really an issue atm).
    """
    chunkIndex: int
    packetId: int
    bgcmd: bgapi.BGCommand
    # Compiled Iris Common Packet bytes for transmission:
    icPacketBytes: bytes


def compile_dfu_file_to_icp(
    dfu_file_path: str,
    chunkSize: int
) -> Tuple[List[DfuChunk], int]:
    """Compiles a DFU file into a sequence of BGCommands, each wrapped in a
    BGAPI Passthrough Command, each wrapped in a preprepared Iris Common Packet
    for transmission.
    DFU file is read in sections of size `chunkSize`.

    Returns (compiledPackets, totalFileSize)
    """
    compiledPackets: List[DfuChunk] = []
    totalFileSize: int = 0

    with open(dfu_file_path, 'rb') as dfu_file:
        chunkIndex = -1
        while True:
            # Read in `chunkSize` bytes. If less than that are left, all
            # remaining bytes will be read.
            # Total DFU file size should be 512kB, which is 4000*128B, so this
            # should chunk evenly.
            chunkData = dfu_file.read(chunkSize)
            if len(chunkData) == 0:
                break  # we're done
            totalFileSize += len(chunkData)

            chunkIndex += 1
            packetId = APP_CONTEXT['file_packet_id_base'] + chunkIndex

            # Pack data:
            bgcmd = bgapi.build_command(
                bgapi.BGAPI_WIFI_API, 'dfu', 'flash_upload', {'data': chunkData})
            packetBytes = compile_bgapi_cmd_as_icp(packetId, bgcmd)

            # Store data:
            compiledPackets.append(DfuChunk(
                chunkIndex=chunkIndex,
                packetId=packetId,
                bgcmd=bgcmd,
                icPacketBytes=packetBytes
            ))

    return (compiledPackets, totalFileSize)


def send_bytes(data: bytes) -> None:
    """Sends the given bytes to the Rover. Abstracted in case we want to reuse
    this logic but change the transceiver behavior later."""
    time.sleep(APP_CONTEXT['throttling_time_ms']/1000)
    XCVR.send_data_wd_serial(data)  # type: ignore


class SlipState(PyEnum):
    FIRST_END = 1
    FIRST_BYTE_OR_STARTING_END = 2
    STARTED = 3


class BasicSerialUdpSlipTransceiver:
    """
    Really terribly written (but functional) Stateful Serial UDP SLIP
    transceiver.
    Basically just wrapped `stream_data_ip_udp_serial` in a class to preserve
    its state data non-globally.
    """

    def __init__(
        self,
        device_sn: str,
        baud: int
    ) -> None:
        self.device_sn = device_sn
        self.baud = baud
        self.ser = None

        self.last_warn_time = datetime.now()
        self.escape = False
        self.keep_running = True
        self.line = b''
        self.data_bytes = bytearray(b'')
        self.full_packets: List[bytes] = []
        self.slip_state = SlipState.FIRST_END

    @property
    def connected(self): return self.ser is not None

    def connect(self) -> bool:
        """
        Open a serial port connection.
        """
        try:
            # Get device ID from SN:
            device = [
                cp.device for cp in list_ports.comports()
                if cp.serial_number is not None and self.device_sn in cp.serial_number
            ][0]
            self.ser = serial.Serial(device, self.baud)  # open serial port
            progLogger.success("Connection Success!")
            return True
        except Exception as e:
            progLogger.error(
                f"Failed to connect to serial device. "
                f"Is the device name right? Check the connection? "
                f"Original error: {e}"
            )
            return False

    def append_byte(self, b):
        if self.escape:
            if b == 0xDC:
                self.data_bytes.append(0xC0)
            elif b == 0xDD:
                self.data_bytes.append(0xDB)
            self.escape = False
        else:
            if b == 0xDB:
                self.escape = True
            else:
                self.data_bytes.append(b)

    def read_until_packet(self) -> Packet:
        """Reads serial until a packet comes out."""
        if self.connected:
            ready = True
        else:
            ready = False
            progLogger.error(
                "Can't read data, serial connection not started. "
                "Try `connect_serial()`."
            )
            # Return something:
            return UnsupportedPacket(raw=b'')

        while self.keep_running and ready and self.ser is not None:
            try:  # safety exception catch to keep things running
                b: Any = self.ser.read(1)
                self.line += b
                b = int.from_bytes(b, 'big')

                if self.slip_state == SlipState.FIRST_END:
                    if b == 0xC0:
                        self.slip_state = SlipState.FIRST_BYTE_OR_STARTING_END
                elif self.slip_state == SlipState.FIRST_BYTE_OR_STARTING_END:
                    if b != 0xC0:
                        self.append_byte(b)
                        self.slip_state = SlipState.STARTED
                elif self.slip_state == SlipState.STARTED:
                    if b == 0xC0:
                        if len(self.data_bytes) != 0:  # packet baked:
                            # Process it:
                            packet = XCVR.parse_ip_udp_packet(self.data_bytes)
                            # Reset:
                            self.data_bytes = bytearray(b'')
                            self.slip_state = SlipState.FIRST_BYTE_OR_STARTING_END
                            # Return packet:
                            return packet
                    else:
                        self.append_byte(b)

            except PacketDecodingException as pde:
                # Reset SLIP:
                self.data_bytes = bytearray(b'')
                self.slip_state = SlipState.FIRST_BYTE_OR_STARTING_END

                # Print what happened:
                progLogger.warning(
                    "While streaming data, a PacketDecodingException occurred. "
                    "The data bytes at the time of the exception were: \n"
                    f"{scp.hexdump(self.data_bytes, dump=True)}\n."
                    f"The PacketDecodingException was: `{pde}`."
                )
            except Exception as e:
                # Debounce this warning:
                if (datetime.now()-self.last_warn_time).total_seconds() > 1:
                    self.last_warn_time = datetime.now()
                    progLogger.warning(
                        f"An otherwise unresolved error occurred during packet "
                        f"streaming: {e}."
                    )
                self.data_bytes = bytearray(b'')
                self.slip_state = SlipState.FIRST_BYTE_OR_STARTING_END

        # Handle in case we somehow get all the way to the end and break out of
        # the loop:
        return UnsupportedPacket(raw=b'')


UDP_SLIP_XCVR: Optional[BasicSerialUdpSlipTransceiver] = None


def read_packet() -> Packet:
    """Reads the datastream from the Rover until a packet comes out.
    Abstracted in case we want to reuse this logic but change the transceiver
    behavior later."""
    if UDP_SLIP_XCVR is None:
        progLogger.error(
            "Can't stream data. Serial transceiver is not initialized."
        )
        # Return something:
        return UnsupportedPacket(raw=b'')
    else:
        packet = UDP_SLIP_XCVR.read_until_packet()
        progLogger.spam(
            f"Got:\n{packet}\n{scp.hexdump(packet.raw, dump=True)}")
        return packet


PFT = TypeVar('PFT')


class PacketFilter(ABC, Generic[PFT]):
    """Functor used for filtering packets until target data is found."""

    def __call__(self, packet: Packet) -> Tuple[bool, Optional[PFT]]:
        """
            Returns a tuple containing:
            (
                if the data was found,
                the matching data.
            )
        """
        raise NotImplementedError()


class PacketTypeFilter(PacketFilter[Packet]):
    """Functor used for filtering packets until target data is found."""
    packet_type: Type[Packet]

    def __init__(self, packet_type: Type[Packet]) -> None:
        self.packet_type = packet_type

    def __call__(self, packet: Packet) -> Tuple[bool, Optional[Packet]]:
        if isinstance(packet, self.packet_type):
            return (True, packet)

        return (False, None)


class PacketBgMsgFilter(PacketFilter[bgapi.BGMsg]):
    """Functor used for filtering packets until a `RadioBgApiPacket` is
    received where the `class_name` and `msg_name` match the given values."""
    class_name: str
    msg_name: str

    def __init__(self, class_name: str, msg_name: str) -> None:
        self.class_name = class_name
        self.msg_name = msg_name

    def __call__(self, packet: Packet) -> Tuple[bool, Optional[bgapi.BGMsg]]:
        if isinstance(packet, RadioBgApiPacket):
            bgmsg = packet.getMessage()
            if (
                bgmsg is not None and
                bgmsg._class_name == self.class_name and
                bgmsg._msg_name == self.msg_name
            ):
                return (True, bgmsg)

        return (False, None)


PPFT = TypeVar('PPFT', bound=Payload)


class PacketPayloadFilter(PacketFilter[List[PPFT]]):
    """Functor used for filtering packets until a packet containing at least
    one payload with the given type is found.
    Returns a list of all payloads of the given type in the matching packet."""
    payload_type: Type[PPFT]

    def __init__(self, payload_type: Type[PPFT]) -> None:
        self.payload_type = payload_type

    def __call__(self, packet: Packet) -> Tuple[bool, Optional[List[PPFT]]]:
        target_payloads = [
            cast(self.payload_type, p)  # type: ignore
            for p in packet.payloads[self.payload_type]
        ]
        if len(target_payloads) > 0:
            return (True, target_payloads)

        return (False, None)


class PacketEventFilter(PacketFilter[List[EventPayload]]):
    """Functor used for filtering packets until a packet
    containing at least one instance of the given event is found
    Returns a list of all payloads of the given type in the matching packet."""
    event: Event

    def __init__(self, event: Event) -> None:
        self.event = event

    def __call__(self, packet: Packet) -> Tuple[bool, Optional[List[EventPayload]]]:
        event_payloads = [cast(EventPayload, ep)
                          for ep in packet.payloads[EventPayload]]
        matching_events = [
            ep for ep in event_payloads if ep.event == self.event]
        if len(matching_events) > 0:
            return (True, matching_events)

        return (False, None)


def read_until_filter(
    packet_filters: List[PacketFilter],
    wait_count_max: int
) -> Optional[List]:
    """Synchronously reads the datastream from the rover until a packet causes
    one of the given `PacketFilter`s to return `True`.

    Returns a list with one entry for every packet filter where the entries are
    the extracted value returned from the PacketFilter when called on the first
    packet that makes any of them return `(True, _)`.
    """
    for _ in range(wait_count_max):
        packet = read_packet()
        if isinstance(packet, Packet):
            # Run on all filters:
            results = [pf(packet) for pf in packet_filters]
            if any(match for match, _ in results):
                # A match was found for one of the filters!
                # Return all the results.
                return [result for _, result in results]

    # Nothing was found:
    return None


class PassthroughVerificationStateMachine:
    """
    State Machine for Sending and Verifying that the packet with the given
    `packetId` and Iris Common Packet byte `icPacketBytes` is received by the
    Rover and executed successfully.

    `bgcmd` is the BGAPI Command spec that is contained inside `icPacketBytes`
    (used for verifying BGAPI response packets).

    If NM indicates that the command failed to go through, it will be resent up
    to `maxSendCount` number of times until it successfully goes through.

    Likewise, if no command response is received, the SM will attempt to
    request records. If those records aren't received in a certain number of
    packets, it'll try again up to `maxRecordsRequestCount` times.

    Used by `send_and_verify_bgapi_packet`.
    """
    packetId: int
    bgcmd: bgapi.BGCommand
    icPacketBytes: bytes
    maxSendCount: int
    maxRecordsRequestCount: int

    send_count: int
    records_request_count: int

    DISPATCH_TABLE: Final[Dict[State, Callable[[], State]]]

    def __init__(
        self,
        packetId: int,
        bgcmd: bgapi.BGCommand,
        icPacketBytes: bytes,
        maxSendCount: int,
        maxRecordsRequestCount: int
    ) -> None:
        self.packetId = packetId
        self.bgcmd = bgcmd
        self.icPacketBytes = icPacketBytes

        self.maxSendCount = maxSendCount
        self.send_count = 0  # start at 0

        self.maxRecordsRequestCount = maxRecordsRequestCount
        self.records_request_count = 0  # start at 0

        # Create dispatch table (can't be ClassVar because handlers are
        # non-static members of `self`):
        self.DISPATCH_TABLE = {
            self.State.INIT: self._handle_init,
            self.State.SEND_PACKET: self._handle_send_packet,
            self.State.AWAIT_BGAPI_RESPONSE: self._handle_await_bgapi_response,
            self.State.REQUEST_RECORDS: self._handle_request_records,
            self.State.AWAIT_RECORDS: self._handle_await_records,
            self.State.RADIO_FAILURE: self._handle_radio_failure,
            self.State.FAULT: self._handle_fault,
            self.State.DONE: self._handle_done
        }

    class State(PyEnum):
        # All States for PassthroughVerificationStateMachine
        INIT = 0x00
        SEND_PACKET = 0x10
        AWAIT_BGAPI_RESPONSE = 0x11
        REQUEST_RECORDS = 0x20
        AWAIT_RECORDS = 0x21
        # Got a response from the Radio and it indicated a failure on its part:
        RADIO_FAILURE = 0xFD
        # A fault prevented the message from getting to the Radio:
        FAULT = 0xFE
        # Everything worked!
        DONE = 0xFF

    def _handle_init(self) -> PassthroughVerificationStateMachine.State:
        progLogger.spam(
            f'PassthroughVerificationStateMachine in INIT for '
            f'ID={self.packetId}, bgcmd={self.bgcmd}.'
        )
        # Go to SEND_PACKET:
        return self.State.SEND_PACKET

    def _handle_send_packet(self) -> PassthroughVerificationStateMachine.State:
        progLogger.spam('In SEND_PACKET.')

        if self.send_count >= self.maxSendCount:
            # We've done this too many times already. We're done.
            progLogger.warning(
                f"Max send count ({self.maxSendCount}) reached. "
                f"Aborting send of {self.bgcmd}."
            )
            return self.State.FAULT

        # Send packet:
        send_bytes(self.icPacketBytes)
        self.send_count += 1
        # Move on to awaiting a response:
        return self.State.AWAIT_BGAPI_RESPONSE

    def _handle_await_bgapi_response(self) -> PassthroughVerificationStateMachine.State:
        progLogger.spam('In AWAIT_BGAPI_RESPONSE.')
        # Read datastream until either:
        # A BGAPI response packet for the command is found
        # OR an ACK event is found.
        fprimeEvent = XCVR.standards.modules['NetworkManager'].events['RadioSendBgApiCommandAck']
        results = read_until_filter(
            [
                PacketBgMsgFilter(self.bgcmd._class_name,
                                  self.bgcmd._msg_name),
                PacketEventFilter(fprimeEvent)
            ],
            # a max of 9 BGAPI packets diff was observed in testing (when the
            # radio was running it's normal script - not in DFU mode).
            # Use 15 to be safe.
            wait_count_max=15
        )

        if results is None or all(x is None for x in results):
            # No response was found. Likely we missed it or didn't get it.
            # Ask for BGAPI passthrough records:
            progLogger.debug('Awaiting results maxxed out.')
            return self.State.REQUEST_RECORDS

        # Check individual results:
        bgResponse, events = results
        if isinstance(bgResponse, bgapi.BGResponse):
            progLogger.debug(f'Got BGResponse: {bgResponse}')
            if bgResponse._errorcode == 0:
                # We received a success message. We're DONE!
                return self.State.DONE
            else:
                # We received a response but it indicated a failure.
                progLogger.warning(
                    f"Got Radio error code {bgResponse._errorcode} "
                    f"in {bgResponse} after sending {self.bgcmd}."
                )
                return self.State.RADIO_FAILURE

        # Check all matching ACK events (should be only 1 but ... maybe there
        # are multiple if lagging occurred in Hercules, either way, should only
        # be 1 for us):
        eventPayloads = cast(List[EventPayload], events)
        for ep in eventPayloads:
            progLogger.debug(f'Got RadioSendBgApiCommandAck Event: {ep}')
            if ep.event == fprimeEvent and ep.args['packet_id'] == self.packetId:
                # This is a RadioSendBgApiCommandAck for us. Decode status:
                status_arg = [
                    a for a in fprimeEvent.args if a.name == 'status'
                ][0]
                status = status_arg.get_enum_formatted_str(ep.args['status'])

                if status in ['NM_BGAPI_SEND_SUCCESS', 'NM_BGAPI_SEND_SUCCESSNORESP']:
                    progLogger.debug(
                        f'Got good RadioSendBgApiCommandAck status: {status}.'
                    )
                    # We met some def of success:
                    return self.State.DONE
                else:
                    progLogger.warning(
                        f'Got bad RadioSendBgApiCommandAck status: {status}.\n'
                        f'Trying again...'
                    )
                    # Try sending again:
                    return self.State.SEND_PACKET

        # If we get here no response contained useful information.
        # So, we'll have to request records:
        progLogger.debug('No useful info. Requesting records.')
        return self.State.REQUEST_RECORDS

    def _handle_request_records(self) -> PassthroughVerificationStateMachine.State:
        progLogger.spam('In REQUEST_RECORDS.')

        if self.records_request_count >= self.maxRecordsRequestCount:
            # We've done this too many times already. We're done.
            progLogger.warning(
                f"Max records request count ({self.maxRecordsRequestCount}). "
                f"reached. Aborting send of {self.bgcmd}."
            )
            return self.State.FAULT

        # Build Request Command:
        command_name = 'NetworkManager_DownlinkBgApiCommandRecords'
        module, command = XCVR.standards.global_command_lookup(command_name)
        # Make ICP payload:
        command_payload = CommandPayload(
            pathway=DataPathway.WIRED,
            source=DataSource.GENERATED,
            magic=Magic.COMMAND,
            module_id=module.ID,
            command_id=command.ID,
            args=OrderedDict()
        )
        # Load payload into ICP packet:
        payloads = EnhancedPayloadCollection(CommandPayload=[command_payload])
        packet = IrisCommonPacket(
            seq_num=0x00,
            payloads=payloads
        )
        packet_bytes = packet.encode()

        # Send request command:
        send_bytes(packet_bytes)
        self.records_request_count += 1

        # Move on to awaiting a response:
        return self.State.AWAIT_BGAPI_RESPONSE

    def _handle_await_records(self) -> PassthroughVerificationStateMachine.State:
        progLogger.spam('In AWAIT_RECORDS.')

        # Read datastream until either a records event is found:
        fprimeEvent = XCVR.standards.modules['NetworkManager'].events['RadioBgApiCommandRecords']
        results = read_until_filter(
            [PacketEventFilter(fprimeEvent)],
            # a max of 9 BGAPI packets diff was observed in testing (when the
            # radio was running it's normal script - not in DFU mode).
            # Use 15 to be safe.
            wait_count_max=15
        )

        if results is None or all(x is None for x in results):
            # No response was found. Likely we missed it or didn't get it.
            # Ask again:
            progLogger.debug('Awaiting results maxxed out.')
            return self.State.REQUEST_RECORDS

        # Check results:
        events = results[0]

        # Check all matching Records events (should be only 1 but ... maybe there
        # are multiple if lagging occurred in Hercules, either way, should only
        # be 1 for us):
        eventPayloads = cast(List[EventPayload], events)
        for ep in eventPayloads:
            if ep.event == fprimeEvent:
                progLogger.debug(f'Got RadioBgApiCommandRecords Event: {ep}')
                # This is a RadioBgApiCommandRecords.

                # Due to FPrime weirdness (it uses UNSCOPED enums for all its
                # enums), we have to have different conversion checking for
                # each enum. This helper helps with that so we don't have to
                # repeat code:
                def checkRecord(recordIdx: int):
                    """Checks the record with the given index and returns:
                    - `True` if it indicates our command was successful,
                    - `False` if not,
                    - `None` if the record isn't for our command.
                    """
                    if ep.args[f'packet_id_{recordIdx}'] != self.packetId:
                        return None

                    # This record is for us. Decode the status:
                    status_name = f'result_{recordIdx}'
                    status_arg = [
                        a for a in fprimeEvent.args
                        if a.name == status_name
                    ][0]
                    status = status_arg.get_enum_formatted_str(
                        ep.args[status_name])

                    # Return if this result indicates any sort of success:
                    return status is not None and 'SUCCESS' in status

                # Check all records, starting with 0 (most recent):
                for i in range(3):
                    status = checkRecord(i)
                    if status is not None:
                        # This record is for us:
                        if status:
                            # Records show we were successful. We're DONE!
                            progLogger.debug(
                                f'Records show {self.packetId} was successful.'
                            )
                            return self.State.DONE
                        else:
                            # Records show it didn't work, try sending again:
                            progLogger.warning(
                                f'Records show {self.packetId} failed.\n'
                                f'Trying again...'
                            )
                            return self.State.SEND_PACKET

        # If we get here no response contained useful information.
        # So, we'll have to request records again (all we can do):
        progLogger.debug('No useful info. Requesting records.')
        return self.State.REQUEST_RECORDS

    def _handle_radio_failure(self) -> PassthroughVerificationStateMachine.State:
        # Stay in RADIO_FAILURE:
        progLogger.spam('In RADIO_FAILURE.')
        return self.State.RADIO_FAILURE

    def _handle_fault(self) -> PassthroughVerificationStateMachine.State:
        # Stay in FAULT:
        progLogger.spam('In FAULT.')
        return self.State.FAULT

    def _handle_done(self) -> PassthroughVerificationStateMachine.State:
        # Stay in DONE:
        progLogger.spam('In DONE.')
        return self.State.DONE

    def dispatch(
        self,
        current_state: PassthroughVerificationStateMachine.State
    ) -> PassthroughVerificationStateMachine.State:
        """ Given the `current_state`, dispatch to the appropriate handler for
        that state and then return the next state to go to (return value of the
        handler)."""
        # Lookup state in dispatch table, dispatch, return result:
        return self.DISPATCH_TABLE[current_state]()

    def run(self) -> PassthroughVerificationStateMachine.State:
        """ Run the state machine until DONE, FAULT, or FAIL.

        Returns: the final state (DONE, FAULT, or RADIO_FAILURE).
        """
        state = self.State.INIT

        while(state not in [self.State.DONE, self.State.FAULT, self.State.RADIO_FAILURE]):
            state = self.dispatch(state)

        return state


def send_and_verify_bgapi_cmd(
    packetId: int,
    bgcmd: bgapi.BGCommand,
    icPacketBytes: bytes
) -> PassthroughVerificationStateMachine.State:
    """ Sends the given IrisCommonPacket bytes, which contain a BGAPI command
    with the given packetId, and then verifies with Hercules that the given
    packet was received correctly, modifying if need.

    `bgcmd` is the BGAPI Command spec that is contained inside `icPacketBytes`
    (used for verifying BGAPI response packets).

    Returns the final state of the message sending (DONE, FAULT, or RADIO_FAILURE).
    """
    # Process this as a really basic state machine (PassthroughVerificationStateMachine).
    pvsm = PassthroughVerificationStateMachine(
        packetId=packetId,
        bgcmd=bgcmd,
        icPacketBytes=icPacketBytes,
        maxSendCount=5,
        maxRecordsRequestCount=5
    )
    return pvsm.run()


# Counter so every KEY command gets a unique ID (these start at 1):
# (Key commands are those which are not part of the DFU data sequence)
KEY_MESSAGE_ID: int = 1


def pack_send_and_verify_key_bgapi_command(
    class_name: str,
    msg_name: str,
    args: Dict[str, Any]
) -> bool:
    """
    Packs a BGCommand with the given `class_name`, `msg_name`, and `args`,
    attempts to send it, and then makes sure it went through.
    This is used for sending key commands (i.e. ones that aren't directly part
    of the DFU binary upload process).

    Returns whether the send was successful.
    """
    # Build the command:
    global KEY_MESSAGE_ID
    bgcmd = bgapi.build_command(
        bgapi.BGAPI_WIFI_API, class_name, msg_name, args)
    packetBytes = compile_bgapi_cmd_as_icp(KEY_MESSAGE_ID, bgcmd)

    # Send it:
    progLogger.verbose(f'Sending {bgcmd} . . .')
    result = send_and_verify_bgapi_cmd(KEY_MESSAGE_ID, bgcmd, packetBytes)
    KEY_MESSAGE_ID += 1

    if result == PassthroughVerificationStateMachine.State.DONE:
        progLogger.success(f"Successfully sent {bgcmd}!")
        return True
    else:
        progLogger.error(
            f"Failed to send {bgcmd} b/c {result}. Check logs for more info."
        )
        return False


def attempt_dfu_flash(dfuChunks: List[DfuChunk]) -> bool:
    """Attempts to program the Radio with the given `DfuChunk`s.
    Returns whether or not it worked.
    """
    progLogger.notice("Beginning Flashing Procedure . . .")

    # First send a basic test command to make sure everything works:
    if not pack_send_and_verify_key_bgapi_command(
        'config', 'get_mac', {'hw_interface': 0}
    ):
        # If it didn't work (i.e. StateMachine couldn't fix it. We're done.
        return False

    # Start the fun. Enter DFU mode:
    if not pack_send_and_verify_key_bgapi_command(
        'dfu', 'reset', {'dfu': 1}
    ):
        # If it didn't work (i.e. StateMachine couldn't fix it. We're done.
        return False

    # Set the address to the start of program memory:
    if not pack_send_and_verify_key_bgapi_command(
        'dfu', 'flash_set_address', {'address': 0}
    ):
        # If it didn't work (i.e. StateMachine couldn't fix it. We're done.
        return False

    # Send every chunk:
    for chunk in dfuChunks:
        # Send it:
        progLogger.verbose(
            f'Sending chunk {chunk.packetId}: {chunk.bgcmd} . . .'
        )
        result = send_and_verify_bgapi_cmd(
            chunk.packetId, chunk.bgcmd, chunk.icPacketBytes
        )

        if result == PassthroughVerificationStateMachine.State.DONE:
            if (chunk.packetId+1) % 100:
                # Emit a success message for every 100 messages:
                n_chunks = chunk.packetId - \
                    APP_CONTEXT['file_packet_id_base'] + 1
                tot_chunks = len(dfuChunks)
                progLogger.success(
                    f"Successfully sent {n_chunks}/{tot_chunks} chunks."
                )
        else:
            progLogger.error(
                f"Sending failed at chunk {chunk.packetId} b/c {result}. "
                f"Check logs for more info."
            )
            return False

    # Flag that we're done sending chunks:
    if not pack_send_and_verify_key_bgapi_command(
        'dfu', 'flash_upload_finish', dict()
    ):
        # If it didn't work (i.e. StateMachine couldn't fix it. We're done.
        return False

    # Now that we're done, restart the device into normal operation:
    if not pack_send_and_verify_key_bgapi_command(
        'dfu', 'reset', {'dfu': 0}
    ):
        # If it didn't work (i.e. StateMachine couldn't fix it. We're done.
        return False

    # Seems like everything worked:
    return True


def wait_for_hercules():
    """Wait until we receive an Iris Common Packet from the ROVER (tells us
    Hercules is alive and talking). Want to check this before trying to talk to
    it.
    """
    return read_until_filter([PacketTypeFilter(IrisCommonPacket)], 100)


if __name__ == "__main__":
    progLogger.notice('Starting Iris BGAPI Passthrough Programmer . . .')

    progLogger.info(
        f'Loading and Compiling DFU file '
        f'{APP_CONTEXT["dfu_file_path"]} . . .'
    )
    dfuChunks, file_size = compile_dfu_file_to_icp(
        APP_CONTEXT["dfu_file_path"],
        APP_CONTEXT["dfu_chunk_size"]
    )
    # Compute total transmission size:
    total_tx_size = sum(len(fc.icPacketBytes) for fc in dfuChunks)
    n_chunks = len(dfuChunks)
    progLogger.verbose(
        f'\tCompiled {file_size}B into {len(dfuChunks)} chunks of '
        f'{APP_CONTEXT["dfu_chunk_size"]}B '
        f'with a total ICP transmission size of {total_tx_size}B.'
    )

    progLogger.info(
        f'Starting Serial using {APP_CONTEXT["serial_device_sn"]} at '
        f'{APP_CONTEXT["baud"]}baud . . .'
    )
    UDP_SLIP_XCVR = BasicSerialUdpSlipTransceiver(
        APP_CONTEXT["serial_device_sn"], APP_CONTEXT["baud"]
    )
    UDP_SLIP_XCVR.connect()

    # Make sure Hercules is talking to us before proceeding...
    progLogger.info(f'Waiting for data from Hercules . . .')
    herc_data = wait_for_hercules()
    if herc_data is None:
        progLogger.error(f"Haven't heard anything from Hercules. Aborting.")
    else:
        progLogger.success(f"Received data from Hercules.")

        # Flash the Radio!
        if attempt_dfu_flash(dfuChunks):
            progLogger.success("Radio Flashing Successful!")
        else:
            progLogger.error("Radio Flashing Failed. See logs for more info.")
