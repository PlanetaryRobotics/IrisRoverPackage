"""
General app-wide context for the GUI.

NOTE: This is **NOT** data shared in a read-write manner between Dash app
callbacks but, rather, context data used for setting up the app.

Author: Connor W. Colombo (colombo@cmu.edu)
Last Updated: 05/09/2023
"""
from typing import Any, Final

from dataclasses import dataclass, field

import IrisBackendv3 as IB3
import IrisBackendv3.ipc as ipc

from dash import Dash, CeleryManager


@dataclass
class GuiContext:
    app_name: Final[str] = "GdsGui"
    ipc_app: ipc.IpcAppHelper = field(
        default_factory=lambda app_name=app_name: (  # type: ignore
            ipc.IpcAppHelper(app_name)
        )
    )

    ipc_mgr: ipc.IpcAppManagerSync = field(default_factory=lambda: (
        ipc.IpcAppManagerSync(socket_specs={
            'sub': ipc.SocketSpec(
                sock_type=ipc.SocketType.SUBSCRIBER,
                port=ipc.Port.TRANSCEIVER_SUB,
                topics=[ipc.Topic.DL_PAYLOADS]
            )
        })
    ))

    STANDARDS: Final[IB3.data_standards.DataStandards] = field(
        default_factory=lambda: IB3.get_codec_standards()
    )

    dash_callback_mgr: CeleryManager | None = None
    dash_app: Dash | None = None
    dom: Any | None = None
