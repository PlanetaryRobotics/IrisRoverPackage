#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Fri Nov 27 15:02:00 2020

@author: connorcolombo
"""
import traceback

import scapy.all as scp  # type: ignore
import struct

from typing import List, Type

from IrisBackendv3.data_standards import DataStandards
from IrisBackendv3.data_standards.prebuilt import add_to_standards, watchdog_heartbeat_tvac
from IrisBackendv3.codec.payload import Payload, PayloadCollection, extract_downlinked_payloads
from IrisBackendv3.codec.metadata import DataPathway, DataSource
from IrisBackendv3.codec.magic import Magic
from IrisBackendv3.codec.logging import logger as CodecLogger
from IrisBackendv3.codec.settings import set_codec_standards
from IrisBackendv3.utils.basic import print_bytearray_hex as printraw
from IrisBackendv3.codec.packet import Packet, IrisCommonPacket, WatchdogTvacHeartbeatPacket

from tvac_tools import telemetry_streams, update_telemetry_streams, load_cache, plot_stream
from tvac_tools import settings as tvac_settings

# Commands and Telemetry: Fp XML Comments
# Logs and Events: Fp XML (parse string formatter)
# Files: structs in FswPacket.hpp

# Two telemetry after last block = dropped block. Close file.
# struct pack (struct)

# maintain state to avoid resetting registers unnecessarily
sequence_number = 10

standards = DataStandards.build_standards()

standards.print_overview()

standards.cache(
    cache_dir="./sandbox-cache",
    filename_base="sandbox", ext="json",
    indent=2
)

uncached_standands = DataStandards.load_cache(
    cache_dir="./sandbox-cache",
    filename_base="sandbox", ext="json"
)

print(f"Standards match: {uncached_standands == standards}")

# Extract Payloads from a sample pcap:
add_to_standards(standards, watchdog_heartbeat_tvac)
set_codec_standards(standards)
# Data Transport:
# Iris_FSWv1.0.0_210409_Telemetry.pcapng'  # PCAP logs
file = './test-data/Iris_FSWv1.0.0_210409_Telemetry.pcapng'
protocol = scp.UDP  # Protocol FSW is using to send data
port = 8080  # Port on the spacecraft FSW is sending data to

# Data Formatting Settings:
packetgap = 0  # number of packets to ignore at beginning of pcap
deadspace = 0  # number of bytes of deadspace at the beginning of the
endianness_code = "<"  # < = little, > = big, ! = network
pcap = scp.rdpcap(file)
packets = list(
    filter(lambda x: x.dport == port, pcap[protocol][packetgap:])
)

all_payloads: PayloadCollection = PayloadCollection(
    CommandPayload=[],
    TelemetryPayload=[],
    EventPayload=[],
    FileBlockPayload=[]
)

tvac_settings['SAVE_FILE_PREFIX'] = 'iris_hercules_tset_2021040907'
load_cache()
plot_stream('Imu_XAcc')
plot_stream('Imu_YAcc')
plot_stream('Imu_ZAcc')
plot_stream('Imu_XAng')
plot_stream('Imu_YAng')
plot_stream('Imu_ZAng')
plot_stream('ActiveRateGroup-RateGroupHiFreq_RgMaxTime')
plot_stream('ActiveRateGroup-RateGroupMedFreq_RgMaxTime')
plot_stream('ActiveRateGroup-RateGroupLowFreq_RgMaxTime')
for packet in packets:
    packet_bytes = scp.raw(packet.getlayer(scp.Raw))[deadspace:]

    # All available packet codecs (in order of use preference):
    codecs: List[Type[Packet]] = [
        IrisCommonPacket,
        WatchdogTvacHeartbeatPacket
    ]
    # Codecs which support this packet:
    supported = [c for c in codecs if c.is_valid(packet_bytes)]

    if len(supported) == 0:
        CodecLogger.warning(
            f"Invalid packet detected. Does not conform to any supported specs: "
            f"{packet_bytes}"
        )

    if len(supported) > 1:
        CodecLogger.warning(
            f"Multiple codecs ({supported}) support received packet. Using "
            f"highest in preference order: {supported[0]}. "
            f"Packet data: {packet_bytes} ."
        )

    # Parse VLP:
    try:
        if len(supported) > 0:
            packet = supported[0].decode(
                packet_bytes,
                pathway=DataPathway.WIRELESS,
                source=DataSource.PCAP
            )
            update_telemetry_streams(packet)
            print(packet)
            for i in range(len(packet.payloads)):
                all_payloads[i].extend(packet.payloads[i])  # type: ignore

    except Exception as e:
        trace = e  # traceback.format_exc()
        CodecLogger.warning(
            f"Had to abort packet parsing due to the following exception: {trace}"
        )

print(f"Loaded {len(all_payloads.TelemetryPayload)} telemetry payloads.")
