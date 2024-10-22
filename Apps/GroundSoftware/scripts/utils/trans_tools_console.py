"""
Helper functions to handle the console functions of `trans_tools`.

NOTE: This was project was a hackathon, so the code quality is pretty rough but
it has been used a lot and seems to work fine.

@author: Connor W. Colombo (CMU)
@last-updated: 04/08/2023
"""
from __future__ import annotations  # Support things like OrderedDict[A,B]
from typing import Any, Final, List, Type, cast, Union, Dict, Tuple, Optional

import re
import os
import subprocess
from datetime import datetime, timedelta

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
from IrisBackendv3.meta.metafield import add_metamodules_to_standards
from config.metafields import ALL_META_MODULES
from IrisBackendv3.data_standards.prebuilt import ALL_PREBUILT_MODULES, add_to_standards, watchdog_heartbeat_tvac, watchdog_heartbeat, watchdog_command_response, watchdog_detailed_status_heartbeat
from IrisBackendv3.codec.payload import Payload, TelemetryPayload, CommandPayload, WatchdogCommandPayload
from IrisBackendv3.codec.payload_collection import EnhancedPayloadCollection, extract_downlinked_payloads
from IrisBackendv3.codec.packet_classes.packet import Packet
from IrisBackendv3.codec.packet_classes.iris_common import IrisCommonPacket

from IrisBackendv3.codec.magic import Magic, MAGIC_SIZE
from IrisBackendv3.codec.logs import logger as CodecLogger
from IrisBackendv3.codec.settings import ENDIANNESS_CODE, set_codec_standards

from scripts.utils.__command_aliases import prepared_commands, Parameter, PreparedCommandType

from IrisBackendv3.utils.console_display import *


DsLoggerLevel('CRITICAL')
standards = DataStandards.build_standards()
add_to_standards(standards, ALL_PREBUILT_MODULES)
add_metamodules_to_standards(standards, ALL_META_MODULES)
set_codec_standards(standards)


class IrisConsoleDisplayDriver:
    """
    Drives the display of Iris Console.
    Basically just wraps a bunch of hacky functions from iris_console.
    NOTE: `iris_console` is generally deprecated in favor of equivalent IPC
    apps. This class was only made to allow for legacy support of iris_console
    in the new IPC era.
    """
    # Title of the window (helps with focus checking):
    IRIS_CONSOLE_WINDOW_TITLE: Final[str] = "Iris Console"
    # Actual title of the window (grabbed once at init, right after setting the window title, just in case setting it didn't work:
    ACTUAL_REFERENCE_WINDOW_TITLE: Optional[str]
    # Whether or not the console is currently paused:
    console_paused: bool
    # All dataframes for storing data:
    telemetry_payload_log_dataframe: pd.DataFrame
    packet_log_dataframe: pd.DataFrame
    # LiFo Queue Log of the string prints of the most recent packets received that don't contain telemetry (i.e. logs, events, debug prints, etc.):
    nontelem_packet_prints: deque
    # String of all keys pressed so far (the user's current input):
    user_cmd_input_str: str
    # Any arguments the user has supplied
    user_args: OrderedDict[str, Any]
    # Name of argument currently being filled:
    current_user_arg: str
    # What the user is being asked to input:
    user_prompt: str
    # Name of the last sent command (for quick re-sending):
    last_sent_command_name: Optional[str]

    # DataFrame of all prepared commands:
    prepared_commands_dataframe: pd.DataFrame = pd.DataFrame()

    def __init__(self) -> None:
        self.ACTUAL_REFERENCE_WINDOW_TITLE = None
        self.console_paused = False
        self.telemetry_payload_log_dataframe = pd.DataFrame()
        self.packet_log_dataframe = pd.DataFrame()
        self.nontelem_packet_prints = deque(maxlen=50)
        self.user_cmd_input_str = ""
        self.user_args = OrderedDict()
        self.current_user_arg = ""
        self.user_prompt = USER_PROMPT_COMMAND
        self.last_sent_command_name = None
        self.prepared_commands_dataframe = pd.DataFrame()

    def init_console_view(self) -> None:
        # Initializes the console view.
        # NOTE: Window needs to be in focus for this ... which it should be since the user *just* started the program.

        # Set the appropriate window title (helps with focus checking):

        # Make sure the console only captures input if this window is focused
        # (only works in bash, but, the same goes for the rest of this program).
        # Likely doesn't work on Windows, even if running bash (though *might* work in WSL - untested):
        set_window_title(self.IRIS_CONSOLE_WINDOW_TITLE)

        # Immediately update the reference title (just in case setting the title didn't work on this platform):
        self.ACTUAL_REFERENCE_WINDOW_TITLE = update_reference_window_title()

        # Make sure console starts unpaused:
        self.unpause_console()

        # Initialize all dataframes:
        self.packet_log_dataframe = init_packet_log_dataframe(
            self.packet_log_dataframe)
        self.telemetry_payload_log_dataframe = init_telemetry_payload_log_dataframe(
            self.telemetry_payload_log_dataframe)
        self.prepared_commands_dataframe = init_command_dataframe(
            self.prepared_commands_dataframe)

        # Build the first frame:
        self.prepared_commands_dataframe = build_command_dataframe(
            prepared_commands,
            self.prepared_commands_dataframe
        )

    @classmethod
    def send_slip(cls, dat: bytes, serial_writer) -> None:
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
                "Can't send data, serial connection not started. Try `connect_serial()`."
            )

    @classmethod
    def send_data_wd_serial(
        cls,
        raw_data: bytes,
        serial_writer,
        ip_dest: str = '127.0.0.1',  # arbitrary (WD doesn't care)
        ip_src: str = '222.173.190.239',  # arbitrary (WD doesn't care)
        port: int = 8080  # arbitrary (WD doesn't care)
    ) -> None:
        # Send WD command over the given serial interface (serial_writer)
        # Build packet
        full_packet = scp.IP(dst=ip_dest, src=ip_src) / \
            scp.UDP(dport=port)/scp.Raw(load=raw_data)
        # printraw(scp.raw(scp.IP(scp.raw(full_packet))))
        # printraw(scp.raw(full_packet))
        data = cast(bytes, scp.raw(full_packet))
        cls.send_slip(data, serial_writer)

    @classmethod
    def send_wifi_adv(cls, data: bytes, app_context, gateway_send_force: bool = False) -> None:
        # Sends an UDP packet to the rover over wifi (INET really).
        # Only works if sending from a machine (NIC) bound to the lander IP while a
        # separate machine (NIC), bound to the gateway ip, is hosting the network
        # which the rover is also connected to.
        #
        # If `gateway_send_force`, this forces it to go through (use this only if
        # sending from the gateway but you need it to look like its coming from the
        # lander). You likely will need to run as sudo for this feature to work.

        ip = app_context['wifi_settings']['rover_ip']
        port = app_context['wifi_settings']['rover_port']
        gwy_ip = app_context['wifi_settings']['gateway_ip']
        src_ip = app_context['wifi_settings']['lander_ip']
        src_port = app_context['wifi_settings']['lander_port']

        if not gateway_send_force:
            # Main way of doing this if connected to network on separate machine
            # with lander address:
            sock = cast(socket.socket, app_context['udp_server'].socket)
            sock.sendto(data, (ip, port))
        else:
            # Craft UDP packet (incl. using the correct src_ip, even if that's not our own):
            full_packet = scp.IP(dst=ip, src=src_ip) / \
                scp.UDP(sport=src_port, dport=port)/scp.Raw(load=data)
            # printraw(scp.raw(scp.IP(scp.raw(full_packet))))
            # printraw(scp.raw(full_packet))
            full_packet_data = scp.raw(full_packet)

            # Open raw socket and send:
            try:
                sock = socket.socket(
                    socket.AF_INET, socket.SOCK_RAW, socket.IPPROTO_RAW)
                # Tell kernel not to add headers (we took care of that above):
                sock.setsockopt(socket.IPPROTO_IP, socket.IP_HDRINCL, 1)
                sock.sendto(full_packet_data, (ip, port))
                sock.close()
            except PermissionError as pe:
                print("Failed to open socket for sending WiFi packets due to PermissionError. You should be running this as `sudo` - are you?")

    def send_console_command(self, pathway: DataPathway, magic: Magic, command_name: str, kwargs: OrderedDict[str, Any], app_context, message_queue, serial_writer) -> None:
        # Attempt to send the given console command:

        # No matter what, we're done with this command now, so reset:
        source_user_cmd_input_str = self.user_cmd_input_str  # store before resetting
        self.user_cmd_input_str, self.current_user_arg, self.user_prompt = reset_console_command()

        command_payload_type = {
            Magic.WATCHDOG_COMMAND: WatchdogCommandPayload,
            Magic.RADIO_COMMAND: CommandPayload,
            Magic.COMMAND: CommandPayload
        }[magic]

        # Try to convert any args to int (since they all would have come in as str):
        for arg_name, arg_val in kwargs.items():
            try:
                if isinstance(arg_val, int):
                    kwargs[arg_name] = arg_val
                else:
                    kwargs[arg_name] = int(str(arg_val), base=0)
            except ValueError:
                pass  # this one's not convertible, that's fine

        module, command = standards.global_command_lookup(command_name)

        # Pack command:
        packet_bytes = b''
        try:
            command_payload = command_payload_type(
                pathway=pathway,
                source=DataSource.GENERATED,
                magic=magic,
                module_id=module.ID,
                command_id=command.ID,
                args=kwargs
            )
            payloads = EnhancedPayloadCollection(
                CommandPayload=[command_payload])
            packet = IrisCommonPacket(
                seq_num=0x00,
                payloads=payloads
            )
            packet_bytes = packet.encode()
            if pathway == DataPathway.WIRED:
                path = "RS422"
            elif pathway == DataPathway.WIRELESS:
                path = (
                    "WIFI ["
                    f"{app_context['wifi_settings']['lander_ip']}:{app_context['wifi_settings']['lander_port']}"
                    f"->{app_context['wifi_settings']['rover_ip']}:{app_context['wifi_settings']['rover_port']}"
                    "]"
                )
            message_queue.put_nowait(
                f"\033[1m\033[48;5;15m\033[38;5;34m({datetime.now().strftime('%m-%d %H:%M:%S')})\033[0m "
                f"Sending over {path}: {command_payload} . . .\n{hexstr(packet_bytes)}"
            )
        except Exception as e:
            message_queue.put_nowait(
                f"An exception occurred while trying to pack bytes for an outbound command: {e}")

        # Send command:
        if len(packet_bytes) > 0:
            # If we get here, we we're able to send the command. Store it:
            self.last_sent_command_name = source_user_cmd_input_str

            try:
                if pathway == DataPathway.WIRED:
                    self.send_data_wd_serial(packet_bytes, serial_writer)
                elif pathway == DataPathway.WIRELESS:
                    # Application Context (settings, flags, etc. to be passed by reference to functions that need it):
                    self.send_wifi_adv(packet_bytes, app_context)
            except Exception as e:
                message_queue.put_nowait(
                    f"An exception occurred while trying to transmit an outbound command: {e}")

    def attempt_console_command_send(self, app_context, message_queue, serial_writer) -> None:
        # Only send if one command is locked in (only one result OR our input exactly matches the first result):
        filtered_results = filter_command_dataframe(
            self.user_cmd_input_str,
            self.prepared_commands_dataframe
        )
        if filtered_results.shape[0] == 1 or filtered_results.shape[0] > 1 and self.user_cmd_input_str == filtered_results.index[0]:
            command_alias = filtered_results.index[0]
            pathway, magic, command_name, kwargs, telem_pathway = prepared_commands[
                command_alias]

            if self.current_user_arg == "":
                # we're just starting this command, so fill up the buffer:
                self.user_args = cast('OrderedDict[str, Any]', kwargs)

            # reset user args pointer (look for any more paste values)
            self.current_user_arg == ""
            # If there are any params still set to PASTE, set the pointer there:
            for arg_name, arg_val in self.user_args.items():
                if arg_val == Parameter.PASTE:
                    self.current_user_arg = arg_name
                    self.user_args[self.current_user_arg] = ""
                    self.user_prompt = USER_PROMPT_ARG
                    break

            # If the pointer is still empty, then all the arguments are accounted for,
            # and we can proceed with the send:
            if self.current_user_arg == "":
                self.send_console_command(pathway, magic, command_name,
                                          self.user_args, app_context, message_queue, serial_writer)

    def handle_keypress(self, key: Any, message_queue, serial_writer, app_context) -> None:
        # Handles new key input:
        key = cast(Union[pynput.keyboard.Key, pynput.keyboard.KeyCode], key)
        something_changed: bool = False

        # If window is not focused, ignore input:
        if not window_is_focused(self.ACTUAL_REFERENCE_WINDOW_TITLE):
            return

        if key == pynput.keyboard.Key.f2:
            # Toggle the console input/output pause state:
            self.toggle_console_pause()
            something_changed = True

        # If the console is paused, ignore input (besides the unpause command above):
        if self.console_is_paused():
            return

        if key == pynput.keyboard.Key.f3:
            # Toggle the packet and SLIP byte printing:
            app_context['echo_all_packet_slip'] = not app_context['echo_all_packet_slip']
            app_context['echo_all_packet_bytes'] = not app_context['echo_all_packet_bytes']
            something_changed = True

        if key == pynput.keyboard.Key.f4:
            # Toggle showing/hiding the command table:
            app_context['collapse_command_table'] = not app_context['collapse_command_table']
            something_changed = True

        if key == pynput.keyboard.Key.esc:
            # Reset the command:
            self.user_cmd_input_str, self.current_user_arg, self.user_prompt = reset_console_command()
            something_changed = True

        elif key == pynput.keyboard.Key.tab:
            # Accept first auto-complete suggestion:
            self.user_cmd_input_str = accept_top_suggestion(
                self.user_cmd_input_str, self.user_prompt, self.prepared_commands_dataframe
            )
            something_changed = True

        elif key == pynput.keyboard.Key.up:
            # Set the input to the name of the last sent command
            # (for quick resending):
            if self.last_sent_command_name is not None:
                self.user_prompt = USER_PROMPT_COMMAND
                self.user_cmd_input_str = self.last_sent_command_name
                something_changed = True

        elif key == pynput.keyboard.Key.backspace:
            # Remove the last character if backspace is pressed:
            if self.user_prompt == USER_PROMPT_COMMAND:
                self.user_cmd_input_str = self.user_cmd_input_str[:-1]
                something_changed = True
            elif self.user_prompt == USER_PROMPT_ARG and self.current_user_arg in self.user_args:
                self.user_args[self.current_user_arg] = self.user_args[self.current_user_arg][:-1]
                something_changed = True

        elif key == pynput.keyboard.Key.space:
            if self.user_prompt == USER_PROMPT_COMMAND:
                self.user_cmd_input_str += ' '
                something_changed = True
            elif self.user_prompt == USER_PROMPT_ARG and self.current_user_arg in self.user_args:
                self.user_args[self.current_user_arg] += ' '
                something_changed = True

        elif key == pynput.keyboard.Key.enter:
            self.attempt_console_command_send(
                app_context, message_queue, serial_writer)
            something_changed = True

        elif isinstance(key, pynput.keyboard.KeyCode) and key.char is not None:
            # Add the given key to the user input:
            if self.user_prompt == USER_PROMPT_COMMAND:
                self.user_cmd_input_str += key.char
                something_changed = True
            elif self.user_prompt == USER_PROMPT_ARG and self.current_user_arg in self.user_args:
                self.user_args[self.current_user_arg] += key.char
                something_changed = True

        # Redraw the screen so the key input is reflected (only if something changed):
        if something_changed:
            self.refresh_console_view(app_context)

    def create_console_view(self, app_context) -> str:
        # Creates a new Console view by pretty-formatting all log DataFrames into one big string:

        # Settings:
        horiz_padding = tabs2spaces("\t\t")
        term_cols, term_lines = os.get_terminal_size()
        # Adjust by subtracting bottom and right side buffers (from experimentation):
        term_lines = term_lines - 2  # -1 for padding -1 for help message on bottom row
        term_cols = term_cols - 2

        # Build each section:
        # Build the left side:
        # Telemetry Table:
        telem_view = (
            "\n\nTelemetry:\n"
            + str_telemetry_payload_log_dataframe(self.telemetry_payload_log_dataframe)
        )
        telem_view_lines = [tabs2spaces(x) for x in telem_view.split('\n')]
        telem_view_width = max(len(l) for l in telem_view_lines)
        telem_view_lines = [l.ljust(telem_view_width, ' ')
                            for l in telem_view_lines]

        # Build the left-hand side (just telem view):
        left_side_lines = telem_view_lines
        left_side_max_width = telem_view_width

        # Build the right side:
        right_side_max_width = term_cols - \
            left_side_max_width - len(horiz_padding)
        # Packets Table:
        packets_view = (
            "\n\nPackets:\n"
            + str_packet_log_dataframe(self.packet_log_dataframe)
        )
        packets_view_lines = [tabs2spaces(x) for x in packets_view.split('\n')]

        # Commands list:
        usr_cmd = str_user_command(
            self.user_prompt, self.user_cmd_input_str, self.prepared_commands_dataframe)
        command_view = f"\n\n{self.user_prompt}: {usr_cmd}\n"
        if not app_context['collapse_command_table']:
            # Include the command table only if it's not collapsed:
            command_view += str_command_dataframe(
                filter_command_dataframe(self.user_cmd_input_str,
                                         self.prepared_commands_dataframe)
            )
        else:
            command_view += "[Table collapsed. F4 to uncollapse.]"
        command_view_lines = [tabs2spaces(x) for x in command_view.split('\n')]
        command_view_width = max(len_noCodes(l) for l in command_view_lines)
        command_view_lines = [l.ljust(command_view_width, ' ')
                              for l in command_view_lines]
        # Cap the size of the command view:
        command_view_max_len = term_lines - len(packets_view_lines)
        command_view_lines = command_view_lines[:command_view_max_len]

        # Non-Telemetry Packets log:
        # Get the boundaries of the area we're going to squeeze this into:
        plog_header_lines = [
            ' ', ' ', 'Non-Telemetry Packets:', '---------------------------']
        plog_footer_lines = [' ']
        plog_width = right_side_max_width - \
            len(horiz_padding) - command_view_width
        plog_max_len = term_lines - \
            len(plog_header_lines) - \
            len(packets_view_lines) - len(plog_footer_lines)
        # Build the raw (ideal / un-squeezed) output:
        # add escape close to ensure end of formatting for each line
        plog_raw = tabs2spaces('\n\n'.join([
            tabs2spaces(x).strip() + "\033[0m" for x in self.nontelem_packet_prints
        ]))
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
                plog_line_wraps.append(textwrap.wrap(
                    line, width=plog_width, replace_whitespace=False, break_on_hyphens=False, tabsize=4))
        plog_lines = [tabs2spaces(
            line) for clump in plog_line_wraps for line in clump]  # Reflatten

        # Make sure formatting ends at end of each line (this would be broken if a formatted line was word wrapped -
        # that's why we're doing it here and not at the end; only need it after word wrap):
        plog_lines = [line + "\033[0m" for line in plog_lines]

        # Grab all lines up to the limit and then tack on header and footer:
        plog_lines = plog_header_lines + \
            plog_lines[:plog_max_len] + plog_footer_lines
        # Make sure no line exceeds max width:
        plog_lines = [l[:plog_width] for l in plog_lines]
        # Make sure it's fixed width:
        plog_lines = [l + ' '*(plog_width-len_noCodes(l)) for l in plog_lines]

        # Build the lower right section (plog on left, command view on right):
        if len(command_view_lines) > len(plog_lines):
            # add extra lines that match the max width:
            plog_lines += [' ' * plog_width] * \
                (len(command_view_lines) - len(plog_lines))
        lower_right_lines = [a + horiz_padding + b for a,
                             b in itertools.zip_longest(plog_lines, command_view_lines, fillvalue="")]

        # Build the right-hand side (packets_view on top of lower right side):
        right_side_lines = packets_view_lines + lower_right_lines

        # Make sure it's fixed width:
        right_side_lines = [l.ljust(right_side_max_width, ' ')
                            for l in right_side_lines]

        # Add notice to the upper right:
        notice_lines = [
            '\033[231;41;1m Iris Lunar Rover Console \033[0m', '(use in full-size terminal) ']
        notice_width = max(len_noCodes(l) for l in notice_lines)
        notice_lines = [l.rjust(notice_width, ' ') for l in notice_lines]
        for i, line in enumerate(notice_lines):
            right_side_lines[i] = right_side_lines[i][:-
                                                      len_noCodes(line)] + line

        # Make sure left side has at least as many lines as right side:
        if len(right_side_lines) > len(left_side_lines):
            # add extra lines that match the max width:
            left_side_lines += [' ' * left_side_max_width] * \
                (len(right_side_lines) - len(left_side_lines))

        # Join left and right sides:
        all_lines = [a + horiz_padding + b for a,
                     b in itertools.zip_longest(left_side_lines, right_side_lines, fillvalue="")]

        # Make sure formatting resets before each line starts:
        all_lines = ["\033[0m" + line for line in all_lines]

        # Pad out total number of lines to max height:
        all_lines += [' '] * (term_lines - len(all_lines))

        # Trim to match console height:
        all_lines = all_lines[:term_lines]

        # Add help message to bottom line:
        all_lines += [ljust_noCodes(
            f"\033[37;40m    Type commands."
            f"    Press \033[1mTab\033[22m to accept autocomplete."
            f"    Press \033[1mEnter\033[22m when \033[32mgreen\033[37m to send."
            f"    Press \033[1mEscape\033[22m to reset."
            f"    Press \033[1mF2\033[22m to un/pause console in/output (packets RX'd, just not displayed)."
            f"    Press \033[1mF3\033[22m to start/stop printing bytes for \033[1mall\033[22m packets."
            f"    Press \033[1mF4\033[22m to un/collapse command table."
            f"    Press \033[1mCtrl+'\\'\033[22m to end session.", term_cols) + "\033[0m"]

        # Build the full message:
        # make sure formatting is reset at the end
        full_str = '\n'.join(all_lines) + "\033[0m"

        return full_str

    def pause_console(self) -> None:
        # "Pauses" the Console window:
        self.console_paused = True

    def unpause_console(self) -> None:
        # "Unpauses" the Console window:
        self.console_paused = False

    def console_is_paused(self) -> bool:
        # Returns whether the console is currently paused:
        return self.console_paused

    def toggle_console_pause(self) -> None:
        # Toggles console pause state:
        if self.console_is_paused():
            self.unpause_console()
        else:
            self.pause_console()

    def refresh_console_view(self, app_context) -> None:
        # Refreshes (clears) the Console Window with High-level Dataview:

        # Build the string before clearing the screen to minimize off-time:
        # (only update console view if not paused):
        # (also don't draw in debug mode)
        if not app_context['debug-mode'] and not self.console_is_paused():
            # Always use this opportunity to update the dataframe timestamps:
            now = datetime.now()
            update_all_packet_log_times(self.packet_log_dataframe, now)

            # Build the screen string:
            data = self.create_console_view(app_context)

            # Update the screen:
            clear_console()
            print(data)
