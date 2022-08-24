"""
Module for looking up definitions from inside the latest Data Standards 
(compiled from latest accessible FPrime).

Created: 10/29/2021
Last Update: 02/05/2022
"""
from typing import Optional
from termcolor import cprint, colored
from trans_tools import *
import argparse

parser = argparse.ArgumentParser(
    description='IRIS Lunar Rover — Data Standards Lookup — CLI')


def get_opts():
    """
    Return hyperparameters wrapped in argparse.

    Returns
    -------
    opts : Hyperparameters wrapped in argparse.

    """
    parser.add_argument('-a', '--all', action='store_true',
                        help='Prints all the Data Standards')

    parser.add_argument('-l', '--list-modules', action='store_true',
                        help='Lists the names of modules registered in the Data Standards.')

    parser.add_argument('-t', '--telem-count', action='store_true',
                        help='Lists (and counts) all telemetry channels (useful for sizing hash buckets in FSW).')

    parser.add_argument('-m', '--module-name', type=str,
                        help='Print standards just for the module with the given name.')

    opts = parser.parse_args()
    return opts


def print_modules_list() -> None:
    cprint("All Registered Modules:", 'magenta', 'on_grey', attrs=['bold'])
    for m in standards.modules.values():
        id_text = colored(f"0x{m.ID:04X}: ", 'blue')
        module_text = colored(f"{m}", 'green')
        print(f"\t {id_text} {module_text}")


def print_lookup(module_to_lookup: Optional[str] = None) -> None:
    if module_to_lookup is None:
        standards.print_overview()
    else:
        def module(x): return cprint(
            f"\n\t{x}", 'magenta', 'on_grey', attrs=['bold'])

        def header(x): return cprint(f"\n\t\t{x}", 'grey', 'on_white')
        def command(i, x): return cprint(f"\n\t\t\t{i}.\t{x}", 'green')
        def telemetry(i, x): return cprint(f"\n\t\t\t{i}.\t{x}", 'red')
        def event(i, x): return cprint(f"\n\t\t\t{i}.\t{x}", 'blue')

        def p_arg(a): return cprint(
            f"\n\t\t\t\t\tAvailable values for `{a.name}`:", 'cyan')

        def p_enum(x): return cprint(
            f"\n\t\t\t\t\t\t'{x.name}' or {x.value} or {hex(x.value)}", 'magenta')

        print("Data Standards Overview: [")
        m = standards.modules[module_to_lookup]
        module(m)
        header('Commands:')
        for i, c in enumerate(m.commands.vals):
            command(i, c)
            for arg in c.args:
                if len(arg.enum) > 0:
                    p_arg(arg)
                    for e in arg.enum:
                        p_enum(e)
        header('Telemetry:')
        for i, t in enumerate(m.telemetry.vals):
            telemetry(i, t)
        header('Events:')
        for i, ev in enumerate(m.events.vals):
            event(i, ev)
        print('\n]')

def all_telem_channels() -> None:
    # Useful for determining number of hash buckets needed in TLMCHAN_HASH_BUCKETS in Svc/TlmChan/TlmChanImplCfg.hpp in the FPrime FSW.
    def module(x): return cprint(
        f"\n\t{x}", 'magenta', 'on_grey', attrs=['bold'])

    def header(x): return cprint(f"\n\t{x}", 'grey', 'on_white')
    
    header('All Telemetry Channels:')
    tlm_count = 0
    for ((m_id, m_name), m) in standards.modules.fast_items():
        module_tlm_count = 0
        for idx, t in enumerate(m.telemetry.vals):
            cprint(f"\n\t\t{m_name}: \t{idx}.\t{t}", 'red', 'on_white')
            module_tlm_count += 1
            tlm_count += 1
        print(f"Telemetry Channels in {m_name}: {module_tlm_count}")

    header(f"There are {tlm_count} Total Telemetry Channels in the System.")


if __name__ == '__main__':
    opts = get_opts()

    parser.print_help()

    if opts.all:
        print_lookup()

    if opts.module_name:
        print_lookup(opts.module_name)

    if opts.list_modules:
        print_modules_list()

    if opts.telem_count:
        all_telem_channels()
