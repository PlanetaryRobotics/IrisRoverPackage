"""
Event Stream Display Component with active lookup from a timestamped Stream in
a Redis Database.

Following pattern from: https://dash.plotly.com/all-in-one-components

Author: Connor W. Colombo (colombo@cmu.edu)
Last Updated: 06/02/2023
"""
from __future__ import annotations

import os

from typing import Any, Final, Dict, Tuple, List, Sequence, cast
from datetime import datetime, timedelta
import uuid

import pandas as pd  # type: ignore
import dash  # type: ignore
from dash import (
    html, dcc, dash_table,
    Output, Input, State,
    callback, MATCH,
    ctx, no_update
)
import dash_bootstrap_components as dbc  # type: ignore
import dash_daq as daq  # type: ignore

from . import aio
from ..context import GuiContext
from .. import backend
from .. import data
from .. import style
from .. import utils

# Maximum amount of time allowed between event stream refreshes (even if a long
# fetch is running):
_STALE_TIME_SEC: Final[float] = 1.0


def stream_id_to_time_id(stream_id: str) -> int:
    """Converts the given stream id string (`"{time}-{seqnum}"`) into an int of
    the form: `timestamp*1000 + seq_num` that can be used to index the event
    stream dataframe.

    NOTE: This only works without collisions assuming there are at most 1000
    events that occur in the same millisecond (a reasonable restriction).
    If more are possible, bump up the coefficient."""
    timestamp, seq_str = stream_id.split('-', 1)
    seq_num: int
    try:
        seq_num = int(seq_str)
    except:
        seq_num = 0  # str is wildcard like '*'
    return int(timestamp)*1000 + int(seq_num)


def event_stream_dataframe_from_entries(
    event_entries: Sequence[Tuple[str, Dict]]
) -> pd.DataFrame:
    """Creates a dataframe representing an event stream with the given
    `event_entries`, given as a list of `(stream_id, event)` tuples where:
    `event` is a `EventDbPayload` pulled from the DB
        (can be any dict, though- only fields from EventDbPayload will be
        included.)
    `stream_id` is the time index when each `event` happened.

    This ensures that the only columns are:
        - index (`time_id`): a time index when all each event row occurred
          (per `stream_id_to_time_id`)
        - `time`: the `datetime` when each event occurred
        - `full_name`: fully qualified name of the event (`module_\nevent`)
        - every field in the `EventDbPayload` `TypedDict`

    This function serves as a the single source of truth for the structure of
    the event stream dataframe.
    """
    # Add index `time_id` column to each
    for stream_id, event in event_entries:
        event['time_id'] = stream_id_to_time_id(stream_id)
        event['time'] = data.stream_parse_id(stream_id)[0]
        # Make a column containing the combined fully qualified name:
        event['full_name'] = (
            f"{event.get('module_name', 'None')}_\n"
            f"{event.get('event_name', 'None')}"
        )

    # Create DataFrame from dict entries, ensuring only and all of the expected
    # columns are present:
    df = pd.DataFrame(
        (e
         for _, e in event_entries
         # TODO: Replace with default filter on lvl col data table of '>0' (if possible):
         if 'severity_lvl' in e and str(e['severity_lvl']) != '0'  # safer
         ),
        columns=[
            'time_id',
            'time',
            'full_name',
            *data.EventDbPayload.__annotations__.keys()
        ]
    )
    df.set_index('time_id', inplace=True)

    return df


def init_event_stream_dataframe() -> pd.DataFrame:
    """Creates an empty event stream dataframe (with appropriately formatted
    columns, index, etc.)."""
    return event_stream_dataframe_from_entries([])


def update_event_stream_dataframe(
    df: pd.DataFrame,
    event_entries: Sequence[Tuple[str, data.EventDbPayload]]
) -> pd.DataFrame:
    """Updates the dataframe containing the event stream with the
    `EventDbPayload`s pulled from the DB (`events`) that occurred at
    the given `stream_ids`. Args are passed in a list of `(stream_id, event)`
    tuples.
    NOTE: This does **NOT** happen in place.
    """
    # TypedDict isn't a run-time thing so we can make it a dict her so its
    # mutable:
    event_entries_mut = cast(Sequence[Tuple[str, Dict]], event_entries)
    new_df = event_stream_dataframe_from_entries(event_entries_mut)
    return pd.concat(
        [df, new_df]
    )


class _EventStreamAIO(html.Div):
    # NOTE: All-in-One Component Classes should be suffixed with 'AIO'
    table: dash_table.DataTable
    # How **LONG** (in time) each page should be:
    page_time_slider: dcc.Slider
    interval: dcc.Interval  # frequently queries the latest data from inner_db

    # Store that contains a tuple of the timestamp and interval count when the
    # last update completed.
    # Updates will only be allowed to fire if their interval count is
    # `1+last_update_count` (subsequent interval) or if time-last_update_time
    # > `_STALE_TIME_SEC` (in case the only allowed interval didn't fire or
    # complete and we need to get the ball rolling again).
    interval_last_update: dcc.Store[Tuple[int, int]]  # (timestamp, count)

    # STORES (note: each output can only be associated with at most 1 callback,
    # so some datastructures may need to be split among stores)
    # Timestamp for the earliest data allowed to be shown
    # (updated to `now` every interval if `keep_live`):
    timespan_head_store: dcc.Store[int]
    # Button to toggle live updating:
    keep_live_toggle: daq.BooleanSwitch

    # Label Displaying the "time_page" being Shown:
    time_page_label: html.Label

    # This comes from the Dash AIO pattern. Allows for global lookup of
    # subcomponent ids given a parent aio_id. Seems to be necessary for
    # MATCH-based pattern matching for callbacks inside the AIO component
    # instance(s).
    class ids:
        table = aio.id_generator('_EventStreamAIO', 'table')
        page_time_slider = aio.id_generator(
            '_EventStreamAIO', 'page_time_slider')
        interval = aio.id_generator('_EventStreamAIO', 'interval')
        interval_last_update = aio.id_generator(
            '_EventStreamAIO', 'interval_last_update')
        timespan_head_store = aio.id_generator(
            '_EventStreamAIO', 'timespan_head_store')
        keep_live_toggle = aio.id_generator(
            '_EventStreamAIO', 'keep_live_toggle')
        time_page_label = aio.id_generator(
            '_EventStreamAIO', 'time_page_label')
        loading_indicator = aio.id_generator(
            '_EventStreamAIO', 'loading_indicator')

    def __init__(
        self,
        context: GuiContext,
        table_props: Dict | None = None,
        page_time_slider_props: Dict | None = None,
        interval_props: Dict | None = None,
        aio_id=None
    ) -> None:
        # Make a uuid if an override is not given:
        aio_id = str(uuid.uuid4()) if aio_id is None else aio_id

        # Supply empty dicts for all prop overrides if none given:
        table_props = aio.props_guard(table_props)
        page_time_slider_props = aio.props_guard(page_time_slider_props)
        interval_props = aio.props_guard(interval_props)

        # Setup data structures:
        init_events_df = init_event_stream_dataframe()

        # Build Components:
        self.page_time_slider = dcc.Slider(
            id=self.ids.page_time_slider(aio_id),
            **{
                'min': 1,
                'max': 60,
                'value': 2,
                'marks': {
                    t: f'{t}min'
                    for t in range(0, 70, 10)
                },
                'tooltip': {"placement": "bottom", "always_visible": False},
                **page_time_slider_props
            }
        )

        table_style = {
            'style_cell_conditional': [
                # General Text Alignment:
                *[{
                    'if': {'column_id': c},
                    'textAlign': 'left'
                } for c in ['full_name', 'module_name', 'event_name', 'event_html']],
                # Special overflow handling for the event_html column:
                {
                    'if': {'column_id': 'event_html'},
                    # force a fixed size...
                    # (other columns will shrink if necessary)
                    'minWidth': '60ch',
                    'width': '60ch',
                    'maxWidth': '60ch',
                    # ... then break and wrap text as needed to fit:
                    'white-space': 'normal',
                    'word-break': 'break-all',
                    'height': 'auto'
                }
            ],
            'style_data': dict(),  # have at least an empty 'style_data'
            'style_table': dict(),  # have at least an empty 'style_table'
            'style_cell': dict(),  # have at least an empty 'style_cell'
            **style.TABLE_STYLE_MIXIN
        }
        # Take whatever the mixed in `'style_data'`` is and add
        # unique properties to it:
        table_style['style_data'] = {
            **cast(Dict, table_style['style_data']),
            **cast(Dict, {  # expand a second dict so this can override preceding keys
                'whiteSpace': 'normal',
                'height': 'auto',
                'lineHeight': '20px'
            })
        }
        table_style['style_table'] = {
            **cast(Dict, table_style['style_table']),
            **cast(Dict, {  # expand a second dict so this can override preceding keys
                'minHeight': '10vh',
                'maxHeight': '72vh',
                'overflowY': 'scroll'
            })
        }
        self.table = dash_table.DataTable(
            id=self.ids.table(aio_id),
            data=self.df_to_display(init_events_df),
            columns=[
                {'id': 'time', 'name': 'Time'},
                {'id': 'pathway_str', 'name': 'Path'},
                {'id': 'severity_lvl', 'name': 'Lvl'},
                # {'id': 'severity_str', 'name': 'Severity'},
                {'id': 'full_name', 'name': 'Event Name'},
                # {'id': 'module_name', 'name': 'Module'},
                # {'id': 'event_name', 'name': 'Event'},
                {'id': 'event_html', 'name':  'Message', 'presentation': 'markdown'}
            ],
            **{
                'markdown_options': {'html': True},
                'page_action': 'custom',
                'page_current': 0,
                'page_size': 20,
                'filter_action': 'custom',
                'filter_query': '',
                'filter_options': {
                    'placeholder_text': 'Filter...',
                    'tooltip': {
                        'enabled': True,
                        'header': (
                            "Filter by column. Consider using '$no X', "
                            "'$no [X, Y, Z]', '$any [X,Y,Z]', or "
                            "'$all [X,Y,Z]'."
                        ),
                        'format': "markdown"
                    }
                },
                **table_style,
                # Change number of columns frozen when x-scrolling:
                'fixed_columns': {'headers': True, 'data': 0},
                **table_props
            }
        )

        self.keep_live_toggle = daq.BooleanSwitch(
            id=self.ids.keep_live_toggle(aio_id),
            label='Live',
            labelPosition="top",
            on=True
        )

        self.interval = dcc.Interval(
            id=self.ids.interval(aio_id),
            **{
                'interval': float(os.environ.get('IBv3_GDS_EVENTS_UPDATE_INTERVAL_MS', 0.2*1000)),
                'n_intervals': 0,
                **interval_props
            }
        )

        self.interval_last_update = dcc.Store(
            id=self.ids.interval_last_update(aio_id),
            data=(0, 0)
        )

        now = datetime.now()
        self.time_page_label = html.Div(
            self.make_time_page_label(now, now, now),
            id=self.ids.time_page_label(aio_id),
            **style.LABEL_STYLE_MIXIN
        )

        # Build store(s):
        # Split keys across stores (needs to be fragmented b/c of the 1
        # callback per output rule):
        self.timespan_head_store = dcc.Store(
            id=self.ids.timespan_head_store(aio_id),
            data=data.stream_datetime_to_timestamp(datetime.now())
        )

        # Build layout:
        super().__init__([  # `html.Div([...])`
            dbc.Row([
                dbc.Col([
                    html.Label(
                        "Minutes Shown:",
                        **{
                            **style.LABEL_STYLE_MIXIN,
                            'style': {
                                **style.LABEL_STYLE_MIXIN.get('style', dict()),
                                'fontWeight': 'thin'
                            }
                        }),
                    self.page_time_slider,
                    dbc.Row([
                        self.time_page_label
                    ])
                ], width=10, align='end'),  # 10/12
                dbc.Col([
                    self.keep_live_toggle
                ], width=1)
            ], justify='center', align='start'),
            html.Br(),
            self.table,
            self.interval,
            self.timespan_head_store,
            self.interval_last_update
        ])

    @staticmethod
    def make_time_page_label(
        from_time: datetime,
        to_time: datetime,
        now: datetime | None = None,
        always_show_seconds: bool = False
    ) -> List:
        """Makes a consistently formatted children for the "time page" label.
        """
        if now is None:
            now = datetime.now()

        to_str: str
        to_delta = (now - to_time).total_seconds()
        if abs(to_delta) < 30 and (not always_show_seconds or to_delta <= 1):
            to_str = "now"
        else:
            to_str = utils.seconds_to_str(to_delta) + " ago"

        from_str = utils.seconds_to_str(
            (now - from_time).total_seconds()) + " ago"

        return [
            " from ",
            html.B(from_str),
            " to ",
            html.B(to_str)
        ]

    @staticmethod
    def table_to_json(df: pd.DataFrame) -> str:
        """Converts the given df to json in a standardized way (being
        careful about datetime conversions, etc).
        Used for putting a DataFrame into a `dcc.Store`."""
        # Convert name to be "datelike" (per`pd.read_json`):
        # Nothing to do for this table atm.
        return str(df.to_json(date_format='iso', date_unit='ms'))

    @staticmethod
    def table_from_json(json_str: str) -> pd.DataFrame:
        """Converts the given json to a df in a standardized way (being
        careful about datetime conversions, etc).
        Used for retrieving a DataFrame into a `dcc.Store`."""
        df = pd.read_json(json_str, convert_dates=True)
        # Convert from "datelike" name (per`pd.read_json`):
        # Nothing to do for this table atm.
        return df

    @staticmethod
    def pretty_format_table(
        df: pd.DataFrame,
        inplace: bool = True
    ) -> pd.DataFrame:
        """Returns a pretty-formatted version of the `event_stream_df`
        `DataFrame`."""
        if inplace:
            df_out = df
        else:
            df_out = df.copy()

        df_out['time'] = [x.strftime('%H:%M:%S') for x in df_out['time']]

        return df_out.fillna('')

    @staticmethod
    def df_to_display(df: pd.DataFrame, **kwargs):
        """Processes and pretty formats dataframe then returns it as a records
        dict (format required by data_table)."""
        return _EventStreamAIO.pretty_format_table(df, **kwargs) \
                              .to_dict('records')


def make_event_stream_aio(context: GuiContext, *args, **kwargs) -> _EventStreamAIO:
    """Makes a subclass of `_EventStreamAIO` with implemented context-dependent
    callbacks."""
    class _EventStreamAIO_w_Callbacks(_EventStreamAIO):
        ids = _EventStreamAIO.ids

        # Stateless pattern-matching callbacks for every component instance:

        @callback(
            output=Output(ids.timespan_head_store(MATCH), 'data'),
            inputs=dict(
                n_intervals=Input(ids.interval(MATCH), 'n_intervals')
            ),
            state=dict(
                keep_live=State(ids.keep_live_toggle(MATCH), 'on')
            )
        )
        def update_timespan_head(
            keep_live: bool,
            n_intervals: int
        ) -> int | dash._callback.NoUpdate:
            """Updates the timespan head to `now` every interval if `keep_live`
            is `on`."""
            if not keep_live:
                return no_update
            else:
                return data.stream_datetime_to_timestamp(datetime.now())

        @callback(
            output=dict(
                table_data=Output(ids.table(MATCH), 'data'),
                time_page_label=Output(ids.time_page_label(MATCH), 'children'),
                interval_last_update=Output(
                    ids.interval_last_update(MATCH), 'data')
            ),
            inputs=dict(
                n_intervals=Input(ids.interval(MATCH), 'n_intervals'),
                interval_last_update=Input(
                    ids.interval_last_update(MATCH), 'data'),
                page_time_mins=Input(ids.page_time_slider(MATCH), 'value'),
                page_current=Input(ids.table(MATCH), 'page_current'),
                time_head=Input(ids.timespan_head_store(MATCH), 'data'),
                keep_live=Input(ids.keep_live_toggle(MATCH), 'on'),
                filter_query=Input(ids.table(MATCH), 'filter_query')
            )
        )
        def fetch_table_data(
            page_time_mins: int,
            page_current: int,
            time_head: int,
            keep_live: bool,
            filter_query: str,
            n_intervals: int,
            interval_last_update: Tuple[int, int]
        ) -> Dict[str, Any] | dash._callback.NoUpdate:
            """
            Updates the data in the events stream table.
            NOTE: Since each object can only be outputted to from (at most) one
            callback, all functions that update the table contents must happen
            here.

            Data is fetched and displayed in the time domain (not as a count).

            The data shown will always be:
                - from: `time_head - (page_current)*page_time`
                - to: `time_head - (page_current-1)*page_time`

            This update is triggered every interval or if any of the settings
            change.

            Updates `page_size` to show so exactly the number of elements
            fetched is the number shown.
            """
            # Abort if we aren't allowed to do this update:
            now = datetime.now()
            last_timestamp, last_count = interval_last_update
            last_time = datetime.fromtimestamp(last_timestamp)
            if not (
                n_intervals == (last_count + 1)  # subsequent fire
                or (now - last_time).total_seconds() > _STALE_TIME_SEC  # stale
                or interval_last_update == (0, 0)  # first update
            ):
                raise dash.exceptions.PreventUpdate

            if not backend.db_ready(context.inner_db):
                raise dash.exceptions.PreventUpdate

            if page_current == 0:
                end_time = data.stream_timestamp_to_datetime(time_head)
            else:
                end_time = (
                    data.stream_timestamp_to_datetime(time_head)
                    - timedelta(minutes=page_time_mins * (page_current-1))
                )
            start_time = end_time - timedelta(minutes=page_time_mins)

            event_entries = data.get_events_from_stream(
                context.inner_db,
                start_timestamp=data.stream_datetime_to_timestamp(start_time),
                end_timestamp=data.stream_datetime_to_timestamp(end_time),
                count=None,
                from_end=True
            )

            # Build & format DataFrame from entries:
            df = event_stream_dataframe_from_entries(event_entries)
            if len(filter_query) != 0:
                df = utils.dash_table_default_filter_table(df, filter_query)
            disp_table = _EventStreamAIO.df_to_display(df)

            return dict(
                table_data=disp_table,
                time_page_label=_EventStreamAIO.make_time_page_label(
                    start_time, end_time,
                    always_show_seconds=(not keep_live)
                ),
                interval_last_update=(now.timestamp(), n_intervals)
            )

    return _EventStreamAIO_w_Callbacks(context, *args, **kwargs)
