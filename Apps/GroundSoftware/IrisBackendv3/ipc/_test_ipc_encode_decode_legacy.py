# -*- coding: utf-8 -*-
"""
Tests codec and IPC encode and decode using a `PcapTransceiver` and a PCAP of
legacy packets (from 2021).

NOTE: **Not** comprehensive at the moment but is a good quick signal test.
@author: Connor W. Colombo (CMU)
@last-updated: 03/06/2023
"""
import pytest
from typing import Any, List, Union, Tuple
import inspect
from copy import deepcopy

from collections import OrderedDict
import jsonpickle  # type: ignore # mypy doesn't see type hints

import asyncio
import traceback
from typing import cast, Final, List

from IrisBackendv3.ipc.messages import DownlinkedPacketsMessage, DownlinkedPacketsContent
from IrisBackendv3.ipc.logging import logger
import IrisBackendv3 as IB3
IB3.init_from_latest()

PERIOD_MS: Final[float] = 1


@pytest.fixture(scope="class")
def suite_setup(request):
    # Load Test Data:
    request.cls.xcvr = IB3.transceiver.prebuilts.build_xcvr_by_name(
        'PCAP-18H',
        packetgap=37000,  # skip first 37000 packets (of 37644)
        fixed_period_ms=PERIOD_MS,
        loop=False,
        log_on_receive=False
    )


@pytest.mark.usefixtures('suite_setup')
class LegacySuite(object):
    """
    Tests codec and IPC encode and decode using a `PcapTransceiver` and a PCAP of
    legacy packets (from 2021).
    """

    def test_pcap_load_decode_pretest(self):
        """Basic test of codec and transceiver
        pcap decode to make sure it works for the next step.
        Really belongs in codec/transceiver test suite but don't have time for
        that atm so this serves as part of a non-comprehensive end-to-end test.
        """
        xcvr = self.__class__.xcvr
        xcvr.begin()
        while len(_ := xcvr.read()) != 0:
            # Make sure it's able to read and decode all packets
            pass

    def test_pcap_replay_encode_decode(self):
        xcvr = self.__class__.xcvr
        xcvr.begin()
        while len(packets := xcvr.read()) != 0:
            # Make sure it's able to read and decode all packets
            dpm = DownlinkedPacketsMessage(DownlinkedPacketsContent(
                packets=packets
            ))

            dpmb = dpm.to_ipc_bytes()
            dpm2 = DownlinkedPacketsMessage.from_ipc_bytes(dpmb)
            print(dpm2)

            # Make sure reconstruction happened correctly:
            assert dpm == dpm2, (
                f"Object reconstructed from binary IPC message doesn't "
                f"match input object. "
                f"\nInput: `{dpm = }`."
                f"\n->Binary: `{dpmb = }`."
                f"\n->Output: `{dpm2 = }`."
            )
