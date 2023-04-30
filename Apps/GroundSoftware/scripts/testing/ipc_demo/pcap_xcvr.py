"""
Simple test-case demo of a synchronous IPC App that reads from a
`PcapTransceiver` and publishes packets as they "arrive".

This app is designed as a simple way to semi-realistically hydrate IPC Apps
that require data originating at the XCVR layer.

@author: Connor W. Colombo (CMU)
@last-updated: 03/08/2023
"""
import time
import argparse
import IrisBackendv3 as IB3
import IrisBackendv3.ipc as ipc
from IrisBackendv3.transceiver.logs import logger_setConsoleLevel as xcvrLoggerLevel
from IrisBackendv3.ipc.messages import (
    DownlinkedPacketsMessage, DownlinkedPacketsContent,
    DownlinkedPayloadsMessage, DownlinkedPayloadsContent
)


IB3.init_from_latest()

app = ipc.IpcAppHelper("PcapTransceiverDemo")
app.setLogLevel('VERBOSE')

parser = argparse.ArgumentParser(description=(
    'IRIS Lunar Rover — IPC PCAP Transceiver Demo — CLI'
))


def get_opts():
    parser.add_argument('-x', '--prebuilt-xcvr-name', type=str,
                        default='PCAP-RC9.5.3-2MIN-IMU',
                        help='Name of the prebuilt Transceiver to use.')
    parser.add_argument('-t', '--period-ms', type=float, default=1000,
                        help='Period between packets in ms.')
    parser.add_argument('-g', '--packet-gap', type=int, default=0,
                        help=(
                            'Number of packets at the start of the pcap to '
                            'skip before reading.'
                        ))
    parser.add_argument('--loop', default=True,
                        action=argparse.BooleanOptionalAction,
                        help="Whether or not to loop the packet.")
    parser.add_argument('--bind', default=False,
                        action=argparse.BooleanOptionalAction,
                        help="Whether or not to bind the port. Should be false except for diagnostics.")
    return parser.parse_args()


opts = get_opts()


# Load data:
xcvrLoggerLevel('NOTICE')
xcvr = IB3.transceiver.prebuilts.build_xcvr_by_name(
    opts.prebuilt_xcvr_name,
    packetgap=opts.packet_gap,  # skip first 37000 packets (of 37644)
    fixed_period_ms=opts.period_ms,
    loop=opts.loop,
    log_on_receive=False
)
xcvr.begin()

# Setup IPC:
manager = ipc.IpcAppManagerSync(socket_specs={
    'pub': ipc.SocketSpec(
        sock_type=ipc.SocketType.PUBLISHER,
        port=ipc.Port.TRANSCEIVER_PUB,
        topics=[ipc.Topic.DL_PACKETS, ipc.Topic.DL_PAYLOADS],
        bind=opts.bind
    )
})

while len(packets := xcvr.read()) != 0:
    msg = DownlinkedPacketsMessage(DownlinkedPacketsContent(
        packets=packets
    ))
    manager.send_to('pub', msg, ipc.Topic.DL_PACKETS, subtopic_bytes=b'pcap')
    app.logger.notice(
        f"Sent {msg.content.simple_str()} -> {ipc.Topic.DL_PACKETS}"
    )

    payloads = packets[0].payloads
    for packet in packets[1:]:
        payloads.extend(packet.payloads)
    msg = DownlinkedPayloadsMessage(DownlinkedPayloadsContent(
        payloads=payloads
    ))
    manager.send_to('pub', msg, ipc.Topic.DL_PAYLOADS, subtopic_bytes=b'pcap')
    app.logger.notice(
        f"Sent {msg.content.simple_str()} -> {ipc.Topic.DL_PAYLOADS}"
    )
