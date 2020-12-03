<title>MotorControl Component Dictionary</title>
# MotorControl Component Dictionary


## Command List

|Mnemonic|ID|Description|Arg Name|Arg Type|Comment
|---|---|---|---|---|---|
|MC_MotorConfiguration|0 (0x0)|| | |
| | | |Motor_ID|U8|The motor id from 0 - 4 (Motor 0 is Front Left motor and goes counter clockwise) 4 is all motors |
| | | |MotorParameter|MP_CommandList||
| | | |Value|U32||
|MC_DrivingConfiguration|1 (0x1)|Configure movement command| | |
| | | |CommandConfiguration|CC_CommandList||
| | | |Parameter|ParameterList||
| | | |Value|U8||
|MC_ExecuteDriving|2 (0x2)|Execute one configured command| | |
| | | |DrivingCommand|DriveCommandList||
|MC_StallDetection|3 (0x3)|Enable / disable detection of motor stall| | |
| | | |motorStallEnable|motorStallEnableList||
|MC_PositionCounterReset|4 (0x4)|Reset the position counter of specific motor(s)| | |
| | | |ResetPositionCounter|U8||
|MC_UpdateTelemetry|5 (0x5)|Requests an update from each of the motor controllers| | |

## Telemetry Channel List

|Channel Name|ID|Type|Description|
|---|---|---|---|
|MC_FL_Encoder_Dist|0 (0x0)|I32|Front Left motor encoder displacement in encoder ticks|
|MC_FL_Current|1 (0x1)|U16|Front Left motor current in milliAmps|
|MC_FL_Speed|2 (0x2)|I16|Front Left motor speed in centimeters per second (positive rotation is counter clockwise|
|MC_FR_Encoder_Dist|3 (0x3)|I32|Front Right motor encoder displacement in encoder ticks|
|MC_FR_Current|4 (0x4)|U16|Front Right motor current in milliAmps|
|MC_FR_Speed|5 (0x5)|I16|Front Right motor speed in centimeters per second|
|MC_RR_Encoder_Dist|6 (0x6)|I32|Rear Right motor encoder displacement in encoder ticks|
|MC_RR_Current|7 (0x7)|U16|Rear Right motor current in milliAmps|
|MC_RR_Speed|8 (0x8)|I16|Rear Right motor speed in centimeters per second|
|MC_RL_Encoder_Dist|9 (0x9)|I32|Rear Left motor encoder displacement in encoder ticks|
|MC_RL_Current|10 (0xa)|U16|Rear Left motor current in milliAmps|
|MC_RL_Speed|11 (0xb)|I16|Rear Left motor speed in centimeters per second|

## Event List

|Event Name|ID|Description|Arg Name|Arg Type|Arg Size|Description
|---|---|---|---|---|---|---|
|MC_moveStarted|0 (0x0)|A notification a move sequence has started| | | | |
|MC_moveComplete|1 (0x1)|A notification a move sequence has finished| | | | |
|MC_MSPFaultDetected|2 (0x2)|A notification an MSP has thrown a fault| | | | |
|MC_MSPNotResponding|3 (0x3)|A notification an MSP is not communicating| | | | |
|MC_MSPNotReporting|4 (0x4)|A notification an MSP is not reporting data| | | | |
