# -*- coding: utf-8 -*-

"""
Classes to encoders/decoders (`Endec`s) to convert bytes for `Transceiver`
messages.

Using instantiatable classes and not namespaces or singletons since some
`Endec`s may be need to be stateful.

NOTE: Technically speaking, there's no difference between an `endec` and a
`codec` but the name `Endec` is used here to avoid confusion with the
`CodecContainers` in the `codec` package used to encode/decode Iris Packets
themselves. These `Endec`s are just used for wrapping and unwrapping data sent
through a `Transceiver`.

NOTE: All `Endec`s expect there to be **exactly one** packet's worth of
information in the supplied data.

@author: Connor W. Colombo (CMU)
@last-updated: 05/12/2022
"""

from abc import ABC, abstractmethod
from typing import cast, ClassVar

import re
import scapy.all as scp  # type: ignore # no type hints

from .logging import logger
from .exceptions import TransceiverDecodingException


class Endec(ABC):
    """
    Abstract Base Class for encoder/decoder to convert bytes for `Transceiver`.
    messages.
    """
    @abstractmethod
    def encode(self, data: bytes) -> bytes:
        raise NotImplementedError()

    @abstractmethod
    def decode(self, data: bytes) -> bytes:
        raise NotImplementedError()


class UnityEndec(Endec):
    def encode(self, data: bytes) -> bytes:
        """ Encodes data to send over a `Transceiver`.
        Default is a unity encoder (just returns the bytes).
        """
        return data

    def decode(self, data: bytes) -> bytes:
        """ Decodes data sent over a `Transceiver`.
        Default is a unity decoder (just returns the bytes).
        """
        return data


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
            logger.warn(
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


class SlipEndec(Endec):
    """ Implementation of SLIP (Serial Line Internet Protocol) per RFC-1055,
    based on the implementation in `sliplib 0.6.2` by Ruud de Jong.

    NOTE: Although this class is used internally in the `SlipTransceiver` class,
    you **don't**  need to (and shouldn't) include `SlipEndec` in the `endecs`
    list.
    """
    # Special SLIP encoding characters:
    B_END: ClassVar[bytes] = b'\xc0'
    B_ESC: ClassVar[bytes] = b'\xdb'
    B_ESC_END: ClassVar[bytes] = b'\xdc'
    B_ESC_ESC: ClassVar[bytes] = b'\xdd'

    def is_valid(self, data: bytes) -> bool:
        """ Returns whether the packet's contents conform to the SLIP spec.
        A packet is valid if:
        * It contains no `B_END` bytes other than leading and/or trailing `B_END` bytes
        * Each `B_ESC` byte is followed by either an `B_ESC_END` or an `B_ESC_ESC` byte.
        """
        data = data.strip(self.B_END)
        return not (
            self.B_END in data
            or data.endswith(self.B_ESC)
            or re.search(self.B_ESC + b'[^' + self.B_ESC_END + self.B_ESC_ESC + b']', data)
        )

    def encode(self, data: bytes) -> bytes:
        """ Encodes data to send over a `Transceiver`.

        Wraps the given data in SLIP.
        """
        return (
            self.B_END
            + (data.replace(self.B_ESC, self.B_ESC + self.B_ESC_ESC)
               .replace(self.B_END, self.B_ESC + self.B_ESC_END))
            + self.B_END
        )

    def decode(self, data: bytes) -> bytes:
        """ Decodes data sent over a `Transceiver`.
        """
        if not self.is_valid(data):
            raise TransceiverDecodingException(data, "Invalid SLIP packet.")
        return (
            data.strip(self.B_END)
            .replace(self.B_ESC + self.B_ESC_END, self.B_END)
            .replace(self.B_ESC + self.B_ESC_ESC, self.B_ESC)
        )
