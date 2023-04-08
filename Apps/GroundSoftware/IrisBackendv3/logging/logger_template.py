"""Standard template for all package loggers.

Any logs at a level >= the level selected below will be displayed.
Levels in ascending order are:
    - SPAM (5)
    - DEBUG (10)
    - VERBOSE (15)
    - INFO (20)
    - NOTICE (25)
    - WARNING (30)
    - SUCCESS (35)
    - ERROR (40)
    - CRITICAL (50)

@author: Connor W. Colombo (CMU)
@last-updated: 03/08/2023
"""

from typing import Any, Final, Callable, Tuple
import os.path
import logging
import logging.handlers
import verboselogs  # type: ignore # mypy doesn't see type hints
import coloredlogs  # type: ignore # mypy doesn't see type hints

import ulid

# Root directory where all log files are stored:
_LOG_FILE_ROOT_DIR: Final[str] = f"./out/package-logs/"
# Extension applied to all log files:
_LOG_FILE_EXT: Final[str] = f"ib3.log"

verboselogs.install()


def create_logger_from_template(
    logger_name: str,
    save_to_file: bool = True,
    log_file_name_suffix: str = "",
    sub_folder: str = "./",
    default_console_log_level: str = "DEBUG",
    default_file_log_level: str = "DEBUG",
    console_format_string: str = "%(asctime)-15s: %(levelname)-8s %(message)s",
    file_format_string: str = "%(asctime)-15s: %(levelname)-8s %(message)s"
) -> Tuple[verboselogs.VerboseLogger, Callable[[str], None], Callable[[str], None]]:
    """Creates a logger from the standard template with the given options.

    Returns a tuple containing the logger, along with a function handle to
        change just the console log level and a function handle to change just
        the file log level.

    Args:
        logger_name (str): Name of this logger (typ. `__name__` of the package
            this is for).
        save_to_file (bool, optional): Whether or not all logs should be saved
            to a file. Defaults to True.
        sub_folder (str, optional): What subfolder in `_LOG_FILE_ROOT_DIR` to
            save to (if any). Defaults to "./".
        log_file_name_suffix (str, optional): Additional suffix to apply to
            `logger_name` when creating the log file name. Defaults to "".
        default_console_log_level (str, optional): Default logging level for
            logs that go to the console.
        default_file_log_level (str, optional): Default logging level for logs
            that go to the log file.
        console_format_string (str, optional): Format string to apply to logs
            that go to the console.
        file_format_string (str, optional): Format string to apply to logs
            that go to the log file.
    """
    logger = verboselogs.VerboseLogger(logger_name)

    # Set up console logging:
    console_handler = logging.StreamHandler()
    # Pull out the function necessary to set the console log level:
    console_handler.setLevel(default_console_log_level)
    console_handler.setFormatter(logging.Formatter(console_format_string))
    logger.addHandler(console_handler)

    # Set up file logging:
    if save_to_file:
        # Build the file name & path:
        file_name = (
            f"{logger_name}{log_file_name_suffix}Logger"
            f"_{ulid.new()}"
            f".{_LOG_FILE_EXT}"
        )
        file_path = os.path.join(_LOG_FILE_ROOT_DIR, sub_folder, file_name)
        file_path = os.path.abspath(file_path)
        # Make sure the directory exists:
        file_dir = os.path.dirname(file_path)
        if not os.path.exists(file_dir):
            os.makedirs(file_dir)
        # Build the handler:
        file_handler = logging.handlers.RotatingFileHandler(
            filename=file_path,
            mode='a',
            delay=True,  # don't make file until we write something to it
            maxBytes=10e6,  # rollover log files after about 1MB
            # create up to 5000 rollovers of the file (50GB max) (before
            # trashing the oldest data - at this point, we likely have a
            # runaway but we still want this threshold to be high in the case
            # of log missions):
            backupCount=5000
        )
        # Pull out the function necessary to set the file log level:
        file_level_changer = file_handler.setLevel
        file_level_changer(default_file_log_level)
        file_handler.setFormatter(logging.Formatter(file_format_string))
        logger.addHandler(file_handler)
    else:
        # Create a no-op lambda:
        def file_level_changer(_: str): pass

    # Add colored logs:
    # NOTE: This will replace (build upon) the `console_handler` made above,
    # so, `console_level_changer` will need to be grabbed after this.
    coloredlogs.install(
        level=default_console_log_level,
        logger=logger,
        fmt=console_format_string
    )

    # Grab the new stream handler:
    for handler in logger.handlers:
        if isinstance(handler, coloredlogs.StandardErrorHandler):
            console_handler = handler
            break
    # Grab the handle to the level setter of that handler:
    console_level_changer = console_handler.setLevel

    return logger, console_level_changer, file_level_changer
