"""
A CLI tool for sending pre-prepared commands via IPC.

Also serves as a real-world example of a synchronous IPC App with a manager.

Last Updated: 03/10/2023
"""
# Activate postponed annotations (for using classes as return type in their own methods)
from __future__ import annotations

import os
import time
import pynput  # type: ignore
import asyncio
import argparse
from datetime import datetime
import pandas as pd
from termcolor import colored
from enum import Enum as PyEnum
from collections import OrderedDict
from dataclasses import dataclass
from typing import Final, Callable, ClassVar, Tuple, Dict, List, cast

if __name__ == "__main__":
    # very slow (embedded) machines may take a while to import the library,
    # so print this first to let the user know things are happening:
    print("Booting . . .")
    # Add notice to not change focus so we can properly grab this window's
    # focus title after boot complete:
    print("DON'T CHANGE FOCUS TO ANOTHER WINDOW UNTIL BOOTED.")

import IrisBackendv3 as IB3
import IrisBackendv3.ipc as ipc
from IrisBackendv3.ipc.messages import (
    UplinkPacketRequestMessage,
    UplinkPacketRequestContent
)

from IrisBackendv3.utils import console_display
from IrisBackendv3.config import command_aliases
from IrisBackendv3.transceiver.xcvr_enum import TransceiverEnum
from IrisBackendv3.codec.magic import Magic
from IrisBackendv3.codec.metadata import DataPathway
from IrisBackendv3.codec.payload import CommandPayload

# Ensure consistent name for publisher socket:
PUB_SOCK_NAME: Final[str] = 'pub'

parser = argparse.ArgumentParser(description=(
    'IRIS Lunar Rover — IPC Commander — CLI'
))


def get_opts():
    parser.add_argument('-n', '--name', type=str, default="TelemetryDisplay",
                        help=(
                            "Name of this App (can be configured to "
                            "accomplish different goals)."
                        ))
    parser.add_argument('-c', '--command-aliases', type=str,
                        default="standard_command_aliases",
                        help=(
                            "Name of the command_aliases file in the "
                            f"`{command_aliases.DEFAULT_COMMAND_ALIASES_DIR}` "
                            "directory which defines which commands are "
                            "available."
                        ))
    return parser.parse_args()


@dataclass
class ConsoleState:
    """Current State of the console."""
    class Activity(str, PyEnum):
        # User input is currently going towards entering a command alias:
        ENTERING_ALIAS = "Entering Alias"
        # User input is currently going towards editing a command string:
        EDITING_COMMAND = "Editing Command"
        # Paused so the user can see a message:
        PAUSED = "Paused"
    # What the user is currently doing:
    activity: Activity = Activity.ENTERING_ALIAS
    # Input for the command alias
    alias_input: str = ""
    # Command string being edited:
    edited_command: str = ""
    # Index where a character should be inserted when editing a command:
    editing_cursor_idx: int = 0
    # Alias of the last sent command:
    last_sent_command_alias: str = ""
    # Last command string sent:
    last_sent_command_string: str = ""
    # What was being done before a pause:
    pre_pause_activity: Activity = Activity.ENTERING_ALIAS  # arbitrary default
    # Message to be displayed during a pause:
    pause_msg: str = ""

    def reset(self) -> None:
        self.activity = ConsoleState.Activity.ENTERING_ALIAS
        self.alias_input = ""
        self.edited_command = ""
        self.editing_cursor_idx = 0
        # Don't reset the memory of the last successful state
        # (that should be done explicitly if desired)

    @property
    def active_str(self) -> str:
        # Active_str is the string currently being edited:
        # (provides a single endpoint to make sure user input uses the right
        # string)
        if self.activity == ConsoleState.Activity.ENTERING_ALIAS:
            return self.alias_input
        elif self.activity == ConsoleState.Activity.EDITING_COMMAND:
            return self.edited_command
        else:
            return self.alias_input

    @active_str.setter
    def active_str(self, val: str) -> None:
        if self.activity == ConsoleState.Activity.ENTERING_ALIAS:
            self.alias_input = val
        elif self.activity == ConsoleState.Activity.EDITING_COMMAND:
            self.edited_command = val

    def advance_edit_cursor(self) -> None:
        self.editing_cursor_idx = min(
            self.editing_cursor_idx+1,
            len(self.edited_command)
        )

    def decrease_edit_cursor(self) -> None:
        self.editing_cursor_idx = max(0, self.editing_cursor_idx-1)


def print_and_pause(
    state: ConsoleState,
    msg: str,
    color='blue'
) -> None:
    state.pause_msg = colored(f"{msg}\nPress ESC to continue.", color)
    if state.activity != ConsoleState.Activity.PAUSED:
        # Save prev. state when *entering* into pause:
        state.pre_pause_activity = state.activity
    state.activity = ConsoleState.Activity.PAUSED


def print_error_and_pause(state: ConsoleState, err: str) -> None:
    print_and_pause(state, err, 'red')


def get_top_command(
    full_aliases_table: command_aliases.CommandAliasesTable,
    df_prepared_cmds: pd.DataFrame
) -> command_aliases.CommandAliasesTable.Entry | None:
    """Returns the `CommandAliasesTable` `Entry` in `full_aliases_table`
    corresponding to the top row in the (likely filtered) `df_prepared_cmds`
    `DataFrame`.
    Returns `None` if there are no entries in the dataframe (no rows).
    """
    if df_prepared_cmds.shape[0] == 0:
        # No entries
        return None
    # Grab command name from top row:
    command_alias = df_prepared_cmds.index[0]
    # Find the corresponding entry in the table:
    return full_aliases_table.get_entry(command_alias)


def update_display(
    full_aliases_table: command_aliases.CommandAliasesTable,
    df_prepared_cmds: pd.DataFrame,
    state: ConsoleState
) -> None:
    """Redraws the display given the current state of the console."""
    if state.activity == ConsoleState.Activity.PAUSED:
        print(state.pause_msg)
        return

    # Filter the dataframe based on the user input:
    filtered_df = console_display.filter_command_dataframe(
        partial_cmd_alias=state.alias_input,
        prepared_commands_dataframe=df_prepared_cmds
    )

    # Build console command view as a string:
    # User input:
    usr_cmd = console_display.str_user_command(
        user_prompt=console_display.USER_PROMPT_COMMAND,  # we only handle cmds here now
        user_cmd_input_str=state.alias_input,
        prepared_commands_dataframe=filtered_df,  # use pre-filtered frame
        filter_input=False
    )
    command_view = f"\n\nCommand: {usr_cmd}\n"
    # Comment of the top command in the filtered table:
    top_cmd_entry = get_top_command(full_aliases_table, filtered_df)
    # NOTE: this should just wrap automatically in most terminals if too long
    command_view += "\nTop Line Comment: \'"
    if top_cmd_entry is not None:
        command_view += top_cmd_entry.prepared_cmd.comment
    command_view += "\'\n"

    # Command Aliases Table:
    command_view += console_display.str_command_dataframe(filtered_df)

    # Clip the table height so it doesn't overflow the screen:
    _, term_lines = os.get_terminal_size()
    # Number of lines in the footer to save room for:
    n_footer_lines = 6
    command_view_lines = command_view.split('\n')[:(term_lines-n_footer_lines)]
    command_view = '\n'.join(command_view_lines)

    # Add command being edited to the bottom:
    if state.activity == ConsoleState.Activity.EDITING_COMMAND:
        # Add the prefix:
        command_view += "\n\nEdit Command before Sending: "
        # Add a space to the `edited_command` we're displaying so the cursor
        # can move one space past the end (so text can be added to the end):
        edited_command_disp = state.edited_command + ' '
        # Add the part before the cursor normally:
        command_view += colored(
            edited_command_disp[:state.editing_cursor_idx],
            'green',
            on_color='on_black'
        )
        # Invert the color at the cursor:
        command_view += colored(
            edited_command_disp[state.editing_cursor_idx],
            'black',
            on_color='on_green'
        )
        # Add the rest normally:
        command_view += colored(
            edited_command_disp[(state.editing_cursor_idx+1):],
            'green',
            on_color='on_black'
        )
        command_view += "\n ... or press ESC to go back ..."

    # Clear the screen and redraw it
    # (all together as once fast op so there's no flicker):
    console_display.clear_console()
    print(command_view)


def alias_entry_to_cmd_string(
    entry: command_aliases.CommandAliasesTable.Entry
) -> str:
    """Converts the given `CommandAliasesTable` `Entry` to a string
    representation (for editing)."""
    args_str = ', '.join([
        f"{n}=" + (f"'{v}'" if type(v) == str else str(v))
        for n, v in entry.prepared_cmd.args.items()
    ])
    return (
        f"{entry.prepared_cmd.magic.name}: "
        f"{entry.prepared_cmd.name} "
        f"[{args_str}] "
        f"-> {TransceiverEnum.ALL.name} "
        f"-> {entry.prepared_cmd.pathway.name}"
    )


def cmd_string_to_command(
    state: ConsoleState,
    cmd_str: str
) -> Tuple[CommandPayload | None, TransceiverEnum | None]:
    """Compiles the given command string in to a tuple containing a `CommandPayload` and a the `TransceiverEnum` specifying how it should be downlinked.
    Returns a `None, None` tuple if the command didn't validate.
    Command string is in the format from `alias_entry_to_cmd_string`.
    """
    orig_cmd_str: Final = cmd_str
    try:
        # Extract + Validate Magic:
        magic_str, cmd_str = cmd_str.split(':', 1)
        try:
            magic = Magic[magic_str.strip()]
        except:
            print_error_and_pause(
                state,
                f"Invalid magic: `{magic_str}`.\n"
                f"Valid options are: {[e.name for e in Magic]}."
            )
            return None, None

        # Extract + Validate Command Name:
        cmd_name, cmd_str = cmd_str.split('[', 1)
        cmd_name = cmd_name.strip()
        standards = IB3.get_codec_standards()
        try:
            # Validate command name:
            standards.global_command_lookup(cmd_name)
        except:
            print_error_and_pause(
                state,
                f"Invalid command name: `{cmd_name}`."
            )
            return None, None

        # Extract + Validate Arguments:
        args_str, cmd_str = cmd_str.split(']', 1)
        args = [a.split("=") for a in args_str.split(',')]
        args_dict = OrderedDict(
            (arg[0].strip(), arg[1].strip())
            for arg in args
        )
        # Cast any non-strings to numbers, remove quotes from strings:
        for k, v in args_dict.items():
            if (
                v.startswith("\'") and v.endswith("\'")
                or v.startswith("\"") and v.endswith("\"")
            ):
                # is a string surrounded by quotes. remove quotes
                args_dict[k] = v[1:-1]
            else:
                # no quotes equals not a string. has to be a number. Cast:
                n = float(v)
                # make it an int if it can be an int:
                if int(n) == n:
                    args_dict[k] = str(int(n))
                else:
                    args_dict[k] = str(n)

        # Extract and Validate the target XCVR:
        cmd_str = cmd_str.strip().removeprefix('->').strip()
        xcvr_str, cmd_str = cmd_str.split('->', 1)
        try:
            target_xcvr = TransceiverEnum[xcvr_str.strip()]
        except:
            print_error_and_pause(
                state,
                f"Invalid Transceiver: `{xcvr_str}`.\n"
                f"Valid options are: {[e.name for e in TransceiverEnum]}."
            )
            return None, None

        # Extract and Validate the pathway (everything that's left)
        pathway_str = cmd_str
        try:
            pathway = DataPathway[pathway_str.strip()]
        except:
            print_error_and_pause(
                state,
                f"Invalid DataPathway: `{pathway_str}`.\n"
                f"Valid options are: {[e.name for e in DataPathway]}."
            )
            return None, None

        # Build command:
        try:
            cmd_payload: CommandPayload = command_aliases.build_command_from_alias(
                command_aliases.PreparedCommand(
                    name=cmd_name,
                    args=args_dict,
                    pathway=pathway,
                    magic=magic,
                    comment="(doesn't matter here since we're just building a payload)"
                )
            )
        except Exception as e:
            print_error_and_pause(
                state,
                f"Command failed to build because: `{e!s}`."
            )
            return None, None

        return cmd_payload, target_xcvr
    except Exception as e:
        print_error_and_pause(
            state,
            f"Processing command string '{orig_cmd_str}' failed.\n"
            f"Likely the string format is wrong.\n"
            f"Format should be:\n"
            f"MAGIC: ModuleName_CommandName[arg1=1] -> TRANSCEIVER -> PATHWAY"
            f"\nFailed b/c:{e!s}."
        )
        return None, None


def uplink_command_request(
    state: ConsoleState,
    manager: ipc.IpcAppManagerSync | ipc.IpcAppManagerAsync,
    cmd_payload: CommandPayload,
    target_xcvr: TransceiverEnum
) -> bool:
    """
    Uses the given `IpcAppManager` to publish an IPC message requesting that
    the given `CommandPayload` be uplinked using the transceiver specified by `TransceiverEnum`.
    Returns whether the request was able to be built and sent successfully.
    """
    # Pack the command into a Packet:
    try:
        packet = IB3.codec.packet.IrisCommonPacket(
            seq_num=0,  # just using 0 for now
            payloads=IB3.codec.payload_collection.EnhancedPayloadCollection(
                CommandPayload=[
                    cmd_payload
                ]
            )
        )
    except Exception as e:
        print_error_and_pause(state, f"Packet failed to build b/c: `{e!s}`.")
        return False
    # Build the message:
    try:
        msg = UplinkPacketRequestMessage(UplinkPacketRequestContent(
            packet=packet,
            target_xcvr=target_xcvr
        ))
        # Send the Message:
        manager.send_to(PUB_SOCK_NAME, msg)
        print_and_pause(state, f"Sent {msg}!", 'green')
    except Exception as e:
        print_error_and_pause(
            state,
            f"Uplink Request failed to send b/c: `{e!s}`."
        )
        return False

    # Everything worked!:
    return True


def handle_keypress(
    key: pynput.keyboard.Key | pynput.keyboard.KeyCode | None,
    manager: ipc.IpcAppManagerSync | ipc.IpcAppManagerAsync,
    full_aliases_table: command_aliases.CommandAliasesTable,
    df_prepared_cmds: pd.DataFrame,
    state: ConsoleState
) -> None:
    """
    Handles one keypress and updates the screen state accordingly
    """
    # Get the dataframe filtered by current user input:
    filtered_df = console_display.filter_command_dataframe(
        partial_cmd_alias=state.alias_input,
        prepared_commands_dataframe=df_prepared_cmds
    )

    if key == pynput.keyboard.Key.esc:
        # Move backwards:
        if state.activity == ConsoleState.Activity.EDITING_COMMAND:
            state.activity = ConsoleState.Activity.ENTERING_ALIAS
        elif state.activity == ConsoleState.Activity.ENTERING_ALIAS:
            state.reset()
        elif state.activity == ConsoleState.Activity.PAUSED:
            state.activity = state.pre_pause_activity

    if state.activity == ConsoleState.Activity.PAUSED:
        # Reprint the exit key:
        print_error_and_pause(state, "")
        return  # Don't process any other input while paused

    if key == pynput.keyboard.Key.tab:
        # Accept first auto-complete suggestion:
        top_entry = get_top_command(full_aliases_table, filtered_df)
        if top_entry is not None:
            state.alias_input = top_entry.alias

    elif key == pynput.keyboard.Key.up:
        # Set the input to the value used for this stage the last time a
        # command was sent:
        if state.activity == ConsoleState.Activity.ENTERING_ALIAS:
            state.alias_input = state.last_sent_command_alias
        elif (
            state.activity == ConsoleState.Activity.EDITING_COMMAND
            and state.last_sent_command_string != ""
        ):
            state.edited_command = state.last_sent_command_string

    elif key == pynput.keyboard.Key.left:
        # Move the cursor if editing a command right now:
        if state.activity == ConsoleState.Activity.EDITING_COMMAND:
            state.decrease_edit_cursor()
    elif key == pynput.keyboard.Key.right:
        # Move the cursor if editing a command right now:
        if state.activity == ConsoleState.Activity.EDITING_COMMAND:
            state.advance_edit_cursor()

    elif key == pynput.keyboard.Key.enter:
        if state.activity == ConsoleState.Activity.ENTERING_ALIAS:
            # Only proceed to editing command if we've narrowed the dataframe
            # down to exactly *1* command OR the entered alias exactly matches
            # the top entry (handle case of one command being the subset of
            # another):
            # Generate the default command string and move to editing it:
            top_entry = get_top_command(full_aliases_table, filtered_df)
            if (top_entry is not None
                        and (
                            filtered_df.shape[0] == 1
                            or top_entry.alias == state.alias_input
                        )
                    ):
                state.edited_command = alias_entry_to_cmd_string(top_entry)
                state.activity = ConsoleState.Activity.EDITING_COMMAND
        elif state.activity == ConsoleState.Activity.EDITING_COMMAND:
            # Send:
            success_so_far: bool = True  # success (so far)
            cmd, xcvr = cmd_string_to_command(state, state.edited_command)
            success_so_far &= (cmd is not None and xcvr is not None)
            cmd, xcvr = cast(CommandPayload, cmd), cast(TransceiverEnum, xcvr)

            if success_so_far:
                success_so_far &= uplink_command_request(
                    state=state,
                    manager=manager,
                    cmd_payload=cmd,
                    target_xcvr=xcvr
                )

            if success_so_far:
                # Store the state at send (if it all worked):
                state.last_sent_command_alias = state.alias_input
                state.last_sent_command_string = state.edited_command

    elif key == pynput.keyboard.Key.backspace:
        if state.activity == ConsoleState.Activity.EDITING_COMMAND:
            # If editing a command, remove character before cursor:
            state.edited_command = (
                state.edited_command[:(state.editing_cursor_idx-1)]
                + state.edited_command[state.editing_cursor_idx:]
            )
            # ... and update the cursor position since it's now shifted:
            state.decrease_edit_cursor()
        else:
            # Remove the last character if backspace is pressed:
            state.active_str = state.active_str[:-1]

    elif (
        key == pynput.keyboard.Key.space
        or (isinstance(key, pynput.keyboard.KeyCode) and key.char is not None)
    ):
        if isinstance(key, pynput.keyboard.KeyCode):
            char = key.char
        else:
            char = ' '

        if state.activity == ConsoleState.Activity.EDITING_COMMAND:
            # If editing a command, insert character at cursor:
            state.edited_command = (
                state.edited_command[:state.editing_cursor_idx]
                + char
                + state.edited_command[state.editing_cursor_idx:]
            )
            # ... and update the cursor position since it's now shifted:
            state.advance_edit_cursor()
        else:
            state.active_str = state.active_str + char


def monitor_and_handle_keyboard(
    manager: ipc.IpcAppManagerSync | ipc.IpcAppManagerAsync,
    aliases_table: command_aliases.CommandAliasesTable,
    df_prepared_cmds: pd.DataFrame,
    state: ConsoleState
) -> None:
    # Get current window title so we can check focus:
    WINDOW_TITLE: Final = console_display.get_active_window_title()

    # Min time in milliseconds that a key must be held down for in order for it
    # to be considered pressed again (only for press-and-hold keys):
    PRESS_HOLD_DEBOUNCE_PERIOD_MS: Final[float] = 25

    # For debouncing, handle a table of the last time each key was pressed:
    key_hold_last_time: Dict[pynput.keyboard.Key |
                             pynput.keyboard.KeyCode, float] = dict()

    HOLDABLE_KEYS: Final[List[pynput.keyboard.Key | pynput.keyboard.KeyCode]] = [
        pynput.keyboard.Key.left,
        pynput.keyboard.Key.right,
        pynput.keyboard.Key.backspace
    ]

    def on_key_core(key):
        # Update window state:
        handle_keypress(
            key=key,
            manager=manager,
            full_aliases_table=aliases_table,
            df_prepared_cmds=df_prepared_cmds,
            state=state
        )
        # Redraw the screen:
        update_display(aliases_table, df_prepared_cmds, state)

    def on_key(key):
        # Only handle keypress if window is focused:
        if console_display.window_is_focused(WINDOW_TITLE):
            # Check the timing for any keys that are allowed to be held:
            if key in HOLDABLE_KEYS:
                # Only allow press-and-hold key operations to occur once per
                # `PRESS_HOLD_DEBOUNCE_PERIOD_MS`:
                now = time.time()
                last = key_hold_last_time.get(
                    key,
                    -PRESS_HOLD_DEBOUNCE_PERIOD_MS  # default to allow first
                )
                if (now - last) > PRESS_HOLD_DEBOUNCE_PERIOD_MS/1000:
                    key_hold_last_time[key] = now
                    on_key_core(key)

            else:
                # Just pass all other keys through:
                on_key_core(key)

    def on_press(key):
        # Allow press-and-hold operations for these keys:
        if key in HOLDABLE_KEYS:
            on_key(key)

    def on_release(key):
        if key not in HOLDABLE_KEYS:
            # holdable keys are handled by on_press
            on_key(key)

    # Collect events until released:
    # (release can happen by external signal like Ctrl+C or StopException)
    with pynput.keyboard.Listener(
        on_press=on_press,
        on_release=on_release
    ) as listener:
        listener.join()


def main(opts):
    # Load settings:
    IB3.init_from_latest()

    # Setup app:
    app = ipc.IpcAppHelper(opts.name)
    state = ConsoleState()

    manager = ipc.IpcAppManagerSync(socket_specs={
        PUB_SOCK_NAME: ipc.SocketSpec(
            sock_type=ipc.SocketType.PUBLISHER,
            port=ipc.Port.TRANSCEIVER_PUB,
            topics=[ipc.Topic.UL_PACKET]
        )
    })

    # Load available command aliases:
    app.logger.notice(f"Loading commands from {opts.command_aliases} . . .")
    aliases_table = command_aliases.CommandAliasesTable.load_from_file(
        opts.command_aliases
    )

    # Build dataframe of available command aliases:
    df_prepared_cmds = console_display.build_command_dataframe(
        prepared_commands=aliases_table.as_legacy_prepared_commands(),
        prepared_commands_dataframe=pd.DataFrame()
    )

    # Draw the first frame:
    update_display(aliases_table, df_prepared_cmds, state)

    # Let the keyboard listener drive things from here out:
    monitor_and_handle_keyboard(
        manager=manager,
        aliases_table=aliases_table,
        df_prepared_cmds=df_prepared_cmds,
        state=state
    )


if __name__ == "__main__":
    # Get CLI args:
    opts = get_opts()

    # Run the app:
    main(opts)
