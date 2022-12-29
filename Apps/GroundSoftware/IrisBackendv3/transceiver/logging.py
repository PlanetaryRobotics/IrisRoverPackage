# IPC Package-wide logging format standards.
from typing import Any
import logging
import verboselogs  # type: ignore # mypy doesn't see type hints
import coloredlogs  # type: ignore # mypy doesn't see type hints

import ulid

verboselogs.install()
# set up logging to file
logging.basicConfig(level=logging.DEBUG,
                    format='%(asctime)s %(name)-12s %(levelname)-8s %(message)s',
                    datefmt='%m-%d %H:%M',
                    filename=f'./out/raw-console-logs/TransceiverLogger_{ulid.new()}.clog',
                    filemode='a'  # keep adding data to log if it already exists, don't overwrite
                    )
# define a Handler which writes INFO messages or higher to the sys.stderr
console = logging.StreamHandler()
console.setLevel(logging.INFO)
# set a format which is simpler for console use
FORMAT = '%(asctime)-15s: %(levelname)-8s %(message)s'
formatter = logging.Formatter(FORMAT)
# tell the handler to use this format
console.setFormatter(formatter)
# add the handler to the root logger
logging.getLogger().addHandler(console)

# Tagging this as `Any` prevents mypy from complaining about methods like
# `verbose` and `notice` that it thinks don't exist.
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
coloredlogs.install(logger=logger, level='DEBUG', fmt=FORMAT)
