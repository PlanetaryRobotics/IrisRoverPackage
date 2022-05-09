#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Basic collection of abstraction tools to make for a successful TVAC test.

Designed to interface solely with Watchdog using IP/UDP-SLIP via RS-422 with
special `WatchdogTvacHeartbeatPackets` and normal commands.

@author: Connor W. Colombo (CMU)
@last-updated: 04/21/2021
"""

import traceback
from typing import Any, List, Type, cast, Union, Dict, Tuple, Optional

import struct
import os
import pickle
from datetime import datetime, timedelta
import csv
from itertools import zip_longest

import serial  # type: ignore # no type hints
import scapy.all as scp  # type: ignore # no type hints
import numpy as np
import matplotlib.pyplot as plt  # type: ignore # no type hints
import ulid

from termcolor import cprint

from IrisBackendv3.utils.basic import print_bytearray_hex as printraw
from IrisBackendv3.utils.basic import bytearray_to_spaced_hex as hexstr
from IrisBackendv3.utils.nameiddict import NameIdDict

from IrisBackendv3.data_standards import DataStandards
from IrisBackendv3.data_standards.logging import logger as DsLogger
from IrisBackendv3.data_standards.prebuilt import add_to_standards, watchdog_heartbeat_tvac
from IrisBackendv3.codec.payload import Payload, TelemetryPayload, CommandPayload, WatchdogCommandPayload
from IrisBackendv3.codec.payload_collection import EnhancedPayloadCollection
from IrisBackendv3.codec.packet import Packet, IrisCommonPacket, WatchdogTvacHeartbeatPacket
from IrisBackendv3.codec.metadata import DataPathway, DataSource
from IrisBackendv3.codec.magic import Magic, MAGIC_SIZE
from IrisBackendv3.codec.logging import logger as CodecLogger
from IrisBackendv3.codec.settings import ENDIANNESS_CODE, set_codec_standards

import seaborn as sns  # type: ignore
sns.set()

settings: Dict[str, Union[str, int]] = {
    'SAVE_DIR': './tvac_logs/test_results',
    'SAVE_FILE_PREFIX': 'iris_logs',
    'SAVE_FILE_EXT': 'tvac',
    # number of minutes after which the old save file won't be overwritten and a new one will be made:
    'NEW_SAVE_PERIOD': 15
}

ser: Any = None

DsLogger.setLevel('CRITICAL')
standards = DataStandards.build_standards()
add_to_standards(standards, watchdog_heartbeat_tvac)
set_codec_standards(standards)

all_payloads: EnhancedPayloadCollection = EnhancedPayloadCollection()
telemetry_streams: NameIdDict[List[Tuple[datetime, Any]]] = NameIdDict()


def parse_ip_udp_packet(packet_bytes: bytes) -> Optional[Packet]:
    """Parses an IP/UDP packet and extracts the meaningful payload."""
    # Convert into a full packet:
    full_packet = scp.IP(packet_bytes)
    # Scrape off the IP/UDP and keep the Raw:
    core_data = scp.raw(full_packet.getlayer(scp.Raw))
    # Parse like payload:
    return parse_packet(core_data)


def parse_packet(packet_bytes: bytes) -> Optional[Packet]:
    # All available packet codecs (in order of use preference):
    codecs: List[Type[Packet]] = [
        WatchdogTvacHeartbeatPacket,  # Only support watchdog heartbeat here
        # IrisCommonPacket
    ]
    # Codecs which support this packet:
    supported = [c for c in codecs if c.is_valid(packet_bytes)]

    # Check for issues:
    if len(supported) == 0:
        CodecLogger.warning(
            f"Invalid packet detected. Does not conform to any supported specs: "  # type: ignore
            f"{packet_bytes}"
        )

    if len(supported) > 1:
        CodecLogger.warning(
            f"Multiple codecs "  # type: ignore
            f"({supported}) support received packet. Using "
            f"highest in preference order: {supported[0]}. "
            f"Packet data: {packet_bytes} ."
        )

    # Parse Packet:
    packet: Optional[Packet] = None  # default
    try:
        if len(supported) > 0:
            # Parse:
            packet = supported[0].decode(
                packet_bytes
            )
            # Store:
            packet = cast(Packet, packet)
            all_payloads.extend(packet.payloads)

    except Exception as e:
        trace = e  # traceback.format_exc()
        CodecLogger.warning(
            f"Had to abort packet parsing due to the following exception: {trace}"
        )

    return packet


def list_commands() -> None:
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


def list_channels() -> None:
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
    Pack up a Watchdog command into bytes to send.
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

    # Arbitrary IPs:
    full_packet = scp.IP(dst='127.0.0.1', src='222.173.190.239') / \
        scp.UDP(dport=8080)/scp.Raw(load=packet)
    # printraw(scp.raw(scp.IP(scp.raw(full_packet))))
    # printraw(scp.raw(full_packet))
    # scp.raw(full_packet)
    return cast(bytes, scp.raw(full_packet))


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


def send_command(command_name: str, **kwargs) -> str:
    data = pack_watchdog_command(command_name, **kwargs)
    try:
        send_slip(data)
    except serial.SerialTimeoutException:
        cprint("Failed to send due to serial timeout. Check the connection?", 'red')
    finally:
        return f"Data: {hexstr(data)}."


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


def update_telemetry_streams(packet: Packet) -> None:
    """
    Add all extracted data values in the packet to their streams:
    """
    for t in packet.payloads.TelemetryPayload:
        # If this payload's channel is new (previously un-logged), add it:
        if t.opcode not in telemetry_streams:
            telemetry_streams[t.opcode, t.module.name+'_'+t.channel.name] = [
                (datetime.now(), t.data)
            ]
        else:  # otherwise, update:
            telemetry_streams[t.opcode].append((datetime.now(), t.data))

    # Save the updated streams:
    cache()


def plot_stream(channel_name: str) -> None:
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
            plt.title('Iris Lunar Rover TVAC')
            plt.show()
        except KeyError:
            cprint(
                f"Given channel name `{channel_name}` is invalid. Valid channel "
                f"names are: {telemetry_streams.names} .", 'yellow'
            )


def get_latest_cache_file(prefix: Optional[str] = None) -> Tuple[str, ulid.ulid.ULID]:
    """
    Returns the path to the most recent cache_file which meets the specs in
    settings, along with the ULID of that latest file.
    """
    cache_dir = str(settings['SAVE_DIR'])
    filename_base = str(settings['SAVE_FILE_PREFIX'])
    ext = str(settings['SAVE_FILE_EXT'])
    if prefix is None:
        filename_base = str(settings['SAVE_FILE_PREFIX'])
    else:
        filename_base = prefix

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

        # Attempt to open the file to see if it's valid:
        try:
            open_cache(os.path.join(cache_dir, f))
            corrupt = False
        except EOFError:
            # File is corrupted, ignore this version.
            corrupt = True

        # Store this as the latest file if it's the first file or its
        # ulid is chronologically more recent that the previous `latest_file`:
        if i == 0:
            latest_file = f, u
        elif not corrupt and u > latest_file[1]:
            latest_file = f, u

    cache_filename = latest_file[0]

    # Build & return the file path:
    return os.path.join(cache_dir, cache_filename), latest_file[1]


def grab_latest_of_every_cache() -> List[Tuple[str, ulid.ulid.ULID]]:
    """
    Make a list of the latest file for every `SAVE_FILE_PREFIX` in the 
    current `SAVE_DIR` with the current `SAVE_FILE_EXT`.
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
    # Grab all unique prefixes:
    unique_prefixes = {
        # separate by ulid delimiter and remove ulid + ext section:
        '_'.join(f.split('_')[:-1])
        for f in files
    }

    # For each unique prefix, grab the latest file with that ulid. Return list:
    return [get_latest_cache_file(p) for p in unique_prefixes]


def stitch_and_export_all_data(out_folder: str, start_time: Optional[datetime] = None, end_time: Optional[datetime] = None) -> None:
    """
    Stich together all the data streams from the latest file for every 
    `SAVE_FILE_PREFIX` in the current `SAVE_DIR` with the current `SAVE_FILE_EXT`,
    then export the data to csv and create and save plots of all streams.
    Places outputs in the given `out_folder`.

    If `start_time` is given, data is only kept if it's from after `start_time`.
    If `end_time` is given, data is only kept if it's from before `end_time`.
    """
    # Make sure output directory exists:
    if not os.path.exists(out_folder):
        cprint(
            f"Directory `{out_folder}` doesn't exist. Making it . . .", 'yellow')
        os.makedirs(out_folder)

    # Grab all data filenames:
    cprint('Collecting all data files . . .', 'blue')
    all_data_files = grab_latest_of_every_cache()

    # Open all cache files and stitch all data together
    cprint('Loading all data & stitching it . . .', 'blue')
    all_data: NameIdDict[List[Tuple[str, datetime, Any]]] = NameIdDict()
    for path, _ in all_data_files:
        try:
            data = open_cache(path)
            data = cast(NameIdDict[List[Tuple[datetime, Any]]], data)
            for ke, vals in data.collect():
                vals = [(path, t, v) for t, v in vals]  # add file to vals
                if ke not in all_data:
                    # First we're seeing this stream's KeyEnumerator, so add it:
                    all_data[ke] = vals
                else:
                    # There's already data for this stream; so, add just append it:
                    all_data[ke].extend(vals)
        except EOFError:
            CodecLogger.error(
                f"Failed to load existing file at `{path}` while performing stitch "
                "and export for cached logs. File appears to be corrupted."
            )
        except FileNotFoundError:
            CodecLogger.warning(
                f"Failed to load existing file at `{path}` while performing stitch "
                "and export for cached logs."
            )

    # Process data by time:
    cprint('Temporal processing . . .', 'blue')
    earliest_time: datetime = datetime.now()  # earliest time observed
    for ke, _ in all_data.collect():
        # Sort all data by time:
        all_data[ke].sort(key=lambda x: x[1])  # sort by timestamp

        # Remove all out of bounds entries:
        if start_time is not None:
            all_data[ke] = [(f, t, v)
                            for f, t, v in all_data[ke] if t >= start_time]
        if end_time is not None:
            all_data[ke] = [(f, t, v)
                            for f, t, v in all_data[ke] if t <= end_time]

        # Find the earliest time observed (for epoch calculation):
        if all_data[ke][0][1] < earliest_time:
            earliest_time = all_data[ke][0][1]

    # Export all data:
    export_name = f"tvac_export_{ulid.new()}"
    inner_ext = str(settings['SAVE_FILE_EXT'])
    data_table: List[List[Any]] = []
    for i, (ke, vals) in enumerate(all_data.collect()):
        # Extract data:
        fs = [os.path.basename(f) for f, _, _ in vals]
        ts = [t for _, t, _ in vals]
        s = [(t - earliest_time).total_seconds() for t in ts]
        vs = [v for _, _, v in vals]

        # Add to data table:
        [stream_name, *_], _ = ke
        data_table.append([stream_name+'_test', *fs])
        data_table.append([stream_name+'_times', *ts])
        data_table.append([stream_name+'_seconds', *s])
        data_table.append([stream_name+'_values', *vs])
        data_table.append([])  # spacer between columns

        # Plot data:
        cprint(f'Plotting {i+1}/{len(all_data)}: {stream_name} . . .', 'blue')
        # Only actually plot if *any* value is non-zero (ignore for unimplemented streams):
        is_zero = [v == 0 for v in vs]
        if all(is_zero):
            cprint(f'\t > Skipping. All values are zero.', 'yellow')
        else:
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
            plt.ylabel(stream_name)
            plt.title('Iris Lunar Rover TVAC')
            plt.tight_layout()
            figpath = os.path.join(
                out_folder, f'{export_name}_{stream_name}.{inner_ext}.png'
            )
            plt.savefig(figpath, dpi=1200)

    # Plot test name over time (nominally):
    cprint(f'Plotting Test Sequence . . .', 'blue')
    fs = data_table[0][1:]
    fs = ['_'.join(f.split('_')[:-1]) for f in fs]  # remove ulid and extension
    ts = data_table[1][1:]
    plt.figure()
    plt.plot(ts, fs,
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
    plt.ylabel('Name of Active Test')
    plt.title('Iris Lunar Rover TVAC')
    plt.tight_layout()
    figpath = os.path.join(
        out_folder, f'{export_name}__test_timing.{inner_ext}.png'
    )
    plt.savefig(figpath, dpi=1200)

    # Transpose data (so each subarray is a column):
    cprint(f'Transposing Data . . .', 'blue')
    output_table = [*zip_longest(*data_table)]
    # Export datatable:
    cprint(f'Saving Data . . .', 'blue')
    csvpath = os.path.join(
        out_folder, f'{export_name}_all_data.{inner_ext}.csv')
    with open(csvpath, 'w', encoding='utf-8') as csvfile:
        writer = csv.writer(csvfile, delimiter=',', quotechar='"')
        writer.writerows(output_table)

    cprint(f'Done! TVAC data successfully saved to `{out_folder}`', 'green')


def cache() -> None:
    """
    Cache this DataStandards instance in a unique file in `cache_dir`.

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


def open_cache(path: str) -> Any:
    """ Loads data from cache file at given path and returns NameIdDict of it. """
    # Open the file:
    with open(path, 'rb') as file:
        data = pickle.load(file)
    return data


def load_cache() -> None:
    """
    **Overrides** the contents of the current `telemetry_stream` with one loaded
    from the latest cache file. Consider saving first.
    """
    try:
        path, _ = get_latest_cache_file()
        data = open_cache(path)
        telemetry_streams.update(data)
    except FileNotFoundError:
        pass  # Do nothing. This is the first go, there's just nothing to load.


def stream_data() -> None:
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
        try:  # safety exception catch to keep things running
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
                        if packet is not None:
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
            # print('%02x ' % b, end='', flush=True)
            # nrx += 1
            # if (nrx % 16) == 0:
            #     print('')
            #     #print('    ' + re.sub(r'[^\x00-\x7F]+', '.', line.decode('ascii', 'ignore')))
            #     line = b''
        except Exception as e:
            cprint(
                f"An otherwise unresolved error occured during packet streaming: {e}", 'red')
