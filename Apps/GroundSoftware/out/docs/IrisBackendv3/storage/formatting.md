Module IrisBackendv3.storage.formatting
=======================================
Functions that define the formatting (+ encoding and decoding) of data in the
archived format.

@author: Colombo, CMU
@last-updated: 09/29/2024

Functions
---------

    
`bytes_to_str(x: bytes | None) ‑> str`
:   Encodes bytes to a string for better storage in HDFStore (pandas compat.)

    
`format_time(t: Union[datetime.datetime, str, Iterable[datetime.datetime | str], ForwardRef(None)]) ‑> pandas.core.indexes.datetimes.DatetimeIndex`
:   

    
`payload_to_storage_format(p: IrisBackendv3.codec.payload.Payload) ‑> Tuple[type, Any]`
:   Converts a payload to its proper storage format for maximum
    compatibility and efficiency with the Pandas HDFStore for DataFrames.
    
    Returns a tuple of the target dtype and the payload value, transformed if
    necessary.

    
`reformat_event_str(val: str) ‑> str`
:   Reformats an event string to make it readable in a table.

    
`remove_ansi_escape_codes(msg: str) ‑> str`
:   Helper function to remove ANSI escape strings from text that was
    designed for the command line:

    
`str_to_bytes_if_bytes(x: Union[Any, str]) ‑> Union[Any, bytes]`
:   Converts string to bytes if given value is a string and encodes bytes.
    Otherwise, leaves the value as is.