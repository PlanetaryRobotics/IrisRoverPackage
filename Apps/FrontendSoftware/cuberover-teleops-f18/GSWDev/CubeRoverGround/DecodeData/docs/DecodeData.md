<title>DecodeData Component Dictionary</title>
# DecodeData Component Dictionary


## Command List

|Mnemonic|ID|Description|Arg Name|Arg Type|Comment
|---|---|---|---|---|---|
|DecodeData_Settings|0 (0x0)|DecodeData Settings| | |   
| | | |Frequency|U32||                    
| | | |Amplitude|U32||                    
| | | |Phase|U32||                    
|DecodeData_Toggle|1 (0x1)|Toggle DecodeData On/Off.| | |   
|DecodeData_Skip|2 (0x2)|Skip next sample| | |   

## Telemetry Channel List

|Channel Name|ID|Type|Description|
|---|---|---|---|
|DecodeData_Output|0 (0x0)|F32|DecodeData Output|

## Event List

|Event Name|ID|Description|Arg Name|Arg Type|Arg Size|Description
|---|---|---|---|---|---|---|
|DecodeData_SettingsChanged|0 (0x0)|DecodeData Settings Changed| | | | |
| | | |Frequency|U32|||    
| | | |Amplitude|U32|||    
| | | |Phase|U32|||    
