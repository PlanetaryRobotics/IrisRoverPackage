"""
Simple enumeration / registry of all available Transceiver classes (allows
specifying which transceiver should handle something).

@author: Connor W. Colombo (CMU)
@last-updated: 03/06/2023
"""
from enum import Enum
from typing import Type

from .transceiver import Transceiver
from .pcap_transceiver import PcapTransceiver
from .slip_transceiver import SlipTransceiver
from .wifi_transceiver import WifiTransceiver
from .yamcs_transceiver import YamcsTransceiver


class TransceiverEnum(Enum):
    """
    Enum for defines all ports used in the IPC.

    This enum should serve as the single source of truth for the topic 
    information.
    """

    PCAP = PcapTransceiver
    SERIAL_SLIP = SlipTransceiver
    WIFI = WifiTransceiver
    YAMCS = YamcsTransceiver
    # Packets can be uplinked by ANY/ALL Transceivers
    # (XCVRs will make their own decisions based on packet contents, e.g.
    # Pathway)
    ALL = None

    # Definition of the topic:
    transceiver_class: Type[Transceiver] | None

    def __new__(cls, val: Type[Transceiver] | None):
        """Constructs a new instance of the Enum."""
        obj = object.__new__(cls)
        obj._value_ = val
        obj.transceiver_class = val
        return obj

    def matches(self, xcvr: Transceiver) -> bool:
        """Checks if the given transceiver matches the one specified by enum
        instance."""
        if self.transceiver_class is None:
            return True  # All are allowed.
        return isinstance(xcvr, self.transceiver_class)
