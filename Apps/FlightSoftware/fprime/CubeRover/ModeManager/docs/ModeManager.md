<title>ModeManager Component Dictionary</title>
# ModeManager Component Dictionary


## Command List

|Mnemonic|ID|Description|Arg Name|Arg Type|Comment
|---|---|---|---|---|---|
|GOTO_MODE|0 (0x0)|Goto a specifified mode| | |
| | | |Mode_cmd|OpModeCmd||


## Event List

|Event Name|ID|Description|Arg Name|Arg Type|Arg Size|Description
|---|---|---|---|---|---|---|
|ModeChanged|0 (0x0)|Rover Mode Changed| | | | |
| | | |past_mode|OpModeEvPast|||
| | | |current_mode|CurrOpModeEvCurr|||
|UnexpectedMode|1 (0x1)|Rover Attempted Unknown Mode| | | | |
| | | |unknown_mode|U32|||
|FatalTriggered|2 (0x2)|Fatal Port Triggered| | | | |
