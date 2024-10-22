# Module containing a template for creating `logger`s for each package.
# Likely this will be more complicated as IPC matures and asynchronous logging
# services get added.

from typing import Final, Tuple

# List of all valid log levels:
VALID_LOG_LEVELS: Final[Tuple[str, ...]] = (
    'SPAM',
    'DEBUG',
    'VERBOSE',
    'INFO',
    'NOTICE',
    'WARNING',
    'SUCCESS',
    'ERROR',
    'CRITICAL'
)


def str_to_log_level(s) -> str | None:
    """Used to validate a log level. Returns the string as an appropriately
    formatted log level (or `None` if it's not valid)."""
    if isinstance(s, str) and s.upper() in VALID_LOG_LEVELS:
        return s.upper()
    return None
