# -*- coding: utf-8 -*-
"""
Extracts the payloads from the given pcap/pcapng file.

@author: Connor W. Colombo (CMU)
@last-updated: 05/06/2021
"""
from typing import List, Type
import traceback

import argparse

from termcolor import cprint

import scapy.all as scp  # type: ignore
import struct
import ulid
from matplotlib import pyplot as plt  # type: ignore

from IrisBackendv3.data_standards import DataStandards
from IrisBackendv3.data_standards.prebuilt import add_to_standards, watchdog_heartbeat_tvac
from IrisBackendv3.codec.payload import Payload, PayloadCollection, extract_downlinked_payloads
from IrisBackendv3.codec.metadata import DataPathway, DataSource
from IrisBackendv3.codec.magic import Magic
from IrisBackendv3.codec.logging import logger as CodecLogger
from IrisBackendv3.codec.settings import set_codec_standards
from IrisBackendv3.utils.basic import print_bytearray_hex as printraw
from IrisBackendv3.codec.packet import Packet, IrisCommonPacket,  WatchdogTvacHeartbeatPacket, WatchdogHeartbeatPacket, WatchdogCommandResponsePacket

from trans_tools import parse_packet, all_payloads, telemetry_streams, update_telemetry_streams_from_payloads, load_cache, cache, plot_stream
from trans_tools import settings as trans_settings

from time import time


def get_opts():
    """
    Return CLI options wrapped in argparse.
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

    parser = argparse.ArgumentParser(
        description='IRIS Lunar Rover — GSW Backend — Parse PCAP'
    )

    parser.add_argument('--pcap-file', type=str, default='./test-data/Iris_210503_18_hours_of_telem.pcap',  # './test-data/Iris_FSWv1.0.0_210420_Telemetry.pcapng',
                        help='Address of the PCAP/NG file to read.')

    parser.add_argument('--port', type=int, default=42000,
                        help='UDP port to look for packets on.')

    parser.add_argument('--cache-dir', type=str, default=f'./pcap_logs/',
                        help='Where to save transceiver cache files.')

    parser.add_argument('--cache-prefix', type=str, default=f'iris_pcap_{ulid.new()}',
                        help='Prefix to add to transceiver cache files.')

    parser.add_argument('--cache-telem', type=str2bool, nargs='?', const=True, default=True,
                        help='Whether telemetry streams should be cached and saved at all.')

    parser.add_argument('--cli-log', type=str2bool, nargs='?', const=True, default=True,
                        help='Whether packets should be logged to the CLI as text.')

    parser.add_argument('--plot', type=str2bool, nargs='?', const=True, default=True,
                        help='Whether all telemetry streams should be plotted.')

    parser.add_argument('--packetgap', type=int, default=0,
                        help='Number of packets to ignore at beginning of pcap.')

    parser.add_argument('--deadspace', type=int, default=0,
                        help='Number of bytes of deadspace at the beginning of each packet payload (Raw section).')

    opts = parser.parse_args()
    return opts


if __name__ == "__main__":
    # Fetch CLI Options:
    opts = get_opts()

    cprint(
        f"Parsing pcap file at {opts.pcap_file}, "
        f"looking for UDP packets on port {opts.port}.\n"
        f"Results will be cached to {opts.cache_dir}, "
        f"using prefix {opts.cache_prefix}.",
        color='blue'
    )

    # Internal Settings:
    # Data Transport:
    protocol = scp.UDP  # Protocol FSW is using to send data

    # Data Formatting Settings:
    print("\t > Opening pcap . . .")
    pcap = scp.rdpcap(opts.pcap_file)

    print(f"\t > Found {len(pcap)} packets in pcap.")
    pcap_packets = list(
        filter(lambda x: x.dport == opts.port, pcap[protocol][opts.packetgap:])
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

    extract_time = time() - extract_start
    cprint(
        (
            f"In {extract_time:.3f}s: "
            f"Successfully loaded {len(all_payloads.TelemetryPayload)} telemetry payloads, "
            f"{len(all_payloads.EventPayload)} event payloads, "
            f"{len(all_payloads.FileBlockPayload)} file block payloads, "
            f"and {len(all_payloads.CommandPayload)} command payloads "
            f"using {len(pcap_packets)} packets of {len(pcap)} packets in the pcap. "
            f"Of the used packets, a total of {failed_packet_count} were corrupted or not parsable as downlink packets."
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
