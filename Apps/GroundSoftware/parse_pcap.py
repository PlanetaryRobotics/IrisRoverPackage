# -*- coding: utf-8 -*-
"""
Extracts any valid payloads from the given pcap/pcapng file. Can be run from the
command line as a standalone program or imported as a python module.

@author: Connor W. Colombo (CMU)
@last-updated: 11/15/2021
"""
from typing import List, AsyncGenerator, Type, cast, Union
import asyncio

import argparse

from termcolor import cprint

import scapy.all as scp  # type: ignore
import ulid
from matplotlib import pyplot as plt  # type: ignore

from IrisBackendv3.codec.payload import TelemetryPayload
from IrisBackendv3.codec.logging import VALID_LOG_LEVELS, logger as CodecLogger
from IrisBackendv3.codec.packet import Packet

from IrisBackendv3.codec.packet import parse_packet
from trans_tools import all_payloads, telemetry_streams, update_telemetry_streams_from_payloads, load_cache, cache, plot_stream
from trans_tools import settings as trans_settings

from time import time


def get_opts(
    description: str = 'IRIS Lunar Rover — GSW Backend — Parse PCAP',
    default_port: Union[str, int] = 'any',
    default_cli_log: bool = True,
    default_plot: bool = True,
    default_log_level: str = 'INFO'
):
    """
    Return CLI options wrapped in argparse.
    If this is being called from another program which has imported `parse_pcap`,
    the program description can be changed using `description`.
    Default values to be used if they're not supplied in the cli can be changed
    by the caller using the `default_xx` arguments.
    """
    def str2bool(v):
        if isinstance(v, bool):
            return v
        if v.lower() in ('yes', 'true', 't', 'y', '1'):
            return True
        elif v.lower() in ('no', 'false', 'f', 'n', '0'):
            return False
        else:
            raise argparse.ArgumentTypeError('Boolean value expected.')

    def str_to_log_level(s):
        if isinstance(s, str) and s.upper() in VALID_LOG_LEVELS:
            return s.upper()
        else:
            raise argparse.ArgumentTypeError(
                f'Valid log level expected. Log levels are: {VALID_LOG_LEVELS}'
            )

    def udp_port_selection(u: Union[int, str]):
        if isinstance(u, str) and u.lower() == 'any':
            return u.lower()

        try:
            return int(u)
        except ValueError as e:
            raise argparse.ArgumentTypeError(
                f"UDP Port should be an int or 'any'. Got: {u} of type: {type(u)}"
            )

    parser = argparse.ArgumentParser(
        description=description
    )

    parser.add_argument('--pcap-file', type=str, default='./test-data/Iris_210503_18_hours_of_telem.pcap',  # './test-data/Iris_FSWv1.0.0_210420_Telemetry.pcapng',
                        help='Address of the PCAP/NG file to read.')

    parser.add_argument('--port', type=udp_port_selection, default=default_port,
                        help="UDP port to look for packets on (or 'any' to grab all UDP packets).")

    parser.add_argument('--cache-dir', type=str, default=f'./pcap_logs/',
                        help='Where to save transceiver cache files.')

    parser.add_argument('--cache-prefix', type=str, default=f'iris_pcap_{ulid.new()}',
                        help='Prefix to add to transceiver cache files.')

    parser.add_argument('--cache-telem', type=str2bool, nargs='?', const=True, default=True,
                        help='Whether telemetry streams should be cached and saved at all.')

    parser.add_argument('--cli-log', type=str2bool, nargs='?', const=True, default=default_cli_log,
                        help='Whether packets should be logged to the CLI as text.')

    parser.add_argument('--plot', type=str2bool, nargs='?', const=True, default=default_plot,
                        help='Whether all telemetry streams should be plotted.')

    parser.add_argument('--packetgap', type=int, default=0,
                        help='Number of packets to ignore at beginning of pcap.')

    parser.add_argument('--deadspace', type=int, default=0,
                        help='Number of bytes of deadspace at the beginning of each packet raw (Raw section).')

    parser.add_argument('--log-level', type=str_to_log_level, default=default_log_level,
                        help=('Logging level to be used (i.e. how annoying the logging '
                              'printouts should be). Only logs with importance greater '
                              'than or equal to the specified logging level are '
                              f'displayed. Valid logging levels are: {VALID_LOG_LEVELS}'))

    opts = parser.parse_args()
    return opts


def parse_pcap(opts):
    """
    Extracts any valid payloads from the pcap/pcapng file specified in
    `pcap_file` of the given `argparse` `opts` Namespace according to the
    options specified in that Namespace.
    """
    extracted_packets: List[Packet] = []

    CodecLogger.setLevel(opts.log_level)

    # if debugging_mode := True:
    #     cprint("WARNING. ARGS ARE BEING MODIFIED BECAUSE DEBUG MODE IS ACTIVE", 'red')
    #     opts.pcap_file = './test-data/Iris_Image_Downlink_201223-ImageDLTest_FixedLens.pcapng'
    #     opts.port = 8080
    #     opts.cli_log = True

    cprint(
        f"Parsing pcap file at {opts.pcap_file}, "
        f"looking for UDP packets on port {opts.port}.\n"
        f"Results will be cached to {opts.cache_dir}, "
        f"using prefix {opts.cache_prefix}."
        f"\n Note: to see all configuration options, run in CLI with `--help` argument.",
        color='blue'
    )

    # Internal Settings:
    # Data Transport:
    protocol = scp.UDP  # Protocol FSW is using to send data

    # Data Formatting Settings:
    print("\t > Opening pcap . . .")
    pcap = scp.rdpcap(opts.pcap_file)

    print(f"\t > Found {len(pcap)} packets in pcap.")
    pcap_packets = list(pcap[protocol][opts.packetgap:])
    if isinstance(opts.port, int):
        pcap_packets = list(
            filter(lambda x: x.dport == opts.port, pcap_packets)
        )
    print(
        f"\t > Found {len(pcap_packets)} matching packets (UDP with port {opts.port}) in pcap."
    )

    # Setup:
    trans_settings['SAVE_DIR'] = opts.cache_dir
    trans_settings['SAVE_FILE_PREFIX'] = opts.cache_prefix

    if opts.cache_telem or opts.plot:
        # Load any existing cache if we plan to save to it OR need to plot from it:
        print("\t > Loading any existing cache . . .")
        load_cache()

    # Parse packets in pcap:
    print(
        f"\t > Extracting packets {'(in silent mode)' if not opts.cli_log else ''} . . ."
    )
    extract_start: float = time()
    failed_packet_count: int = 0
    for i, pcap_packet in enumerate(pcap_packets):
        packet_bytes = scp.raw(pcap_packet.getlayer(scp.Raw))[opts.deadspace:]
        packet = parse_packet(packet_bytes)
        if not i % 100:
            cprint(
                f'\t\t > Parsed {i+1:5.0f}/{len(pcap_packets):5.0f} packets.',
                'blue'
            )
        if packet is None:
            failed_packet_count += 1
        else:
            # Log the data:
            if opts.cli_log:
                print(f"[{i:5.0f}/{len(pcap_packets)-1:5.0f}] {packet}")
            # Store:
            extracted_packets.append(packet)
            packet = cast(Packet, packet)
            all_payloads.extend(packet.payloads)

    # Print summary of the results:
    extract_time = time() - extract_start
    corrupted_fileblock_count = sum(
        1 for p in all_payloads.FileBlockPayload if p.possible_corruption
    )

    cprint(
        (
            f"In {extract_time:.3f}s: "
            f"Successfully loaded {len(all_payloads.TelemetryPayload)} telemetry payloads, "
            f"{len(all_payloads.EventPayload)} event payloads, "
            f"{len(all_payloads.FileBlockPayload)} file block payloads, "
            f"and {len(all_payloads.CommandPayload)} command payloads "
            f"using {len(pcap_packets)} packets of {len(pcap)} packets in the pcap. "
            f"Of the used packets, a total of {failed_packet_count} were "
            "unrecognizably corrupted or not parsable as downlink packets and "
            f"{corrupted_fileblock_count} of the file blocks were possibly corrupted but still parsable."
        ),
        'green'
    )

    if opts.cache_telem or opts.plot:
        print("\t > Building telemetry streams from payloads . . .")
        update_telemetry_streams_from_payloads(all_payloads, auto_cache=False)

    if opts.cache_telem:
        print("\t > Caching telemetry streams . . .")
        cache()

    # Plot all telemetry streams
    if opts.plot:
        print("\t > Displaying plots . . .")
        title = f'Iris Log: {opts.pcap_file}'
        for name in telemetry_streams.names:
            plot_stream(name, title=title, auto_show=False)
        plt.show()

    # Return results:
    return {
        'extracted_packets': extracted_packets,
        'all_payloads': all_payloads,
        'telemetry_streams': telemetry_streams,
        'failed_packet_count': failed_packet_count
    }


async def replay_telemetry_at_fixed_rate(
    packets: List[Packet],
    fixed_rate: int
) -> AsyncGenerator[List[TelemetryPayload], None]:
    """
    Generator class for replaying a pcap file at a fixed rate (fixed number of
    milliseconds between each packet).

    Args:
        packets (List[Packet]): Collection of all packets to be replayed.
        fixed_rate (int): Number of milliseconds between each packet receipt.

    Yields:
        Generator[List[TelemetryPayload]]: List of all payloads in the packet that's being replayed.
    """
    for packet in packets:
        telemetry = packet.payloads.TelemetryPayload
        await asyncio.sleep(fixed_rate/1000.0)
        yield telemetry


if __name__ == "__main__":
    # Fetch CLI Options:
    opts = get_opts()

    parse_pcap(opts)
