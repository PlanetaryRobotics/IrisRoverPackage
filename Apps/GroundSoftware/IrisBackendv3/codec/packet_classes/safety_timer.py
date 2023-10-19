"""
Defines `WatchdogSafetyTimerPacket`, a `Packet` wrapper for a safety timer
status message string from the Watchdog. This packet type doesn't contain any
Telemetry but will be treated as an Event.

Since we couldn't get WD to cleanly export cs as an *unsigned* int, we downlink
as hex and this packet converts hex timestamps to integers for strings and
events (raw bytes are left intact for archive purposes).
Also handles converting tenth-timer times to full-timer times and converting cs
to sec or min.

@author: Connor W. Colombo (CMU)
@last-updated: 10/19/2023
"""
# Activate postponed annotations (for using classes as return type in their own methods)
from __future__ import annotations

from .gds_packet_event_mixin import GdsPacketEventPacket, GDS_EVT_PT
from IrisBackendv3.codec.packet_classes.packet import Packet, CT

from typing import Any, Optional, Final, Tuple, List

from scapy.utils import hexdump  # type: ignore

from IrisBackendv3.codec.payload_collection import EnhancedPayloadCollection

from IrisBackendv3.codec.settings import ENDIANNESS_CODE
from IrisBackendv3.codec.logs import logger

import re

from termcolor import colored

# Regex for identifying a safety timer packet:
SAFETY_TIMER_REGEX: Final = re.compile(
    b"SAFETY.?TIMER|\[ST\]", flags=re.IGNORECASE)
# Regex for identifying hex time values in centiseconds that need to be
# converted:
# Times that look like "0xHEX*X cs" are converted to decimal then multiplied by X.
# Times that just look like "0xHEX cs" will be converted to decimal.
# All time stamps will then be converted to human-readable time units.
# Hex values need to be prefixed by "0x" in order to be found.
TIME_REGEX_CS: Final = re.compile(
    b"0[xX][0-9a-fA-F]+(?:\s?\*\s?[0-9]+\s?|\s)cs")
# Regex for capturing the coefficient from a time value:
TIME_COEFF_REGEX: Final = re.compile(b"\*\s?([0-9]+)")
# Regex for just identifying a hex number and capturing the hex part:
HEX_CAPTURE_REGEX: Final = re.compile(b"(?:0[xX])?([0-9a-fA-F]+)")
# Regex for capturing all key fields from a Safety Timer Status Report:
STATUS_REPORT_CAPTURE_REGEX: Final = re.compile(
    b"\[ST\]"  # Header
    b"\s*ON:*(?:0[xX])?([0-9a-fA-F]+)"  # ON status
    b"\s*@:"  # divider before timer status
    # timer values
    b"\s*(?:0[xX])?([0-9a-fA-F]+)\s*/\s*(?:0[xX])?([0-9a-fA-F]+)"
    b"\s*([0-9]+)\s*/\s*([0-9]+)"  # expiration counter (and thresh)
    b"\s*WF:(?:0[xX])?([0-9a-fA-F]+):([0-9a-fA-F]+)"  # `*watchdogFlags`
)


def extract_first_hex(data: bytes) -> int | None:
    """Extracts & converts the first hex number from the given bytes sequence.
    """
    match = HEX_CAPTURE_REGEX.search(data)
    if match is None:
        return None
    # NOTE: 0 returns all groups, we just want the 1st capture group (1)
    return int(match.group(1), 16)


def convert_hex_times_cs(time_vals_hex: List[bytes]) -> List[int | None]:
    """Processes the given list of hex time strings in centiseconds to and
    converts them to decimal, multiplying by a coefficient if required."""
    time_vals: List[int | None] = []
    for tv in time_vals_hex:
        dec = extract_first_hex(tv)
        if dec is None:
            time_vals.append(None)
            continue
        coeff_search = TIME_COEFF_REGEX.search(tv)
        if coeff_search is not None:
            coeff = int(coeff_search.group(1), 10)
        else:
            coeff = 1
        time_vals.append(coeff*dec)
    return time_vals


def cs_time_to_human_time(time: int) -> bytes:
    """Converts a time value in centiseconds to a human readable time in
    minutes, seconds, or ms."""
    if time // 6000:
        # major unit of time is >= minutes:
        return b"%dm%ds" % (time // 6000, (time % 6000)/100)
    if time // 100:
        # major unit of time is seconds:
        return b"%.3fs" % (time / 100)
    # major unit of time is centiseconds, convert to ms:
    return b"%dms" % (time*10)


def humanize_hex_cs_match(time_val_hex_match: re.Match[bytes]) -> bytes:
    """Converts the given hex centisecond match obj. (from `TIME_REGEX_CS`)
    to centiseconds (multiplying by a coeff if necessary) and converts it to
    human-readable units.
    Helper function for `re.sub`.
    """
    time_val_hex = time_val_hex_match.group(0)
    time_val_cs = convert_hex_times_cs([time_val_hex])[0]
    if time_val_cs is None:
        # Couldn't convert, return the original unmodified:
        return time_val_hex
    return cs_time_to_human_time(time_val_cs)


def humanize_all_hex_cs_times(data: bytes) -> bytes:
    """Extracts all centisecond hex times from the given string, converts them
    to decimal, multiplying as necessary, and them converts them to a
    human-readable form (sensible units)."""
    return TIME_REGEX_CS.sub(humanize_hex_cs_match, data)


def humanize_safety_timer_status_report(data: bytes) -> bytes:
    """Reports a human-readable version of Safety Timer Status Reports
    (part of Report Status responses)."""
    if b"[ST]" not in data:
        # Not a Status Report
        return data
    # Extract Values:
    vals: List[Tuple[bytes, ...]] = STATUS_REPORT_CAPTURE_REGEX.findall(data)
    on_hex, timer_hex_cs, cutoff_hex_cs, exp_count, exp_trigger, flags_hex_hi, flags_hex_lo = vals[
        0]
    # Process data:
    on = int(on_hex, 16)
    on_str = b"ON" if on == 0xFF else (b"OFF" if on == 0x00 else b"BAD")
    timer_cs = int(timer_hex_cs, 16)
    timer = cs_time_to_human_time(timer_cs)
    cutoff_cs = int(cutoff_hex_cs, 16)
    cutoff = cs_time_to_human_time(cutoff_cs)
    exp_count_int = int(exp_count, 10)
    exp_trigger_int = int(exp_trigger, 10)

    # Compute the total timer state (and make human-readable):
    # (How many times has the timer expired * expiration time + time in this timer):
    timer_real_cs = cutoff_cs * exp_count_int + timer_cs
    timer_real = cs_time_to_human_time(timer_real_cs)
    # (How many times can the timer expire before triggering reboot * expiration time):
    cutoff_real_cs = cutoff_cs * exp_trigger_int
    cutoff_real = cs_time_to_human_time(cutoff_real_cs)
    time_left_cs = (cutoff_real_cs - timer_real_cs)
    time_left_str = cs_time_to_human_time(time_left_cs).decode('utf-8')

    # Combine two U16s of watchdogFlags:
    wf = (int(flags_hex_hi, 16) << 16) | int(flags_hex_lo, 16)
    # Build message:
    return (
        b"[ST] SAFETY TIMER: " + on_str +
        b" \t " + timer + b" / " + cutoff +
        b" \t @ " + exp_count + b"/" + exp_trigger +
        b" \t -> \t " + timer_real + b" / " + cutoff_real +
        b" \t " + colored(time_left_str.upper()+" LEFT", attrs=['bold']).encode('utf-8') +
        b" \t WF: 0x" + b':'.join(b'%02X' % x for x in wf.to_bytes(4, 'big'))
    )


class WatchdogSafetyTimerPacketInterface(GdsPacketEventPacket[GDS_EVT_PT]):
    # empty __slots__ preserves parent class __slots__
    __slots__: List[str] = []


class WatchdogSafetyTimerPacket(WatchdogSafetyTimerPacketInterface[WatchdogSafetyTimerPacketInterface]):
    """
    Creates a data-structure to allow storing and handling a safety timer
    update from the Watchdog.

    @author: Connor W. Colombo (CMU)
    @last-updated: 10/19/2023
    """
    __slots__: List[str] = []  # empty __slots__ preserves parent __slots__

    def __init__(self,
                 payloads: Optional[EnhancedPayloadCollection] = None,
                 raw: Optional[bytes] = None,
                 endianness_code: str = ENDIANNESS_CODE
                 ) -> None:

        if raw is None and (payloads is None or payloads.is_empty()):
            # If both `payloads` and `raw` are `None`, what even caused
            # this to be generated?
            logger.debug(
                "A `WatchdogSafetyTimerP` was constructed with no `payloads` and "
                "no `raw` data. This suggests it's being created from "
                "nothing as a completely empty packet. Was this "
                "intentional or is this a bug?"
            )

        if payloads is None:
            # Except possibly in future subclasses, this should normally be
            # empty for an `WatchdogDebugPacket`. `payloads` needs to stay as
            # an `__init__` arg to avoid violating the Liskov substitution
            # principle.
            payloads = EnhancedPayloadCollection()

        super().__init__(
            payloads=payloads,
            raw=raw,
            endianness_code=endianness_code
        )  # passthru

    def encode(self, **kwargs: Any) -> bytes:
        # There's no encoding to do. It's just raw data.
        if self._raw is None:
            return b''
        else:
            return self._raw

    @classmethod
    def build_minimum_packet(
        cls,
        payloads: EnhancedPayloadCollection,
        raw: Optional[bytes],
        endianness_code: str
    ) -> WatchdogSafetyTimerPacket:
        """ Minimum packet is just the packet. """
        return cls(
            payloads=payloads,
            raw=raw,
            endianness_code=endianness_code
        )

    @classmethod
    def is_valid(cls, data: bytes, endianness_code: str = ENDIANNESS_CODE) -> bool:
        """Valid if the packet starts with `b'DEBUG'` and contains the SAFETY
        TIMER message identifier."""
        return (
            data[:5].upper() == b'DEBUG'
            and SAFETY_TIMER_REGEX.search(data) is not None
        )

    def __repr__(self) -> str:
        if self._raw is None:
            l = 0
            data = b''
        else:
            l = len(self._raw)
            data = self._raw
        return f"WatchdogSafetyTimerPacket[{l}B]: {data!r} "

    def __str__(self) -> str:
        if self._raw is None:
            data = b''
        else:
            data = self._raw

        # Strip off the debug:
        data = data[5:]

        # Make human-readable:
        if is_status_report := (b"[ST]" in data):
            # Apply special formatting to "[ST]" summary packets
            # (part of Report Status response):
            data = humanize_safety_timer_status_report(data)
        else:
            # Just a message.
            # Humanize any hex centisecond times in here:
            data = humanize_all_hex_cs_times(data[5:])

        # Format the result:
        try:
            formatted_data = data.decode('utf-8').rstrip('\x00').rstrip()
        except UnicodeDecodeError:
            # Contains non-unicode characters
            formatted_data = str(data)

        # Add special color if this is warning:
        data_upper = data.upper()
        if (
            b'PERFORMING REBOOT' in data_upper or
            b'REBOOTING' in data_upper or
            b'FINAL' in data_upper or
            b'WARNING' in data_upper or
            b'BITFLIP' in data_upper
        ):
            on_color = 'on_red'
        elif is_status_report:
            on_color = 'on_black'
        elif b'ACK KICK PROCESSED' in data_upper:
            on_color = 'on_green'
        else:
            on_color = 'on_orange'
        return colored(formatted_data, 'white', on_color)
