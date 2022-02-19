<title>ComLogger Component Dictionary</title>
# ComLogger Component Dictionary


## Command List

|Mnemonic|ID|Description|Arg Name|Arg Type|Comment
|---|---|---|---|---|---|
|CloseFile|0 (0x0)|Forces a close of the currently opened file.| | |
|SendLog|1 (0x1)|Sends a set of logs from flash to Ground| | |
| | | |prefix|Fw::CmdStringArg|The prefix for a flash saved file|
| | | |time|U32|The time (seconds) for when a file was opened|

## Telemetry Channel List

|Channel Name|ID|Type|Description|
|---|---|---|---|
|TOTAL_BYTES_READ|0 (0x0)|U32|Number of bytes read from flash|
|TOTAL_BYTES_WRITTEN|1 (0x1)|U32|Number of bytes written to flash|
|CUR_FILE_BYTES|2 (0x2)|U32|Number of bytes saved in current file|
|CUR_FILE_TIME|3 (0x3)|U32|Time in seconds of current file|
|CUR_FILE_TYPE|4 (0x4)|U8|Type of current file (sent as U8 enum rep)|
|CUR_FILE_STATUS|5 (0x5)|bool|If file is open or closed currently (0 = closed, 1 = open)|

## Event List

|Event Name|ID|Description|Arg Name|Arg Type|Arg Size|Description
|---|---|---|---|---|---|---|
|FileOpenError|0 (0x0)|The ComLogger encountered an error opening a file| | | | |
| | | |errornum|U32||The error number returned from open file|
| | | |file|Fw::LogStringArg&|240|The file|
|FileWriteError|1 (0x1)|The ComLogger encountered an error writing to a file| | | | |
| | | |errornum|U32||The error number returned from write file|
| | | |bytesWritten|U32||The number of bytes successfully written to file|
| | | |bytesToWrite|U32||The number of bytes attempted to write to file|
| | | |file|Fw::LogStringArg&|240|The file|
|FileValidationError|2 (0x2)|The ComLogger encountered an error writing the validation file| | | | |
| | | |validationFile|Fw::LogStringArg&|240|The validation file|
| | | |file|Fw::LogStringArg&|240|The file|
| | | |status|U32||The Os::Validate::Status return|
|FileClosed|3 (0x3)|The ComLogger successfully closed a file on command.| | | | |
| | | |file|Fw::LogStringArg&|240|The file|
|TimeNotAvaliable|4 (0x4)|Time sent by ground not currently stored in memory| | | | |
| | | |start|U32||The start time requested by ground|
| | | |end|U32||The end time requested by ground|
|FileReadError|5 (0x5)|The ComLogger encountered an error reading a file| | | | |
| | | |errornum|U32||The error number returned from write file|
| | | |bytesReading|U32||The number of bytes to read|
| | | |file|Fw::LogStringArg&|240|The file|
