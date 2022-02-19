<title>NetworkManager Component Dictionary</title>
# NetworkManager Component Dictionary


## Telemetry Channel List

|Channel Name|ID|Type|Description|
|---|---|---|---|
|RSSI|0 (0x0)|I8|Received Signal Strength Indication abt WF121 connection to AP in dB|
|SNR|1 (0x1)|I8|Signal to noise radio|
|PktRecv|2 (0x2)|U32|Number of packets received (uplink)|
|PktSent|3 (0x3)|U32|Number of packets sent (downlink)|
|WIFIErrorStatus|4 (0x4)|U16|Returned Error code of WIFI chip|
|WIFIStateStatus|5 (0x5)|WIFIState|Returned State Transition code of WIFI chip|

## Event List

|Event Name|ID|Description|Arg Name|Arg Type|Arg Size|Description
|---|---|---|---|---|---|---|
|StateChange|0 (0x0)|Emit event when ever a state transition occurs within the WF121| | | | |
| | | |from_state|networkmanager_state_from|||
| | | |to_state|networkmanager_state_to|||
|ExecuteCallbackError|1 (0x1)|Emit event when there is an error executing a WF121 callback| | | | |
| | | |ErrorCode|U16|||
|WF121InitializationFailed|2 (0x2)|Emit event when there is an error executing a WF121 callback| | | | |
