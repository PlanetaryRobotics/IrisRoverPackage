<title>UWB Component Dictionary</title>
# UWB Component Dictionary


## Command List

|Mnemonic|ID|Description|Arg Name|Arg Type|Comment
|---|---|---|---|---|---|
|Get_Time|0 (0x0)|Command to get the time recordings from the UWB module| | |
|Get_Data|1 (0x1)|Command to get the full time and data recordings from the UWB module| | |

## Telemetry Channel List

|Channel Name|ID|Type|Description|
|---|---|---|---|
|UWBPower|1 (0x1)|U32|U32 representing the power from the UWB|
|UWBBytesSent|2 (0x2)|U32|U32 representing the number of bytes sent from UWB|

## Event List

|Event Name|ID|Description|Arg Name|Arg Type|Arg Size|Description
|---|---|---|---|---|---|---|
|ResponseTimeout|0 (0x0)|Warning that the UWB Module response timed out| | | | |
