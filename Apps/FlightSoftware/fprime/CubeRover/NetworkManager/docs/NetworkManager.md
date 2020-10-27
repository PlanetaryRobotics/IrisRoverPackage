<title>NetworkManager Component Dictionary</title>
# NetworkManager Component Dictionary


## Telemetry Channel List

|Channel Name|ID|Type|Description|
|---|---|---|---|
|RSSI|0 (0x0)|I8|Received Signal Strength Indication abt WF121 connection to AP in dB|
|SNR|1 (0x1)|I8|Signal to noise radio|
|pkt_recv|2 (0x2)|U32|Number of packets received (uplink)|
|pkt_sent|3 (0x3)|U32|Number of packets sent (downlink)|

## Event List

|Event Name|ID|Description|Arg Name|Arg Type|Arg Size|Description
|---|---|---|---|---|---|---|
|state_change|0 (0x0)|Emit event when ever a state transition occurs within the WF121| | | | |
| | | |from_state|networkmanager_state_from|||
| | | |to_state|networkmanager_state_to|||
|GroundInterfaceError|0 (0x0)|An error in the ground interface, here to make the UTs happy.| | | | |
