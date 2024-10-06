from IrisBackendv3.transceiver.endec import Endec
from IrisBackendv3.transceiver.logs import logger

from typing import cast
import scapy.all as scp  # type: ignore # no type hints


class IpUdpEndec(Endec):
    """Encoder applies a UDP header and then an IP header, decoder does reverse."""
    ip_dest: str  # ex. '127.0.0.2'
    ip_src: str
    port: int  # ex. 8080

    def __init__(
        self,
        ip_dest: str = '127.0.0.2',
        ip_src: str = '222.173.190.239',
        port: int = 8080
    ) -> None:
        # NOTE: The ip/port is arbitrary (this is only used for WD, which
        # doesn't care):
        self.ip_dest = ip_dest
        self.ip_src = ip_src
        self.port = port

    def encode(self, data: bytes) -> bytes:
        """ Encodes data to send over a `Transceiver`.
        """
        # Build packet
        full_packet = (
            scp.IP(dst=self.ip_dest, src=self.ip_src)
            / scp.UDP(dport=self.port)
            / scp.Raw(load=data)
        )
        return cast(bytes, scp.raw(full_packet))

    def decode(self, data: bytes) -> bytes:
        """ Decodes data sent over a `Transceiver`.
        """
        # Convert into a full scapy packet:
        full_packet = scp.IP(data)
        no_ip = full_packet.payload  # scrape off IP
        if no_ip.name.upper() != 'UDP':
            logger.warning(
                "In `IpUdpEndec.decode`, after stripping off the IP header, a "
                f"'UDP' header was expected but instead {no_ip.name} was "
                "found. "
                "Packet bytes fed into the `IpUdpEndec` decoder were: "
                f"`{scp.hexstr(data)}`."
                "GSW will attempt to strip off the layer after the IP layer "
                "anyway and continue packet parsing."
            )
        no_udp = no_ip.payload
        return cast(bytes, scp.raw(no_udp))
