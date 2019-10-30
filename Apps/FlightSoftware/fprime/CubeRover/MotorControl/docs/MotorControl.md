<title>MotorControl Component Dictionary</title>
# MotorControl Component Dictionary


## Command List

|Mnemonic|ID|Description|Arg Name|Arg Type|Comment
|---|---|---|---|---|---|
|MC_DrivingConfiguration|1 (0x1)|Configure forward command| | |
| | | |CommandConfiguration|CommandList||
| | | |Parameter|ParameterList||
| | | |Value|U8||
|MC_ExecuteDriving|2 (0x2)|Execute one configured command| | |
| | | |DrivingCommand|DriveCommandList||
|MC_TuningParameters|3 (0x3)|Contains all motor control parameters| | |
| | | |TuningParameter|tuningParameterList||
| | | |Value|U16||
|MC_AccelerationProfiles|4 (0x4)|Contains acceleration and deceleration profile| | |
| | | |AccelerationParameter|accelerationParameterList||
| | | |Value|U16||
|MC_StallDetection|5 (0x5)|Enable / disable detection of motor stall| | |
| | | |motorStallEnable|motorStallEnableList||
|MC_PositionCounterReset|6 (0x6)|Reset position counter| | |
| | | |ResetPositionCounter|U8||

## Telemetry Channel List

|Channel Name|ID|Type|Description|
|---|---|---|---|
|MC_FL_Current|0 (0x0)|U16|Front Left motor current in milliAmps|
|MC_FR_Current|1 (0x1)|U16|Front Right motor current in milliAmps|
|MC_RL_Current|2 (0x2)|U16|Rear Left motor current in milliAmps|
|MC_RR_Current|3 (0x3)|U16|Rear Right motor current in milliAmps|
|MC_FL_Speed|4 (0x4)|I32|Front Left motor speed (RPM)|
|MC_FR_Speed|5 (0x5)|I32|Front Right motor speed (RPM)|
|MC_RL_Speed|6 (0x6)|I32|Rear Left motor speed (RPM)|
|MC_RR_Speed|7 (0x7)|I32|Rear Right motor speed (RPM)|
|MC_FL_Revolution|8 (0x8)|I32|Front Left motor Revolution|
|MC_FR_Revolution|9 (0x9)|I32|Front Right motor Revolution|
|MC_RL_Revolution|10 (0xa)|I32|Rear Left motor Revolution|
|MC_RR_Revolution|11 (0xb)|I32|Rear Right motor Revolution|

## Event List

|Event Name|ID|Description|Arg Name|Arg Type|Arg Size|Description
|---|---|---|---|---|---|---|
|MC_LedChangedState|0 (0x0)|This diagnostic event indicates the status of the led| | | | |
| | | |ledState|Fw::LogStringArg&|5|The led state|
