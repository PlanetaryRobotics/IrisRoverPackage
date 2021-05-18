"""
Various Classes defining Metadata for Packets and Payloads.

@author: Connor W. Colombo (CMU)
@last-updated: 05/18/2021
"""

from typing import List

from enum import Enum
from datetime import datetime


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
    PCAP = 0x10  # Loaded from a Packet Capture (pcap)
    GENERATED = 0x20  # Data Generated within the GSW (eg. in a metachannel)
    MONGO = 0x30  # Data received (likely from Frontend) via MongoDB


class DownlinkTimes():
    """
    Container for timestamps for every stage of the downlinking pipeline.
    Note: All of these times are the same across an entire packet. 
    Rover-specific times are sent in and managed by each Payload timestamp.
    """
    __slots__: List[str] = [
        # When received by lander buffer (`generationTime` in YAMCS):
        'lander_rx',
        # When sent by lander:
        # 'lander_tx',
        # When received by AMCC [Astrobotic Mission Control] (`acquisitionTime` in YAMCS):
        'amcc_rx',
        # When received by PMCC (Payload Mission Control <- the Iris GSW backend)
        'pmcc_rx'
    ]
    # lander_tx: datetime
    lander_rx: datetime
    amcc_rx: datetime
    pmcc_rx: datetime

    def __init__(self,
                 #  lander_tx: datetime,
                 lander_rx: datetime,
                 amcc_rx: datetime,
                 pmcc_rx: datetime
                 ) -> None:
        self.lander_rx = lander_rx
        # self.lander_tx = lander_tx
        self.amcc_rx = amcc_rx
        self.pmcc_rx = pmcc_rx


class UplinkTimes():
    """
    Container for timestamps for every stage of the uplinking pipeline.
    """
    __slots__: List[str] = [
        # When the data was generated (i.e. command sent from frontend):
        'generated',
        # When the PMCC GSW backend (this application) first received the data:
        'pmcc_rx',
        # When the PMCC GSW backend (this application) sent the data to AMCC:
        'pmcc_tx',
        # When AMCC acknowledged receipt of the data:
        'amcc_rx',
        # When AMCC reports having sent the data to the spacecraft:
        # 'amcc_tx'
        # When the rover indicates that it received the data:
        'ack_rover',
        # DownlinkTimes for the rover acknowledgement packet:
        'ack_downlink_times'
    ]

    generated: datetime
    pmcc_rx: datetime
    pmcc_tx: datetime
    amcc_rx: datetime
    # amcc_tx: datetime
    ack_rover: datetime
    ack_downlink_times: DownlinkTimes

    def __init__(self,
                 generated: datetime,
                 pmcc_rx: datetime,
                 pmcc_tx: datetime,
                 amcc_rx: datetime,
                 # amcc_tx: datetime,
                 ack_rover: datetime,
                 ack_downlink_times: DownlinkTimes
                 ) -> None:
        self.generated = generated
        self.pmcc_rx = pmcc_rx
        self.pmcc_tx = pmcc_tx
        self.amcc_rx = amcc_rx
        # self.amcc_tx = amcc_tx
        self.ack_rover = ack_rover
        self.ack_downlink_times = ack_downlink_times
