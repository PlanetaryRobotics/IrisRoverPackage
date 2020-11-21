<title>IMU Component Dictionary</title>
# IMU Component Dictionary


## Command List

|Mnemonic|ID|Description|Arg Name|Arg Type|Comment
|---|---|---|---|---|---|
|Imu_ReportData|0 (0x0)|A command to force an IMU data.| | |

## Telemetry Channel List

|Channel Name|ID|Type|Description|
|---|---|---|---|
|X_Acc|0 (0x0)|I32|Accelerometer X|
|Y_Acc|1 (0x1)|I32|Accelerometer Y|
|Z_Acc|2 (0x2)|I32|Accelerometer Z|
|X_Ang|3 (0x3)|I32|Gyrometer X|
|Y_Ang|4 (0x4)|I32|Gyrometer Y|
|Z_Ang|5 (0x5)|I32|Gyrometer Z|

## Event List

|Event Name|ID|Description|Arg Name|Arg Type|Arg Size|Description
|---|---|---|---|---|---|---|
|Imu_AngleWarning|0 (0x0)|A notification that the rover is close to tipping point| | | | |
|Imu_CommunicationFailure|1 (0x1)|A notification that a communication has failed| | | | |
