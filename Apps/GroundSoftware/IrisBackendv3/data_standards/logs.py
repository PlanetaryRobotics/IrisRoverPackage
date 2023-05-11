# Codec Package-wide logging format standards.
from typing import Final
from IrisBackendv3.logs.logger_template import create_logger_from_template

from IrisBackendv3.ipc.settings import settings

logger_build: Final = create_logger_from_template(
    logger_name='IrisBackendv3.DataStandards',
    sub_folder='DataStandards',
    save_to_file=True,
    default_console_log_level="INFO",
    default_file_log_level="NOTICE"
)
logger: Final = logger_build[0]
logger_setConsoleLevel: Final = logger_build[1]
logger_setFileLevel: Final = logger_build[2]