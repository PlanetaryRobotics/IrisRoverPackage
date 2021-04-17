"""
Special prebuilt data standards which exist outside of the FPrime XML (e.g. for 
watchdog heartbeat)

@author: Connor W. Colombo (CMU)
@last-updated: 04/17/2021
"""
from IrisBackendv3.utils.nameiddict import NameIdDict

from .data_standards import DataStandards
from .module import Module, TelemetryChannel, EnumItem
from .fsw_data_type import FswDataType
from .logging import logger


def add_to_standards(standards: DataStandards, module: Module) -> None:
    """
    Adds the given prebuilt module to the given data so long as it doesn't 
    override anything.
    """
    if module.ID not in standards.modules:
        standards.modules[module.ID, module.name] = module
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
