<title>GroundInterface Component Dictionary</title>
# GroundInterface Component Dictionary


## Command List

|Mnemonic|ID|Description|Arg Name|Arg Type|Comment
|---|---|---|---|---|---|
|Set_Primary_Interface|0 (0x0)|Sets the primary interface.| | |
| | | |primary_interface|PrimaryInterface||
|Set_GroundInterface_Telemetry_Level|1 (0x1)|Sets the telemetry level of this component| | |
| | | |telemetry_level|TelemetryLevel||

## Telemetry Channel List

|Channel Name|ID|Type|Description|
|---|---|---|---|
|GI_UplinkSeqNum|0 (0x0)|U8|Increment the uplink sequence number every packet|
|GI_DownlinkSeqNum|1 (0x1)|U8|Update the downlink sequence number every packet|
|GI_PacketsReceived|2 (0x2)|U32|Number of packets received from link to F'|
|GI_PacketsTransmitted|3 (0x3)|U32|Number of packets transmitted from F' to link|
|GI_TlmItemsReceived|4 (0x4)|U32|Number of telemetry items received and queued|
|GI_TlmItemsDownlinked|5 (0x5)|U32|Number of telemetry items downlinked|
|GI_LogsReceived|6 (0x6)|U32|Number of logs received and queued|
|GI_LogsDownlinked|7 (0x7)|U32|Number of logs downlinked|
|GI_CmdsUplinked|8 (0x8)|U32|Number of commands uplinked.|
|GI_CmdsSent|9 (0x9)|U32|Commands sent to Command Dispatcher queue.|
|GI_UplinkPktErrs|10 (0xa)|U32|Number of uplinked packets with errors|
|GI_AppBytesReceived|11 (0xb)|U32|Number of bytes from packets from components not TlmChan, ComLogger received|
|GI_AppBytesDownlinked|12 (0xc)|U32|Number of bytes from packets from components not TlmChan, ComLogger downlinked|

## Event List

|Event Name|ID|Description|Arg Name|Arg Type|Arg Size|Description
|---|---|---|---|---|---|---|
|GI_UplinkedPacketError|0 (0x0)|Error validating uplinked packet. Expected field is only logged for MISMATCHED_LENGTH and OUT_OF_SEQUENCE.| | | | |
| | | |errorType|uplinkedPacketError|||
| | | |expected|U16|||
| | | |got|U16|||
|GI_UplinkedBadMagic|1 (0x1)|Error validating uplinked packet. The first payload in the variable length payload section had an incorrect magic.| | | | |
| | | |magicReceived|U32|||
|GI_CommandReceived|2 (0x2)|A command packet was received. One command per packet (and datagram) is strictly enforced| | | | |
| | | |seq|U8|||
| | | |checksum|U16|||
|GI_DownlinkedPacket|3 (0x3)|A packet sent to UDP receiver for downlink| | | | |
| | | |seq|U8|||
| | | |checksum|U16|||
| | | |length|U16|||
|GI_DownlinkedItem|4 (0x4)|An item was added to the packet for downlink| | | | |
| | | |seq|U8|||
| | | |from|downlinkPacketType|||
