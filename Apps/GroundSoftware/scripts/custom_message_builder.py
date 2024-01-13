"""
Simple script to generate NoOp echo command binaries from messages.

@author: Connor W. Colombo (CMU)
@last-updated: 01/13/2024
"""
from typing import cast, Final, List, Dict, Type
from datetime import datetime, timedelta

import argparse
import time

from IrisBackendv3.logs import VALID_LOG_LEVELS

import IrisBackendv3 as IB3
import IrisBackendv3.ipc as ipc
from IrisBackendv3.ipc.messages import (
    DownlinkedPayloadsMessage, DownlinkedPayloadsContent
)
from IrisBackendv3.codec.payload_collection import EnhancedPayloadCollection
from IrisBackendv3.codec.payload import (
    TelemetryPayload, EventPayload, CommandPayload
)
from IrisBackendv3.codec.magic import Magic
from IrisBackendv3.codec.metadata import DataPathway, DataSource

from ipc_apps.dl_processor_lib.timestamping import RoverTimeEstimator

IB3.init_from_latest()

# Setup:
app = ipc.IpcAppHelper("MessageBuilder")
manager = ipc.IpcAppManagerSync(socket_specs={
    'sub': ipc.SocketSpec(
        sock_type=ipc.SocketType.SUBSCRIBER,
        port=ipc.Port.TRANSCEIVER_SUB,
        topics=[ipc.Topic.DL_PAYLOADS]
    ),
})


parser = argparse.ArgumentParser(description=(
    'IRIS Lunar Rover — Send a Custom Message — CLI'
))


def get_opts(
    default_log_level: str = 'VERBOSE'
):
    parser.add_argument('-n', '--sent-by', type=str, default="Earth",
                        help="Name of person sending this message.")
    # parser.add_argument('-m', '--message', type=str, default="Hi, Iris!",
    #                     help="Custom message to be sent.")
    parser.add_argument('-d', '--roundtrip-delay-sec', type=float, default=0,
                        help="Amount to sleep to simulate roundtrip delay.")
    parser.add_argument('-f', '--file', type=str, default="./messages.xlsx",
                        help="Spreadsheet of custom message to load.")

    def str_to_log_level(s):
        if isinstance(s, str) and s.upper() in VALID_LOG_LEVELS:
            return s.upper()
        else:
            raise argparse.ArgumentTypeError(
                f'Valid log level expected. Log levels are: {VALID_LOG_LEVELS}'
            )

    parser.add_argument('--log-level', type=str_to_log_level, default=default_log_level,
                        help=('Logging level to be used (i.e. how annoying the logging '
                              'printouts should be). Only logs with importance greater '
                              'than or equal to the specified logging level are '
                              f'displayed. Valid logging levels are: {VALID_LOG_LEVELS}'))

    parsed_opts = parser.parse_known_args()
    opts = parsed_opts[0]
    opts.message = ' '.join(parsed_opts[1])
    return opts


# Run:
if __name__ == "__main__":
    opts = get_opts()
    app.setLogLevel(opts.log_level)

    COMMAND_NAME: Final[str] = "CommandDispatcher_Cmdnoopstring"
    MAX_MESSAGE_LEN: Final[int] = 39
    COUNTER_BUFFER_LEN: Final[int] = 4  # " X/Y"

    sender = opts.sent_by
    message = opts.message
    app.logger.info(f"{sender}: '{message}'. Sending . . .")

    # Split into sub-messages as needed:
    sub_messages: List[str] = []
    if len(message) <= MAX_MESSAGE_LEN:
        sub_messages = [message]
    else:
        n_msgs: int = 0
        head: int = 0
        while head < len(message):
            sub_message_data = message[
                head:(head+MAX_MESSAGE_LEN-COUNTER_BUFFER_LEN)
            ]
            sub_message = sub_message_data + f" {n_msgs+1}/"
            sub_messages.append(sub_message)
            n_msgs += 1
            head += len(sub_message_data)
        sub_messages = [sm + str(n_msgs) for sm in sub_messages]

    standards = IB3.get_codec_standards()
    module, command = standards.global_command_lookup(COMMAND_NAME)
    command_payloads = [
        CommandPayload(
            pathway=DataPathway.WIRED,
            source=DataSource.GENERATED,
            magic=Magic.COMMAND,
            module_id=module.ID,
            command_id=command.ID,
            args=dict(arg_1=sm[:MAX_MESSAGE_LEN])
        )
        for sm in sub_messages
    ]

    # Build dummy uplink packet:
    packets = [
        IB3.codec.packet.IrisCommonPacket(
            seq_num=0,  # just using 0 for now
            payloads=IB3.codec.payload_collection.EnhancedPayloadCollection(
                CommandPayload=[command_payload]
            )
        )
        for command_payload in command_payloads
    ]

    for sub_message, command, packet in zip(sub_messages, command_payloads, packets):
        time.sleep(opts.roundtrip_delay_sec)
        pkt_data_str = ''.join(f'{x:02x}' for x in packet.encode())
        pathway_str = command.pathway.yamcs_suffix
        app.logger.success(
            f"\t > {sender}: \t '{sub_message}' \t -> \t {command} !"
        )
        app.logger.verbose(
            f"\t\t > {command.command.name}_{pathway_str}:\t"
            f"0x {pkt_data_str} !"
        )
    app.logger.notice("MESSAGES COMPLETE.")
