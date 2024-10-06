Module IrisBackendv3.ipc.port
=============================
This module supplies an Enum which defines all ports used in the IPC.

Actual port numbers are defined in `ports.env`.

@author: Connor W. Colombo (CMU)
@last-updated: 11/30/2023

Classes
-------

`Port(value, names=None, *, module=None, qualname=None, type=None, start=1)`
:   Enum for defines all ports used in the IPC.
    
    Try to make sure these ports aren't occupied on `localhost` the hosting 
    device but feel free to change them here if needed. This enum should serve 
    as the single source of truth for the port numbers.

    ### Ancestors (in MRO)

    * IrisBackendv3.ipc.port._EnumFromEnv
    * enum.Enum

    ### Class variables

    `ATLAS_PUB`
    :

    `ATLAS_SUB`
    :

    `CODEC_PUB`
    :

    `CODEC_SUB`
    :

    `GUI_PUB`
    :

    `GUI_SUB`
    :

    `REMOTEDB_PUB`
    :

    `REMOTEDB_SUB`
    :

    `STORAGE_PUB`
    :

    `STORAGE_SUB`
    :

    `TRANSCEIVER_PUB`
    :

    `TRANSCEIVER_SUB`
    :