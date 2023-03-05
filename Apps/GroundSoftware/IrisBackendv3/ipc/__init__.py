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

from .settings import settings
from .wrapper import *
from .inter_process_message import *
from .ipc_payload import *
from .topics_registry import *

# Expose important IPC modules as modules (so they can be dot-accessed when
# including IrisBackendv3 as a package):
from . import port
from . import messages
from . import topics
