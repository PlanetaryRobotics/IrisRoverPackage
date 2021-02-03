<title>WatchDogInterface Component Dictionary</title>
# WatchDogInterface Component Dictionary


## Command List

|Mnemonic|ID|Description|Arg Name|Arg Type|Comment
|---|---|---|---|---|---|
|Reset_Specific|0 (0x0)|Command to reset the specific parts of rover| | |
| | | |reset_value|U8|
                    	U8 Value that specifies which components or hardware need to be reset
                    |
|Disengage_From_Lander|1 (0x1)|Command to send signal to MSP430 that it should send a signal to lander to disengage| | |

## Telemetry Channel List

|Channel Name|ID|Type|Description|
|---|---|---|---|
|VOLTAGE_2_5V|2 (0x2)|int16_t|Voltage from 2.5V line from Watchdog|
|VOLTAGE_2_8V|3 (0x3)|int16_t|Voltage from 2.8V line from Watchdog|
|VOLTAGE_24V|4 (0x4)|int16_t|Voltage from 24V line from Watchdog|
|VOLTAGE_28V|5 (0x5)|int16_t|Voltage from 28V line from Watchdog|
|BATTERY_THERMISTOR|16 (0x10)|U8|Boolean for Battery Charging or not from Watchdog|
|SYSTEM_STATUS|23 (0x17)|int8_t|Boolean for Heater On/Off from Watchdog|
|BATTERY_LEVEL|24 (0x18)|U16|Value of Charge in Battery|
|THERM_0|25 (0x19)|U16|12 bit Value for Thermistor value|
|THERM_1|26 (0x1a)|U16|12 bit Value for Thermistor value|
|THERM_2|27 (0x1b)|U16|12 bit Value for Thermistor value|
|THERM_3|28 (0x1c)|U16|12 bit Value for Thermistor value|
|THERM_4|29 (0x1d)|U16|12 bit Value for Thermistor value|
|THERM_5|30 (0x1e)|U16|12 bit Value for Thermistor value|

## Event List

|Event Name|ID|Description|Arg Name|Arg Type|Arg Size|Description
|---|---|---|---|---|---|---|
|WatchDogIncorrectResp|0 (0x0)|Warning that the WatchDog MSP430 sent back a response different than what was sent to it| | | | |
|WatchDogTimedOut|1 (0x1)|Warning that a WatchDog MSP430 watchdog timer went off| | | | |
|WatchDogCmdReceived|2 (0x2)|Notification that watchdog interface recieved a command from Cmd_Dispatcher| | | | |
| | | |Cmd|Fw::LogStringArg&|50|The cmd that watchdog interface processed|
|WatchDogCommError|3 (0x3)|Warning that a WatchDog MSP430 error has occured.| | | | |
| | | |error|U32||The watchdog error value (reference to documentation)|
|ADCThermistorError|4 (0x4)|Warning that less than 16 thermistors had conversions or conversion never completed meaning no new tempurature values are being generated| | | | |
