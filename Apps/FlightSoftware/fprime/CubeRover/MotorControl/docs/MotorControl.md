<title>MotorControl Component Dictionary</title>
# MotorControl Component Dictionary


## Command List

|Mnemonic|ID|Description|Arg Name|Arg Type|Comment
|---|---|---|---|---|---|
|MC_UpdateTelemetry|0 (0x0)|Forces a telemetry update from the motors:
                REG_I2C_ADDRESS         = 0,    // RO - uint8   1 Byte
                REG_TARGET_POSITION     = 1,    // RW - int32   4 Bytes
                REG_TARGET_SPEED        = 2,    // RW - uint8   1 Byte
                REG_CURRENT_POSITION    = 3,    // RO - int32   4 Bytes
                REG_CURRENT_SPEED       = 4,    // RO - uint8   1 Bytes
                REG_MOTOR_CURRENT       = 5,    // RO - int32   4 Bytes
                REG_P_CURRENT           = 6,    // RW - uint16  2 Bytes
                REG_I_CURRENT           = 7,    // RW - uint16  2 Bytes
                REG_P_SPEED             = 8,    // RW - uint16  2 Bytes
                REG_I_SPEED             = 9,    // RW - uint16  2 Bytes
                REG_ACC_RATE            = 10,   // RW - uint16  2 Bytes
                REG_DEC_RATE            = 11,   // RW - uint16  2 Bytes
                REG_MC_CTRL             = 12,   // RW - uint8   1 Byte
                REG_MC_STATUS           = 13,   // RO - uint8   1 Byte
                REG_MC_FAULT            = 14,   // RO - uint8   1 Byte| | |
|MC_SetAllParams|16 (0x10)|Sets ALL RW params for EVERY motor
                TargetDir : 0 Positive Position, 1 Negative Position
                REG_TARGET_POSITION     = 1,    // int32
                REG_TARGET_SPEED        = 2,    // uint8
                REG_P_CURRENT           = 6,    // uint16
                REG_I_CURRENT           = 7,    // uint16
                REG_P_SPEED             = 8,    // uint16
                REG_I_SPEED             = 9,    // uint16
                REG_ACC_RATE            = 10,   // uint16
                REG_DEC_RATE            = 11,   // uint16
                ( REG_MC_CTRL = MC_CMD_UPDATE_CONFIG )| | |
| | | |MotorA_TargetDir|U8||
| | | |MotorA_TargetPosition|U32||
| | | |MotorA_TargetSpeed|U8||
| | | |MotorA_Current_P_Val|U16||
| | | |MotorA_Current_I_Val|U16||
| | | |MotorA_Speed_P_Val|U16||
| | | |MotorA_Speed_I_Val|U16||
| | | |MotorA_Accel_Rate|U16||
| | | |MotorA_Decel_Rate|U16||
| | | |MotorB_TargetDir|U8||
| | | |MotorB_TargetPosition|U32||
| | | |MotorB_TargetSpeed|U8||
| | | |MotorB_Current_P_Val|U16||
| | | |MotorB_Current_I_Val|U16||
| | | |MotorB_Speed_P_Val|U16||
| | | |MotorB_Speed_I_Val|U16||
| | | |MotorB_Accel_Rate|U16||
| | | |MotorB_Decel_Rate|U16||
| | | |MotorC_TargetDir|U8||
| | | |MotorC_TargetPosition|U32||
| | | |MotorC_TargetSpeed|U8||
| | | |MotorC_Current_P_Val|U16||
| | | |MotorC_Current_I_Val|U16||
| | | |MotorC_Speed_P_Val|U16||
| | | |MotorC_Speed_I_Val|U16||
| | | |MotorC_Accel_Rate|U16||
| | | |MotorC_Decel_Rate|U16||
| | | |MotorD_TargetDir|U8||
| | | |MotorD_TargetPosition|U32||
| | | |MotorD_TargetSpeed|U8||
| | | |MotorD_Current_P_Val|U16||
| | | |MotorD_Current_I_Val|U16||
| | | |MotorD_Speed_P_Val|U16||
| | | |MotorD_Speed_I_Val|U16||
| | | |MotorD_Accel_Rate|U16||
| | | |MotorD_Decel_Rate|U16||
|MC_SetParameter|17 (0x11)|Update single parameter of selected motor controller(s).| | |
| | | |Motor_ID|U8|Bitmask of motors to update.
                        MotorA    : 0x01
                        MotorB    : 0x02
                        MotorC    : 0x04
                        MotorD    : 0x08
                        (eg. AllMotors = MotorA + MotorB + MotorC + MotorD = 0x0F)
                    |
| | | |Param_RegAddr|U8|The register address of the parameter to set.
                        Target_Dir              = 16    // bool
                        REG_TARGET_POSITION     = 1,    // int32
                        REG_TARGET_SPEED        = 2,    // uint8
                        REG_P_CURRENT           = 6,    // uint16
                        REG_I_CURRENT           = 7,    // uint16
                        REG_P_SPEED             = 8,    // uint16
                        REG_I_SPEED             = 9,    // uint16
                        REG_ACC_RATE            = 10,   // uint16
                        REG_DEC_RATE            = 11,   // uint16
                        ( REG_MC_CTRL = MC_CMD_UPDATE_CONFIG )
                    |
| | | |Param_NewValue|U32| The new value of the parameter. 
                        (Registers will be type-cast from U32)
                    |
|MC_Spin|32 (0x20)|The new value of the parameter.| | |
| | | |Motor_ID|U8|Bitmask of motors to update.
                        MotorA : 0x01
                        MotorB : 0x02
                        MotorC : 0x04
                        MotorD : 0x08
                        (eg. "Left"-side motors = MotorA + MotorD = 0x09
                        and "Right"-side motors = MotorB + MotorC = 0x06)
                    |
| | | |Dir|U8|Direction motor movement
                        Positive Ticks : 0 , Negative Ticks : 1
                    |
| | | |Raw_Ticks|U32|Distance to spin in ticks.
                        (Default = 20000)
                    |
|MC_Spin_Configured|33 (0x21)|The new value of the parameter.| | |
| | | |Motor_ID|U8|Bitmask of motors to update.
                        MotorA : 0x01
                        MotorB : 0x02
                        MotorC : 0x04
                        MotorD : 0x08
                        (eg. "Left"-side motors = MotorA + MotorD = 0x09
                        and "Right"-side motors = MotorB + MotorC = 0x06)
                    |
| | | |Dir|U8|Bitmask of direction of each motor
                        Positive Ticks : 0 , Negative Ticks : 1
                    |
| | | |Raw_Ticks|U32|Distance to spin in ticks.
                      (Default = 20000)
                    |
| | | |Percent_Speed|U8|Speed set by percent of MAX_SPEED.
                      (Default = 70)
                    |

## Telemetry Channel List

|Channel Name|ID|Type|Description|
|---|---|---|---|
|MC_FL_Encoder_Ticks|0 (0x0)|I32|Front Left motor encoder displacement in encoder ticks|
|MC_FL_Current|1 (0x1)|U16|Front Left motor current in milliAmps|
|MC_FR_Encoder_Ticks|3 (0x3)|I32|Front Right motor encoder displacement in encoder ticks|
|MC_FR_Current|4 (0x4)|U16|Front Right motor current in milliAmps|
|MC_RR_Encoder_Ticks|6 (0x6)|I32|Rear Right motor encoder displacement in encoder ticks|
|MC_RR_Current|7 (0x7)|U16|Rear Right motor current in milliAmps|
|MC_RL_Encoder_Ticks|9 (0x9)|I32|Rear Left motor encoder displacement in encoder ticks|
|MC_RL_Current|10 (0xa)|U16|Rear Left motor current in milliAmps|
|MC_FL_Status|11 (0xb)|U8|Front left status|
|MC_FR_Status|12 (0xc)|U8|Front right status|
|MC_RR_Status|13 (0xd)|U8|Rear right status|
|MC_RL_Status|14 (0xe)|U8|Rear left status|

## Event List

|Event Name|ID|Description|Arg Name|Arg Type|Arg Size|Description
|---|---|---|---|---|---|---|
|MC_moveStarted|0 (0x0)|A notification a move sequence has started| | | | |
|MC_moveComplete|1 (0x1)|A notification a move sequence has finished| | | | |
|MC_MSPFaultDetected|2 (0x2)|A notification an MSP has thrown a fault| | | | |
|MC_MSPNotResponding|3 (0x3)|A notification an MSP is not communicating| | | | |
|MC_MSPNotReporting|4 (0x4)|A notification an MSP is not reporting data| | | | |
|MC_ParameterChanged|5 (0x5)|A notification that an MC parameter has been modified| | | | |
|MC_EncoderReset|6 (0x6)|A notification that an MC parameter has been modified| | | | |
