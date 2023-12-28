"""
Simple test-case demo of a synchronous IPC App that reads from a
`PcapTransceiver` and publishes packets as they "arrive".

This app is designed as a simple way to semi-realistically hydrate IPC Apps
that require data originating at the XCVR layer.

@author: Connor W. Colombo (CMU)
@last-updated: 06/01/2023
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
                        default='PCAP_GENERIC',
                        help='Name of the prebuilt Transceiver to use.')
    parser.add_argument('-f', '--pcap-file', type=str, default="",
                        help='PCAP file to use.')
    parser.add_argument('-t', '--period-ms', type=float, default=1000,
                        help='Period between packets in ms.')
    parser.add_argument('-g', '--packet-gap', type=int, default=0,
                        help=(
                            'Number of packets at the start of the pcap to '
                            'skip before reading.'
                        ))
    parser.add_argument('-l', '--log-level', type=str, default="NOTICE",
                        choices=IB3.logs.VALID_LOG_LEVELS,
                        help="Logging level.")
    parser.add_argument('--loop', default=True,
                        action=argparse.BooleanOptionalAction,
                        help="Whether or not to loop the packet.")
    return parser.parse_args()


opts = get_opts()


# Load data:
xcvrLoggerLevel(opts.log_level)
app.setLogLevel(opts.log_level)
kwargs = dict(
    packetgap=opts.packet_gap,  # skip first 37000 packets (of 37644)
    fixed_period_ms=opts.period_ms,
    loop=opts.loop,
    log_on_receive=False
)
if opts.pcap_file != "" and opts.pcap_file is not None:
    kwargs['pcap_file'] = opts.pcap_file
xcvr = IB3.transceiver.prebuilts.build_xcvr_by_name(
    opts.prebuilt_xcvr_name,
    **kwargs
)
xcvr.begin()

# Setup IPC:
manager = ipc.IpcAppManagerSync(socket_specs={
    'pub': ipc.SocketSpec(
        sock_type=ipc.SocketType.PUBLISHER,
        port=ipc.Port.TRANSCEIVER_PUB,
        topics=[ipc.Topic.DL_PACKETS],
        publish_only=True
    )
})

n_packets_sent: int = 0  # count total packets sent for diagnostics
while len(packets := xcvr.read()) != 0 or opts.loop:
    try:
        msg = DownlinkedPacketsMessage(DownlinkedPacketsContent(
            packets=packets
        ))
        manager.send_to('pub', msg, subtopic_bytes=b'pcap')
        n_packets_sent += len(packets)
        app.logger.notice(
            f"[{n_packets_sent:5d}] "
            f"Sent {msg.content.simple_str()} -> {ipc.Topic.DL_PACKETS}"
        )
    except Exception as e:
        app.logger.error(
            f"Uncaught exception in encoding packet(s): `{packets}`. \n"
            f"Exception: `{e}`."
        )
