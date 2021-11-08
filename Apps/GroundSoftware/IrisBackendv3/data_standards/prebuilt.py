"""
Special prebuilt data standards which exist outside of the FPrime XML (e.g. for 
watchdog heartbeat)

@author: Connor W. Colombo (CMU)
@last-updated: 11/07/2021
"""
from typing import Any, Type, Union, List, Tuple, TypeVar
from collections import OrderedDict

from IrisBackendv3.utils.nameiddict import NameIdDict

from .data_standards import DataStandards
from .module import Module, TelemetryChannel, EnumItem, BitfieldStruct
from .fsw_data_type import FswDataType
from .logging import logger

T = TypeVar('T')


def nid_nic_entry(t: Type[Any], ID: int, name: str, *args, **kwargs) -> Tuple[Tuple[int, str], T]:
    """
    Makes a tuple which can be used as a name-id-containing `NameIdDict` entry 
    where the `name` and `ID` are also parameters of the entry value in addition 
    to being the keys in the NameIdDict (e.g. in `TelemetryChannels` or 
    `Commands`).
    This convenience function prevents the `name` and `ID` from needing to be 
    entered twice when manually instantiationing a NameIdDict with 
    name-id-containing entries, preventing them from possibly becoming out of 
    sync.
    """
    return ((ID, name), t(*args, name=name, ID=ID, **kwargs))


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
        else:
            logger.warning(
                f"Unable to add special prebuilt module {module} since its ID "
                f"({module.ID}={hex(module.ID)}) is already in the DataStandard."
            )


watchdog_heartbeat_tvac: Module = Module(
    name="WatchdogHeartbeatTvac",
    ID=0xFA00,
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
    ID=0xFB00,
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

watchdog_detailed_status_heartbeat: Module = Module(
    name="WatchdogDetailedStatus",
    ID=0xFC00,
    commands=NameIdDict(),
    events=NameIdDict(),
    telemetry=NameIdDict([
        # FROM I/Os:
        nid_nic_entry(
            TelemetryChannel, 0x00, 'Io_ChargingStatus1', datatype=FswDataType.U8  # 1b
        ),
        nid_nic_entry(
            TelemetryChannel, 0x01, 'Io_ChargingStatus2', datatype=FswDataType.U8  # 1b
        ),
        nid_nic_entry(
            TelemetryChannel, 0x02, 'Io_ChargerState', datatype=FswDataType.ENUM,  # calc
            enum=[  # cstat1 << 1 | cstat2
                EnumItem('OFF', 0b00),
                EnumItem('CHARGING', 0b01),
                EnumItem('DONE_CHARGING', 0b10),
                EnumItem('FAULT', 0b11),
            ]
        ),

        nid_nic_entry(
            TelemetryChannel, 0x03, 'Io_BatteryConnectionStatus', datatype=FswDataType.ENUM,  # 1b
            enum=[
                EnumItem('DISCONNECTED', 0x00),
                EnumItem('CONNECTED', 0x01)
            ]
        ), nid_nic_entry(
            TelemetryChannel, 0x04, 'Io_BatteryLatchStatus', datatype=FswDataType.ENUM,  # 1b
            enum=[
                EnumItem('NOT_LATCHED', 0x00),
                EnumItem('LATCHED', 0x01)
            ]
        ),
        nid_nic_entry(
            TelemetryChannel, 0x05, 'Io_BatteryState', datatype=FswDataType.ENUM,  # calc
            enum=[  # bstat << 1 | lstat
                EnumItem('DISCONNECTED', 0b00),
                EnumItem('OR_GATE__FAULT', 0b01),
                EnumItem('TEMP_CONNECTED', 0b10),
                EnumItem('LATCHED_CONNECTED', 0b11),
            ]
        ),

        nid_nic_entry(
            TelemetryChannel, 0x06, 'Io_1V2PowerGood', datatype=FswDataType.ENUM,  # 1b
            enum=[
                EnumItem('BAD', 0x00),
                EnumItem('GOOD', 0x01)
            ]
        ),
        nid_nic_entry(
            TelemetryChannel, 0x07, 'Io_1V8PowerGood', datatype=FswDataType.ENUM,  # 1b
            enum=[
                EnumItem('BAD', 0x00),
                EnumItem('GOOD', 0x01)
            ]
        ),
        nid_nic_entry(
            TelemetryChannel, 0x08, 'Io_3V3PowerGood', datatype=FswDataType.ENUM,  # 1b
            enum=[
                EnumItem('BAD', 0x00),
                EnumItem('GOOD', 0x01)
            ]
        ),
        nid_nic_entry(
            TelemetryChannel, 0x09, 'Io_5V0PowerGood', datatype=FswDataType.ENUM,  # 1b
            enum=[
                EnumItem('BAD', 0x00),
                EnumItem('GOOD', 0x01)
            ]
        ),


        # FROM WD STATE:
        nid_nic_entry(
            TelemetryChannel, 0x10, 'Watchdog_State', datatype=FswDataType.ENUM,  # 8b
            enum=[
                EnumItem(
                    'RS_INIT', 0,
                    comment='The state that the state machine is in during initialization.'
                ),
                EnumItem(
                    'RS_ENTERING_KEEP_ALIVE', 7,
                    comment='Performing all non-instant tasks to set up the appropriate system state for KEEP_ALIVE'
                ),
                EnumItem(
                    'RS_KEEP_ALIVE', 8,
                    comment='In keep alive mode, used in transit to use little power but stay in communication'
                ),
                EnumItem(
                    'RS_ENTERING_SERVICE', 3,
                    comment='Performing all non-instant tasks to set up the appropriate system state for SERVICE'
                ),
                EnumItem(
                    'RS_SERVICE', 4,
                    comment='In diagnostic service mode, which is also a precondition of entering MISSION'
                ),
                EnumItem(
                    'RS_ENTERING_MISSION', 15,
                    comment='Performing all non-instant tasks to set up the appropriate system state for MISSION.'
                ),
                EnumItem(
                    'RS_MISSION', 16,
                    comment='In the fully-powered on mode to perform the mission.'
                )
            ]
        ),

        nid_nic_entry(
            TelemetryChannel, 0x11, 'Watchdog_DeploymentStatus', datatype=FswDataType.ENUM,  # 2b
            enum=[
                EnumItem('NOT_DEPLOYED', 0b00),
                EnumItem('DEPLOYING', 0b01),
                EnumItem('DEPLOYED', 0b10)
            ]
        ),

        nid_nic_entry(
            TelemetryChannel, 0x12, 'Watchdog_Uart0State', datatype=FswDataType.ENUM,  # 1b
            enum=[
                EnumItem('OFF', 0x00),
                EnumItem('INITIALIZED_&_ACTIVE', 0x01)
            ]
        ),
        nid_nic_entry(
            TelemetryChannel, 0x13, 'Watchdog_Uart1State', datatype=FswDataType.ENUM,  # 1b
            enum=[
                EnumItem('OFF', 0x00),
                EnumItem('INITIALIZED_&_ACTIVE', 0x01)
            ]
        ),

        nid_nic_entry(
            TelemetryChannel, 0x14, 'Watchdog_DetailedHeartbeatSequenceNumber', datatype=FswDataType.U8  # 8b
        ),

        nid_nic_entry(
            TelemetryChannel, 0x15, 'Watchdog_DigitalOutputStates', datatype=FswDataType.U32,  # 32b
            bitfields=BitfieldStruct(
                fields=OrderedDict(reversed([
                    # Output statuses:
                    ('OPSBI__V_LANDER_REG_EN', 1),
                    ('OPSBI__HEATER', 1),
                    ('OPSBI__DEPLOYMENT', 1),
                    ('OPSBI__FPGA_KICK_AKA_CAM_SELECT', 1),
                    ('OPSBI__LATCH_BATT', 1),
                    ('OPSBI__3V3_EN', 1),
                    ('OPSBI__HERCULES_ON', 1),
                    ('OPSBI__FPGA_ON', 1),
                    ('OPSBI__MOTOR_ON', 1),
                    ('OPSBI__CHRG_EN', 1),
                    ('OPSBI__CHRG_EN_FORCE_HIGH', 1),
                    ('OPSBI__BATTERY_EN', 1),
                    ('OPSBI__V_SYS_ALL_EN', 1),
                    ('OPSBI__V_SYS_ALL_EN_FORCE_LOW', 1),
                    ('OPSBI__HERCULES_N_RST', 1),
                    ('OPSBI__HERCULES_N_PORRST', 1),
                    ('OPSBI__FPGA_N_RST', 1),
                    ('OPSBI__RADIO_N_RST', 1),
                    ('OPSBI__RADIO_ON', 1),
                    ('OPSBI__BMS_BOOT', 1),
                    ('OPSBI__LATCH_SET', 1),
                    ('OPSBI__LATCH_RESET', 1),
                    ('OPSBI__BATT_STAT', 1),
                    # Direction statuses:
                    ('OPSBI__RADIO_N_RESET_IS_INPUT', 1),
                    ('OPSBI__HERCULES_N_RST_IS_INPUT', 1),
                    ('OPSBI__HERCULES_N_PORRST_IS_INPUT', 1),
                    ('OPSBI__FPGA_N_RST_IS_INPUT', 1),
                    ('OPSBI__LATCH_SET_IS_INPUT', 1),
                    ('OPSBI__LATCH_RESET_IS_INPUT', 1),
                    ('OPSBI__BATT_STAT_IS_INPUT', 1)
                ])),
                total_padding=2
            )
        ),
        nid_nic_entry(
            TelemetryChannel, 0x17, 'Watchdog_CombinedDigitalStates', datatype=FswDataType.U32,  # calc
            bitfields=BitfieldStruct(
                fields=OrderedDict(reversed([
                    ('V_LANDER_REG_EN', 1),
                    ('HEATER', 1),
                    ('DEPLOYMENT', 1),
                    ('FPGA_KICK_AKA_CAM_SELECT', 1),
                    ('LATCH_BATT', 1),
                    ('3V3_EN', 1),
                    ('HERCULES_ON', 1),
                    ('FPGA_ON', 1),
                    ('MOTOR_ON', 1),
                    ('CHRG_EN', 2),  # input capable (0b10=Hi-Z)
                    ('BATTERY_EN', 1),
                    ('V_SYS_ALL_EN', 2),  # input capable (0b10=Hi-Z)
                    ('HERCULES_N_RST', 2),  # input capable (0b10=Hi-Z)
                    ('HERCULES_N_PORRST', 2),  # input capable (0b10=Hi-Z)
                    ('FPGA_N_RST', 2),  # input capable (0b10=Hi-Z)
                    ('RADIO_N_RST', 2),  # input capable (0b10=Hi-Z)
                    ('RADIO_ON', 1),
                    ('BMS_BOOT', 1),
                    ('LATCH_SET', 2),  # input capable (0b10=Hi-Z)
                    ('LATCH_RESET', 2),  # input capable (0b10=Hi-Z)
                    ('BATT_STAT', 2),  # input capable (0b10=Hi-Z)
                ])),
                total_padding=2
            )
        ),

        nid_nic_entry(
            TelemetryChannel, 0x18, 'Watchdog_ResetLogs', datatype=FswDataType.U64,  # 40b
            bitfields=BitfieldStruct(
                fields=OrderedDict(reversed([
                    ('RABI__NO_RESET', 1),
                    ('RABI__HERCULES_RESET', 1),
                    ('RABI__HERCULES_UNRESET', 1),
                    ('RABI__HERCULES_POWER_ON', 1),
                    ('RABI__HERCULES_POWER_OFF', 1),
                    ('RABI__RADIO_RESET', 1),
                    ('RABI__RADIO_UNRESET', 1),
                    ('RABI__RADIO_POWER_ON', 1),
                    ('RABI__RADIO_POWER_OFF', 1),
                    ('RABI__CAM_FPGA_RESET', 1),
                    ('RABI__CAM_FPGA_UNRESET', 1),
                    ('RABI__CAM_FPGA_POWER_ON', 1),
                    ('RABI__CAM_FPGA_POWER_OFF', 1),
                    ('RABI__ALL_MOTORS_POWER_ON', 1),
                    ('RABI__ALL_MOTORS_POWER_OFF', 1),
                    ('RABI__3V3_EN_RESET', 1),
                    ('RABI__3V3_EN_UNRESET', 1),
                    ('RABI__3V3_EN_POWER_ON', 1),
                    ('RABI__3V3_EN_POWER_OFF', 1),
                    ('RABI__24V_EN_RESET', 1),
                    ('RABI__24V_EN_UNRESET', 1),
                    ('RABI__24V_EN_POWER_ON', 1),
                    ('RABI__24V_EN_POWER_OFF', 1),
                    ('RABI__HDRM_DEPLOY_SIGNAL_POWER_OFF', 1),
                    ('RABI__FPGA_CAM_0_SELECT', 1),
                    ('RABI__FPGA_CAM_1_SELECT', 1),
                    ('RABI__BATTERY_CHARGE_START', 1),
                    ('RABI__BATTERY_CHARGE_STOP', 1),
                    ('RABI__RS422_UART_ENABLE', 1),
                    ('RABI__RS422_UART_DISABLE', 1),
                    ('RABI__AUTO_HEATER_CONTROLLER_ENABLE', 1),
                    ('RABI__AUTO_HEATER_CONTROLLER_DISABLE', 1),
                    ('RABI__HERCULES_WATCHDOG_ENABLE', 1),
                    ('RABI__HERCULES_WATCHDOG_DISABLE', 1),
                    ('RABI__BATTERIES_ENABLE', 1),
                    ('RABI__BATTERIES_DISABLE', 1),
                    ('RABI__HDRM_DEPLOY_SIGNAL_POWER_ON', 1),
                    ('RABI__HERCULES_WATCHDOG_RESET', 1)
                ])),
                total_padding=26
            )
        ),


        # FROM ADC:
        nid_nic_entry(
            TelemetryChannel, 0x20, 'Adc_LanderVoltageRaw', datatype=FswDataType.U8  # 7b
        ),
        nid_nic_entry(
            TelemetryChannel, 0x21, 'Adc_LanderVoltage', datatype=FswDataType.F64  # calc
        ),

        nid_nic_entry(
            TelemetryChannel, 0x22, 'Adc_BatteryChargingTempRaw', datatype=FswDataType.U16  # 9b
        ),
        nid_nic_entry(
            TelemetryChannel, 0x23, 'Adc_BatteryChargingTempKelvin', datatype=FswDataType.F64  # calc
        ),
        nid_nic_entry(
            TelemetryChannel, 0x24, 'Adc_BatteryChargingTempUncertaintyKelvin', datatype=FswDataType.F64  # calc
        ),

        nid_nic_entry(
            TelemetryChannel, 0x25, 'Adc_BatteryTempRaw', datatype=FswDataType.U16  # 12b
        ),
        nid_nic_entry(
            TelemetryChannel, 0x26, 'Adc_BatteryTempKelvin', datatype=FswDataType.F64  # calc
        ),
        nid_nic_entry(
            TelemetryChannel, 0x27, 'Adc_BatteryTempUncertaintyKelvin', datatype=FswDataType.F64  # calc
        ),

        nid_nic_entry(
            TelemetryChannel, 0x28, 'Adc_FullSystemVoltageRaw', datatype=FswDataType.U8  # 5b
        ),
        nid_nic_entry(
            TelemetryChannel, 0x29, 'Adc_FullSystemVoltage', datatype=FswDataType.F64  # calc
        ),

        nid_nic_entry(
            TelemetryChannel, 0x2A, 'Adc_FullSystemCurrentRaw', datatype=FswDataType.U16  # 9b
        ),
        nid_nic_entry(
            TelemetryChannel, 0x2B, 'Adc_FullSystemCurrent', datatype=FswDataType.F64  # calc
        ),

        nid_nic_entry(
            TelemetryChannel, 0x2C, 'Adc_SwitchedBatteryVoltageRaw', datatype=FswDataType.U16  # 9b
        ),
        nid_nic_entry(
            TelemetryChannel, 0x2D, 'Adc_SwitchedBatteryVoltage', datatype=FswDataType.F64  # calc
        ),

        nid_nic_entry(
            TelemetryChannel, 0x30, 'Adc_2V5VoltageRaw', datatype=FswDataType.U8  # 5b
        ),
        nid_nic_entry(
            TelemetryChannel, 0x31, 'Adc_2V5Voltage', datatype=FswDataType.F64  # calc
        ),

        nid_nic_entry(
            TelemetryChannel, 0x32, 'Adc_2V8VoltageRaw', datatype=FswDataType.U8  # 5b
        ),
        nid_nic_entry(
            TelemetryChannel, 0x33, 'Adc_2V8Voltage', datatype=FswDataType.F64  # calc
        ),

        nid_nic_entry(
            TelemetryChannel, 0x34, 'Adc_Vcc28VoltageRaw', datatype=FswDataType.U8  # 6b
        ),
        nid_nic_entry(
            TelemetryChannel, 0x35, 'Adc_Vcc28Voltage', datatype=FswDataType.F64  # calc
        ),

        nid_nic_entry(
            TelemetryChannel, 0x36, 'Adc_Vcc24VoltageRaw', datatype=FswDataType.U8  # 7b
        ),
        nid_nic_entry(
            TelemetryChannel, 0x37, 'Adc_Vcc24Voltage', datatype=FswDataType.F64  # calc
        ),


        # FROM HEATERS:
        nid_nic_entry(
            TelemetryChannel, 0x40, 'Heater_Kp', datatype=FswDataType.U16  # 16b
        ),

        nid_nic_entry(
            TelemetryChannel, 0x41, 'Heater_PwmLimit_DutyCycleCounter', datatype=FswDataType.U16  # 16b
        ),
        nid_nic_entry(
            TelemetryChannel, 0x42, 'Heater_PwmLimit_DutyCyclePercent', datatype=FswDataType.F64  # calc
        ),
        nid_nic_entry(
            TelemetryChannel, 0x43, 'Heater_EffectivePowerLimit', datatype=FswDataType.F64  # calc
        ),

        nid_nic_entry(
            TelemetryChannel, 0x44, 'Heater_SetpointValue', datatype=FswDataType.U16  # 16b
        ),
        nid_nic_entry(
            TelemetryChannel, 0x45, 'Heater_SetpointKelvin', datatype=FswDataType.F64  # calc
        ),

        nid_nic_entry(
            TelemetryChannel, 0x46, 'Heater_OnValue', datatype=FswDataType.U16  # 16b
        ),
        nid_nic_entry(
            TelemetryChannel, 0x47, 'Heater_OnTempKelvin', datatype=FswDataType.F64  # calc
        ),

        nid_nic_entry(
            TelemetryChannel, 0x48, 'Heater_OffValue', datatype=FswDataType.U16  # 16b
        ),
        nid_nic_entry(
            TelemetryChannel, 0x49, 'Heater_OffTempKelvin', datatype=FswDataType.F64  # calc
        ),

        nid_nic_entry(
            TelemetryChannel, 0x4A, 'Heater_ControlEnabled', datatype=FswDataType.ENUM,  # 1b
            enum=[
                EnumItem('DISABLED', 0x00),
                EnumItem('ENABLED', 0x01)
            ]
        ),
        nid_nic_entry(
            TelemetryChannel, 0x4B, 'Heater_IsHeating', datatype=FswDataType.ENUM,  # 1b
            enum=[
                EnumItem('NOT_HEATING', 0x00),
                EnumItem('HEATING', 0x01)
            ]
        ),

        nid_nic_entry(
            TelemetryChannel, 0x4C, 'Heater_DutyCyclePeriodCycles', datatype=FswDataType.U16  # 16b
        ),
        nid_nic_entry(
            TelemetryChannel, 0x4D, 'Heater_DutyCyclePeriodMs', datatype=FswDataType.F64  # calc
        ),

        nid_nic_entry(
            TelemetryChannel, 0x4E, 'Heater_DutyCycleCounter', datatype=FswDataType.U16  # 16b
        ),
        nid_nic_entry(
            TelemetryChannel, 0x4F, 'Heater_DutyCyclePercent', datatype=FswDataType.F64  # calc
        ),
        nid_nic_entry(
            TelemetryChannel, 0x50, 'Heater_EffectiveVoltage', datatype=FswDataType.F64  # calc
        ),
        nid_nic_entry(
            TelemetryChannel, 0x51, 'Heater_EffectivePower', datatype=FswDataType.F64  # calc
        ),


        # From I2C:
        nid_nic_entry(
            TelemetryChannel, 0x60, 'I2C_BatteryChargeRaw', datatype=FswDataType.U16  # 16b
        ),
        nid_nic_entry(
            TelemetryChannel, 0x61, 'I2C_BatteryChargeMah', datatype=FswDataType.F64  # calc
        ),

        nid_nic_entry(
            TelemetryChannel, 0x62, 'I2C_BatteryVoltageRaw', datatype=FswDataType.U16  # 16b
        ),
        nid_nic_entry(
            TelemetryChannel, 0x63, 'I2C_BatteryVoltage', datatype=FswDataType.F64  # calc
        ),

        nid_nic_entry(
            TelemetryChannel, 0x64, 'I2C_BatteryCurrentRaw', datatype=FswDataType.U16  # 16b
        ),
        nid_nic_entry(
            TelemetryChannel, 0x65, 'I2C_BatteryCurrent', datatype=FswDataType.F64  # calc
        ),

        nid_nic_entry(
            TelemetryChannel, 0x66, 'I2C_FuelGaugeTempRaw', datatype=FswDataType.U16  # 16b
        ),
        nid_nic_entry(
            TelemetryChannel, 0x67, 'I2C_FuelGaugeTempKelvin', datatype=FswDataType.F64  # calc
        ),

        nid_nic_entry(
            TelemetryChannel, 0x68, 'I2C_BatteryChargeTelemRaw', datatype=FswDataType.U16  # 16b
        ),
        nid_nic_entry(
            TelemetryChannel, 0x69, 'I2C_BatteryChargeTelemMah', datatype=FswDataType.F64  # calc
        ),

        nid_nic_entry(
            TelemetryChannel, 0x6A, 'I2C_BatteryCurrentTelemRaw', datatype=FswDataType.U8  # 8b
        ),
        nid_nic_entry(
            TelemetryChannel, 0x6B, 'I2C_BatteryCurrentTelemAmps', datatype=FswDataType.F64  # calc
        ),
    ])
)

watchdog_command_response: Module = Module(
    name="WatchdogCommandResponse",
    ID=0xFD00,
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
