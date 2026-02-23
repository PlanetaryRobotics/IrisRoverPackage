Module IrisBackendv3.logging.logger_template
============================================
Standard template for all package loggers.

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
@last-updated: 03/07/2023

Functions
---------

    
`create_logger_from_template(logger_name: str, save_to_file: bool = True, log_file_name_suffix: str = '', sub_folder: str = './', default_console_log_level: str = 'DEBUG', default_file_log_level: str = 'DEBUG', console_format_string: str = '%(asctime)-15s: %(levelname)-8s %(message)s', file_format_string: str = '%(asctime)-15s: %(levelname)-8s %(message)s') ‑> Tuple[verboselogs.VerboseLogger, Callable[[str], None], Callable[[str], None]]`
:   Creates a logger from the standard template with the given options.
    
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