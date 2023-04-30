# IPC Package-wide logging format standards for the IPC Layer.
import os.path
from typing import Final, Callable, Tuple
import verboselogs  # type: ignore # mypy doesn't see type hints
from IrisBackendv3.logs.logger_template import create_logger_from_template

from IrisBackendv3.ipc.settings import settings

logger_build: Final = create_logger_from_template(
    logger_name='IrisBackendv3.Ipc',
    sub_folder='Ipc',
    save_to_file=True,
    default_console_log_level="DEBUG"
)
logger: Final = logger_build[0]
logger_setConsoleLevel: Final = logger_build[1]
logger_setFileLevel: Final = logger_build[2]


def create_app_logger() -> Tuple[verboselogs.VerboseLogger, Callable[[str], None], Callable[[str], None]]:
    """Creates a special logger for this IPC App.
    NOTE: You should set `ipc.settings.settings['app_name']` first.

    Returns a tuple containing the logger, along with a function handle to
    change just the console log level and a function handle to change just
    the file log level.
    """
    app_name = settings['app_name']
    return create_logger_from_template(
        logger_name=f'IrisBackendv3.IpcApp.{app_name}',
        sub_folder=os.path.join("IpcApps", app_name),
        log_file_name_suffix="",
        default_console_log_level="DEBUG"
    )
