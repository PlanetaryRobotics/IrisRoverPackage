"""
CLI Arg Processor for the Iris Frontend's Backend Interface Layer (BIL).

@author: Connor W. Colombo (CMU)
@last-updated: 01/07/2023
"""
import argparse

parser = argparse.ArgumentParser(
    description='IRIS Lunar Rover — Frontend-Backend Interface Layer (BIL) — CLI')


def get_opts():
    """
    Return settings wrapped in argparse.
    """
    parser.add_argument('-p', '--port', type=int, nargs='?', default=54041,
                        help='Port to open the server on.')

    parser.add_argument('-s', '--server', type=str,
                        help='Address of the server the user is connecting to.')
    parser.add_argument('-m', '--mission_name', type=str,
                        help='Name of the mission (DB partition) to connect to.')
    parser.add_argument('-u', '--user_name', type=str,
                        help='Name of the user connecting.')
    parser.add_argument('-c', '--code', type=str,
                        help='Code for the connecting user.')
    parser.add_argument('-x', '--protocol', type=str, nargs='?', default='mongodb+srv',
                        help='Protocol to use for server connection.')

    opts = parser.parse_args()
    return opts
