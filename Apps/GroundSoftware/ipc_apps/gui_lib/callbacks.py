"""
Responsible for all application-level Callbacks (as opposed to component-level
callbacks which operate on data from within one component and change only that
same component with it).

Author: Connor W. Colombo (colombo@cmu.edu)
Last Updated: 05/09/2023
"""
from typing import cast, Final, Union, Dict, List

import pandas as pd

import dash
from dash import Dash
import dash_bootstrap_components as dbc
import dash_bootstrap_templates as dbt

from .context import GuiContext
from .layout import GuiAIO

import IrisBackendv3 as IB3
import IrisBackendv3.ipc as ipc
from IrisBackendv3.codec.packet import Packet
from IrisBackendv3.codec.payload import TelemetryPayload, EventPayload
from IrisBackendv3.ipc.messages import (
    DownlinkedPacketsMessage, DownlinkedPayloadsMessage
)

from IrisBackendv3.utils import console_display


def add_app_callbacks(context: GuiContext) -> Dash:
    """Adds Callbacks to the given Dash App.
    Returns the Dash App.
    """
    # Make sure all the necessary data is present:
    if context.dash_app is None:
        raise Exception(
            "`context.dash_app` needs to be added to the the context before "
            "attempting to add app callbacks. "
        )
    if context.dom is None:
        raise Exception(
            "`context.dom` needs to be added to the the context before "
            "attempting to add app callbacks. "
        )
    if not isinstance(context.dom, GuiAIO):
        raise Exception(
            "`context.dom` needs to be an instance of `GuiAIO`, "
            f"instead it is: `{type(context.dom)}`. "
        )

    # Add application callbacks:

    # NOTE: At present, there are no application-level callbacks.
    # Below is a template of what one looked like before being removed:
    """
    ```py
    @context.dash_app.callback(
        output=dict(
            telem=dash.Output(context.dom.telem_table.telem_store, 'data')
        ),
        inputs=dict(signal=dash.Input(context.dom.ipc_signal, 'data')),
        state=dict(
            telem_store=dash.State(context.dom.telem_table.telem_store, 'data')
        ),
        background=True,
        manager=context.dash_callback_mgr
    )
    def get_more_data(signal, telem_store):
        "Background callback to fetch IPC data. Triggers itself when done
        (effectively a loop)."
        telem_df = pd.read_json(telem_store)
        telem_df = ingest_data(context, telem_df)
        return dict(
            telem=telem_df.to_json(date_format='iso', date_unit='ms')
        )
    ```
    """

    return context.dash_app
