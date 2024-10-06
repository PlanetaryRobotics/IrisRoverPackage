"""
Functions that define the formatting (+ encoding and decoding) of data in the
archived format.

@author: Colombo, CMU
@last-updated: 09/29/2024
"""
from typing import Any, Final, Tuple, List, Dict, cast, Iterable

from IrisBackendv3.codec.packet import Packet
from IrisBackendv3.codec.payload import Payload, TelemetryPayload, EventPayload

from IrisBackendv3.data_standards.fsw_data_type import (
    FswDataType,
    Category as FswDataCategory
)
from ipc_apps.dl_processor import process_dl_payloads

from datetime import datetime, timedelta, timezone

import re
import lxml.html  # type: ignore
from html import unescape
import pandas as pd  # type: ignore

_ANSI_ESCAPE_PATTERN: Final = re.compile(r'(\x9B|\x1B\[)[0-?]*[ -\/]*[@-~]')


def remove_ansi_escape_codes(msg: str) -> str:
    """Helper function to remove ANSI escape strings from text that was
    designed for the command line:"""
    return _ANSI_ESCAPE_PATTERN.sub('', msg)


def bytes_to_str(x: bytes | None) -> str:
    """Encodes bytes to a string for better storage in HDFStore (pandas compat.)"""
    if x is None or isinstance(x, bytes) and len(x) == 0:
        return ''
    else:
        return '0x' + x.hex()


def str_to_bytes_if_bytes(x: Any | str) -> Any | bytes:
    """Converts string to bytes if given value is a string and encodes bytes.
    Otherwise, leaves the value as is."""
    if isinstance(x, str):
        # For HDFStore, we've encoded bytes as hex string starting with 0x.
        if len(x) > 2 and x[:2] == '0x':
            try:
                return bytes.fromhex(x[2:])
            except ValueError:
                # Couldn't convert from hex. Likely not actually hex string.
                # Leave as string.
                return x

    return x


def reformat_event_str(val: str) -> str:
    """Reformats an event string to make it readable in a table."""
    # Remove any ANSI escape codes:
    val = remove_ansi_escape_codes(val)
    # Some packets, specifically `GdsPacketEvents`, ship their event
    # strings as HTML. To make this viewable in a table, strip off all the
    # HTML formatting and
    if '</' in val:
        # This part can be expensive, so we can perform this quick check
        # and skip if there aren't even any HTML tag primitives
        # (typ. most event strings)
        val = ''.join(lxml.html.fromstring(val).xpath('//text()'))
    # Undo any lingering HTML escaping (e.g. '>' becomes '&gt;'):
    val = unescape(val)
    return val


def format_time(
    t: datetime | str | Iterable[datetime | str] | None
) -> pd.DatetimeIndex:
    if t is None:
        return None
    return pd.to_datetime(t, utc=True)


def payload_to_storage_format(p: Payload) -> Tuple[type, Any]:
    """Converts a payload to its proper storage format for maximum
    compatibility and efficiency with the Pandas HDFStore for DataFrames.

    Returns a tuple of the target dtype and the payload value, transformed if
    necessary.
    """
    val: Any
    dtype: type
    if isinstance(p, TelemetryPayload):
        p = cast(TelemetryPayload, p)

        if p.channel.is_enum:
            # If it's an enum, store as string:
            # (not as space efficient but the target is human legibility when
            # unpacked)
            dtype = str
            val = p.channel.get_enum_name(p.data)
        elif p.channel.datatype.category == FswDataCategory.IRISBYTESTRING:
            # If it's bytes, store it as a hex string:
            dtype = str
            val = bytes_to_str(p.data)
        else:
            # Default python type is otherwise sufficient:
            if not isinstance(p.channel.datatype.python_type, type):
                # Not a type, likely a tuple of types we haven't accounted for.
                # Handle as object in special cases like this.
                dtype = object
            else:
                dtype = p.channel.datatype.python_type
            val = p.data

    elif isinstance(p, EventPayload):
        p = cast(EventPayload, p)
        # We represent Events as strings:
        dtype = str
        val = reformat_event_str(p.formatted_string)

    return dtype, val
