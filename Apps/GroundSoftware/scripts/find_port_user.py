"""
Finds who's using each GDS IPC port.

@author: Connor W. Colombo (CMU)
@last-updated: 04/30/2023
"""
import argparse
import psutil
from typing import Any, Dict, Tuple
from signal import SIGTERM  # use SIGTERM so it's graceful

from IrisBackendv3.ipc import Port
from IrisBackendv3.ipc import IpcAppHelper
from IrisBackendv3.logs import VALID_LOG_LEVELS

app = IpcAppHelper("PortInvestigator")


def get_cli_opts():
    parser = argparse.ArgumentParser(description=(
        'IRIS Lunar Rover — Port Investigator — CLI'
    ))
    port_names = [p.name for p in Port]
    parser.add_argument('-p', '--port',
                        type=str, required=False,
                        nargs='+', default=[],
                        choices=port_names,
                        help=(
                            "Which port(s) to investigate. "
                            "If none given, all will be investigated"
                        ))
    parser.add_argument('-k', '--kill-users', default=False,
                        action=argparse.BooleanOptionalAction,
                        help=(
                            "Kill any processes which are currently using "
                            "GDS ports."
                        ))
    parser.add_argument('-l', '--log-level',
                        type=str, required=False, default='NOTICE',
                        choices=VALID_LOG_LEVELS,
                        help="Logging level.")
    return parser.parse_args()


# Proxy as a Process:
if __name__ == "__main__":
    # Process CLI Args:
    opts = get_cli_opts()
    app.setLogLevel(opts.log_level)
    if len(opts.port) == 0:
        # Use all ports:
        ports = [p for p in Port]
    else:
        # Convert names to ports:
        ports = [Port[pn] for pn in opts.port]

    # Grab all ports used:
    port_users: Dict[Port, Tuple[psutil.Process, Any] | None] = {
        p: None for p in ports
    }
    port_vals = [p.value for p in ports]
    for proc in psutil.process_iter():
        try:
            for conns in proc.connections(kind='inet'):
                if conns.laddr.port in port_vals:
                    port = Port(conns.laddr.port)
                    port_users[port] = (proc, conns)
        except (PermissionError, psutil.AccessDenied):
            app.logger.info(f"Couldn't investigate {proc}. Permission denied.")

    # Print status report:
    user_strs = []
    for port, user in port_users.items():
        if user is None:
            user_strs.append(f"{port}:\t {user}")
        else:
            proc, conn = user
            user_strs.append(f"{port}:\n\tPROC: {proc}\n\tCONN: {conn}")
    app.logger.notice('\n'+'\n'.join(user_strs))

    # Optionally, kill any users:
    if opts.kill_users:
        for port, (proc, _) in [(p, u) for p, u in port_users.items() if u is not None]:
            app.logger.notice(f"Killing user of {port}: {proc} . . .")
            proc.send_signal(SIGTERM)
