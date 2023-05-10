""" GUI for low-level access to the Iris Ground Data System via IPC.
Essentially a graphical IPC-based successor to the CLI-based Iris Console
(which still exists).

Run this from the `GroundSoftware` directory as:
`./run-script.sh ipc_apps/gui.py`
"""

import os
import multiprocessing
import asyncio

from ipc_apps.gui_lib.context import GuiContext
from ipc_apps.gui_lib import backend, layout, callbacks, data

import IrisBackendv3 as IB3


def init_process(log_level: str) -> GuiContext:
    """Init procedure common to all Processes to be called from inside the
    Process."""
    # Load latest DataStandards (**from inside the Process**):
    IB3.init_from_latest()
    # Create independent App Context for this process:
    context = GuiContext()
    context.ipc_app.setLogLevel(log_level)
    # Connect to DB:
    context.inner_db = backend.InternalDatabase(context)
    return context


def run_ipc():
    """Runner process for IPC."""
    # Set up the context for this process:
    context = init_process(log_level='VERBOSE')
    # Bring up IPC Manager and run:
    context.ipc_mgr = data.make_ipc_manager(context)
    asyncio.run(context.ipc_mgr.run())


def run_dash():
    """Runner process for Dash."""
    # Set up the context for this process:
    context = init_process(log_level='VERBOSE')
    # Build Dash App and Manager:
    context.dash_callback_mgr = backend.make_callback_manager(context)
    context.dash_app, context.dom = layout.build_dash_app(context)
    context.dash_app = callbacks.add_app_callbacks(context)
    # Run:
    context.dash_app.run_server(
        debug=False,  # sometimes `True` causes a "no module named gui" issue
        processes=min(os.cpu_count() // 2 + 1, os.cpu_count()),
        threaded=False  # use multiprocessing not threads
    )


def main():
    # Launch independent processes:
    processes = [
        multiprocessing.Process(target=run_ipc),
        multiprocessing.Process(target=run_dash)
    ]
    for process in processes:
        process.start()
    for process in processes:
        process.join()


if __name__ == "__main__":
    main()
