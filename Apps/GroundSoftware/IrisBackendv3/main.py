#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Fri Nov 20 13:18:56 2020

@author: connorcolombo
"""

import pcapng as pcap  # type: ignore
from IrisBackendv3.data_standards import DataStandards

# !! To avoid needing a codec process: rebuild the standards **once** then **cache** them, everything else will just quickly load the cached representation.

# TODO: Kick off all ipc processes for various layers:
# -- Process CLI args to configure
# NB: Metachannels inside transceiver process
"""
# parse CLI opts inputs
standards = DataStandards() 
database = MongoInterface()
codec = IrisCodec() | ArtemisCodec()
transceiver = YAMCSTransceiver()
storage = LocalArchive()
"""

# standards = DataStandards.build_standards()

with open('../test-data/Iris_Telemetry_201118.pcap', 'rb') as fp:
    raw_data = pcap.structs.read_bytes(stream=fp, size=100)
    scanner = pcap.FileScanner(fp)
    for block in scanner:
        print(block)
