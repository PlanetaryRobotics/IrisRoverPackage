"""
Spies on the BGAPI comms between Hercules and the WF121.
Designed for use leading up to RC4.

@author: Connor W. Colombo (CMU)
@last-updated: 10/18/2022
"""

from serial.tools import list_ports, list_ports_common
from datetime import datetime
from dataclasses import dataclass, InitVar, field
from typing import Any, Final, Optional, Dict, cast, List
import asyncio
import threading
import socketserver
import serial_asyncio  # type: ignore
from multi_await import multi_await  # type: ignore
import scapy.all as scp  # type: ignore
from termcolor import cprint, colored

# Async Serial Writer Handle:
serial_writer: Optional[asyncio.StreamWriter] = None

# Application Context (settings, flags, etc. to be passed by reference to functions that need it):
app_context_default: Dict[str, Any] = {
    # Baud rate between Hercules and the WF121:
    'baud': 115200,
    # Serial devices to listen to:
    # SBC RX (connected to XCVR RX pin)
    'radio_tx_device': '/dev/cu.usbserial-A50285BI',
    # SBC TX (connected to XCVR RX pin)
    'herc_tx_device': '/dev/cu.usbserial-3'
}
DATETIME_FORMAT_STR: Final[str] = '%m-%d %H:%M:%S'

# Special message types that wrap primitives (so we can check type in Queue receiver):


@dataclass
class BgApiMessage:
    source_name: str
    time: datetime  # time *first* byte in message received
    data: bytes


@dataclass
class QueueMessage:
    msg: str


@dataclass
class QueueTick:
    count: int


@dataclass
class QueueByte:
    byte: bytes


class BgApiSerialTransceiver:
    source_name: str
    data_bytes: bytearray
    # Queue to push bytes into:
    packet_queue: asyncio.Queue
    # Queue to push non-telemetry messages into:
    message_queue: asyncio.Queue
    # Max time since first byte before a packet is considered complete and cut
    # off:
    packet_timeout_ms: float
    # Time of first byte in current packet:
    time_of_first_byte: datetime

    def __init__(self,
                 source_name: str,
                 packet_queue: asyncio.Queue,
                 message_queue: asyncio.Queue,
                 packet_timeout_ms: float = 10
                 ) -> None:
        self.source_name = source_name
        self.packet_queue = packet_queue
        self.message_queue = message_queue
        self.packet_timeout_ms = packet_timeout_ms
        self.time_of_first_byte = datetime.now()

        # Init state:
        self.data_bytes: bytearray = bytearray(b'')

    def append_byte(self, b: int) -> None:
        self.data_bytes.append(b)

    # Returns if we just completed a packet
    def parse_new_byte(self, newByte: bytes) -> bool:
        madeAPacket: bool = False
        now = datetime.now()

        if (now - self.time_of_first_byte).total_seconds() * 1000 > self.packet_timeout_ms:
            # This byte isn't part of the same packet as the last one. Emit a packet:
            if len(self.data_bytes) > 0:
                message = BgApiMessage(
                    source_name=self.source_name,
                    time=self.time_of_first_byte,
                    data=bytes(self.data_bytes)
                )
                self.packet_queue.put_nowait(message)
                madeAPacket = True

            # Reset the timer:
            self.time_of_first_byte = now

        b = int.from_bytes(newByte, 'big')
        self.append_byte(b)

        return madeAPacket


@dataclass
class SerialHw:
    source_name: str
    baud: int
    device: str
    reader: asyncio.StreamReader = field(init=False)
    writer: asyncio.StreamWriter = field(init=False)
    bgapi_xcvr: BgApiSerialTransceiver = field(init=False)
    ready: bool = False

    async def begin(self, message_queue: asyncio.Queue):
        # Attempts to start up serial connection and create async reader and
        # writer. Status messages are directed to `message_queue`.
        try:
            self.reader, self.writer = await serial_asyncio.open_serial_connection(url=self.device, baudrate=self.baud)
            self.ready = True
            message_queue.put_nowait(QueueMessage(
                f"{self.source_name} ({self.device}) Serial Connection Success!"
            ))
        except Exception as e:
            self.ready = False
            message_queue.put_nowait(QueueMessage(
                f"Failed to connect to {self.device} serial device. "
                f"Is the device name right? Check the connection? "
                f"Original error: {e}"
            ))

    async def stream_BGAPI(self, packet_queue: asyncio.Queue, message_queue: asyncio.Queue, app_context):
        # Stream BGAPI packets into the given Queue `packet_queue` from bytes received
        # on this serial interface.
        # Any error (etc.) messages that come up are put into `message_queue`.

        # Log of all bytes received in this packet:
        packet_bytes_log: bytes = b''

        # Only proceed if ready (or able to be made ready):
        if not self.ready:
            await self.begin(message_queue)
            if not self.ready:
                return  # Don't continue

        # Fire up the Transceiver:
        self.bgapi_xcvr = BgApiSerialTransceiver(
            self.source_name, packet_queue, message_queue
        )

        # Parse serial data as it arrives:
        while True:
            newByte = await self.reader.readexactly(1)
            madeAPacket = self.bgapi_xcvr.parse_new_byte(newByte)
            # Add to log:
            packet_bytes_log += newByte

            if madeAPacket:
                # Reset log no matter what (packet's done):
                packet_bytes_log = b''


async def ticker(tick_period_s: int, tick_queue):
    # Makes one tick every `tick_period_s` seconds.
    # Puts the current tick count into `tick_queue`.
    tick_count = 0
    while True:
        await asyncio.sleep(tick_period_s)
        tick_count += 1
        tick_queue.put_nowait(QueueTick(tick_count))


async def run_forever():
    # Does nothing, just keeps stuff open.
    while True:
        await asyncio.sleep(1)


bgapi_msg_colors: Final[List[str]] = ['cyan', 'magenta']
bgapi_source_colors: Dict[str, str] = dict()


def handle_async_bgapi(msg: BgApiMessage) -> None:
    """Handles a BGAPI message pushed asynchronously to a queue."""
    if msg.source_name not in bgapi_source_colors:
        # if we haven't seen this source before, pick a color for it:
        idx = len(bgapi_source_colors) % len(bgapi_msg_colors)
        bgapi_source_colors[msg.source_name] = bgapi_msg_colors[idx]

    color = bgapi_source_colors[msg.source_name]

    time_txt = colored(f"{msg.time.strftime(DATETIME_FORMAT_STR)}: ", 'blue')
    source_txt = colored(f" {msg.source_name.upper()} ",
                         'white', f'on_{color}', ['bold'])
    data_txt = colored(scp.hexdump(msg.data, dump=True), color)
    print(f"\n{time_txt} {source_txt} {len(msg.data)}B \n {data_txt}\n")


def handle_async_message(msg: str) -> None:
    """Handles a message pushed asynchronously to a queue."""
    msg_txt = colored(f"{msg}", 'blue')
    print(f"{msg_txt}")


async def console_main(app_context):
    # Main console drawing process that streams packets, bytes, and error messages and updates the screen whenever something comes in.
    # Uses the serial interface defined by the given settings.

    # Setup IPC Queues:
    bgapi_packet_queue = asyncio.Queue()
    message_queue = asyncio.Queue()
    tick_queue = asyncio.Queue()

    # Create Serial Listeners:
    herc_tx = SerialHw(
        'Hercules', app_context['baud'], app_context['herc_tx_device'])
    radio_tx = SerialHw(
        'Radio', app_context['baud'], app_context['radio_tx_device'])

    # Start tasks:
    tasks = [
        asyncio.create_task(herc_tx.stream_BGAPI(
            bgapi_packet_queue, message_queue, app_context)),
        asyncio.create_task(radio_tx.stream_BGAPI(
            bgapi_packet_queue, message_queue, app_context)),
        # this controls the min draw rate of the screen (i.e. how frequently we update if no packets or key presses are received)
        asyncio.create_task(ticker(1, tick_queue))
    ]

    async with multi_await() as m:
        m.add(bgapi_packet_queue.get)
        m.add(message_queue.get)
        m.add(tick_queue.get)
        m.add(run_forever)  # Does nothing but keep things open

        done = False
        while not done:
            results, failures = await m.get()

            assert failures == [None] * len(m.srcs)

            for r in results:
                if r is not None:
                    if isinstance(r, BgApiMessage):
                        handle_async_bgapi(cast(BgApiMessage, r))
                    elif isinstance(r, str):
                        handle_async_message(cast(str, r))
                    elif isinstance(r, QueueMessage):
                        handle_async_message(cast(QueueMessage, r).msg)
                    elif isinstance(r, QueueTick):
                        # Don't do anything special.
                        pass

        # Clean up if the above closes for some reason:
        [await t for t in tasks]


def start_console():
    # Starts the console, connecting to a serial device with the given serial number at the given baud rate.

    # Build serial settings struct to pass around:
    app_context = {
        **app_context_default
    }

    # Start the main process:
    asyncio.run(console_main(app_context))


if __name__ == "__main__":
    start_console()
