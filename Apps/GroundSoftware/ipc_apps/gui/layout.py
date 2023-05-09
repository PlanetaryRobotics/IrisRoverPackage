"""
Responsible for the Dash App Layout.

Author: Connor W. Colombo (colombo@cmu.edu)
Last Updated: 05/08/2023
"""
from typing import Tuple

import uuid

from dash import Dash, html, dcc, dash_table
import dash_bootstrap_components as dbc
import dash_bootstrap_templates as dbt

from gui.context import GuiContext
from gui.components.telem_table import TelemTableAIO
from gui.components import aio


class GuiAIO(html.Div):
    """Dash AIO Component representing the whole GUI."""
    # NOTE: All-in-One Components should be suffixed with 'AIO'
    telem_table:  TelemTableAIO
    # store used as an inter-callback signal to tell the background to fetch
    # more data from IPC (closes the loop):
    ipc_signal: dcc.Store

    def __init__(
        self,
        context: GuiContext,
        aio_id=None
    ) -> None:
        # Make a uuid if an override is not given:
        aio_id = str(uuid.uuid4()) if aio_id is None else aio_id

        # Build Components:
        self.telem_table = TelemTableAIO(context)

        # Initialize the store:
        self.ipc_signal = dcc.Store(
            id='ipc-signal',
            data=1  # start the count
        )

        # Build layout:
        super().__init__([  # `html.Div([...])`
            self.telem_table,
            self.ipc_signal
        ])


def build_dash_app(context: GuiContext) -> Tuple[Dash, GuiAIO]:
    """Creates a Dash app and adds the layout for the GDS GUI to it.
    Returns the Dash App and a handle to the Gui AIO component.
    """
    dash_app = Dash(context.app_name, external_stylesheets=[dbc.themes.LUX])
    dbt.load_figure_template('LUX')

    gui_aio = GuiAIO(context)
    dash_app.layout = gui_aio

    return dash_app, gui_aio
