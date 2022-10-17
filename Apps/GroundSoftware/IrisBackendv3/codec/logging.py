# Codec Package-wide logging format standards.
import logging
import verboselogs  # type: ignore # mypy doesn't see type hints
import coloredlogs  # type: ignore # mypy doesn't see type hints

verboselogs.install()
logger = logging.getLogger(__name__)
"""
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
"""
VALID_LOG_LEVELS = ('SPAM', 'DEBUG', 'VERBOSE', 'INFO', 'NOTICE',
                    'WARNING', 'SUCCESS', 'ERROR', 'CRITICAL')
coloredlogs.install(logger=logger, level='DEBUG')
