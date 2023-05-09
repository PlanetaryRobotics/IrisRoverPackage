"""
Filtering Telemetry Table Component.

Following pattern from: https://dash.plotly.com/all-in-one-components

Author: Connor W. Colombo (colombo@cmu.edu)
Last Updated: 05/09/2023
"""
from __future__ import annotations

from typing import Dict, Type, List, ClassVar
import uuid

import pandas as pd
from dash import (
    html, dcc, dash_table,
    Output, Input, State,
    callback, MATCH
)

from IrisBackendv3.utils.console_display import init_telemetry_payload_log_dataframe

from . import aio
from ..context import GuiContext


def modules_dropdown(
    context: GuiContext,
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
        preselected = [n for n in context.STANDARDS.modules.names]
    else:
        preselected = [
            n for n in preselected if n in context.STANDARDS.modules.names
        ]
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
            for m in context.STANDARDS.modules.vals
        ],
        value=preselected,
        **kwargs
    )


class TelemTableAIO(html.Div):
    # NOTE: All-in-One Components should be suffixed with 'AIO'
    title: str
    table: dash_table.DataTable
    size_slider: dcc.Slider
    modules: dcc.Dropdown
    show_all: html.Button
    telem_store: dcc.Store  # storage for state data
    mods_store: dcc.Store  # storage for names of all modules (from context)

    # This comes from the Dash AIO pattern. Allows for global lookup of
    # subcomponent ids given a parent aio_id:
    class ids:
        table = aio.id_generator('TelemTableAIO', 'table')
        size_slider = aio.id_generator('TelemTableAIO', 'size_slider')
        modules = aio.id_generator('TelemTableAIO', 'modules')
        show_all = aio.id_generator('TelemTableAIO', 'show_all')
        telem_store = aio.id_generator('TelemTableAIO', 'telem_store')
        mods_store = aio.id_generator('TelemTableAIO', 'mods_store')

    def __init__(
        self,
        context: GuiContext,
        title='Telemetry',
        table_props: Dict | None = None,
        size_slider_props: Dict | None = None,
        modules_props: Dict | None = None,
        show_all_props: Dict | None = None,
        aio_id=None
    ) -> None:
        # Make a uuid if an override is not given:
        aio_id = str(uuid.uuid4()) if aio_id is None else aio_id

        # Supply empty dicts for all prop overrides if none given:
        table_props = aio.props_guard(table_props)
        size_slider_props = aio.props_guard(size_slider_props)
        modules_props = aio.props_guard(modules_props)
        show_all_props = aio.props_guard(show_all_props)

        # Create an initial empty dataframe:
        init_telem_df = init_telemetry_payload_log_dataframe(pd.DataFrame())

        # Build Components:
        self.title = title
        self.modules = modules_dropdown(
            context,
            id=self.ids.modules(aio_id),
            **modules_props
        )
        self.show_all = html.Button(
            'Show All',
            id=self.ids.show_all(aio_id),
            **show_all_props
        )
        self.size_slider = dcc.Slider(
            id=self.ids.size_slider(aio_id),
            **{
                'min': 2,
                'max': min(init_telem_df.shape[0], 100),
                'value': 10,
                **show_all_props
            }
        )
        self.table = dash_table.DataTable(
            id=self.ids.table(aio_id),
            **{
                'data': init_telem_df.to_dict('records'),
                'page_size': 10,
                'sort_action': "native",
                **table_props
            }
        )

        # Initialize the store(s):
        self.telem_store = dcc.Store(
            id=self.ids.telem_store(aio_id),
            data=init_telem_df.to_json()
        )
        self.mods_store = dcc.Store(
            id=self.ids.mods_store(aio_id),
            data=[n for n in context.STANDARDS.modules.names]
        )

        # Build layout:
        super().__init__([  # `html.Div([...])`
            html.Label(self.title),
            html.Div([
                self.modules,
                self.show_all
            ], style={'display': 'flex', 'flex-direction': 'row'}),
            self.size_slider,
            self.table,
            self.telem_store,
            self.mods_store
        ])

    @staticmethod
    def pretty_format_table(
        df: pd.DataFrame,
        inplace: bool = True
    ) -> pd.DataFrame:
        """Returns a pretty-formatted version of the `telem_df` `DataFrame`."""
        if inplace:
            df_out = df
        else:
            df_out = df.copy()
        df_out.index = df_out.index.map(lambda x: f"0x{x:04X}")
        df_out['Updated'] = [x.strftime('%H:%M:%S') for x in df_out['Updated']]
        return df_out.fillna('').sort_index(ascending=True)

    @staticmethod
    def df_to_pretty_records(df: pd.DataFrame, **kwargs):
        """Pretty formats dataframe then returns it as records JSON (format 
        required by data_table)."""
        return TelemTableAIO.pretty_format_table(df, **kwargs).to_json('records')

    @staticmethod
    def filter_data(
        df: pd.DataFrame,
        selected_modules: List[str]
    ) -> pd.DataFrame:
        return df[df['Module'].isin(selected_modules)]

    # Stateless pattern-matching callbacks for every component instance:
    @callback(
        Output(ids.table(MATCH), 'data'),
        Input(ids.telem_store(MATCH), 'data'),
        Input(ids.modules(MATCH), 'value')
    )
    def refresh_telem_data(telem_store, selected_modules):
        # fires when either input changes (filter or stored data)
        df = pd.read_json(telem_store)
        df = TelemTableAIO.filter_data(df, selected_modules)
        return TelemTableAIO.df_to_pretty_records(df)

    @callback(
        Output(ids.table(MATCH), 'page_size'),
        Input(ids.size_slider(MATCH), 'value')
    )
    def update_telem_table_size(new_size):
        return new_size

    @callback(
        Output(ids.modules(MATCH), 'value'),
        Input(ids.show_all(MATCH), 'n_clicks'),
        State(ids.mods_store(MATCH), 'data')
    )
    def update_telem_show_all(n_clicks, mods_store):
        return mods_store
