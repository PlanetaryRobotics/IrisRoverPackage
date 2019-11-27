<title>NeutronDetector Component Dictionary</title>
# NeutronDetector Component Dictionary


## Command List

|Mnemonic|ID|Description|Arg Name|Arg Type|Comment
|---|---|---|---|---|---|
|ND_CollectData|1 (0x1)|Collect the data on asynchronous command| | |

## Telemetry Channel List

|Channel Name|ID|Type|Description|
|---|---|---|---|
|ArrayOutput|0 (0x0)|string|Telemetry data output of the neutron detector, each byte represent a sensor data|

## Event List

|Event Name|ID|Description|Arg Name|Arg Type|Arg Size|Description
|---|---|---|---|---|---|---|
|ND_DeviceFailure|0 (0x0)|This diagnostic event indicates a failure with the neutron detector| | | | |
| | | |errorCode|Fw::LogStringArg&|5|The reported error code|
