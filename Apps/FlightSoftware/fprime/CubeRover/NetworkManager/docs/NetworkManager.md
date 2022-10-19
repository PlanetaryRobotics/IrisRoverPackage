<title>NetworkManager Component Dictionary</title>
# NetworkManager Component Dictionary


## Command List

|Mnemonic|ID|Description|Arg Name|Arg Type|Comment
|---|---|---|---|---|---|
|Set_Radio_Communications_Mode|0 (0x0)|Sets the Radio communications mode. For Radio debugging and UART/DFU programming.| | |
| | | |mode|nm_radio_communications_mode||

## Telemetry Channel List

|Channel Name|ID|Type|Description|
|---|---|---|---|
|RSSI|0 (0x0)|I8|Received Signal Strength Indication abt WF121 connection to AP in dB|
|SNR|1 (0x1)|I8|Signal to noise radio|
|PktRecv|2 (0x2)|U32|Number of packets received (uplink)|
|PktSent|3 (0x3)|U32|Number of packets sent (downlink)|
|WIFIErrorStatus|4 (0x4)|WIFIErrorCode|Returned Error code of WIFI chip|
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
|RadioCommunicationsModeChange|3 (0x3)|Emit event whenever the radio communications mode is changed.| | | | |
| | | |from_mode|nm_radio_communications_mode_from|||
| | | |to_mode|nm_radio_communications_mode_to|||
|RadioCommunicationsModeState|4 (0x4)|Current state of the Radio Communications mode. Emitted anytime a change is requested, even if a change isn't performed because the radio is already in the desired mode.| | | | |
| | | |mode|nm_radio_communications_mode_now|||
