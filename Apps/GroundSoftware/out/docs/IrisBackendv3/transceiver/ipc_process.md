Module IrisBackendv3.transceiver.ipc_process
============================================
Standalone transceiver IPC App which manages the reading of telemetry and
sending of commands using (a) transceiver(s) in both the uplink and downlink
direction simultaneously.

@author: Connor W. Colombo (CMU)
@last-updated: 06/21/2023

Functions
---------

    
`build_xcvrs(app: ipc.IpcAppHelper, opts) ‑> Dict[Type[IrisBackendv3.transceiver.transceiver.Transceiver], IrisBackendv3.transceiver.transceiver.Transceiver]`
:   

    
`get_opts()`
:   Return settings wrapped in argparse.

    
`main(app: ipc.IpcAppHelper, opts) ‑> None`
:   

    
`make_uplink_handler(app: ipc.IpcAppHelper, xcvrs: Dict[Type[Transceiver], Transceiver]) ‑> Type[IrisBackendv3.ipc.app_manager.SocketTopicHandlerAsync]`
:   Makes an uplink handler that listens for uplink packets and dispatches
    them to the appropriate Transceivers from the given Transceiver map `xcvrs`.

Classes
-------

`DownlinkCoroutine(app: ipc.IpcAppHelper, manager: ipc.IpcAppManagerAsync, xcvr: Transceiver, n_packets_rcvd: int = 0)`
:   Async functor for an `asyncio` coroutine to handle processing downlinked
    messages for a given transceiver.

    ### Class variables

    `app: IrisBackendv3.ipc.app_manager.IpcAppHelper`
    :

    `manager: IrisBackendv3.ipc.app_manager.IpcAppManagerAsync`
    :

    `n_packets_rcvd: int`
    :

    `xcvr: IrisBackendv3.transceiver.transceiver.Transceiver`
    :

    `xcvr_tag: bytes`
    :