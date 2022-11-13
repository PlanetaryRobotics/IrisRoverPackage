<title>NetworkManager Component Dictionary</title>
# NetworkManager Component Dictionary


## Command List

|Mnemonic|ID|Description|Arg Name|Arg Type|Comment
|---|---|---|---|---|---|
|Set_Radio_Communications_Mode|0 (0x0)|Sets the Radio communications mode. For Radio debugging and UART/DFU programming.| | |
| | | |mode|nm_radio_communications_mode||
|Set_Radio_Uart_Baud|1 (0x1)|Sets the (peristent) baud rate for UART communication with the 
            WF121 Radio. Note: if Radio-Hercules comms appear not to work 
            properly after issuing this command, you may have to reboot. 
            
            A `RadioUartBaudRateChange` event is emitted when 
            called (if no change was made, `changeMade=FALSE`).| | |
| | | |newBaud|U32||
|Set_Radio_BgApi_Passthrough|2 (0x2)|If TRUE: 
                Turns off the state machine in Hercules managing UART BGAPI 
                communications with the WF121 Radio and begins forwarding all 
                BGAPI responses and events to Ground via the 
                `WatchDog DebugPrintf` pipeline. 
                During this time, the `Send_BgApi_Command` CAN be used to send 
                BGAPI commands to the Radio.

                If FALSE: 
                Turns back on and resets the state machine in Hercules managing 
                UART BGAPI communications with the WF121 Radio and stops 
                forwarding all BGAPI responses and events to Ground via the 
                `WatchDog DebugPrintf` pipeline. 
                During this time, the `Send_BgApi_Command` CANNOT be used to 
                send BGAPI commands to the Radio.

                Changes are only made if the passthrough mode is being changed. 
            
                A `RadioBgApiPassthroughChange` event is emitted when 
                called (if no change was made, `changeMade=FALSE`).| | |
| | | |passthrough|bool||
|Send_BgApi_Command|3 (0x3)|Forwards the given BGAPI command (packed as binary) to the 
                Radio. 
                A `RadioSendBgApiCommandAck` event is emitted when this command is received| | |
| | | |crc32|U32|
                        CRC32 of the packed BGAPI packet, as a uint32.
                    |
| | | |packetId|U32|
                        ID of the packet, assigned by ground. This is just 
                        included in the response event so ground can know what 
                        packet to resend if it needs to resend a packet.
                    |
| | | |expectResponse|nm_radio_send_bgapi_command_expect_response|
                        Whether or not we should expect (and wait for) a BGAPI 
                        response from the Radio after sending this command 
                        (certain BGAPI DFU flashing operations don't return a 
                        response). Setting this correctly will ensure fast and
                        reliable data transfers. Using an enum here instead of
                        a bool because it's easier to detect corruption with
                    |
| | | |bgapiPacket|Fw::IrisCmdByteStringArg|
                        The data as a 'byte string', with a MAX length of 134B 
                        (4B of BGAPI header + 1B 'BGAPI uint8array' length byte 
                        + 128B of data + 1B null termination). To increase this 
                        limit, you'll likely need to bump up 
                        'FW_COM_BUFFER_MAX_SIZE' and 'FW_CMD_STRING_MAX_SIZE' 
                        in 'fprime/Fw/Cfg/Config.hpp' (read the notes there for 
                        more information about required padding). Note that 
                        increasing the max string size has a pretty big effect 
                        on the total program size in memory.
 
 
                        Here string here just means a null terminated char 
                        array. To be specific, the NULL termination is EXCLUDED 
                        from the length and does NOT need to be (and should not 
                        be) included in the data sent. That is, if length is 3, 
                        the data sent would be [0x00, 0x03, byte0, byte1, byte2] 
                        and the memory in the `CmdStringArg->m_buf` inside 
                        Hercules would look like: [byte0, byte1, byte2, NULL].
                    |
|Downlink_BgApi_Command_Records|4 (0x4)|Triggers a `RadioBgApiCommandRecords` event to see what BgApi
                packets have been processed recently and what the outcomes were.| | |

## Telemetry Channel List

|Channel Name|ID|Type|Description|
|---|---|---|---|
|RSSI|0 (0x0)|I8|Received Signal Strength Indication abt WF121 connection to AP in dB|
|SNR|1 (0x1)|I8|Signal to noise radio|
|PktRecv|2 (0x2)|U32|Number of packets received (uplink)|
|PktSent|3 (0x3)|U32|Number of packets sent (downlink)|
|WIFIErrorStatus|4 (0x4)|WIFIErrorCode|Returned Error code of WIFI chip|
|WIFIStateStatus|5 (0x5)|WIFIState|Returned State Transition code of WIFI chip|
|UartBytesReceived|6 (0x6)|U32|Number of raw bytes received over UART from the Radio (valid or invalid - used for diagnostics).|

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
|RadioUartBaudRateChange|5 (0x5)|Fired in response to a `Set_Radio_Uart_Baud` command.
                `fromBaud` will always be the baud rate before the command. 
                `toBaud` will always be the actual baud rate after running this command. 
                `changeMade` indicates whether the change was actually 
                (successfully) changed. It can be false because either the 
                change was not able to be made or because it didn't need to be 
                made (requested baud rate is the current baud rate). 
 
                This event will also be fired once upon boot with 
                `changeMade=FALSE` and `fromBaud=toBaud` to indicate the 
                current value of the persistent baud rate.| | | | |
| | | |changeMade|bool|||
| | | |fromBaud|U32|||
| | | |toBaud|U32|||
|RadioBgApiPassthroughChange|6 (0x6)|Fired in response to a `Set_Radio_BgApi_Passthrough` command. 
                `from` will always be the state before the command. 
                `to` will always be the actual state after running this command. 
                `changeMade` indicates whether the change was actually 
                (successfully) changed. It can be false because either the 
                change was not able to be made or because it didn't need to be 
                made (requested state is the current state). 

                This event will also be fired once upon boot with 
                `changeMade=FALSE` and `from=to` to indicate the 
                current value of the passthrough state.| | | | |
| | | |changeMade|bool|||
| | | |from|bool|||
| | | |to|bool|||
|RadioSendBgApiCommandAck|7 (0x7)|Fired in response to a `Send_BgApi_Command` command.

                Not actually a warning but sent using the `WARNING_LO` queue 
                because it has high importance, a (comparatively) large buffer, 
                and not many events use the `WARNING_LO` queue.

                NOTE: CRC values will only be populated if command FAILED
                validation (for any reason, including CRC failure). If status
                indicates there was no validation error, both CRCs will be
                `0xFF'FF'FF'FF`.| | | | |
| | | |packetId|U32||Packet ID given from ground.|
| | | |targetCrc32|U32||Uplinked CRC32 (as received), or `0xFF'FF'FF'FF` if no validation error.|
| | | |computedCrc32|U32||CRC32 of the `bgapiPacket` received, as a uint32, or `0xFF'FF'FF'FF` if no validation error.|
| | | |status|nm_radio_send_bgapi_command_ack_status||Status of the `Send_BgApi_Command`.|
|RadioBgApiCommandRecords|8 (0x8)|Fired in response to a `Downlink_BgApi_Command_Records` command. 
                Downlinks the GSW-Assigned `PacketId` of the last 3 
                `Send_BgApi_Command` s processed by the NetworkManager as well 
                as what the resulting status of that downlink was (used for 
                ensuring all packets in a programming sequence are uplinked 
                correctly and in the correct order).
 
                Technically it's a bit hacky to just have three args with 
                manually synced enums and, instead, this should be using an 
                FPrime Array of custom serializables but we don't have that 
                serialization built in our GDS and don't have time to add it 
                since it would be non-trivial.
 
                Not actually a warning but sent using the `WARNING_LO` queue 
                because it has high importance, a (comparatively) large buffer, 
                and not many events use the `WARNING_LO` queue.| | | | |
| | | |packetId0|U32||GSW-Assigned Packet ID of the most recently processed BgApi Packet.|
| | | |result0|nm_radio_rec0_bgapi_command_ack_status||Result of processing the most recently processed BgApi Packet.
                        NOTE: This enum needs to EXACTLY match 
                        `nm_radio_send_bgapi_command_ack_status` in `status` of 
                        `RadioSendBgApiCommandAck` (with the exception of 
                        `_EMPTY_RECORD` which is used to indicate
                        that the record is empty).|
| | | |packetId1|U32||GSW-Assigned Packet ID of the 2nd most recently processed BgApi Packet.|
| | | |result1|nm_radio_rec1_bgapi_command_ack_status||Result of processing the 2nd most recently processed BgApi Packet.
                        NOTE: This enum needs to EXACTLY match 
                        `nm_radio_send_bgapi_command_ack_status` in `status` of 
                        `RadioSendBgApiCommandAck` (with the exception of 
                        `_EMPTY_RECORD` which is used to indicate
                        that the record is empty).|
| | | |packetId2|U32||GSW-Assigned Packet ID of the 3rd most recently processed BgApi Packet.|
| | | |result2|nm_radio_rec2_bgapi_command_ack_status||Result of processing the 3rd most recently processed BgApi Packet.
                        NOTE: This enum needs to EXACTLY match 
                        `nm_radio_send_bgapi_command_ack_status` in `status` of 
                        `RadioSendBgApiCommandAck` (with the exception of 
                        `_EMPTY_RECORD` which is used to indicate
                        that the record is empty).|
