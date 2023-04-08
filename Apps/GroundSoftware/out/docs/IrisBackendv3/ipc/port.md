Module IrisBackendv3.ipc.port
=============================
This module supplies an Enum which defines all ports used in the IPC.

@author: Connor W. Colombo (CMU)
@last-updated: 05/17/2021

Classes
-------

`Port(value, names=None, *, module=None, qualname=None, type=None, start=1)`
:   Enum for defines all ports used in the IPC.
    
    Try to make sure these ports aren't occupied on `localhost` the hosting 
    device but feel free to change them here if needed. This enum should serve 
    as the single source of truth for the port numbers.

    ### Ancestors (in MRO)

    * enum.Enum

    ### Class variables

    `ATLAS`
    :

    `CLI_WINDOW`
    :

    `CODEC`
    :

    `GUI`
    :

    `REMOTEDB`
    :

    `STORAGE`
    :

    `TELEM_WINDOW`
    :

    `TRANSCEIVER`
    :