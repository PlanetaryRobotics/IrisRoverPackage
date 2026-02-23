# -*- coding: utf-8 -*-
"""
This package handles all transceivers (broadly speaking, I/O interfaces with
the outside/physical HW world).

@author: Connor W. Colombo (CMU)
@last-updated: 03/06/2023
"""

# Expose internal modules (allow them to be accessed directly):
from IrisBackendv3.transceiver.endec import *
from IrisBackendv3.transceiver.transceiver import *
from IrisBackendv3.transceiver.slip_transceiver import *
from IrisBackendv3.transceiver.pcap_transceiver import *
from IrisBackendv3.transceiver.xcvr_enum import TransceiverEnum

# Keep specialized modules still wrapped in a namespace (make them dot
# accessible from root):
from IrisBackendv3.transceiver import exceptions
from IrisBackendv3.transceiver import prebuilts
from IrisBackendv3.transceiver import settings
from IrisBackendv3.transceiver import logging
