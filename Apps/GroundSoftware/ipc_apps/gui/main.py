# Activate postponed annotations (for using classes as return type in their own methods):
from __future__ import annotations

import asyncio
from typing import cast, Final, Union, Dict, List
from dataclasses import dataclass, field

import numpy as np
import matplotlib.pyplot as plt  # type: ignore

from termcolor import cprint

import dash
from dash import Dash, html, dcc, dash_table
import dash_bootstrap_components as dbc
import dash_bootstrap_templates as dbt

import operator as op
import numpy as np
import pandas as pd  # type: ignore
import requests
import panel as pn  # type: ignore
import hvplot.pandas  # type: ignore
import hvplot.streamz  # type: ignore
import holoviews as hv  # type: ignore
from holoviews.element.tiles import EsriImagery  # type: ignore
from holoviews.selection import link_selections  # type: ignore
from datashader.utils import lnglat_to_meters  # type: ignore
from streamz import Stream  # type: ignore
from streamz.dataframe import DataFrame as StreamingDataFrame  # type: ignore

from IrisBackendv3.utils import console_display

import IrisBackendv3 as IB3
import IrisBackendv3.ipc as ipc
from IrisBackendv3.codec.packet import Packet
from IrisBackendv3.codec.payload import TelemetryPayload, EventPayload
from IrisBackendv3.ipc.messages import (
    DownlinkedPacketsMessage, DownlinkedPayloadsMessage
)

IB3.init_from_latest()
STANDARDS: Final = IB3.get_codec_standards()

# Setup:
app = ipc.IpcAppHelper("GdsGui")
app.setLogLevel('VERBOSE')
manager = ipc.IpcAppManagerSync(socket_specs={
    'sub': ipc.SocketSpec(
        sock_type=ipc.SocketType.SUBSCRIBER,
        port=ipc.Port.TRANSCEIVER_SUB,
        topics=[ipc.Topic.DL_PAYLOADS]
    )
})


@dataclass
class GuiContext:
    telem_df: pd.DataFrame = field(default_factory=lambda: (
        console_display.init_telemetry_payload_log_dataframe(  # type: ignore
            pd.DataFrame()
        )
    ))

    @property
    def telem_df_pretty(self) -> pd.DataFrame:
        """Returns a pretty-formatted version of the `telem_df` `DataFrame`."""
        df_out = self.telem_df.copy()
        df_out.index = df_out.index.map(lambda x: f"0x{x:04X}")
        df_out['Updated'] = [x.strftime('%H:%M:%S') for x in df_out['Updated']]
        return df_out.fillna('').sort_index(ascending=True)


def ingest_data(context: GuiContext) -> None:
    payloads = IB3.codec.payload_collection.EnhancedPayloadCollection()
    while len([*payloads[TelemetryPayload]]) == 0:
        try:
            ipc_payload = manager.read('sub')
            msg = ipc.guard_msg(ipc_payload.message,
                                DownlinkedPayloadsMessage)
            payloads = msg.content.payloads
        except Exception as e:
            app.logger.error(
                f"Failed to decode IPC message `{msg}` "
                f"of `{ipc_payload=}` b/c: `{e}`."
            )
            continue

    # If this request has passed us payloads, update the payload dataframe:
    context.telem_df = console_display.update_telemetry_payload_log_from_payloads(
        context.telem_df,
        payloads
    )


def modules_dropdown(
    preselected: List[str] | None = None,
    **kwargs
) -> dcc.Dropdown:
    """Creates a dropdown of all modules with any modules with a name in
    `preselected` preselected. Each name in `preselected` must be the name of a
    module in `STANDARDS`. If `preselected` is not given, all modules will be
    preselected.

    Any extra `kwargs` given are passed straight through to `dcc.Dropdown`.
    """
    if preselected is None:
        preselected = [n for n in STANDARDS.modules.names]
    else:
        preselected = [n for n in preselected if n in STANDARDS.modules.names]
    default_kwargs = dict(
        multi=True,
        searchable=True
    )
    kwargs = {**default_kwargs, **kwargs}
    return dcc.Dropdown(
        [
            {
                "label": html.Span([m.name], style={'color': 'Purple'}),
                "value": m.name,
            }
            for m in STANDARDS.modules.vals
        ],
        value=preselected,
        **kwargs
    )


def build_dash_app(context: GuiContext) -> Dash:
    dash_app = Dash(__name__, external_stylesheets=[dbc.themes.LUX])
    dbt.load_figure_template('LUX')

    dash_app.layout = html.Div([
        html.Label('Telemetry'),
        html.Div([
            modules_dropdown(id='telem-table-modules'),
            html.Button('Show All', id='telem-table-show-all')
        ], style={'display': 'flex', 'flex-direction': 'row'}),
        dcc.Slider(
            id='telem-table-size-slider',
            min=2,
            max=min(context.telem_df.shape[0], 100),
            value=10
        ),
        dash_table.DataTable(
            id='telem-table',
            data=context.telem_df_pretty.to_dict('records'),
            page_size=10,
            sort_action="native"
        )
    ])

    @dash_app.callback(
        dash.Output('telem-table', 'data'),
        dash.Input('telem-table-modules', 'value')
    )
    def update_telem_table(selected_modules):
        df = context.telem_df_pretty
        df = df[df['Module'].isin(selected_modules)]
        return df.to_dict('records')

    @dash_app.callback(
        dash.Output('telem-table', 'page_size'),
        dash.Input('telem-table-size-slider', 'value')
    )
    def update_telem_table_size(new_size):
        return new_size

    @dash_app.callback(
        dash.Output('telem-table-modules', 'value'),
        dash.Input('telem-table-show-all', 'n_clicks')
    )
    def update_telem_show_all(*_):
        return [n for n in STANDARDS.modules.names]

    return dash_app


def main():
    context = GuiContext()
    ingest_data(context)
    dash_app = build_dash_app(context)
    dash_app.run_server(debug=True)


if __name__ == "__main__":
    main()
