"""Module for storing & archiving all data from across the entire system.

# TODO: Create a system (seq_id?) for making sure important sent from xcvr layer
# through IPC isn't lost since there are presently no **guarantees** that
# packets don't get dropped or missed.
# ... or consider ROUTER-DEALER pattern between XCVR and STOR

# TODO:: Cache data locally in xcvr layer (volatile might even be fine) so it can be
# retrieved/replayed into STOR in case STOR crashed and missed something.
# Look into [PUB-SUB clone pattern](https://zguide.zeromq.org/docs/chapter5/#Reliable-Pub-Sub-Clone-Pattern)
"""
