"""Quick and dirty script to indiscriminantly kill every process it can using
the given port address that it can.

@author: Connor W. Colombo (CMU)
@last-updated: 05/30/2023"""
import argparse
from psutil import process_iter
from signal import SIGTERM  # or SIGKILL


def get_cli_opts():
    parser = argparse.ArgumentParser(description=(
        'IRIS Lunar Rover — Simple Port Killer — CLI'
    ))
    parser.add_argument('-p', '--port',
                        type=int, required=False,
                        nargs='+', default=[],
                        help=(
                            "Which port(s) to kill. "
                            "If none given, none will be investigated"
                        ))
    return parser.parse_args()


if __name__ == "__main__":
    # Process CLI Args:
    opts = get_cli_opts()

    if len(opts.port) > 0:
        for proc in process_iter():
            try:
                for conns in proc.connections(kind='inet'):
                    try:
                        if conns.laddr.port in opts.port:
                            proc.send_signal(SIGTERM)  # or SIGKILL
                    except Exception as e:
                        pass
            except Exception as e:
                pass
