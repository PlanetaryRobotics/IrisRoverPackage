<title>Navigation Component Dictionary</title>
# Navigation Component Dictionary


## Command List

|Mnemonic|ID|Description|Arg Name|Arg Type|Comment
|---|---|---|---|---|---|
|Nav_DriveForward|0 (0x0)|Forward movement command| | |
| | | |Distance|U8|0x00 - 0x5A meaning 0-30 cm|
| | | |Speed|U8|0x00 - 0x0A meaning 0-10 cm/s|
| | | |Callback_ID|U16||
|Nav_DriveBackwards|1 (0x1)|Backward movement command| | |
| | | |Distance|U8|0x00 - 0x5A meaning 0-30 cm|
| | | |Speed|U8|0x00 - 0x0A meaning 0-10 cm/s|
| | | |Callback_ID|U16||
|Nav_RotateRight|2 (0x2)|Clockwise rotation command| | |
| | | |Distance|U8|0x00 - 0x5A meaning 0-90 deg|
| | | |Speed|U8|0x00 - 0x0A meaning 0-10 deg/s|
| | | |Callback_ID|U16||
|Nav_RotateLeft|3 (0x3)|Counter-clockwise rotation command| | |
| | | |Distance|U8|0x00 - 0x5A meaning 0-90 deg|
| | | |Speed|U8|0x00 - 0x0A meaning 0-10 deg/s|
| | | |Callback_ID|U16||
|Nav_StopMovement|4 (0x4)|Stops all movement| | |

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
|Nav_moveInterrupted|0 (0x0)|A notification a move has been interrupted| | | | |
|Nav_moveQueueLimit|1 (0x1)|A notification a move was not placed on the move queue| | | | |
