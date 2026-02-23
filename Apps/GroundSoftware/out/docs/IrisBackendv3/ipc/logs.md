Module IrisBackendv3.ipc.logs
=============================

Functions
---------

    
`create_app_logger() ‑> Tuple[verboselogs.VerboseLogger, Callable[[str], None], Callable[[str], None]]`
:   Creates a special logger for this IPC App.
    NOTE: You should set `ipc.settings.settings['app_name']` first.
    
    Returns a tuple containing the logger, along with a function handle to
    change just the console log level and a function handle to change just
    the file log level.