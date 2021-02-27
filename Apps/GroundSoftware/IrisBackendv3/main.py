#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Fri Nov 20 13:18:56 2020

@author: connorcolombo
"""

import pcapng as pcap  # type: ignore

"""
# parse CLI opts inputs
standards = DataStandards() 
database = MongoInterface()
codec = IrisCodec() | ArtemisCodec()
transceiver = YAMCSTransceiver()
storage = LocalArchive()
"""


with open('../test-data/Iris_Telemetry_201118.pcap', 'rb') as fp:
    raw_data = pcap.structs.read_bytes(stream=fp, size=100)
    scanner = pcap.FileScanner(fp)
    for block in scanner:
        print(block)
