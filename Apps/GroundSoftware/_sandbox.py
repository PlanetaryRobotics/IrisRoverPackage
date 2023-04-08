import sys
import pkgutil
from typing import cast, Final, List

import IrisBackendv3 as IB3
IB3.init_from_latest()

ca_package_path: Final = './config/command_aliases'


def print_modules():
    print('\n'.join([
        f"{m}:\t\t\t\t {k}"
        for m, k in sys.modules.items()
        if 'IrisBackend' in m or 'alias' in m.lower()
    ]))


if __name__ == "__main__":
    aliases_table = IB3.config.command_aliases.CommandAliasesTable.load_from_file(
        'standard_command_aliases'
    )
    print_modules()
