"""
Responsible for the Dash App Layout.

Author: Connor W. Colombo (colombo@cmu.edu)
Last Updated: 05/13/2023
"""
from typing import Tuple, List

import os
import uuid

from dash import Dash, html, dcc
import dash_bootstrap_components as dbc
import dash_bootstrap_templates as dbt
from flask import send_from_directory

from .context import GuiContext
from .components.telem_table import _TelemTableAIO, make_telem_table_aio
from .components.packet_table import _PacketTableAIO, make_packet_table_aio
from .components.time_plot import _TimePlotAIO, make_time_plot_aio
from . import style


class GuiAIO(html.Div):
    """Dash AIO Component representing the whole GUI."""
    # NOTE: All-in-One Components should be suffixed with 'AIO'
    telem_table:  _TelemTableAIO
    packet_table: _PacketTableAIO
    time_plots: List[_TimePlotAIO]

    def __init__(
        self,
        context: GuiContext,
        aio_id=None
    ) -> None:
        # Make a uuid if an override is not given:
        aio_id = str(uuid.uuid4()) if aio_id is None else aio_id

        # Build Components:
        self.telem_table = make_telem_table_aio(context)
        self.packet_table = make_packet_table_aio(context)
        self.time_plots = [
            make_time_plot_aio(context),
            # make_time_plot_aio(
            #     context,
            #     default_channels=['Imu_XAcc', 'Imu_YAcc', 'Imu_ZAcc']
            # )
        ]

        # Build layouts for each tab:
        table_tab = dbc.Tab(label='Tables', children=[
            # Build the actual page:
            html.Br(),
            self.packet_table,
            html.Br(),
            self.telem_table
        ])

        plots_tab = dbc.Tab(label='Plots', children=[
            *self.time_plots
        ])

        camera_tab = dbc.Tab(label='Camera', children=[
            html.Label('Camera')
        ])

        # Build layout:
        super().__init__(
            [  # `html.Div([...])`
                # Locally load Iris CSS stylesheet
                # (see `static_file` below for routing):
                html.Link(
                    rel='stylesheet',
                    href='/static/custom.css'
                ),
                dbc.Row([
                    dbc.Col([
                        dbc.Tabs([table_tab, plots_tab, camera_tab])
                    ], width=9),
                    dbc.Col([
                        html.Label("Event Stream.")
                    ], width=3)  # 3/12 grid spots
                ])
            ],
            style={
                'background-color': style.IrisDerivedColor.BACKGROUND.value
            }
        )


def build_dash_app(context: GuiContext) -> Tuple[Dash, GuiAIO]:
    """Creates a Dash app and adds the layout for the GDS GUI to it.
    Returns the Dash App and a handle to the Gui AIO component.
    """
    dash_app = Dash(
        __name__,
        title='Iris GDS GUI',
        update_title=False,  # Keep it from blinking 'Updating' when DOM updates
        external_stylesheets=[
            # Apply an Iris-Cosmos-like dark theme to cover our bases in
            # case we miss something in style.py:
            dbc.themes.DARKLY
        ]
    )
    # Dark theme plots & figures:
    dbt.load_figure_template('DARKLY')

    # Keep external resources local:
    dash_app.css.config.serve_locally = True
    dash_app.scripts.config.serve_locally = True

    # Recipe for linking external resources from the assets directory:
    @dash_app.server.route('/static/<path:path>')
    def static_file(path):
        # getcwd is `GroundSoftware` if `./run-script.sh` is called from there.
        static_folder = os.path.join(os.getcwd(), './ipc_apps/gui_lib/assets')
        return send_from_directory(static_folder, path)

    gui_aio = GuiAIO(context)
    dash_app.layout = gui_aio

    return dash_app, gui_aio
