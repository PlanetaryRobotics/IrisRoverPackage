"""
Various Classes defining Metadata for Packets and Payloads.

@author: Connor W. Colombo (CMU)
@last-updated: 02/27/2021
"""

from typing import Optional, List

from enum import Enum
from datetime import datetime

import attr


class DataPathway(Enum):
    """
    Enumeration of all pathways through which data can be sent to/from the rover.

    As usual, for backwards compatibility and data preservation, *never* change 
    any of the enum values or delete entries, just deprecate old ones.
    """
    NONE = -1, ''
    WIRED = 0x00, '_rs422'
    WIRELESS = 0x01, '_wlan'

    # Suffix applied to command names when sent to YAMCS to take this path:
    yamcs_suffix: str

    def __new__(cls, val: int, yamcs_suffix: str):
        """Constructs a new instance of the Enum."""
        obj = object.__new__(cls)
        obj._value_ = val
        obj.yamcs_suffix = yamcs_suffix
        return obj


class DataSource(Enum):
    """
    Enumeration of entry-points (sources) for data into the GSW backend.
    This includes the Transceiver layers for downlinked data, etc.

    As usual, for backwards compatibility and data preservation, *never* change 
    any of the enum values or delete entries, just deprecate old ones.
    """
    NONE = -1  # Invalid (No source given)
    YAMCS = 0x00  # Any YAMCS Connection
    UDP_SERIAL_DIRECT = 0x01  # Direct UDP Serial Connection
    WIFI_CONNECTION_DIRECT = 0x02  # Direct WiFi Connection
    PCAP = 0x10  # Loaded from a Packet Capture (pcap)
    GENERATED = 0x20  # Data Generated within the GSW (eg. in a metachannel)
    MONGO = 0x30  # Data received (likely from Frontend) via MongoDB


@attr.s(cmp=True, slots=True, auto_attribs=True)
class DownlinkTimes():
    """
    Container for timestamps for every stage of the downlinking pipeline.
    Note: All of these times are the same across an entire packet. 
    Rover-specific times are sent in and managed by each Payload timestamp.
    """
    # When received by lander buffer (`generationTime` in YAMCS):
    lander_rx: Optional[datetime] = None
    # When sent by lander:
    # lander_tx: datetime
    # When received by AMCC [Astrobotic Mission Control] (`acquisitionTime` in YAMCS):
    amcc_rx: Optional[datetime] = None
    # When received by PMCC (Payload Mission Control <- the Iris GSW backend)
    pmcc_rx: Optional[datetime] = None


@attr.s(cmp=True, slots=True, auto_attribs=True)
class UplinkTimes():
    """
    Container for timestamps for every stage of the uplinking pipeline.
    """
    # When the data was generated (i.e. command sent from frontend):
    generated: Optional[datetime] = None
    # When the PMCC GSW backend (this application) first received the data:
    pmcc_rx: Optional[datetime] = None
    # When the PMCC GSW backend (this application) sent the data to AMCC:
    pmcc_tx: Optional[datetime] = None
    # When AMCC acknowledged receipt of the data:
    amcc_rx: Optional[datetime] = None
    # When AMCC reports having sent the data to the spacecraft / released it
    # from the YAMCS queue:
    amcc_tx: Optional[datetime] = None
    # When the rover indicates that it received the data:
    ack_rover: Optional[datetime] = None
    # DownlinkTimes for the rover acknowledgement packet:
    ack_downlink_times: DownlinkTimes = attr.field(factory=DownlinkTimes)
