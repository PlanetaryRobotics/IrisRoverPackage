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
    print_modules()

    mod = IB3.config.command_aliases.import_command_aliases_file(
        'standard_command_aliases'
    )

    mod.ALIASES(IB3.get_codec_standards())

    mods = {
        m: k
        for m, k in sys.modules.items()
        if 'IrisBackend' in m or 'alias' in m.lower()
    }

    print_modules()
