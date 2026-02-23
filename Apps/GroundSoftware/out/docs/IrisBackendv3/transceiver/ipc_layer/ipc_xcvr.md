Module IrisBackendv3.transceiver.ipc_layer.ipc_xcvr
===================================================
Standalone codec process which manages the reading of telemetry and sending of
commands using a transceiver.

@author: Connor W. Colombo (CMU)
@last-updated: 03/06/2023

Functions
---------

    
`build_app(opts) ‑> IrisBackendv3.ipc.app_manager.IpcAppManagerAsync`
:   Builds the app based on the settings in opts.

    
`get_opts()`
:   Return settings wrapped in argparse.

    
`main()`
:   

    
`run_main(opts) ‑> None`
: