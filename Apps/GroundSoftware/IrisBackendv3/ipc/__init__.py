# -*- coding: utf-8 -*-
"""
This package handles all InterProcess Communication (IPC) between the various 
processes (ex. database, codec, transceiver) that make up the GSW using 
client/server and pub/sub models. Designed initially to wrap ZeroMQ (ZMQ/0MQ), 
all classes and functions are wrapped so that this interface can be changed 
later if needed to support new requirements without disrupting the rest of the 
system.

@author: Connor W. Colombo (CMU)
@last-updated: 05/18/2021
"""

# Expose internal modules (allow them to be accessed directly):
from .port import *
from .topic import *

from .wrapper import *
