<title>UWB Component Dictionary</title>
# UWB Component Dictionary


## Command List

|Mnemonic|ID|Description|Arg Name|Arg Type|Comment
|---|---|---|---|---|---|
|Change_Rate|0 (0x0)|Command to change the rate the UWB takes measurements| | |
| | | |cycles|U8|
                    	U8 Value that represents how many cycles the UWB module waits to execute
                    |

## Telemetry Channel List

|Channel Name|ID|Type|Description|
|---|---|---|---|
|TimeStamp1|1 (0x1)|U32|Timestamp 1 Array from UWB|
|TimeStamp2|2 (0x2)|U32|Timestamp 2 Array from UWB|
|TimeStamp3|3 (0x3)|U32|Timestamp 3 Array from UWB|
|TimeStamp4|4 (0x4)|U32|Timestamp 4 Array from UWB|
|UWBPower|5 (0x5)|U32|U32 representing the power from the UWB|

## Event List

|Event Name|ID|Description|Arg Name|Arg Type|Arg Size|Description
|---|---|---|---|---|---|---|
|FirstResponseTimeout|0 (0x0)|Warning that the UWB Module first response timed out| | | | |
| | | |error_code|U32||U32 value that represents the Error Code for UWB Module|
|SecondResponseTimeout|1 (0x1)|Warning that the UWB Module second response timed out| | | | |
| | | |error_code|U32||U32 value that represents the Error Code for UWB Module|
