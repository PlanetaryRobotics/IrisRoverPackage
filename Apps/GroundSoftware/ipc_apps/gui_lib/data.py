"""
IPC Data Pipeline for GDS GUI.

Author: Connor W. Colombo (colombo@cmu.edu)
Last Updated: 05/09/2023
"""
import asyncio
from typing import Any, Final, ClassVar, List

import pandas as pd

from IrisBackendv3.codec.packet_classes.packet import Packet
from IrisBackendv3.codec.payload_collection import EnhancedPayloadCollection
from IrisBackendv3.utils.console_display import (
    init_telemetry_payload_log_dataframe,
    update_telemetry_payload_log_from_payloads,
    init_packet_log_dataframe,
    update_packet_log_dataframe
)

import IrisBackendv3.ipc as ipc
from IrisBackendv3.ipc.messages import (
    DownlinkedPacketsMessage, DownlinkedPayloadsMessage
)

from .context import GuiContext
from .backend import DatabaseKey, db_ready


def make_ipc_manager(context: GuiContext) -> ipc.IpcAppManagerAsync:
    class Sub(ipc.SocketTopicHandlerAsync['Sub']):
        """Handles subscriptions to all topics we care about."""
        _raise_on_unhandled_topics: ClassVar[bool] = False
        _require_unhandled_topic_handler: ClassVar[bool] = False
        # Decorator shorthand (also plays more nicely with syntax highlighting):
        topic_handler = ipc.SocketTopicHandlerAsync['Sub'].TopicHandlerFactory(
        )

        # Setup internal state:
        telem_df: pd.DataFrame

        def __init__(self, *args, **kwargs) -> None:
            # Init internal copy of dataframes:
            # NOTE: We're using an internal copy of the telem_df instead of
            # loading it from db first because, if a DB push fails for some
            # reason, we don't want to lose our updates. We'll just layer on
            # another update next time and try the push again.
            self.telem_df = init_telemetry_payload_log_dataframe(
                pd.DataFrame()
            )
            # Set a specific `dtype` for each column so it's serializable
            # (can't change this inside `init_telemetry_payload_log_dataframe`
            # because it needs to provide legacy support to CLI apps).
            self.telem_df = self.telem_df.astype(dtype={
                'Module': 'string',
                'Channel': 'string',
                'nRX': 'Int64',
                'Updated': 'datetime64',
                'Current Value': 'string',
                'H+1': 'string',
                'H+2': 'string',
                'H+3': 'string'
            })

            self.packet_df = init_packet_log_dataframe(
                pd.DataFrame()
            )

            super().__init__(*args, **kwargs)

        @topic_handler
        async def dl_payload_handler(
            self,
            manager: ipc.IpcAppManagerAsync,
            ipc_payload: ipc.IpcPayload
        ) -> None:
            payloads: EnhancedPayloadCollection
            try:
                msg = ipc.guard_msg(ipc_payload.message,
                                    DownlinkedPayloadsMessage)
                payloads = msg.content.payloads
            except Exception as e:
                context.ipc_app.logger.error(
                    f"Failed to decode IPC message `{ipc_payload.msg_bytes!r}` "
                    f"of `{ipc_payload=}` b/c: `{e}`."
                )
                # Just make an empty payload collection:
                payloads = EnhancedPayloadCollection()

            # Update the payload dataframe with whatever we got (if anything):
            self.telem_df = update_telemetry_payload_log_from_payloads(
                self.telem_df, payloads
            )

            # Update the internal database with this record (to make the data
            # available to Dash):
            if db_ready(context.inner_db):
                context.inner_db.save(DatabaseKey.TELEM_DF, self.telem_df)

        @topic_handler
        async def dl_packet_handler(
            self,
            manager: ipc.IpcAppManagerAsync,
            ipc_payload: ipc.IpcPayload
        ) -> None:
            packets: List[Packet]
            try:
                msg = ipc.guard_msg(ipc_payload.message,
                                    DownlinkedPacketsMessage)
                packets = msg.content.packets
            except Exception as e:
                context.ipc_app.logger.error(
                    f"Failed to decode IPC message `{ipc_payload.msg_bytes!r}` "
                    f"of `{ipc_payload=}` b/c: `{e}`."
                )
                packets = []  # Just make an empty list:

            # Update the payload dataframe with whatever we got (if anything):
            for packet in packets:
                self.packet_df = update_packet_log_dataframe(
                    self.packet_df, packet
                )

            # Update the internal database with this record (to make the data
            # available to Dash):
            if db_ready(context.inner_db):
                context.inner_db.save(DatabaseKey.PACKET_DF, self.packet_df)

        _topic_handlers: ClassVar[ipc.SocketTopicHandlerAsync.TopicHandlerRegistry] = {
            ipc.Topic.DL_PAYLOADS: dl_payload_handler,
            ipc.Topic.DL_PACKETS: dl_packet_handler
        }

    return ipc.IpcAppManagerAsync(socket_specs={
        'sub': ipc.SocketSpec(
            sock_type=ipc.SocketType.SUBSCRIBER,
            port=ipc.Port.TRANSCEIVER_SUB,
            topics=Sub.TOPICS(),
            rx_handler=Sub()
        )
    })
