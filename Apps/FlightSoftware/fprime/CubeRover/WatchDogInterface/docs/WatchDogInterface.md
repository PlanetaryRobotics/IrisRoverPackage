<title>WatchDogInterface Component Dictionary</title>
# WatchDogInterface Component Dictionary


## Command List

|Mnemonic|ID|Description|Arg Name|Arg Type|Comment
|---|---|---|---|---|---|
|Reset_Specific|0 (0x0)|Command to reset the specific parts of rover| | |
| | | |reset_value|U16|
                    	U16 Value that specifies which components or hardware need to be reset
                    |

## Telemetry Channel List

|Channel Name|ID|Type|Description|
|---|---|---|---|
|LAST_COMMAND|0 (0x0)|string|The command last sent to watchdog interface|

## Event List

|Event Name|ID|Description|Arg Name|Arg Type|Arg Size|Description
|---|---|---|---|---|---|---|
|WatchDogMSP430IncorrectResp|0 (0x0)|Warning that the WatchDog MSP430 sent back a response different than what was sent to it| | | | |
|WatchDogTimedOut|1 (0x1)|Warning that a WatchDog MSP430 watchdog timer went off| | | | |
|WatchDogCmdReceived|2 (0x2)|Notification that watchdog interface recieved a command from Cmd_Dispatcher| | | | |
| | | |Cmd|Fw::LogStringArg&|50|The cmd that watchdog interface processed|
|WatchDogCommError|3 (0x3)|Warning that a WatchDog MSP430 error has occured.| | | | |
| | | |error|U32||The watchdog error value (reference to documentation)|
