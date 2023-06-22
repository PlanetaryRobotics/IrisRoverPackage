"""
Responsible for the Dash App Layout.

Author: Connor W. Colombo (colombo@cmu.edu)
Last Updated: 06/07/2023
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
from .components.events_stream import _EventStreamAIO, make_event_stream_aio
from .components.command_line import _CommandLineAIO, make_command_line_aio
from . import style


class GuiAIO(html.Div):
    """Dash AIO Component representing the whole GUI."""
    # NOTE: All-in-One Components should be suffixed with 'AIO'
    telem_table:  _TelemTableAIO
    packet_table: _PacketTableAIO
    time_plots: List[_TimePlotAIO]
    event_stream: _EventStreamAIO
    command_line: _CommandLineAIO

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
        self.event_stream = make_event_stream_aio(context)
        self.command_line = make_command_line_aio(context)

        # Build layouts for each tab:
        table_tab = dbc.Tab(label='Tables', children=[
            # Build the actual page:
            html.Br(),
            self.packet_table,
            html.Br(),
            self.telem_table
        ])

        detailed_status_tab = dbc.Tab(label='WD Status', children=[
            html.Label('Watchdog Detailed Status')
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
                html.Div([
                    dbc.Row([
                        dbc.Col(
                            [
                                dbc.Tabs([
                                    table_tab,
                                    detailed_status_tab,
                                    plots_tab,
                                    camera_tab
                                ])
                            ],
                            width=8,
                            style={
                                'minWidth': '60vw',
                                'minHeight': '20vh',
                                'maxHeight': '86vh',
                                'overflowY': 'scroll'
                            }
                        ),
                        dbc.Col([
                            self.event_stream
                        ], width=4)  # 4/12 grid spots
                    ], style={
                        'flex-grow': '1',
                        'flex-shrink': '1',
                        'flex-basis': 'auto'
                    }),
                    dbc.Row([
                        self.command_line
                    ], style={
                        'margin-top': 'auto',  # pushes this to the end
                        'flex-grow': '0',
                        'flex-shrink': '0',
                        'flex-basis': 'auto'
                    })
                ], style={
                    'display': 'flex',
                    'flex-direction': 'column',
                    'justify-content': 'space-between',
                    'height': '100%'
                })
            ],
            style={
                'background-color': style.IrisDerivedColor.BACKGROUND.value,
                'position': 'fixed',
                'maxWidth': '100vw',
                'overflowX': 'hidden',
                'maxHeight': '100vh',
                'overflowY': 'hidden'
            }
        )


def app_custom_index_template(dash_app: Dash) -> None:
    """Applies custom modifications to the existing index.html template for
    Dash.
    (allows us to do things like patch in a custom CSS <style> section that was
    generated in the Python backend.)"""
    idx_string = dash_app.index_string
    # Add custom formatted CSS wherever the CSS template is placed:
    idx_string = idx_string.replace(
        "{%css%}",
        f"""{{%css%}}
        <style>
            {style.CUSTOM_FORMATTED_CSS}
        </style>
        """
    )
    # Apply the new template:
    dash_app.index_string = idx_string


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
            '/static/bootstrap_darkly.min.css',  # == dbc.themes.DARKLY
            '/static/ansi_html.min.css'  # from `ansi2html`
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

    app_custom_index_template(dash_app)

    gui_aio = GuiAIO(context)
    dash_app.layout = gui_aio

    return dash_app, gui_aio
