Module IrisBackendv3.codec
==========================
This package handles all encoding and decoding (codec) operations for:
- Ingesting raw bytes from the `Transceiver` layer, determining if they're 
intact, matching them to the correct `DataStandard`, and exporting intelligible 
payloads to the `Database` layer.
- Ingesting payloads from the `Database` layer, loading specs from the
corresponding `DataStandards` and exporting a properly formatted stream of bytes 
to the `Transceiver` layer.

@author: Connor W. Colombo (CMU)
@last-updated: 03/03/2023

Sub-modules
-----------
* IrisBackendv3.codec.bgapi
* IrisBackendv3.codec.container
* IrisBackendv3.codec.exceptions
* IrisBackendv3.codec.files
* IrisBackendv3.codec.fsw_data_codec
* IrisBackendv3.codec.ipc_messages
* IrisBackendv3.codec.ipc_process
* IrisBackendv3.codec.logging
* IrisBackendv3.codec.magic
* IrisBackendv3.codec.metadata
* IrisBackendv3.codec.packet
* IrisBackendv3.codec.packet_classes
* IrisBackendv3.codec.payload
* IrisBackendv3.codec.payload_collection
* IrisBackendv3.codec.settings