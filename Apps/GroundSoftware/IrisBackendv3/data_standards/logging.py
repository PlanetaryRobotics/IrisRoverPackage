# DataStandards Package-wide logging format standards.
import logging
import verboselogs
import coloredlogs

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
coloredlogs.install(logger=logger, level='DEBUG')
