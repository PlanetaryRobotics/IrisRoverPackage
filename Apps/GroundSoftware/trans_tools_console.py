"""
Helper functions to handle the console functions of `trans_tools`.

@author: Connor W. Colombo (CMU)
@last-updated: 08/30/2022
"""
from typing import Any, Final, List, OrderedDict, Type, cast, Union, Dict, Tuple, Optional

import re
import os
import subprocess
from datetime import datetime, timedelta

import socket
try:
    import pynput
except ImportError:
    # ignore import on pynput (if console is currently running, this will fail. Which is fine b/c console is already running.)
    pass
import numpy as np
import pandas as pd
from tabulate import tabulate
import scapy.all as scp  # type: ignore # no type hints
import itertools
import textwrap
from collections import deque

from termcolor import cprint

from IrisBackendv3.utils.basic import print_bytearray_hex as printraw
from IrisBackendv3.utils.basic import bytearray_to_spaced_hex as hexstr
from IrisBackendv3.utils.nameiddict import NameIdDict

from IrisBackendv3.data_standards import DataStandards
from IrisBackendv3.codec.metadata import DataPathway, DataSource
from IrisBackendv3.data_standards.fsw_data_type import FswDataType
from IrisBackendv3.data_standards.logging import logger as DsLogger
from IrisBackendv3.data_standards.prebuilt import add_to_standards, watchdog_heartbeat_tvac, watchdog_heartbeat, watchdog_command_response, watchdog_detailed_status_heartbeat
from IrisBackendv3.codec.payload import Payload, TelemetryPayload, CommandPayload, WatchdogCommandPayload
from IrisBackendv3.codec.payload_collection import EnhancedPayloadCollection, extract_downlinked_payloads
from IrisBackendv3.codec.packet import Packet, IrisCommonPacket, WatchdogTvacHeartbeatPacket, WatchdogHeartbeatPacket, WatchdogCommandResponsePacket, WatchdogDetailedStatusPacket
from IrisBackendv3.codec.magic import Magic, MAGIC_SIZE
from IrisBackendv3.codec.logging import logger as CodecLogger
from IrisBackendv3.codec.settings import ENDIANNESS_CODE, set_codec_standards

from __command_aliases import prepared_commands, Parameter

# Title of the window (helps with focus checking):
IRIS_CONSOLE_WINDOW_TITLE: Final[str] = "Iris Console"
# Actual title of the window (grabbed once at init, right after setting the window title, just in case setting it didn't work:
ACTUAL_REFERENCE_WINDOW_TITLE: Optional[str] = None
# Whether or not the console is currently paused:
console_paused: bool = False
# All dataframes for storing data:
telemetry_payload_log_dataframe: pd.DataFrame = pd.DataFrame(columns=['Opcode', 'Module', 'Channel', 'Num. Received', 'Last Updated', 'Current Value', 'H+1', 'H+2', 'H+3'], dtype=object).set_index('Opcode')
packet_log_dataframe: pd.DataFrame = pd.DataFrame(columns=['Packet Type', 'Num. Received', 'Last Updated', 'Avg. Interval [s]', 'Current Interval [s]', 'Bytes Received', 'Avg. bits/sec', 'Avg. bits/sec w/CCSDS'], dtype=object).set_index('Packet Type')
# LiFo Queue Log of the string prints of the most recent packets received that don't contain telemetry (i.e. logs, events, debug prints, etc.):
nontelem_packet_prints: deque = deque(maxlen=50)
# DataFrame of all prepared commands:
prepared_commands_dataframe: pd.DataFrame = pd.DataFrame(columns=['Alias', 'Pathway', 'Type', 'Command', 'Args'], dtype=object).set_index('Alias')
# String of all keys pressed so far (the user's current input):
user_cmd_input_str: str = ""
# Any arguments the user has supplied
user_args: OrderedDict[str, Any] = OrderedDict()
# Name of argument currently being filled:
current_user_arg: str = ""
# What the user is being asked to input:
USER_PROMPT_COMMAND: Final[str] = "Command"
USER_PROMPT_ARG: Final[str] = "Argument"
user_prompt: str = USER_PROMPT_COMMAND

DsLogger.setLevel('CRITICAL')
standards = DataStandards.build_standards()
add_to_standards(standards, [
    watchdog_detailed_status_heartbeat,
    watchdog_heartbeat_tvac,
    watchdog_heartbeat,
    watchdog_command_response
])
set_codec_standards(standards)

# Small general helper functions:
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

def set_window_title() -> None:
    print(f"\033]0;{IRIS_CONSOLE_WINDOW_TITLE}\a")

def get_active_window_title() -> Optional[str]:
    # Gets the title of the current window (used for focus checking)
    # Currently only works in windows:
    root = subprocess.Popen(['xprop', '-root', '_NET_ACTIVE_WINDOW'], stdout=subprocess.PIPE)
    stdout, stderr = root.communicate()

    m = re.search(b'^_NET_ACTIVE_WINDOW.* ([\w]+)$', stdout)
    if m is not None:
        window_id = m.group(1)
        window = subprocess.Popen(['xprop', '-id', window_id, 'WM_NAME'], stdout=subprocess.PIPE)
        stdout, stderr = window.communicate()
    else:
        return None

    match = re.match(b"WM_NAME\(\w+\) = (?P<name>.+)$", stdout)
    if match is not None:
        return match.group("name").strip(b'"').decode()
    return None

def update_reference_window_title():
    global ACTUAL_REFERENCE_WINDOW_TITLE
    ACTUAL_REFERENCE_WINDOW_TITLE = get_active_window_title()

def window_is_focused() -> bool:
    return get_active_window_title() == ACTUAL_REFERENCE_WINDOW_TITLE

def init_console_view() -> None:
    # Initializes the console view.
    # NOTE: Window needs to be in focus for this ... which it should be since the user *just* started the program.

    # Set the appropriate window title (helps with focus checking):

    # Make sure the console only captures input if this window is focused
    # (only works in bash, but, the same goes for the rest of this program).
    # Likely doesn't work on Windows, even if running bash (though *might* work in WSL - untested):
    set_window_title()
    
    # Immediately update the reference title (just in case setting the title didn't work on this platform):
    update_reference_window_title()

    # Make sure console starts unpaused:
    unpause_console()

    # Build the first frame:
    build_command_dataframe()

def build_command_dataframe() -> None:
    # Builds a DataFrame of all available prepared
    # commands (from `__command_aliases.py`):
    
    # Add each command alias to the dataframe:
    for alias in prepared_commands:
        pathway, magic, command_name, args, telem_pathway = prepared_commands[alias]
        new_row = {
            'Pathway': pathway.name,
            'Type': magic.name,
            'Command': ':\n'.join(command_name.split('_')),
            'Args': tabs2spaces('\n'.join( f"{k}:\n\t{'XXX' if v == Parameter.PASTE else v}" for k,v in cast(Dict, args).items() ))
        }
        # Save row:
        prepared_commands_dataframe.loc[alias, [*new_row.keys()]] = [*new_row.values()]

def filter_command_dataframe(partial_cmd_alias: str) -> pd.DataFrame:
    """
    Returns a copy of `prepared_commands_dataframe` that:
        A. only contains commands whose aliases contain `partial_cmd`.
        B. are sorted by what percentage of the command name matches `partial_cmd_alias`.
    """
    filtered = prepared_commands_dataframe.copy()
    # Grab only commands whose aliases contain the given partial_cmd_alias:
    filtered = filtered[filtered.apply(lambda row: partial_cmd_alias in row.name, axis=1)]
    # Sort by match percentage:
    filtered['match_percent'] = [len(partial_cmd_alias) / len(i) for i in filtered.index]
    filtered = filtered.sort_values(by='match_percent', ascending=False)
    filtered = filtered.drop('match_percent', axis=1)

    return filtered

# Pretty-Formats the given Command Dataframe (possibly after being filtered):
def str_command_dataframe(cdf: pd.DataFrame) -> str:
    table = tabulate(cdf.fillna(''), headers='keys', tablefmt='fancy_grid', numalign='left', stralign='left')
    
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

# Pretty formats the user command based on the `user_cmd_input_str`, highlighting the part that the user input (not auto-completed):
def str_user_command() -> str:
    if user_cmd_input_str == '':
        command = ""
    else:
        filtered_results = filter_command_dataframe(user_cmd_input_str)
        if filtered_results.shape[0] == 0:
            # There are no matches. Just return the input (but highlighted and red):
            command = f"\033[31;1m{user_cmd_input_str}\033[0m"
        else:
            top_result = filtered_results.index[0]

            command = top_result

            # If there's only one result, make the whole thing green:
            if filtered_results.shape[0] == 1:
                command = f"\033[37;42m{command}\033[0m"
                # And still highlight the part the user supplied:
                command = command.replace(user_cmd_input_str, f"\033[37;42;1m{user_cmd_input_str}\033[37;42m")
            else:
                # Else, just highlight the part the user supplied:
                command = command.replace(user_cmd_input_str, f"\033[1m{user_cmd_input_str}\033[0m")
    
    if user_prompt == USER_PROMPT_ARG:
        command += "["
        command += ','.join(
            f"{arg_name}: {arg_val if arg_val != Parameter.PASTE else 'XXX'}"
            for arg_name, arg_val in user_args.items()
        )
        command += "]"

    return command

def reset_console_command() -> None:
    global user_cmd_input_str, current_user_arg, user_prompt
    # Reset the command:
    user_cmd_input_str = ""
    current_user_arg = ""
    user_prompt = USER_PROMPT_COMMAND

def accept_top_suggestion() -> None:
    """
    Sets current command to top suggestion in command table if:
    A. The user is currently being prompted for a command.
    B. There is at least one suggestion present (i.e. the currently
       entered next needs to at least partially match something).
    C. At least one character has been entered so far (so you
       can't just hit tab-enter to send the top suggestion -
       could be accident prone).
    """
    global user_cmd_input_str, current_user_arg, user_prompt
    filtered_results = filter_command_dataframe(user_cmd_input_str)
    if (
        user_prompt == USER_PROMPT_COMMAND
        and filtered_results.shape[0] > 0
        and user_cmd_input_str != ""
    ):
        user_cmd_input_str = filtered_results.index[0]

def send_slip(dat: bytes, serial_writer) -> None:
    """
    Wraps the given data in SLIP and sends it over RS422,
    using the given serial interface `serial_writer`
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

    if serial_writer is not None:
        serial_writer.write(bytes(buf))
    else:
        raise Exception(
            "Can't send data, serial connection not started. Try `connect_serial()`.",
            'red')

def send_data_wd_serial(
    raw_data: bytes,
    serial_writer,
    ip_dest: str = '127.0.0.1',  # arbitrary (WD doesn't care)
    ip_src: str = '222.173.190.239',  # arbitrary (WD doesn't care)
    port: int = 8080  # arbitrary (WD doesn't care)
) -> None:
    # Send WD command over the givenn serial interface (serial_writer)
    # Build packet
    full_packet = scp.IP(dst=ip_dest, src=ip_src) / \
        scp.UDP(dport=port)/scp.Raw(load=raw_data)
    # printraw(scp.raw(scp.IP(scp.raw(full_packet))))
    # printraw(scp.raw(full_packet))
    data = cast(bytes, scp.raw(full_packet))
    send_slip(data, serial_writer)

def send_wifi_adv(data: bytes, ip="192.168.150.3", port=42000, src_ip="192.168.10.105", src_port=43531) -> None:
    # Craft UDP packet (incl. using the correct src_ip, even if that's not our own):
    full_packet = scp.IP(dst=ip, src=src_ip) / scp.UDP(sport=src_port, dport=port)/scp.Raw(load=data)
    # printraw(scp.raw(scp.IP(scp.raw(full_packet))))
    # printraw(scp.raw(full_packet))
    full_packet_data = scp.raw(full_packet)

    # Open raw socket and send:
    try:
        sock = socket.socket(socket.AF_INET, socket.SOCK_RAW, socket.IPPROTO_RAW)
        # Tell kernel not to add headers (we took care of that above):
        sock.setsockopt(socket.IPPROTO_IP, socket.IP_HDRINCL, 1)
        sock.sendto(full_packet_data, (ip, 0))
        sock.close()
    except PermissionError as pe:
        print("Failed to open socket for sending WiFi packets due to PermissionError. You should be running this as `sudo` - are you?")

def send_console_command(pathway: DataPathway, magic: Magic, command_name: str, kwargs: OrderedDict[str, Any], message_queue, serial_writer) -> None:
    # Attempt to send the given console command:

    # No matter what, we're done with this command now, so reset:
    reset_console_command()

    command_payload_type = {
        Magic.WATCHDOG_COMMAND: WatchdogCommandPayload,
        Magic.COMMAND: CommandPayload
    }[magic]

    # Try to convert any args to int (since they all would have come in as str):
    for arg_name, arg_val in kwargs.items():
        try:
            kwargs[arg_name] = int(arg_val, base=0)
        except ValueError:
            pass # this one's not convertible, that's fine

    module, command = standards.global_command_lookup(command_name)

    # Pack command:
    try:
        command_payload =  command_payload_type(
            pathway=pathway,
            source=DataSource.GENERATED,
            magic=magic,
            module_id=module.ID,
            command_id=command.ID,
            args=kwargs
        )
        payloads = EnhancedPayloadCollection(CommandPayload=[command_payload])
        packet = IrisCommonPacket(
            seq_num=0x00,
            payloads=payloads
        )
        packet_bytes = packet.encode()
        message_queue.put_nowait(f"Sending: {command_payload} . . .\n{hexstr(packet_bytes)}")
    except Exception as e:
        message_queue.put_nowait(f"An exception occured while trying to pack bytes for an outbound command: {e}")

    # Send command:
    try:
        if pathway == DataPathway.WIRED:
            send_data_wd_serial(packet_bytes, serial_writer)
        elif pathway == DataPathway.WIRELESS:
            send_wifi_adv(packet_bytes)
    except Exception as e:
        message_queue.put_nowait(f"An exception occured while trying to transmit an outbound command: {e}")


def attempt_console_command_send(message_queue, serial_writer) -> None:
    global user_args, current_user_arg, user_prompt

    # Only send if one command is locked in (only one result):
    filtered_results = filter_command_dataframe(user_cmd_input_str)
    if filtered_results.shape[0] == 1:
        command_alias = filtered_results.index[0]
        pathway, magic, command_name, kwargs, telem_pathway = prepared_commands[command_alias]

        if current_user_arg == "":
            # we're just starting this command, so fill up the buffer:
            user_args = cast(OrderedDict[str,Any], kwargs)

        current_user_arg == "" # reset user args pointer (look for any more paste values)
        # If there are any params still set to PASTE, set the pointer there:
        for arg_name, arg_val in user_args.items():
            if arg_val == Parameter.PASTE:
                current_user_arg = arg_name
                user_args[current_user_arg] = ""
                user_prompt = USER_PROMPT_ARG
                break
        
        # If the pointer is still empty, then all the arguments are accounted for,
        # and we can proceed with the send:
        if current_user_arg == "":
            send_console_command(pathway, magic, command_name, user_args, message_queue, serial_writer)


def handle_keypress(key: Any, message_queue, serial_writer) -> None:
    key = cast(Union[pynput.keyboard.Key, pynput.keyboard.KeyCode], key)
    # Handles new key input:
    global user_cmd_input_str, user_prompt
    
    something_changed: bool = False

    # If window is not focused, ignore input:
    if not window_is_focused():
        return

    if key == pynput.keyboard.Key.f2:
        # Toggle the console input/output pause state:
        toggle_console_pause()
        something_changed = True

    # If the console is paused, ignore input (besides the unpause command):
    if console_is_paused():
        return

    if key == pynput.keyboard.Key.esc:
        # Reset the command:
        reset_console_command()
        something_changed = True

    elif key == pynput.keyboard.Key.tab:
        # Accept first auto-complete suggestion:
        accept_top_suggestion()
        something_changed = True

    elif key == pynput.keyboard.Key.backspace:
        # Remove the last character if backspace is pressed:
        if user_prompt == USER_PROMPT_COMMAND:
            user_cmd_input_str = user_cmd_input_str[:-1]
            something_changed = True
        elif user_prompt == USER_PROMPT_ARG and current_user_arg in user_args:
            user_args[current_user_arg] = user_args[current_user_arg][:-1]
            something_changed = True
    
    elif key == pynput.keyboard.Key.space:
        if user_prompt == USER_PROMPT_COMMAND:
            user_cmd_input_str += ' '
            something_changed = True
        elif user_prompt == USER_PROMPT_ARG and current_user_arg in user_args:
            user_args[current_user_arg] += ' '
            something_changed = True

    elif key == pynput.keyboard.Key.enter:
        attempt_console_command_send(message_queue, serial_writer)
        something_changed = True
    
    elif isinstance(key, pynput.keyboard.KeyCode) and key.char is not None:
        # Add the given key to the user input:
        if user_prompt == USER_PROMPT_COMMAND:
            user_cmd_input_str += key.char
            something_changed = True
        elif user_prompt == USER_PROMPT_ARG and current_user_arg in user_args:
            user_args[current_user_arg] += key.char
            something_changed = True

    # Redraw the screen so the key input is reflected (only if something changed):
    if something_changed:
        refresh_console_view()


def update_telemetry_payload_log_dataframe(t: TelemetryPayload) -> None:
    # Update the given row in the telemetry payload log dataframe (used for tabular printing)
    # with data from the given telemetry payload `t`.

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


# Creates a new Console view by pretty-formatting all log DataFrames into one big string:
def create_console_view() -> str:
    # Settings:
    horiz_padding = tabs2spaces("\t\t")
    term_cols, term_lines = os.get_terminal_size()
    # Adjust by subtracing bottom and right side buffers (from experimentation):
    term_lines = term_lines - 2 # -1 for padding -1 for help message on bottom row
    term_cols = term_cols - 2

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
    right_side_max_width = term_cols - left_side_max_width - len(horiz_padding)
    # Packets Table:
    packets_view = (
        "\n\nPackets:\n"
        +str_packet_log_dataframe()
    )
    packets_view_lines = [tabs2spaces(x) for x in packets_view.split('\n')]

    # Commands list:
    command_view = (
        f"\n\n{user_prompt}: {str_user_command()}\n"
        +str_command_dataframe(filter_command_dataframe(user_cmd_input_str))
    )
    command_view_lines = [tabs2spaces(x) for x in command_view.split('\n')]
    command_view_width = max(len_noCodes(l) for l in command_view_lines)
    command_view_lines = [l.ljust(command_view_width, ' ') for l in command_view_lines]
    # Cap the size of the command view:
    command_view_max_len = term_lines - len(packets_view_lines)
    command_view_lines = command_view_lines[:command_view_max_len]

    # Non-Telemetry Packets log:
    # Get the boundaries of the area we're going to squeeze this into:
    plog_header_lines = [' ', ' ', 'Non-Telemetry Packets:', '---------------------------']
    plog_footer_lines = [' ']
    plog_width = right_side_max_width - len(horiz_padding) - command_view_width
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
    plog_lines = [tabs2spaces(line) for clump in plog_line_wraps for line in clump] # Reflatten
    
    # Make sure formatting ends at end of each line (this would be broken if a formatted line was word wrapped -
    # that's why we're doing it here and not at the end; only need it after word wrap):
    plog_lines = [line + "\033[0m" for line in plog_lines]

    # Grab all lines up to the limit and then tack on header and footer:
    plog_lines = plog_header_lines + plog_lines[:plog_max_len] + plog_footer_lines
    # Make sure no line exceeds max width:
    plog_lines = [l[:plog_width] for l in plog_lines]
    # Make sure it's fixed width:
    plog_lines = [l + ' '*(plog_width-len_noCodes(l)) for l in plog_lines]

    # Build the lower right section (plog on left, command view on right):
    if len(command_view_lines) > len(plog_lines):
        # add extra lines that match the max width:
        plog_lines += [' ' * plog_width] * (len(command_view_lines) - len(plog_lines))
    lower_right_lines = [a + horiz_padding + b for a,b in itertools.zip_longest(plog_lines, command_view_lines, fillvalue="")]

    # Build the right-hand side (packets_view on top of lower right side):
    right_side_lines = packets_view_lines + lower_right_lines

    # Make sure it's fixed width:
    right_side_lines = [l.ljust(right_side_max_width, ' ') for l in right_side_lines]
    
    # Add notice to the upper right:
    notice_lines = ['\033[231;41;1m Iris Lunar Rover Console \033[0m', '(use in full-size terminal) ']
    notice_width = max(len_noCodes(l) for l in notice_lines)
    notice_lines = [l.rjust(notice_width, ' ') for l in notice_lines]
    for i, line in enumerate(notice_lines):
        right_side_lines[i] = right_side_lines[i][:-len_noCodes(line)] + line

    # Make sure left side has at least as many lines as right side:
    if len(right_side_lines) > len(left_side_lines):
        # add extra lines that match the max width:
        left_side_lines += [' ' * left_side_max_width] * (len(right_side_lines) - len(left_side_lines))



    ## Join left and right sides:
    all_lines = [a + horiz_padding + b for a,b in itertools.zip_longest(left_side_lines, right_side_lines, fillvalue="")]

    # Make sure formatting resets before each line starts:
    all_lines = ["\033[0m" + line for line in all_lines]

    # Pad out total number of lines to max height:
    all_lines += [' '] * (term_lines - len(all_lines))

    ## Add help message to bottom line:
    all_lines += [ljust_noCodes(
        f"\033[37;40m    Type to enter command."
        f"    Press \033[1mTab\033[22m to accept autocomplete."
        f"    Press \033[1mEnter\033[22m when \033[32mgreen\033[37m to send."
        f"    Press \033[1mEscape\033[22m to reset input."
        f"    Press \033[1mF2\033[22m to pause/unpause console input/output (packets are still received, just not displayed)."
        f"    Press \033[1mCtrl+'\\'\033[22m to end session.", term_cols) + "\033[0m"]

    ### Build the full message:
    full_str = '\n'.join(all_lines) + "\033[0m" # make sure formatting is reset at the end

    return full_str

# "Pauses" the Console window:
def pause_console() -> None:
    global console_paused
    console_paused = True

# "Unpauses" the Console window:
def unpause_console() -> None:
    global console_paused
    console_paused = False

# Returns whether the console is currently paused:
def console_is_paused() -> bool:
    return console_paused

# Toggles console pause state:
def toggle_console_pause() -> None:
    if console_is_paused():
        unpause_console()
    else:
        pause_console()

# Clears the Console Window:
def clear_console() -> None:
    os.system('cls' if os.name in ('nt', 'dos') else 'clear')

# Refreshes (clears) the Console Window with Highlevel Telemetry Dataview:
def refresh_console_view() -> None:
    # Build the string before clearing the screen to minimize off-time:
    # (only update console view if not paused):
    if not console_is_paused():
        data = create_console_view()
        clear_console()
        print(data)
