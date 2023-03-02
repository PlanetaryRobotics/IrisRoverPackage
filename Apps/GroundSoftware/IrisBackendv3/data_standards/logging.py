# DataStandards Package-wide logging format standards.
import logging
import verboselogs  # type: ignore # mypy doesn't see type hints
import coloredlogs  # type: ignore # mypy doesn't see type hints
from typing import Any

verboselogs.install()
# type = Any so errors aren't thrown on verboselogs levels
logger: Any = logging.getLogger(__name__)
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
coloredlogs.install(logger=logger, level='DEBUG')
