"""
Time Series Plotting Component with active lookup from a Redis TimeSeries 
database.

Following pattern from: https://dash.plotly.com/all-in-one-components

Author: Connor W. Colombo (colombo@cmu.edu)
Last Updated: 05/13/2023
"""
from __future__ import annotations

from typing import Any, Dict, Tuple, List, cast, TypedDict, TypeAlias
from datetime import datetime, timedelta
import dateutil.parser
from dataclasses import dataclass
import uuid

import redis
import pandas as pd
import plotly.graph_objs as go
import dash
from dash import (
    html, dcc,
    Output, Input, State,
    callback, MATCH,
    ctx, no_update
)
import dash_bootstrap_components as dbc
import dash_daq as daq

from IrisBackendv3.utils.console_display import init_packet_log_dataframe

from . import aio
from ..context import GuiContext
from .. import backend
from .. import data
from .. import style


@dataclass
class PlotConfig:
    """Simple struct for data about the last reload."""
    channels_shown: List[str]  # fully qualified names of channels shown rn
    disp_timespan: Tuple[int, int]  # visible timespan (x-axis range)
    loaded_timespan: Tuple[int, int]  # timespan of x-axis data loaded


class _TimePlotAIO(html.Div):
    # NOTE: All-in-One Component Classes should be suffixed with 'AIO'
    channel_selector: dcc.Dropdown
    plot: dcc.Graph
    interval: dcc.Interval  # frequently queries the latest data from inner_db

    # STORES (note: each output can only be associated with at most 1 callback,
    # so some datastructures may need to be split among stores)
    # Which channels are being shown:
    channels_store: dcc.Store[List[str]]
    # Tuple of timestamps of the span of time displayed on the x-axis:
    disp_timespan_store: dcc.Store[Tuple[int, int]]
    # Tuple of timestamps of the span of time loaded into the graph (even if
    # not visible):
    loaded_timespan_store: dcc.Store[Tuple[int, int]]
    # Button to toggle live updating:
    keep_live_toggle: daq.BooleanSwitch
    # Button to purge all data outside the display window (for performance):
    purge_outside_button: dbc.Button
    # Refreshes all data in the loaded time span (in case progressive updates
    # seem to screw up the data somehow - e.g. un-sorting it):
    refresh_button: dbc.Button

    # This comes from the Dash AIO pattern. Allows for global lookup of
    # subcomponent ids given a parent aio_id. Seems to be necessary for
    # MATCH-based pattern matching for callbacks inside the AIO component
    # instance(s).
    class ids:
        channel_selector = aio.id_generator('_TimePlotAIO', 'channel_selector')
        plot = aio.id_generator('_TimePlotAIO', 'plot')
        interval = aio.id_generator('_TimePlotAIO', 'interval')
        channels_store = aio.id_generator('_TimePlotAIO', 'channels_store')
        disp_timespan_store = aio.id_generator(
            '_TimePlotAIO', 'disp_timespan_store')
        loaded_timespan_store = aio.id_generator(
            '_TimePlotAIO', 'loaded_timespan_store')
        keep_live_toggle = aio.id_generator('_TimePlotAIO', 'keep_live_toggle')
        purge_outside_button = aio.id_generator(
            '_TimePlotAIO', 'purge_outside_button')
        refresh_button = aio.id_generator('_TimePlotAIO', 'refresh_button')

    def __init__(
        self,
        context: GuiContext,
        default_channels: List[str] | None = None,
        channel_selector_props: Dict | None = None,
        plot_props: Dict | None = None,
        interval_props: Dict | None = None,
        aio_id=None
    ) -> None:
        # Make a uuid if an override is not given:
        aio_id = str(uuid.uuid4()) if aio_id is None else aio_id

        # Supply empty dicts for all prop overrides if none given:
        channel_selector_props = aio.props_guard(channel_selector_props)
        plot_props = aio.props_guard(plot_props)
        interval_props = aio.props_guard(interval_props)

        # Setup data structures:
        all_channel_names = sorted(
            f"{m.name}_{ch.name}"
            for m in context.STANDARDS.modules.vals
            for ch in m.telemetry.vals
        )

        # Create initial PlotConfig struct:
        now = datetime.now()
        if default_channels is None:
            default_channels = [
                'NetworkManager_Rssi',
                'RadioGround_Rssi'
            ]
        default_channels = [
            n for n in default_channels
            if n in all_channel_names
        ]
        plot_cfg = PlotConfig(
            # Start with arbitrary channels visible to demonstrate multiple
            # plotting:
            channels_shown=default_channels,
            # Start with an arbitrary timespan of the last 5 minutes:
            disp_timespan=(
                data.datetime_to_timestamp(now - timedelta(minutes=5)),
                data.datetime_to_timestamp(now)
            ),
            loaded_timespan=(
                data.datetime_to_timestamp(now - timedelta(minutes=5)),
                data.datetime_to_timestamp(now)
            )
        )

        # Build Components:
        self.channel_selector = dcc.Dropdown(
            [
                {
                    "label": html.Span(
                        [name],
                        **style.DROPDOWN_LABEL_STYLE_MIXIN
                    ),
                    "value": name,
                }
                for name in sorted(all_channel_names)
            ],
            id=self.ids.channel_selector(aio_id),
            style={
                **style.DROPDOWN_STYLE_MIXIN['style'],
                'min-width': '100ch'
            },
            **{
                'value': plot_cfg.channels_shown,
                'multi': True,
                'searchable': True,
                'placeholder': "Pick a Telemetry Channel to Plot...",
                **channel_selector_props
            }
        )

        # Create a base figure:
        # Timespan is chosen by the x-axis of this plot, so we need to use this
        # to initialize our x-axis range:
        now = datetime.now()
        self.plot = dcc.Graph(
            id=self.ids.plot(aio_id),
            figure=go.Figure(
                go.Scatter(x=[*plot_cfg.disp_timespan], y=[0, 0])
            ),
            **{
                'responsive': True,
                'animate': True,
                'hoverData': True,
                **plot_props
            }
        )

        self.keep_live_toggle = daq.BooleanSwitch(
            id=self.ids.keep_live_toggle(aio_id),
            label='Live',
            on=True
        )

        self.purge_outside_button = dbc.Button(
            "Purge",
            id=self.ids.purge_outside_button(aio_id),
            outline=True,
            color="secondary",
            class_name="me-1",
            n_clicks=0
        )
        self.purge_outside_button_tooltip = dbc.Tooltip(
            "Purge all data out of view (frees memory).",
            id=aio.id_generator('_TimePlotAIO', 'pob_tooltip')(aio_id),
            target=self.ids.purge_outside_button(aio_id),
            placement='bottom'
        )
        self.refresh_button = dbc.Button(
            "Refresh",
            id=self.ids.refresh_button(aio_id),
            outline=True,
            color="secondary",
            class_name="me-1",
            n_clicks=0
        )
        self.refresh_button_tooltip = dbc.Tooltip(
            "Reloads all data, in and out of view (panacea for weird dataviz).",
            id=aio.id_generator('_TimePlotAIO', 'rb_tooltip')(aio_id),
            target=self.ids.refresh_button(aio_id),
            placement='bottom'
        )

        self.interval = dcc.Interval(
            id=self.ids.interval(aio_id),
            **{
                'interval': 0.2*1000,  # ms
                'n_intervals': 0,
                **interval_props
            }
        )

        # Build store(s):
        # Split keys across stores (needs to be fragmented b/c of the 1
        # callback per output rule):
        self.channels_store = dcc.Store(
            id=self.ids.channels_store(aio_id),
            data=[]  # start empty so we know when the figure has actually been updated
        )
        self.disp_timespan_store = dcc.Store(
            id=self.ids.disp_timespan_store(aio_id),
            data=plot_cfg.disp_timespan
        )
        self.loaded_timespan_store = dcc.Store(
            id=self.ids.loaded_timespan_store(aio_id),
            data=plot_cfg.loaded_timespan
        )

        # Build layout:
        super().__init__([  # `html.Div([...])`
            dbc.Row([
                dbc.Col([
                    html.Label(
                        'Telemetry Channels:', **style.LABEL_STYLE_MIXIN),
                    self.channel_selector
                ]),
                dbc.Col([
                    self.purge_outside_button,
                    self.purge_outside_button_tooltip
                ], width=1, align='end'),
                dbc.Col([
                    self.refresh_button,
                    self.refresh_button_tooltip
                ], width=1, align='end'),
                dbc.Col([
                    self.keep_live_toggle
                ], width=1)
            ], justify='center', align='center'),
            self.plot,
            self.interval,
            self.channels_store,
            self.disp_timespan_store,
            self.loaded_timespan_store
        ])


def make_time_plot_aio(context: GuiContext, *args, **kwargs) -> _TimePlotAIO:
    """Makes a subclass of `_PacketTableAIO` with implemented context-dependent
    callbacks."""
    class _TimePlotAIO_w_Callbacks(_TimePlotAIO):
        ids = _TimePlotAIO.ids

        @staticmethod
        def plot_config_from_stores(
            channels_shown_store: List[str],
            disp_timespan_store: Tuple[int, int],
            loaded_timespan_store: Tuple[int, int]
        ) -> PlotConfig:
            """Creates a `PlotConfig` object from the values stored across
            various data stores.
            Needs to be done this way because each output (store) can be
            associated with at most 1 callback, so if this data is to be
            mutated by multiple callbacks, it needs to be fragmented across
            multiple stores.
            """
            # Check types:
            # (if there's a type failure, log it and just don't update but
            # continue app):
            if not (
                isinstance(channels_shown_store, list)
                and all(isinstance(x, str) for x in channels_shown_store)
            ):
                if channels_shown_store is not None:
                    # This isn't just a pre-initialization issue:
                    context.ipc_app.logger.warning(
                        "Can't build `PlotConfig`. `channels_shown_store` "
                        "should be `List[str]` but instead is: "
                        f"`{channels_shown_store}`."
                    )
                raise dash.exceptions.PreventUpdate
            if not (
                isinstance(disp_timespan_store, (tuple, list))
                and len(disp_timespan_store) == 2
                and all(isinstance(x, int) for x in disp_timespan_store)
            ):
                if disp_timespan_store is not None:
                    # This isn't just a pre-initialization issue:
                    context.ipc_app.logger.warning(
                        "Can't build `PlotConfig`. `disp_timespan_store` "
                        "should be `Tuple[int, int]` but instead is: "
                        f"`{disp_timespan_store}`."
                    )
                raise dash.exceptions.PreventUpdate
            if not (
                isinstance(loaded_timespan_store, (tuple, list))
                and len(loaded_timespan_store) == 2
                and all(isinstance(x, int) for x in loaded_timespan_store)
            ):
                if loaded_timespan_store is not None:
                    # This isn't just a pre-initialization issue:
                    context.ipc_app.logger.warning(
                        "Can't build `PlotConfig`. `loaded_timespan_store` "
                        "should be `Tuple[int, int]` but instead is: "
                        f"`{loaded_timespan_store}`."
                    )
                raise dash.exceptions.PreventUpdate
            # Everything checks out. Build & return:
            return PlotConfig(
                channels_shown=channels_shown_store,
                disp_timespan=(disp_timespan_store[0], disp_timespan_store[1]),
                loaded_timespan=(
                    loaded_timespan_store[0], loaded_timespan_store[1])
            )

        @staticmethod
        def get_channel_data(
            full_channel_names: List[str],
            from_time: datetime | int,
            to_time: datetime | int,
            span_if_blank: bool = False
        ) -> Dict[str, Tuple[List[datetime], List[Any]]]:
            """Extracts corresponding time and value vectors for channels with
            the given fully qualified names over the given timespan from the 
            Time Series Database.

            If there's no data to return, a tuple of empty lists will be
            returned **UNLESS** `span_if_blank`, then a 2-tuple spanning the
            start to end window will be returned.
            """
            # Make sure time endpoints are timestamps:
            if not isinstance(from_time, int):
                from_time = data.datetime_to_timestamp(from_time)
            if not isinstance(to_time, int):
                to_time = data.datetime_to_timestamp(to_time)

            # Craft a blank series that spans the requested time (in case we
            # can't plot, we don't want to change the view frame):
            blank_series: Tuple[List[datetime], List[Any]]
            if span_if_blank:
                blank_series = (
                    [data.timestamp_to_datetime(from_time),
                     data.timestamp_to_datetime(to_time)],
                    [0, 0]
                )
            else:
                blank_series = ([], [])

            if not backend.db_ready(context.inner_db):
                # Can't get data. Return empty data:
                return {n: blank_series for n in full_channel_names}

            # Grab all data in the time range for each channel:
            results: Dict[str, Tuple[List[datetime], List[Any]]] = dict()
            for full_ch_name in full_channel_names:
                full_ch_name = cast(str, full_ch_name)
                module_name, channel_name = full_ch_name.split('_', 1)
                key = data.channel_to_ts_key(module_name, channel_name)
                try:
                    ts = context.inner_db._redis.ts()
                    val_tuples: List[Tuple[int, Any]] = ts.range(
                        key, from_time=from_time, to_time=to_time
                    )
                except redis.exceptions.ResponseError as e:
                    context.ipc_app.logger.error(
                        f"Failed to load data for channel {full_ch_name} -> "
                        f"{key=} b/c: `{e}`."
                    )
                    val_tuples = []
                # Unzip into a time series and a value series:
                if len(val_tuples) > 0:
                    times, vs = [*zip(*val_tuples)]
                    ds = [data.timestamp_to_datetime(t) for t in times]
                    results[full_ch_name] = (ds, vs)
                else:
                    results[full_ch_name] = blank_series

            return results

        # Alias for the `[updateData, traceIndices, [maxPoints]]` form required
        # by `prependData` and `extendData` in `dcc.Graph`:
        GraphUpdate_T: TypeAlias = List[Dict[str, List] | List[int]]

        @staticmethod
        def channel_data_to_graph_update(
            ch_data: Dict[str, Tuple]
        ) -> GraphUpdate_T:
            """Converts fetched channel data into the form required by
            `dcc.Graph` to update via `prependData` and `extendData`."""
            return [
                {
                    'x': [ds for ds, _ in ch_data.values()],
                    'y': [vs for _, vs in ch_data.values()]
                },
                [*range(len(ch_data))]
            ]

        # Stateless pattern-matching callbacks for every component instance:

        @callback(
            Output(ids.disp_timespan_store(MATCH), 'data'),
            Input(ids.plot(MATCH), 'relayoutData')
        )
        def relayout_to_timespan(
            relayoutData: Dict[str, Any]
        ) -> Tuple[int, int]:
            """Extracts the timespan of the x-axis from a `relayoutData`
            update and updates the timespan store if it contains an x-axis
            change event."""
            if (
                relayoutData
                and 'xaxis.range[0]' in relayoutData
                and 'xaxis.range[1]' in relayoutData
            ):
                x0 = dateutil.parser.parse(relayoutData['xaxis.range[0]'])
                x1 = dateutil.parser.parse(relayoutData['xaxis.range[1]'])
                return (
                    data.datetime_to_timestamp(x0),
                    data.datetime_to_timestamp(x1)
                )

            # Data isn't available. Whatever was depending on this probably
            # shouldn't update:
            raise dash.exceptions.PreventUpdate

        @callback(
            output=dict(
                figure=Output(ids.plot(MATCH), 'figure'),
                channels=Output(ids.channels_store(MATCH), 'data')
            ),
            inputs=dict(
                selected_channels=Input(ids.channel_selector(MATCH), 'value'),
                # Clicking refresh will just trigger this to grab all new data:
                n_refresh=Input(ids.refresh_button(MATCH), 'n_clicks')
            ),
            state=dict(
                disp_timespan=State(ids.disp_timespan_store(MATCH), 'data'),
                loaded_timespan=State(ids.loaded_timespan_store(MATCH), 'data')
            )
        )
        def fetch_new_plot_data(
            selected_channels: List[str],
            disp_timespan: Tuple[int, int],
            loaded_timespan: Tuple[int, int],
            **_
        ) -> Dict:
            """Changes what channels are displayed in the plot and make a new
            figure, reloading data for the entire loaded_timespan in the
            process."""
            if not backend.db_ready(context.inner_db):
                raise dash.exceptions.PreventUpdate

            # Create a basic plot config object:
            # (this will implicitly validate the inputs too.)
            plot_cfg = _TimePlotAIO_w_Callbacks.plot_config_from_stores(
                channels_shown_store=selected_channels,
                disp_timespan_store=disp_timespan,
                loaded_timespan_store=loaded_timespan
            )

            # Grab data for each selected channel in the time range:
            ch_data = _TimePlotAIO_w_Callbacks.get_channel_data(
                plot_cfg.channels_shown,
                from_time=plot_cfg.loaded_timespan[0],
                to_time=plot_cfg.loaded_timespan[1],
                # make sure our output data isn't an empty series:
                span_if_blank=True
            )
            # Make the data series for all channels:
            data_series: List[go.Scatter] = [
                go.Scatter(
                    name=full_ch_name,
                    x=ds, y=vs,
                    mode='lines+markers'
                )
                for full_ch_name, (ds, vs) in ch_data.items()
            ]

            # Return figure and data struct:
            return dict(
                figure=go.Figure(
                    data=data_series,
                    layout={
                        'plot_bgcolor': style.IrisDerivedColor.BACKGROUND.value,
                        'paper_bgcolor': style.IrisDerivedColor.BACKGROUND.value,
                        'hovermode': 'x unified',
                        'hoverdistance': -1  # no limit
                    }
                ),
                channels=plot_cfg.channels_shown
            )

        class UpdatePlotOutputMultiplexer_T(TypedDict):
            """Dict typedef so mypy can make sure
            `update_plot_data_multiplexer` is returning the right data in its
            return dict."""
            loaded_timespan: Tuple[int, int]
            prependData: _TimePlotAIO_w_Callbacks.GraphUpdate_T | dash._callback.NoUpdate
            extendData: _TimePlotAIO_w_Callbacks.GraphUpdate_T | dash._callback.NoUpdate
            n_refresh: int | dash._callback.NoUpdate

        @callback(
            output=dict(
                prependData=Output(ids.plot(MATCH), 'prependData'),
                extendData=Output(ids.plot(MATCH), 'extendData'),
                loaded_timespan=Output(
                    ids.loaded_timespan_store(MATCH), 'data'),
                n_refresh=Output(ids.refresh_button(MATCH), 'n_clicks')
            ),
            inputs=dict(
                n_intervals=Input(ids.interval(MATCH), 'n_intervals'),
                disp_timespan=Input(ids.disp_timespan_store(MATCH), 'data'),
                n_purge=Input(ids.purge_outside_button(MATCH), 'n_clicks')
            ),
            state=dict(
                loaded_timespan=State(
                    ids.loaded_timespan_store(MATCH), 'data'),
                selected_channels=State(ids.channels_store(MATCH), 'data'),
                keep_live=State(ids.keep_live_toggle(MATCH), 'on'),
                n_refresh=State(ids.refresh_button(MATCH), 'n_clicks')
            )
        )
        def update_plot_data_multiplexer(
            n_intervals: int,
            disp_timespan: Tuple[int, int],
            loaded_timespan: Tuple[int, int],
            selected_channels: List[str],
            keep_live: bool,
            n_purge: int,
            n_refresh: int
        ) -> UpdatePlotOutputMultiplexer_T:
            """Updates plot data, multiplexing the various distinct
            sub-callbacks that want to mutate that data.

            Why:
            Due to dash's one callback per output rule, in the case where
            multiple distinct callbacks want to mutate the same output, we need
            to multiplex them and decide which one to call based on the
            inputs."""
            # Don't proceed if the graph hasn't been initialized by the fetch
            # process yet:
            if not (
                isinstance(selected_channels, list)
                and len(selected_channels) > 0
            ):
                raise dash.exceptions.PreventUpdate

            # Initialize default output with nothing changing:
            output = _TimePlotAIO_w_Callbacks.UpdatePlotOutputMultiplexer_T(
                loaded_timespan=loaded_timespan,
                prependData=no_update,
                extendData=no_update,
                n_refresh=no_update
            )

            # Check if this is a purge event, in which case
            # (NOTE: unfortunately, we can't just have this in a separate
            # callback b/c dash doesn't let multiple callbacks output to the
            # save property, in this case `loaded_timespan_store`):
            if ctx.args_grouping['n_purge']['triggered']:
                # Update outputs with outputs from the sub-callback
                return {
                    **output,  # type: ignore
                    **_TimePlotAIO_w_Callbacks.purge_outside_view(
                        disp_timespan=disp_timespan,
                        n_refresh=n_refresh
                    )
                }
            else:
                # Update outputs with outputs from the sub-callback
                return {
                    **output,  # type: ignore
                    **_TimePlotAIO_w_Callbacks.update_plot_data(
                        n_intervals=n_intervals,
                        disp_timespan=disp_timespan,
                        loaded_timespan=loaded_timespan,
                        selected_channels=selected_channels,
                        keep_live=keep_live
                    )
                }

        class PurgeOutsideView_T(TypedDict):
            """Dict typedef so mypy can make sure `update_plot_data` is
            returning the right data in its return dict."""
            loaded_timespan: Tuple[int, int]
            n_refresh: int | dash._callback.NoUpdate

        @staticmethod
        def purge_outside_view(
            disp_timespan: Tuple[int, int],
            n_refresh: int
        ) -> PurgeOutsideView_T:
            """
            Sets the loaded timespan to match the display window, effectively
            purging all data outside the loaded view. This is completed by
            triggering a data refresh.
            """
            return dict(
                loaded_timespan=disp_timespan,
                n_refresh=n_refresh+1  # trigger refresh update
            )

        class UpdatePlotOutput_T(TypedDict):
            """Dict typedef so mypy can make sure `update_plot_data` is
            returning the right data in its return dict."""
            prependData: _TimePlotAIO_w_Callbacks.GraphUpdate_T
            extendData: _TimePlotAIO_w_Callbacks.GraphUpdate_T
            loaded_timespan: Tuple[int, int]

        @staticmethod
        def update_plot_data(
            n_intervals: int,
            disp_timespan: Tuple[int, int],
            loaded_timespan: Tuple[int, int],
            selected_channels: List[str],
            keep_live: bool
        ) -> UpdatePlotOutput_T:
            # Fetch latest data for the given range at a fixed rate:
            if not backend.db_ready(context.inner_db):
                raise dash.exceptions.PreventUpdate

            # Initialize no-update return structures:
            prependData: _TimePlotAIO_w_Callbacks.GraphUpdate_T = [dict(), []]
            extendData: _TimePlotAIO_w_Callbacks.GraphUpdate_T = [dict(), []]
            new_loaded_timespan: List[int] = [*loaded_timespan]

            now_timestamp = data.datetime_to_timestamp(datetime.now())
            # Create a basic plot config object:
            # (this will implicitly validate the inputs too.)
            plot_cfg = _TimePlotAIO_w_Callbacks.plot_config_from_stores(
                channels_shown_store=selected_channels,
                disp_timespan_store=disp_timespan,
                loaded_timespan_store=loaded_timespan
            )

            # If the display window is now earlier than the loaded data,
            # load new data:
            if plot_cfg.disp_timespan[0] < plot_cfg.loaded_timespan[0]:
                ch_data = _TimePlotAIO_w_Callbacks.get_channel_data(
                    plot_cfg.channels_shown,
                    from_time=plot_cfg.disp_timespan[0],
                    to_time=plot_cfg.loaded_timespan[0]
                )
                prependData = _TimePlotAIO_w_Callbacks.channel_data_to_graph_update(
                    ch_data
                )
                new_loaded_timespan[0] = plot_cfg.disp_timespan[0]

            if keep_live:
                # If keep_live, load data from the end of the load window to
                # the present:
                ch_data = _TimePlotAIO_w_Callbacks.get_channel_data(
                    plot_cfg.channels_shown,
                    from_time=plot_cfg.loaded_timespan[1],
                    to_time=now_timestamp
                )
                extendData = _TimePlotAIO_w_Callbacks.channel_data_to_graph_update(
                    ch_data
                )
                new_loaded_timespan[1] = now_timestamp
            else:
                # If the display window is now later than the loaded data,
                # load new data:
                ch_data = _TimePlotAIO_w_Callbacks.get_channel_data(
                    plot_cfg.channels_shown,
                    from_time=plot_cfg.loaded_timespan[1],
                    to_time=plot_cfg.disp_timespan[1]
                )
                extendData = _TimePlotAIO_w_Callbacks.channel_data_to_graph_update(
                    ch_data
                )
                new_loaded_timespan[1] = plot_cfg.disp_timespan[1]

            return dict(
                prependData=prependData,
                extendData=extendData,
                loaded_timespan=(
                    new_loaded_timespan[0], new_loaded_timespan[1]
                )
            )

    return _TimePlotAIO_w_Callbacks(context, *args, **kwargs)
