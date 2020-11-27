<title>UdpInterface Component Dictionary</title>
# UdpInterface Component Dictionary


## Telemetry Channel List

|Channel Name|ID|Type|Description|
|---|---|---|---|
|UR_PacketsReceived|0 (0x0)|U32|Number of packets received|
|UR_BytesReceived|1 (0x1)|U32|Number of bytes received|
|UR_PacketsDropped|2 (0x2)|U32|Number of packets dropped|
|UR_DecodeErrors|3 (0x3)|U32|Number of packet decode errors|

## Event List

|Event Name|ID|Description|Arg Name|Arg Type|Arg Size|Description
|---|---|---|---|---|---|---|
|UR_PortOpened|0 (0x0)|Connection opened| | | | |
| | | |port|U32|||
|UR_RecvError|1 (0x1)|UDP receive error| | | | |
| | | |error|I32|||
|UR_DecodeError|2 (0x2)|Port decode error| | | | |
| | | |stage|DecodeStage|||
| | | |error|I32|||
|UR_DroppedPacket|5 (0x5)|Dropped packet was detected| | | | |
| | | |checksum|U32|||
