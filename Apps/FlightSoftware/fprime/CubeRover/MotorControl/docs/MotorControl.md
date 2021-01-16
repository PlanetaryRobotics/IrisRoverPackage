<title>MotorControl Component Dictionary</title>
# MotorControl Component Dictionary


## Command List

|Mnemonic|ID|Description|Arg Name|Arg Type|Comment
|---|---|---|---|---|---|
|MC_Current_PID|0 (0x0)|| | |
| | | |Motor_ID|U8|The motor id from 0 - 4 (Motor 0 is Front Left motor and goes counter clockwise) 4 is all motors |
| | | |PI_Values|U32||
|MC_Speed_PID|1 (0x1)|| | |
| | | |Motor_ID|U8|The motor id from 0 - 4 (Motor 0 is Front Left motor and goes counter clockwise) 4 is all motors |
| | | |PID_Values|U64||
|MC_Position_PID|2 (0x2)|| | |
| | | |Motor_ID|U8|The motor id from 0 - 4 (Motor 0 is Front Left motor and goes counter clockwise) 4 is all motors |
| | | |PID_Values|U64||
|MC_Acceleration|3 (0x3)|| | |
| | | |Motor_ID|U8|The motor id from 0 - 4 (Motor 0 is Front Left motor and goes counter clockwise) 4 is all motors |
| | | |Rate_Values|U32||
|MC_StallDetection|4 (0x4)|| | |
| | | |Motor_ID|U8|The motor id from 0 - 4 (Motor 0 is Front Left motor and goes counter clockwise) 4 is all motors |
| | | |Value|U8|0x00 is disabled, 0xFF is enabled |
|MC_ResetPosition|5 (0x5)|| | |
| | | |Motor_ID|U8|The motor id from 0 - 4 (Motor 0 is Front Left motor and goes counter clockwise) 4 is all motors |
|MC_Spin|6 (0x6)|| | |
| | | |Motor_ID|U8|The motor id from 0 - 4 (Motor 0 is Front Left motor and goes counter clockwise) 4 is all motors |
| | | |Spin_Type|U8||
|MC_PowerBoost|7 (0x7)|| | |
| | | |Motor_ID|U8|The motor id from 0 - 4 (Motor 0 is Front Left motor and goes counter clockwise) 4 is all motors |
| | | |Value|U8|0x00 is On, 0xFF is Off |
|MC_SetParameter|8 (0x8)|| | |
| | | |ParamSelect|MC_ParameterSelection| Change internal parameters of the module |
| | | |New_Value|U32| The new value to be used in place |
|MC_GetParameters|9 (0x9)|Returns all current parameters of the module| | |
|MC_UpdateTelemetry|16 (0x10)|Forces a telemetry update from the motors| | |
|MC_SelfTest|153 (0x99)|Runs through a routine to confirms the MSP are functioning properly| | |

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
|MC_ParameterChanged|5 (0x5)|A notification that an MC parameter has been modified| | | | |
|MC_EncoderReset|5 (0x5)|A notification that an MC parameter has been modified| | | | |
