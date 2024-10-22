# Transceiver Package-wide logging format standards.
from typing import Final
from IrisBackendv3.logs.logger_template import create_logger_from_template

from IrisBackendv3.transceiver.settings import settings

logger_build: Final = create_logger_from_template(
    logger_name='IrisBackendv3.Transceiver',
    sub_folder='Transceiver',
    save_to_file=True,
    default_console_log_level="INFO"
)
logger: Final = logger_build[0]
logger_setConsoleLevel: Final = logger_build[1]
logger_setFileLevel: Final = logger_build[2]
