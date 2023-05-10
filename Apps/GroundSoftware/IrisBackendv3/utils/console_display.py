"""
Tools extracted from Iris Console for making a CLI window full of
semi-interactive DataFrames. This contains **only** the console display
management tools. Actual keyboard based interaction still lives in
`trans_tools_console` and hasn't been broken out yet.

NOTE: This is largely extracted from `trans_tools_console` (and a bit from
`trans_tools`) which was a hack built on top of a hack. Everything that's been
extracted from it is relatively good (it works) but the architecture is garbage,
namely this currently is designed to interact with individual variables but
really should be a stateful system instead which mutates a singular console
state struct.
These functions could probably be wrapped pretty easily to make it stateful but
that's super low priority right now since it all works.

@author: Connor W. Colombo (CMU)
@last-updated: 03/08/2023
"""
from __future__ import annotations  # Support things like OrderedDict[A,B]
from typing import Any, Final, List, Type, cast, Union, Dict, Tuple, Optional

import re
import os
import sys
import subprocess
from datetime import datetime, timedelta

if sys.platform == "darwin":
    from AppKit import NSWorkspace
    from Quartz import (
        CGWindowListCopyWindowInfo,
        kCGWindowListOptionOnScreenOnly,
        kCGNullWindowID
    )

import socket
try:
    import pynput  # type: ignore
except ImportError:
    # ignore import on pynput (if console is currently running, this will fail. Which is fine b/c console is already running.)
    pass
import numpy as np
import pandas as pd  # type: ignore
from tabulate import tabulate  # type: ignore
import scapy.all as scp  # type: ignore # no type hints
import itertools
import textwrap
from collections import deque, OrderedDict

from termcolor import cprint

from IrisBackendv3.utils.basic import print_bytearray_hex as printraw
from IrisBackendv3.utils.basic import bytearray_to_spaced_hex as hexstr
from IrisBackendv3.utils.nameiddict import NameIdDict

from IrisBackendv3.data_standards import DataStandards
from IrisBackendv3.codec.metadata import DataPathway, DataSource
from IrisBackendv3.data_standards.fsw_data_type import FswDataType
from IrisBackendv3.data_standards.logs import logger as DsLogger
from IrisBackendv3.data_standards.logs import logger_setConsoleLevel as DsLoggerLevel
from IrisBackendv3.data_standards.prebuilt import ALL_PREBUILT_MODULES, add_to_standards, watchdog_heartbeat_tvac, watchdog_heartbeat, watchdog_command_response, watchdog_detailed_status_heartbeat
from IrisBackendv3.codec.payload import Payload, TelemetryPayload, CommandPayload, EventPayload, WatchdogCommandPayload
from IrisBackendv3.codec.payload_collection import EnhancedPayloadCollection, extract_downlinked_payloads
from IrisBackendv3.codec.packet import (
    Packet,
    IrisCommonPacket,
    WatchdogDetailedStatusPacket,
    WatchdogCommandResponsePacket,
    RadioUartBytePacket
)


from IrisBackendv3.codec.magic import Magic, MAGIC_SIZE
from IrisBackendv3.codec.logs import logger as CodecLogger
from IrisBackendv3.codec.settings import ENDIANNESS_CODE, set_codec_standards

from scripts.utils.__command_aliases import prepared_commands, Parameter, PreparedCommandType

USER_PROMPT_COMMAND: Final[str] = "Command"
USER_PROMPT_ARG: Final[str] = "Argument"

##
# Small general helper functions:
##


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


def ljust_noCodes(x: str, targ_len: int, padding_char: str = ' ') -> str:
    # Pads given string out to `targ_len` using `padding_char`
    # (like `ljust`), **NOT** counting ANSI escape codes in
    # the length.
    # NOTE: If `padding_char` is more than one character long, only the
    # first character will be used for padding. If it's no characters
    # long, ' ' (space) will be used.
    padding_char = padding_char[0] if len(padding_char) > 0 else ' '
    x += padding_char * (targ_len - len_noCodes(x))
    return x


def set_window_title(title) -> None:
    print(f"\033]0;{title}\a")


def get_active_window_title() -> Optional[str]:
    # Gets the title of the current window (used for focus checking)
    # Currently only works in linux:
    try:
        if sys.platform == "darwin":
            app = NSWorkspace.sharedWorkspace().frontmostApplication()
            active_app_name = app.localizedName()

            options = kCGWindowListOptionOnScreenOnly
            windowList = CGWindowListCopyWindowInfo(options, kCGNullWindowID)
            windowTitle = 'Unknown'
            for window in windowList:
                ownerName = window['kCGWindowOwnerName']
                winNum = window.get('kCGWindowNumber', u'Unknown')
                # Grab the window name if there, otherwise just use number:
                windowTitle = window.get('kCGWindowName', f'WinNum={winNum}')
                if windowTitle and ownerName == active_app_name:
                    break

            return f"{active_app_name}: {windowTitle}"
        else:
            root = subprocess.Popen(
                ['xprop', '-root', '_NET_ACTIVE_WINDOW'], stdout=subprocess.PIPE)
            stdout, stderr = root.communicate()

            m = re.search(b'^_NET_ACTIVE_WINDOW.* ([\w]+)$', stdout)
            if m is not None:
                window_id = m.group(1)
                window = subprocess.Popen(
                    ['xprop', '-id', window_id, 'WM_NAME'], stdout=subprocess.PIPE)
                stdout, stderr = window.communicate()
            else:
                return None

            match = re.match(b"WM_NAME\(\w+\) = (?P<name>.+)$", stdout)
            if match is not None:
                return match.group("name").strip(b'"').decode()
            return None
    except:
        return None


def update_reference_window_title() -> str:
    return str(get_active_window_title())


def window_is_focused(actual_reference_window_title: str) -> bool:
    return str(get_active_window_title()) == actual_reference_window_title


def clear_console() -> None:
    # Clears the Console Window:
    os.system('cls' if os.name in ('nt', 'dos') else 'clear')

##
# Command table functions:
##


def init_command_dataframe(prepared_commands_dataframe: pd.DataFrame) -> pd.DataFrame:
    if len(prepared_commands_dataframe.columns) == 0:
        prepared_commands_dataframe = pd.DataFrame(
            columns=['Alias', 'Pathway', 'Type', 'Command', 'Args'],
            dtype=object
        ).set_index('Alias')
    return prepared_commands_dataframe


def build_command_dataframe(
    prepared_commands: Dict[str, PreparedCommandType],
    prepared_commands_dataframe: pd.DataFrame
) -> pd.DataFrame:
    # Builds a DataFrame of all available prepared commands
    # (from `__command_aliases.py`):

    # Check if the dataframe has been initialized and init if not:
    prepared_commands_dataframe = init_command_dataframe(
        prepared_commands_dataframe)

    # Add each command alias to the dataframe:
    for alias in prepared_commands:
        pathway, magic, command_name, args, telem_pathway = prepared_commands[alias]
        new_row = {
            'Pathway': pathway.name,
            'Type': magic.name,
            'Command': ':\n'.join(command_name.split('_')),
            'Args': tabs2spaces('\n'.join(f"{k}:\n\t{'XXX' if v == Parameter.PASTE else v}" for k, v in cast(Dict, args).items()))
        }
        # Save row:
        prepared_commands_dataframe.loc[alias, [*new_row.keys()]] = \
            [*new_row.values()]

    return prepared_commands_dataframe


def filter_command_dataframe(
    partial_cmd_alias: str,
    prepared_commands_dataframe: pd.DataFrame
) -> pd.DataFrame:
    """
    Returns a copy of `prepared_commands_dataframe` that:
        A. only contains commands whose aliases contain `partial_cmd`.
        B. are sorted by what percentage of the command name matches `partial_cmd_alias`.
    """
    if partial_cmd_alias == "":
        # If the input is nothing, just return the input table:
        return prepared_commands_dataframe
    filtered = prepared_commands_dataframe.copy()
    # Grab only commands whose aliases contain the given partial_cmd_alias:
    filtered = filtered[filtered.apply(
        lambda row: partial_cmd_alias in row.name, axis=1)]
    # Sort by match percentage:
    filtered['match_percent'] = [
        len(partial_cmd_alias) / len(i) for i in filtered.index]
    filtered = filtered.sort_values(by='match_percent', ascending=False)
    filtered = filtered.drop('match_percent', axis=1)

    return filtered


def str_command_dataframe(cdf: pd.DataFrame) -> str:
    # Pretty-Formats the given Command Dataframe (possibly after being filtered):
    table: str = tabulate(cdf.fillna(''), headers='keys',
                          tablefmt='fancy_grid', numalign='left', stralign='left')

    # If there's only one entry (we've locked it in), make table green:
    if cdf.shape[0] == 1:
        table_lines = table.split('\n')
        for i, line in enumerate(table_lines):
            table_lines[i] = f"\033[32m{line}\033[0m"
        table = '\n'.join(table_lines)

    # If there are no entries (no matches), make table red:
    if cdf.shape[0] == 0:
        table_lines = table.split('\n')
        for i, line in enumerate(table_lines):
            table_lines[i] = f"\033[31m{line}\033[0m"
        table = '\n'.join(table_lines)

    return table


def str_user_command(
    user_prompt: str,
    user_cmd_input_str: str,
    prepared_commands_dataframe: pd.DataFrame,
    filter_input: bool = True  # whether the input still needs to be filtered
) -> str:
    # Pretty formats the user command based on the `user_cmd_input_str`, highlighting the part that the user input (not auto-completed):
    if user_cmd_input_str == '':
        command = ""
    else:
        if filter_input:
            filtered_results = filter_command_dataframe(
                partial_cmd_alias=user_cmd_input_str,
                prepared_commands_dataframe=prepared_commands_dataframe
            )
        else:
            filtered_results = prepared_commands_dataframe
        if filtered_results.shape[0] == 0:
            # There are no matches. Just return the input (but highlighted and red):
            command = f"\033[31;1m{user_cmd_input_str}\033[0m"
        else:
            top_result = filtered_results.index[0]

            command = top_result

            # If there's only one result or out input matches the top result exactly (i.e. we've locked it in), make the whole thing green:
            if filtered_results.shape[0] == 1 or user_cmd_input_str == filtered_results.index[0]:
                command = f"\033[37;42m{command}\033[0m"
                # And still highlight the part the user supplied:
                command = command.replace(
                    user_cmd_input_str, f"\033[37;42;1m{user_cmd_input_str}\033[37;42m")
            else:
                # Else, just highlight the part the user supplied:
                command = command.replace(
                    user_cmd_input_str, f"\033[1m{user_cmd_input_str}\033[0m")

    if user_prompt == USER_PROMPT_ARG:
        command += "["
        command += ','.join(
            f"{arg_name}: {arg_val if arg_val != Parameter.PASTE else 'XXX'}"
            for arg_name, arg_val in user_args.items()  # type: ignore
        )
        command += "]"

    return command


def reset_console_command() -> Tuple[str, str, str]:
    # Reset the command:
    user_cmd_input_str = ""
    current_user_arg = ""
    user_prompt = USER_PROMPT_COMMAND
    return user_cmd_input_str, current_user_arg, user_prompt


def accept_top_suggestion(user_cmd_input_str: str, user_prompt: str, prepared_commands_dataframe: pd.DataFrame) -> str:
    """
    Sets current command to top suggestion in command table if:
    A. The user is currently being prompted for a command.
    B. There is at least one suggestion present (i.e. the currently
       entered next needs to at least partially match something).
    C. At least one character has been entered so far (so you
       can't just hit tab-enter to send the top suggestion -
       could be accident prone).
    """
    filtered_results = filter_command_dataframe(
        user_cmd_input_str, prepared_commands_dataframe)
    if (
        user_prompt == USER_PROMPT_COMMAND
        and filtered_results.shape[0] > 0
        and user_cmd_input_str != ""
    ):
        user_cmd_input_str = filtered_results.index[0]

    return user_cmd_input_str


##
# TelemetryPayload table functions:
##


def init_telemetry_payload_log_dataframe(telemetry_payload_log_dataframe: pd.DataFrame) -> pd.DataFrame:
    if len(telemetry_payload_log_dataframe.columns) == 0:
        telemetry_payload_log_dataframe = pd.DataFrame(
            columns=[
                'Opcode', 'Module', 'Channel', 'nRX', 'Updated', 'Current Value', 'H+1', 'H+2', 'H+3'
            ],
            dtype=object
        ).set_index('Opcode')
    return telemetry_payload_log_dataframe


def format_telem_value(val: Any) -> str:
    if (
        isinstance(val, int)
        or isinstance(val, float) and int(val) == val
    ):
        return str(int(val))
    if isinstance(val, float):
        return f"{val:.6g}"
    else:
        return str(val)


def update_telemetry_payload_log_dataframe(
    telemetry_payload_log_dataframe: pd.DataFrame,
    t: TelemetryPayload
) -> pd.DataFrame:
    # Update the given row in the telemetry payload log dataframe (used for tabular printing)
    # with data from the given telemetry payload `t`.

    # Init dataframe if not init'd yet:
    telemetry_payload_log_dataframe = init_telemetry_payload_log_dataframe(
        telemetry_payload_log_dataframe)

    # Format value (if enum):
    if t.channel.is_enum:
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
            'nRX': old_row['nRX'] + 1,
            'Updated': datetime.now(),
            'Current Value': format_telem_value(val),
            'H+1': old_row['Current Value'],
            'H+2': old_row['H+1'],
            'H+3': old_row['H+2']
        }
    else:
        new_data = {
            'Module': t.module.name,
            'Channel': t.channel.name,
            'nRX': 1,
            'Updated': datetime.now(),
            'Current Value': format_telem_value(val),
            'H+1': "",
            'H+2': "",
            'H+3': ""
        }

    # Save row:
    telemetry_payload_log_dataframe.loc[t.opcode, [*new_data.keys()]] = \
        [*new_data.values()]

    return telemetry_payload_log_dataframe


def update_telemetry_payload_log_from_payloads(telemetry_payload_log_dataframe: pd.DataFrame, payloads: EnhancedPayloadCollection) -> pd.DataFrame:
    telem = cast(List[TelemetryPayload], [*payloads[TelemetryPayload]])
    for t in telem:
        telemetry_payload_log_dataframe = update_telemetry_payload_log_dataframe(
            telemetry_payload_log_dataframe, t)
    return telemetry_payload_log_dataframe


def update_telemetry_payload_log_from_packet(telemetry_payload_log_dataframe: pd.DataFrame, packet: Packet) -> pd.DataFrame:
    return update_telemetry_payload_log_from_payloads(telemetry_payload_log_dataframe, packet.payloads)


def str_telemetry_payload_log_dataframe(telemetry_payload_log_dataframe: pd.DataFrame) -> str:
    # Pretty-Formats Telemetry Payload Log Dataframe:
    df_out = telemetry_payload_log_dataframe.copy()
    df_out.index = df_out.index.map(lambda x: f"0x{x:04X}")
    df_out['Updated'] = [x.strftime('%H:%M:%S') for x in df_out['Updated']]
    return str(tabulate(df_out.fillna('').sort_index(ascending=True), headers='keys', tablefmt='fancy_grid', numalign='right', stralign='right'))


##
# Packet table functions:
##


def update_all_packet_log_times(packet_log_dataframe: pd.DataFrame, now: datetime) -> None:
    # Updates the `current_interval` times for every packet (done in response
    # to ticks so we can see how long its been since we've received something):
    for row_name in packet_log_dataframe.index:
        row = packet_log_dataframe.loc[row_name]
        current_interval = (now - row['Updated']).total_seconds()
        row['Current Dt [s]'] = current_interval


def init_packet_log_dataframe(packet_log_dataframe: pd.DataFrame) -> pd.DataFrame:
    if len(packet_log_dataframe.columns) == 0:
        packet_log_dataframe = pd.DataFrame(
            columns=[
                'Packet Type', 'nRX', 'nRS422', 'nWIFI', 'Updated',
                'Avg. Dt [s]', 'Current Dt [s]', 'Bytes Received',
                'Avg. bits/sec', 'Avg. bits/sec w/CCSDS'
            ],
            dtype=object
        ).set_index('Packet Type')
    return packet_log_dataframe


def update_packet_log_dataframe_row(
    packet_log_dataframe: pd.DataFrame,
    row_name: str,
    now: datetime,
    num_bytes: int,
    is_rs422: bool,
    is_wifi: bool
) -> pd.DataFrame:
    # Update the given row in the packet log dataframe (used for tabular printing) for a packet
    # containing `num_bytes` received at time `now`.

    # Initialize dataframe if not initialized:
    packet_log_dataframe = init_packet_log_dataframe(packet_log_dataframe)

    if row_name in packet_log_dataframe.index:  # 'Packet Type',
        # Row already exists for this field, just update it:
        old_row = packet_log_dataframe.loc[row_name]
        total_num_recv = old_row['nRX'] + 1
        current_interval = (now - old_row['Updated']).total_seconds()
        avg_interval = (
            ((old_row['nRX']-1) * old_row['Avg. Dt [s]']) + current_interval) / old_row['nRX']
        total_bytes_recv = old_row['Bytes Received'] + num_bytes
        new_data = {
            'Updated': now,
            'nRX': total_num_recv,
            'nRS422': old_row['nRS422'] + int(is_rs422),
            'nWIFI': old_row['nWIFI'] + int(is_wifi),
            'Avg. Dt [s]': avg_interval,
            'Current Dt [s]': current_interval,
            'Bytes Received': total_bytes_recv,
            'Avg. bits/sec':  total_bytes_recv / total_num_recv / avg_interval,
            'Avg. bits/sec w/CCSDS': (total_bytes_recv + total_num_recv*14*8) / total_num_recv / avg_interval
        }
    else:
        new_data = {
            'Updated': now,
            'nRX': 1,
            'nRS422': int(is_rs422),
            'nWIFI': int(is_wifi),
            'Avg. Dt [s]': 0,
            'Current Dt [s]': 0,
            'Bytes Received': num_bytes,
            'Avg. bits/sec': 0,
            'Avg. bits/sec w/CCSDS': 0
        }
    packet_log_dataframe.loc[row_name, [*new_data.keys()]] = \
        [*new_data.values()]
    return packet_log_dataframe


def update_packet_log_dataframe(packet_log_dataframe: pd.DataFrame, packet: Packet) -> pd.DataFrame:
    # Update packet log dataframe (used for tabular printing) after
    # receiving the given packet:
    if isinstance(packet, Packet):
        # Update the dataframe for this packet:
        now = datetime.now()
        name = packet.__class__.__name__
        num_bytes = len(
            packet.raw if packet.raw is not None
            else packet.encode()
        )

        is_rs422 = packet.pathway == DataPathway.WIRED
        is_wifi = packet.pathway == DataPathway.WIRELESS

        packet_log_dataframe = update_packet_log_dataframe_row(
            packet_log_dataframe,
            name, now, num_bytes, is_rs422, is_wifi
        )

        # Update total across all packets:
        packet_log_dataframe = update_packet_log_dataframe_row(
            packet_log_dataframe,
            'zTotal', now, num_bytes, is_rs422, is_wifi
        )
    return packet_log_dataframe


def str_packet_log_dataframe(packet_log_dataframe: pd.DataFrame) -> str:
    # Pretty-Formats Packet Log Dataframe:
    df_out = packet_log_dataframe.copy()
    df_out['Current Dt [s]'] = [f"{x:.3f}" for x in df_out['Current Dt [s]']]
    df_out['Avg. Dt [s]'] = [f"{x:.3f}" for x in df_out['Avg. Dt [s]']]
    df_out['Updated'] = [
        x.isoformat(sep=' ', timespec='milliseconds')
        for x in df_out['Updated']
    ]
    return str(tabulate(df_out.fillna('').sort_index(ascending=True), headers='keys', tablefmt='fancy_grid', floatfmt=".3f", numalign='right', stralign='right'))


def packet_print_string(
    packet: Optional[Packet],
    datetime_format: str = '%m-%d %H:%M:%S'
) -> str:
    # Creates a "Print" string of the given packet, along with accompanying metadata like the current time:
    return (
        f"\033[35;47;1m({datetime.now().strftime(datetime_format)})\033[0m "
        f"\033[48;5;248m\033[38;5;233m\033[1m {packet.pathway.name if packet is not None else 'NONE'} \033[0m "
        f"{packet!s}"
    )


def packet_to_messages(
    packet: Packet,
    echo_packet_classes: bool = False,
    echo_message_packets: bool = True,
    echo_events: bool = True,
    echo_all_packet_bytes: bool = False,
    datetime_format: str = '%m-%d %H:%M:%S'
) -> List[str]:
    """Creates a list of messages that are contained in this packet.

    `echo_packet_classes` Whether or not to print the packet class when
        any packet is received.
    `echo_message_packets` Whether or not to print message packets (i.e.
        packets which are, themselves, a message - i.e. command response,
        reset ack, etc.).
    `echo_events` Whether or not to print events inside payloads.
    `echo_all_packet_bytes` Whether or not to print the packet bytes when
        any packet is received.
    """
    messages: List[str] = []

    # Echo packet class:
    if echo_packet_classes:
        messages.append(
            f"Received: {packet.__class__.__name__}"
        )

    # If the packet doesn't contain any telemetry or events (i.e. log,
    # debug print, etc.), add it to the messages list in LiFo manner:
    # - Also do this for WatchdogDetailedStatusPacket since they're *very* detailed (contain way too much data to display so we're just
    # going to display it here instead).
    # - Also push command responses to the prints section so they're seen explicitly (its packet printer includes a special parser to decode the command name)
    # - So long as it's not a `RadioUartBytePacket` (they clog the interface):
    if echo_message_packets:
        if (
            (len([*packet.payloads[TelemetryPayload]]) == 0  # no telem
             and len([*packet.payloads[EventPayload]]) == 0  # no events
             or isinstance(packet, (  # has telem/events but is special:
                WatchdogDetailedStatusPacket,
                WatchdogCommandResponsePacket
                 )))
            and not isinstance(packet, (  # just never print these:
                RadioUartBytePacket
            ))
        ):
            messages.append(packet_print_string(packet, datetime_format))

    # Extract any events:
    if echo_events:
        events = [*packet.payloads[EventPayload]]
        for event in events:
            # Push directly to the queue:
            # ... the handle_streamed_packet will take care of the refreshing
            messages.append(
                f"\033[35;47;1m({datetime.now().strftime(datetime_format)})\033[0m {event!s}"
            )

    # If requested, tack on the packet bytes:
    if echo_all_packet_bytes:
        # Echo the data out...
        echo_bytes: bytes = packet.raw if packet.raw is not None else b''
        prefix = ''
        if packet.pathway == DataPathway.WIRED:
            prefix = 'RS422 '
        elif packet.pathway == DataPathway.WIRELESS:
            prefix = 'WIFI '
        # To the console:
        messages.append(
            f"{prefix}Packet Bytes:\n-----------------\n" +
            scp.hexdump(echo_bytes, dump=True)
        )

    return messages
