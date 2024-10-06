Module IrisBackendv3.utils.console_display
==========================================
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

Functions
---------

    
`CGWindowListCopyWindowInfo(arg0, arg1, /)`
:   CFArrayRef CGWindowListCopyWindowInfo(unsigned int arg0, unsigned int arg1);

    
`accept_top_suggestion(user_cmd_input_str: str, user_prompt: str, prepared_commands_dataframe: pd.DataFrame) ‑> str`
:   Sets current command to top suggestion in command table if:
    A. The user is currently being prompted for a command.
    B. There is at least one suggestion present (i.e. the currently
       entered next needs to at least partially match something).
    C. At least one character has been entered so far (so you
       can't just hit tab-enter to send the top suggestion -
       could be accident prone).

    
`build_command_dataframe(prepared_commands: Dict[str, PreparedCommandType], prepared_commands_dataframe: pd.DataFrame) ‑> pandas.core.frame.DataFrame`
:   

    
`clear_console() ‑> None`
:   

    
`filter_command_dataframe(partial_cmd_alias: str, prepared_commands_dataframe: pd.DataFrame) ‑> pandas.core.frame.DataFrame`
:   Returns a copy of `prepared_commands_dataframe` that:
        A. only contains commands whose aliases contain `partial_cmd`.
        B. are sorted by what percentage of the command name matches `partial_cmd_alias`.

    
`format_telem_value(val: Any) ‑> str`
:   

    
`get_active_window_title() ‑> str | None`
:   

    
`init_command_dataframe(prepared_commands_dataframe: pd.DataFrame) ‑> pandas.core.frame.DataFrame`
:   

    
`init_packet_log_dataframe(packet_log_dataframe: pd.DataFrame) ‑> pandas.core.frame.DataFrame`
:   

    
`init_telemetry_payload_log_dataframe(telemetry_payload_log_dataframe: pd.DataFrame) ‑> pandas.core.frame.DataFrame`
:   

    
`len_noCodes(x: str) ‑> int`
:   

    
`ljust_noCodes(x: str, targ_len: int, padding_char: str = ' ') ‑> str`
:   

    
`packet_print_string(packet: Optional[Packet], datetime_format: str = '%m-%d %H:%M:%S') ‑> str`
:   

    
`packet_to_messages(packet: Packet, echo_packet_classes: bool = False, echo_message_packets: bool = True, echo_events: bool = True, echo_all_packet_bytes: bool = False, datetime_format: str = '%m-%d %H:%M:%S') ‑> List[str]`
:   Creates a list of messages that are contained in this packet.
    
    `echo_packet_classes` Whether or not to print the packet class when
        any packet is received.
    `echo_message_packets` Whether or not to print message packets (i.e.
        packets which are, themselves, a message - i.e. command response,
        reset ack, etc.).
    `echo_events` Whether or not to print events inside payloads.
    `echo_all_packet_bytes` Whether or not to print the packet bytes when
        any packet is received.

    
`remove_ansi_escape_codes(x: str) ‑> str`
:   

    
`reset_console_command() ‑> Tuple[str, str, str]`
:   

    
`set_window_title(title) ‑> None`
:   

    
`str_command_dataframe(cdf: pd.DataFrame) ‑> str`
:   

    
`str_packet_log_dataframe(packet_log_dataframe: pd.DataFrame) ‑> str`
:   

    
`str_telemetry_payload_log_dataframe(telemetry_payload_log_dataframe: pd.DataFrame) ‑> str`
:   

    
`str_user_command(user_prompt: str, user_cmd_input_str: str, prepared_commands_dataframe: pd.DataFrame, filter_input: bool = True) ‑> str`
:   

    
`tabs2spaces(x: str) ‑> str`
:   

    
`update_all_packet_log_times(packet_log_dataframe: pd.DataFrame, now: datetime) ‑> None`
:   

    
`update_packet_log_dataframe(packet_log_dataframe: pd.DataFrame, packet: Packet) ‑> pandas.core.frame.DataFrame`
:   

    
`update_packet_log_dataframe_row(packet_log_dataframe: pd.DataFrame, row_name: str, now: datetime, num_bytes: int, is_rs422: bool, is_wifi: bool) ‑> pandas.core.frame.DataFrame`
:   

    
`update_reference_window_title() ‑> str`
:   

    
`update_telemetry_payload_log_dataframe(telemetry_payload_log_dataframe: pd.DataFrame, t: TelemetryPayload) ‑> pandas.core.frame.DataFrame`
:   

    
`update_telemetry_payload_log_from_packet(telemetry_payload_log_dataframe: pd.DataFrame, packet: Packet) ‑> pandas.core.frame.DataFrame`
:   

    
`update_telemetry_payload_log_from_payloads(telemetry_payload_log_dataframe: pd.DataFrame, payloads: EnhancedPayloadCollection) ‑> pandas.core.frame.DataFrame`
:   

    
`window_is_focused(actual_reference_window_title: str) ‑> bool`
: