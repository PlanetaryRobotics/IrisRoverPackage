<title>NetworkManager Component Dictionary</title>
# NetworkManager Component Dictionary


## Telemetry Channel List

|Channel Name|ID|Type|Description|
|---|---|---|---|
|NM_PacketsReceived|0 (0x0)|U32|Number of packets received|
|NM_PacketsTransmitted|1 (0x1)|U32|Number of packets transmitted|
|NM_Rssi|2 (0x2)|U32|RSSI level|
|NM_BytesReceived|3 (0x3)|U32|Total number of bytes received|
|NM_BytesTransmitted|4 (0x4)|U32|Total number of bytes transmitted|
|NM_DecodeErrors|5 (0x5)|U32|Total number of packet decode errors|

## Event List

|Event Name|ID|Description|Arg Name|Arg Type|Arg Size|Description
|---|---|---|---|---|---|---|
|NM_NetworkLevelChange|0 (0x0)|Network level change notification| | | | |
| | | |level|U8|||
|NM_PacketDecodingError|1 (0x1)|Error during packet decoding| | | | |
| | | |totalDecodingErrors|U32|||
|NM_WifiModuleError|2 (0x2)|Error during communication with wifi module| | | | |
| | | |errorCode|U16|||
