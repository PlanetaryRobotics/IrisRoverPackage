# TODO: ... metadata should be added at transceiver layer to all payloads and then stored alongside them upon decode if being decoded (basically, let metadata have a life of its own outside of and independent of `_raw`)
## TODO: Still need to prop this to all xcvr impl.s, incl. old `trans_` and `tvac_tools`


# Remove `UplinkPacket` and `DownlinkPacket`. !<<- DONE.
# Consider adding a `UnknownPacket` class? which just allows for storing bunk packets along side others (same pipelines) - these can then be reloaded and decoded later.

# TODO: Break all `Packet` subclasses out into their own files in their own directory (?)

# TODO: Warn if packet contains a mix of UplinkPayload, DownlinkPayload, and normal Payload subclasses.
# Convert all TODOs in codebase into issues / stories.
## ^- fully unit test XCVR, etc. pathways so you can make sure nothing is broken from fixing these in rapid succession.