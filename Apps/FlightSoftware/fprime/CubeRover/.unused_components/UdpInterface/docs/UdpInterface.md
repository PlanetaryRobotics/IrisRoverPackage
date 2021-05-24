<title>UdpInterface Component Dictionary</title>
# UdpInterface Component Dictionary


## Telemetry Channel List

|Channel Name|ID|Type|Description|
|---|---|---|---|
|UR_PacketsSent|0 (0x0)|U32|Number of packetssent|
|UR_BytesSent|1 (0x1)|U32|Number of bytes sent|
|UR_PacketsReceived|2 (0x2)|U32|Number of packets received|
|UR_BytesReceived|3 (0x3)|U32|Number of bytes received|
|UR_PacketsDropped|4 (0x4)|U32|Number of packets dropped|
|UR_DecodeErrors|5 (0x5)|U32|Number of packet decode errors|

## Event List

|Event Name|ID|Description|Arg Name|Arg Type|Arg Size|Description
|---|---|---|---|---|---|---|
|UR_SendError|0 (0x0)|UDP send error| | | | |
| | | |error|I32|||
|UR_RecvError|1 (0x1)|UDP receive error| | | | |
| | | |error|I32|||
|UR_DecodeError|2 (0x2)|Port decode error| | | | |
| | | |stage|DecodeStage|||
| | | |error|I32|||
|UR_DroppedPacket|5 (0x5)|Dropped packet was detected| | | | |
| | | |checksum|U32|||
