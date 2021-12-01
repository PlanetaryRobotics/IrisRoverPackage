"""
Module for looking up definitions from inside the latest Data Standards 
(compiled from latest accessible FPrime).

Created: 10/29/2021
Last Update: 10/29/2021
"""
from typing import Optional
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

    parser.add_argument('-m', '--module-name', type=str,
                        help='Print standards just for the module with the given name.')

    opts = parser.parse_args()
    return opts


def print_lookup(module_to_lookup: Optional[str] = None):
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
            for e in t.enum:
                p_enum(e)
        header('Events:')
        for i, ev in enumerate(m.events.vals):
            event(i, ev)
        print('\n]')


if __name__ == '__main__':
    opts = get_opts()

    parser.print_help()

    if opts.all:
        print_lookup()

    if opts.module_name:
        print_lookup(opts.module_name)
