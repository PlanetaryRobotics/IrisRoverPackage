# Storage Package-wide logging format standards.
from typing import Final
from IrisBackendv3.logs.logger_template import create_logger_from_template

from IrisBackendv3.storage.settings import settings

logger_build = create_logger_from_template(
    logger_name='IrisBackendv3.Storage',
    sub_folder='Storage',
    save_to_file=True,
    default_console_log_level="DEBUG"
)
logger: Final = logger_build[0]
logger_setConsoleLevel: Final = logger_build[1]
logger_setFileLevel: Final = logger_build[2]
