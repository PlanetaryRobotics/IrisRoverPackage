"""
Handles processing of downlink packets into payloads (and metafields).

@author: Connor W. Colombo (CMU)
@last-updated: 10/18/2024
"""
from typing import cast, Final, List, Dict, Type
from datetime import datetime, timedelta

import IrisBackendv3 as IB3

from IrisBackendv3.codec.payload_collection import EnhancedPayloadCollection
from IrisBackendv3.codec.payload import DownlinkedPayload
from IrisBackendv3.codec.metadata import DownlinkTimes
from IrisBackendv3.meta.metafield import process_payloads_for_meta_modules, MetaModule

from ipc_apps.dl_processor_lib.timestamping import RoverTimeEstimator


def generate_metafields(payloads: EnhancedPayloadCollection, ALL_META_MODULES: List[MetaModule]) -> EnhancedPayloadCollection:
    """Generates all metafields that can be generated, adds them to the given
    `payloads` collection, and returns it (for chaining)."""
    meta_payloads = process_payloads_for_meta_modules(
        modules=ALL_META_MODULES,
        payloads=[
            cast(DownlinkedPayload, p) for p in payloads[DownlinkedPayload]
        ]
    )
    payloads.extend(meta_payloads)
    return payloads


def process_dl_payloads(
    payloads: IB3.codec.payload_collection.EnhancedPayloadCollection,
    ALL_META_MODULES: List[MetaModule]
) -> IB3.codec.payload_collection.EnhancedPayloadCollection:
    """Performs post-processing on all the given payloads.
    - Calculates and applies the appropriate SCET to each payload.
    - Generates metafields.
    """
    # Create a tool to estimate the on-rover emission datetime for
    # any payloads in this collection:
    time_est = RoverTimeEstimator(payloads)

    # Add SCET-estimate to all payloads that don't already have one:
    for i, p in enumerate(payloads[DownlinkedPayload]):
        p = cast(DownlinkedPayload, p)
        scet, delay = time_est.estimate_rover_scet(p)
        # Add a microsecond offset of the index to increase the odds that
        # timestamps are unique to minimize the odds of a collision:
        scet = scet + timedelta(microseconds=i)
        # Add to payload times:
        if p.downlink_times is None:
            p.downlink_times = DownlinkTimes()
        p.downlink_times.scet_est = scet
        p.downlink_times.scet_dl_delay_est = delay

    payloads = generate_metafields(payloads, ALL_META_MODULES)

    return payloads
