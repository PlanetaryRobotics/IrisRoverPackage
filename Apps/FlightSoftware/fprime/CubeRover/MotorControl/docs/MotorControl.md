<title>MotorControl Component Dictionary</title>
# MotorControl Component Dictionary


## Command List

|Mnemonic|ID|Description|Arg Name|Arg Type|Comment
|---|---|---|---|---|---|
|MC_LedCtrl|0 (0x0)|Control the led| | |
| | | |control|controlState||

## Telemetry Channel List

|Channel Name|ID|Type|Description|
|---|---|---|---|
|MotorControl_ledState|0 (0x0)|U8|The led status|

## Event List

|Event Name|ID|Description|Arg Name|Arg Type|Arg Size|Description
|---|---|---|---|---|---|---|
|MC_LedChangedState|0 (0x0)|This diagnostic event indicates the status of the led| | | | |
| | | |ledState|Fw::LogStringArg&|5|The led state|
