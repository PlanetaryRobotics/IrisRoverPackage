Module IrisBackendv3.ipc
========================
This package handles all InterProcess Communication (IPC) between the various 
processes (ex. database, codec, transceiver) that make up the GSW using 
client/server and pub/sub models. Designed initially to wrap ZeroMQ (ZMQ/0MQ), 
all classes and functions are wrapped so that this interface can be changed 
later if needed to support new requirements without disrupting the rest of the 
system.

@author: Connor W. Colombo (CMU)
@last-updated: 04/30/2023

Sub-modules
-----------
* IrisBackendv3.ipc.app_manager
* IrisBackendv3.ipc.exceptions
* IrisBackendv3.ipc.inter_process_message
* IrisBackendv3.ipc.ipc_payload
* IrisBackendv3.ipc.logs
* IrisBackendv3.ipc.messages
* IrisBackendv3.ipc.port
* IrisBackendv3.ipc.restricted_pickler
* IrisBackendv3.ipc.serializer
* IrisBackendv3.ipc.settings
* IrisBackendv3.ipc.signature
* IrisBackendv3.ipc.topic_definition
* IrisBackendv3.ipc.topic_proxy
* IrisBackendv3.ipc.topics
* IrisBackendv3.ipc.topics_registry
* IrisBackendv3.ipc.wrapper