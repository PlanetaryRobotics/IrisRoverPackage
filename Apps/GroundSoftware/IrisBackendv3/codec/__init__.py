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
@last-updated: 03/03/2023
"""

# Expose internal modules (allow them to be accessed directly):
from .magic import *

from .fsw_data_codec import *

from .exceptions import *

# Expose important codec modules as modules (so they can be dot-accessed when
# including IrisBackendv3 as a package):
from . import metadata
from . import packet
from . import payload_collection
from . import payload
from . import settings
