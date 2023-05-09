import os
import IrisBackendv3 as IB3

from gui.context import GuiContext
from gui import backend, layout, callbacks

IB3.init_from_latest()


def main():
    context = GuiContext()
    context.ipc_app.setLogLevel('VERBOSE')

    # Create Celery Manager with Redis:
    context.dash_callback_mgr = backend.make_callback_manager(context)

    # Build Dash App:
    context.dash_app, context.dom = layout.build_dash_app(context)
    context.dash_app = callbacks.add_app_callbacks(context)

    # Run Dash App:
    context.dash_app.run_server(
        debug=True,
        processes=min(os.cpu_count()//2+1, os.cpu_count()),
        threaded=False  # use multiprocessing not threads
    )


if __name__ == "__main__":
    main()
