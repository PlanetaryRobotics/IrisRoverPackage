"""
Special prebuilt data standards which exist outside of the FPrime XML (e.g. for 
watchdog heartbeat)

@author: Connor W. Colombo (CMU)
@last-updated: 10/16/2021
"""
from typing import Union, List

from IrisBackendv3.utils.nameiddict import NameIdDict

from .data_standards import DataStandards
from .module import Module, Command, Argument, TelemetryChannel, EnumItem
from .fsw_data_type import FswDataType
from .logging import logger


def add_to_standards(standards: DataStandards, modules: Union[Module, List[Module]]) -> None:
    """
    Adds the given prebuilt module to the given data so long as it doesn't 
    override anything.
    """
    # Support being given just a single module to add:
    if not isinstance(modules, list) and isinstance(modules, Module):
        modules = [modules]
    # Add each module to standards:
    for module in modules:
        if module.ID not in standards.modules:
            standards.modules[module.ID, module.name] = module
            tmp = standards.modules
            standards.modules = tmp
        else:
            logger.warning(
                f"Unable to add special prebuilt module {module} since its ID "
                f"({module.ID}={hex(module.ID)}) is already in the DataStandard."
            )


watchdog_heartbeat_tvac: Module = Module(
    name="WatchdogHeartbeatTvac",
    ID=0xFF00,
    commands=NameIdDict(),
    events=NameIdDict(),
    telemetry=NameIdDict({
        (0x00, 'AdcTempRaw'): TelemetryChannel(
            name='AdcTempRaw',
            ID=0x00,
            datatype=FswDataType.U16
        ),
        (0x01, 'AdcTempKelvin'): TelemetryChannel(
            name='AdcTempKelvin',
            ID=0x01,
            datatype=FswDataType.F64
        ),

        (0x10, 'ChargeRaw'): TelemetryChannel(
            name='ChargeRaw',
            ID=0x10,
            datatype=FswDataType.U16
        ),
        (0x11, 'ChargeMah'): TelemetryChannel(
            name='ChargeMah',
            ID=0x11,
            datatype=FswDataType.F64
        ),

        (0x20, 'VoltageRaw'): TelemetryChannel(
            name='VoltageRaw',
            ID=0x20,
            datatype=FswDataType.U16
        ),
        (0x21, 'Voltage'): TelemetryChannel(
            name='Voltage',
            ID=0x21,
            datatype=FswDataType.F64
        ),

        (0x30, 'CurrentRaw'): TelemetryChannel(
            name='CurrentRaw',
            ID=0x30,
            datatype=FswDataType.U16
        ),
        (0x31, 'CurrentAmps'): TelemetryChannel(
            name='CurrentAmps',
            ID=0x31,
            datatype=FswDataType.F64
        ),

        (0x40, 'FuelTempRaw'): TelemetryChannel(
            name='FuelTempRaw',
            ID=0x40,
            datatype=FswDataType.U16
        ),
        (0x41, 'FuelTempKelvin'): TelemetryChannel(
            name='FuelTempKelvin',
            ID=0x41,
            datatype=FswDataType.F64
        ),

        (0x50, 'KpHeater'): TelemetryChannel(
            name='KpHeater', ID=0x50, datatype=FswDataType.U16
        ),
        (0x51, 'HeaterSetpoint'): TelemetryChannel(
            name='HeaterSetpoint', ID=0x51, datatype=FswDataType.U16
        ),
        (0x52, 'HeaterSetpointKelvin'): TelemetryChannel(
            name='HeaterSetpointKelvin', ID=0x52, datatype=FswDataType.U16
        ),
        (0x53, 'HeaterWindow'): TelemetryChannel(
            name='HeaterWindow', ID=0x53, datatype=FswDataType.U16
        ),
        (0x54, 'HeaterWindowKelvin'): TelemetryChannel(
            name='HeaterWindowKelvin', ID=0x54, datatype=FswDataType.U16
        ),
        (0x55, 'HeaterPwmLimit'): TelemetryChannel(
            name='HeaterPwmLimit', ID=0x55, datatype=FswDataType.U16
        ),
        (0x56, 'WatchdogMode'): TelemetryChannel(
            name='WatchdogMode', ID=0x56, datatype=FswDataType.ENUM,
            enum=[
                EnumItem('RS_SLEEP', 0x02),
                EnumItem('RS_SERVICE', 0x04),
                EnumItem('RS_KEEPALIVE', 0x08),
                EnumItem('RS_MISSION', 0x10),
                EnumItem('RS_FAULT', 0x20)
            ]
        ),
        (0x57, 'HeaterStatus'): TelemetryChannel(
            name='HeaterStatus', ID=0x57, datatype=FswDataType.U8
        ),
        (0x58, 'HeatingControlEnabled'): TelemetryChannel(
            name='HeatingControlEnabled', ID=0x58, datatype=FswDataType.U8
        ),
        (0x59, 'HeaterPwmDutyCycle'): TelemetryChannel(
            name='HeaterPwmDutyCycle', ID=0x59, datatype=FswDataType.U16
        )
    })
)


watchdog_heartbeat: Module = Module(
    name="WatchdogHeartbeat",
    ID=0xFF01,
    commands=NameIdDict(),
    events=NameIdDict(),
    telemetry=NameIdDict({
        (0x00, 'BattAdcTempRaw'): TelemetryChannel(
            name='BattAdcTempRaw',
            ID=0x00,
            datatype=FswDataType.U8
        ),
        (0x01, 'BattAdcTempKelvin'): TelemetryChannel(
            name='BattAdcTempKelvin',
            ID=0x01,
            datatype=FswDataType.F64
        ),

        (0x10, 'ChargeRaw'): TelemetryChannel(
            name='ChargeRaw',
            ID=0x10,
            datatype=FswDataType.U8
        ),
        (0x11, 'ChargeMah'): TelemetryChannel(
            name='ChargeMah',
            ID=0x11,
            datatype=FswDataType.F64
        ),
        (0x12, 'ChargePercent'): TelemetryChannel(
            name='ChargePercent',
            ID=0x12,
            datatype=FswDataType.F64
        ),

        (0x22, 'BatteryVoltageOk'): TelemetryChannel(
            # Note: won't use FPrime BOOL std. (so treat as U8)
            name='BatteryVoltageOk', ID=0x22, datatype=FswDataType.U8
        ),

        (0x30, 'CurrentRaw'): TelemetryChannel(
            name='CurrentRaw',
            ID=0x30,
            datatype=FswDataType.U8
        ),
        (0x31, 'CurrentMilliamps'): TelemetryChannel(
            name='CurrentMilliamps',
            ID=0x31,
            datatype=FswDataType.F64
        ),

        (0x57, 'HeaterStatus'): TelemetryChannel(
            # Note: won't use FPrime BOOL std. (so treat as U8)
            name='HeaterStatus', ID=0x57, datatype=FswDataType.U8
        )
    })
)


watchdog_command_response: Module = Module(
    name="WatchdogCommandResponse",
    ID=0xFF10,
    commands=NameIdDict(),
    events=NameIdDict(),
    telemetry=NameIdDict({
        (0x00, 'CommandId'): TelemetryChannel(
            name='CommandId', ID=0x00, datatype=FswDataType.U8
        ),
        (0x01, 'ErrorFlag'): TelemetryChannel(
            name='ErrorFlag', ID=0x01, datatype=FswDataType.ENUM,
            enum=[
                EnumItem('NO_ERROR', 0x00,
                         comment="Command processed correctly."
                         ),
                EnumItem('BAD_PACKET_LENGTH', 0x01,
                         comment="Given packet length doesn't match the actual length of the data. Note: associated reply command ID may be wrong."
                         ),
                EnumItem('CHECKSUM_FAILED', 0x02,
                         comment="Checksum of data doesn't match given checksum. Possible packet corruption. Note: associated reply command ID may be wrong."
                         ),
                EnumItem('BAD_MODULE_ID', 0x03,
                         comment="Incorrect Module ID received (not the watchdog ID). Note: associated reply command ID may be wrong."
                         ),
                EnumItem('BAD_COMMAND_ID', 0x04,
                         comment="Command ID received doesn't match any known command. Note: associated reply command ID may be wrong."
                         ),
                EnumItem('BAD_COMMAND_PARAMETER', 0x05,
                         comment="Command parameter (argument) isn't formatted correctly or doesn't match an expected value (in the case of magic confirmation values)."
                         ),
                EnumItem('BAD_COMMAND_SEND_ORDER', 0x06,
                         comment="Command received in the wrong order (e.g. `Deploy` received before `Prepare for Deploy`)."
                         ),
                EnumItem('DEPLOYMENT_SIGNAL_SENT', 96,
                         comment="WatchDog Deployment Interlock Released. (Watchdog HDRM pin is high)."
                         )
            ]
        ),
    })
)