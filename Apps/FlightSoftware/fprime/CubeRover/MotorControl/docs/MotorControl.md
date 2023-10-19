<title>MotorControl Component Dictionary</title>
# MotorControl Component Dictionary


## Command List

|Mnemonic|ID|Description|Arg Name|Arg Type|Comment
|---|---|---|---|---|---|
|MC_Current_PID|0 (0x0)|| | |
| | | |Motor_ID|U8|The motor id from 0 - 4 Motor 0 is all motors, Motorsss 1 - 4 are FL, FR, BL, BR |
| | | |PI_Values|U32||
|MC_Speed_PID|1 (0x1)|| | |
| | | |Motor_ID|U8|The motor id from 0 - 4 Motor 0 is all motors, Motorsss 1 - 4 are FL, FR, BL, BR |
| | | |PID_Values|U32||
|MC_Acceleration|3 (0x3)|| | |
| | | |Motor_ID|U8|The motor id from 0 - 4 Motor 0 is all motors, Motorsss 1 - 4 are FL, FR, BL, BR |
| | | |Rate_Values|U32||
|MC_StallDetection|4 (0x4)|| | |
| | | |Motor_ID|U8|The motor id from 0 - 4 Motor 0 is all motors, Motorsss 1 - 4 are FL, FR, BL, BR |
| | | |Value|U8|0x00 is disabled, 0xFF is enabled |
|MC_ResetPosition|5 (0x5)|| | |
| | | |Motor_ID|U8|The motor id from 0 - 4 Motor 0 is all motors, Motorsss 1 - 4 are FL, FR, BL, BR |
|MC_Spin|6 (0x6)|| | |
| | | |Motor_ID|U8|The motor id from 0 - 4 Motor 0 is all motors, Motorsss 1 - 4 are FL, FR, BL, BR |
| | | |Raw_Ticks|U32||
|MC_PowerBoost|7 (0x7)|| | |
| | | |Motor_ID|U8|The motor id from 0 - 4 Motor 0 is all motors, Motorsss 1 - 4 are FL, FR, BL, BR |
| | | |Value|U8|0x00 is On, 0xFF is Off |
|MC_SetParameter|8 (0x8)|Update Parameters of a Motor Controller:
            REG_TARGET_POSITION     = 1,    // RW - 4 Bytes
            REG_TARGET_SPEED        = 2,    // RW - 1 Byte
            REG_P_CURRENT           = 6,    // RW - 2 Bytes
            REG_I_CURRENT           = 7,    // RW - 2 Bytes
            REG_P_SPEED             = 8,    // RW - 2 Bytes
            REG_I_SPEED             = 9,    // RW - 2 Bytes
            REG_ACC_RATE            = 10,   // RW - 2 Bytes
            REG_DEC_RATE            = 11,   // RW - 2 Bytes
            REG_MC_CTRL             = 12,   // RW - 1 Byte| | |
| | | |Motor_ID|U8|The motor id:
            Motor 0 is all motors
            Motors 1 - 4 are FL, FR, BL, BR
          |
| | | |Param_RegAddr|U8|The register address of the parameter to update.
          |
| | | |Param_NewValue|U32| The new value to send to the controller |
|MC_UpdateTelemetry|16 (0x10)|Forces a telemetry update from the motors:
        REG_I2C_ADDRESS         = 0,    // RO - 1 Byte
        REG_TARGET_POSITION     = 1,    // RW - 4 Bytes
        REG_TARGET_SPEED        = 2,    // RW - 1 Byte
        REG_CURRENT_POSITION    = 3,    // RO - 4 Bytes
        REG_CURRENT_SPEED       = 4,    // RO - 1 Bytes
        REG_MOTOR_CURRENT       = 5,    // RO - 4 Bytes
        REG_P_CURRENT           = 6,    // RW - 2 Bytes
        REG_I_CURRENT           = 7,    // RW - 2 Bytes
        REG_P_SPEED             = 8,    // RW - 2 Bytes
        REG_I_SPEED             = 9,    // RW - 2 Bytes
        REG_ACC_RATE            = 10,   // RW - 2 Bytes
        REG_DEC_RATE            = 11,   // RW - 2 Bytes
        REG_MC_CTRL             = 12,   // RW - 1 Byte
        REG_MC_STATUS           = 13,   // RO - 1 Byte
        REG_MC_FAULT            = 14,   // RO - 1 Byte| | |

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
