# -*- coding: utf-8 -*-
"""
Tests the FSW-GSW CRC Module.

NOTE: **Not** comprehensive at the moment but is a good quick signal test.
@author: Connor W. Colombo (CMU)
@last-updated: 03/07/2023
"""
from IrisBackendv3.utils.crc import _stress_test_crc


class StressSuite(object):
    """
    Tests codec and IPC encode and decode using a `PcapTransceiver` and a PCAP of
    legacy packets (from 2021).
    """

    def test_stress_test(self):
        """
        Runs GSW's algorithm against a collection of known good results
        generated using the FSW CRC32 algorithm in C++ (see
        `_crc32_fsw_test_gen.cpp`).
        """
        _stress_test_crc(silent_success=True, silent_fail=False)
