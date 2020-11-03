<title>WatchDogInterface Component Dictionary</title>
# WatchDogInterface Component Dictionary


## Command List

|Mnemonic|ID|Description|Arg Name|Arg Type|Comment
|---|---|---|---|---|---|
|Reset Everything|0 (0x0)|Command to reset the entire rover| | |
|Reset Specific|1 (0x1)|Command to reset the specific parts of rover| | |
| | | |reset_value|U32|
                    	U32 Value where each bit represents which component/device to reset (0 = no reset, 1 = reset)
                    |


## Event List

|Event Name|ID|Description|Arg Name|Arg Type|Arg Size|Description
|---|---|---|---|---|---|---|
|WatchDog MSP430 Not Responding|0 (0x0)|Warning that the WatchDog MSP430 is not responding| | | | |
|WatchDog Timed Out|1 (0x1)|Warning that a WatchDog MSP430 watchdog timer went off| | | | |
| | | |timer|Fw::LogStringArg&|40|The watchdog timer that has gone off|
|WatchDog Cmd Received|2 (0x2)|Notification that watchdog interface recieved a command from Cmd_Dispatcher| | | | |
| | | |Cmd|Fw::LogStringArg&|40|The cmd that watchdog interface processed|
