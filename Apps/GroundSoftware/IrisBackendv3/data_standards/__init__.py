# -*- coding: utf-8 -*-
"""
This package interprets the definitions for all of the Data Standards defined in 
the Command & Telemetry List (C&TL). Essentially, this package contains various 
containers which describe the *specifications* (herein referred to as 
'data standards') for how various pieces of data (Commands, Telemetry, etc.) are 
structured. These specifications are then populated using the building tools in 
`data_standards.py` which either parses FPrime XML (which *should* be built 
according to C&TL specifications) or various supporting pieces of Flight 
Software (FSW) definition code (eg. cpp headers). In addition, there are parsing 
tools which support finding discrepancies between the FSW FPrime and the C&TL.

The FSW is treated as the source of truth (rather than the C&TL or GSW) so that 
the FSW team can have the freedom to readily change their standards with the 
assurance that GSW will be able to accommodate immediately. This also allows GSW 
to save compact `data_standards` caches alongside data logs from the rover to 
ensure the formatting of logged data streams is always known.

@author: Connor W. Colombo (CMU)
@last-updated: 01/31/2021
"""

# Expose internal modules (allow them to be accessed directly):
from .data_standards import *

from .fsw_data_type import *

from .exceptions import *
