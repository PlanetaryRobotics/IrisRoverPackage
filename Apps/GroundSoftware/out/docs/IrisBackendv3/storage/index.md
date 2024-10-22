Module IrisBackendv3.storage
============================
Module for storing & archiving data from mission.

Goal is to store all from across the entire system (all IPC transactions) but
current approach is to just archive all data in and out of the transceiver
layer so it can be replayed into the system and used to regenerate all the
other data.

Initially only archives exported from YAMCS data are supported.

Usage:
- To open or save an archive file use `IrisBackendv3.storage.DataSet`.
- The `DataSet.data` member can then be used to explore the data.
- `.yamcs.X` files archive just data received from / sent to YAMCS.
- `.telem.X` files archive all telemetry data extracted from YAMCS data
- `.h5` files store both under `yamcs` and `telem` keys. Not suitable for
multi-day / high data volume missions.

TODO: Add archiver process that pulls data from transceiver layer and stores it
in `DataSet`. 

# TODO: Create a system (seq_id?) for making sure important sent from xcvr layer
# through IPC isn't lost since there are presently no **guarantees** that
# packets don't get dropped or missed.
# ... or consider ROUTER-DEALER pattern between XCVR and STOR

# TODO:: Cache data locally in xcvr layer (volatile might even be fine) so it can be
# retrieved/replayed into STOR in case STOR crashed and missed something.
# Look into [PUB-SUB clone pattern](https://zguide.zeromq.org/docs/chapter5/#Reliable-Pub-Sub-Clone-Pattern)

Sub-modules
-----------
* IrisBackendv3.storage.commands
* IrisBackendv3.storage.dataset
* IrisBackendv3.storage.formatting
* IrisBackendv3.storage.logs
* IrisBackendv3.storage.settings
* IrisBackendv3.storage.telemetry
* IrisBackendv3.storage.utils