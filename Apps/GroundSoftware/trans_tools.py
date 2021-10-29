#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Prototyping toolkit for building the Transceiver layer which allows for running 
tests which require the Transceiver layer while the real thing is still being 
built.

@author: Connor W. Colombo (CMU)
@last-updated: 05/06/2021
"""

import traceback
from typing import Any, List, Type, cast, Union, Dict, Tuple, Optional

import struct
import math
import os
import pickle
from datetime import datetime, timedelta

import socket
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
from IrisBackendv3.data_standards.prebuilt import add_to_standards, watchdog_heartbeat_tvac, watchdog_heartbeat, watchdog_command_response
from IrisBackendv3.codec.payload import Payload, PayloadCollection, CommandPayload, WatchdogCommandPayload, extract_downlinked_payloads
from IrisBackendv3.codec.packet import Packet, IrisCommonPacket, WatchdogTvacHeartbeatPacket, WatchdogHeartbeatPacket, WatchdogCommandResponsePacket
from IrisBackendv3.codec.metadata import DataPathway, DataSource
from IrisBackendv3.codec.magic import Magic, MAGIC_SIZE
from IrisBackendv3.codec.logging import logger as CodecLogger
from IrisBackendv3.codec.settings import ENDIANNESS_CODE, set_codec_standards

import seaborn as sns  # type: ignore
sns.set()

settings: Dict[str, Union[str, int]] = {
    'SAVE_DIR': './transceiver_logs/',
    'SAVE_FILE_PREFIX': 'iris_logs',
    'SAVE_FILE_EXT': 'tsc',
    # number of minutes after which the old save file won't be overwritten and a new one will be made:
    'NEW_SAVE_PERIOD': 300
}

ser: Any = None

DsLogger.setLevel('CRITICAL')
standards = DataStandards.build_standards()
add_to_standards(standards, [
    watchdog_heartbeat_tvac,
    watchdog_heartbeat,
    watchdog_command_response
])
set_codec_standards(standards)

all_payloads: PayloadCollection = PayloadCollection(
    CommandPayload=[],
    TelemetryPayload=[],
    EventPayload=[],
    FileBlockPayload=[]
)
telemetry_streams: NameIdDict[List[Tuple[datetime, Any]]] = NameIdDict()


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


def parse_packet(packet_bytes: bytes) -> Optional[Packet]:
    # All available packet codecs (in order of use preference):
    codecs: List[Type[Packet]] = [
        WatchdogHeartbeatPacket,
        WatchdogCommandResponsePacket,
        IrisCommonPacket,
        WatchdogTvacHeartbeatPacket
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
                packet_bytes,
                pathway=DataPathway.WIRELESS,
                source=DataSource.PCAP
            )
            # Store:
            packet = cast(Packet, packet)
            for i in range(len(packet.payloads)):
                all_payloads[i].extend(packet.payloads[i])  # type: ignore

    except Exception as e:
        trace = e  # traceback.format_exc()
        CodecLogger.warning(
            f"Had to abort packet parsing due to the following exception: {trace}"
        )

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


def update_telemetry_streams_from_payloads(payloads: PayloadCollection, auto_cache=True):
    """
    Updates the `telemetry_streams` from a PayloadCollection.
    """
    for t in payloads.TelemetryPayload:
        # If this payload's channel is new (previously un-logged), add it:
        if t.opcode not in telemetry_streams:
            telemetry_streams[t.opcode, t.module.name+'_'+t.channel.name] = [
                (datetime.now(), t.data)
            ]
        else:  # otherwise, update:
            telemetry_streams[t.opcode].append((datetime.now(), t.data))

    # Save the updated streams:
    if auto_cache:
        cache()


def update_telemetry_streams(packet: Packet, auto_cache=True) -> None:
    """
    Add all extracted data values in the packet to their streams:
    """
    update_telemetry_streams_from_payloads(
        packet.payloads,
        auto_cache=auto_cache
    )


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

    startedPacket = False
    gotFirstNonStop = False

    while keep_running and ready:
        try:  # safety exception catch to keep things running
            b: Any = ser.read(1)
            #line += b
            b = int.from_bytes(b, 'big')

            if startedPacket:
                if b == 0xC0:
                    if gotFirstNonStop:
                        full_packets.append(data_bytes)
                        #print(data_bytes)
                        # Process it:
                        packet = parse_ip_udp_packet(data_bytes)
                        if packet is not None:
                            # Log the data:
                            print(packet)
                            # Feed the streams:
                            update_telemetry_streams(packet)
                        # Move on:
                        data_bytes = bytearray(b'')
                        startedPacket = False

                    else:
                        # Got sequentials STOP bytes, continue to ignore them
                        pass
                else: 
                    gotFirstNonStop = True

                    if escape:
                        if b == 0xDC:
                            data_bytes.append(0xC0)
                        elif b == 0xDD:
                            data_bytes.append(0xDB)
                        escape = False
                    elif b == 0xDB:
                        escape = True
                    else:
                        data_bytes.append(b)
                        # data_bytes.append(bytes(b.hex(), 'utf-8'))


            elif b == 0xC0:
                startedPacket = True
                gotFirstNonStop = False
                data_bytes = bytearray(b'')

            # print stuff
            #print('%02x ' % b, end='', flush=True)
            #nrx += 1
            #import re
            #if (nrx % 16) == 0:
            #    print('')
            #    print('    ' + re.sub(r'[^\x00-\x7F]+', '.', line.decode('ascii', 'ignore')))
            #    line = b''
        #except KeyboardInterrupt:
        #    save_pcap(full_packets)

        except Exception as e:
            cprint(
                f"An otherwise unresolved error occurred during packet streaming: {e}", 'red')


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
                        for i in range(len(packet.payloads)):
                            all_payloads[i].extend(
                                packet.payloads[i]  # type: ignore
                            )
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
            #print('    ' + re.sub(r'[^\x00-\x7F]+', '.', line.decode('ascii', 'ignore')))
            line = b''
"""
