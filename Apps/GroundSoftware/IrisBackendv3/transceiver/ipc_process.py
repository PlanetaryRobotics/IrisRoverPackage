"""
Standalone transceiver IPC App which manages the reading of telemetry and
sending of commands using (a) transceiver(s) in both the uplink and downlink
direction simultaneously.

@author: Connor W. Colombo (CMU)
@last-updated: 06/21/2023
"""
# Activate postponed annotations (for using classes as return type in their own methods)
from __future__ import annotations

from typing import Any, Final, ClassVar, Awaitable, Callable, Coroutine, Dict, TypeAlias, Type, Coroutine

if __name__ == "__main__":
    # very slow (embedded) machines may take a while to import the library,
    # so print this first to let the user know things are happening:
    print("Booting . . .")

import yaml
import asyncio
import argparse
import scapy.all as scp
from dataclasses import dataclass, field

from IrisBackendv3.transceiver.logs import logger_setConsoleLevel as xcvrLoggerLevel
from IrisBackendv3.transceiver.transceiver import Transceiver
from IrisBackendv3.transceiver.prebuilts import PrebuiltTransceiver, build_xcvr_by_name

import IrisBackendv3 as IB3
from IrisBackendv3.codec.packet import Packet

import IrisBackendv3.ipc as ipc
from IrisBackendv3.ipc.messages import (
    UplinkPacketRequestMessage, UplinkPacketRequestContent,
    DownlinkedPacketsMessage, DownlinkedPacketsContent,
)


# Create `Literal` names for each socket:
DOWNLINK_SOCK: Final[str] = 'downlink'
UPLINK_SOCK: Final[str] = 'uplink'


# Create CLI Parser:
parser = argparse.ArgumentParser(description=(
    'IRIS Lunar Rover — Transceiver IPC App — CLI'
))


def get_opts():
    """
    Return settings wrapped in argparse.
    """
    xcvr_names = [x.name for x in PrebuiltTransceiver]
    parser.add_argument('-x', '--prebuilt-xcvr-name',
                        type=str, required=True,
                        nargs='+', default=[],
                        choices=xcvr_names,
                        help=(
                            "Name of the prebuilt XCVR to use. "
                            "Multiple can be given to run in parallel. "
                            "At least 1 must be given."
                        ))

    # Several ways to specify YAML arguments:
    yaml_desc = (
        "Top level entries in the YAML file must be the name(s) of a "
        "transceiver given with the `--prebuilt-xcvr-name` arg and each "
        "entry's members are optional arguments which will get "
        "passed into the Transceiver constructor as "
        "keyword arguments."
    )
    parser.add_argument(
        '-y', '--xcvr-yaml-file', type=str,
        required=False, default="",
        help=(
            "Path to a YAML file to be used as optional args for the "
            "transceiver(s). " + yaml_desc
        )
    )
    parser.add_argument(
        '-s', '--xcvr-yaml-str', type=str,
        required=False, default="",
        help=(
            "YAML string to be used as optional args for the "
            "transceiver(s). If a YAML file is also given, any settings in "
            "the string will override settings in the YAML file. If adding "
            "additional settings for a certain transceiver that weren't in "
            "the file, the settings from both the file and the string for "
            "that transceiver will be merged. " + yaml_desc
        )
    )

    def str_to_log_level(s) -> str:
        if (level := IB3.logs.str_to_log_level(s)) is not None:
            return level
        else:
            raise argparse.ArgumentTypeError(
                f"Valid log level expected. "
                f"Log levels are: {IB3.logs.VALID_LOG_LEVELS}"
            )
    parser.add_argument('-l', '--log-level', type=str_to_log_level, default='VERBOSE',
                        help=(
                            "Console logging level to be used (i.e. how "
                            "annoying the logging printouts should be). Only "
                            "logs with this level or greater are displayed in "
                            "the console. Valid logging levels are: "
                            f"{IB3.logs.VALID_LOG_LEVELS}"
                        ))

    return parser.parse_args()


@dataclass
class DownlinkCoroutine:
    """Async functor for an `asyncio` coroutine to handle processing downlinked
    messages for a given transceiver."""
    app: ipc.IpcAppHelper
    manager: ipc.IpcAppManagerAsync
    xcvr: Transceiver
    n_packets_rcvd: int = 0
    xcvr_tag: bytes = field(init=False)

    def __post_init__(self) -> None:
        self.xcvr_tag = self.xcvr.name.encode('utf-8')

    async def _downlink_coro(self) -> None:
        """Inner async coroutine."""
        while True:
            # Wait until new packets arrive:
            packets = await self.xcvr.async_read()
            if len(packets) == 0:
                app.logger.notice(f"({self.xcvr.name}) Got 0 packets.")
                continue

            # Process them:
            for packet in packets:
                self.n_packets_rcvd += 1
                app.logger.notice(
                    f"({self.xcvr.name}) "
                    f"[{self.n_packets_rcvd:5d}] "
                    f"Got {packet.__class__.__name__}."
                )
            msg = DownlinkedPacketsMessage(DownlinkedPacketsContent(
                packets=packets
            ))

            # Publish them:
            try:
                await self.manager.send_to(
                    DOWNLINK_SOCK, msg, subtopic_bytes=self.xcvr_tag
                )
                app.logger.info(
                    f"({self.xcvr.name}) "
                    f"Published: {msg.content.simple_str()} "
                    f"-> {ipc.Topic.DL_PACKETS}"
                )
            except Exception as e:
                err_msg = (
                    f"[!DOWNLINK_PIPE_FAILURE!] \t"
                    f"Failed to publish DownlinkedPacketsMessage `{msg}` "
                    f"because: `{e!s}`. \n"
                    f"For archive purposes, contents of all packets:"
                )
                for packet in packets:
                    err_msg += (
                        f"\n\n{packet!s}\n"
                        f"\n{scp.hexdump(packet.encode(), dump=True)}\n"
                    )
                app.logger.error(err_msg)

    def __call__(self) -> Coroutine:
        return self._downlink_coro()


def make_uplink_handler(
    app: ipc.IpcAppHelper,
    xcvrs: Dict[Type[Transceiver], Transceiver]
) -> Type[ipc.SocketTopicHandlerAsync]:
    """Makes an uplink handler that listens for uplink packets and dispatches
    them to the appropriate Transceivers from the given Transceiver map `xcvrs`."""

    class UplinkSub(ipc.SocketTopicHandlerAsync['UplinkSub']):
        """Subscriber / task for uplinked packets."""
        _raise_on_unhandled_topics: ClassVar[bool] = False
        _require_unhandled_topic_handler: ClassVar[bool] = False
        # Decorator shorthand (also plays more nicely with syntax highlighting):
        topic_handler = \
            ipc.SocketTopicHandlerAsync['UplinkSub'].TopicHandlerFactory()

        def _dispatch_to_xcvr(
            self,
            xcvr: Transceiver,
            packet: Packet,
        ) -> bool:
            """Dispatches handling of the uplink of the given packet to the
            given Transceiver.

            Returns whether the Transceiver was able to uplink the packet
            successfully.
            """
            success = xcvr.send(packet)
            if success:
                app.logger.success(
                    f" \tTransceiver `{xcvr.name}` uplinked successfully."
                )
            else:
                app.logger.error(
                    f" \tTransceiver `{xcvr.name}` uplink failed."
                )
            return success

        @topic_handler
        async def uplink_handler(
            self,
            manager: ipc.IpcAppManagerAsync,
            payload: ipc.IpcPayload
        ) -> None:
            # Decode the message:
            msg = ipc.guard_msg(payload.message, UplinkPacketRequestMessage)
            packet = msg.content.packet

            # Print the packet and the request metadata:
            app.logger.notice(
                f"[UPLINK - {packet.pathway.name}]\t "
                f"({payload.subtopic_bytes!r}) \t "
                f"{msg.content.packet!s} "
                f"-> \t{msg.content.target_xcvr.name}"
            )

            target_xcvr_class = msg.content.target_xcvr.transceiver_class
            if target_xcvr_class is None:
                # Dispatch to all XCVRs:
                for xcvr in xcvrs.values():
                    self._dispatch_to_xcvr(xcvr, packet)
            elif target_xcvr_class in xcvrs:
                # Dispatch to matching XCVR:
                self._dispatch_to_xcvr(xcvrs[target_xcvr_class], packet)
            else:
                # No XCVRs match:
                app.logger.warning(
                    f"No matching Transceivers. Packet not uplinked. "
                    f"For archive purposes, packet contents:\n"
                    f"{packet!s}\n"
                    f"\n{scp.hexdump(packet.encode(), dump=True)}\n"
                )

        _topic_handlers: ClassVar[ipc.SocketTopicHandlerAsync.TopicHandlerRegistry] = {
            ipc.Topic.UL_PACKET: uplink_handler
        }

    return UplinkSub


def build_xcvrs(
    app: ipc.IpcAppHelper,
    opts
) -> Dict[Type[Transceiver], Transceiver]:
    # Load XCVR settings YAML (if given):
    xcvrs_settings: Dict[str, Dict[str, Any]] = dict()
    if opts.xcvr_yaml_file is not None and opts.xcvr_yaml_file != "":
        with open(opts.xcvr_args_yaml, 'r') as f:
            try:
                xcvrs_settings = yaml.safe_load(f)
            except Exception as e:
                raise ValueError(
                    f"Failed to load YAML Transceiver settings from "
                    f"`{opts.xcvr_args_yaml}` because: {e}."
                )
    if opts.xcvr_yaml_str is not None and opts.xcvr_yaml_str != "":
        # Parse YAML string:
        try:
            yaml_str_settings = yaml.safe_load(opts.xcvr_yaml_str)
        except Exception as e:
            raise ValueError(
                f"Failed to load YAML Transceiver settings from "
                f"`{opts.xcvr_args_yaml}` because: {e}."
            )
        # Merge string settings with any pre-existing settings (from file),
        # override when there's a conflict:
        for k, v in yaml_str_settings.items():
            if k not in xcvrs_settings:
                xcvrs_settings[k] = v
            else:
                xcvrs_settings[k] = {
                    **xcvrs_settings[k],
                    **v
                }

    # Make transceiver(s) specified by CLI Opts:
    xcvrLoggerLevel(opts.log_level)
    xcvrs: Dict[Type[Transceiver], Transceiver] = dict()
    for xcvr_name in opts.prebuilt_xcvr_name:
        settings = xcvrs_settings.get(xcvr_name, dict())
        app.logger.notice(
            f"Bringing up {xcvr_name} Transceiver with settings: {settings}."
        )
        xcvr = build_xcvr_by_name(
            xcvr_name,  **settings
        )
        xcvr.begin()
        xcvrs[xcvr.__class__] = xcvr
        app.logger.info(
            f"{xcvr_name} Transceiver Started."
        )

    return xcvrs


async def main(app: ipc.IpcAppHelper, opts) -> None:
    xcvrs = build_xcvrs(app, opts)

    # Make Manager and Tasks:
    UplinkSub = make_uplink_handler(app, xcvrs)
    manager = ipc.IpcAppManagerAsync(socket_specs={
        DOWNLINK_SOCK: ipc.SocketSpec(
            sock_type=ipc.SocketType.PUBLISHER,
            port=ipc.Port.TRANSCEIVER_PUB,
            topics=[ipc.Topic.DL_PACKETS],
            publish_only=True,
            bind=False  # proxy handles binding
        ),
        UPLINK_SOCK: ipc.SocketSpec(
            sock_type=ipc.SocketType.SUBSCRIBER,
            port=ipc.Port.TRANSCEIVER_SUB,
            topics=UplinkSub.TOPICS(),
            rx_handler=UplinkSub(),
            bind=False  # proxy handles binding
        )
    })
    manager.spawn_core_tasks()
    # Add parallel tasks / coroutines for transceiver downlink:
    manager.add_coros([
        DownlinkCoroutine(app, manager, xcvr)()
        for xcvr in xcvrs.values()
    ])

    # Run all tasks:
    await manager.run(return_when=asyncio.FIRST_COMPLETED)


if __name__ == "__main__":
    # Get CLI Options:
    opts = get_opts()

    # Load the latest DataStandards:
    IB3.init_from_latest()

    # Setup app:
    app_name = 'Transceiver_' + '_'.join(opts.prebuilt_xcvr_name)
    app = ipc.IpcAppHelper(app_name)
    app.setLogLevel(opts.log_level)

    # Run:
    asyncio.run(main(app, opts))
