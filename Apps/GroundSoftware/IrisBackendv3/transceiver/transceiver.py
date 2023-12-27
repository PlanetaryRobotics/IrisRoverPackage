# -*- coding: utf-8 -*-

"""
Abstract Base Class encapsulating common behaviors and interfaces for all
transceiver classes.

TODO: Handle setting `rover_ack` for `UplinkedPayload`s.
TODO: Handle multi-packet unpackings like `FileBlockPayload`s.
^ Do all this at the `Transceiver` level (not in subclasses) so all subclasses
benefit from the impl.

TODO: Handle auto-updating of seq-num (maybe this needs to work across
Transceivers?)

TODO: Add pcap-save function (writes every outbound bytes_packet to a pcap).

@author: Connor W. Colombo (CMU)
@last-updated: 03/03/2023
"""
from __future__ import annotations  # Activate postponed annotations (for using classes as return type in their own methods)

from typing import Optional, Awaitable, List, Type
from abc import ABC, abstractmethod, abstractclassmethod, abstractproperty
from datetime import datetime
import scapy.all as scp  # type: ignore # no type hints
import traceback
import asyncio
import atexit

from IrisBackendv3.transceiver.logs import logger
from IrisBackendv3.transceiver.endec import Endec, UnityEndec
from IrisBackendv3.transceiver.exceptions import TransceiverDecodingException, TransceiverConnectionException

from IrisBackendv3.codec.packet import parse_packet
from IrisBackendv3.codec.packet_classes.packet import Packet
from IrisBackendv3.codec.packet_classes.iris_common import IrisCommonPacket
from IrisBackendv3.codec.payload import Payload, UplinkedPayload, DownlinkedPayload
from IrisBackendv3.codec.payload_collection import EnhancedPayloadCollection
from IrisBackendv3.codec.metadata import DataPathway, DataSource, UplinkTimes, DownlinkTimes
from IrisBackendv3.codec.exceptions import PacketDecodingException


class Transceiver(ABC):
    """ Abstract base class for all `Transceivers`. This class handles all
    logic interfacing with `Packet` classes and `Endec`s so the implementation
    of the subclasses can be as simple as possible (just handling how to get
    and send packet bytes.)
    """
    # String name of this transceiver (for logging purposes):
    name: str
    # Endec used to encode and decode data using this transceiver:
    endecs: List[Endec]
    # How this data is transmitted received (wired/wireless):
    data_pathway: DataPathway
    # Where this data is transmitted received (yamcs, serial, etc.):
    data_source: DataSource
    # Sequence number tracking:
    seq_num: int = 0
    # Whether to log when a packet is successfully sent:
    log_on_send: bool
    # Whether to log when a packet is successfully received:
    log_on_receive: bool

    def __init__(
        self,
        name: str,
        endecs: Optional[List[Endec]] = None,
        pathway: DataPathway = DataPathway.NONE,
        source: DataSource = DataSource.NONE,
        log_on_send: bool = True,
        log_on_receive: bool = True,
        **_  # allow any kwargs to be passed in (i.e. via CLI)
    ) -> None:
        """
        Creates a `Transceiver` over `pathway` (wired/wireless) connecting to
        `source` (WIFI, Serial, YAMCS, PCAP, etc.).

        Data is en/decoded using `endecs`, a list of `Endec` instances in the
        order of **DECODING** (where the left-most endec is stripped off first
        when decoding and applied last when encoding.)
        """
        self.name = name
        # Initialize sequence number counter:
        self.seq_num = 0
        self.log_on_send = log_on_send
        self.log_on_receive = log_on_receive

        # If endecs given, make sure they're valid and use if so:
        if endecs is not None:
            if isinstance(endecs, list):
                if len(endecs) > 0:
                    self.endecs = endecs
                else:
                    logger.error(
                        f"When initializing `Transceiver` `endecs = {endecs}` "
                        f"was given as an empty list (len=0). The `endecs` "
                        "list must have non-zero length. "
                        "See `Transceiver.__init__` for more information."
                    )
                    endecs = None
            else:
                logger.error(
                    f"When initializing `Transceiver` `endecs = {endecs}` was "
                    f"given which has type {type(endecs)}. "
                    "`endecs` must be a list. "
                    "A `UnityEndec` was used instead. "
                    "See `Transceiver.__init__` for more information."
                )
                endecs = None

        # Just use `UnityEndec` if none (or no valid options) are given:
        if endecs is None:
            self.endecs = [UnityEndec()]

        # Store pathway and source for annotating all payloads:
        self.data_pathway = pathway
        self.data_source = source

        # Register to make sure `__del__` gets called when program exits:
        # (via interrupt or actual close)
        atexit.register(self.__del__)

    def __del__(self) -> None:
        """Override in subclasses to provide custom cleanup behavior.
        Transceiver base class handles registering this with `atexit`.
        """
        logger.notice(f"Cleaning up Transceiver {self.__class__.__name__}.")

    def begin(self) -> None:
        """Initialize any special registers, etc. for this transceiver."""
        # (Re)Initialize sequence number counter:
        self.seq_num = 0

    @abstractmethod
    async def _async_downlink_byte_packets(self) -> List[bytes]:
        """ 
        Asynchronously awaits the next available complete packet(s) of bytes on
        this `Transceiver`'s downlink transmission line.
        No decoding occurs with the `endec`, just transmits the packet of bytes
        as given.

        Returns a list of all packets received, as bytes.
        """
        raise NotImplementedError()

    def _downlink_byte_packets(self) -> List[bytes]:
        """ Synchronously (blocking) reads all complete packets of bytes on
        this `Transceiver`'s downlink transmission line.
        No decoding occurs with the `endec`, just transmits the packet of bytes
        as given.

        NOTE: The default implementation of this function just synchronously
        awaits the output of `_async_downlink_byte_packets` but this can be
        overridden if the synchronous and asynchronous interactions with this
        interface are truly unique.

        Returns a list of all packets received, as bytes.
        """
        # NOTE: This approach may not work if running from a Jupyter notebook
        # ... but, in that case, you can simply `await` the async version.
        return asyncio.get_event_loop().run_until_complete(
            self._async_downlink_byte_packets()
        )

    @abstractmethod
    def _uplink_byte_packet(
        self,
        packet_bytes: bytes,
        **uplink_metadata
    ) -> bool:
        """ Transmits the given packet of bytes on this `Transceiver`'s uplink
        transmission line.
        No encoding occurs with the `endecs`, just transmits the bytes as given.

        `**uplink_metadata` contains any special data needed by methods further
        down the uplink pipeline.

        Returns whether the uplink was successful.
        """
        raise NotImplementedError()

    def endecs_encode(self, data: bytes) -> bytes:
        """Encodes the given data using all the `endecs`."""
        # Encode raw data (add one layer at a time):
        for endec in self.endecs[::-1]:
            try:
                data = endec.encode(data)
            except Exception as e:
                logger.error(
                    f"While encoding using the `{endec.__class__.__name__}` "
                    "Endec, an exception occurred. The bytes fed to it will "
                    "just be passed through in order to preserve them (so they "
                    "make it out the other end of the `Transceiver` and into "
                    "`Storage`). "
                    f"The exception was: {e}"
                )
        return data

    def endecs_decode(self, data: bytes) -> bytes:
        """Decodes the given data using all the `endecs`."""
        # Decode raw data (strip off one at a time):
        for endec in self.endecs:
            try:
                data = endec.decode(data)
            except Exception as e:
                logger.error(
                    f"While decoding using the `{endec.__class__.__name__}` "
                    "Endec, an exception occurred. The bytes fed to it will "
                    "just be passed through in order to preserve them (so they "
                    "make it out the other end of the `Transceiver` and into "
                    "`Storage`). "
                    f"The exception was: {e}"
                )
        return data

    def _read_processor(self, byte_packets: List[bytes]) -> List[Packet]:
        """The processing that's common to both synchronous and asynchronous
        reads."""
        packets: List[Packet] = []

        try:
            for bp in byte_packets:
                try:
                    # Decode transmitted data to recover raw data:
                    bp = self.endecs_decode(bp)
                    # Parse packet (decode raw data):
                    packet = parse_packet(bp)
                    # Add metadata to packet (and all its payloads):
                    packet.pathway = self.data_pathway
                    packet.source = self.data_source
                    # Add metadata to all the payloads in Packet:
                    for payload in packet.payloads.all_payloads:
                        if isinstance(payload, DownlinkedPayload):
                            # Add Downlink metadata:
                            if payload.downlink_times is None:
                                payload.downlink_times = DownlinkTimes()
                            payload.downlink_times.pmcc_rx = datetime.now()
                        else:
                            # What's a non-DownlinkedPayload doing in here?:
                            logger.warning(  # type: ignore
                                f"[{self.__class__.__name__}] When processing "
                                f"a downlinked packet `{packet}`, a payload "
                                "was found that's **NOT** a "
                                f"`DownlinkedPayload`: `{payload}`. "
                                "Payload was still included in "
                                "the Packet's (Enhanced)PayloadCollection."
                            )

                    # Packet parsing complete (either successfully or
                    # unsuccessfully (yielded an `UnsupportedPacket`)).
                    # Keep the packet:
                    packets.append(packet)
                    # Print the packet into the logs:
                    if self.log_on_receive:
                        logger.info(packet)
                except PacketDecodingException as pde:
                    logger.error(
                        "While processing downlinked data, a "
                        "`PacketDecodingException` occurred. "
                        "The data bytes at the time of the exception were: \n"
                        f"{scp.hexdump(bp, dump=True)}\n."
                        f"The PacketDecodingException was: `{pde}`."
                    )
                except TransceiverDecodingException as tde:
                    logger.error(
                        "While processing downlinked data, a "
                        "`TransceiverDecodingException` occurred. "
                        "The data bytes at the time of the exception were: \n"
                        f"{scp.hexdump(bp, dump=True)}\n."
                        f"The TransceiverDecodingException was: `{tde}`."
                    )

        except TransceiverConnectionException as tce:
            # Log it:
            trace = '\n'.join(traceback.format_tb(tce.__traceback__))
            logger.error(
                f"While attempting to read bytes from Transceiver {self}, "
                f"a TransceiverConnectionException occurred: `{tce}`. "
                f"The trace was:\n{trace}."
            )
            # Then re-raise to get out of here:
            raise tce

        except Exception as e:
            trace = '\n'.join(traceback.format_tb(e.__traceback__))
            logger.error(
                "An otherwise unresolved error occurred during packet "
                f"processing: {e}. "
                f"The trace was:\n{trace}."
            )

        # Returns all packets successfully retrieved:
        return packets

    async def async_read(self) -> List[Packet]:
        """Asynchronously awaits the next available packet(s) on the transceiver input."""
        try:
            byte_packets = await self._async_downlink_byte_packets()
        except TransceiverConnectionException as tce:
            # Log it:
            trace = '\n'.join(traceback.format_tb(tce.__traceback__))
            logger.error(
                f"While attempting to read bytes from Transceiver {self}, "
                f"a TransceiverConnectionException occurred: `{tce}`. "
                f"The trace was:\n{trace}."
            )
            # Then re-raise to get out of here:
            raise tce
        except Exception as e:
            trace = '\n'.join(traceback.format_tb(e.__traceback__))
            logger.error(
                "An otherwise unresolved error occurred during packet "
                f"streaming: {e}. "
                f"The trace was:\n{trace}."
            )
            # Nothing to process, just return empty:
            return []

        return self._read_processor(byte_packets)

    def read(self) -> List[Packet]:
        """Reads all available packets on the transceiver input."""
        try:
            byte_packets = self._downlink_byte_packets()
        except TransceiverConnectionException as tce:
            # Log it:
            trace = '\n'.join(traceback.format_tb(tce.__traceback__))
            logger.error(
                f"While attempting to read bytes from Transceiver {self}, "
                f"a TransceiverConnectionException occurred: `{tce}`. "
                f"The trace was:\n{trace}."
            )
            # Then re-raise to get out of here:
            raise tce
        except Exception as e:
            trace = '\n'.join(traceback.format_tb(e.__traceback__))
            logger.error(
                "An otherwise unresolved error occurred during packet "
                f"streaming: {e}. "
                f"The trace was:\n{trace}."
            )
            # Nothing to process, just return empty:
            return []

        return self._read_processor(byte_packets)

    def send(self, packet: Packet, **uplink_metadata) -> bool:
        """ Sends the given packet over this `Transceiver`'s transmission line,
        encoding it as necessary.

        `**uplink_metadata` contains any special data needed by methods further
        down the uplink pipeline, particularly `_uplink_byte_packet`.

        Returns whether the send was successful.
        """
        # See if a pathway is specified for this packet already.
        # If so, bail if it doesn't match this transceiver's pathway:
        if (
            packet.pathway != DataPathway.NONE
            and self.data_pathway != DataPathway.NONE
            and packet.pathway != self.data_pathway
        ):
            logger.warning(
                f"`{self.__class__.__name__} received but is **NOT** "
                f"uplinking packet {packet} b/c `{packet.pathway=}`, "
                f"which isn't compatible with this transceiver's "
                f"`data_pathway={self.data_pathway}`."
            )
            return False

        # Add metadata to all the payloads in Packet first (before uplink):
        for payload in packet.payloads.all_payloads:
            payload.pathway = self.data_pathway
            if isinstance(payload, UplinkedPayload):
                # Add Uplink metadata:
                if payload.uplink_times is None:
                    payload.uplink_times = UplinkTimes()
                payload.uplink_times.pmcc_tx = datetime.now()
            else:
                # What's a non-UplinkedPayload doing in here?:
                logger.warning(  # type: ignore
                    f"[{self.__class__.__name__}] When uplinking packet "
                    f"`{packet}`, a payload was found that's **NOT** an "
                    f"`UplinkedPayload`: `{payload}`. "
                    "Payload was still uplinked."
                )

        # Generate raw data:
        packet_bytes = packet.encode()

        # Encode raw data for transmission:
        packet_bytes = self.endecs_encode(packet_bytes)

        # Uplink encoded data:
        success = self._uplink_byte_packet(packet_bytes, **uplink_metadata)

        if success and self.log_on_send:
            logger.info(
                f"`{self.__class__.__name__}` sent: \n"
                f" \t{packet!s} \n"
                f"{scp.hexdump(packet_bytes, dump=True)} \n\n"
            )

        return success

    def send_payloads(
        self,
        payloads: EnhancedPayloadCollection,
        **uplink_metadata
    ) -> List[Packet]:
        """ Sends the given payloads in as few packets as possible.

        `**uplink_metadata` contains any special data needed by methods further
        down the uplink pipeline, particularly `_uplink_byte_packet`.

        TODO: Mind the MTUs and actually handle splitting up the `Payload`s
        into multiple `Packet`s as necessary.

        Returns: A list of all `Packet`s transmitted.
        """
        # Log the payloads received:
        logger.info(  # type: ignore
            f"[{self.__class__.__name__}] Attempting to packetize and send "
            f"payloads: `{payloads}`."
        )

        packets_sent: List[Packet] = []
        # Build packet:
        packet = IrisCommonPacket(
            seq_num=self.seq_num,
            payloads=payloads
        )

        # Send!:
        success = self.send(packet, **uplink_metadata)
        if success:
            packets_sent.append(packet)
            self.seq_num += 1

        return packets_sent
