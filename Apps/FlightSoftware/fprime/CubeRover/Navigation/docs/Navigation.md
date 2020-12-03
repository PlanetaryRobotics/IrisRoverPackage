<title>Navigation Component Dictionary</title>
# Navigation Component Dictionary


## Command List

|Mnemonic|ID|Description|Arg Name|Arg Type|Comment
|---|---|---|---|---|---|
|Nav_DriveCommand|0 (0x0)|Movement command| | |
| | | |Command|NAV_COMMANDLIST||
| | | |Distance|U8||
| | | |Speed|U8||

## Telemetry Channel List

|Channel Name|ID|Type|Description|
|---|---|---|---|
|Nav_Moved_Forward|0 (0x0)|U16|The distance moved forward in cm|
|Nav_Moved_Backward|1 (0x1)|U16|The distance moved backward in cm|
|Nav_Turned_Left|2 (0x2)|U16|The angle turned to the left (counter clockwise) in degrees|
|Nav_Turned_Right|3 (0x3)|U16|The angle turned to the right (clockwise) in degrees|
|Nav_Image_Size|4 (0x4)|U16|Image size|

## Event List

|Event Name|ID|Description|Arg Name|Arg Type|Arg Size|Description
|---|---|---|---|---|---|---|
|MC_moveStarted|0 (0x0)|A notification a move sequence has started| | | | |
|MC_moveComplete|1 (0x1)|A notification a move sequence has finished| | | | |
|MC_MSPFaultDetected|2 (0x2)|A notification an MSP has thrown a fault| | | | |
|MC_MSPNotResponding|3 (0x3)|A notification an MSP is not communicating| | | | |
|MC_MSPNotReporting|4 (0x4)|A notification an MSP is not reporting data| | | | |
