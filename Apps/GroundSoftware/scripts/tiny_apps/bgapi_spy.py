"""
Spies on the BGAPI comms between Hercules and the WF121.
Designed for use leading up to RC4.

@author: Connor W. Colombo (CMU)
@last-updated: 10/19/2022
"""

from asyncio import events
import os
from serial.tools import list_ports, list_ports_common  # type: ignore
from datetime import datetime
from dataclasses import dataclass, InitVar, field
from typing import Any, Final, Optional, Dict, cast, Union, List
import asyncio
import threading
import socketserver
import serial_asyncio  # type: ignore
from multi_await import multi_await  # type: ignore
import scapy.all as scp  # type: ignore
from termcolor import cprint, colored
import logging

import IrisBackendv3.codec.bgapi as bgapi

# Async Serial Writer Handle:
serial_writer: Optional[asyncio.StreamWriter] = None

# Application Context (settings, flags, etc. to be passed by reference to functions that need it):
app_context_default: Dict[str, Any] = {
    # Baud rate between Hercules and the WF121:
    'baud': 115200,
    # Serial devices to listen to:
    # SBC RX (connected to XCVR RX pin)
    'radio_tx_device': '/dev/cu.usbserial-3',
    # 'radio_tx_device': '/dev/cu.usbserial-A50285BI',
    # SBC TX (connected to XCVR RX pin)
    # 'herc_tx_device': '/dev/cu.usbserial-3',
    'herc_tx_device': '/dev/cu.usbserial-A50285BI',
    'listen_to_herc_tx': True,
    'listen_to_radio_tx': False
}
DATETIME_FORMAT_STR: Final[str] = '%m-%d %H:%M:%S.%f'

# Special message types that wrap primitives (so we can check type in Queue receiver):


logger = logging.getLogger(__file__)
file_log_handler = logging.FileHandler('___bgapi_spy.log')
logger.addHandler(file_log_handler)

stderr_log_handler = logging.StreamHandler()
logger.addHandler(stderr_log_handler)

# nice output format
formatter = logging.Formatter(
    '[%(levelname)s] %(message)s')
file_log_handler.setFormatter(formatter)
stderr_log_handler.setFormatter(formatter)
logging.getLogger().setLevel(logging.DEBUG)
logger.info("BOOT")


@dataclass
class BgApiMessage:
    source_name: str
    time: datetime  # time *first* byte in message received
    msg: Union[bgapi.BGCommand,
               bgapi.BGResponse, bgapi.BGEvent]


@dataclass
class QueueMessage:
    msg: str


@dataclass
class QueueTick:
    count: int


@dataclass
class QueueByte:
    byte: bytes


def make_bgapi_msg_handler(source_name: str, bgapi_message_queue: asyncio.Queue):
    def handler(msg: bgapi.BGMsg):
        bgapi_message_queue.put_nowait(BgApiMessage(
            source_name=source_name,
            time=datetime.fromtimestamp(msg._timestamp),
            msg=msg
        ))
    return handler


@dataclass
class BgApiSerial:
    source_name: str
    baud: int
    device: str
    # False if the BGAPI device (e.g. WF121), True if the host (Hercules):
    is_bgapi_host: bool
    bgapi_message_queue: asyncio.Queue
    message_queue: asyncio.Queue
    bgapi_lib: bgapi.BGLibListener = field(init=False)
    ready: bool = False

    def __post_init__(self):
        # Attempts to start up an async serial connection using BGLib:
        try:
            self.bgapi_lib = bgapi.BGLibListener(
                bgapi.SerialConnector(self.device),
                bgapi.BGAPI_WIFI_DEF,
                make_bgapi_msg_handler(
                    self.source_name, self.bgapi_message_queue
                ),
                listening_to_host=self.is_bgapi_host
            )
            # Fire up the Transceiver:
            self.ready = True
            self.message_queue.put_nowait(QueueMessage(
                f"{self.source_name} ({self.device}) BGAPI Serial Connection Success!"
            ))
        except Exception as e:
            self.ready = False
            self.message_queue.put_nowait(QueueMessage(
                f"Failed to connect to {self.device} serial device using BGLib "
                f"for {self.source_name}. "
                f"Is the device name right? Check the connection? "
                f"Original error: {e}"
            ))

    def begin(self):
        if self.ready:
            # Fire up the Transceiver:
            self.bgapi_lib.open()


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
    source_txt = colored(f" {msg.source_name.upper()}-{msg.msg.__class__.__name__} ",
                         'white', f'on_{color}', ['bold'])
    data_txt = colored(str(msg.msg), color)
    logger.info(f"\n{time_txt} {source_txt} {data_txt}")


def handle_async_message(msg: str) -> None:
    """Handles a message pushed asynchronously to a queue."""
    msg_txt = colored(f"{msg}", 'blue')
    logger.debug(f"{msg_txt}")


async def console_main(app_context):
    # Main console drawing process that streams packets, bytes, and error messages and updates the screen whenever something comes in.
    # Uses the serial interface defined by the given settings.

    # Setup IPC Queues:
    bgapi_message_queue = asyncio.Queue()
    message_queue = asyncio.Queue()
    tick_queue = asyncio.Queue()

    # Create Serial Listeners:
    herc_tx = BgApiSerial(
        'Hercules', app_context['baud'], app_context['herc_tx_device'],
        is_bgapi_host=True,
        bgapi_message_queue=bgapi_message_queue,
        message_queue=message_queue)
    radio_tx = BgApiSerial(
        'Radio', app_context['baud'], app_context['radio_tx_device'],
        is_bgapi_host=False,
        bgapi_message_queue=bgapi_message_queue,
        message_queue=message_queue)

    # Start listeners:
    if app_context['listen_to_herc_tx']:
        herc_tx.begin()
    if app_context['listen_to_radio_tx']:
        radio_tx.begin()

    # Start tasks:
    tasks = [
        # this controls the min draw rate of the screen (i.e. how frequently we update if no packets or key presses are received)
        asyncio.create_task(ticker(1, tick_queue))
    ]

    async with multi_await() as m:
        m.add(bgapi_message_queue.get)
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
    # Clean up listeners:
    if app_context['listen_to_herc_tx']:
        herc_tx.bgapi_lib.close()
    if app_context['listen_to_radio_tx']:
        radio_tx.bgapi_lib.close()


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
