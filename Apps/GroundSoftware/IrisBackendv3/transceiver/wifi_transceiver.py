"""
Defines the Transceiver class for interfacing with the rover over Wifi.

By default this side (Ground) acts as the lander and connects to a gateway on
the PM1LWAP hotspot.

Includes any supporting functions necessary for maintaining wifi connection.

@author: Connor W. Colombo (CMU)
@last-updated: 06/01/2023
"""
from __future__ import annotations

from typing import Any, Optional, Callable, Dict, Deque, List, Union, Type, cast, Tuple, TypeAlias, TypeGuard

import socket
import asyncio
import threading
import socketserver
import scapy.all as scp  # type: ignore
from time import time
import logging
from collections import OrderedDict

from IrisBackendv3.transceiver.transceiver import Transceiver
from IrisBackendv3.transceiver.endec import Endec, SlipEndec
from IrisBackendv3.transceiver.logs import logger
from IrisBackendv3.transceiver.exceptions import TransceiverConnectionException, TransceiverDecodingException

from IrisBackendv3.codec.packet_classes.packet import Packet
from IrisBackendv3.codec.packet_classes.iris_common import IrisCommonPacket
from IrisBackendv3.codec.payload_collection import EnhancedPayloadCollection
from IrisBackendv3.codec.metadata import DataPathway, DataSource
from IrisBackendv3.utils.basic import type_guard_argument

from .utils.asyncio_udp import _AddrType, Endpoint, open_local_endpoint

IP: TypeAlias = Tuple[int, int, int, int]


def coerce_ip(ip: Any) -> IP:
    """Converts the given input to an IP type if possible.
    Raises TypeError if not."""
    if isinstance(ip, str):
        ip = ip.split('.')
    if not isinstance(ip, (list, tuple)):
        raise TypeError(
            f"IP needs to be list or tuple "
            f"(or string able to be split on '.')."
            f"Got `{ip=}` with `{type(ip)=}`."
        )
    ip = list(ip)  # list not tuple for now so we can coerce values
    if len(ip) != 4:
        raise TypeError(f"IP needs to be length-4 tuple. Got `{ip=}`.")
    for i, x in enumerate(ip):
        # Coerce types if necessary:
        if not isinstance(x, int):
            try:
                ip[i] = int(x)
            except:
                raise TypeError(
                    f"IP needs to be length-4 tuple of ints (or values that "
                    f"can become ints). "
                    f"Got `{ip}` with types: `{[type(xx) for xx in ip]}`."
                )

    return cast(IP, tuple(ip))


def ip_to_str(ip: IP) -> str:
    return '.'.join(str(x) for x in ip)


def coerce_addr(addr: _AddrType) -> Tuple[IP, int]:
    """Standardizes format of an address from UDP layer."""
    host, port = addr
    return coerce_ip(host), int(port)


class WifiTransceiver(Transceiver):
    rover_ip: IP
    rover_port: int
    gateway_ip: IP
    lander_ip: IP
    lander_port: int

    # UDP Datagram handlers:
    # Server (handles receiving data from a remote target - the Rover):
    server_endpoint: Endpoint | None
    # How big the buffer queue for inbound packets should be:
    server_queue_size: int
    # Whether or not to force sending data to the gateway (normally `False`).
    # See `_uplink_byte_packet` for more details.
    gateway_send_force: bool

    def __init__(
        self,
        rover_ip: IP | str = (192, 168, 150, 3),
        rover_port: int = 42000,
        gateway_ip: IP | str = (192, 168, 150, 254),
        lander_ip: IP | str = (192, 168, 10, 105),
        lander_port: int = 43531,
        inbound_queue_size: int = 100,
        gateway_send_force: bool = False,
        endecs: Optional[List[Endec]] = None,
        pathway: DataPathway = DataPathway.WIRELESS,
        source: DataSource = DataSource.WIFI_CONNECTION_DIRECT,
        **kwargs
    ) -> None:
        """ Initializes a `WifiTransceiver`.

        NOTE: You'll need to supply the appropriate list of `endecs` which were
        used to encode the data that was put into the PCAP.
        """
        full_kwargs: Dict[str, Any] = {
            'name': 'wifi',  # allow this to be overridden in kwargs
            **kwargs  # fwd all other kwargs to parent
        }
        super().__init__(
            endecs=endecs,
            pathway=pathway,
            source=source,
            **full_kwargs
        )

        # Validate inputs:
        self.rover_ip = coerce_ip(rover_ip)
        self.rover_port = type_guard_argument(
            int, 'rover_port', rover_port,
            calling_function_name='WifiTransceiver.__init__'
        )
        self.gateway_ip = coerce_ip(gateway_ip)
        self.lander_ip = coerce_ip(lander_ip)
        self.lander_port = type_guard_argument(
            int, 'lander_port', lander_port,
            calling_function_name='WifiTransceiver.__init__'
        )

        # Init State:
        self.server_endpoint = None
        self.server_queue_size = inbound_queue_size
        self.gateway_send_force = gateway_send_force

    def __del__(self) -> None:
        """Override in subclasses to provide custom cleanup behavior.
        Transceiver base class handles registering this with `atexit`.
        """
        logger.notice(
            f"Cleaning up WifiTransceiver . . ."
        )

        if self.server_endpoint is not None:
            logger.info(f"\t Closing Server Endpoint . . .")
            self.server_endpoint.close()

        # Call super class too:
        Transceiver.__del__(self)

    def begin(self) -> None:
        """ Initialize any special registers, etc. for this transceiver.
        Can also be used to reset the state of the transceiver.
        """
        super().begin()

        logger.notice("Beginning `WifiTransceiver`. ")

        # Nothing special to actually do here since we perform endpoint setups
        # asynchronously in the appropriate async handlers.

    async def start_server(self) -> None:
        """Starts the UDP Server Endpoint for incoming processing incoming 
        (downlinked) packets.

        NOTE `reuse_address=False` to ensure a default is set since having it
        not False is generally an antipattern for UDP and presents a security
        risk on Linux per:
        https://docs.python.org/3/library/asyncio-eventloop.html#asyncio.loop.create_datagram_endpoint
        """
        logger.notice("Starting inbound WiFi server and binding . . .")
        try:
            self.server_endpoint = await open_local_endpoint(
                host=ip_to_str(self.lander_ip),
                port=self.lander_port,
                logger=logger,
                queue_size=self.server_queue_size,
                reuse_address=False
            )
        except Exception as e:
            raise TransceiverConnectionException(
                "WifiTransceiver failed to connect. Likely a binding problem. "
                "Are you on the right network with the right IP? "
                "Is someone else connected as the lander on this ip and port? "
                f"`{self.lander_ip=}`, `{self.lander_port=}`. "
                f"Root exception is: `{e}`."
            )
        logger.success(
            "Inbound WiFi server started successfully on "
            f"{ip_to_str(self.lander_ip)}:{self.lander_port}!"
        )

    async def _async_downlink_byte_packets(self) -> List[bytes]:
        """ Asynchronously reads the next set of raw packet_bytes from
        `packet_bytes_list` (if enough time has passed).

        Returns a list of all packet_bytes that should have been "received"
            since the last time this function returned a packet. If not enough
            time has elapsed yet, this list is empty.
        """
        if self.server_endpoint is None:
            await self.start_server()
        # `server_endpoint` is not None now if we got here:
        server = cast(Endpoint, self.server_endpoint)

        # Wait for new data:
        data_bytes, addr = await server.receive()
        src_ip, src_port = coerce_addr(addr)

        # Make sure src IP is the rover or the gateway:
        if src_ip not in (self.rover_ip, self.gateway_ip):
            logger.warning(
                f"Got bad datagram IP in WiFi UDP packet: {src_ip}"
            )

        # Make sure src port is the rover or lander reflected by the gateway:
        if (src_port not in [
            self.rover_port,
            self.rover_port+1,  # A special testing case
            self.lander_port
        ]):
            logger.warning(
                f"Got bad datagram port in WiFi UDP packet: {src_port}"
            )

        return [data_bytes]

    def _uplink_byte_packet(self, packet_bytes: bytes, **_) -> bool:
        """Transmits the given packet of bytes on this `Transceiver`'s uplink
        transmission line.

        Sends an UDP packet to the rover over wifi (INET really).
        Only works if sending from a machine (NIC) bound to the lander IP while
        a separate machine (NIC), bound to the gateway ip, is hosting the
        network which the rover is also connected to.

        If `self.gateway_send_force`, this forces it to go through (use this
        only if sending from the gateway but you need it to look like its
        coming from the lander). You likely will need to run as sudo for this
        feature to work.

        NOTE: `**uplink_metadata` is given in the superclass and contains any
        special data needed by methods further down the uplink pipeline, but is
        unused by `PcapTransceiver` since it doesn't even have a meaningful
        uplink.
        """

        dest_ip, dest_port = ip_to_str(self.rover_ip), self.rover_port
        dest_addr = (dest_ip, dest_port)

        if not self.gateway_send_force:
            # Main way of doing this if connected to network on separate
            # machine with lander address:
            if self.server_endpoint is None:
                logger.error(
                    "WiFi transceiver can't send packets yet. "
                    "Server endpoint not yet set up."
                )
                return False
            self.server_endpoint.send(packet_bytes, dest_addr)
        else:
            # Craft UDP packet (incl. using the correct src_ip, even if that's not our own):
            full_packet = scp.IP(dst=dest_ip, src=self.lander_ip) / \
                scp.UDP(sport=self.lander_port, dport=dest_port) / \
                scp.Raw(load=packet_bytes)
            full_packet_data = scp.raw(full_packet)

            # Open raw socket and send:
            try:
                sock = socket.socket(
                    socket.AF_INET, socket.SOCK_RAW, socket.IPPROTO_RAW
                )
                # Tell kernel not to add headers (we took care of that above):
                sock.setsockopt(socket.IPPROTO_IP, socket.IP_HDRINCL, 1)
                sock.sendto(full_packet_data, dest_addr)
                sock.close()
            except PermissionError as pe:
                logger.error(
                    f"Failed to open socket for sending WiFi packets using "
                    f"`gateway_send_force` due to PermissionError: `{pe}`. \n"
                    f"You should be running this as `sudo` - are you?"
                )

        return True
