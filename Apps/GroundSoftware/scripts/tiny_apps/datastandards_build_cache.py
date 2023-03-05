"""
Builds the DataStandards used by the GSW GDS based on the given CLI options
and caches them to a `dsc` file with the given name in the
`out/DataStandardsCache/` directory.

Useful as a first step before queuing up a sequence of IPC apps, all of which
will operate off these same datastandards (so each process doesn't have to
rebuild them itself).

Created: 03/01/2023
Last Update: 03/01/2023
"""
import os.path
import argparse

from IrisBackendv3 import data_standards
from IrisBackendv3.data_standards.logging import logger as DsLogger

parser = argparse.ArgumentParser(
    description='IRIS Lunar Rover — DataStandards Cache Builder — CLI')


def get_opts():
    """
    Return settings wrapped in argparse.

    Returns
    -------
    opts : settings wrapped in argparse.

    """
    parser.add_argument('--prebuilts', default=True,
                        action=argparse.BooleanOptionalAction,
                        help=(
                            'Whether or not to include prebuilt modules. '
                            'True = includes prebuilts, '
                            'False = only vanilla modules directly from '
                            'FPrime are included (no GSW-only modules).'
                        ))

    parser.add_argument('--suffix', default=True,
                        action=argparse.BooleanOptionalAction,
                        help=(
                            'Whether or not to add a ULID suffix to name in '
                            'order to make the datastandards cache file name '
                            'unique.'
                        ))

    parser.add_argument('-n', '--name', type=str,
                        default=data_standards.data_standards._FILENAME_BASE_DEFAULT,
                        help='Name of these DataStandards Cache.')

    parser.add_argument('-d', '--dir', type=str,
                        default=data_standards.data_standards._CACHE_DIR,
                        help='Directory to save the datastandards cache to.')

    parser.add_argument('-l', '--log-level', type=str, default="INFO",
                        help='Name of these data standards.')

    opts = parser.parse_args()
    return opts


def build_datastandards_cache(opts) -> str:
    """Builds the datastandards, caches them.
    Returns absolute path to cache."""
    DsLogger.setLevel(opts.log_level.upper())

    filepath = data_standards.build_and_cache(
        cache_dir=opts.dir,
        cache_name_base=opts.name,
        include_ulid_suffix=opts.suffix,
        include_all_prebuilts=opts.prebuilts
    )

    return filepath


if __name__ == "__main__":
    opts = get_opts()
    build_datastandards_cache(opts)
