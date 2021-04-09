#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Fri Nov 27 15:02:00 2020

@author: connorcolombo
"""
from IrisBackendv3.data_standards import DataStandards

# Commands and Telemetry: Fp XML Comments
# Logs and Events: Fp XML (parse string formatter)
# Files: structs in FswPacket.hpp

# Two telemetry after last block = dropped block. Close file.
# struct pack (struct)

# maintain state to avoid resetting registers unnecessarily
sequence_number = 10

standards = DataStandards.build_standards()

standards.print_overview()

standards.cache(
    cache_dir="./sandbox-cache",
    filename_base="sandbox", ext="json",
    indent=2
)

uncached_standands = DataStandards.load_cache(
    cache_dir="./sandbox-cache",
    filename_base="sandbox", ext="json"
)

print(f"Standards match: {uncached_standands == standards}")