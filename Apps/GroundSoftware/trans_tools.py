#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Prototyping toolkit for building the Transceiver layer which allows for running
tests which require the Transceiver layer while the real thing is still being
built.

@author: Connor W. Colombo (CMU)
@last-updated: 08/28/2022
"""

import logging
import coloredlogs  # type: ignore # mypy doesn't see type hints
import traceback
from typing import Any, Final, List, Type, cast, Union, Dict, Tuple, Optional

import re
import struct
import math
import os
import pickle
from datetime import datetime, timedelta

import socket
import serial  # type: ignore # no type hints
import scapy.all as scp  # type: ignore # no type hints
import numpy as np
import pandas as pd
from tabulate import tabulate
import matplotlib.pyplot as plt  # type: ignore # no type hints
import ulid
import itertools
import textwrap
from collections import deque

from termcolor import cprint

from IrisBackendv3.utils.basic import print_bytearray_hex as printraw
from IrisBackendv3.utils.basic import bytearray_to_spaced_hex as hexstr
from IrisBackendv3.utils.nameiddict import NameIdDict

from IrisBackendv3.data_standards import DataStandards
from IrisBackendv3.data_standards.fsw_data_type import FswDataType
from IrisBackendv3.data_standards.logging import logger as DsLogger
from IrisBackendv3.data_standards.prebuilt import add_to_standards, watchdog_heartbeat_tvac, watchdog_heartbeat, watchdog_command_response, watchdog_detailed_status_heartbeat
from IrisBackendv3.codec.payload import Payload, TelemetryPayload, CommandPayload, WatchdogCommandPayload
from IrisBackendv3.codec.payload_collection import EnhancedPayloadCollection, extract_downlinked_payloads
from IrisBackendv3.codec.packet import Packet, IrisCommonPacket, WatchdogTvacHeartbeatPacket, WatchdogHeartbeatPacket, WatchdogCommandResponsePacket, WatchdogDetailedStatusPacket
from IrisBackendv3.codec.packet import parse_packet as core_parse_packet
from IrisBackendv3.codec.exceptions import PacketDecodingException
from IrisBackendv3.codec.metadata import DataPathway, DataSource
from IrisBackendv3.codec.magic import Magic, MAGIC_SIZE
from IrisBackendv3.codec.logging import logger as CodecLogger
from IrisBackendv3.codec.settings import ENDIANNESS_CODE, set_codec_standards

import seaborn as sns  # type: ignore
sns.set()

settings: Dict[str, Union[str, int]] = {
    'SAVE_DIR': './transceiver_logs/',
    'PACKET_PRINTING_DIR': './packet_prints/',
    'SAVE_FILE_PREFIX': 'iris_logs',
    'SAVE_FILE_EXT': 'tsc',
    # number of minutes after which the old save file won't be overwritten and a new one will be made:
    'NEW_SAVE_PERIOD': 300
}

ser: Any = None

transLogger: Optional[Any] = None

# Set up a logger which both prints to console and logs to file:

DATETIME_FORMAT_STR: Final[str] = '%m-%d %H:%M:%S'
def setup_logger(name: str):
    # set up logging to file - see previous section for more details
    logging.basicConfig(level=logging.DEBUG,
                        format='%(asctime)s %(name)-12s %(levelname)-8s %(message)s',
                        datefmt=DATETIME_FORMAT_STR,
                        filename=f'./raw-console-logs/{name}.clog',
                        filemode='a'  # keep adding data to log, don't overwrite
                        )
    # define a Handler which writes INFO messages or higher to the sys.stderr
    console = logging.StreamHandler()
    console.setLevel(logging.INFO)
    # set a format which is simpler for console use
    FORMAT = '%(asctime)-15s: %(levelname)-8s %(message)s'
    formatter = logging.Formatter(FORMAT)
    # tell the handler to use this format
    console.setFormatter(formatter)
    # add the handler to the root logger
    logging.getLogger().addHandler(console)

    # Now, we can log to the root logger, or any other logger. First the root...
    global transLogger
    transLogger = logging.getLogger(__name__)
    coloredlogs.install(logger=transLogger, level='DEBUG', fmt=FORMAT)


def log_print(*args, **kwargs):
    if transLogger is None:
        print(*args, **kwargs)
    else:
        transLogger.info(*args, **kwargs)


def err_print(*args, **kwargs):
    if transLogger is None:
        cprint(*args, 'red', **kwargs)
    else:
        transLogger.error(*args, **kwargs)


DsLogger.setLevel('CRITICAL')
standards = DataStandards.build_standards()
add_to_standards(standards, [
    watchdog_detailed_status_heartbeat,
    watchdog_heartbeat_tvac,
    watchdog_heartbeat,
    watchdog_command_response
])
set_codec_standards(standards)

all_payloads: EnhancedPayloadCollection = EnhancedPayloadCollection()
telemetry_streams: NameIdDict[List[Tuple[datetime, Any]]] = NameIdDict()
# Dataframes used for tabular printing of highlevel info about data received:
USE_LOG_DATAFRAMES: Final[bool] = True
telemetry_payload_log_dataframe: pd.DataFrame = pd.DataFrame(columns=['Opcode', 'Module', 'Channel', 'Num. Received', 'Last Updated', 'Current Value', 'H+1', 'H+2', 'H+3'], dtype=object).set_index('Opcode')
packet_log_dataframe: pd.DataFrame = pd.DataFrame(columns=['Packet Type', 'Num. Received', 'Last Updated', 'Avg. Interval [s]', 'Current Interval [s]', 'Bytes Received', 'Avg. bits/sec', 'Avg. bits/sec w/CCSDS'], dtype=object).set_index('Packet Type')
# LiFo Queue Log of the string prints of the most recent packets received that don't contain telemetry (i.e. logs, events, debug prints, etc.):
nontelem_packet_prints: deque = deque(maxlen=50)


def parse_ip_udp_packet(packet_bytes: bytes, deadspace: int = 0) -> Optional[Packet]:
    """Parses an IP/UDP packet and extracts the meaningful payload.

    Args:
        packet_bytes (bytes): Raw data.
        deadspace (int, optional): Number of bytes to ignore at beginning of Raw section (due to some fault). Defaults to 0.

    Returns:
        Optional[Packet]: `Packet` if parsing succeeded.
    """
    # Convert into a full packet:
    full_packet = scp.IP(packet_bytes)
    # Scrape off the IP/UDP and keep the Raw:
    core_data = scp.raw(full_packet.getlayer(scp.Raw))[deadspace:]
    # Parse like payload:
    return parse_packet(core_data)


def parse_packet(
    packet_bytes: bytes,
    codecs: Optional[List[Type[Packet]]] = None
) -> Packet:
    packet = core_parse_packet(packet_bytes, codecs)
    all_payloads.extend(packet.payloads)
    return packet


def list_commands_wd() -> None:
    """
    List all available Watchdog commands for TVAC testing.
    """
    def p_command(i, x): return cprint(
        f"\n\t{i}.\t'{x.name}'{f', args: {x.args}' if len(x.args) > 0 else ''}", 'blue', 'on_grey', attrs=['bold'])

    def p_arg(a): return cprint(
        f"\n\t\t\tAvailable values for `{a.name}`:", 'cyan')

    def p_enum(x): return cprint(
        f"\n\t\t\t\t'{x.name}' or {x.value} or {hex(x.value)}", 'magenta')

    module = standards.modules['WatchDogInterface']
    for i, command in enumerate(module.commands.vals):
        p_command(i, command)
        for arg in command.args:
            if len(arg.enum) > 0:
                p_arg(arg)
                for e in arg.enum:
                    p_enum(e)


def list_channels_wd() -> None:
    """
    List all available Watchdog channels for TVAC testing.
    """
    def p_channel(i, x): return cprint(
        f"\n\t{i}.\tChannel '{x.name}': {x.datatype.value}", 'red', 'on_grey', attrs=['bold'])

    module = standards.modules['WatchdogHeartbeatTvac']
    for i, channel in enumerate(module.telemetry.vals):
        p_channel(i, channel)


def pack_watchdog_command(command_name: str, **kwargs) -> bytes:
    """
    Manually pack up a Watchdog command into bytes to send.
    """
    module, command = standards.global_command_lookup(command_name)
    payload = WatchdogCommandPayload(
        pathway=DataPathway.WIRED,
        source=DataSource.GENERATED,
        magic=Magic.WATCHDOG_COMMAND,
        module_id=module.ID,
        command_id=command.ID,
        args=kwargs
    )

    VLP = Magic.WATCHDOG_COMMAND.encode() + payload.encode()

    vlp_len = len(VLP)

    checksum = 0  # ignore for now TODO: impl. me
    seq_num = 0  # watchdog doesn't care
    CPH = struct.pack(ENDIANNESS_CODE + 'B H B', seq_num, vlp_len, checksum)
    packet = CPH + VLP
    return packet


def build_command_packet(
    seq_num: int,
    pathway: DataPathway,
    magic: Magic,
    command_name: str,
    kwargs,
    source: DataSource = DataSource.GENERATED
) -> IrisCommonPacket:
    """
    Builds a Command Payload Packet given the specified command package data
    (source, pathway, magic type, command name and kwargs).
    """

    command_payload_type = {
        Magic.WATCHDOG_COMMAND: WatchdogCommandPayload,
        Magic.COMMAND: CommandPayload
    }[magic]

    module, command = standards.global_command_lookup(command_name)

    payloads = EnhancedPayloadCollection(
        CommandPayload=[
            command_payload_type(
                pathway=pathway,
                source=source,
                magic=magic,
                module_id=module.ID,
                command_id=command.ID,
                args=kwargs
            )
        ]
    )
    packet = IrisCommonPacket(
        seq_num=seq_num,
        payloads=payloads
    )
    return packet


def connect_serial(device: str = '/dev/ttyUSB0', baud: int = 9600) -> None:
    """
    Open a serial port connection.
    """
    global ser
    try:
        ser = serial.Serial(device, baud)  # open serial port
        cprint("Connection Success!", 'green')
    except serial.SerialException as e:
        cprint(
            f"Failed to connect to serial device. Is the device name right? Check the connection? Original error: {e}",
            'red')


def send_wifi(data: bytes, ip="192.168.1.2", port=8080) -> None:
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.sendto(data, (ip, port))
    sock.close()


def send_slip(dat: bytes) -> None:
    """
    Wraps the given data in SLIP and sends it over RS422.
    """
    buf = bytearray(b'')
    buf.append(0xC0)

    for d in dat:
        if d == 0xC0:
            buf.append(0xDB)
            buf.append(0xDC)
        elif d == 0xDB:
            buf.append(0xDB)
            buf.append(0xDD)
        else:
            buf.append(d)

    buf.append(0xC0)

    if ser is not None:
        ser.write(bytes(buf))
    else:
        cprint(
            "Can't send data, serial connection not started. Try `connect_serial()`.",
            'red')


def send_data_wd_serial(
    raw_data: bytes,
    ip_dest: str = '127.0.0.1',  # arbitrary (WD doesn't care)
    ip_src: str = '222.173.190.239',  # arbitrary (WD doesn't care)
    port: int = 8080  # arbitrary (WD doesn't care)
) -> str:
    try:
        # Build packet
        full_packet = scp.IP(dst=ip_dest, src=ip_src) / \
            scp.UDP(dport=port)/scp.Raw(load=raw_data)
        # printraw(scp.raw(scp.IP(scp.raw(full_packet))))
        # printraw(scp.raw(full_packet))
        data = cast(bytes, scp.raw(full_packet))
        send_slip(data)
    except serial.SerialTimeoutException:
        cprint("Failed to send due to serial timeout. Check the connection?", 'red')
    finally:
        return f"Data: {hexstr(data)}."


def send_command_wd_serial(command_name: str, **kwargs) -> str:
    data = pack_watchdog_command(command_name, **kwargs)
    return send_data_wd_serial(data)


def send_packet(
    packet: Packet,
    pathway: DataPathway,
    ip: Optional[str] = None,
    port: Optional[str] = None
) -> None:
    """
    Sends the given Packet over the given DataPathway.
    """
    if pathway == DataPathway.WIRED:
        send_data_wd_serial(packet.encode())
    elif pathway == DataPathway.WIRELESS:
        send_wifi(packet.encode(), ip=ip, port=port)


def adc_to_degC(adc: int) -> float:
    tab = WatchdogTvacHeartbeatPacket.CustomPayload.THERMISTOR_LOOKUP_TABLE
    return float(np.interp(adc, tab['Vadc'][::-1], tab['degC'][::-1]))


def adc_to_degK(adc: int) -> float:
    return adc_to_degC(adc) + 273.15


def degC_to_adc(tempC: float) -> int:
    tab = WatchdogTvacHeartbeatPacket.CustomPayload.THERMISTOR_LOOKUP_TABLE
    return int(np.interp(tempC, tab['degC'], tab['Vadc']))


def degK_to_adc(tempK: float) -> int:
    return degC_to_adc(tempK - 273.15)


def update_telemetry_streams(packet: Packet, auto_cache=True) -> None:
    """
    Add all extracted data values in the packet to their streams:
    """
    # Don't include WatchdogDetailedStatusPackets in the Dataframe since they're *very* detailed
    # (contain way too much data to display)
    update_telemetry_streams_from_payloads(
        packet.payloads,
        auto_cache=auto_cache,
        include_in_dataframe = not isinstance(packet, WatchdogDetailedStatusPacket)
    )

def update_telemetry_streams_from_payloads(payloads: EnhancedPayloadCollection, auto_cache=True, include_in_dataframe: bool = True):
    """
    Updates the `telemetry_streams` from an EnhancedPayloadCollection.
    """
    before = telemetry_payload_log_dataframe.copy()
    for t in payloads[TelemetryPayload]:
        # If this payload's channel is new (previously un-logged), add it:
        t = cast(TelemetryPayload, t)
        if t.opcode not in telemetry_streams:
            telemetry_streams[t.opcode, t.module.name+'_'+t.channel.name] = [
                (datetime.now(), t.data)
            ]
        else:  # otherwise, update:
            telemetry_streams[t.opcode].append((datetime.now(), t.data))

        # Update the dataframe (used for tabular printing):
        if USE_LOG_DATAFRAMES and include_in_dataframe:
            # Format value (if enum):
            if t.channel.datatype == FswDataType.ENUM:
                if isinstance(t.data, str):
                    val = f"{t.data}[{t.channel.get_enum_value(t.data)}]"
                else:
                    val = f"{t.channel.get_enum_name(t.data)}[{t.data}]"
            elif type(t.data) == float:
                val = f"{t.data:.3f}"
            else:
                val = t.data

            # Create row:
            if t.opcode in telemetry_payload_log_dataframe.index:
                # Row already exists for this field, just update it (shifting older data over):
                old_row = telemetry_payload_log_dataframe.loc[t.opcode]
                new_data = {
                    'Module': t.module.name,
                    'Channel': t.channel.name,
                    'Num. Received': old_row['Num. Received'] + 1,
                    'Last Updated': datetime.now(),
                    'Current Value': val,
                    'H+1': old_row['Current Value'],
                    'H+2': old_row['H+1'],
                    'H+3': old_row['H+2']
                }
            else:
                new_data = {
                    'Module': t.module.name,
                    'Channel': t.channel.name,
                    'Num. Received': 1,
                    'Last Updated': datetime.now(),
                    'Current Value': val,
                    'H+1': np.nan,
                    'H+2': np.nan,
                    'H+3': np.nan
                }

            # Save row:
            telemetry_payload_log_dataframe.loc[t.opcode, [*new_data.keys()]] = [*new_data.values()]

    # Save the updated streams:
    if auto_cache:
        cache()


def update_packet_log_dataframe_row(row_name: str, now: datetime, num_bytes: int) -> None:
    # Update the given row in the packet log dataframe (used for tabular printing) for a packet
    # containing `num_bytes` received at time `now`.
    if row_name in packet_log_dataframe.index: # 'Packet Type', 
        # Row already exists for this field, just update it:
        old_row = packet_log_dataframe.loc[row_name]
        total_num_recv = old_row['Num. Received'] + 1
        current_interval = (now - old_row['Last Updated']).total_seconds()
        avg_interval = (((old_row['Num. Received']-1) * old_row['Avg. Interval [s]']) + current_interval) / old_row['Num. Received']
        total_bytes_recv = old_row['Bytes Received'] + num_bytes
        new_data = {
            'Last Updated': now,
            'Num. Received': total_num_recv,
            'Avg. Interval [s]': avg_interval,
            'Current Interval [s]': current_interval,
            'Bytes Received': total_bytes_recv,
            'Avg. bits/sec':  total_bytes_recv / total_num_recv / avg_interval,
            'Avg. bits/sec w/CCSDS': (total_bytes_recv + total_num_recv*14*8) / total_num_recv / avg_interval
        }
    else:
        new_data = {
            'Last Updated': now,
            'Num. Received': 1,
            'Avg. Interval [s]': 0,
            'Current Interval [s]': 0,
            'Bytes Received': num_bytes,
            'Avg. bits/sec': 0,
            'Avg. bits/sec w/CCSDS': 0
        }
    packet_log_dataframe.loc[row_name, [*new_data.keys()]] = [*new_data.values()]


def update_packet_log_dataframe(packet: Packet) -> None:
    # Update packet log dataframe (used for tabular printing) after
    # receiving the given packet:
    if isinstance(packet, Packet):
        # Update the dataframe for this packet:
        now = datetime.now()
        name = packet.__class__.__name__
        num_bytes =  len(packet.raw if packet.raw is not None else packet.encode())
        update_packet_log_dataframe_row(name, now, num_bytes)

        # Update total across all packets:
        update_packet_log_dataframe_row('zTotal', now, num_bytes)




# Pretty-Formats Telemetry Payload Log Dataframe:
def str_telemetry_payload_log_dataframe() -> str:
    df_out = telemetry_payload_log_dataframe.copy()
    df_out.index = df_out.index.map(lambda x: f"0x{x:04X}")
    return tabulate(df_out.fillna('').sort_index(ascending=True), headers='keys', tablefmt='fancy_grid', numalign='right', stralign='right')

# Pretty-Formats Packet Log Dataframe:
def str_packet_log_dataframe() -> str:
    return tabulate(packet_log_dataframe.fillna('').sort_index(ascending=True), headers='keys', tablefmt='fancy_grid', numalign='right', stralign='right')

# Pretty-Formats all Log DataFrames with Nice Formatting into one big string:
def str_log_dataframes() -> str:
    def tabs2spaces(x: str) -> str:
        # tabs screw up spacing calculation. 
        # just replace with appropriate amount of spaces, accounting for tabstop:
        return x.expandtabs(4)
    def remove_ansi_escape_codes(x: str) -> str:
        # removes any ansi escape codes from given string:
        return re.sub('\033\[[^m]*m', '', x)
    def len_noCodes(x: str) -> int:
        # returns the length of the given string, excluding any escape codes in it:
        return len(remove_ansi_escape_codes(x))

    # Settings:
    horiz_padding = tabs2spaces("\t\t")
    term_cols, term_lines = os.get_terminal_size()
    # Adjust by subtracing bottom and right side buffers (from experimentation):
    term_lines = term_lines - 1
    term_cols = term_cols - 8

    ### Build each section:
    ## Build the left side:
    # Telemetry Table:
    telem_view = (
        "\n\nTelemetry:\n"
        +str_telemetry_payload_log_dataframe()
    )
    telem_view_lines = [tabs2spaces(x) for x in telem_view.split('\n')]
    telem_view_width = max(len(l) for l in telem_view_lines)
    telem_view_lines = [l.ljust(telem_view_width, ' ') for l in telem_view_lines]

    # Build the left-hand side (just telem view):
    left_side_lines = telem_view_lines
    left_side_max_width = telem_view_width

    ## Build the right side:
    right_side_max_width = (term_cols-1) - left_side_max_width
    # Packets Table:
    packets_view = (
        "\n\nPackets:\n"
        +str_packet_log_dataframe()
    )
    packets_view_lines = [tabs2spaces(x) for x in packets_view.split('\n')]

    # Non-Telemetry Packets log:
    # Get the boundaries of the area we're going to squeeze this into:
    plog_header_lines = [' ', ' ', 'Non-Telemetry Packets:', '---------------------------']
    plog_footer_lines = [' ']
    plog_width = right_side_max_width
    plog_max_len = term_lines - len(plog_header_lines) - len(packets_view_lines) - len(plog_footer_lines)
    # Build the raw (ideal / un-squeezed) output:
    plog_raw = tabs2spaces('\n\n'.join([tabs2spaces(x).strip() + "\033[0m" for x in nontelem_packet_prints])) # add escape close to ensure end of formatting for each line
    plog_lines = plog_raw.split('\n')
    # Wrap each line to the width:
    plog_line_wraps = []
    for i, line in enumerate(plog_lines):
        if line.strip() == '':
            # if the line is just whitespace and that's it, keep it
            # (it's intentional but would get wiped out by textwrap.wrap)
            # just tack on a clump of the line instead:
            plog_line_wraps.append([line])
        else:
            # otherwise, it contains text, so we wrap it like normal:
            plog_line_wraps.append(textwrap.wrap(line, width=plog_width, replace_whitespace=False, break_on_hyphens=False, tabsize=4))
    plog_lines = [line for clump in plog_line_wraps for line in clump] # Reflatten
    
    # Grab all lines up to the limit and then tack on header and footer:
    plog_lines = plog_header_lines + plog_lines[:plog_max_len] + plog_footer_lines

    # Build the right-hand side (packets_view on top of (non-telemetry) packets log):
    right_side_lines = packets_view_lines + plog_lines

    # Make sure it's fixed width:
    right_side_lines = [l.ljust(right_side_max_width, ' ') for l in right_side_lines]
    
    # Add notice to the upper right:
    notice_lines = ['\033[231;41;1m Iris Lunar Rover Telemetry Viewer \033[0m', '(use in full-size terminal) ']
    notice_width = max(len_noCodes(l) for l in notice_lines)
    notice_lines = [l.rjust(notice_width, ' ') for l in notice_lines]
    for i, line in enumerate(notice_lines):
        right_side_lines[i] = right_side_lines[i][:-len_noCodes(line)] + line

    # Make sure left side has at least as many lines as right side:
    if len(right_side_lines) > len(left_side_lines):
        # add extra lines that match the max width:
        left_side_lines += [' ' * left_side_max_width] * (len(right_side_lines) - len(left_side_lines))

    ### Join left and right sides:
    all_lines = '\n'.join([a + horiz_padding + b for a,b in itertools.zip_longest(left_side_lines, right_side_lines, fillvalue="")]) + "\033[0m" # make sure formatting is reset at the end

    return all_lines

# Clears the Console Window:
def clear_console() -> None:
    os.system('cls' if os.name in ('nt', 'dos') else 'clear')

# Refreshes (clears) the Console Window with Highlevel Telemetry Dataview:
def refresh_telemetry_dataview() -> None:
    # Build the string before clearing the screen to minimize off-time:
    data = str_log_dataframes()
    clear_console()
    print(data)


def plot_stream(channel_name: str, title: str = 'Iris Payload', auto_show: bool = True) -> None:
    if len(telemetry_streams) == 0:
        cprint(
            f"There is no logged data matching this file pattern to plot: "
            f"`prefix`: '{settings['SAVE_FILE_PREFIX']}', `extension`: "
            f"'{settings['SAVE_FILE_EXT']}' in `directory`: '{settings['SAVE_DIR']}' .",
            'yellow')
    else:
        try:
            stream = telemetry_streams[channel_name]
            ts = [t for t, _ in stream]
            vs = [v for _, v in stream]
            plt.figure()
            plt.plot(ts, vs,
                     color='lightskyblue',
                     marker='o',
                     markeredgecolor='steelblue',
                     markerfacecolor='ghostwhite',
                     linestyle='-',
                     linewidth=2,
                     markersize=3
                     )
            plt.gcf().autofmt_xdate()
            plt.xlabel('Time')
            plt.ylabel(channel_name)
            plt.title(title)
            if auto_show:
                plt.show()
        except KeyError:
            cprint(
                f"Given channel name `{channel_name}` is invalid. Valid channel "
                f"names are: {telemetry_streams.names} .", 'yellow'
            )


def get_latest_cache_file() -> Tuple[str, ulid.ulid.ULID]:
    """
    Returns the path to the most recent cache_file which meets the specs in
    settings, along with the ULID of that latest file.
    """
    cache_dir = str(settings['SAVE_DIR'])
    filename_base = str(settings['SAVE_FILE_PREFIX'])
    ext = str(settings['SAVE_FILE_EXT'])
    # Grab all files in dir with extension:
    files = os.listdir(cache_dir)
    files = [f for f in files if f.endswith('.'+ext)]
    if len(files) == 0:
        raise FileNotFoundError(
            f"No save files with given extension {ext} found "
            f"in given directory {cache_dir}."
        )
    # Filter for those with desired prefix:
    files = [f for f in files if f.startswith(filename_base)]
    if len(files) == 0:
        raise FileNotFoundError(
            f"No save files with given extension {ext} which "
            f"start with the given `filename_base` '{filename_base}' found "
            f"in given directory {cache_dir}."
        )

    # Grab file with the latest ulid:
    latest_file: Tuple[str, ulid.ULID]  # name, ulid
    for i, f in enumerate(files):
        # Extract the ulid:
        ulid_str = f.replace('.'+ext, '').split('_')[-1]
        try:
            u = ulid.from_str(ulid_str)
        except ValueError as e:
            raise ValueError(
                f"Unable to rebuild the ULID for save file cache "
                f"file '{f}'. Extracted ULID string was '{ulid_str}' "
                f"but appears to be an invalid ulid. Is this really a "
                f"DS cache file? Is it from an old version? Has the file "
                f"been renamed?"
                f"\n Original ValueError from `ulid-py`: {e}"
            )

        # Store this as the latest file if it's the first file or its
        # ulid is chronologically more recent that the previous `latest_file`:
        if i == 0:
            latest_file = f, u
        elif u > latest_file[1]:
            latest_file = f, u

    cache_filename = latest_file[0]

    # Build & return the file path:
    return os.path.join(cache_dir, cache_filename), latest_file[1]


def cache() -> None:
    """
    Cache into a unique file in `cache_dir`.

    Returns the unique filename.
    """
    cache_dir = str(settings['SAVE_DIR'])
    filename_base = str(settings['SAVE_FILE_PREFIX'])
    ext = str(settings['SAVE_FILE_EXT'])
    # Make sure directory exists:
    if not os.path.exists(cache_dir):
        os.makedirs(cache_dir)

    # Grab the most recent save file to compare times:
    u_new = ulid.from_timestamp(datetime.now())
    try:
        path0, u0 = get_latest_cache_file()

        if u_new.timestamp().datetime < (u0.timestamp().datetime + timedelta(minutes=float(settings['NEW_SAVE_PERIOD']))):
            # If the new-save period hasn't passed yet, keep the old file name and override it:
            path = path0
        else:
            path = os.path.join(cache_dir, f'{filename_base}_{u_new}.{ext}')

    except FileNotFoundError:
        # There is no old file, so make a new one:
        path = os.path.join(cache_dir, f'{filename_base}_{u_new}.{ext}')

    # Save in a new file
    with open(path, 'wb') as file:
        pickle.dump(telemetry_streams, file)


def load_cache() -> None:
    """
    **Overrides** the contents of the current `telemetry_stream` with one loaded
    from the latest cache file. Consider saving first.
    """
    cache_dir = str(settings['SAVE_DIR'])
    filename_base = str(settings['SAVE_FILE_PREFIX'])
    ext = str(settings['SAVE_FILE_EXT'])
    try:
        path, _ = get_latest_cache_file()

        # Open the file:
        with open(path, 'rb') as file:
            data = pickle.load(file)

        telemetry_streams.update(data)
    except FileNotFoundError:
        pass  # Do nothing. This is the first go, there's just nothing to load.


# Creates a "Print" string of the given packet, along with accompanying metadata like the current time:
def packet_print_string(packet: Optional[Packet]) -> str:
    return f"\033[35;47;1m({datetime.now().strftime(DATETIME_FORMAT_STR)})\033[0m {packet!s}"

# Saves the given printout of the given packet into the current packet_prints log file:
def save_packet_to_packet_prints(packet: Optional[Packet]) -> None:
    if packet is not None:
        # Build the path to save to:
        dir = str(settings['PACKET_PRINTING_DIR'])
        filename_base = str(settings['SAVE_FILE_PREFIX'])
        file_path = os.path.join(dir, f'packet_prints_{filename_base}.txt.ansi')

        # Make sure directory exists:
        if not os.path.exists(dir):
            os.makedirs(dir)

        # Make sure file exists:
        if not os.path.exists(file_path):
            with open(file_path, 'w') as file:
                # Initialize with descriptor:
                file.write(
                    "A colored-text log of the print outs of all packets received during the {filename_base} session:\n"
                    "NOTE: This includes any ANSI escape codes (like coloring) which will show up weird if viewed as "
                    "plain-text. To view this appropriately, use the terminal `cat` command or view in VSCode with the "
                    "`ANSI Colors` extension installed.\n\n"
                )
        
        # Append the packet print:
        with open(file_path, 'a') as file:
            file.write(f"{packet_print_string(packet)}\n\n")

# Handles a new incoming streamed packet, including logging and display.
# packet [Packet]: Newly received packet.
# use_telem_dataview [bool]: Whether to display the new data using the new Telemetry Dataview (True) or by just printing the packet (False).
def handle_streamed_packet(packet: Optional[Packet], use_telem_dataview: bool = False) -> None:
    if packet is not None:
        # Feed the streams:
        update_telemetry_streams(packet) # telem dataframe updated in here

        if USE_LOG_DATAFRAMES:
            # Normal packet. Load it:
            update_packet_log_dataframe(packet)
            # If the packet doesn't contain any telemetry (i.e. log, debug print, etc.), add it to the non-telem packet log in LiFo manner:
            # - Also do this for WatchdogDetailedStatusPacket since they're *very* detailed (contain way too much data to display so we're just
            # going to display it here instead).
            # - Also push command responses to the prints section so they're seen explicitly (its packet printer includes a special parser to decode the command name):
            if len([*packet.payloads[TelemetryPayload]]) == 0 or isinstance(packet, (WatchdogDetailedStatusPacket, WatchdogCommandResponsePacket)):
                nontelem_packet_prints.appendleft(packet_print_string(packet))
        
        # Display the data:
        if use_telem_dataview:
            # Update the display:
            refresh_telemetry_dataview()
        else:
            # Just log the data:
            log_print(packet)

        # Save the printout of the packet:
        save_packet_to_packet_prints(packet)


def save_pcap(full_packets):
    pcap_fp = open(os.path.join(cast(str, settings['SAVE_DIR']),
                                f'data_{ulid.new()}.pcapng'), 'wb')

    # build the header block
    # 28 bytes since no options
    sh_block = struct.pack('=LLLHHQL', 0x0A0D0D0A, 28, 0x1A2B3C4D, 1, 0,
                           0xFFFFFFFFFFFFFFFF, 28)
    pcap_fp.write(sh_block)  # must be 1st block

    # write IDB
    # 20 bytes since no options
    idb_block = struct.pack('=LLHHLL', 0x00000001, 20, 228, 0, 0, 20)
    pcap_fp.write(idb_block)

    # loop through all the bytes and write them out
    for packet in full_packets:
        packet_len = len(packet)
        packet_len_pad = math.ceil(packet_len/4)*4
        # write out the header
        packet_block_hdr = struct.pack('=LLL', 0x00000003, 16 + packet_len_pad,
                                       packet_len)
        pcap_fp.write(packet_block_hdr)

        # write the packet itself
        pcap_fp.write(packet)

        # pad
        pad = (b'\0') * (packet_len_pad - packet_len)
        if len(pad) > 0:
            pcap_fp.write(pad)

        # footer
        packet_block_ftr = struct.pack('=L', 16 + packet_len_pad)
        pcap_fp.write(packet_block_ftr)

    pcap_fp.close()


from enum import Enum
class SlipState(Enum):
     FIRST_END = 1
     FIRST_BYTE_OR_STARTING_END = 2
     STARTED = 3

# Streams data over
def stream_data_ip_udp_serial(use_telem_dataview: bool = False) -> None:
    escape = False
    keep_running = True
    nrx = 0
    line = b''
    bin_file = 'file.bin'
    data_bytes = bytearray(b'')
    full_packets: List[bytes] = []
    slip_state = SlipState.FIRST_END

    if ser is not None:
        ready = True
    else:
        ready = False
        cprint("Can't read data, serial connection not started. Try `connect_serial()`.",
               'red')

    while keep_running and ready:
        try:  # safety exception catch to keep things running
            b: Any = ser.read(1)
            line += b
            b = int.from_bytes(b, 'big')

            def append_byte(b):
                nonlocal escape
                nonlocal data_bytes
                nonlocal line
                nonlocal nrx

                if escape:
                    if b == 0xDC:
                        data_bytes.append(0xC0)
                    elif b == 0xDD:
                        data_bytes.append(0xDB)
                    escape = False
                else:
                    if b == 0xDB:
                        escape = True
                    else:
                        data_bytes.append(b)
                        # data_bytes.append(bytes(b.hex(), 'utf-8'))

                # print stuff
                # print('%02x ' % b, end='', flush=True)
                # nrx += 1
                # if (nrx % 16) == 0:
                #     print('')
                #     #print('    ' + re.sub(r'[^\x00-\x7F]+', '.', line.decode('ascii', 'ignore')))
                #     line = b''
            
            if slip_state == SlipState.FIRST_END:
                if b == 0xC0:
                    slip_state = SlipState.FIRST_BYTE_OR_STARTING_END
            elif slip_state == SlipState.FIRST_BYTE_OR_STARTING_END:
                if b != 0xC0:
                    append_byte(b)
                    slip_state = SlipState.STARTED
            elif slip_state == SlipState.STARTED:
                if b == 0xC0:
                    if len(data_bytes) != 0:  # packet baked:
                        full_packets.append(data_bytes)
                        # Process it:
                        packet = parse_ip_udp_packet(data_bytes)
                        # Handle it:
                        handle_streamed_packet(packet, use_telem_dataview)
                        # Move on:
                        data_bytes = bytearray(b'')
                        slip_state = SlipState.FIRST_BYTE_OR_STARTING_END
                else:
                    append_byte(b)

            # print stuff
            # print('%02x ' % b, end='', flush=True)
            # nrx += 1
            # if (nrx % 16) == 0:
            #     print('')
            #     #print('    ' + re.sub(r'[^\x00-\x7F]+', '.', line.decode('ascii', 'ignore')))
            #     line = b''
        except KeyboardInterrupt:
            save_pcap(full_packets)
            data_bytes = bytearray(b'')
            slip_state = SlipState.FIRST_BYTE_OR_STARTING_END
        except PacketDecodingException as pde:
            err_print(
                "While streaming data, a PacketDecodingException occured. The data "
                "bytes at the time of the exception were: \n"
                f"{scp.hexdump(data_bytes, dump=True)}\n."
                f"The PacketDecodingException was: `{pde}`."
            )
            data_bytes = bytearray(b'')
            slip_state = SlipState.FIRST_BYTE_OR_STARTING_END
        except Exception as e:
            if not use_telem_dataview:
                # Dont print in data view, that would be problematic.
                err_print(
                    f"An otherwise unresolved error occurred during packet streaming: {e}"
                )
            else:
                # In data view, just push the string to the packet print console:
                nontelem_packet_prints.appendleft(f"An otherwise unresolved error occurred during packet streaming: {e}")
            data_bytes = bytearray(b'')
            slip_state = SlipState.FIRST_BYTE_OR_STARTING_END


"""
def stream_data_ip_udp_serial() -> None:
    escape = False
    keep_running = True
    nrx = 0
    line = b''
    bin_file = 'file.bin'
    data_bytes = bytearray(b'')
    full_packets: List[bytes] = []

    if ser is not None:
        ready = True
    else:
        ready = False
        cprint("Can't read data, serial connection not started. Try `connect_serial()`.",
               'red')

    while keep_running and ready:
        b: Any = ser.read(1)
        line += b
        b = int.from_bytes(b, 'big')
        if escape:
            if b == 0xDC:
                data_bytes.append(0xC0)
            elif b == 0xDD:
                data_bytes.append(0xDB)
            escape = False
        else:
            if b == 0xC0:
                if len(data_bytes) >= 1:  # packet baked:
                    # Process it:
                    packet = parse_ip_udp_packet(data_bytes)
                    if packet is None:
                        print("\> Empty packet received.")
                    else:
                        # Log the data:
                        all_payloads.extend(packet.payloads)
                        print(packet)
                        # Feed the streams:
                        update_telemetry_streams(packet)
                    # Move on:
                    data_bytes = bytearray(b'')
                pass
            elif b == 0xDB:
                escape = True
            else:
                data_bytes.append(b)
                # data_bytes.append(bytes(b.hex(), 'utf-8'))

        # print stuff
        print('%02x ' % b, end='', flush=True)
        nrx += 1
        if (nrx % 16) == 0:
            print('')
            # print('    ' + re.sub(r'[^\x00-\x7F]+', '.', line.decode('ascii', 'ignore')))
            line = b''
"""
