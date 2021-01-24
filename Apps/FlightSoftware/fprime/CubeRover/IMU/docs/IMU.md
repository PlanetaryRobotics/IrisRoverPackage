<title>IMU Component Dictionary</title>
# IMU Component Dictionary


## Command List

|Mnemonic|ID|Description|Arg Name|Arg Type|Comment
|---|---|---|---|---|---|
|IMU_ReportData|0 (0x0)|A command to force an IMU data.| | |
|IMU_TurnOn|1 (0x1)|A command to turn on the hardware| | |
|IMU_TurnOff|2 (0x2)|A command to turn off the hardware| | |

## Telemetry Channel List

|Channel Name|ID|Type|Description|
|---|---|---|---|
|X_Acc|0 (0x0)|I16|Accelerometer X|
|Y_Acc|1 (0x1)|I16|Accelerometer Y|
|Z_Acc|2 (0x2)|I16|Accelerometer Z|
|X_Ang|3 (0x3)|U16|Gyrometer X|
|Y_Ang|4 (0x4)|U16|Gyrometer Y|
|Z_Ang|5 (0x5)|U16|Gyrometer Z|

## Event List

|Event Name|ID|Description|Arg Name|Arg Type|Arg Size|Description
|---|---|---|---|---|---|---|
|IMU_AngleWarning|0 (0x0)|A notification that the rover is close to tipping point| | | | |
|IMU_CommunicationFailure|1 (0x1)|A notification that a communication has failed| | | | |
|IMU_Powered_ON|2 (0x2)|A notification that the IMU is powered on| | | | |
|IMU_Powered_OFF|3 (0x3)|A notification that the IMU is powered off| | | | |
