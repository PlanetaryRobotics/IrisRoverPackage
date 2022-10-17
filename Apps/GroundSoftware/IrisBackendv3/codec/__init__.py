# -*- coding: utf-8 -*-
"""
This package handles all encoding and decoding (codec) operations for:
- Ingesting raw bytes from the `Transceiver` layer, determining if they're 
intact, matching them to the correct `DataStandard`, and exporting intelligible 
payloads to the `Database` layer.
- Ingesting payloads from the `Database` layer, loading specs from the
corresponding `DataStandards` and exporting a properly formatted stream of bytes 
to the `Transceiver` layer.

@author: Connor W. Colombo (CMU)
@last-updated: 04/08/2021
"""

# Expose internal modules (allow them to be accessed directly):
from .magic import *

from .fsw_data_codec import *

from .exceptions import *
