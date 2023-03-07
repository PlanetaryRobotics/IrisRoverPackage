# -*- coding: utf-8 -*-
"""
This package handles all InterProcess Communication (IPC) between the various 
processes (ex. database, codec, transceiver) that make up the GSW using 
client/server and pub/sub models. Designed initially to wrap ZeroMQ (ZMQ/0MQ), 
all classes and functions are wrapped so that this interface can be changed 
later if needed to support new requirements without disrupting the rest of the 
system.

@author: Connor W. Colombo (CMU)
@last-updated: 03/07/2023
"""
# Import `restricted_pickler` before **ANYTHING ELSE** so it has first dibs on
# how the modules it cares about are imported:
from IrisBackendv3.ipc import restricted_pickler

# Expose internal modules (allow them to be accessed directly):

from IrisBackendv3.ipc.settings import settings
from IrisBackendv3.ipc.exceptions import *
from IrisBackendv3.ipc.inter_process_message import *
from IrisBackendv3.ipc.ipc_payload import *
from IrisBackendv3.ipc.topics_registry import *
from IrisBackendv3.ipc.wrapper import *
from IrisBackendv3.ipc.app_manager import *

# Expose important IPC modules as modules (so they can be dot-accessed when
# including IrisBackendv3 as a package):
from IrisBackendv3.ipc import port
from IrisBackendv3.ipc import messages
from IrisBackendv3.ipc import topics
from IrisBackendv3.ipc import logging
