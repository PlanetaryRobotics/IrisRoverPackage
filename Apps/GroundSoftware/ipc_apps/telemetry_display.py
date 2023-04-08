"""
CLI display of telemetry.

Also serves as a demo/test of one of the simplest possible real-world IPC Apps
that use `IpcAppManagerAsync` involving additional tasks outside of a
manager (and inter-task communication).

Last Updated: 03/09/2023
"""
import asyncio
import argparse
from datetime import datetime
import pandas as pd
from dataclasses import dataclass
from typing import Callable, ClassVar, List

if __name__ == "__main__":
    # very slow (embedded) machines may take a while to import the library,
    # so print this first to let the user know things are happening:
    print("Booting . . .")

import IrisBackendv3 as IB3
import IrisBackendv3.ipc as ipc
from IrisBackendv3.ipc.messages import (
    DownlinkedPacketsMessage
)

from IrisBackendv3.utils import console_display

parser = argparse.ArgumentParser(description=(
    'IRIS Lunar Rover — IPC Telemetry Display — CLI'
))


def get_opts():
    parser.add_argument('-n', '--name', type=str, default="TelemetryDisplay",
                        help=(
                            "Name of this App (can be configured to "
                            "accomplish different goals)."
                        ))
    parser.add_argument('-r', '--refresh-interval-sec', type=float, default=1.0,
                        help=(
                            "How many seconds (max) between screen refreshes. "
                            "(should be a multiple of 1s or 0.1s to minimize "
                            "flicker on the time entries.)"
                        ))
    return parser.parse_args()


@dataclass
class RedrawScreenRequest:
    # If this update is triggered from packets being received, packets will contain this list.
    # If this update is just because the screen needs to be redrawn, `packets is None`.
    packets: List[IB3.codec.packet.Packet] | None = None


async def screen_updater(queue: asyncio.Queue[RedrawScreenRequest]) -> None:
    """Task responsible for redrawing the screen.

    We buffer this with a queue since we want to redraw the queue when either:
        - A new collection of packets arrives from the XCVR(s).
        - A fixed update interval passes.

    Args:
        queue (asyncio.Queue[RedrawScreenMessage]): Async queue for
    """
    # Initialize DataFrames:
    packet_log_dataframe = console_display.init_packet_log_dataframe(
        pd.DataFrame()
    )
    telemetry_payload_log_dataframe = console_display.init_telemetry_payload_log_dataframe(
        pd.DataFrame()
    )

    while True:
        # Wait for a new update request:
        request = await queue.get()

        # If this request has passed us packets to use to update the dataframes,
        # update the dataframes:
        if request.packets is not None:
            # Add packet data to dataframes:
            for packet in request.packets:
                packet_log_dataframe = console_display.update_packet_log_dataframe(
                    packet_log_dataframe,
                    packet
                )
                telemetry_payload_log_dataframe = console_display.update_telemetry_payload_log_from_packet(
                    telemetry_payload_log_dataframe,
                    packet
                )

        # Update the timestamps no matter what:
        console_display.update_all_packet_log_times(
            packet_log_dataframe,
            now=datetime.now()
        )

        # Build the screen:
        screen = ""
        # Packet summary up top:
        screen += console_display.str_packet_log_dataframe(
            packet_log_dataframe
        )
        # Then draw the telemetry table below:
        screen += "\n"
        screen += console_display.str_telemetry_payload_log_dataframe(
            telemetry_payload_log_dataframe
        )

        # Clear the screen and redraw it
        # (all together as once fast op so there's no flicker):
        console_display.clear_console()
        print(screen)

        # Flag that we've gotten everything we need from this queue item and
        # are done processing it:
        queue.task_done()


async def screen_refresher(
    interval_sec: float,
    screen_redraw_queue: asyncio.Queue[RedrawScreenRequest]
) -> None:
    """Pushes an screen redraw request to the queue at a fixed interval.
    """
    while True:
        await asyncio.sleep(interval_sec)
        screen_redraw_queue.put_nowait(RedrawScreenRequest())


# Setup the app:
class Sub(ipc.SocketTopicHandlerAsync['Sub']):
    """A demo Subscriber that only cares about 1 topic and ends everything
    after it gets 10 messages."""
    _raise_on_unhandled_topics: ClassVar[bool] = False
    _require_unhandled_topic_handler: ClassVar[bool] = False
    # Decorator shorthand (also plays more nicely with syntax highlighting):
    topic_handler = ipc.SocketTopicHandlerAsync['Sub'].TopicHandlerFactory()

    # Queue to push to in order to request a screen redraw:
    screen_redraw_queue: asyncio.Queue[RedrawScreenRequest]

    def __init__(
        self,
        screen_redraw_queue: asyncio.Queue[RedrawScreenRequest],
        *args, **kwargs
    ) -> None:
        self.screen_redraw_queue = screen_redraw_queue
        super().__init__(*args, **kwargs)

    @topic_handler
    async def downlink_handler(
        self,
        manager: ipc.IpcAppManagerAsync,
        payload: ipc.IpcPayload
    ) -> None:
        # Decode the message:
        msg = ipc.guard_msg(payload.message, DownlinkedPacketsMessage)
        # Push the packets to the screen update queue:
        self.screen_redraw_queue.put_nowait(
            RedrawScreenRequest(msg.content.packets)
        )

    _topic_handlers: ClassVar[ipc.SocketTopicHandlerAsync.TopicHandlerRegistry] = {
        ipc.Topic.DL_PACKETS: downlink_handler
    }


def build_manager(
    screen_redraw_queue: asyncio.Queue[RedrawScreenRequest]
) -> ipc.IpcAppManagerAsync:
    manager = ipc.IpcAppManagerAsync(socket_specs={
        'sub': ipc.SocketSpec(
            sock_type=ipc.SocketType.SUBSCRIBER,
            port=ipc.Port.TRANSCEIVER,
            topics=Sub.TOPICS(),
            rx_handler=Sub(screen_redraw_queue),
            bind=False
        )
    })
    return manager


async def main(opts):
    screen_redraw_queue: asyncio.Queue[RedrawScreenRequest] = asyncio.Queue()

    manager = build_manager(screen_redraw_queue)
    # Init all core tasks for the manager:
    manager.spawn_core_tasks()
    # Add coroutines outside the manager to the task list:
    manager.add_coros([
        screen_updater(screen_redraw_queue),
        screen_refresher(opts.refresh_interval_sec, screen_redraw_queue)
    ])

    # Run all tasks:
    await manager.run(return_when=asyncio.FIRST_COMPLETED)


if __name__ == "__main__":
    # Get CLI args:
    opts = get_opts()

    # Set app config:
    app = ipc.IpcAppHelper(opts.name)
    # Initialize Iris Backend:
    IB3.init_from_latest()

    # Run the app:
    asyncio.run(main(opts))
