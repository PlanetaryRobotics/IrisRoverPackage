# TODO: Reconsider the relationship between what data is passed in and what data is stored alongside `_raw` (do packets really need source and pathway if payloads have it? (just make sure homogeneous) does encode really need to pass through downlinktimes or can that be handled by the transceiver since decode will be happening by ipc and will likely override this anyway if kept here...)
## -- ^ done.

# TODO: ... metadata should be added at transceiver layer to all payloads and then stored alongside them upon decode if being decoded (basically, let metadata have a life of its own outside of and independent of `_raw`)
## TODO: Still need to prop this to all xcvr impl.s, incl. old `trans_` and `tvac_tools`

# ! TODO: TOSS THIS OUT (meta should be handled elsewhere)
 - then refactor Packet.decode so extra args to decode aren't necessary -- scrub out `pathway`, `source`, and `times` from everything in Packet - they're Payload only concepts now (and then edit all calls to decode in the entire codebase) !<<-
 
 - then also handle special one-way packet types like WatchdogHeartbeat (have them just add to metadata instead of encode?)
  # ^- Done. All `CustomPayloadPacket` subclasses handled.

# fix `decode()` impl.s <<- DONE.
# scrub `pathway`, `source` (and `times`) from all `Packet`s <<- DONE.
# fix `__init__()` impl.s <<- DONE.

# fix all `decode()` uses <<- DONE.
# fix all `__init__()` uses <<- DONE.

# fix all `.*_PI` and `.*_CP` TypeAliases (type declarations are bunk) <<- DONE.

# Remove `UplinkPacket` and `DownlinkPacket`. !<<- Commit first.

## -> Impl. `Packet.build_minimum_packet` in every subclass. <<- DONE.

# TODO: Consider removing `custom_payload` object from `CustomPayloadPacket` since it contains the same data as `payloads`. It's def. necessary for construction but it's redundant after construction...
# ... consider the impact this has on serialization.  <<- HANDLED IT (removed but cached).

# Consider adding a `UnknownPacket` class? which just allows for storing bunk packets along side others (same pipelines) - these can then be reloaded and decoded later.

# TODO: Break all `Packet` subclasses out into their own files in their own directory (?)

# Convert all TODOs in codebase into issues / stories.
## ^- fully unit test XCVR, etc. pathways so you can make sure nothing is broken from fixing these in rapid succession.