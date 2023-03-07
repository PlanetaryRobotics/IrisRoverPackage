# YAMCS Testing Script from Iris FM1+Peregrine Payload Functional Test (PFT) on Nov. 11 2022
# NOTE: Only wired telem was tested during this op.
# NOTE: For some reason this doesn't work with `tee` in bash, even when just using raw YAMCS (causes prints to only happen once every ~4mins)
# Running this normally (`pyenv exec python3 ./_pft_peregrine.py -u UNAME -p PASS` from inside WSL on OSIRIS works like a charm)

from yamcs.client import YamcsClient  # type: ignore
from yamcs.core.auth import Credentials as YamcsCredentials  # type: ignore

from typing import Final

import argparse

import asyncio
from dataclasses import dataclass
from multi_await import multi_await  # type: ignore
from serial.tools import list_ports, list_ports_common  # type: ignore
from termcolor import colored

from scripts.utils.trans_tools import *
from IrisBackendv3.codec.payload import EventPayload

import scapy.all as scp  # type: ignore

# Load Dependencies:
from IrisBackendv3.codec.settings import set_codec_standards
from IrisBackendv3.data_standards.prebuilt import add_to_standards, ALL_PREBUILT_MODULES
from IrisBackendv3.data_standards.logging import (
    logger as DsLogger,
    logger_setConsoleLevel as DsLoggerLevel
)
from IrisBackendv3.codec.magic import Magic
from IrisBackendv3.codec.payload import WatchdogCommandPayload, CommandPayload
from IrisBackendv3.codec.payload_collection import EnhancedPayloadCollection
from IrisBackendv3.codec.packet import parse_packet
from IrisBackendv3.codec.packet_classes.packet import Packet
from IrisBackendv3.codec.packet_classes.iris_common import IrisCommonPacket
from IrisBackendv3.codec.metadata import DataPathway, DataSource
from IrisBackendv3.data_standards import DataStandards
import scapy.all as scp
from termcolor import cprint

from scripts.utils.__command_aliases import get_command

TITLE: Final[str] = 'IRIS Lunar Rover —  FM1 Peregrine PFT YAMCS Listener — CLI'
IRIS_TELEM_PARAM_NAME: Final[str] = '/Peregrine/payloads/iris/iris-payload-tm-rs422'

# Re-build the `DataStandards` fresh from latest fprime and add in all prebuilt
# (WD Telem) Modules.
# These `DataStandards` will serve as TMTC definitions be used by the `Codec`
# layer used by the `Transceiver` layer to interpret packets.
# TODO: Rebuild this once in the main IPC module, cache it, and load from cache in other IPC apps.
DsLoggerLevel('CRITICAL')
standards = DataStandards.build_standards()
add_to_standards(standards, ALL_PREBUILT_MODULES)
set_codec_standards(standards)

parser = argparse.ArgumentParser(
    description=TITLE)


def get_opts():
    """
    Return settings wrapped in argparse.
    """
    parser.add_argument('-u', '--username', default='iris',
                        help='YAMCS Username')
    parser.add_argument('-p', '--password', default='not-a-real-password',
                        help='YAMCS Password')

    opts = parser.parse_args()
    return opts


def obtain_client(opts) -> YamcsClient:
    credentials = YamcsCredentials(
        username=opts.username, password=opts.password)
    return YamcsClient("plyamcs1.mission.local:443", credentials=credentials, tls=True, tls_verify=True)


@dataclass
class QueueMessage:
    msg: str


@dataclass
class QueueTick:
    count: int


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


def create_basic_param_printer(message_queue, param_name: str):
    def print_data(data) -> None:
        if data is not None:
            for parameter in data.parameters:
                message_queue.put_nowait(
                    colored(f'[{parameter.generation_time}]', 'green') +
                    colored(f' {param_name}:', 'blue') +
                    f' {parameter.raw_value} \t->\t {parameter.eng_value}'
                )
    return print_data


def create_packet_data_printer(message_queue):
    def print_packet_data(data) -> None:
        if data is not None:
            for parameter in data.parameters:
                t = parameter.generation_time
                packet_bytes = parameter.raw_value
                packet = parse_packet(packet_bytes)
                message_queue.put_nowait(
                    "\nP"+colored(f"[{t}])", 'green') +
                    colored(f" {packet}", 'magenta') +
                    f'\n{scp.hexdump(packet_bytes, dump=True)}\n'
                )
    return print_packet_data


async def main():
    print(TITLE)

    opts = get_opts()
    client = obtain_client(opts)

    message_queue = asyncio.Queue()
    tick_queue = asyncio.Queue()

    # Exercise non-websocket API:
    mdb = client.get_mdb(instance='Astrobotic-M1')
    params = [p for p in mdb.list_parameters()]
    print(f"Parameters: {[p.qualified_name for p in params]}")

    # Exercise websocket API:
    if len(params) == 0:
        return

    # Subscribe to every data parameter we have access to:
    processor = client.get_processor(
        instance='Astrobotic-M1', processor='realtime')
    for param in params:
        if param.qualified_name == IRIS_TELEM_PARAM_NAME:
            func = create_packet_data_printer(message_queue)
        else:
            func = create_basic_param_printer(
                message_queue, param.qualified_name)
        print(f"Subscribing to updates of parameter: {param.qualified_name}")
        processor.create_parameter_subscription(
            param.qualified_name, on_data=func)

    # Listen for data:
    tasks = [
        # this controls the base draw rate of the screen (i.e. how frequently we update if no packets or key presses are received)
        asyncio.create_task(ticker(1, tick_queue))
    ]

    async with multi_await() as m:
        m.add(message_queue.get)
        m.add(tick_queue.get)
        m.add(run_forever)  # Does nothing but keep things open

        done = False
        while not done:
            results, failures = await m.get()
            assert failures == [None] * len(m.srcs)

            for r in results:
                if r is not None:
                    if isinstance(r, QueueTick):
                        # Do nothing
                        pass
                    else:
                        print(r)

    # Clean up if the above closes for some reason:
    [await t for t in tasks]


def run_yamcs():
    print('Booting . . .')
    asyncio.run(main())


if __name__ == "__main__":
    run_yamcs()
    print("Closing.")
