Module IrisBackendv3.storage.commands
=====================================
Methods for handling & archiving commands.
Processing of uplinked data.

@author: Colombo, CMU
@last-updated: 10/03/2024

Functions
---------

    
`extract_data_from_yamcs(yamcs_cmds: List[yamcs.tmtc.model.CommandHistory]) ‑> List[Tuple[IrisBackendv3.codec.payload.CommandPayload, IrisBackendv3.storage.commands.YamcsCommandMetadata]]`
:   Extracts Command Data from a YAMCS CommandHistory:

    
`generate_command_row(cmd: IrisBackendv3.codec.payload.CommandPayload, yamcs_data: IrisBackendv3.storage.commands.YamcsCommandMetadata) ‑> pandas.core.frame.DataFrame`
:   Generates a row for the Archive table from relevant command data.

    
`yamcs_history_to_commands(yamcs_cmds: List[yamcs.tmtc.model.CommandHistory]) ‑> IrisBackendv3.storage.dataset.DataSet`
:   Produces a DataSet of all commands and relevant metadata logged in YAMCS.
    All data is sorted chronologically.

Classes
-------

`YamcsCommandMetadata(user: str | None = None, xtce_name: str | None = None, queue: str | None = None, comment: str | None = None)`
:   Container for all Command metadata unique to YAMCS (not used in GDS):

    ### Class variables

    `comment: str | None`
    :

    `queue: str | None`
    :

    `user: str | None`
    :

    `xtce_name: str | None`
    :