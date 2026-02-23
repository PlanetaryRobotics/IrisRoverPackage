"""
Packet Display Table Component.

Following pattern from: https://dash.plotly.com/all-in-one-components

Author: Connor W. Colombo (colombo@cmu.edu)
Last Updated: 05/10/2023
"""
from __future__ import annotations

import os

from typing import Dict, Type, List, ClassVar
from datetime import datetime
import uuid

import pandas as pd  # type: ignore
from dash import (  # type: ignore
    no_update,
    html, dcc, dash_table,
    Output, Input, State,
    callback, MATCH
)

from IrisBackendv3.utils.console_display import init_packet_log_dataframe

from . import aio
from ..context import GuiContext
from ..backend import DatabaseKey, db_ready
from ..style import LABEL_STYLE_MIXIN, TABLE_STYLE_MIXIN


class _PacketTableAIO(html.Div):
    # NOTE: All-in-One Components should be suffixed with 'AIO'
    title: str
    table: dash_table.DataTable
    telem_store: dcc.Store  # storage for state data
    interval: dcc.Interval  # frequently queries the latest data from inner_db

    # This comes from the Dash AIO pattern. Allows for global lookup of
    # subcomponent ids given a parent aio_id. Seems to be necessary for
    # MATCH-based pattern matching for callbacks inside the AIO component
    # instance(s).
    class ids:
        table = aio.id_generator('_PacketTableAIO', 'table')
        telem_store = aio.id_generator('_PacketTableAIO', 'telem_store')
        interval = aio.id_generator('_PacketTableAIO', 'interval')

    def __init__(
        self,
        context: GuiContext,
        title='Packets',
        table_props: Dict | None = None,
        interval_props: Dict | None = None,
        aio_id=None
    ) -> None:
        # Make a uuid if an override is not given:
        aio_id = str(uuid.uuid4()) if aio_id is None else aio_id

        # Supply empty dicts for all prop overrides if none given:
        table_props = aio.props_guard(table_props)
        interval_props = aio.props_guard(interval_props)

        # Create an initial empty dataframe:
        init_telem_df = init_packet_log_dataframe(pd.DataFrame())

        # Build Components:
        self.title = title
        self.table = dash_table.DataTable(
            id=self.ids.table(aio_id),
            **{
                'data': init_telem_df.to_dict('records'),
                'sort_action': "native",
                'style_cell_conditional': [
                    {
                        'if': {'column_id': c},
                        'textAlign': 'left'
                    } for c in ['Packet Type']
                ],
                **TABLE_STYLE_MIXIN,
                **table_props
            }
        )
        self.interval = dcc.Interval(
            id=self.ids.interval(aio_id),
            **{
                'interval': float(os.environ.get('IBv3_GDS_PACKET_TABLE_UPDATE_INTERVAL_MS', 500)),
                'n_intervals': 0,
                **interval_props
            }
        )

        # Initialize the store(s):
        self.telem_store = dcc.Store(
            id=self.ids.telem_store(aio_id),
            data=_PacketTableAIO.packets_to_json(init_telem_df)
        )

        # Build layout:
        super().__init__([  # `html.Div([...])`
            html.Label(self.title, **LABEL_STYLE_MIXIN),
            self.table,
            self.telem_store,
            self.interval
        ])

    @staticmethod
    def packets_to_json(df: pd.DataFrame) -> str:
        """Converts the given packet_df to json in a standardized way (being
        careful about datetime conversions, etc)."""
        # Convert name to be "datelike" (per`pd.read_json`):
        df.rename(columns={'Updated': 'Updated_time'}, inplace=True)
        return str(df.to_json(date_format='iso', date_unit='ms'))

    @staticmethod
    def packets_from_json(json_str: str) -> pd.DataFrame:
        """Converts the given json to packet_df in a standardized way (being
        careful about datetime conversions, etc)."""
        df = pd.read_json(json_str, convert_dates=True)
        # Convert from "datelike" name (per`pd.read_json`):
        df.rename(columns={'Updated_time': 'Updated'}, inplace=True)
        return df

    @staticmethod
    def pre_process_table(
        df: pd.DataFrame,
        inplace: bool = True,
        **_
    ) -> pd.DataFrame:
        """Pre-processes dataframe data before display.
        (changes to the data itself, not just formatting to make it look nice).
        """
        if inplace:
            df_out = df
        else:
            df_out = df.copy()
        now = datetime.now()
        current_interval = [x.total_seconds()
                            for x in (now - df_out['Updated'])]
        # Make 'Current Dt' update live:
        df_out['Current Dt [s]'] = current_interval
        return df_out

    @staticmethod
    def pretty_format_table(
        df: pd.DataFrame,
        inplace: bool = True,
        **_
    ) -> pd.DataFrame:
        """Returns a pretty-formatted version of the `packet_df` `DataFrame`."""
        if inplace:
            df_out = df
        else:
            df_out = df.copy()
        # Make the index visible (dash_table won't show index):
        df_out.sort_index(ascending=True, inplace=True)  # sort first
        df_out.reset_index(inplace=True)
        # Format Columns:
        df_out['Current Dt [s]'] = [
            f"{x:.3f}" for x in df_out['Current Dt [s]']]
        df_out['Avg. Dt [s]'] = [
            f"{x:.3f}" for x in df_out['Avg. Dt [s]']]
        df_out['Avg. bits/sec'] = [
            f"{x:.1f}" for x in df_out['Avg. bits/sec']]
        df_out['Avg. bits/sec w/CCSDS'] = [
            f"{x:.1f}" for x in df_out['Avg. bits/sec w/CCSDS']]
        df_out['Updated'] = [
            x.isoformat(sep=' ', timespec='milliseconds')[5:]  # trim year
            for x in df_out['Updated']
        ]
        # Rename columns for display:
        df_out.rename(columns={
            'index': 'Packet Type',
            'Avg. bits/sec': 'Avg. bits/s',
            'Avg. bits/sec w/CCSDS': 'Avg. bits/s w/CCSDS',
        }, inplace=True)
        return df_out.fillna('')

    @staticmethod
    def df_to_display(df: pd.DataFrame, **kwargs):
        """Processes and pretty formats dataframe then returns it as a records
        dict (format required by data_table)."""
        # Pre-process dataframe:
        df = _PacketTableAIO.pre_process_table(df, **kwargs)
        # Format, Convert, and Return:
        return _PacketTableAIO.pretty_format_table(df, **kwargs).to_dict('records')


def make_packet_table_aio(context: GuiContext, *args, **kwargs) -> _PacketTableAIO:
    """Makes a subclass of `_PacketTableAIO` with implemented context-dependent
    callbacks."""
    class _PacketTableAIO_w_Callbacks(_PacketTableAIO):
        ids = _PacketTableAIO.ids

        # Stateless pattern-matching callbacks for every component instance:

        @callback(
            Output(ids.telem_store(MATCH), 'data'),
            Input(ids.interval(MATCH), 'n_intervals')
        )
        def fetch_telem_data(n_intervals):
            # fetches updated telem data from the database
            if db_ready(context.inner_db):
                df = context.inner_db.read(DatabaseKey.PACKET_DF)
                if isinstance(df, pd.DataFrame):
                    return _PacketTableAIO.packets_to_json(df)
            return no_update

        @callback(
            Output(ids.table(MATCH), 'data'),
            Input(ids.telem_store(MATCH), 'data')
        )
        def refresh_telem_data(telem_store):
            # fires when either input changes (filter or stored data)
            df = _PacketTableAIO.packets_from_json(telem_store)
            return _PacketTableAIO.df_to_display(df)

    return _PacketTableAIO_w_Callbacks(context, *args, **kwargs)
