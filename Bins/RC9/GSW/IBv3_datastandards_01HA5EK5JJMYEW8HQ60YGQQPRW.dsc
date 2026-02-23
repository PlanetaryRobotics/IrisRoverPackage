{
"py/object": "IrisBackendv3.utils.nameiddict.NameIdDict",
"py/state": [
{
"names": [
"BlockDriver"
],
"ID": 256,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Module",
"py/state": {
"name": "BlockDriver",
"ID": 256,
"commands": {
"py/object": "IrisBackendv3.utils.nameiddict.NameIdDict",
"id_dict": {},
"name_id_mapping": {}
},
"telemetry": {
"py/object": "IrisBackendv3.utils.nameiddict.NameIdDict",
"py/state": [
{
"names": [
"BdCycles"
],
"ID": 0,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "BdCycles",
"ID": 0,
"type": {
"py/reduce": [
{
"py/type": "IrisBackendv3.data_standards.fsw_data_type.FswDataType"
},
{
"py/tuple": [
"uint32"
]
}
]
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
}
]
},
"events": {
"py/object": "IrisBackendv3.utils.nameiddict.NameIdDict",
"id_dict": {},
"name_id_mapping": {}
}
}
}
},
{
"names": [
"RateGroupDriver"
],
"ID": 512,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Module",
"py/state": {
"name": "RateGroupDriver",
"ID": 512,
"commands": {
"py/object": "IrisBackendv3.utils.nameiddict.NameIdDict",
"id_dict": {},
"name_id_mapping": {}
},
"telemetry": {
"py/object": "IrisBackendv3.utils.nameiddict.NameIdDict",
"id_dict": {},
"name_id_mapping": {}
},
"events": {
"py/object": "IrisBackendv3.utils.nameiddict.NameIdDict",
"id_dict": {},
"name_id_mapping": {}
}
}
}
},
{
"names": [
"ActiveRateGroup-RateGroupLowFreq"
],
"ID": 768,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Module",
"py/state": {
"name": "ActiveRateGroup-RateGroupLowFreq",
"ID": 768,
"commands": {
"py/object": "IrisBackendv3.utils.nameiddict.NameIdDict",
"id_dict": {},
"name_id_mapping": {}
},
"telemetry": {
"py/object": "IrisBackendv3.utils.nameiddict.NameIdDict",
"py/state": [
{
"names": [
"RgMaxTime"
],
"ID": 0,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "RgMaxTime",
"ID": 0,
"type": {
"py/id": 15
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"RgCycleSlips"
],
"ID": 1,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "RgCycleSlips",
"ID": 1,
"type": {
"py/id": 15
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
}
]
},
"events": {
"py/object": "IrisBackendv3.utils.nameiddict.NameIdDict",
"py/state": [
{
"names": [
"RateGroupStarted"
],
"ID": 0,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Event",
"py/state": {
"name": "RateGroupStarted",
"ID": 0,
"severity": "DIAGNOSTIC",
"format_string": "Rate group started.",
"args": [],
"metadata": {}
}
}
},
{
"names": [
"RateGroupCycleSlip"
],
"ID": 1,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Event",
"py/state": {
"name": "RateGroupCycleSlip",
"ID": 1,
"severity": "WARNING_HI",
"format_string": "Rate group cycle slipped on cycle %s",
"args": [
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "cycle",
"type": {
"py/id": 15
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
],
"metadata": {}
}
}
}
]
}
}
}
},
{
"names": [
"ActiveRateGroup-RateGroupMedFreq"
],
"ID": 1024,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Module",
"py/state": {
"name": "ActiveRateGroup-RateGroupMedFreq",
"ID": 1024,
"commands": {
"py/object": "IrisBackendv3.utils.nameiddict.NameIdDict",
"id_dict": {},
"name_id_mapping": {}
},
"telemetry": {
"py/object": "IrisBackendv3.utils.nameiddict.NameIdDict",
"py/state": [
{
"names": [
"RgMaxTime"
],
"ID": 0,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "RgMaxTime",
"ID": 0,
"type": {
"py/id": 15
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"RgCycleSlips"
],
"ID": 1,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "RgCycleSlips",
"ID": 1,
"type": {
"py/id": 15
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
}
]
},
"events": {
"py/object": "IrisBackendv3.utils.nameiddict.NameIdDict",
"py/state": [
{
"names": [
"RateGroupStarted"
],
"ID": 0,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Event",
"py/state": {
"name": "RateGroupStarted",
"ID": 0,
"severity": "DIAGNOSTIC",
"format_string": "Rate group started.",
"args": [],
"metadata": {}
}
}
},
{
"names": [
"RateGroupCycleSlip"
],
"ID": 1,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Event",
"py/state": {
"name": "RateGroupCycleSlip",
"ID": 1,
"severity": "WARNING_HI",
"format_string": "Rate group cycle slipped on cycle %s",
"args": [
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "cycle",
"type": {
"py/id": 15
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
],
"metadata": {}
}
}
}
]
}
}
}
},
{
"names": [
"ActiveRateGroup-RateGroupHiFreq"
],
"ID": 1280,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Module",
"py/state": {
"name": "ActiveRateGroup-RateGroupHiFreq",
"ID": 1280,
"commands": {
"py/object": "IrisBackendv3.utils.nameiddict.NameIdDict",
"id_dict": {},
"name_id_mapping": {}
},
"telemetry": {
"py/object": "IrisBackendv3.utils.nameiddict.NameIdDict",
"py/state": [
{
"names": [
"RgMaxTime"
],
"ID": 0,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "RgMaxTime",
"ID": 0,
"type": {
"py/id": 15
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"RgCycleSlips"
],
"ID": 1,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "RgCycleSlips",
"ID": 1,
"type": {
"py/id": 15
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
}
]
},
"events": {
"py/object": "IrisBackendv3.utils.nameiddict.NameIdDict",
"py/state": [
{
"names": [
"RateGroupStarted"
],
"ID": 0,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Event",
"py/state": {
"name": "RateGroupStarted",
"ID": 0,
"severity": "DIAGNOSTIC",
"format_string": "Rate group started.",
"args": [],
"metadata": {}
}
}
},
{
"names": [
"RateGroupCycleSlip"
],
"ID": 1,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Event",
"py/state": {
"name": "RateGroupCycleSlip",
"ID": 1,
"severity": "WARNING_HI",
"format_string": "Rate group cycle slipped on cycle %s",
"args": [
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "cycle",
"type": {
"py/id": 15
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
],
"metadata": {}
}
}
}
]
}
}
}
},
{
"names": [
"CubeRoverTime"
],
"ID": 1536,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Module",
"py/state": {
"name": "CubeRoverTime",
"ID": 1536,
"commands": {
"py/object": "IrisBackendv3.utils.nameiddict.NameIdDict",
"id_dict": {},
"name_id_mapping": {}
},
"telemetry": {
"py/object": "IrisBackendv3.utils.nameiddict.NameIdDict",
"id_dict": {},
"name_id_mapping": {}
},
"events": {
"py/object": "IrisBackendv3.utils.nameiddict.NameIdDict",
"id_dict": {},
"name_id_mapping": {}
}
}
}
},
{
"names": [
"TlmChan"
],
"ID": 1792,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Module",
"py/state": {
"name": "TlmChan",
"ID": 1792,
"commands": {
"py/object": "IrisBackendv3.utils.nameiddict.NameIdDict",
"id_dict": {},
"name_id_mapping": {}
},
"telemetry": {
"py/object": "IrisBackendv3.utils.nameiddict.NameIdDict",
"id_dict": {},
"name_id_mapping": {}
},
"events": {
"py/object": "IrisBackendv3.utils.nameiddict.NameIdDict",
"id_dict": {},
"name_id_mapping": {}
}
}
}
},
{
"names": [
"CommandDispatcher"
],
"ID": 2048,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Module",
"py/state": {
"name": "CommandDispatcher",
"ID": 2048,
"commands": {
"py/object": "IrisBackendv3.utils.nameiddict.NameIdDict",
"py/state": [
{
"names": [
"CommandDispatcher_Cmdnoop"
],
"ID": 0,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Command",
"py/state": {
"name": "CommandDispatcher_Cmdnoop",
"mnemonic": "CMD_NO_OP",
"ID": 0,
"args": [],
"metadata": {}
}
}
},
{
"names": [
"CommandDispatcher_Cmdnoopstring"
],
"ID": 1,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Command",
"py/state": {
"name": "CommandDispatcher_Cmdnoopstring",
"mnemonic": "CMD_NO_OP_STRING",
"ID": 1,
"args": [
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "arg_1",
"type": {
"py/reduce": [
{
"py/type": "IrisBackendv3.data_standards.fsw_data_type.FswDataType"
},
{
"py/tuple": [
"char[40]"
]
}
]
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
],
"metadata": {}
}
}
},
{
"names": [
"CommandDispatcher_Cmdtestcmd1"
],
"ID": 2,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Command",
"py/state": {
"name": "CommandDispatcher_Cmdtestcmd1",
"mnemonic": "CMD_TEST_CMD_1",
"ID": 2,
"args": [
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "arg_1",
"type": {
"py/reduce": [
{
"py/type": "IrisBackendv3.data_standards.fsw_data_type.FswDataType"
},
{
"py/tuple": [
"int32"
]
}
]
},
"enum": [],
"bitfields": null,
"metadata": {}
}
},
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "arg_2",
"type": {
"py/reduce": [
{
"py/type": "IrisBackendv3.data_standards.fsw_data_type.FswDataType"
},
{
"py/tuple": [
"float"
]
}
]
},
"enum": [],
"bitfields": null,
"metadata": {}
}
},
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "arg_3",
"type": {
"py/reduce": [
{
"py/type": "IrisBackendv3.data_standards.fsw_data_type.FswDataType"
},
{
"py/tuple": [
"uint8"
]
}
]
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
],
"metadata": {}
}
}
},
{
"names": [
"CommandDispatcher_Cmdcleartracking"
],
"ID": 3,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Command",
"py/state": {
"name": "CommandDispatcher_Cmdcleartracking",
"mnemonic": "CMD_CLEAR_TRACKING",
"ID": 3,
"args": [],
"metadata": {}
}
}
}
]
},
"telemetry": {
"py/object": "IrisBackendv3.utils.nameiddict.NameIdDict",
"py/state": [
{
"names": [
"CommandsDispatched"
],
"ID": 0,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "CommandsDispatched",
"ID": 0,
"type": {
"py/id": 15
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"CommandErrors"
],
"ID": 1,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "CommandErrors",
"ID": 1,
"type": {
"py/id": 15
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
}
]
},
"events": {
"py/object": "IrisBackendv3.utils.nameiddict.NameIdDict",
"py/state": [
{
"names": [
"OpCodeRegistered"
],
"ID": 0,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Event",
"py/state": {
"name": "OpCodeRegistered",
"ID": 0,
"severity": "DIAGNOSTIC",
"format_string": "Opcode 0x%04X registered to port %s slot %s",
"args": [
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "opcode",
"type": {
"py/id": 15
},
"enum": [],
"bitfields": null,
"metadata": {}
}
},
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "port",
"type": {
"py/id": 207
},
"enum": [],
"bitfields": null,
"metadata": {}
}
},
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "slot",
"type": {
"py/id": 207
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
],
"metadata": {}
}
}
},
{
"names": [
"OpCodeDispatched"
],
"ID": 1,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Event",
"py/state": {
"name": "OpCodeDispatched",
"ID": 1,
"severity": "COMMAND",
"format_string": "Opcode 0x%04X dispatched to port %s",
"args": [
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "opcode",
"type": {
"py/id": 15
},
"enum": [],
"bitfields": null,
"metadata": {}
}
},
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "port",
"type": {
"py/id": 207
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
],
"metadata": {}
}
}
},
{
"names": [
"OpCodeCompleted"
],
"ID": 2,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Event",
"py/state": {
"name": "OpCodeCompleted",
"ID": 2,
"severity": "COMMAND",
"format_string": "Opcode 0x%04X completed",
"args": [
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "opcode",
"type": {
"py/id": 15
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
],
"metadata": {}
}
}
},
{
"names": [
"OpCodeError"
],
"ID": 3,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Event",
"py/state": {
"name": "OpCodeError",
"ID": 3,
"severity": "WARNING_HI",
"format_string": "Opcode 0x%04X completed with error %s ",
"args": [
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "opcode",
"type": {
"py/id": 15
},
"enum": [],
"bitfields": null,
"metadata": {}
}
},
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "error",
"type": {
"py/reduce": [
{
"py/type": "IrisBackendv3.data_standards.fsw_data_type.FswDataType"
},
{
"py/tuple": [
"enum/*uint32*/"
]
}
]
},
"enum": [
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "ERRINVALIDOPCODE",
"value": 0,
"comment": "Invalid opcode dispatched"
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "ERRVALIDATIONERROR",
"value": 1,
"comment": "Command failed validation"
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "ERRFORMATERROR",
"value": 2,
"comment": "Command failed to deserialize"
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "ERREXECUTIONERROR",
"value": 3,
"comment": "Command had execution error"
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "ERRBUSY",
"value": 4,
"comment": "Component busy"
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "ERRUNEXP",
"value": 5,
"comment": "Unexpected response"
}
}
],
"bitfields": null,
"metadata": {}
}
}
],
"metadata": {}
}
}
},
{
"names": [
"MalformedCommand"
],
"ID": 4,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Event",
"py/state": {
"name": "MalformedCommand",
"ID": 4,
"severity": "WARNING_HI",
"format_string": "Received malformed command packet. Status: %s",
"args": [
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "status",
"type": {
"py/id": 296
},
"enum": [
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "ERRBUFFERTOOSMALL",
"value": 0,
"comment": "Buffer too small"
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "ERRBUFFERFORMAT",
"value": 1,
"comment": "Buffer wrong format"
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "ERRSIZEMISMATCH",
"value": 2,
"comment": "Buffer size mismatch"
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "ERRTYPEMISMATCH",
"value": 3,
"comment": "Buffer type mismatch"
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "ERRUNEXPSTAT",
"value": 4,
"comment": "Unexpected status"
}
}
],
"bitfields": null,
"metadata": {}
}
}
],
"metadata": {}
}
}
},
{
"names": [
"InvalidCommand"
],
"ID": 5,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Event",
"py/state": {
"name": "InvalidCommand",
"ID": 5,
"severity": "WARNING_HI",
"format_string": "Invalid opcode 0x%04X received.",
"args": [
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "opcode",
"type": {
"py/id": 15
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
],
"metadata": {}
}
}
},
{
"names": [
"TooManyCommands"
],
"ID": 6,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Event",
"py/state": {
"name": "TooManyCommands",
"ID": 6,
"severity": "WARNING_HI",
"format_string": "Too many outstanding commands. opcode=0x%04X",
"args": [
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "opcode",
"type": {
"py/id": 15
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
],
"metadata": {}
}
}
},
{
"names": [
"NoOpReceived"
],
"ID": 7,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Event",
"py/state": {
"name": "NoOpReceived",
"ID": 7,
"severity": "ACTIVITY_HI",
"format_string": "Received a NO-OP command",
"args": [],
"metadata": {}
}
}
},
{
"names": [
"NoOpStringReceived"
],
"ID": 8,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Event",
"py/state": {
"name": "NoOpStringReceived",
"ID": 8,
"severity": "ACTIVITY_HI",
"format_string": "Received a NO-OP string=%s",
"args": [
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "message",
"type": {
"py/id": 196
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
],
"metadata": {}
}
}
},
{
"names": [
"TestCmd1Args"
],
"ID": 9,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Event",
"py/state": {
"name": "TestCmd1Args",
"ID": 9,
"severity": "ACTIVITY_HI",
"format_string": "TEST_CMD_1 args: I32: %s, F32: %f, U8: %s",
"args": [
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "arg_1",
"type": {
"py/id": 207
},
"enum": [],
"bitfields": null,
"metadata": {}
}
},
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "arg_2",
"type": {
"py/id": 212
},
"enum": [],
"bitfields": null,
"metadata": {}
}
},
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "arg_3",
"type": {
"py/id": 217
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
],
"metadata": {}
}
}
}
]
}
}
}
},
{
"names": [
"GroundInterface"
],
"ID": 2304,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Module",
"py/state": {
"name": "GroundInterface",
"ID": 2304,
"commands": {
"py/object": "IrisBackendv3.utils.nameiddict.NameIdDict",
"py/state": [
{
"names": [
"GroundInterface_SetPrimaryInterface"
],
"ID": 0,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Command",
"py/state": {
"name": "GroundInterface_SetPrimaryInterface",
"mnemonic": "Set_Primary_Interface",
"ID": 0,
"args": [
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "primary_interface",
"type": {
"py/id": 296
},
"enum": [
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "WATCHDOG",
"value": 0,
"comment": "Watchdog wired interface (RS422). Matches WatchdogInterface component attached to port 0."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "WF_121",
"value": 1,
"comment": "WF121 wireless interface (WiFi). Matches NetworkManager component attached to port 1."
}
}
],
"bitfields": null,
"metadata": {}
}
}
],
"metadata": {}
}
}
},
{
"names": [
"GroundInterface_SetGroundInterfaceTelemetryLevel"
],
"ID": 1,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Command",
"py/state": {
"name": "GroundInterface_SetGroundInterfaceTelemetryLevel",
"mnemonic": "Set_GroundInterface_Telemetry_Level",
"ID": 1,
"args": [
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "telemetry_level",
"type": {
"py/id": 296
},
"enum": [
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "CRITICAL",
"value": 0,
"comment": "Emit CRITICAL telemetry items only."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "IMPORTANT",
"value": 1,
"comment": "Emit CRITICAL and important telemetry items only."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "ALL",
"value": 2,
"comment": "Emit all telemetry items."
}
}
],
"bitfields": null,
"metadata": {}
}
}
],
"metadata": {}
}
}
},
{
"names": [
"GroundInterface_SetInterfaceAutoSwitch"
],
"ID": 2,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Command",
"py/state": {
"name": "GroundInterface_SetInterfaceAutoSwitch",
"mnemonic": "SetInterfaceAutoSwitch",
"ID": 2,
"args": [
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "on",
"type": {
"py/reduce": [
{
"py/type": "IrisBackendv3.data_standards.fsw_data_type.FswDataType"
},
{
"py/tuple": [
"bool"
]
}
]
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
],
"metadata": {}
}
}
},
{
"names": [
"GroundInterface_RollCredits"
],
"ID": 160,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Command",
"py/state": {
"name": "GroundInterface_RollCredits",
"mnemonic": "RollCredits",
"ID": 160,
"args": [
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "on",
"type": {
"py/id": 429
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
],
"metadata": {}
}
}
},
{
"names": [
"GroundInterface_SetNameAndMessagePeriod"
],
"ID": 161,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Command",
"py/state": {
"name": "GroundInterface_SetNameAndMessagePeriod",
"mnemonic": "Set_NameAndMessage_Period",
"ID": 161,
"args": [
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "seconds",
"type": {
"py/reduce": [
{
"py/type": "IrisBackendv3.data_standards.fsw_data_type.FswDataType"
},
{
"py/tuple": [
"uint16"
]
}
]
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
],
"metadata": {}
}
}
}
]
},
"telemetry": {
"py/object": "IrisBackendv3.utils.nameiddict.NameIdDict",
"py/state": [
{
"names": [
"GiUplinkSeqNum"
],
"ID": 0,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "GiUplinkSeqNum",
"ID": 0,
"type": {
"py/id": 217
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"GiDownlinkSeqNum"
],
"ID": 1,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "GiDownlinkSeqNum",
"ID": 1,
"type": {
"py/id": 217
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"GiPacketsReceived"
],
"ID": 2,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "GiPacketsReceived",
"ID": 2,
"type": {
"py/id": 15
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"GiPacketsTransmitted"
],
"ID": 3,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "GiPacketsTransmitted",
"ID": 3,
"type": {
"py/id": 15
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"GiTlmItemsReceived"
],
"ID": 4,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "GiTlmItemsReceived",
"ID": 4,
"type": {
"py/id": 15
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"GiTlmItemsDownlinked"
],
"ID": 5,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "GiTlmItemsDownlinked",
"ID": 5,
"type": {
"py/id": 15
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"GiLogsReceived"
],
"ID": 6,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "GiLogsReceived",
"ID": 6,
"type": {
"py/id": 15
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"GiLogsDownlinked"
],
"ID": 7,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "GiLogsDownlinked",
"ID": 7,
"type": {
"py/id": 15
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"GiCmdsUplinked"
],
"ID": 8,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "GiCmdsUplinked",
"ID": 8,
"type": {
"py/id": 15
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"GiCmdsSent"
],
"ID": 9,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "GiCmdsSent",
"ID": 9,
"type": {
"py/id": 15
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"GiUplinkPktErrs"
],
"ID": 10,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "GiUplinkPktErrs",
"ID": 10,
"type": {
"py/id": 15
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"GiAppBytesReceived"
],
"ID": 11,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "GiAppBytesReceived",
"ID": 11,
"type": {
"py/id": 15
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"GiAppBytesDownlinked"
],
"ID": 12,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "GiAppBytesDownlinked",
"ID": 12,
"type": {
"py/id": 15
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
}
]
},
"events": {
"py/object": "IrisBackendv3.utils.nameiddict.NameIdDict",
"py/state": [
{
"names": [
"GiUplinkedPacketError"
],
"ID": 0,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Event",
"py/state": {
"name": "GiUplinkedPacketError",
"ID": 0,
"severity": "WARNING_HI",
"format_string": "Error with uplinked packet: %s. Expected: %s. Got: %s.",
"args": [
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "error_type",
"type": {
"py/id": 296
},
"enum": [
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "MISMATCHEDLENGTH",
"value": 0,
"comment": ""
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "OUTOFSEQUENCE",
"value": 1,
"comment": ""
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "BADCHECKSUM",
"value": 2,
"comment": ""
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "INCORRECTTYPE",
"value": 3,
"comment": ""
}
}
],
"bitfields": null,
"metadata": {}
}
},
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "expected",
"type": {
"py/id": 450
},
"enum": [],
"bitfields": null,
"metadata": {}
}
},
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "got",
"type": {
"py/id": 450
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
],
"metadata": {}
}
}
},
{
"names": [
"GiUplinkedBadMagic"
],
"ID": 1,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Event",
"py/state": {
"name": "GiUplinkedBadMagic",
"ID": 1,
"severity": "WARNING_LO",
"format_string": "Bad magic received: 0x%04X",
"args": [
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "magic_received",
"type": {
"py/id": 15
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
],
"metadata": {}
}
}
},
{
"names": [
"GiCommandReceived"
],
"ID": 2,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Event",
"py/state": {
"name": "GiCommandReceived",
"ID": 2,
"severity": "ACTIVITY_HI",
"format_string": "Command Received with seq_num=%s, checksum=%s.",
"args": [
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "seq",
"type": {
"py/id": 217
},
"enum": [],
"bitfields": null,
"metadata": {}
}
},
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "checksum",
"type": {
"py/id": 450
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
],
"metadata": {}
}
}
},
{
"names": [
"GiDownlinkedPacket"
],
"ID": 3,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Event",
"py/state": {
"name": "GiDownlinkedPacket",
"ID": 3,
"severity": "ACTIVITY_LO",
"format_string": "Downlinked Packet #%s (checksum=%s, %sB).",
"args": [
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "seq",
"type": {
"py/id": 217
},
"enum": [],
"bitfields": null,
"metadata": {}
}
},
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "checksum",
"type": {
"py/id": 450
},
"enum": [],
"bitfields": null,
"metadata": {}
}
},
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "length",
"type": {
"py/id": 450
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
],
"metadata": {}
}
}
},
{
"names": [
"GiDownlinkedItem"
],
"ID": 4,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Event",
"py/state": {
"name": "GiDownlinkedItem",
"ID": 4,
"severity": "DIAGNOSTIC",
"format_string": "Item Downlinked with seq_num=%s from %s.",
"args": [
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "seq",
"type": {
"py/id": 217
},
"enum": [],
"bitfields": null,
"metadata": {}
}
},
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "from",
"type": {
"py/id": 296
},
"enum": [
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "DOWNLINK_TELEMETRY",
"value": 0,
"comment": ""
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "DOWNLINK_LOG",
"value": 1,
"comment": ""
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "DOWNLINK_FILE",
"value": 2,
"comment": ""
}
}
],
"bitfields": null,
"metadata": {}
}
}
],
"metadata": {}
}
}
},
{
"names": [
"InterfaceAutoSwitchChanged"
],
"ID": 5,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Event",
"py/state": {
"name": "InterfaceAutoSwitchChanged",
"ID": 5,
"severity": "ACTIVITY_HI",
"format_string": "Interface auto-switching changed to %s.",
"args": [
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "on",
"type": {
"py/id": 429
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
],
"metadata": {}
}
}
},
{
"names": [
"InterfaceAutoSwitch"
],
"ID": 6,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Event",
"py/state": {
"name": "InterfaceAutoSwitch",
"ID": 6,
"severity": "ACTIVITY_HI",
"format_string": "Hercules downlink interface automatically changed from %s to %s.",
"args": [
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "from",
"type": {
"py/id": 296
},
"enum": [
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "FROMWATCHDOG",
"value": 0,
"comment": "Watchdog wired interface (RS422). Matches WatchdogInterface component attached to port 0."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "FROMWF_121",
"value": 1,
"comment": "WF121 wireless interface (WiFi). Matches NetworkManager component attached to port 1."
}
}
],
"bitfields": null,
"metadata": {}
}
},
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "to",
"type": {
"py/id": 296
},
"enum": [
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "TOWATCHDOG",
"value": 0,
"comment": "Watchdog wired interface (RS422). Matches WatchdogInterface component attached to port 0."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "TOWF_121",
"value": 1,
"comment": "WF121 wireless interface (WiFi). Matches NetworkManager component attached to port 1."
}
}
],
"bitfields": null,
"metadata": {}
}
}
],
"metadata": {}
}
}
},
{
"names": [
"BroughtToYouBy"
],
"ID": 160,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Event",
"py/state": {
"name": "BroughtToYouBy",
"ID": 160,
"severity": "ACTIVITY_LO",
"format_string": "Made Possible By: %s.",
"args": [
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "name",
"type": {
"py/reduce": [
{
"py/type": "IrisBackendv3.data_standards.fsw_data_type.FswDataType"
},
{
"py/tuple": [
"char[50]"
]
}
]
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
],
"metadata": {}
}
}
},
{
"names": [
"SpecialMessage"
],
"ID": 161,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Event",
"py/state": {
"name": "SpecialMessage",
"ID": 161,
"severity": "ACTIVITY_LO",
"format_string": "%s says: %s",
"args": [
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "name",
"type": {
"py/id": 663
},
"enum": [],
"bitfields": null,
"metadata": {}
}
},
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "message",
"type": {
"py/reduce": [
{
"py/type": "IrisBackendv3.data_standards.fsw_data_type.FswDataType"
},
{
"py/tuple": [
"char[240]"
]
}
]
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
],
"metadata": {}
}
}
}
]
}
}
}
},
{
"names": [
"NetworkManager"
],
"ID": 2560,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Module",
"py/state": {
"name": "NetworkManager",
"ID": 2560,
"commands": {
"py/object": "IrisBackendv3.utils.nameiddict.NameIdDict",
"py/state": [
{
"names": [
"NetworkManager_SetRadioCommunicationsMode"
],
"ID": 0,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Command",
"py/state": {
"name": "NetworkManager_SetRadioCommunicationsMode",
"mnemonic": "Set_Radio_Communications_Mode",
"ID": 0,
"args": [
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "mode",
"type": {
"py/id": 296
},
"enum": [
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "HERCULES",
"value": 0,
"comment": "Normal mode. Hercules-Radio UART is ON inside Hercules and Hercules is the one talking to the Radio. Returning to this mode *might* not work, in which case you'll need to just send a reset-herc command to the WatchDog."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "EXTERNAL",
"value": 1,
"comment": "External mode. Hercules-Radio UART is turned OFF inside Hercules allowing an external device to talk to the Radio via J33. Used for debugging and programming the Radio. NOTE: in this mode, Hercules's RTS (which is connected to the Radio's CTS) is kept in the active/ready mode (LOW) so that the Radio feels free to send data whenever. This effectively deactivates control flow, though an external device can still listen to the Radio's RTS if needed - though this wasn't needed in development."
}
}
],
"bitfields": null,
"metadata": {}
}
}
],
"metadata": {}
}
}
},
{
"names": [
"NetworkManager_SetRadioUartBaud"
],
"ID": 1,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Command",
"py/state": {
"name": "NetworkManager_SetRadioUartBaud",
"mnemonic": "Set_Radio_Uart_Baud",
"ID": 1,
"args": [
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "new_baud",
"type": {
"py/id": 15
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
],
"metadata": {}
}
}
},
{
"names": [
"NetworkManager_SetRadioBgApiPassthrough"
],
"ID": 2,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Command",
"py/state": {
"name": "NetworkManager_SetRadioBgApiPassthrough",
"mnemonic": "Set_Radio_BgApi_Passthrough",
"ID": 2,
"args": [
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "passthrough",
"type": {
"py/id": 429
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
],
"metadata": {}
}
}
},
{
"names": [
"NetworkManager_SendBgApiCommand"
],
"ID": 3,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Command",
"py/state": {
"name": "NetworkManager_SendBgApiCommand",
"mnemonic": "Send_BgApi_Command",
"ID": 3,
"args": [
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "crc_32",
"type": {
"py/id": 15
},
"enum": [],
"bitfields": null,
"metadata": {}
}
},
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "packet_id",
"type": {
"py/id": 15
},
"enum": [],
"bitfields": null,
"metadata": {}
}
},
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "expect_response",
"type": {
"py/id": 296
},
"enum": [
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "NM_BGAPI_CMD_EXPECTRESPONSE",
"value": 4294942297,
"comment": "Hercules SHOULD expect the Radio to return a response to this command."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "NM_BGAPI_CMD_DONTEXPECTRESPONSE",
"value": 3122462720,
"comment": "Hercules SHOULD NOT expect the Radio to return a response to this command."
}
}
],
"bitfields": null,
"metadata": {}
}
},
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "bgapi_packet",
"type": {
"py/reduce": [
{
"py/type": "IrisBackendv3.data_standards.fsw_data_type.FswDataType"
},
{
"py/tuple": [
"char[/*up to*/134]"
]
}
]
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
],
"metadata": {}
}
}
},
{
"names": [
"NetworkManager_DownlinkBgApiCommandRecords"
],
"ID": 4,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Command",
"py/state": {
"name": "NetworkManager_DownlinkBgApiCommandRecords",
"mnemonic": "Downlink_BgApi_Command_Records",
"ID": 4,
"args": [],
"metadata": {}
}
}
}
]
},
"telemetry": {
"py/object": "IrisBackendv3.utils.nameiddict.NameIdDict",
"py/state": [
{
"names": [
"Rssi"
],
"ID": 0,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "Rssi",
"ID": 0,
"type": {
"py/reduce": [
{
"py/type": "IrisBackendv3.data_standards.fsw_data_type.FswDataType"
},
{
"py/tuple": [
"int8"
]
}
]
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"Snr"
],
"ID": 1,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "Snr",
"ID": 1,
"type": {
"py/id": 761
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"PktRecv"
],
"ID": 2,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "PktRecv",
"ID": 2,
"type": {
"py/id": 15
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"PktSent"
],
"ID": 3,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "PktSent",
"ID": 3,
"type": {
"py/id": 15
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"WifiErrorStatus"
],
"ID": 4,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "WifiErrorStatus",
"ID": 4,
"type": {
"py/id": 296
},
"enum": [
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "WEC_NOERROR",
"value": 0,
"comment": ""
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "WEC_TRYAGAIN",
"value": 1,
"comment": ""
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "WEC_INVALIDPARAMETER",
"value": 384,
"comment": "This error code indicates that a command contained an invalid parameter"
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "WEC_DEVICEWRONGSTATE",
"value": 385,
"comment": "This error code indicates that the device is in wrong state to accept commands."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "WEC_OUTOFMEMORY",
"value": 386,
"comment": "This error indicates that the device has run out of memory."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "WEC_FEATURENOTIMPLEMENTED",
"value": 387,
"comment": "This error indicates that the feature in question has not been implemented."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "WEC_COMMANDNOTRECOGNIZED",
"value": 388,
"comment": "This error indicates that the issued command was not recognized."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "WEC_TIMEOUT",
"value": 389,
"comment": "This error indicates that a command or procedure failed due to timeout.This error code is generated e.g. if you send an incomplete command to the Wi-Fi module - after the timeout of 1 second this error code is sent to the host transported by the event .endpoint_syntax_errorThis error code is generated also e.g. when the maximum number of retry attempts (10) to try to connect to a wireless network have been executed. A typical example of such a case might be when the issued password is invalid, in which case the error code is transported by the event .wifi_evt_sme_connect_failed"
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "WEC_UNSPECIFIEDERROR",
"value": 390,
"comment": "This error code is generated when an unspecified error is detected."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "WEC_HARDWAREFAILURE",
"value": 391,
"comment": "This error code is generated when a hardware failure is detected."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "WEC_INTERNALBUFFERFULL",
"value": 392,
"comment": "This error code is generated the a command was not accepted due to full internal buffers."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "WEC_DISCONNECTED",
"value": 393,
"comment": "This error code is generated when a command or procedure has failed due to disconnection."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "WEC_TOOMANYREQUEST",
"value": 394,
"comment": "This error code is generated when there are too many simultaneous requests."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "WEC_ACCESSPOINTNOTINSCANLIST",
"value": 395,
"comment": "This error code is generated when the defined Access Point is not found from the scanlist."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "WEC_INVALIDPASSWORD",
"value": 396,
"comment": "This error code is generated in the following cases:1) you try to connect to a secured network without setting the password or the password is too short2) you try to start a secured AP without setting the password or the password is too short3) you try to set a 64-character PSK with non-hex characters4) you try to set an invalid WEP key (invalid characters or invalid length)Notice that WPA does not contain any standard way for the Access Point to communicate to the station that the password is invalid. The Access Point just disconnects the client during authentication if the password is found invalid. Some stations take an educated guess that this probably means the password is incorrect. WF121 simply retries the authentication until it exceeds the maximum amount of retries (10) which then causes the 0x0185 Timeout Error."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "WEC_AUTHENTIFICATIONFAILURE",
"value": 397,
"comment": "This error code is generated when the WPA/WPA2 authentication has failed."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "WEC_OVERFLOW",
"value": 398,
"comment": "This error code is generated when an overflow has been detected."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "WEC_MULTIPLEPBCSESSIONS",
"value": 399,
"comment": "This error code is generated when multiple PBC (Push Button Configuration) sessions have been detected."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "WEC_ETHERNETNOTCONNECTED",
"value": 400,
"comment": "This error code is generated when the Ethernet cable is not connected"
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "WEC_ETHERNETROUTENOTSET",
"value": 401,
"comment": "This error code is generated if the Ethernet route is not set"
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "WEC_WRONGOPERATINGMODE",
"value": 402,
"comment": "This error code is generated if the operating mode is wrong for the issued command"
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "WEC_WIFINOTFOUND",
"value": 403,
"comment": "This error code is generated if the requested resource was not found."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "WEC_WIFINOTALREADYEXIST",
"value": 404,
"comment": "This error is generated if the requested resource already exists."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "WEC_WIFIINVALIDCONFIGURATION",
"value": 405,
"comment": "This error code is generated if the current configuration is invalid."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "WEC_WIFIACCESSPOINTLOST",
"value": 406,
"comment": "This error code is generated if the connection to an Access Point is lost."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "WEC_PSSTOREFULL",
"value": 769,
"comment": "This error is generated if the Flash which is reserved for the PS (Persistent Store) is full."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "WEC_PSKEYNOTFOUND",
"value": 770,
"comment": "This error is generated if the PS key (Persistent Store key) was not found."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "WEC_I_2_CWRITEALREADYINPROGRESS",
"value": 771,
"comment": "This error is generated if I2C transmission was initiated while a transmission was already in progress."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "WEC_I_2_CACKMISSING",
"value": 772,
"comment": "This error is generated if an acknowledgement for I2C was not received."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "WEC_FLASHWRITEFAILED",
"value": 776,
"comment": "This error code is generated if writing to Flash failed."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "WEC_FILENOTOPENED",
"value": 773,
"comment": "This error code is generated if an access was attempted to an unopened file."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "WEC_FILENOTFOUND",
"value": 774,
"comment": "This error code is generated if the requested file was not found in the SD card."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "WEC_DISKERROR",
"value": 775,
"comment": "This error code is generated if an SD card error was detected of if the SD card is full"
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "WEC_TCPIPSUCCESS",
"value": 512,
"comment": "This code indicates that no error was detected."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "WEC_TCPIPOUTOFMEMORY",
"value": 513,
"comment": "This error code is generated when the system has run out of memory."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "WEC_TCPIPBUFFERERROR",
"value": 514,
"comment": "This error code is generated when handling of buffers has failed"
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "WEC_TCPIPTIMEOUT",
"value": 515,
"comment": "This error code is generated when a timeout has been detected."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "WEC_TCPIPROUTING",
"value": 516,
"comment": "This error code is generated when a route could not be found."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "WEC_TCPIPINPROGRESS",
"value": 517,
"comment": "This error code is generated when an operation is in progress"
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "WEC_TCPIPILLEGALVALUE",
"value": 518,
"comment": "This error code is generated when the issued value is deemed illegal."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "WEC_TCPIPWOULDBLOCK",
"value": 519,
"comment": "This error code is generated when an operation blocks."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "WEC_TCPIPADDRESSINUSE",
"value": 520,
"comment": "This error code is generated when the issued address is already in use"
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "WEC_TCPIPALREADYCONNECTED",
"value": 521,
"comment": "This error code is generated when the Wi-Fi module is already connected"
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "WEC_TCPIPCONNECTIONABORTED",
"value": 522,
"comment": "This error code is generated when a connection is aborted."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "WEC_TCPIPCONNECTIONRESET",
"value": 523,
"comment": "This error code is generated when a connection has been reset"
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "WEC_TCPIPCONNECTIONCLOSED",
"value": 524,
"comment": "This error code is generated when a connection has been closed."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "WEC_TCPIPNOTCONNECTED",
"value": 525,
"comment": "This error code is generated when the Wi-Fi module is not connected."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "WEC_TCPIPILLEGALARGUMENT",
"value": 526,
"comment": "This error code is generated if an illegal argument was issued."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "WEC_TCPIPINTERFACELEVELERROR",
"value": 527,
"comment": "This error code is generated if an interface error was detected"
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "WEC_TCPIPUNKNOWNHOST",
"value": 640,
"comment": "This error is generated if an unknown host is detected."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "WEC_SERVICENOTRUNNING",
"value": 528,
"comment": "This error code is generated if the specified service is not running."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "WEC_SERVICERUNNING",
"value": 529,
"comment": "This error code is generated if the specified service is already running"
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "WEC_HOSTNAMENOTSET",
"value": 530,
"comment": "This error code is generated if the hostname has not been set"
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "WEC_HOSTNAMECONFLICT",
"value": 531,
"comment": "This error code is generated if a hostname conflict was detected."
}
}
],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"WifiStateStatus"
],
"ID": 5,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "WifiStateStatus",
"ID": 5,
"type": {
"py/id": 296
},
"enum": [
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "NM_NONE",
"value": 0,
"comment": "None (we haven't received any state update yet)."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "NM_BADMESSAGE",
"value": 15,
"comment": "The last state update from the Radio contained a valid message header but the body was corrupted (didn't match any known message). Bad state message was received (we don't know what's really going on inside the Radio rn)."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "NM_BOOT",
"value": 16,
"comment": "Booted but hasn't initialized its state_driver yet."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "NM_INIT",
"value": 32,
"comment": "In the initial state but WiFi radio isn't powered up yet."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "NM_WIFION",
"value": 48,
"comment": "Radio hardware (power amplifier) has turned on successfully - radio has enough power to work."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "NM_CONNECTED",
"value": 64,
"comment": "Radio is connected to the network (ARP, etc) but UDP client & socket aren't active yet."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "NM_UDPCONNECTED",
"value": 80,
"comment": "UDP client & socket are active. Radio is ready to communicate."
}
}
],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"UartBytesReceived"
],
"ID": 6,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "UartBytesReceived",
"ID": 6,
"type": {
"py/id": 15
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
}
]
},
"events": {
"py/object": "IrisBackendv3.utils.nameiddict.NameIdDict",
"py/state": [
{
"names": [
"StateChange"
],
"ID": 0,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Event",
"py/state": {
"name": "StateChange",
"ID": 0,
"severity": "ACTIVITY_HI",
"format_string": "WF121 State Changed %s -> %s",
"args": [
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "from_state",
"type": {
"py/id": 296
},
"enum": [
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "FROM_NONE",
"value": 0,
"comment": "None (we haven't received any state update yet)."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "FROM_BADMESSAGE",
"value": 15,
"comment": "The last state update from the Radio contained a valid message header but the body was corrupted (didn't match any known message). Bad state message was received (we don't know what's really going on inside the Radio rn)."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "FROM_BOOT",
"value": 16,
"comment": "Booted but hasn't initialized its state_driver yet."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "FROM_INIT",
"value": 32,
"comment": "In the initial state but WiFi radio isn't powered up yet."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "FROM_WIFION",
"value": 48,
"comment": "Radio hardware (power amplifier) has turned on successfully - radio has enough power to work."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "FROM_CONNECTED",
"value": 64,
"comment": "Radio is connected to the network (ARP, etc) but UDP client & socket aren't active yet."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "FROM_UDPCONNECTED",
"value": 80,
"comment": "UDP client & socket are active. Radio is ready to communicate."
}
}
],
"bitfields": null,
"metadata": {}
}
},
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "to_state",
"type": {
"py/id": 296
},
"enum": [
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "TO_NONE",
"value": 0,
"comment": "None (we haven't received any state update yet)."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "TO_BADMESSAGE",
"value": 15,
"comment": "The last state update from the Radio contained a valid message header but the body was corrupted (didn't match any known message). Bad state message was received (we don't know what's really going on inside the Radio rn)."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "TO_BOOT",
"value": 16,
"comment": "Booted but hasn't initialized its state_driver yet."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "TO_INIT",
"value": 32,
"comment": "In the initial state but WiFi radio isn't powered up yet."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "TO_WIFION",
"value": 48,
"comment": "Radio hardware (power amplifier) has turned on successfully - radio has enough power to work."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "TO_CONNECTED",
"value": 64,
"comment": "Radio is connected to the network (ARP, etc) but UDP client & socket aren't active yet."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "TO_UDPCONNECTED",
"value": 80,
"comment": "UDP client & socket are active. Radio is ready to communicate."
}
}
],
"bitfields": null,
"metadata": {}
}
}
],
"metadata": {}
}
}
},
{
"names": [
"ExecuteCallbackError"
],
"ID": 1,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Event",
"py/state": {
"name": "ExecuteCallbackError",
"ID": 1,
"severity": "WARNING_HI",
"format_string": "ExecuteCallBackError %s",
"args": [
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "error_code",
"type": {
"py/id": 450
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
],
"metadata": {}
}
}
},
{
"names": [
"Wf121InitializationFailed"
],
"ID": 2,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Event",
"py/state": {
"name": "Wf121InitializationFailed",
"ID": 2,
"severity": "FATAL",
"format_string": "WF121 Failed to initialize",
"args": [],
"metadata": {}
}
}
},
{
"names": [
"RadioCommunicationsModeChange"
],
"ID": 3,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Event",
"py/state": {
"name": "RadioCommunicationsModeChange",
"ID": 3,
"severity": "ACTIVITY_HI",
"format_string": "Radio Communications Mode Changed %s -> %s",
"args": [
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "from_mode",
"type": {
"py/id": 296
},
"enum": [
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "FROM_HERCULES",
"value": 0,
"comment": "Normal mode. Hercules-Radio UART is ON inside Hercules and Hercules is the one talking to the Radio. Returning to this mode *might* not work, in which case you'll need to just send a reset-herc command to the WatchDog."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "FROM_EXTERNAL",
"value": 1,
"comment": "External mode. Hercules-Radio UART is turned OFF inside Hercules allowing an external device to talk to the Radio via J33. Used for debugging and programming the Radio. NOTE: in this mode, Hercules's RTS (which is connected to the Radio's CTS) is kept in the active/ready mode (LOW) so that the Radio feels free to send data whenever. This effectively deactivates control flow, though an external device can still listen to the Radio's RTS if needed - though this wasn't needed in development."
}
}
],
"bitfields": null,
"metadata": {}
}
},
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "to_mode",
"type": {
"py/id": 296
},
"enum": [
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "TO_HERCULES",
"value": 0,
"comment": "Normal mode. Hercules-Radio UART is ON inside Hercules and Hercules is the one talking to the Radio. Returning to this mode *might* not work, in which case you'll need to just send a reset-herc command to the WatchDog."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "TO_EXTERNAL",
"value": 1,
"comment": "External mode. Hercules-Radio UART is turned OFF inside Hercules allowing an external device to talk to the Radio via J33. Used for debugging and programming the Radio. NOTE: in this mode, Hercules's RTS (which is connected to the Radio's CTS) is kept in the active/ready mode (LOW) so that the Radio feels free to send data whenever. This effectively deactivates control flow, though an external device can still listen to the Radio's RTS if needed - though this wasn't needed in development."
}
}
],
"bitfields": null,
"metadata": {}
}
}
],
"metadata": {}
}
}
},
{
"names": [
"RadioCommunicationsModeState"
],
"ID": 4,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Event",
"py/state": {
"name": "RadioCommunicationsModeState",
"ID": 4,
"severity": "ACTIVITY_LO",
"format_string": "Radio Communications Mode now: %s",
"args": [
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "mode",
"type": {
"py/id": 296
},
"enum": [
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "NOW_HERCULES",
"value": 0,
"comment": "Normal mode. Hercules-Radio UART is ON inside Hercules and Hercules is the one talking to the Radio. Returning to this mode *might* not work, in which case you'll need to just send a reset-herc command to the WatchDog."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "NOW_EXTERNAL",
"value": 1,
"comment": "External mode. Hercules-Radio UART is turned OFF inside Hercules allowing an external device to talk to the Radio via J33. Used for debugging and programming the Radio. NOTE: in this mode, Hercules's RTS (which is connected to the Radio's CTS) is kept in the active/ready mode (LOW) so that the Radio feels free to send data whenever. This effectively deactivates control flow, though an external device can still listen to the Radio's RTS if needed - though this wasn't needed in development."
}
}
],
"bitfields": null,
"metadata": {}
}
}
],
"metadata": {}
}
}
},
{
"names": [
"RadioUartBaudRateChange"
],
"ID": 5,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Event",
"py/state": {
"name": "RadioUartBaudRateChange",
"ID": 5,
"severity": "ACTIVITY_HI",
"format_string": "Hercules' Radio UART Baud Rate changed (%s) %s -> %s",
"args": [
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "change_made",
"type": {
"py/id": 429
},
"enum": [],
"bitfields": null,
"metadata": {}
}
},
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "from_baud",
"type": {
"py/id": 15
},
"enum": [],
"bitfields": null,
"metadata": {}
}
},
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "to_baud",
"type": {
"py/id": 15
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
],
"metadata": {}
}
}
},
{
"names": [
"RadioBgApiPassthroughChange"
],
"ID": 6,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Event",
"py/state": {
"name": "RadioBgApiPassthroughChange",
"ID": 6,
"severity": "ACTIVITY_HI",
"format_string": "BGAPI Passthrough Mode changed (%s) %s -> %s",
"args": [
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "change_made",
"type": {
"py/id": 429
},
"enum": [],
"bitfields": null,
"metadata": {}
}
},
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "from",
"type": {
"py/id": 429
},
"enum": [],
"bitfields": null,
"metadata": {}
}
},
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "to",
"type": {
"py/id": 429
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
],
"metadata": {}
}
}
},
{
"names": [
"RadioSendBgApiCommandAck"
],
"ID": 7,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Event",
"py/state": {
"name": "RadioSendBgApiCommandAck",
"ID": 7,
"severity": "WARNING_LO",
"format_string": "Got BGAPI Command Send #%s. Target CRC: 0x%08X. Computed CRC: 0x%08X. Status: %s.",
"args": [
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "packet_id",
"type": {
"py/id": 15
},
"enum": [],
"bitfields": null,
"metadata": {}
}
},
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "target_crc_32",
"type": {
"py/id": 15
},
"enum": [],
"bitfields": null,
"metadata": {}
}
},
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "computed_crc_32",
"type": {
"py/id": 15
},
"enum": [],
"bitfields": null,
"metadata": {}
}
},
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "status",
"type": {
"py/id": 296
},
"enum": [
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "NM_BGAPI_SEND_BADEXPECTRESPONSEVAL",
"value": 195943031,
"comment": "Value for `expectResponse` didn't match any of the acceptable enum values. Suggests data corruption. Nothing will be sent to the Radio."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "NM_BGAPI_SEND_BADLEN",
"value": 195943031,
"comment": "Bad length received (decoded). Nothing was sent to the Radio. Either 0 or bigger than max possible size: WF121_BGAPI_PASSTHROUGH_MAX_MESSAGE_SIZE."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "NM_BGAPI_SEND_CRCFAIL",
"value": 3384867345,
"comment": "Computed CRC of data received did not match target CRC received. Nothing was sent to the Radio."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "NM_BGAPI_SEND_UARTSENDFAILED",
"value": 3665492497,
"comment": "All given data was valid but failed to send the packet to the Radio over UART. Try again?"
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "NM_BGAPI_SEND_BADSTATE",
"value": 3134552702,
"comment": "Hercules is in the wrong state to do this (not in passthrough mode - need to send `Set_Radio_BgApi_Passthrough[passthrough=TRUE]` first)."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "NM_BGAPI_SEND_SUCCESSNORESP",
"value": 330106,
"comment": "Data passed all validation and was sent to the Radio successfully over UART but no response was received from the radio (this can be expected some BGAPI DFU flashing commands)."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "NM_BGAPI_SEND_SUCCESS",
"value": 1611520634,
"comment": "Data passed all validation and was sent to the Radio successfully over UART and a response was received from the radio (this doesn't necessarily happen for some BGAPI DFU flashing commands)."
}
}
],
"bitfields": null,
"metadata": {}
}
}
],
"metadata": {}
}
}
},
{
"names": [
"RadioBgApiCommandRecords"
],
"ID": 8,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Event",
"py/state": {
"name": "RadioBgApiCommandRecords",
"ID": 8,
"severity": "WARNING_LO",
"format_string": "BGAPI Command Records (ID -> Result): \\n\\tLatest:(%s -> %s), \\n\\t2nd Latest:(%s -> %s), \\n\\tOldest:(%s -> %s)",
"args": [
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "packet_id_0",
"type": {
"py/id": 15
},
"enum": [],
"bitfields": null,
"metadata": {}
}
},
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "result_0",
"type": {
"py/id": 296
},
"enum": [
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "NM_BGAPI_REC_0_EMPTYRECORD",
"value": 47645,
"comment": "This record is empty (we haven't actually processed this many commands yet)."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "NM_BGAPI_REC_0_BADEXPECTRESPONSEVAL",
"value": 195943031,
"comment": "Value for `expectResponse` didn't match any of the acceptable enum values. Suggests data corruption. Nothing will be sent to the Radio."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "NM_BGAPI_REC_0_BADLEN",
"value": 195943031,
"comment": "Bad length received (decoded). Nothing was sent to the Radio. Either 0 or bigger than max possible size: WF121_BGAPI_PASSTHROUGH_MAX_MESSAGE_SIZE."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "NM_BGAPI_REC_0_CRCFAIL",
"value": 3384867345,
"comment": "Computed CRC of data received did not match target CRC received. Nothing was sent to the Radio."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "NM_BGAPI_REC_0_UARTSENDFAILED",
"value": 3665492497,
"comment": "All given data was valid but failed to send the packet to the Radio over UART. Try again?"
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "NM_BGAPI_REC_0_BADSTATE",
"value": 3134552702,
"comment": "Hercules is in the wrong state to do this (not in passthrough mode - need to send `Set_Radio_BgApi_Passthrough[passthrough=TRUE]` first)."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "NM_BGAPI_REC_0_SUCCESSNORESP",
"value": 330106,
"comment": "Data passed all validation and was sent to the Radio successfully over UART but no response was received from the radio, or we didn't look for a response if the command was flagged as `DONT_EXPECT_RESPONSE` (this can be expected for some BGAPI DFU flashing commands)."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "NM_BGAPI_REC_0_SUCCESS",
"value": 1611520634,
"comment": "Data passed all validation and was sent to the Radio successfully over UART and a response was received from the radio (this doesn't necessarily happen for some BGAPI DFU flashing commands)."
}
}
],
"bitfields": null,
"metadata": {}
}
},
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "packet_id_1",
"type": {
"py/id": 15
},
"enum": [],
"bitfields": null,
"metadata": {}
}
},
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "result_1",
"type": {
"py/id": 296
},
"enum": [
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "NM_BGAPI_REC_1_EMPTYRECORD",
"value": 47645,
"comment": "This record is empty (we haven't actually processed this many commands yet)."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "NM_BGAPI_REC_1_BADEXPECTRESPONSEVAL",
"value": 195943031,
"comment": "Value for `expectResponse` didn't match any of the acceptable enum values. Suggests data corruption. Nothing will be sent to the Radio."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "NM_BGAPI_REC_1_BADLEN",
"value": 195943031,
"comment": "Bad length received (decoded). Nothing was sent to the Radio. Either 0 or bigger than max possible size: WF121_BGAPI_PASSTHROUGH_MAX_MESSAGE_SIZE."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "NM_BGAPI_REC_1_CRCFAIL",
"value": 3384867345,
"comment": "Computed CRC of data received did not match target CRC received. Nothing was sent to the Radio."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "NM_BGAPI_REC_1_UARTSENDFAILED",
"value": 3665492497,
"comment": "All given data was valid but failed to send the packet to the Radio over UART. Try again?"
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "NM_BGAPI_REC_1_BADSTATE",
"value": 3134552702,
"comment": "Hercules is in the wrong state to do this (not in passthrough mode - need to send `Set_Radio_BgApi_Passthrough[passthrough=TRUE]` first)."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "NM_BGAPI_REC_1_SUCCESSNORESP",
"value": 330106,
"comment": "Data passed all validation and was sent to the Radio successfully over UART but no response was received from the radio, or we didn't look for a response if the command was flagged as `DONT_EXPECT_RESPONSE` (this can be expected for some BGAPI DFU flashing commands)."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "NM_BGAPI_REC_1_SUCCESS",
"value": 1611520634,
"comment": "Data passed all validation and was sent to the Radio successfully over UART and a response was received from the radio (this doesn't necessarily happen for some BGAPI DFU flashing commands)."
}
}
],
"bitfields": null,
"metadata": {}
}
},
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "packet_id_2",
"type": {
"py/id": 15
},
"enum": [],
"bitfields": null,
"metadata": {}
}
},
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "result_2",
"type": {
"py/id": 296
},
"enum": [
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "NM_BGAPI_REC_2_EMPTYRECORD",
"value": 47645,
"comment": "This record is empty (we haven't actually processed this many commands yet)."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "NM_BGAPI_REC_2_BADEXPECTRESPONSEVAL",
"value": 195943031,
"comment": "Value for `expectResponse` didn't match any of the acceptable enum values. Suggests data corruption. Nothing will be sent to the Radio."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "NM_BGAPI_REC_2_BADLEN",
"value": 195943031,
"comment": "Bad length received (decoded). Nothing was sent to the Radio. Either 0 or bigger than max possible size: WF121_BGAPI_PASSTHROUGH_MAX_MESSAGE_SIZE."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "NM_BGAPI_REC_2_CRCFAIL",
"value": 3384867345,
"comment": "Computed CRC of data received did not match target CRC received. Nothing was sent to the Radio."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "NM_BGAPI_REC_2_UARTSENDFAILED",
"value": 3665492497,
"comment": "All given data was valid but failed to send the packet to the Radio over UART. Try again?"
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "NM_BGAPI_REC_2_BADSTATE",
"value": 3134552702,
"comment": "Hercules is in the wrong state to do this (not in passthrough mode - need to send `Set_Radio_BgApi_Passthrough[passthrough=TRUE]` first)."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "NM_BGAPI_REC_2_SUCCESSNORESP",
"value": 330106,
"comment": "Data passed all validation and was sent to the Radio successfully over UART but no response was received from the radio, or we didn't look for a response if the command was flagged as `DONT_EXPECT_RESPONSE` (this can be expected for some BGAPI DFU flashing commands)."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "NM_BGAPI_REC_2_SUCCESS",
"value": 1611520634,
"comment": "Data passed all validation and was sent to the Radio successfully over UART and a response was received from the radio (this doesn't necessarily happen for some BGAPI DFU flashing commands)."
}
}
],
"bitfields": null,
"metadata": {}
}
}
],
"metadata": {}
}
}
}
]
}
}
}
},
{
"names": [
"ActiveLogger"
],
"ID": 2816,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Module",
"py/state": {
"name": "ActiveLogger",
"ID": 2816,
"commands": {
"py/object": "IrisBackendv3.utils.nameiddict.NameIdDict",
"py/state": [
{
"names": [
"ActiveLogger_Alogseteventreportfilter"
],
"ID": 0,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Command",
"py/state": {
"name": "ActiveLogger_Alogseteventreportfilter",
"mnemonic": "ALOG_SET_EVENT_REPORT_FILTER",
"ID": 0,
"args": [
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "filter_level",
"type": {
"py/id": 296
},
"enum": [
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "INPUTWARNINGHI",
"value": 0,
"comment": ""
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "INPUTWARNINGLO",
"value": 1,
"comment": ""
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "INPUTCOMMAND",
"value": 2,
"comment": ""
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "INPUTACTIVITYHI",
"value": 3,
"comment": ""
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "INPUTACTIVITYLO",
"value": 4,
"comment": ""
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "INPUTDIAGNOSTIC",
"value": 5,
"comment": ""
}
}
],
"bitfields": null,
"metadata": {}
}
},
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "filter_enable",
"type": {
"py/id": 296
},
"enum": [
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "INPUTENABLED",
"value": 0,
"comment": ""
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "INPUTDISABLED",
"value": 1,
"comment": ""
}
}
],
"bitfields": null,
"metadata": {}
}
}
],
"metadata": {}
}
}
},
{
"names": [
"ActiveLogger_Alogseteventsendfilter"
],
"ID": 1,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Command",
"py/state": {
"name": "ActiveLogger_Alogseteventsendfilter",
"mnemonic": "ALOG_SET_EVENT_SEND_FILTER",
"ID": 1,
"args": [
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "filter_level",
"type": {
"py/id": 296
},
"enum": [
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "SENDWARNINGHI",
"value": 0,
"comment": ""
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "SENDWARNINGLO",
"value": 1,
"comment": ""
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "SENDCOMMAND",
"value": 2,
"comment": ""
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "SENDACTIVITYHI",
"value": 3,
"comment": ""
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "SENDACTIVITYLO",
"value": 4,
"comment": ""
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "SENDDIAGNOSTIC",
"value": 5,
"comment": ""
}
}
],
"bitfields": null,
"metadata": {}
}
},
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "filter_enable",
"type": {
"py/id": 296
},
"enum": [
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "SENDENABLED",
"value": 0,
"comment": ""
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "SENDDISABLED",
"value": 1,
"comment": ""
}
}
],
"bitfields": null,
"metadata": {}
}
}
],
"metadata": {}
}
}
},
{
"names": [
"ActiveLogger_Alogdumpeventlog"
],
"ID": 2,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Command",
"py/state": {
"name": "ActiveLogger_Alogdumpeventlog",
"mnemonic": "ALOG_DUMP_EVENT_LOG",
"ID": 2,
"args": [
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "filename",
"type": {
"py/id": 196
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
],
"metadata": {}
}
}
},
{
"names": [
"ActiveLogger_Alogsetidfilter"
],
"ID": 3,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Command",
"py/state": {
"name": "ActiveLogger_Alogsetidfilter",
"mnemonic": "ALOG_SET_ID_FILTER",
"ID": 3,
"args": [
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "id",
"type": {
"py/id": 15
},
"enum": [],
"bitfields": null,
"metadata": {}
}
},
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "id_filter_enable",
"type": {
"py/id": 296
},
"enum": [
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "IDENABLED",
"value": 0,
"comment": ""
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "IDDISABLED",
"value": 1,
"comment": ""
}
}
],
"bitfields": null,
"metadata": {}
}
}
],
"metadata": {}
}
}
},
{
"names": [
"ActiveLogger_Alogdumpfilterstate"
],
"ID": 4,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Command",
"py/state": {
"name": "ActiveLogger_Alogdumpfilterstate",
"mnemonic": "ALOG_DUMP_FILTER_STATE",
"ID": 4,
"args": [],
"metadata": {}
}
}
}
]
},
"telemetry": {
"py/object": "IrisBackendv3.utils.nameiddict.NameIdDict",
"id_dict": {},
"name_id_mapping": {}
},
"events": {
"py/object": "IrisBackendv3.utils.nameiddict.NameIdDict",
"py/state": [
{
"names": [
"Alogfilewriteerr"
],
"ID": 0,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Event",
"py/state": {
"name": "Alogfilewriteerr",
"ID": 0,
"severity": "WARNING_HI",
"format_string": "Event buffer write failed in stage %s with error %s",
"args": [
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "stage",
"type": {
"py/id": 296
},
"enum": [
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "LOGWRITEOPEN",
"value": 0,
"comment": ""
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "LOGWRITEFATALDELIMETER",
"value": 1,
"comment": ""
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "LOGWRITEFATALRECORD",
"value": 2,
"comment": ""
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "LOGWRITEWARNINGHIDELIMETER",
"value": 3,
"comment": ""
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "LOGWRITEWARNINGHIRECORD",
"value": 4,
"comment": ""
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "LOGWRITEWARNINGLODELIMETER",
"value": 5,
"comment": ""
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "LOGWRITEWARNINGLORECORD",
"value": 6,
"comment": ""
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "LOGWRITECOMMANDDELIMETER",
"value": 7,
"comment": ""
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "LOGWRITECOMMANDRECORD",
"value": 8,
"comment": ""
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "LOGWRITEACTIVITYHIDELIMETER",
"value": 9,
"comment": ""
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "LOGWRITEACTIVITYHIRECORD",
"value": 10,
"comment": ""
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "LOGWRITEACTIVITYLODELIMETER",
"value": 11,
"comment": ""
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "LOGWRITEACTIVITYLORECORD",
"value": 12,
"comment": ""
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "LOGWRITEDIAGNOSTICDELIMETER",
"value": 13,
"comment": ""
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "LOGWRITEDIAGNOSTICRECORD",
"value": 14,
"comment": ""
}
}
],
"bitfields": null,
"metadata": {}
}
},
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "error",
"type": {
"py/id": 207
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
],
"metadata": {}
}
}
},
{
"names": [
"Alogfilewritecomplete"
],
"ID": 1,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Event",
"py/state": {
"name": "Alogfilewritecomplete",
"ID": 1,
"severity": "ACTIVITY_HI",
"format_string": "Event buffer write complete. Wrote %s records.",
"args": [
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "records",
"type": {
"py/id": 15
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
],
"metadata": {}
}
}
},
{
"names": [
"Alogseverityfilterstate"
],
"ID": 2,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Event",
"py/state": {
"name": "Alogseverityfilterstate",
"ID": 2,
"severity": "ACTIVITY_LO",
"format_string": "%s filter state. Recv: %s Send: %s",
"args": [
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "severity",
"type": {
"py/id": 296
},
"enum": [
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "EVENTFILTERWARNINGHI",
"value": 0,
"comment": ""
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "EVENTFILTERWARNINGLO",
"value": 1,
"comment": ""
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "EVENTFILTERCOMMAND",
"value": 2,
"comment": ""
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "EVENTFILTERACTIVITYHI",
"value": 3,
"comment": ""
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "EVENTFILTERACTIVITYLO",
"value": 4,
"comment": ""
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "EVENTFILTERDIAGNOSTIC",
"value": 5,
"comment": ""
}
}
],
"bitfields": null,
"metadata": {}
}
},
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "recv_enabled",
"type": {
"py/id": 429
},
"enum": [],
"bitfields": null,
"metadata": {}
}
},
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "send_enabled",
"type": {
"py/id": 429
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
],
"metadata": {}
}
}
},
{
"names": [
"Alogidfilterenabled"
],
"ID": 3,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Event",
"py/state": {
"name": "Alogidfilterenabled",
"ID": 3,
"severity": "ACTIVITY_HI",
"format_string": "ID %s is filtered.",
"args": [
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "id",
"type": {
"py/id": 15
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
],
"metadata": {}
}
}
},
{
"names": [
"Alogidfilterlistfull"
],
"ID": 4,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Event",
"py/state": {
"name": "Alogidfilterlistfull",
"ID": 4,
"severity": "WARNING_LO",
"format_string": "ID filter list is full. Cannot filter %s .",
"args": [
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "id",
"type": {
"py/id": 15
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
],
"metadata": {}
}
}
},
{
"names": [
"Alogidfilterremoved"
],
"ID": 5,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Event",
"py/state": {
"name": "Alogidfilterremoved",
"ID": 5,
"severity": "ACTIVITY_HI",
"format_string": "ID filter ID %s removed.",
"args": [
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "id",
"type": {
"py/id": 15
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
],
"metadata": {}
}
}
},
{
"names": [
"Alogidfilternotfound"
],
"ID": 6,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Event",
"py/state": {
"name": "Alogidfilternotfound",
"ID": 6,
"severity": "WARNING_LO",
"format_string": "ID filter ID %s not found.",
"args": [
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "id",
"type": {
"py/id": 15
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
],
"metadata": {}
}
}
}
]
}
}
}
},
{
"names": [
"Navigation"
],
"ID": 3328,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Module",
"py/state": {
"name": "Navigation",
"ID": 3328,
"commands": {
"py/object": "IrisBackendv3.utils.nameiddict.NameIdDict",
"py/state": [
{
"names": [
"Navigation_NavDriveForward"
],
"ID": 0,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Command",
"py/state": {
"name": "Navigation_NavDriveForward",
"mnemonic": "Nav_DriveForward",
"ID": 0,
"args": [
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "distance",
"type": {
"py/id": 217
},
"enum": [],
"bitfields": null,
"metadata": {}
}
},
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "speed",
"type": {
"py/id": 217
},
"enum": [],
"bitfields": null,
"metadata": {}
}
},
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "callback_id",
"type": {
"py/id": 450
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
],
"metadata": {}
}
}
},
{
"names": [
"Navigation_NavDriveBackwards"
],
"ID": 1,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Command",
"py/state": {
"name": "Navigation_NavDriveBackwards",
"mnemonic": "Nav_DriveBackwards",
"ID": 1,
"args": [
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "distance",
"type": {
"py/id": 217
},
"enum": [],
"bitfields": null,
"metadata": {}
}
},
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "speed",
"type": {
"py/id": 217
},
"enum": [],
"bitfields": null,
"metadata": {}
}
},
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "callback_id",
"type": {
"py/id": 450
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
],
"metadata": {}
}
}
},
{
"names": [
"Navigation_NavRotateRight"
],
"ID": 2,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Command",
"py/state": {
"name": "Navigation_NavRotateRight",
"mnemonic": "Nav_RotateRight",
"ID": 2,
"args": [
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "distance",
"type": {
"py/id": 217
},
"enum": [],
"bitfields": null,
"metadata": {}
}
},
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "speed",
"type": {
"py/id": 217
},
"enum": [],
"bitfields": null,
"metadata": {}
}
},
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "callback_id",
"type": {
"py/id": 450
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
],
"metadata": {}
}
}
},
{
"names": [
"Navigation_NavRotateLeft"
],
"ID": 3,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Command",
"py/state": {
"name": "Navigation_NavRotateLeft",
"mnemonic": "Nav_RotateLeft",
"ID": 3,
"args": [
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "distance",
"type": {
"py/id": 217
},
"enum": [],
"bitfields": null,
"metadata": {}
}
},
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "speed",
"type": {
"py/id": 217
},
"enum": [],
"bitfields": null,
"metadata": {}
}
},
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "callback_id",
"type": {
"py/id": 450
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
],
"metadata": {}
}
}
},
{
"names": [
"Navigation_NavStopMovement"
],
"ID": 4,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Command",
"py/state": {
"name": "Navigation_NavStopMovement",
"mnemonic": "Nav_StopMovement",
"ID": 4,
"args": [],
"metadata": {}
}
}
}
]
},
"telemetry": {
"py/object": "IrisBackendv3.utils.nameiddict.NameIdDict",
"py/state": [
{
"names": [
"NavMovedForward"
],
"ID": 0,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "NavMovedForward",
"ID": 0,
"type": {
"py/id": 450
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"NavMovedBackward"
],
"ID": 1,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "NavMovedBackward",
"ID": 1,
"type": {
"py/id": 450
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"NavTurnedLeft"
],
"ID": 2,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "NavTurnedLeft",
"ID": 2,
"type": {
"py/id": 450
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"NavTurnedRight"
],
"ID": 3,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "NavTurnedRight",
"ID": 3,
"type": {
"py/id": 450
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"NavImageSize"
],
"ID": 4,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "NavImageSize",
"ID": 4,
"type": {
"py/id": 450
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
}
]
},
"events": {
"py/object": "IrisBackendv3.utils.nameiddict.NameIdDict",
"py/state": [
{
"names": [
"NavMoveInterrupted"
],
"ID": 0,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Event",
"py/state": {
"name": "NavMoveInterrupted",
"ID": 0,
"severity": "COMMAND",
"format_string": "A move has been interrupted",
"args": [],
"metadata": {}
}
}
},
{
"names": [
"NavMoveQueueLimit"
],
"ID": 1,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Event",
"py/state": {
"name": "NavMoveQueueLimit",
"ID": 1,
"severity": "WARNING_HI",
"format_string": "The latest move would overwrite a move not completed",
"args": [],
"metadata": {}
}
}
}
]
}
}
}
},
{
"names": [
"MotorControl"
],
"ID": 3584,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Module",
"py/state": {
"name": "MotorControl",
"ID": 3584,
"commands": {
"py/object": "IrisBackendv3.utils.nameiddict.NameIdDict",
"py/state": [
{
"names": [
"MotorControl_McCurrentPid"
],
"ID": 0,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Command",
"py/state": {
"name": "MotorControl_McCurrentPid",
"mnemonic": "MC_Current_PID",
"ID": 0,
"args": [
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "motor_id",
"type": {
"py/id": 217
},
"enum": [],
"bitfields": null,
"metadata": {}
}
},
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "pi_values",
"type": {
"py/id": 15
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
],
"metadata": {}
}
}
},
{
"names": [
"MotorControl_McSpeedPid"
],
"ID": 1,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Command",
"py/state": {
"name": "MotorControl_McSpeedPid",
"mnemonic": "MC_Speed_PID",
"ID": 1,
"args": [
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "motor_id",
"type": {
"py/id": 217
},
"enum": [],
"bitfields": null,
"metadata": {}
}
},
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "pid_values",
"type": {
"py/id": 15
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
],
"metadata": {}
}
}
},
{
"names": [
"MotorControl_McAcceleration"
],
"ID": 3,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Command",
"py/state": {
"name": "MotorControl_McAcceleration",
"mnemonic": "MC_Acceleration",
"ID": 3,
"args": [
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "motor_id",
"type": {
"py/id": 217
},
"enum": [],
"bitfields": null,
"metadata": {}
}
},
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "rate_values",
"type": {
"py/id": 15
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
],
"metadata": {}
}
}
},
{
"names": [
"MotorControl_McStallDetection"
],
"ID": 4,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Command",
"py/state": {
"name": "MotorControl_McStallDetection",
"mnemonic": "MC_StallDetection",
"ID": 4,
"args": [
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "motor_id",
"type": {
"py/id": 217
},
"enum": [],
"bitfields": null,
"metadata": {}
}
},
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "value",
"type": {
"py/id": 217
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
],
"metadata": {}
}
}
},
{
"names": [
"MotorControl_McResetPosition"
],
"ID": 5,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Command",
"py/state": {
"name": "MotorControl_McResetPosition",
"mnemonic": "MC_ResetPosition",
"ID": 5,
"args": [
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "motor_id",
"type": {
"py/id": 217
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
],
"metadata": {}
}
}
},
{
"names": [
"MotorControl_McSpin"
],
"ID": 6,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Command",
"py/state": {
"name": "MotorControl_McSpin",
"mnemonic": "MC_Spin",
"ID": 6,
"args": [
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "motor_id",
"type": {
"py/id": 217
},
"enum": [],
"bitfields": null,
"metadata": {}
}
},
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "raw_ticks",
"type": {
"py/id": 15
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
],
"metadata": {}
}
}
},
{
"names": [
"MotorControl_McPowerBoost"
],
"ID": 7,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Command",
"py/state": {
"name": "MotorControl_McPowerBoost",
"mnemonic": "MC_PowerBoost",
"ID": 7,
"args": [
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "motor_id",
"type": {
"py/id": 217
},
"enum": [],
"bitfields": null,
"metadata": {}
}
},
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "value",
"type": {
"py/id": 217
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
],
"metadata": {}
}
}
},
{
"names": [
"MotorControl_McSetParameter"
],
"ID": 8,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Command",
"py/state": {
"name": "MotorControl_McSetParameter",
"mnemonic": "MC_SetParameter",
"ID": 8,
"args": [
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "param_select",
"type": {
"py/id": 296
},
"enum": [
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "MC_PARAM_1",
"value": 0,
"comment": ""
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "MC_PARAM_2",
"value": 1,
"comment": ""
}
}
],
"bitfields": null,
"metadata": {}
}
},
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "new_value",
"type": {
"py/id": 15
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
],
"metadata": {}
}
}
},
{
"names": [
"MotorControl_McUpdateTelemetry"
],
"ID": 16,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Command",
"py/state": {
"name": "MotorControl_McUpdateTelemetry",
"mnemonic": "MC_UpdateTelemetry",
"ID": 16,
"args": [],
"metadata": {}
}
}
}
]
},
"telemetry": {
"py/object": "IrisBackendv3.utils.nameiddict.NameIdDict",
"py/state": [
{
"names": [
"McflEncoderTicks"
],
"ID": 0,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "McflEncoderTicks",
"ID": 0,
"type": {
"py/id": 207
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"McflCurrent"
],
"ID": 1,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "McflCurrent",
"ID": 1,
"type": {
"py/id": 450
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"McfrEncoderTicks"
],
"ID": 3,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "McfrEncoderTicks",
"ID": 3,
"type": {
"py/id": 207
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"McfrCurrent"
],
"ID": 4,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "McfrCurrent",
"ID": 4,
"type": {
"py/id": 450
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"McrrEncoderTicks"
],
"ID": 6,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "McrrEncoderTicks",
"ID": 6,
"type": {
"py/id": 207
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"McrrCurrent"
],
"ID": 7,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "McrrCurrent",
"ID": 7,
"type": {
"py/id": 450
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"McrlEncoderTicks"
],
"ID": 9,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "McrlEncoderTicks",
"ID": 9,
"type": {
"py/id": 207
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"McrlCurrent"
],
"ID": 10,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "McrlCurrent",
"ID": 10,
"type": {
"py/id": 450
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"McflStatus"
],
"ID": 11,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "McflStatus",
"ID": 11,
"type": {
"py/id": 217
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"McfrStatus"
],
"ID": 12,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "McfrStatus",
"ID": 12,
"type": {
"py/id": 217
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"McrrStatus"
],
"ID": 13,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "McrrStatus",
"ID": 13,
"type": {
"py/id": 217
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"McrlStatus"
],
"ID": 14,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "McrlStatus",
"ID": 14,
"type": {
"py/id": 217
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
}
]
},
"events": {
"py/object": "IrisBackendv3.utils.nameiddict.NameIdDict",
"py/state": [
{
"names": [
"McMoveStarted"
],
"ID": 0,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Event",
"py/state": {
"name": "McMoveStarted",
"ID": 0,
"severity": "COMMAND",
"format_string": "Move step has started",
"args": [],
"metadata": {}
}
}
},
{
"names": [
"McMoveComplete"
],
"ID": 1,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Event",
"py/state": {
"name": "McMoveComplete",
"ID": 1,
"severity": "COMMAND",
"format_string": "Move step has finished",
"args": [],
"metadata": {}
}
}
},
{
"names": [
"McmspFaultDetected"
],
"ID": 2,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Event",
"py/state": {
"name": "McmspFaultDetected",
"ID": 2,
"severity": "WARNING_HI",
"format_string": "Motor controller has detected a fault",
"args": [],
"metadata": {}
}
}
},
{
"names": [
"McmspNotResponding"
],
"ID": 3,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Event",
"py/state": {
"name": "McmspNotResponding",
"ID": 3,
"severity": "WARNING_HI",
"format_string": "Motor controller not responding",
"args": [],
"metadata": {}
}
}
},
{
"names": [
"McmspNotReporting"
],
"ID": 4,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Event",
"py/state": {
"name": "McmspNotReporting",
"ID": 4,
"severity": "WARNING_HI",
"format_string": "Motor controller not communicating requested data",
"args": [],
"metadata": {}
}
}
},
{
"names": [
"McParameterChanged"
],
"ID": 5,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Event",
"py/state": {
"name": "McParameterChanged",
"ID": 5,
"severity": "COMMAND",
"format_string": "A Motor controller parameter has been changed",
"args": [],
"metadata": {}
}
}
},
{
"names": [
"McEncoderReset"
],
"ID": 6,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Event",
"py/state": {
"name": "McEncoderReset",
"ID": 6,
"severity": "COMMAND",
"format_string": "A Motor encoder count has been reset",
"args": [],
"metadata": {}
}
}
}
]
}
}
}
},
{
"names": [
"Imu"
],
"ID": 3840,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Module",
"py/state": {
"name": "Imu",
"ID": 3840,
"commands": {
"py/object": "IrisBackendv3.utils.nameiddict.NameIdDict",
"py/state": [
{
"names": [
"Imu_ImuReportData"
],
"ID": 0,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Command",
"py/state": {
"name": "Imu_ImuReportData",
"mnemonic": "IMU_ReportData",
"ID": 0,
"args": [],
"metadata": {}
}
}
},
{
"names": [
"Imu_ImuTurnOn"
],
"ID": 1,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Command",
"py/state": {
"name": "Imu_ImuTurnOn",
"mnemonic": "IMU_TurnOn",
"ID": 1,
"args": [],
"metadata": {}
}
}
},
{
"names": [
"Imu_ImuTurnOff"
],
"ID": 2,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Command",
"py/state": {
"name": "Imu_ImuTurnOff",
"mnemonic": "IMU_TurnOff",
"ID": 2,
"args": [],
"metadata": {}
}
}
}
]
},
"telemetry": {
"py/object": "IrisBackendv3.utils.nameiddict.NameIdDict",
"py/state": [
{
"names": [
"XAcc"
],
"ID": 0,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "XAcc",
"ID": 0,
"type": {
"py/reduce": [
{
"py/type": "IrisBackendv3.data_standards.fsw_data_type.FswDataType"
},
{
"py/tuple": [
"int16"
]
}
]
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"YAcc"
],
"ID": 1,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "YAcc",
"ID": 1,
"type": {
"py/id": 1799
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"ZAcc"
],
"ID": 2,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "ZAcc",
"ID": 2,
"type": {
"py/id": 1799
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"XAng"
],
"ID": 3,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "XAng",
"ID": 3,
"type": {
"py/id": 450
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"YAng"
],
"ID": 4,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "YAng",
"ID": 4,
"type": {
"py/id": 450
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"ZAng"
],
"ID": 5,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "ZAng",
"ID": 5,
"type": {
"py/id": 450
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
}
]
},
"events": {
"py/object": "IrisBackendv3.utils.nameiddict.NameIdDict",
"py/state": [
{
"names": [
"ImuAngleWarning"
],
"ID": 0,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Event",
"py/state": {
"name": "ImuAngleWarning",
"ID": 0,
"severity": "WARNING_HI",
"format_string": "High roll/tilt angle",
"args": [],
"metadata": {}
}
}
},
{
"names": [
"ImuCommunicationFailure"
],
"ID": 1,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Event",
"py/state": {
"name": "ImuCommunicationFailure",
"ID": 1,
"severity": "WARNING_HI",
"format_string": "Communication failure with IMU",
"args": [],
"metadata": {}
}
}
},
{
"names": [
"ImuPoweredOn"
],
"ID": 2,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Event",
"py/state": {
"name": "ImuPoweredOn",
"ID": 2,
"severity": "COMMAND",
"format_string": "IMU module turned on",
"args": [],
"metadata": {}
}
}
},
{
"names": [
"ImuPoweredOff"
],
"ID": 3,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Event",
"py/state": {
"name": "ImuPoweredOff",
"ID": 3,
"severity": "COMMAND",
"format_string": "IMU module turned off",
"args": [],
"metadata": {}
}
}
}
]
}
}
}
},
{
"names": [
"WatchDogInterface"
],
"ID": 4096,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Module",
"py/state": {
"name": "WatchDogInterface",
"ID": 4096,
"commands": {
"py/object": "IrisBackendv3.utils.nameiddict.NameIdDict",
"py/state": [
{
"names": [
"WatchDogInterface_ResetSpecific"
],
"ID": 0,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Command",
"py/state": {
"name": "WatchDogInterface_ResetSpecific",
"mnemonic": "Reset_Specific",
"ID": 0,
"args": [
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "reset_value",
"type": {
"py/id": 296
},
"enum": [
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "NO_RESET",
"value": 0,
"comment": ""
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "RESET_HERCULES",
"value": 1,
"comment": ""
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "HERCULES_POWER_ON",
"value": 2,
"comment": ""
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "HERCULES_POWER_OFF",
"value": 3,
"comment": ""
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "RESET_RADIO",
"value": 4,
"comment": ""
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "RADIO_POWER_ON",
"value": 5,
"comment": ""
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "RADIO_POWER_OFF",
"value": 6,
"comment": ""
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "RESET_FPGA",
"value": 7,
"comment": ""
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "FPGA_POWER_ON",
"value": 8,
"comment": ""
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "FPGA_POWER_OFF",
"value": 9,
"comment": ""
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "RESET_MOTOR_1",
"value": 10,
"comment": "Reset, wait, then Unreset Motor Controller 1 (A) MCU."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "RESET_MOTOR_2",
"value": 11,
"comment": "Reset, wait, then Unreset Motor Controller 2 (B) MCU."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "RESET_MOTOR_3",
"value": 12,
"comment": "Reset, wait, then Unreset Motor Controller 3 (C) MCU."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "RESET_MOTOR_4",
"value": 13,
"comment": "Reset, wait, then Unreset Motor Controller 4 (D) MCU."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "RESET_HOLD_MOTOR_1",
"value": 160,
"comment": "Reset and hold the reset for Motor Controller 1 (A) MCU."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "RESET_HOLD_MOTOR_2",
"value": 161,
"comment": "Reset and hold the reset for Motor Controller 2 (B) MCU."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "RESET_HOLD_MOTOR_3",
"value": 162,
"comment": "Reset and hold the reset for Motor Controller 3 (C) MCU."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "RESET_HOLD_MOTOR_4",
"value": 163,
"comment": "Reset and hold the reset for Motor Controller 4 (D) MCU."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "RESET_RELEASE_MOTOR_1",
"value": 175,
"comment": "Release the Reset for (unreset) Motor Controller 1 (A) MCU."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "RESET_RELEASE_MOTOR_2",
"value": 174,
"comment": "Release the Reset for (unreset) Motor Controller 2 (B) MCU."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "RESET_RELEASE_MOTOR_3",
"value": 173,
"comment": "Release the Reset for (unreset) Motor Controller 3 (C) MCU."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "RESET_RELEASE_MOTOR_4",
"value": 172,
"comment": "Release the Reset for (unreset) Motor Controller 4 (D) MCU."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "RESET_HOLD_MOTORS_LEFT",
"value": 176,
"comment": "Reset and hold the reset for the Motor Controllers on the Left of the Rover (A, D). For emergency steering."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "RESET_HOLD_MOTORS_RIGHT",
"value": 177,
"comment": "Reset and hold the reset for the Motor Controllers on the Right of the Rover (B, C). For emergency steering."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "RESET_HOLD_MOTORS_FRONT",
"value": 178,
"comment": "Reset and hold the reset for the Motor Controllers on the Front of the Rover (A, B). For emergency steering."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "RESET_HOLD_MOTORS_REAR",
"value": 179,
"comment": "Reset and hold the reset for the Motor Controllers on the Rear of the Rover (C, D). For emergency steering."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "RESET_HOLD_MOTORS_DIAG_AC",
"value": 180,
"comment": "Reset and hold the reset for the Motor Controllers on the A-C diagonal (A, C). For emergency steering."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "RESET_HOLD_MOTORS_DIAG_DB",
"value": 181,
"comment": "Reset and hold the reset for the Motor Controllers on the D-B diagonal (D, B). For emergency steering."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "RESET_RELEASE_MOTORS_LEFT",
"value": 191,
"comment": "Release the Reset for (unreset) the Motor Controllers on the Left of the Rover (A, D). For emergency steering."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "RESET_RELEASE_MOTORS_RIGHT",
"value": 190,
"comment": "Release the Reset for (unreset) the Motor Controllers on the Right of the Rover (B, C). For emergency steering."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "RESET_RELEASE_MOTORS_FRONT",
"value": 189,
"comment": "Release the Reset for (unreset) the Motor Controllers on the Front of the Rover (A, B). For emergency steering."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "RESET_RELEASE_MOTORS_REAR",
"value": 188,
"comment": "Release the Reset for (unreset) the Motor Controllers on the Rear of the Rover (C, D). For emergency steering."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "RESET_RELEASE_MOTORS_DIAG_AC",
"value": 187,
"comment": "Release the Reset for (unreset) the Motor Controllers on the A-C diagonal (A, C). For emergency steering."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "RESET_RELEASE_MOTORS_DIAG_DB",
"value": 186,
"comment": "Release the Reset for (unreset) the Motor Controllers on the D-B diagonal (D, B). For emergency steering."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "RESET_HOLD_ALL_MOTORS",
"value": 185,
"comment": "Reset and hold all Motor Controller MCUs."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "RESET_ALL_MOTORS",
"value": 14,
"comment": "Reset, wait, then Unreset all Motor Controller MCUs."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "ALL_MOTORS_ON",
"value": 15,
"comment": ""
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "ALL_MOTORS_OFF",
"value": 16,
"comment": ""
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "RESET_EN_3_3",
"value": 17,
"comment": ""
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "EN_3_3_POWER_ON",
"value": 18,
"comment": ""
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "EN_3_3_POWER_OFF",
"value": 19,
"comment": ""
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "SYSTEM_POWER_CYCLE",
"value": 20,
"comment": "V_SYS_ALL OFF, wait, then ON"
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "SYSTEM_ON",
"value": 21,
"comment": "V_SYS_ALL ON"
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "SYSTEM_OFF",
"value": 22,
"comment": "V_SYS_ALL OFF"
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "HDRM_OFF",
"value": 24,
"comment": ""
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "FPGA_CAM_0",
"value": 25,
"comment": ""
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "FPGA_CAM_1",
"value": 26,
"comment": ""
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "BATTERY_START_CHARGE",
"value": 27,
"comment": ""
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "BATTERY_STOP_CHARGE",
"value": 28,
"comment": ""
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "ENABLE_RS_422_UART",
"value": 29,
"comment": ""
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "DISABLE_RS_422_UART",
"value": 30,
"comment": ""
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "AUTO_HEATER_CONTROLLER_ENABLE",
"value": 31,
"comment": "Activates the heater controller, with a starting state of ON."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "AUTO_HEATER_CONTROLLER_DISABLE",
"value": 32,
"comment": "Deactivates the heater controller and turns the heater OFF."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "HEATER_FORCE_OFF",
"value": 192,
"comment": "Forces the Heater to always be OFF, no matter the controller state."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "HEATER_FORCE_ON",
"value": 193,
"comment": "Forces the Heater to always be ON (at the specified duty cycle), no matter the controller state."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "HEATER_FORCE_NOTHING",
"value": 194,
"comment": "Doesn't force the heater to do anything (allows the heater controller to take back over if it's on). Leaves the heater status unchanged."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "AUTO_HEATER_CONTROLLER_USE_RT_INPUT",
"value": 207,
"comment": "Makes the heater controller use the normal Battery RT Thermistor as its input (NOTE: changing this should be accompanied by changing the ADC thresholds)."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "AUTO_HEATER_CONTROLLER_USE_CHARGER_INPUT_DEACTIVATE",
"value": 206,
"comment": "Deactivate charging circuitry used to allow Auto Heater Controller to use Charger Thermistor as the temperature input (for use if returning to BATT_RT from CHRG)."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "AUTO_HEATER_CONTROLLER_USE_CHARGER_INPUT_ACTIVATE",
"value": 205,
"comment": "Activate charging circuitry but disable charging so the Auto Heater Controller can use Charger Thermistor as the temperature input (needed for `WD_CMD_MSGS__RESET_ID__AUTO_HEATER_CONTROLLER_USE_CHARGER_INPUT` to work)."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "AUTO_HEATER_CONTROLLER_USE_CHARGER_INPUT",
"value": 204,
"comment": "Makes the heater controller use the special Battery Charging Thermistor as its input. ONLY TO BE USED IN A LAST DITCH EFFORT IF BATT_RT DOESN'T WORK. Using this would require `WD_CMD_MSGS__RESET_ID__AUTO_HEATER_CONTROLLER_USE_CHARGER_INPUT_ACTIVATE`, which would increase power wastage and is **RISKY**. (NOTE: changing this should be accompanied by changing the ADC thresholds)."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "HERCULES_WATCHDOG_ENABLE",
"value": 33,
"comment": "Enable Hercules Watchdog behavior (Watchdog resets Hercules if it's unresponsive)."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "HERCULES_WATCHDOG_DISABLE",
"value": 34,
"comment": "Disable Hercules Watchdog behavior (Watchdog **DOESN'T** reset Hercules if it's unresponsive)."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "ENABLE_BATTERIES",
"value": 35,
"comment": "Properly enable the batteries (with persistent latching)."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "DISABLE_BATTERIES",
"value": 36,
"comment": "Properly disable the batteries (with persistent latching)."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "CLEAR_PERSISTENT_DEPLOY",
"value": 221,
"comment": "Clear the persistent 'deployed' status (in case it comes on erroneously during transit - e.g. due to cosmic radiation)."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "HDRM_DEPLOY_SIGNAL_POWER_ON",
"value": 238,
"comment": "Have the MSP430 Watchdog raise **its** deploy interlock signal. NOTE: This is not the normal way to do this. Only due this if the normal deploy pathway isn't working for some reason."
}
}
],
"bitfields": null,
"metadata": {}
}
}
],
"metadata": {}
}
}
},
{
"names": [
"WatchDogInterface_PrepareForDeployment"
],
"ID": 1,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Command",
"py/state": {
"name": "WatchDogInterface_PrepareForDeployment",
"mnemonic": "Prepare_For_Deployment",
"ID": 1,
"args": [
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "confirm",
"type": {
"py/id": 296
},
"enum": [
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "CONFIRM_PREP",
"value": 96,
"comment": ""
}
}
],
"bitfields": null,
"metadata": {}
}
}
],
"metadata": {}
}
}
},
{
"names": [
"WatchDogInterface_DisengageFromLander"
],
"ID": 2,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Command",
"py/state": {
"name": "WatchDogInterface_DisengageFromLander",
"mnemonic": "Disengage_From_Lander",
"ID": 2,
"args": [
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "confirm",
"type": {
"py/id": 296
},
"enum": [
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "CONFIRM_DEPLOY",
"value": 96,
"comment": ""
}
}
],
"bitfields": null,
"metadata": {}
}
}
],
"metadata": {}
}
}
},
{
"names": [
"WatchDogInterface_EngageFromLander"
],
"ID": 3,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Command",
"py/state": {
"name": "WatchDogInterface_EngageFromLander",
"mnemonic": "Engage_From_Lander",
"ID": 3,
"args": [],
"metadata": {}
}
}
},
{
"names": [
"WatchDogInterface_SwitchConnectionMode"
],
"ID": 4,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Command",
"py/state": {
"name": "WatchDogInterface_SwitchConnectionMode",
"mnemonic": "Switch_Connection_Mode",
"ID": 4,
"args": [
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "mode",
"type": {
"py/id": 296
},
"enum": [
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "WDCMWIRED",
"value": 1,
"comment": ""
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "WDCMWIRELESS",
"value": 2,
"comment": ""
}
}
],
"bitfields": null,
"metadata": {}
}
}
],
"metadata": {}
}
}
},
{
"names": [
"WatchDogInterface_SetDebugCommsState"
],
"ID": 170,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Command",
"py/state": {
"name": "WatchDogInterface_SetDebugCommsState",
"mnemonic": "Set_Debug_Comms_State",
"ID": 170,
"args": [
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "confirm",
"type": {
"py/id": 296
},
"enum": [
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "CONFIRM_CHANGE_DEBUG",
"value": 204,
"comment": ""
}
}
],
"bitfields": null,
"metadata": {}
}
},
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "state",
"type": {
"py/id": 296
},
"enum": [
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "DEBUGON",
"value": 255,
"comment": ""
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "DEBUGOFF",
"value": 0,
"comment": ""
}
}
],
"bitfields": null,
"metadata": {}
}
}
],
"metadata": {}
}
}
},
{
"names": [
"WatchDogInterface_SetAutoHeaterOnValue"
],
"ID": 171,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Command",
"py/state": {
"name": "WatchDogInterface_SetAutoHeaterOnValue",
"mnemonic": "Set_Auto_Heater_On_Value",
"ID": 171,
"args": [
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "on",
"type": {
"py/id": 450
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
],
"metadata": {}
}
}
},
{
"names": [
"WatchDogInterface_SetAutoHeaterOffValue"
],
"ID": 172,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Command",
"py/state": {
"name": "WatchDogInterface_SetAutoHeaterOffValue",
"mnemonic": "Set_Auto_Heater_Off_Value",
"ID": 172,
"args": [
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "off",
"type": {
"py/id": 450
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
],
"metadata": {}
}
}
},
{
"names": [
"WatchDogInterface_SetHeaterDutyCycle"
],
"ID": 173,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Command",
"py/state": {
"name": "WatchDogInterface_SetHeaterDutyCycle",
"mnemonic": "Set_Heater_Duty_Cycle",
"ID": 173,
"args": [
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "duty",
"type": {
"py/id": 450
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
],
"metadata": {}
}
}
},
{
"names": [
"WatchDogInterface_SetHeaterDutyCyclePeriod"
],
"ID": 174,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Command",
"py/state": {
"name": "WatchDogInterface_SetHeaterDutyCyclePeriod",
"mnemonic": "Set_Heater_Duty_Cycle_Period",
"ID": 174,
"args": [
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "period",
"type": {
"py/id": 450
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
],
"metadata": {}
}
}
},
{
"names": [
"WatchDogInterface_SetVsaeState"
],
"ID": 218,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Command",
"py/state": {
"name": "WatchDogInterface_SetVsaeState",
"mnemonic": "Set_VSAE_State",
"ID": 218,
"args": [
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "confirm",
"type": {
"py/id": 296
},
"enum": [
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "CONFIRM_VSAE_CHANGE",
"value": 187,
"comment": ""
}
}
],
"bitfields": null,
"metadata": {}
}
},
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "state",
"type": {
"py/id": 296
},
"enum": [
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "VSAEON",
"value": 255,
"comment": ""
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "VSAEOFF",
"value": 0,
"comment": ""
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "VSAEFORCELOW",
"value": 102,
"comment": "VSAE is normally OFF by being set as an input and floated. This sets it as an OUTPUT and drives it low."
}
}
],
"bitfields": null,
"metadata": {}
}
}
],
"metadata": {}
}
}
},
{
"names": [
"WatchDogInterface_SwitchToSleepMode"
],
"ID": 234,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Command",
"py/state": {
"name": "WatchDogInterface_SwitchToSleepMode",
"mnemonic": "Switch_to_Sleep_Mode",
"ID": 234,
"args": [
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "confirm",
"type": {
"py/id": 296
},
"enum": [
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "CONFIRM_SLEEP",
"value": 119,
"comment": ""
}
}
],
"bitfields": null,
"metadata": {}
}
}
],
"metadata": {}
}
}
},
{
"names": [
"WatchDogInterface_SwitchToKeepAliveMode"
],
"ID": 235,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Command",
"py/state": {
"name": "WatchDogInterface_SwitchToKeepAliveMode",
"mnemonic": "Switch_to_Keep_Alive_Mode",
"ID": 235,
"args": [
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "confirm",
"type": {
"py/id": 296
},
"enum": [
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "CONFIRM_ALIVE",
"value": 119,
"comment": ""
}
}
],
"bitfields": null,
"metadata": {}
}
}
],
"metadata": {}
}
}
},
{
"names": [
"WatchDogInterface_SwitchToServiceMode"
],
"ID": 236,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Command",
"py/state": {
"name": "WatchDogInterface_SwitchToServiceMode",
"mnemonic": "Switch_to_Service_Mode",
"ID": 236,
"args": [
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "confirm",
"type": {
"py/id": 296
},
"enum": [
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "CONFIRM_SERVICE",
"value": 119,
"comment": ""
}
}
],
"bitfields": null,
"metadata": {}
}
}
],
"metadata": {}
}
}
},
{
"names": [
"WatchDogInterface_ClearResetMemory"
],
"ID": 244,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Command",
"py/state": {
"name": "WatchDogInterface_ClearResetMemory",
"mnemonic": "Clear_Reset_Memory",
"ID": 244,
"args": [
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "confirm_1",
"type": {
"py/id": 296
},
"enum": [
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "CONFIRM_CLEAR_RESETS_1",
"value": 199,
"comment": ""
}
}
],
"bitfields": null,
"metadata": {}
}
},
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "confirm_2",
"type": {
"py/id": 296
},
"enum": [
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "CONFIRM_CLEAR_RESETS_2",
"value": 25,
"comment": ""
}
}
],
"bitfields": null,
"metadata": {}
}
}
],
"metadata": {}
}
}
},
{
"names": [
"WatchDogInterface_DangerousForceBatteryStateDangerous"
],
"ID": 245,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Command",
"py/state": {
"name": "WatchDogInterface_DangerousForceBatteryStateDangerous",
"mnemonic": "DANGEROUS_Force_Battery_State_DANGEROUS",
"ID": 245,
"args": [
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "confirm_1",
"type": {
"py/id": 296
},
"enum": [
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "DANGEROUS_CONFIRM_FORCE_BSTAT_1",
"value": 240,
"comment": ""
}
}
],
"bitfields": null,
"metadata": {}
}
},
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "confirm_2",
"type": {
"py/id": 296
},
"enum": [
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "DANGEROUS_CONFIRM_FORCE_BSTAT_2",
"value": 1,
"comment": ""
}
}
],
"bitfields": null,
"metadata": {}
}
},
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "state",
"type": {
"py/id": 296
},
"enum": [
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "DANGEROUSBSTATLOW",
"value": 0,
"comment": "Sets BSTAT to OUTPUT and drives it LOW. This one is dangerous (can blow stuff up)."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "DANGEROUSBSTATHIGH",
"value": 255,
"comment": "Sets BSTAT to OUTPUT and drives it HIGH. This one is dangerous (can blow stuff up)."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "BSTATRESTORE",
"value": 170,
"comment": "Returns BSTAT to its nominal state as an INPUT. This one isn't explictly dangerous to run."
}
}
],
"bitfields": null,
"metadata": {}
}
}
],
"metadata": {}
}
}
},
{
"names": [
"WatchDogInterface_RequestStatusReport"
],
"ID": 246,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Command",
"py/state": {
"name": "WatchDogInterface_RequestStatusReport",
"mnemonic": "Request_Status_Report",
"ID": 246,
"args": [
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "confirm",
"type": {
"py/id": 296
},
"enum": [
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "CONFIRM_REQUEST",
"value": 87,
"comment": ""
}
}
],
"bitfields": null,
"metadata": {}
}
}
],
"metadata": {}
}
}
},
{
"names": [
"WatchDogInterface_SetChargerEnable"
],
"ID": 247,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Command",
"py/state": {
"name": "WatchDogInterface_SetChargerEnable",
"mnemonic": "Set_Charger_Enable",
"ID": 247,
"args": [
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "charge_en",
"type": {
"py/id": 296
},
"enum": [
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "CHARGE_EN_OFF",
"value": 0,
"comment": ""
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "CHARGE_EN_ON",
"value": 255,
"comment": ""
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "CHARGE_EN_FORCE_HIGH",
"value": 153,
"comment": "Forces the CE pin high. Not for normal use. Use in case of a voltage divider fault (e.g. due to part failure during launch vibe)."
}
}
],
"bitfields": null,
"metadata": {}
}
}
],
"metadata": {}
}
}
},
{
"names": [
"WatchDogInterface_SetChargerPowerConnection"
],
"ID": 248,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Command",
"py/state": {
"name": "WatchDogInterface_SetChargerPowerConnection",
"mnemonic": "Set_Charger_Power_Connection",
"ID": 248,
"args": [
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "v_lander_reg_en",
"type": {
"py/id": 429
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
],
"metadata": {}
}
}
},
{
"names": [
"WatchDogInterface_SetBatteryConnection"
],
"ID": 249,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Command",
"py/state": {
"name": "WatchDogInterface_SetBatteryConnection",
"mnemonic": "Set_Battery_Connection",
"ID": 249,
"args": [
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "batt_en",
"type": {
"py/id": 429
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
],
"metadata": {}
}
}
},
{
"names": [
"WatchDogInterface_SetBatteryControlEnable"
],
"ID": 250,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Command",
"py/state": {
"name": "WatchDogInterface_SetBatteryControlEnable",
"mnemonic": "Set_Battery_Control_Enable",
"ID": 250,
"args": [
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "batt_ctrl_en",
"type": {
"py/id": 296
},
"enum": [
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "BATT_CTRL_EN_OFF",
"value": 0,
"comment": ""
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "BATT_CTRL_EN_ON",
"value": 255,
"comment": ""
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "BATT_CTRL_EN_FORCE_HIGH",
"value": 153,
"comment": "Forces the BCTRLE pin high. Not for normal use. Use in case of a pull-up resistor fault (e.g. due to part failure during launch vibe)."
}
}
],
"bitfields": null,
"metadata": {}
}
}
],
"metadata": {}
}
}
},
{
"names": [
"WatchDogInterface_SetBatteryLatch"
],
"ID": 251,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Command",
"py/state": {
"name": "WatchDogInterface_SetBatteryLatch",
"mnemonic": "Set_Battery_Latch",
"ID": 251,
"args": [
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "latch_batt",
"type": {
"py/id": 296
},
"enum": [
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "LATCH_BATT_OFF",
"value": 0,
"comment": ""
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "LATCH_BATT_EN_ON",
"value": 255,
"comment": ""
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "LATCH_BATT_EN_PULSE_HIGH",
"value": 170,
"comment": "Pulses the LBATT pin to make the battery D-Latch take on the state of BATT_EN."
}
}
],
"bitfields": null,
"metadata": {}
}
}
],
"metadata": {}
}
}
},
{
"names": [
"WatchDogInterface_SetLatchSet"
],
"ID": 252,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Command",
"py/state": {
"name": "WatchDogInterface_SetLatchSet",
"mnemonic": "Set_Latch_Set",
"ID": 252,
"args": [
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "latch_set",
"type": {
"py/id": 296
},
"enum": [
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "LATCH_SET_OFF",
"value": 0,
"comment": "Set as input."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "PULSE_LATCH_SET_LOW",
"value": 21,
"comment": "Pulse LS HIGH-LOW-HIGH as an output."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "FORCE_LATCH_SET_LOW",
"value": 255,
"comment": "Make an output and drive LOW."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "FORCE_LATCH_SET_HIGH",
"value": 187,
"comment": "Make an output and drive HIGH."
}
}
],
"bitfields": null,
"metadata": {}
}
}
],
"metadata": {}
}
}
},
{
"names": [
"WatchDogInterface_SetLatchReset"
],
"ID": 253,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Command",
"py/state": {
"name": "WatchDogInterface_SetLatchReset",
"mnemonic": "Set_Latch_Reset",
"ID": 253,
"args": [
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "latch_reset",
"type": {
"py/id": 296
},
"enum": [
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "LATCH_RESET_OFF",
"value": 0,
"comment": "Set as input."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "PULSE_LATCH_RESET_LOW",
"value": 21,
"comment": "Pulse LR HIGH-LOW-HIGH as an output."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "FORCE_LATCH_RESET_LOW",
"value": 255,
"comment": "Make an output and drive LOW."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "FORCE_LATCH_RESET_HIGH",
"value": 187,
"comment": "Make an output and drive HIGH."
}
}
],
"bitfields": null,
"metadata": {}
}
}
],
"metadata": {}
}
}
},
{
"names": [
"WatchDogInterface_Echo"
],
"ID": 255,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Command",
"py/state": {
"name": "WatchDogInterface_Echo",
"mnemonic": "Echo",
"ID": 255,
"args": [
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "length",
"type": {
"py/id": 217
},
"enum": [],
"bitfields": null,
"metadata": {}
}
},
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "message",
"type": {
"py/reduce": [
{
"py/type": "IrisBackendv3.data_standards.fsw_data_type.FswDataType"
},
{
"py/tuple": [
"char[10]"
]
}
]
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
],
"metadata": {}
}
}
}
]
},
"telemetry": {
"py/object": "IrisBackendv3.utils.nameiddict.NameIdDict",
"py/state": [
{
"names": [
"Voltage2.5V"
],
"ID": 2,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "Voltage2.5V",
"ID": 2,
"type": {
"py/id": 1799
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"Voltage2.8V"
],
"ID": 3,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "Voltage2.8V",
"ID": 3,
"type": {
"py/id": 1799
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"Voltage24V"
],
"ID": 4,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "Voltage24V",
"ID": 4,
"type": {
"py/id": 1799
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"Voltage28V"
],
"ID": 5,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "Voltage28V",
"ID": 5,
"type": {
"py/id": 1799
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"Batterythermistor"
],
"ID": 16,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "Batterythermistor",
"ID": 16,
"type": {
"py/id": 217
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"Therm0"
],
"ID": 27,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "Therm0",
"ID": 27,
"type": {
"py/id": 450
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"Therm1"
],
"ID": 28,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "Therm1",
"ID": 28,
"type": {
"py/id": 450
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"Therm2"
],
"ID": 29,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "Therm2",
"ID": 29,
"type": {
"py/id": 450
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"Therm3"
],
"ID": 30,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "Therm3",
"ID": 30,
"type": {
"py/id": 450
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"Therm4"
],
"ID": 31,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "Therm4",
"ID": 31,
"type": {
"py/id": 450
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"Therm5"
],
"ID": 32,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "Therm5",
"ID": 32,
"type": {
"py/id": 450
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
}
]
},
"events": {
"py/object": "IrisBackendv3.utils.nameiddict.NameIdDict",
"py/state": [
{
"names": [
"WatchDogIncorrectResp"
],
"ID": 0,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Event",
"py/state": {
"name": "WatchDogIncorrectResp",
"ID": 0,
"severity": "WARNING_HI",
"format_string": "Watchdog Sent back Incorrect Response: %u",
"args": [
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "error",
"type": {
"py/id": 217
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
],
"metadata": {}
}
}
},
{
"names": [
"WatchDogTimedOut"
],
"ID": 1,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Event",
"py/state": {
"name": "WatchDogTimedOut",
"ID": 1,
"severity": "WARNING_HI",
"format_string": "WatchDog Timer has timed out",
"args": [],
"metadata": {}
}
}
},
{
"names": [
"WatchDogCmdReceived"
],
"ID": 2,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Event",
"py/state": {
"name": "WatchDogCmdReceived",
"ID": 2,
"severity": "ACTIVITY_HI",
"format_string": "WatchDog Cmd to reset %s received",
"args": [
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "cmd",
"type": {
"py/id": 663
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
],
"metadata": {}
}
}
},
{
"names": [
"WatchDogCommError"
],
"ID": 3,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Event",
"py/state": {
"name": "WatchDogCommError",
"ID": 3,
"severity": "WARNING_HI",
"format_string": "WatchDog Interface has error: %u",
"args": [
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "error",
"type": {
"py/id": 15
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
],
"metadata": {}
}
}
},
{
"names": [
"AdcThermistorError"
],
"ID": 4,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Event",
"py/state": {
"name": "AdcThermistorError",
"ID": 4,
"severity": "WARNING_HI",
"format_string": "ADC Thermistor Conversion Error",
"args": [],
"metadata": {}
}
}
},
{
"names": [
"WatchDogIncorrectResetValue"
],
"ID": 5,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Event",
"py/state": {
"name": "WatchDogIncorrectResetValue",
"ID": 5,
"severity": "WARNING_LO",
"format_string": "Incorrect Reset Value Sent",
"args": [],
"metadata": {}
}
}
}
]
}
}
}
},
{
"names": [
"Camera"
],
"ID": 4352,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Module",
"py/state": {
"name": "Camera",
"ID": 4352,
"commands": {
"py/object": "IrisBackendv3.utils.nameiddict.NameIdDict",
"py/state": [
{
"names": [
"Camera_GetStatus"
],
"ID": 0,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Command",
"py/state": {
"name": "Camera_GetStatus",
"mnemonic": "Get_Status",
"ID": 0,
"args": [],
"metadata": {}
}
}
},
{
"names": [
"Camera_TakeImage"
],
"ID": 1,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Command",
"py/state": {
"name": "Camera_TakeImage",
"mnemonic": "Take_Image",
"ID": 1,
"args": [
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "camera_num",
"type": {
"py/id": 217
},
"enum": [],
"bitfields": null,
"metadata": {}
}
},
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "callback_id",
"type": {
"py/id": 450
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
],
"metadata": {}
}
}
},
{
"names": [
"Camera_Error"
],
"ID": 2,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Command",
"py/state": {
"name": "Camera_Error",
"mnemonic": "Error",
"ID": 2,
"args": [
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "action",
"type": {
"py/id": 217
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
],
"metadata": {}
}
}
},
{
"names": [
"Camera_ConfigureCamera0"
],
"ID": 3,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Command",
"py/state": {
"name": "Camera_ConfigureCamera0",
"mnemonic": "Configure_Camera0",
"ID": 3,
"args": [
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "config",
"type": {
"py/reduce": [
{
"py/type": "IrisBackendv3.data_standards.fsw_data_type.FswDataType"
},
{
"py/tuple": [
"uint64"
]
}
]
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
],
"metadata": {}
}
}
},
{
"names": [
"Camera_Camera0Crop"
],
"ID": 4,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Command",
"py/state": {
"name": "Camera_Camera0Crop",
"mnemonic": "Camera0_Crop",
"ID": 4,
"args": [
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "config",
"type": {
"py/id": 2506
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
],
"metadata": {}
}
}
},
{
"names": [
"Camera_ConfigureCamera1"
],
"ID": 5,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Command",
"py/state": {
"name": "Camera_ConfigureCamera1",
"mnemonic": "Configure_Camera1",
"ID": 5,
"args": [
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "config",
"type": {
"py/id": 2506
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
],
"metadata": {}
}
}
},
{
"names": [
"Camera_Camera1Crop"
],
"ID": 6,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Command",
"py/state": {
"name": "Camera_Camera1Crop",
"mnemonic": "Camera1_Crop",
"ID": 6,
"args": [
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "config",
"type": {
"py/id": 2506
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
],
"metadata": {}
}
}
},
{
"names": [
"Camera_ReadImage"
],
"ID": 7,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Command",
"py/state": {
"name": "Camera_ReadImage",
"mnemonic": "Read_Image",
"ID": 7,
"args": [
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "callback_id",
"type": {
"py/id": 450
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
],
"metadata": {}
}
}
},
{
"names": [
"Camera_EraseImage"
],
"ID": 8,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Command",
"py/state": {
"name": "Camera_EraseImage",
"mnemonic": "Erase_Image",
"ID": 8,
"args": [],
"metadata": {}
}
}
},
{
"names": [
"Camera_SoftCameraReset"
],
"ID": 9,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Command",
"py/state": {
"name": "Camera_SoftCameraReset",
"mnemonic": "Soft_Camera_Reset",
"ID": 9,
"args": [
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "camera_num",
"type": {
"py/id": 217
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
],
"metadata": {}
}
}
},
{
"names": [
"Camera_ImageDump"
],
"ID": 10,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Command",
"py/state": {
"name": "Camera_ImageDump",
"mnemonic": "Image_Dump",
"ID": 10,
"args": [],
"metadata": {}
}
}
},
{
"names": [
"Camera_DownlinkGrid"
],
"ID": 11,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Command",
"py/state": {
"name": "Camera_DownlinkGrid",
"mnemonic": "Downlink_Grid",
"ID": 11,
"args": [
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "via_flash",
"type": {
"py/id": 429
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
],
"metadata": {}
}
}
},
{
"names": [
"Camera_DownlinkTestSequence"
],
"ID": 12,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Command",
"py/state": {
"name": "Camera_DownlinkTestSequence",
"mnemonic": "Downlink_Test_Sequence",
"ID": 12,
"args": [
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "via_flash",
"type": {
"py/id": 429
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
],
"metadata": {}
}
}
},
{
"names": [
"Camera_TakeImageSection"
],
"ID": 161,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Command",
"py/state": {
"name": "Camera_TakeImageSection",
"mnemonic": "Take_Image_Section",
"ID": 161,
"args": [
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "camera_num",
"type": {
"py/id": 217
},
"enum": [],
"bitfields": null,
"metadata": {}
}
},
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "start_line",
"type": {
"py/id": 450
},
"enum": [],
"bitfields": null,
"metadata": {}
}
},
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "end_line",
"type": {
"py/id": 450
},
"enum": [],
"bitfields": null,
"metadata": {}
}
},
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "callback_id",
"type": {
"py/id": 450
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
],
"metadata": {}
}
}
}
]
},
"telemetry": {
"py/object": "IrisBackendv3.utils.nameiddict.NameIdDict",
"py/state": [
{
"names": [
"CamComponentImagesRequested"
],
"ID": 0,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "CamComponentImagesRequested",
"ID": 0,
"type": {
"py/id": 15
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"CamCommandImagesRequested"
],
"ID": 1,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "CamCommandImagesRequested",
"ID": 1,
"type": {
"py/id": 15
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"CamImagesSent"
],
"ID": 2,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "CamImagesSent",
"ID": 2,
"type": {
"py/id": 15
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"CamBytesSent"
],
"ID": 3,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "CamBytesSent",
"ID": 3,
"type": {
"py/id": 15
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"CamLatestCallbackId"
],
"ID": 4,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "CamLatestCallbackId",
"ID": 4,
"type": {
"py/id": 450
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
}
]
},
"events": {
"py/object": "IrisBackendv3.utils.nameiddict.NameIdDict",
"py/state": [
{
"names": [
"CameraFailedToTakeImage"
],
"ID": 0,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Event",
"py/state": {
"name": "CameraFailedToTakeImage",
"ID": 0,
"severity": "WARNING_LO",
"format_string": "Failed to capture image. Try again.",
"args": [],
"metadata": {}
}
}
}
]
}
}
}
},
{
"names": [
"GdsPackets"
],
"ID": 52992,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Module",
"py/state": {
"name": "GdsPackets",
"ID": 52992,
"commands": {
"py/object": "IrisBackendv3.utils.nameiddict.NameIdDict",
"id_dict": {},
"name_id_mapping": {}
},
"telemetry": {
"py/object": "IrisBackendv3.utils.nameiddict.NameIdDict",
"id_dict": {},
"name_id_mapping": {}
},
"events": {
"py/object": "IrisBackendv3.utils.nameiddict.NameIdDict",
"py/state": [
{
"names": [
"RadioBgApiPacket"
],
"ID": 0,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Event",
"py/state": {
"name": "RadioBgApiPacket",
"ID": 0,
"severity": "ACTIVITY_LO",
"format_string": "%s",
"args": [
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "msg",
"type": {
"py/reduce": [
{
"py/type": "IrisBackendv3.data_standards.fsw_data_type.FswDataType"
},
{
"py/tuple": [
"char[/*up to*/10000]"
]
}
]
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
],
"metadata": {}
}
}
},
{
"names": [
"RadioDirectMessagePacket"
],
"ID": 1,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Event",
"py/state": {
"name": "RadioDirectMessagePacket",
"ID": 1,
"severity": "ACTIVITY_HI",
"format_string": "%s",
"args": [
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "msg",
"type": {
"py/id": 2673
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
],
"metadata": {}
}
}
},
{
"names": [
"RadioUartBytePacket"
],
"ID": 2,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Event",
"py/state": {
"name": "RadioUartBytePacket",
"ID": 2,
"severity": "DIAGNOSTIC",
"format_string": "%s",
"args": [
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "msg",
"type": {
"py/id": 2673
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
],
"metadata": {}
}
}
},
{
"names": [
"UnsupportedPacket"
],
"ID": 3,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Event",
"py/state": {
"name": "UnsupportedPacket",
"ID": 3,
"severity": "DIAGNOSTIC",
"format_string": "%s",
"args": [
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "msg",
"type": {
"py/id": 2673
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
],
"metadata": {}
}
}
},
{
"names": [
"WatchdogCommandResponsePacket"
],
"ID": 4,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Event",
"py/state": {
"name": "WatchdogCommandResponsePacket",
"ID": 4,
"severity": "COMMAND",
"format_string": "%s",
"args": [
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "msg",
"type": {
"py/id": 2673
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
],
"metadata": {}
}
}
},
{
"names": [
"WatchdogDebugPacket"
],
"ID": 5,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Event",
"py/state": {
"name": "WatchdogDebugPacket",
"ID": 5,
"severity": "ACTIVITY_LO",
"format_string": "%s",
"args": [
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "msg",
"type": {
"py/id": 2673
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
],
"metadata": {}
}
}
},
{
"names": [
"WatchdogDebugImportantPacket"
],
"ID": 6,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Event",
"py/state": {
"name": "WatchdogDebugImportantPacket",
"ID": 6,
"severity": "ACTIVITY_HI",
"format_string": "%s",
"args": [
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "msg",
"type": {
"py/id": 2673
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
],
"metadata": {}
}
}
},
{
"names": [
"WatchdogHelloPacket"
],
"ID": 7,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Event",
"py/state": {
"name": "WatchdogHelloPacket",
"ID": 7,
"severity": "COMMAND",
"format_string": "%s",
"args": [
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "msg",
"type": {
"py/id": 2673
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
],
"metadata": {}
}
}
},
{
"names": [
"WatchdogRadioDebugPacket"
],
"ID": 8,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Event",
"py/state": {
"name": "WatchdogRadioDebugPacket",
"ID": 8,
"severity": "ACTIVITY_HI",
"format_string": "%s",
"args": [
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "msg",
"type": {
"py/id": 2673
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
],
"metadata": {}
}
}
},
{
"names": [
"WatchdogResetSpecificAckPacket"
],
"ID": 9,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Event",
"py/state": {
"name": "WatchdogResetSpecificAckPacket",
"ID": 9,
"severity": "COMMAND",
"format_string": "%s",
"args": [
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "msg",
"type": {
"py/id": 2673
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
],
"metadata": {}
}
}
},
{
"names": [
"RadioDownlinkFlushPacket"
],
"ID": 10,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Event",
"py/state": {
"name": "RadioDownlinkFlushPacket",
"ID": 10,
"severity": "DIAGNOSTIC",
"format_string": "%s",
"args": [
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "msg",
"type": {
"py/id": 2673
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
],
"metadata": {}
}
}
},
{
"names": [
"HerculesRadioUplinkAckPacket"
],
"ID": 11,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Event",
"py/state": {
"name": "HerculesRadioUplinkAckPacket",
"ID": 11,
"severity": "ACTIVITY_LO",
"format_string": "%s",
"args": [
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "msg",
"type": {
"py/id": 2673
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
],
"metadata": {}
}
}
}
]
}
}
}
},
{
"names": [
"Peregrine"
],
"ID": 57088,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Module",
"py/state": {
"name": "Peregrine",
"ID": 57088,
"commands": {
"py/object": "IrisBackendv3.utils.nameiddict.NameIdDict",
"id_dict": {},
"name_id_mapping": {}
},
"telemetry": {
"py/object": "IrisBackendv3.utils.nameiddict.NameIdDict",
"py/state": [
{
"names": [
"IrisOperationalEnabledFet"
],
"ID": 0,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "IrisOperationalEnabledFet",
"ID": 0,
"type": {
"py/id": 296
},
"enum": [
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "DISABLED",
"value": 0,
"comment": "Iris Operational Power Circuit Disabled."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "ENABLED",
"value": 1,
"comment": "Iris Operational Power Circuit Enabled."
}
}
],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"IrisReleaseEnabledFet"
],
"ID": 1,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "IrisReleaseEnabledFet",
"ID": 1,
"type": {
"py/id": 296
},
"enum": [
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "DISABLED",
"value": 0,
"comment": "Iris Release Power Circuit Disabled."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "ENABLED",
"value": 1,
"comment": "Iris Release Power Circuit Enabled."
}
}
],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"IrisOperationalAvgCurrent"
],
"ID": 2,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "IrisOperationalAvgCurrent",
"ID": 2,
"type": {
"py/reduce": [
{
"py/type": "IrisBackendv3.data_standards.fsw_data_type.FswDataType"
},
{
"py/tuple": [
"double"
]
}
]
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"IrisReleaseAvgCurrent"
],
"ID": 3,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "IrisReleaseAvgCurrent",
"ID": 3,
"type": {
"py/id": 2820
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"IrisOperationalMaxCurrent"
],
"ID": 4,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "IrisOperationalMaxCurrent",
"ID": 4,
"type": {
"py/id": 2820
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"IrisReleaseMaxCurrent"
],
"ID": 5,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "IrisReleaseMaxCurrent",
"ID": 5,
"type": {
"py/id": 2820
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"SlipServiceStatus"
],
"ID": 6,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "SlipServiceStatus",
"ID": 6,
"type": {
"py/id": 296
},
"enum": [
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "ACTIVE",
"value": 0,
"comment": "Service is currently active."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "RELOADING",
"value": 1,
"comment": "Service is currently reloading."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "INACTIVE",
"value": 2,
"comment": "Service is currently inactive."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "FAILED",
"value": 3,
"comment": "Service is currently failed."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "ACTIVATING",
"value": 4,
"comment": "Service is currently activating."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "DEACTIVATING",
"value": 5,
"comment": "Service is currently deactivating."
}
}
],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"WifiServiceStatus"
],
"ID": 7,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "WifiServiceStatus",
"ID": 7,
"type": {
"py/id": 296
},
"enum": [
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "ACTIVE",
"value": 0,
"comment": "Service is currently active."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "RELOADING",
"value": 1,
"comment": "Service is currently reloading."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "INACTIVE",
"value": 2,
"comment": "Service is currently inactive."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "FAILED",
"value": 3,
"comment": "Service is currently failed."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "ACTIVATING",
"value": 4,
"comment": "Service is currently activating."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "DEACTIVATING",
"value": 5,
"comment": "Service is currently deactivating."
}
}
],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"DeckD1TempKelvin"
],
"ID": 8,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "DeckD1TempKelvin",
"ID": 8,
"type": {
"py/id": 2820
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"DeckD2TempKelvin"
],
"ID": 9,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "DeckD2TempKelvin",
"ID": 9,
"type": {
"py/id": 2820
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"DeckD3TempKelvin"
],
"ID": 10,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "DeckD3TempKelvin",
"ID": 10,
"type": {
"py/id": 2820
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"IrisOperationalAvgPower"
],
"ID": 242,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "IrisOperationalAvgPower",
"ID": 242,
"type": {
"py/id": 2820
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
}
]
},
"events": {
"py/object": "IrisBackendv3.utils.nameiddict.NameIdDict",
"id_dict": {},
"name_id_mapping": {}
}
}
}
},
{
"names": [
"RadioGround"
],
"ID": 61184,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Module",
"py/state": {
"name": "RadioGround",
"ID": 61184,
"commands": {
"py/object": "IrisBackendv3.utils.nameiddict.NameIdDict",
"py/state": [
{
"names": [
"RadioGround_Echo"
],
"ID": 0,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Command",
"py/state": {
"name": "RadioGround_Echo",
"mnemonic": "Echo",
"ID": 0,
"args": [
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "text_to_echo",
"type": {
"py/reduce": [
{
"py/type": "IrisBackendv3.data_standards.fsw_data_type.FswDataType"
},
{
"py/tuple": [
"char[/*up to*/255]"
]
}
]
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
],
"metadata": {}
}
}
},
{
"names": [
"RadioGround_ExitStasis"
],
"ID": 94,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Command",
"py/state": {
"name": "RadioGround_ExitStasis",
"mnemonic": "ExitStasis",
"ID": 94,
"args": [
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "confirm_by_typing_STASIS",
"type": {
"py/reduce": [
{
"py/type": "IrisBackendv3.data_standards.fsw_data_type.FswDataType"
},
{
"py/tuple": [
"char[6]"
]
}
]
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
],
"metadata": {}
}
}
},
{
"names": [
"RadioGround_ResetRadio"
],
"ID": 187,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Command",
"py/state": {
"name": "RadioGround_ResetRadio",
"mnemonic": "ResetRadio",
"ID": 187,
"args": [
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "confirm_by_typing_RESET",
"type": {
"py/reduce": [
{
"py/type": "IrisBackendv3.data_standards.fsw_data_type.FswDataType"
},
{
"py/tuple": [
"char[5]"
]
}
]
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
],
"metadata": {}
}
}
},
{
"names": [
"RadioGround_WriteUart"
],
"ID": 219,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Command",
"py/state": {
"name": "RadioGround_WriteUart",
"mnemonic": "WriteUart",
"ID": 219,
"args": [
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "text_to_echo",
"type": {
"py/id": 2917
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
],
"metadata": {}
}
}
},
{
"names": [
"RadioGround_HerculesDm"
],
"ID": 221,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Command",
"py/state": {
"name": "RadioGround_HerculesDm",
"mnemonic": "HerculesDm",
"ID": 221,
"args": [
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "text_to_echo",
"type": {
"py/id": 2917
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
],
"metadata": {}
}
}
},
{
"names": [
"RadioGround_EnterStasis"
],
"ID": 229,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Command",
"py/state": {
"name": "RadioGround_EnterStasis",
"mnemonic": "EnterStasis",
"ID": 229,
"args": [
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "confirm_by_typing_STASIS",
"type": {
"py/id": 2928
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
],
"metadata": {}
}
}
}
]
},
"telemetry": {
"py/object": "IrisBackendv3.utils.nameiddict.NameIdDict",
"py/state": [
{
"names": [
"Rssi"
],
"ID": 0,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "Rssi",
"ID": 0,
"type": {
"py/id": 1799
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"StateAbbr"
],
"ID": 1,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "StateAbbr",
"ID": 1,
"type": {
"py/id": 296
},
"enum": [
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "BOOT",
"value": 0,
"comment": "BOOT. Radio just booted."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "INIT",
"value": 1,
"comment": "INITIALIZED. Trying to turn radio antenna on."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "WFON",
"value": 2,
"comment": "WIFI_ON. Radio antenna is on. Trying to connect to the network."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "CONN",
"value": 3,
"comment": "CONNECTED. Connected to WiFi network. Attempting to establish UDP connection."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "UDPC",
"value": 4,
"comment": "UDP_CONNECTED. UDP connection established over WiFi. Everything is good to go."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "CRPT",
"value": 255,
"comment": "The abbreviation received didn't match any known Radio State Abbreviation. See raw data for actual bytes received."
}
}
],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"UptimeTicks"
],
"ID": 2,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "UptimeTicks",
"ID": 2,
"type": {
"py/id": 15
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"UptimeMs"
],
"ID": 3,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "UptimeMs",
"ID": 3,
"type": {
"py/id": 15
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"UdpRxPacketCount"
],
"ID": 4,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "UdpRxPacketCount",
"ID": 4,
"type": {
"py/id": 15
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"BadHercPacketCount"
],
"ID": 5,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "BadHercPacketCount",
"ID": 5,
"type": {
"py/id": 15
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"HerculesUdpInterlockCountingSemaphore"
],
"ID": 6,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "HerculesUdpInterlockCountingSemaphore",
"ID": 6,
"type": {
"py/id": 217
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
}
]
},
"events": {
"py/object": "IrisBackendv3.utils.nameiddict.NameIdDict",
"py/state": [
{
"names": [
"Heartbeat"
],
"ID": 0,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Event",
"py/state": {
"name": "Heartbeat",
"ID": 0,
"severity": "ACTIVITY_LO",
"format_string": "Radio Heartbeat: %s with %s RSSI at %s.",
"args": [
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "state_abbr",
"type": {
"py/id": 296
},
"enum": [
{
"py/id": 2986
},
{
"py/id": 2988
},
{
"py/id": 2990
},
{
"py/id": 2992
},
{
"py/id": 2994
},
{
"py/id": 2996
}
],
"bitfields": null,
"metadata": {}
}
},
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "rssi",
"type": {
"py/id": 1799
},
"enum": [],
"bitfields": null,
"metadata": {}
}
},
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "uptime_str",
"type": {
"py/id": 2917
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
],
"metadata": {}
}
}
},
{
"names": [
"Connected"
],
"ID": 1,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Event",
"py/state": {
"name": "Connected",
"ID": 1,
"severity": "ACTIVITY_HI",
"format_string": "Radio (Re)Connected: %s",
"args": [
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "message",
"type": {
"py/reduce": [
{
"py/type": "IrisBackendv3.data_standards.fsw_data_type.FswDataType"
},
{
"py/tuple": [
"char[39]"
]
}
]
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
],
"metadata": {}
}
}
},
{
"names": [
"Echo"
],
"ID": 2,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Event",
"py/state": {
"name": "Echo",
"ID": 2,
"severity": "COMMAND",
"format_string": "Radio Echoed: '%s'.",
"args": [
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "echoed_text",
"type": {
"py/id": 2917
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
],
"metadata": {}
}
}
},
{
"names": [
"GotCommand"
],
"ID": 16,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Event",
"py/state": {
"name": "GotCommand",
"ID": 16,
"severity": "COMMAND",
"format_string": "Radio got command '%s' with data: `%s`.",
"args": [
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "command",
"type": {
"py/id": 296
},
"enum": [
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "ECHO",
"value": 0,
"comment": "Echo back the given bytes."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "RESET_RADIO",
"value": 187,
"comment": "Reset the Radio."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "ENTER_STASIS",
"value": 229,
"comment": "Enter stasis mode."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "EXIT_STASIS",
"value": 94,
"comment": "Exit stasis mode."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "WRITE_UART",
"value": 219,
"comment": "Write the given message to the Hercules UART."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "HERCULES_DM",
"value": 221,
"comment": "Send the given data to Hercules as a Radio Direct Message."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "CORRUPTED",
"value": 255,
"comment": "Given ID doesn't match any known IDs. Likely corrupted."
}
}
],
"bitfields": null,
"metadata": {}
}
},
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "string_of_command_data",
"type": {
"py/id": 2917
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
],
"metadata": {}
}
}
},
{
"names": [
"DidCommand"
],
"ID": 17,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Event",
"py/state": {
"name": "DidCommand",
"ID": 17,
"severity": "COMMAND",
"format_string": "Radio successfully executed command '%s' with data: `%s`.",
"args": [
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "command",
"type": {
"py/id": 296
},
"enum": [
{
"py/id": 3078
},
{
"py/id": 3080
},
{
"py/id": 3082
},
{
"py/id": 3084
},
{
"py/id": 3086
},
{
"py/id": 3088
},
{
"py/id": 3090
}
],
"bitfields": null,
"metadata": {}
}
},
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "string_of_command_data",
"type": {
"py/id": 2917
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
],
"metadata": {}
}
}
},
{
"names": [
"BadCommand"
],
"ID": 31,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Event",
"py/state": {
"name": "BadCommand",
"ID": 31,
"severity": "COMMAND",
"format_string": "Radio failed to execute a command '%s' with data: `%s`.",
"args": [
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "command",
"type": {
"py/id": 296
},
"enum": [
{
"py/id": 3078
},
{
"py/id": 3080
},
{
"py/id": 3082
},
{
"py/id": 3084
},
{
"py/id": 3086
},
{
"py/id": 3088
},
{
"py/id": 3090
}
],
"bitfields": null,
"metadata": {}
}
},
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "string_of_command_data",
"type": {
"py/id": 2917
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
],
"metadata": {}
}
}
},
{
"names": [
"BadHerculesPacket"
],
"ID": 240,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Event",
"py/state": {
"name": "BadHerculesPacket",
"ID": 240,
"severity": "WARNING_LO",
"format_string": "Radio received a bad packet from Hercules. Issue: %s.",
"args": [
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "issue",
"type": {
"py/id": 296
},
"enum": [
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "NO_ERROR",
"value": 0,
"comment": "No error exists."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "INVALID_PARAMETER",
"value": 384,
"comment": "This error code indicates that a command contained an invalid parameter"
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "DEVICE_WRONG_STATE",
"value": 385,
"comment": "This error code indicates that the device is in wrong state to accept commands."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "OUT_OF_MEMORY",
"value": 386,
"comment": "This error indicates that the device has run out of memory."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "FEATURE_NOT_IMPLEMENTED",
"value": 387,
"comment": "This error indicates that the feature in question has not been implemented."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "COMMAND_NOT_RECOGNIZED",
"value": 388,
"comment": "This error indicates that the issued command was not recognized."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "TIMEOUT",
"value": 389,
"comment": "This error indicates that a command or procedure failed due to timeout. This error code is generated e.g. if you send an incomplete command to the Wi-Fi module - after the timeout of 1 second this error code is sent to the host transported by the event .endpoint_syntax_errorThis error code is generated also e.g. when the maximum number of retry attempts (10) to try to connect to a wireless network have been executed. A typical example of such a case might be when the issued password is invalid, in which case the error code is transported by the event .wifi_evt_sme_connect_failed"
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "UNSPECIFIED_ERROR",
"value": 390,
"comment": "This error code is generated when an unspecified error is detected."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "HARDWARE_FAILURE",
"value": 391,
"comment": "This error code is generated when a hardware failure is detected."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "INTERNAL_BUFFER_FULL",
"value": 392,
"comment": "This error code is generated the a command was not accepted due to full internal buffers."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "DISCONNECTED",
"value": 393,
"comment": "This error code is generated when a command or procedure has failed due to disconnection."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "TOO_MANY_REQUEST",
"value": 394,
"comment": "This error code is generated when there are too many simultaneous requests."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "ACCESS_POINT_NOT_IN_SCANLIST",
"value": 395,
"comment": "This error code is generated when the defined Access Point is not found from the scanlist."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "INVALID_PASSWORD",
"value": 396,
"comment": "This error code is generated in the following cases:1) you try to connect to a secured network without setting the password or the password is too short2) you try to start a secured AP without setting the password or the password is too short3) you try to set a 64-character PSK with non-hex characters4) you try to set an invalid WEP key (invalid characters or invalid length)Notice that WPA does not contain any standard way for the Access Point to communicate to the station that the password is invalid. The Access Point just disconnects the client during authentication if the password is found invalid. Some stations take an educated guess that this probably means the password is incorrect. WF121 simply retries the authentication until it exceeds the maximum amount of retries (10) which then causes the 0x0185 Timeout Error."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "AUTHENTICATION_FAILURE",
"value": 397,
"comment": "This error code is generated when the WPA/WPA2 authentication has failed."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "OVERFLOW",
"value": 398,
"comment": "This error code is generated when an overflow has been detected."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "MULTIPLE_PBC_SESSIONS",
"value": 399,
"comment": "This error code is generated when multiple PBC (Push Button Configuration) sessions have been detected."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "ETHERNET_NOT_CONNECTED",
"value": 400,
"comment": "This error code is generated when the Ethernet cable is not connected"
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "ETHERNET_ROUTE_NOT_SET",
"value": 401,
"comment": "This error code is generated if the Ethernet route is not set"
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "WRONG_OPERATING_MODE",
"value": 402,
"comment": "This error code is generated if the operating mode is wrong for the issued command"
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "WIFI_NOT_FOUND",
"value": 403,
"comment": "This error code is generated if the requested resource was not found."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "WIFI_NOT_ALREADY_EXIST",
"value": 404,
"comment": "This error is generated if the requested resource already exists."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "WIFI_INVALID_CONFIGURATION",
"value": 405,
"comment": "This error code is generated if the current configuration is invalid."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "WIFI_ACCESS_POINT_LOST",
"value": 406,
"comment": "This error code is generated if the connection to an Access Point is lost."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "PS_STORE_FULL",
"value": 769,
"comment": "This error is generated if the Flash which is reserved for the PS (Persistent Store) is full."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "PS_KEY_NOT_FOUND",
"value": 770,
"comment": "This error is generated if the PS key (Persistent Store key) was not found."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "I2C_WRITE_ALREADY_IN_PROGRESS",
"value": 771,
"comment": "This error is generated if I2C transmission was initiated while a transmission was already in progress."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "I2C_ACK_MISSING",
"value": 772,
"comment": "This error is generated if an acknowledgement for I2C was not received."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "FLASH_WRITE_FAILED",
"value": 776,
"comment": "This error code is generated if writing to Flash failed."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "FILE_NOT_OPENED",
"value": 773,
"comment": "This error code is generated if an access was attempted to an unopened file."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "FILE_NOT_FOUND",
"value": 774,
"comment": "This error code is generated if the requested file was not found in the SD card."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "DISK_ERROR",
"value": 775,
"comment": "This error code is generated if an SD card error was detected of if the SD card is full"
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "TCP_IP_SUCCESS",
"value": 512,
"comment": "This code indicates that no error was detected."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "TCP_IP_OUT_OF_MEMORY",
"value": 513,
"comment": "This error code is generated when the system has run out of memory."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "TCP_IP_BUFFER_ERROR",
"value": 514,
"comment": "This error code is generated when handling of buffers has failed"
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "TCP_IP_TIMEOUT",
"value": 515,
"comment": "This error code is generated when a timeout has been detected."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "TCP_IP_ROUTING",
"value": 516,
"comment": "This error code is generated when a route could not be found."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "TCP_IP_IN_PROGRESS",
"value": 517,
"comment": "This error code is generated when an operation is in progress"
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "TCP_IP_ILLEGAL_VALUE",
"value": 518,
"comment": "This error code is generated when the issued value is deemed illegal."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "TCP_IP_WOULD_BLOCK",
"value": 519,
"comment": "This error code is generated when an operation blocks."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "TCP_IP_ADDRESS_IN_USE",
"value": 520,
"comment": "This error code is generated when the issued address is already in use"
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "TCP_IP_ALREADY_CONNECTED",
"value": 521,
"comment": "This error code is generated when the Wi-Fi module is already connected"
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "TCP_IP_CONNECTION_ABORTED",
"value": 522,
"comment": "This error code is generated when a connection is aborted."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "TCP_IP_CONNECTION_RESET",
"value": 523,
"comment": "This error code is generated when a connection has been reset"
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "TCP_IP_CONNECTION_CLOSED",
"value": 524,
"comment": "This error code is generated when a connection has been closed."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "TCP_IP_NOT_CONNECTED",
"value": 525,
"comment": "This error code is generated when the Wi-Fi module is not connected."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "TCP_IP_ILLEGAL_ARGUMENT",
"value": 526,
"comment": "This error code is generated if an illegal argument was issued."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "TCP_IP_INTERFACE_LEVEL_ERROR",
"value": 527,
"comment": "This error code is generated if an interface error was detected"
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "TCP_IP_UNKNOWN_HOST",
"value": 640,
"comment": "This error is generated if an unknown host is detected."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "SERVICE_NOT_RUNNING",
"value": 528,
"comment": "This error code is generated if the specified service is not running."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "SERVICE_RUNNING",
"value": 529,
"comment": "This error code is generated if the specified service is already running"
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "HOSTNAME_NOT_SET",
"value": 530,
"comment": "This error code is generated if the hostname has not been set"
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "HOSTNAME_CONFLICT",
"value": 531,
"comment": "This error code is generated if a hostname conflict was detected."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "OTHER__UNEXPECTED",
"value": 65535,
"comment": "An unknown/unexpected BGAPI reason was given for this syntax_error. Perhaps the error code bytes were corrupted during downlink. See the raw bytes of the packet for the actual error code bytes."
}
}
],
"bitfields": null,
"metadata": {}
}
}
],
"metadata": {}
}
}
},
{
"names": [
"BadEndpointData"
],
"ID": 241,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Event",
"py/state": {
"name": "BadEndpointData",
"ID": 241,
"severity": "WARNING_LO",
"format_string": "One of the Radio's routing endpoints experienced an issue. Endpoint: %s, Issue: %s.",
"args": [
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "endpoint",
"type": {
"py/id": 217
},
"enum": [],
"bitfields": null,
"metadata": {}
}
},
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "issue",
"type": {
"py/id": 296
},
"enum": [
{
"py/id": 3134
},
{
"py/id": 3136
},
{
"py/id": 3138
},
{
"py/id": 3140
},
{
"py/id": 3142
},
{
"py/id": 3144
},
{
"py/id": 3146
},
{
"py/id": 3148
},
{
"py/id": 3150
},
{
"py/id": 3152
},
{
"py/id": 3154
},
{
"py/id": 3156
},
{
"py/id": 3158
},
{
"py/id": 3160
},
{
"py/id": 3162
},
{
"py/id": 3164
},
{
"py/id": 3166
},
{
"py/id": 3168
},
{
"py/id": 3170
},
{
"py/id": 3172
},
{
"py/id": 3174
},
{
"py/id": 3176
},
{
"py/id": 3178
},
{
"py/id": 3180
},
{
"py/id": 3182
},
{
"py/id": 3184
},
{
"py/id": 3186
},
{
"py/id": 3188
},
{
"py/id": 3190
},
{
"py/id": 3192
},
{
"py/id": 3194
},
{
"py/id": 3196
},
{
"py/id": 3198
},
{
"py/id": 3200
},
{
"py/id": 3202
},
{
"py/id": 3204
},
{
"py/id": 3206
},
{
"py/id": 3208
},
{
"py/id": 3210
},
{
"py/id": 3212
},
{
"py/id": 3214
},
{
"py/id": 3216
},
{
"py/id": 3218
},
{
"py/id": 3220
},
{
"py/id": 3222
},
{
"py/id": 3224
},
{
"py/id": 3226
},
{
"py/id": 3228
},
{
"py/id": 3230
},
{
"py/id": 3232
},
{
"py/id": 3234
},
{
"py/id": 3236
},
{
"py/id": 3238
},
{
"py/id": 3240
}
],
"bitfields": null,
"metadata": {}
}
}
],
"metadata": {}
}
}
},
{
"names": [
"BadUdp"
],
"ID": 242,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Event",
"py/state": {
"name": "BadUdp",
"ID": 242,
"severity": "WARNING_HI",
"format_string": "Radio received %s in UDP packet with IP=%s, Port=%s.",
"args": [
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "issue",
"type": {
"py/id": 296
},
"enum": [
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "BAD_IP",
"value": 1,
"comment": ""
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "BAD_PORT",
"value": 2,
"comment": ""
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "BAD_IP_AND_PORT",
"value": 3,
"comment": ""
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "NOTHING__THIS_SHOULD_NOT_HAPPEN",
"value": 0,
"comment": "We should never get a BAD UDP message with 'NOTHING' as the issue. If we get this, there must have been a bitflip in the message."
}
}
],
"bitfields": null,
"metadata": {}
}
},
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "ip",
"type": {
"py/reduce": [
{
"py/type": "IrisBackendv3.data_standards.fsw_data_type.FswDataType"
},
{
"py/tuple": [
"char[15]"
]
}
]
},
"enum": [],
"bitfields": null,
"metadata": {}
}
},
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "port",
"type": {
"py/id": 450
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
],
"metadata": {}
}
}
},
{
"names": [
"CriticalReset"
],
"ID": 243,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Event",
"py/state": {
"name": "CriticalReset",
"ID": 243,
"severity": "FATAL",
"format_string": "Radio had to reset itself due to a critical (unrecoverable) issue it discovered during self-monitoring: %s.",
"args": [
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "issue",
"type": {
"py/id": 296
},
"enum": [
{
"py/id": 3134
},
{
"py/id": 3136
},
{
"py/id": 3138
},
{
"py/id": 3140
},
{
"py/id": 3142
},
{
"py/id": 3144
},
{
"py/id": 3146
},
{
"py/id": 3148
},
{
"py/id": 3150
},
{
"py/id": 3152
},
{
"py/id": 3154
},
{
"py/id": 3156
},
{
"py/id": 3158
},
{
"py/id": 3160
},
{
"py/id": 3162
},
{
"py/id": 3164
},
{
"py/id": 3166
},
{
"py/id": 3168
},
{
"py/id": 3170
},
{
"py/id": 3172
},
{
"py/id": 3174
},
{
"py/id": 3176
},
{
"py/id": 3178
},
{
"py/id": 3180
},
{
"py/id": 3182
},
{
"py/id": 3184
},
{
"py/id": 3186
},
{
"py/id": 3188
},
{
"py/id": 3190
},
{
"py/id": 3192
},
{
"py/id": 3194
},
{
"py/id": 3196
},
{
"py/id": 3198
},
{
"py/id": 3200
},
{
"py/id": 3202
},
{
"py/id": 3204
},
{
"py/id": 3206
},
{
"py/id": 3208
},
{
"py/id": 3210
},
{
"py/id": 3212
},
{
"py/id": 3214
},
{
"py/id": 3216
},
{
"py/id": 3218
},
{
"py/id": 3220
},
{
"py/id": 3222
},
{
"py/id": 3224
},
{
"py/id": 3226
},
{
"py/id": 3228
},
{
"py/id": 3230
},
{
"py/id": 3232
},
{
"py/id": 3234
},
{
"py/id": 3236
},
{
"py/id": 3238
},
{
"py/id": 3240
}
],
"bitfields": null,
"metadata": {}
}
}
],
"metadata": {}
}
}
},
{
"names": [
"CriticalResetDueToSoftwareException"
],
"ID": 244,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Event",
"py/state": {
"name": "CriticalResetDueToSoftwareException",
"ID": 244,
"severity": "FATAL",
"format_string": "Radio had to reset itself due to a critical software exception. The exception occurred at memory address: 0x%04X and has numerical type: %s.",
"args": [
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "memory_address",
"type": {
"py/id": 15
},
"enum": [],
"bitfields": null,
"metadata": {}
}
},
{
"py/object": "IrisBackendv3.data_standards.module.Argument",
"py/state": {
"name": "exception_type",
"type": {
"py/id": 217
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
],
"metadata": {}
}
}
}
]
}
}
}
},
{
"names": [
"WatchdogHeartbeatTvac"
],
"ID": 64000,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Module",
"py/state": {
"name": "WatchdogHeartbeatTvac",
"ID": 64000,
"commands": {
"py/object": "IrisBackendv3.utils.nameiddict.NameIdDict",
"id_dict": {},
"name_id_mapping": {}
},
"telemetry": {
"py/object": "IrisBackendv3.utils.nameiddict.NameIdDict",
"py/state": [
{
"names": [
"AdcTempRaw"
],
"ID": 0,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "AdcTempRaw",
"ID": 0,
"type": {
"py/id": 450
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"AdcTempKelvin"
],
"ID": 1,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "AdcTempKelvin",
"ID": 1,
"type": {
"py/id": 2820
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"ChargeRaw"
],
"ID": 16,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "ChargeRaw",
"ID": 16,
"type": {
"py/id": 450
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"ChargeMah"
],
"ID": 17,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "ChargeMah",
"ID": 17,
"type": {
"py/id": 2820
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"VoltageRaw"
],
"ID": 32,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "VoltageRaw",
"ID": 32,
"type": {
"py/id": 450
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"Voltage"
],
"ID": 33,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "Voltage",
"ID": 33,
"type": {
"py/id": 2820
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"CurrentRaw"
],
"ID": 48,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "CurrentRaw",
"ID": 48,
"type": {
"py/id": 450
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"CurrentAmps"
],
"ID": 49,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "CurrentAmps",
"ID": 49,
"type": {
"py/id": 2820
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"FuelTempRaw"
],
"ID": 64,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "FuelTempRaw",
"ID": 64,
"type": {
"py/id": 450
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"FuelTempKelvin"
],
"ID": 65,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "FuelTempKelvin",
"ID": 65,
"type": {
"py/id": 2820
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"KpHeater"
],
"ID": 80,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "KpHeater",
"ID": 80,
"type": {
"py/id": 450
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"HeaterSetpoint"
],
"ID": 81,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "HeaterSetpoint",
"ID": 81,
"type": {
"py/id": 450
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"HeaterSetpointKelvin"
],
"ID": 82,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "HeaterSetpointKelvin",
"ID": 82,
"type": {
"py/id": 2820
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"HeaterWindow"
],
"ID": 83,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "HeaterWindow",
"ID": 83,
"type": {
"py/id": 450
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"HeaterWindowKelvin"
],
"ID": 84,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "HeaterWindowKelvin",
"ID": 84,
"type": {
"py/id": 2820
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"HeaterPwmLimit"
],
"ID": 85,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "HeaterPwmLimit",
"ID": 85,
"type": {
"py/id": 450
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"WatchdogMode"
],
"ID": 86,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "WatchdogMode",
"ID": 86,
"type": {
"py/id": 296
},
"enum": [
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "INIT",
"value": 0,
"comment": "The state that the state machine is in during initialization."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "ENTERING_KEEP_ALIVE",
"value": 7,
"comment": "Performing all non-instant tasks to set up the appropriate system state for KEEP_ALIVE"
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "KEEP_ALIVE",
"value": 8,
"comment": "In keep alive mode, used in transit to use little power but stay in communication"
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "ENTERING_SERVICE",
"value": 3,
"comment": "Performing all non-instant tasks to set up the appropriate system state for SERVICE"
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "SERVICE",
"value": 4,
"comment": "In diagnostic service mode, which is also a precondition of entering MISSION"
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "ENTERING_MISSION",
"value": 15,
"comment": " Performing all non-instant tasks to set up the appropriate system state for MISSION."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "MISSION",
"value": 16,
"comment": "In the fully-powered on mode to perform the mission."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "ENTERING_STASIS",
"value": 31,
"comment": " Performing all non-instant tasks to set up the appropriate system state for STASIS"
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "STASIS",
"value": 32,
"comment": "In mission, enter low-power stasis."
}
}
],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"HeaterStatus"
],
"ID": 87,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "HeaterStatus",
"ID": 87,
"type": {
"py/id": 217
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"HeatingControlEnabled"
],
"ID": 88,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "HeatingControlEnabled",
"ID": 88,
"type": {
"py/id": 217
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"HeaterPwmDutyCycle"
],
"ID": 89,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "HeaterPwmDutyCycle",
"ID": 89,
"type": {
"py/id": 450
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
}
]
},
"events": {
"py/object": "IrisBackendv3.utils.nameiddict.NameIdDict",
"id_dict": {},
"name_id_mapping": {}
}
}
}
},
{
"names": [
"WatchdogHeartbeat"
],
"ID": 64256,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Module",
"py/state": {
"name": "WatchdogHeartbeat",
"ID": 64256,
"commands": {
"py/object": "IrisBackendv3.utils.nameiddict.NameIdDict",
"id_dict": {},
"name_id_mapping": {}
},
"telemetry": {
"py/object": "IrisBackendv3.utils.nameiddict.NameIdDict",
"py/state": [
{
"names": [
"BattAdcTempRaw"
],
"ID": 0,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "BattAdcTempRaw",
"ID": 0,
"type": {
"py/id": 217
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"BattAdcTempKelvin"
],
"ID": 1,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "BattAdcTempKelvin",
"ID": 1,
"type": {
"py/id": 2820
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"ChargeRaw"
],
"ID": 16,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "ChargeRaw",
"ID": 16,
"type": {
"py/id": 217
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"ChargeMah"
],
"ID": 17,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "ChargeMah",
"ID": 17,
"type": {
"py/id": 2820
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"ChargePercent"
],
"ID": 18,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "ChargePercent",
"ID": 18,
"type": {
"py/id": 2820
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"BatteryVoltageOk"
],
"ID": 34,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "BatteryVoltageOk",
"ID": 34,
"type": {
"py/id": 217
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"CurrentRaw"
],
"ID": 48,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "CurrentRaw",
"ID": 48,
"type": {
"py/id": 217
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"CurrentMilliamps"
],
"ID": 49,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "CurrentMilliamps",
"ID": 49,
"type": {
"py/id": 2820
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"HeaterStatus"
],
"ID": 87,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "HeaterStatus",
"ID": 87,
"type": {
"py/id": 217
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
}
]
},
"events": {
"py/object": "IrisBackendv3.utils.nameiddict.NameIdDict",
"id_dict": {},
"name_id_mapping": {}
}
}
}
},
{
"names": [
"WatchdogDetailedStatus"
],
"ID": 64512,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Module",
"py/state": {
"name": "WatchdogDetailedStatus",
"ID": 64512,
"commands": {
"py/object": "IrisBackendv3.utils.nameiddict.NameIdDict",
"id_dict": {},
"name_id_mapping": {}
},
"telemetry": {
"py/object": "IrisBackendv3.utils.nameiddict.NameIdDict",
"py/state": [
{
"names": [
"Io_ChargingStatus1"
],
"ID": 0,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "Io_ChargingStatus1",
"ID": 0,
"type": {
"py/id": 217
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"Io_ChargingStatus2"
],
"ID": 1,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "Io_ChargingStatus2",
"ID": 1,
"type": {
"py/id": 217
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"Io_ChargerState"
],
"ID": 2,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "Io_ChargerState",
"ID": 2,
"type": {
"py/id": 296
},
"enum": [
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "OFF",
"value": 0,
"comment": ""
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "CHARGING",
"value": 1,
"comment": ""
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "DONE_CHARGING",
"value": 2,
"comment": ""
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "FAULT",
"value": 3,
"comment": ""
}
}
],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"Io_BatteryConnectionStatus"
],
"ID": 3,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "Io_BatteryConnectionStatus",
"ID": 3,
"type": {
"py/id": 296
},
"enum": [
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "DISCONNECTED",
"value": 0,
"comment": ""
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "CONNECTED",
"value": 1,
"comment": ""
}
}
],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"Io_BatteryLatchStatus"
],
"ID": 4,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "Io_BatteryLatchStatus",
"ID": 4,
"type": {
"py/id": 296
},
"enum": [
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "NOT_LATCHED",
"value": 0,
"comment": ""
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "LATCHED",
"value": 1,
"comment": ""
}
}
],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"Io_BatteryState"
],
"ID": 5,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "Io_BatteryState",
"ID": 5,
"type": {
"py/id": 296
},
"enum": [
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "DISCONNECTED",
"value": 0,
"comment": ""
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "OR_GATE__FAULT",
"value": 1,
"comment": ""
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "TEMP_CONNECTED",
"value": 2,
"comment": ""
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "LATCHED_CONNECTED",
"value": 3,
"comment": ""
}
}
],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"Io_1V2PowerGood"
],
"ID": 6,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "Io_1V2PowerGood",
"ID": 6,
"type": {
"py/id": 296
},
"enum": [
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "BAD",
"value": 0,
"comment": ""
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "GOOD",
"value": 1,
"comment": ""
}
}
],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"Io_1V8PowerGood"
],
"ID": 7,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "Io_1V8PowerGood",
"ID": 7,
"type": {
"py/id": 296
},
"enum": [
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "BAD",
"value": 0,
"comment": ""
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "GOOD",
"value": 1,
"comment": ""
}
}
],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"Io_3V3PowerGood"
],
"ID": 8,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "Io_3V3PowerGood",
"ID": 8,
"type": {
"py/id": 296
},
"enum": [
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "BAD",
"value": 0,
"comment": ""
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "GOOD",
"value": 1,
"comment": ""
}
}
],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"Io_5V0PowerGood"
],
"ID": 9,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "Io_5V0PowerGood",
"ID": 9,
"type": {
"py/id": 296
},
"enum": [
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "BAD",
"value": 0,
"comment": ""
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "GOOD",
"value": 1,
"comment": ""
}
}
],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"Watchdog_State"
],
"ID": 16,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "Watchdog_State",
"ID": 16,
"type": {
"py/id": 296
},
"enum": [
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "RS_INIT",
"value": 0,
"comment": "The state that the state machine is in during initialization."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "RS_ENTERING_KEEP_ALIVE",
"value": 7,
"comment": "Performing all non-instant tasks to set up the appropriate system state for KEEP_ALIVE"
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "RS_KEEP_ALIVE",
"value": 8,
"comment": "In keep alive mode, used in transit to use little power but stay in communication"
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "RS_ENTERING_SERVICE",
"value": 3,
"comment": "Performing all non-instant tasks to set up the appropriate system state for SERVICE"
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "RS_SERVICE",
"value": 4,
"comment": "In diagnostic service mode, which is also a precondition of entering MISSION"
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "RS_ENTERING_MISSION",
"value": 15,
"comment": " Performing all non-instant tasks to set up the appropriate system state for MISSION."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "RS_MISSION",
"value": 16,
"comment": "In the fully-powered on mode to perform the mission."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "RS_ENTERING_STASIS",
"value": 31,
"comment": " Performing all non-instant tasks to set up the appropriate system state for STASIS"
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "RS_STASIS",
"value": 32,
"comment": "In mission, enter low-power stasis."
}
}
],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"Watchdog_DeploymentStatus"
],
"ID": 17,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "Watchdog_DeploymentStatus",
"ID": 17,
"type": {
"py/id": 296
},
"enum": [
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "NOT_DEPLOYED",
"value": 0,
"comment": ""
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "DEPLOYING",
"value": 1,
"comment": ""
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "DEPLOYED",
"value": 2,
"comment": ""
}
}
],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"Watchdog_Uart0State"
],
"ID": 18,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "Watchdog_Uart0State",
"ID": 18,
"type": {
"py/id": 296
},
"enum": [
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "OFF",
"value": 0,
"comment": ""
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "INITIALIZED_&_ACTIVE",
"value": 1,
"comment": ""
}
}
],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"Watchdog_Uart1State"
],
"ID": 19,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "Watchdog_Uart1State",
"ID": 19,
"type": {
"py/id": 296
},
"enum": [
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "OFF",
"value": 0,
"comment": ""
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "INITIALIZED_&_ACTIVE",
"value": 1,
"comment": ""
}
}
],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"Watchdog_DetailedHeartbeatSequenceNumber"
],
"ID": 20,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "Watchdog_DetailedHeartbeatSequenceNumber",
"ID": 20,
"type": {
"py/id": 217
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"Watchdog_DigitalOutputStates"
],
"ID": 21,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "Watchdog_DigitalOutputStates",
"ID": 21,
"type": {
"py/id": 15
},
"enum": [],
"bitfields": {
"py/object": "IrisBackendv3.data_standards.module.BitfieldStruct",
"py/state": {
"fields": {
"py/reduce": [
{
"py/type": "collections.OrderedDict"
},
{
"py/tuple": []
},
null,
null,
{
"py/tuple": [
{
"py/tuple": [
"OPSBI__V_LANDER_REG_EN",
1
]
},
{
"py/tuple": [
"OPSBI__HEATER",
1
]
},
{
"py/tuple": [
"OPSBI__DEPLOYMENT",
1
]
},
{
"py/tuple": [
"OPSBI__FPGA_KICK_AKA_CAM_SELECT",
1
]
},
{
"py/tuple": [
"OPSBI__LATCH_BATT",
1
]
},
{
"py/tuple": [
"OPSBI__3V3_EN",
1
]
},
{
"py/tuple": [
"OPSBI__HERCULES_ON",
1
]
},
{
"py/tuple": [
"OPSBI__FPGA_ON",
1
]
},
{
"py/tuple": [
"OPSBI__MOTOR_ON",
1
]
},
{
"py/tuple": [
"OPSBI__CHRG_EN",
1
]
},
{
"py/tuple": [
"OPSBI__CHRG_EN_FORCE_HIGH",
1
]
},
{
"py/tuple": [
"OPSBI__BATTERY_EN",
1
]
},
{
"py/tuple": [
"OPSBI__V_SYS_ALL_EN",
1
]
},
{
"py/tuple": [
"OPSBI__V_SYS_ALL_EN_FORCE_LOW",
1
]
},
{
"py/tuple": [
"OPSBI__HERCULES_N_RST",
1
]
},
{
"py/tuple": [
"OPSBI__HERCULES_N_PORRST",
1
]
},
{
"py/tuple": [
"OPSBI__FPGA_N_RST",
1
]
},
{
"py/tuple": [
"OPSBI__RADIO_N_RST",
1
]
},
{
"py/tuple": [
"OPSBI__RADIO_ON",
1
]
},
{
"py/tuple": [
"OPSBI__BMS_BOOT",
1
]
},
{
"py/tuple": [
"OPSBI__LATCH_SET",
1
]
},
{
"py/tuple": [
"OPSBI__LATCH_RESET",
1
]
},
{
"py/tuple": [
"OPSBI__BATT_STAT",
1
]
},
{
"py/tuple": [
"OPSBI__RADIO_N_RESET_IS_INPUT",
1
]
},
{
"py/tuple": [
"OPSBI__HERCULES_N_RST_IS_INPUT",
1
]
},
{
"py/tuple": [
"OPSBI__HERCULES_N_PORRST_IS_INPUT",
1
]
},
{
"py/tuple": [
"OPSBI__FPGA_N_RST_IS_INPUT",
1
]
},
{
"py/tuple": [
"OPSBI__LATCH_SET_IS_INPUT",
1
]
},
{
"py/tuple": [
"OPSBI__LATCH_RESET_IS_INPUT",
1
]
},
{
"py/tuple": [
"OPSBI__BATT_STAT_IS_INPUT",
1
]
}
]
}
]
},
"padding": 2
}
},
"metadata": {}
}
}
},
{
"names": [
"Watchdog_CombinedDigitalStates"
],
"ID": 23,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "Watchdog_CombinedDigitalStates",
"ID": 23,
"type": {
"py/id": 15
},
"enum": [],
"bitfields": {
"py/object": "IrisBackendv3.data_standards.module.BitfieldStruct",
"py/state": {
"fields": {
"py/reduce": [
{
"py/type": "collections.OrderedDict"
},
{
"py/tuple": []
},
null,
null,
{
"py/tuple": [
{
"py/tuple": [
"V_LANDER_REG_EN",
1
]
},
{
"py/tuple": [
"HEATER",
1
]
},
{
"py/tuple": [
"DEPLOYMENT",
1
]
},
{
"py/tuple": [
"FPGA_KICK_AKA_CAM_SELECT",
1
]
},
{
"py/tuple": [
"LATCH_BATT",
1
]
},
{
"py/tuple": [
"3V3_EN",
1
]
},
{
"py/tuple": [
"HERCULES_ON",
1
]
},
{
"py/tuple": [
"FPGA_ON",
1
]
},
{
"py/tuple": [
"MOTOR_ON",
1
]
},
{
"py/tuple": [
"CHRG_EN",
2
]
},
{
"py/tuple": [
"BATTERY_EN",
1
]
},
{
"py/tuple": [
"V_SYS_ALL_EN",
2
]
},
{
"py/tuple": [
"HERCULES_N_RST",
2
]
},
{
"py/tuple": [
"HERCULES_N_PORRST",
2
]
},
{
"py/tuple": [
"FPGA_N_RST",
2
]
},
{
"py/tuple": [
"RADIO_N_RST",
2
]
},
{
"py/tuple": [
"RADIO_ON",
1
]
},
{
"py/tuple": [
"BMS_BOOT",
1
]
},
{
"py/tuple": [
"LATCH_SET",
2
]
},
{
"py/tuple": [
"LATCH_RESET",
2
]
},
{
"py/tuple": [
"BATT_STAT",
2
]
}
]
}
]
},
"padding": 2
}
},
"metadata": {}
}
}
},
{
"names": [
"Watchdog_ResetLogs"
],
"ID": 24,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "Watchdog_ResetLogs",
"ID": 24,
"type": {
"py/id": 2506
},
"enum": [],
"bitfields": {
"py/object": "IrisBackendv3.data_standards.module.BitfieldStruct",
"py/state": {
"fields": {
"py/reduce": [
{
"py/type": "collections.OrderedDict"
},
{
"py/tuple": []
},
null,
null,
{
"py/tuple": [
{
"py/tuple": [
"RABI__NO_RESET",
1
]
},
{
"py/tuple": [
"RABI__HERCULES_RESET",
1
]
},
{
"py/tuple": [
"RABI__HERCULES_UNRESET",
1
]
},
{
"py/tuple": [
"RABI__HERCULES_POWER_ON",
1
]
},
{
"py/tuple": [
"RABI__HERCULES_POWER_OFF",
1
]
},
{
"py/tuple": [
"RABI__RADIO_RESET",
1
]
},
{
"py/tuple": [
"RABI__RADIO_UNRESET",
1
]
},
{
"py/tuple": [
"RABI__RADIO_POWER_ON",
1
]
},
{
"py/tuple": [
"RABI__RADIO_POWER_OFF",
1
]
},
{
"py/tuple": [
"RABI__CAM_FPGA_RESET",
1
]
},
{
"py/tuple": [
"RABI__CAM_FPGA_UNRESET",
1
]
},
{
"py/tuple": [
"RABI__CAM_FPGA_POWER_ON",
1
]
},
{
"py/tuple": [
"RABI__CAM_FPGA_POWER_OFF",
1
]
},
{
"py/tuple": [
"RABI__ALL_MOTORS_POWER_ON",
1
]
},
{
"py/tuple": [
"RABI__ALL_MOTORS_POWER_OFF",
1
]
},
{
"py/tuple": [
"RABI__3V3_EN_RESET",
1
]
},
{
"py/tuple": [
"RABI__3V3_EN_UNRESET",
1
]
},
{
"py/tuple": [
"RABI__3V3_EN_POWER_ON",
1
]
},
{
"py/tuple": [
"RABI__3V3_EN_POWER_OFF",
1
]
},
{
"py/tuple": [
"RABI__V_SYS_ALL_OFF__RESET",
1
]
},
{
"py/tuple": [
"RABI__V_SYS_ALL_ON__UNRESET",
1
]
},
{
"py/tuple": [
"RABI__V_SYS_ALL_POWER_ON",
1
]
},
{
"py/tuple": [
"RABI__V_SYS_ALL_POWER_OFF",
1
]
},
{
"py/tuple": [
"RABI__HDRM_DEPLOY_SIGNAL_POWER_OFF",
1
]
},
{
"py/tuple": [
"RABI__FPGA_CAM_0_SELECT",
1
]
},
{
"py/tuple": [
"RABI__FPGA_CAM_1_SELECT",
1
]
},
{
"py/tuple": [
"RABI__BATTERY_CHARGE_START",
1
]
},
{
"py/tuple": [
"RABI__BATTERY_CHARGE_STOP",
1
]
},
{
"py/tuple": [
"RABI__RS422_UART_ENABLE",
1
]
},
{
"py/tuple": [
"RABI__RS422_UART_DISABLE",
1
]
},
{
"py/tuple": [
"RABI__AUTO_HEATER_CONTROLLER_ENABLE",
1
]
},
{
"py/tuple": [
"RABI__AUTO_HEATER_CONTROLLER_DISABLE",
1
]
},
{
"py/tuple": [
"RABI__HERCULES_WATCHDOG_ENABLE",
1
]
},
{
"py/tuple": [
"RABI__HERCULES_WATCHDOG_DISABLE",
1
]
},
{
"py/tuple": [
"RABI__BATTERIES_ENABLE",
1
]
},
{
"py/tuple": [
"RABI__BATTERIES_DISABLE",
1
]
},
{
"py/tuple": [
"RABI__HDRM_DEPLOY_SIGNAL_POWER_ON",
1
]
},
{
"py/tuple": [
"RABI__HERCULES_WATCHDOG_RESET",
1
]
}
]
}
]
},
"padding": 26
}
},
"metadata": {}
}
}
},
{
"names": [
"Adc_LanderVoltageRaw"
],
"ID": 32,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "Adc_LanderVoltageRaw",
"ID": 32,
"type": {
"py/id": 217
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"Adc_LanderVoltage"
],
"ID": 33,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "Adc_LanderVoltage",
"ID": 33,
"type": {
"py/id": 2820
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"Adc_BatteryChargingTempRaw"
],
"ID": 34,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "Adc_BatteryChargingTempRaw",
"ID": 34,
"type": {
"py/id": 450
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"Adc_BatteryChargingTempKelvin"
],
"ID": 35,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "Adc_BatteryChargingTempKelvin",
"ID": 35,
"type": {
"py/id": 2820
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"Adc_BatteryChargingTempUncertaintyKelvin"
],
"ID": 36,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "Adc_BatteryChargingTempUncertaintyKelvin",
"ID": 36,
"type": {
"py/id": 2820
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"Adc_BatteryTempRaw"
],
"ID": 37,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "Adc_BatteryTempRaw",
"ID": 37,
"type": {
"py/id": 450
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"Adc_BatteryTempKelvin"
],
"ID": 38,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "Adc_BatteryTempKelvin",
"ID": 38,
"type": {
"py/id": 2820
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"Adc_BatteryTempUncertaintyKelvin"
],
"ID": 39,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "Adc_BatteryTempUncertaintyKelvin",
"ID": 39,
"type": {
"py/id": 2820
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"Adc_FullSystemVoltageRaw"
],
"ID": 40,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "Adc_FullSystemVoltageRaw",
"ID": 40,
"type": {
"py/id": 217
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"Adc_FullSystemVoltage"
],
"ID": 41,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "Adc_FullSystemVoltage",
"ID": 41,
"type": {
"py/id": 2820
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"Adc_FullSystemCurrentRaw"
],
"ID": 42,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "Adc_FullSystemCurrentRaw",
"ID": 42,
"type": {
"py/id": 450
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"Adc_FullSystemCurrent"
],
"ID": 43,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "Adc_FullSystemCurrent",
"ID": 43,
"type": {
"py/id": 2820
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"Adc_SwitchedBatteryVoltageRaw"
],
"ID": 44,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "Adc_SwitchedBatteryVoltageRaw",
"ID": 44,
"type": {
"py/id": 450
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"Adc_SwitchedBatteryVoltage"
],
"ID": 45,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "Adc_SwitchedBatteryVoltage",
"ID": 45,
"type": {
"py/id": 2820
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"Adc_2V5VoltageRaw"
],
"ID": 48,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "Adc_2V5VoltageRaw",
"ID": 48,
"type": {
"py/id": 217
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"Adc_2V5Voltage"
],
"ID": 49,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "Adc_2V5Voltage",
"ID": 49,
"type": {
"py/id": 2820
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"Adc_2V8VoltageRaw"
],
"ID": 50,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "Adc_2V8VoltageRaw",
"ID": 50,
"type": {
"py/id": 217
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"Adc_2V8Voltage"
],
"ID": 51,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "Adc_2V8Voltage",
"ID": 51,
"type": {
"py/id": 2820
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"Adc_Vcc28VoltageRaw"
],
"ID": 52,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "Adc_Vcc28VoltageRaw",
"ID": 52,
"type": {
"py/id": 217
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"Adc_Vcc28Voltage"
],
"ID": 53,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "Adc_Vcc28Voltage",
"ID": 53,
"type": {
"py/id": 2820
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"Adc_Vcc24VoltageRaw"
],
"ID": 54,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "Adc_Vcc24VoltageRaw",
"ID": 54,
"type": {
"py/id": 217
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"Adc_Vcc24Voltage"
],
"ID": 55,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "Adc_Vcc24Voltage",
"ID": 55,
"type": {
"py/id": 2820
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"Heater_Kp"
],
"ID": 64,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "Heater_Kp",
"ID": 64,
"type": {
"py/id": 450
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"Heater_PwmLimit_DutyCycleCounter"
],
"ID": 65,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "Heater_PwmLimit_DutyCycleCounter",
"ID": 65,
"type": {
"py/id": 450
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"Heater_PwmLimit_DutyCyclePercent"
],
"ID": 66,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "Heater_PwmLimit_DutyCyclePercent",
"ID": 66,
"type": {
"py/id": 2820
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"Heater_EffectivePowerLimit"
],
"ID": 67,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "Heater_EffectivePowerLimit",
"ID": 67,
"type": {
"py/id": 2820
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"Heater_SetpointValue"
],
"ID": 68,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "Heater_SetpointValue",
"ID": 68,
"type": {
"py/id": 450
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"Heater_SetpointKelvin"
],
"ID": 69,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "Heater_SetpointKelvin",
"ID": 69,
"type": {
"py/id": 2820
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"Heater_OnValue"
],
"ID": 70,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "Heater_OnValue",
"ID": 70,
"type": {
"py/id": 450
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"Heater_OnTempKelvin"
],
"ID": 71,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "Heater_OnTempKelvin",
"ID": 71,
"type": {
"py/id": 2820
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"Heater_OffValue"
],
"ID": 72,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "Heater_OffValue",
"ID": 72,
"type": {
"py/id": 450
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"Heater_OffTempKelvin"
],
"ID": 73,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "Heater_OffTempKelvin",
"ID": 73,
"type": {
"py/id": 2820
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"Heater_ControlEnabled"
],
"ID": 74,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "Heater_ControlEnabled",
"ID": 74,
"type": {
"py/id": 296
},
"enum": [
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "DISABLED",
"value": 0,
"comment": ""
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "ENABLED",
"value": 1,
"comment": ""
}
}
],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"Heater_IsHeating"
],
"ID": 75,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "Heater_IsHeating",
"ID": 75,
"type": {
"py/id": 296
},
"enum": [
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "NOT_HEATING",
"value": 0,
"comment": ""
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "HEATING",
"value": 1,
"comment": ""
}
}
],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"Heater_DutyCyclePeriodCycles"
],
"ID": 76,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "Heater_DutyCyclePeriodCycles",
"ID": 76,
"type": {
"py/id": 450
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"Heater_DutyCyclePeriodMs"
],
"ID": 77,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "Heater_DutyCyclePeriodMs",
"ID": 77,
"type": {
"py/id": 2820
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"Heater_DutyCycleCounter"
],
"ID": 78,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "Heater_DutyCycleCounter",
"ID": 78,
"type": {
"py/id": 450
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"Heater_DutyCyclePercent"
],
"ID": 79,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "Heater_DutyCyclePercent",
"ID": 79,
"type": {
"py/id": 2820
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"Heater_EffectiveVoltage"
],
"ID": 80,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "Heater_EffectiveVoltage",
"ID": 80,
"type": {
"py/id": 2820
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"Heater_EffectivePower"
],
"ID": 81,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "Heater_EffectivePower",
"ID": 81,
"type": {
"py/id": 2820
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"I2C_BatteryChargeRaw"
],
"ID": 96,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "I2C_BatteryChargeRaw",
"ID": 96,
"type": {
"py/id": 450
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"I2C_BatteryChargeMah"
],
"ID": 97,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "I2C_BatteryChargeMah",
"ID": 97,
"type": {
"py/id": 2820
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"I2C_BatteryVoltageRaw"
],
"ID": 98,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "I2C_BatteryVoltageRaw",
"ID": 98,
"type": {
"py/id": 450
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"I2C_BatteryVoltage"
],
"ID": 99,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "I2C_BatteryVoltage",
"ID": 99,
"type": {
"py/id": 2820
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"I2C_BatteryCurrentRaw"
],
"ID": 100,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "I2C_BatteryCurrentRaw",
"ID": 100,
"type": {
"py/id": 450
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"I2C_BatteryCurrent"
],
"ID": 101,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "I2C_BatteryCurrent",
"ID": 101,
"type": {
"py/id": 2820
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"I2C_FuelGaugeTempRaw"
],
"ID": 102,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "I2C_FuelGaugeTempRaw",
"ID": 102,
"type": {
"py/id": 450
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"I2C_FuelGaugeTempKelvin"
],
"ID": 103,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "I2C_FuelGaugeTempKelvin",
"ID": 103,
"type": {
"py/id": 2820
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"I2C_BatteryChargeTelemRaw"
],
"ID": 104,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "I2C_BatteryChargeTelemRaw",
"ID": 104,
"type": {
"py/id": 450
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"I2C_BatteryChargeTelemMah"
],
"ID": 105,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "I2C_BatteryChargeTelemMah",
"ID": 105,
"type": {
"py/id": 2820
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"I2C_BatteryCurrentTelemRaw"
],
"ID": 106,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "I2C_BatteryCurrentTelemRaw",
"ID": 106,
"type": {
"py/id": 217
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"I2C_BatteryCurrentTelemAmps"
],
"ID": 107,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "I2C_BatteryCurrentTelemAmps",
"ID": 107,
"type": {
"py/id": 2820
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
}
]
},
"events": {
"py/object": "IrisBackendv3.utils.nameiddict.NameIdDict",
"id_dict": {},
"name_id_mapping": {}
}
}
}
},
{
"names": [
"WatchdogCommandResponse"
],
"ID": 64768,
"val": {
"py/object": "IrisBackendv3.data_standards.module.Module",
"py/state": {
"name": "WatchdogCommandResponse",
"ID": 64768,
"commands": {
"py/object": "IrisBackendv3.utils.nameiddict.NameIdDict",
"id_dict": {},
"name_id_mapping": {}
},
"telemetry": {
"py/object": "IrisBackendv3.utils.nameiddict.NameIdDict",
"py/state": [
{
"names": [
"CommandId"
],
"ID": 0,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "CommandId",
"ID": 0,
"type": {
"py/id": 217
},
"enum": [],
"bitfields": null,
"metadata": {}
}
}
},
{
"names": [
"ErrorFlag"
],
"ID": 1,
"val": {
"py/object": "IrisBackendv3.data_standards.module.TelemetryChannel",
"py/state": {
"name": "ErrorFlag",
"ID": 1,
"type": {
"py/id": 296
},
"enum": [
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "NO_ERROR",
"value": 0,
"comment": "Command processed correctly."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "BAD_PACKET_LENGTH",
"value": 1,
"comment": "Given packet length doesn't match the actual length of the data. Note: associated reply command ID may be wrong."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "CHECKSUM_FAILED",
"value": 2,
"comment": "Checksum of data doesn't match given checksum. Possible packet corruption. Note: associated reply command ID may be wrong."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "BAD_MODULE_ID",
"value": 3,
"comment": "Incorrect Module ID received (not the watchdog ID). Note: associated reply command ID may be wrong."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "BAD_COMMAND_ID",
"value": 4,
"comment": "Command ID received doesn't match any known command. Note: associated reply command ID may be wrong."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "BAD_COMMAND_PARAMETER",
"value": 5,
"comment": "Command parameter (argument) isn't formatted correctly or doesn't match an expected value (in the case of magic confirmation values)."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "BAD_COMMAND_SEND_ORDER",
"value": 6,
"comment": "Command received in the wrong order (e.g. `Deploy` received before `Prepare for Deploy`)."
}
},
{
"py/object": "IrisBackendv3.data_standards.module.EnumItem",
"py/state": {
"name": "DEPLOYMENT_SIGNAL_SENT",
"value": 96,
"comment": "WatchDog Deployment Interlock Released. (Watchdog HDRM pin is high)."
}
}
],
"bitfields": null,
"metadata": {}
}
}
}
]
},
"events": {
"py/object": "IrisBackendv3.utils.nameiddict.NameIdDict",
"id_dict": {},
"name_id_mapping": {}
}
}
}
}
]
}
