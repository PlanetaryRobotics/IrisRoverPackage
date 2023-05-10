"""
Filtering Telemetry Table Component.

Following pattern from: https://dash.plotly.com/all-in-one-components

Author: Connor W. Colombo (colombo@cmu.edu)
Last Updated: 05/10/2023
"""
from __future__ import annotations

from typing import Dict, Type, List, ClassVar
import uuid

import pandas as pd
from dash import (
    no_update,
    html, dcc, dash_table,
    Output, Input, State,
    callback, MATCH
)

from IrisBackendv3.utils.console_display import init_telemetry_payload_log_dataframe

from . import aio
from ..context import GuiContext
from ..backend import DatabaseKey, db_ready
from ..style import LABEL_STYLE_MIXIN, DROPDOWN_STYLE_MIXIN, TABLE_STYLE_MIXIN


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
        searchable=True,
        placeholder="Type Module to Show..."
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
        **DROPDOWN_STYLE_MIXIN,
        **kwargs
    )


class _TelemTableAIO(html.Div):
    # NOTE: All-in-One Components should be suffixed with 'AIO'
    title: str
    table: dash_table.DataTable
    size_slider: dcc.Slider
    modules: dcc.Dropdown
    show_all: html.Button
    telem_store: dcc.Store  # storage for state data
    mods_store: dcc.Store  # storage for names of all modules (from context)
    interval: dcc.Interval  # frequently queries the latest data from inner_db

    # This comes from the Dash AIO pattern. Allows for global lookup of
    # subcomponent ids given a parent aio_id. Seems to be necessary for
    # MATCH-based pattern matching for callbacks inside the AIO component
    # instance(s).
    class ids:
        table = aio.id_generator('_TelemTableAIO', 'table')
        size_slider = aio.id_generator('_TelemTableAIO', 'size_slider')
        modules = aio.id_generator('_TelemTableAIO', 'modules')
        show_all = aio.id_generator('_TelemTableAIO', 'show_all')
        telem_store = aio.id_generator('_TelemTableAIO', 'telem_store')
        mods_store = aio.id_generator('_TelemTableAIO', 'mods_store')
        interval = aio.id_generator('_TelemTableAIO', 'interval')

    def __init__(
        self,
        context: GuiContext,
        title='Telemetry',
        table_props: Dict | None = None,
        size_slider_props: Dict | None = None,
        modules_props: Dict | None = None,
        show_all_props: Dict | None = None,
        interval_props: Dict | None = None,
        aio_id=None
    ) -> None:
        # Make a uuid if an override is not given:
        aio_id = str(uuid.uuid4()) if aio_id is None else aio_id

        # Supply empty dicts for all prop overrides if none given:
        table_props = aio.props_guard(table_props)
        size_slider_props = aio.props_guard(size_slider_props)
        modules_props = aio.props_guard(modules_props)
        show_all_props = aio.props_guard(show_all_props)
        interval_props = aio.props_guard(interval_props)

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
                'max': 100,
                'value': 11,
                **size_slider_props
            }
        )
        self.table = dash_table.DataTable(
            id=self.ids.table(aio_id),
            **{
                'data': init_telem_df.to_dict('records'),
                'page_size': 10,
                'sort_action': 'native',
                # 'filter_action': 'native',
                # 'filter_options': {'placeholder_text': 'Filter...'},
                'style_cell_conditional': [
                    {
                        'if': {'column_id': c},
                        'textAlign': 'left'
                    } for c in ['Module', 'Channel']
                ],
                **TABLE_STYLE_MIXIN,
                # Change number of columns frozen when x-scrolling:
                'fixed_columns': {'headers': True, 'data': 3},
                **table_props
            }
        )
        self.interval = dcc.Interval(
            id=self.ids.interval(aio_id),
            **{
                'interval': 500,  # once every 0.5s,
                'n_intervals': 0,
                **interval_props
            }
        )

        # Initialize the store(s):
        self.telem_store = dcc.Store(
            id=self.ids.telem_store(aio_id),
            data=_TelemTableAIO.telem_to_json(init_telem_df)
        )
        self.mods_store = dcc.Store(
            id=self.ids.mods_store(aio_id),
            data=context.STANDARDS.modules.names
        )

        # Build layout:
        super().__init__([  # `html.Div([...])`
            html.Label(self.title, **LABEL_STYLE_MIXIN),
            html.Div([
                self.modules,
                self.show_all
            ], style={'display': 'flex', 'flex-direction': 'row'}),
            self.size_slider,
            self.table,
            self.telem_store,
            self.mods_store,
            self.interval
        ])

    @staticmethod
    def telem_to_json(df: pd.DataFrame) -> str:
        """Converts the given telem_df to json in a standardized way (being
        careful about datetime conversions, etc)."""
        # Convert name to be "datelike" (per`pd.read_json`):
        df.rename(columns={'Updated': 'Updated_time'}, inplace=True)
        return str(df.to_json(date_format='iso', date_unit='ms'))

    @staticmethod
    def telem_from_json(json_str: str) -> pd.DataFrame:
        """Converts the given json to telem_df in a standardized way (being
        careful about datetime conversions, etc)."""
        df = pd.read_json(json_str, convert_dates=True)
        # Convert from "datelike" name (per`pd.read_json`):
        df.rename(columns={'Updated_time': 'Updated'}, inplace=True)
        return df

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
        # Make the index visible (dash_table won't show index):
        df_out.sort_index(ascending=True, inplace=True)  # sort first
        df_out.reset_index(inplace=True)
        df_out.rename(columns={'index': 'Opcode'}, inplace=True)
        df_out.index = df_out.index.map(lambda x: f"0x{x:04X}")
        df_out['Updated'] = [x.strftime('%H:%M:%S') for x in df_out['Updated']]
        return df_out.fillna('')

    @staticmethod
    def df_to_display(df: pd.DataFrame, **kwargs):
        """Processes and pretty formats dataframe then returns it as a records
        dict (format required by data_table)."""
        return _TelemTableAIO.pretty_format_table(df, **kwargs).to_dict('records')

    @staticmethod
    def filter_data(
        df: pd.DataFrame,
        selected_modules: List[str]
    ) -> pd.DataFrame:
        return df[df['Module'].isin(selected_modules)]


def make_telem_table_aio(context: GuiContext, *args, **kwargs) -> _TelemTableAIO:
    """Makes a subclass of `_TelemTableAIO` with implemented context-dependent
    callbacks."""
    class _TelemTableAIO_w_Callbacks(_TelemTableAIO):
        ids = _TelemTableAIO.ids

        # Stateless pattern-matching callbacks for every component instance:

        @callback(
            Output(ids.telem_store(MATCH), 'data'),
            Input(ids.interval(MATCH), 'n_intervals')
        )
        def fetch_telem_data(n_intervals):
            # fetches updated telem data from the database
            if db_ready(context.inner_db):
                df = context.inner_db.read(DatabaseKey.TELEM_DF)
                if isinstance(df, pd.DataFrame):
                    return _TelemTableAIO.telem_to_json(df)
            return no_update

        @callback(
            Output(ids.table(MATCH), 'data'),
            Input(ids.telem_store(MATCH), 'data'),
            Input(ids.modules(MATCH), 'value')
        )
        def refresh_telem_data(telem_store, selected_modules):
            # fires when either input changes (filter or stored data)
            df = _TelemTableAIO.telem_from_json(telem_store)
            df = _TelemTableAIO.filter_data(df, selected_modules)
            return _TelemTableAIO.df_to_display(df)

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

    return _TelemTableAIO_w_Callbacks(context, *args, **kwargs)
