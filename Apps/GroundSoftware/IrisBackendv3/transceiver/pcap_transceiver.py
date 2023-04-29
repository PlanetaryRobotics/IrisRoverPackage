"""
Defines the Transceiver class for reading telemetry from a pcap. Main use case
is replaying logs from previous testing.

Includes any supporting functions necessary for maintaining serial connection.

@author: Connor W. Colombo (CMU)
@last-updated: 03/11/2023
"""
from typing import Any, Optional, Callable, Dict, Deque, List, Union, Type, cast

import attr
import asyncio
import scapy.all as scp  # type: ignore
from time import time
import logging
from collections import OrderedDict

from IrisBackendv3.transceiver.transceiver import Transceiver
from IrisBackendv3.transceiver.endec import Endec, SlipEndec
from IrisBackendv3.transceiver.logging import logger
from IrisBackendv3.transceiver.exceptions import TransceiverConnectionException, TransceiverDecodingException

from IrisBackendv3.codec.packet_classes.packet import Packet
from IrisBackendv3.codec.packet_classes.iris_common import IrisCommonPacket
from IrisBackendv3.codec.payload_collection import EnhancedPayloadCollection
from IrisBackendv3.codec.metadata import DataPathway, DataSource
from IrisBackendv3.utils.basic import type_guard_argument


@attr.s(auto_attribs=True)
class PcapParseOpts:
    pcap_file: str = './test-data/Iris_210503_18_hours_of_telem.pcap'
    # port to filter packets with (only select packets sent to here):
    filter_port: Union[str, int] = 'any'
    # protocol to filter packets with (only select packets with this protocol - e.g. `scp.UDP`):
    filter_protocol: Optional[Any] = None
    # Exclude any packets containing these layers:
    exclude_packets_with_layers: Optional[List[Any]] = [scp.ICMP]
    packetgap: int = 0  # 36000
    deadspace: int = 0
    logging_level: str = 'INFO'


def load_pcap(opts: PcapParseOpts) -> List[bytes]:
    """
    Extracts any valid packets from the pcap/pcapng file specified in
    `pcap_file` specified in `opts` using the settings given in `opts`.

    Returns a list of the raw **unparsed** bytes of all (scapy) packets in
    the pcap.
    """
    extracted_packet_bytes: List[bytes] = []

    # Packet counts at various stages for diagnostics:
    diag_packet_counts: OrderedDict[str, int] = OrderedDict()

    # if debugging_mode := True:
    #     cprint("WARNING. ARGS ARE BEING MODIFIED BECAUSE DEBUG MODE IS ACTIVE", 'red')
    #     opts.pcap_file = './test-data/Iris_Image_Downlink_201223-ImageDLTest_FixedLens.pcapng'
    #     opts.port = 8080
    #     opts.cli_log = True

    logger.notice(  # type: ignore
        f"Parsing pcap file at {opts.pcap_file}, "
        f"looking for packets on port {opts.filter_port} "
        f"{f'with packet using protocol {opts.filter_protocol}' if opts.filter_protocol else ''}"
        ".\nSubsequent parser status updates will be logged at "
        f"`{opts.logging_level}` level."
    )

    log_level = logging.getLevelName(opts.logging_level.upper())

    # Data Formatting Settings:
    logger.log(
        log_level,
        "\t > Opening pcap . . ."
    )
    pcap = scp.rdpcap(opts.pcap_file)
    diag_packet_counts['Packets in PCAP'] = len(pcap)
    logger.log(
        log_level,
        f"\t > Found {len(pcap)} packets in pcap."
    )

    if opts.filter_protocol is not None:
        pcap_usable = pcap[opts.filter_protocol]
    else:
        pcap_usable = pcap
    diag_packet_counts['Matching protocol filter'] = len(pcap_usable)

    pcap_packets = list(pcap_usable[opts.packetgap:])
    diag_packet_counts['After applying packetgap'] = len(pcap_packets)

    # Exclude any packets containing any of the exclusion layers:
    pcap_packets = [
        p for p in pcap_packets
        if len(set(opts.exclude_packets_with_layers) & set(p.layers())) == 0
    ]
    diag_packet_counts['Excluding packets banned layers'] = len(pcap_packets)

    if isinstance(opts.filter_port, int):
        pcap_packets = list(
            filter(lambda x: x.dport == opts.filter_port, pcap_packets)
        )
    diag_packet_counts['After applying port filter'] = len(pcap_packets)

    logger.log(
        log_level,
        f"\t > Found {len(pcap_packets)} packets matching filter criteria."
    )
    diag_packet_counts['Matching all filter criteria'] = len(pcap_packets)

    if len(pcap_packets) == 0:
        n_packets_str = '\n'.join(
            f'\t\t>> {k}:\t {v}' for k, v in diag_packet_counts.items()
        )
        logger.error(
            "\t > Found NO packets matching filter criteria. "
            f"Packet counts at each stage:\n{n_packets_str}."
        )

    # Parse packets in pcap:
    logger.log(
        log_level,
        "\t > Extracting packets . . ."
    )
    extract_start: float = time()
    empty_packet_count: int = 0
    failed_packet_count: int = 0
    for i, pcap_packet in enumerate(pcap_packets):
        try:
            if pcap_packet.getlayer(scp.Raw) is None:
                # Skip this packet if it doesn't contain any Raw data (example
                # ARP that made it through the filter). Safety.
                empty_packet_count += 1
                continue
            packet_bytes = scp.raw(pcap_packet.getlayer(scp.Raw))[
                opts.deadspace:]
        except Exception as e:
            logger.warning(
                f"Failed to parse {pcap_packet} "
                f"with data:\n{scp.hexdump(pcap_packet)}\n"
                f"because:\n{e!s}"
            )
            packet_bytes = b''
        if not i % 100:
            logger.log(
                log_level,
                f'\t\t > Loaded {i+1:5.0f}/{len(pcap_packets):5.0f} packets.'
            )
        if len(packet_bytes) == 0:
            failed_packet_count += 1
        else:
            # Store:
            extracted_packet_bytes.append(packet_bytes)

    # Print summary of the results:
    extract_time = time() - extract_start

    logger.log(
        log_level,
        f"In {extract_time:.3f}s: "
        f"Successfully loaded {len(extracted_packet_bytes)} raw packets "
        f"out of {len(pcap)} total packets in the pcap. "
        f"Of the used packets, a total of {failed_packet_count} were "
        f"unloadable and {empty_packet_count} were empty (no `Raw` layer)."
    )

    # Return results:
    return extracted_packet_bytes


class PcapTransceiver(Transceiver):
    # Fixed time period between packets being revealed [ms]:
    fixed_period_ms: float
    # Whether the pcap should be looped (when the end is reached, set the head
    # back to the start):
    loop: bool

    # Options used to parse the pcap of packet_bytes:
    _pcap_opts: PcapParseOpts
    # List of all the raw packet_bytes extracted from the pcap:
    packet_bytes_list: List[bytes]
    # Head (index of where we are in the list of all packet_bytes):
    _head: int
    # The last time a packet was downlinked [in seconds] (from `time()`):
    _last_downlink_time_s: float

    def restart(self) -> None:
        """Sets the head back to the beginning of the `packet_bytes` list."""
        self._head = 0

    def __init__(
        self,
        pcap_opts: PcapParseOpts,
        fixed_period_ms: float,
        loop: bool = False,
        endecs: Optional[List[Endec]] = None,
        pathway: DataPathway = DataPathway.WIRED,
        source: DataSource = DataSource.PCAP,
        **kwargs
    ) -> None:
        """ Initializes a `PcapTransceiver`.

        NOTE: You'll need to supply the appropriate list of `endecs` which were
        used to encode the data that was put into the PCAP.
        """
        super().__init__(
            endecs=endecs,
            pathway=pathway,
            source=source,
            **kwargs  # fwd all other kwargs to parent
        )

        # Validate inputs:
        self.fixed_period_ms = cast(float, type_guard_argument(
            float, 'fixed_period_ms', fixed_period_ms,
            calling_function_name='PcapTransceiver.__init__'
        ))
        self.loop = cast(bool, type_guard_argument(
            bool, 'loop', loop,
            calling_function_name='PcapTransceiver.__init__'
        ))

        self._pcap_opts = pcap_opts

        # Initialize data:
        self.packet_bytes_list = []
        self._head = 0

    def begin(self) -> None:
        """ Initialize any special registers, etc. for this transceiver.
        Can also be used to reset the state of the transceiver.
        """
        super().begin()

        # Parse the file and store the data:
        logger.notice(  # type: ignore
            "Beginning `PcapTransceiver`. "
            f"Loading pcap file with: {self._pcap_opts} . . ."
        )
        # (re)set the head:
        self._head = 0

        # Load the packet_bytes_list from the pcap:
        self.packet_bytes_list = load_pcap(self._pcap_opts)

        # Only **after** the pcap is loaded...
        # reset the timer so that the amount of time until the first packet is
        # the expected value of a random start with packets being downlinked at
        # a fixed interval (half the mean period):
        self._last_downlink_time_s = time() - self.fixed_period_ms / 1000.0

    async def _async_downlink_byte_packets(self) -> List[bytes]:
        """ Asynchronously treads the next set of raw packet_bytes from
        `packet_bytes_list` (if enough time has passed).

        Returns a list of all packet_bytes that should have been "received"
            since the last time this function returned a packet. If not enough
            time has elapsed yet, this list is empty.
        """
        _byte_packets: List[bytes] = []

        if self._head < len(self.packet_bytes_list):
            # How much time has elapsed since the last return of a packet:
            now = time()  # for optimal timing, only grab `now` once.
            Dt_ms = (now - self._last_downlink_time_s) * 1000
            # Time until next packet needs to be emitted:
            time_to_next_packet_ms = max(self.fixed_period_ms - Dt_ms, 0)

            # if the time to emit a packet hasn't passed yet, wait:
            if time_to_next_packet_ms > 0:
                # wait until this period ends:
                await asyncio.sleep(time_to_next_packet_ms/1000)
                # Update Dt:
                now = time()
                Dt_ms = (now - self._last_downlink_time_s) * 1000

            # Determine how many full time periods have elapsed:
            n_periods = int(Dt_ms / self.fixed_period_ms)

            if n_periods >= 1:
                # if more than one period has elapsed, we can downlink.
                # Grab as many packets as t he number of periods that have
                # elapsed (or up to the end of the list):
                end_idx = self._head + n_periods
                _byte_packets = self.packet_bytes_list[self._head:end_idx]
                self._head = end_idx  # update the `_head`
                self._last_downlink_time_s = now  # update the last grab time

        else:
            logger.verbose(  # type: ignore
                "`PcapTransceiver` has reached the end of the pcap loaded from "
                f"`{self._pcap_opts.pcap_file}`."
            )
            if self.loop:
                logger.verbose("Looping the pcap file")  # type: ignore
                self.restart()  # set the head back to 0
                # try again:
                _byte_packets = await self._async_downlink_byte_packets()

        return _byte_packets

    def _uplink_byte_packet(self, packet_bytes: bytes, **_) -> bool:
        """ This would transmit the given packet of bytes on this
        `Transceiver`'s uplink transmission line. **HOWEVER** this is a
        recording. You can't send commands to a recording. So this will just
        toss the bytes.

        NOTE: `**uplink_metadata` is given in the superclass and contains any
        special data needed by methods further down the uplink pipeline, but is
        unused by `PcapTransceiver` since it doesn't even have a meaningful
        uplink.
        """
        logger.debug(  # type: ignore
            "`PcapTransceiver._uplink_byte_packet` was called. "
            "This `Transceiver` replays a recording, so you can't send using "
            f"it. The data will be thrown out. Data was: `{packet_bytes!r}`."
        )

        # Just return `True`. It didn't fail and it's likely this function is
        # being called as part of a pipeline for a reason so, let's no break
        # that pipeline.
        return True
