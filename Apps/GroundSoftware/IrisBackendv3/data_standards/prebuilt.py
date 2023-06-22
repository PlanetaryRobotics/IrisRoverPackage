"""
Special prebuilt data standards which exist outside of the FPrime XML (e.g. for
watchdog heartbeat)

@author: Connor W. Colombo (CMU)
@last-updated: 06/01/2023
"""
from typing import Any, Final, Type, Union, List, Tuple, TypeVar, TypeAlias
from collections import OrderedDict

from IrisBackendv3.utils.nameiddict import NameIdDict

# TODO: Import these and use in `_packet_classes_to_events`. Presently results in circular import and don't have time to resolve rn.
# from IrisBackendv3.codec import packet
# from IrisBackendv3.codec.packet_classes.gds_packet_event_mixin import (
#     GdsPacketEventMixin,
#     MSG_ARGUMENT_NAME as GDS_PACKET_EVENT_MSG_ARG_NAME
# )

from .data_standards import DataStandards
from .module import Module, Command, Event, Argument, TelemetryChannel, EnumItem, BitfieldStruct
from .fsw_data_type import FswDataType
from .logs import logger

T = TypeVar('T')


_NID_NIC_ENTRY_T: TypeAlias = Tuple[Tuple[int, str], T]


def nid_nic_entry(t: Type[Any], ID: int, name: str, *args, **kwargs) -> _NID_NIC_ENTRY_T:
    """
    Makes a tuple which can be used as a name-id-containing `NameIdDict` entry
    where the `name` and `ID` are also parameters of the entry value in addition
    to being the keys in the NameIdDict (e.g. in `TelemetryChannels` or
    `Commands`).
    This convenience function prevents the `name` and `ID` from needing to be
    entered twice when manually instantiating a NameIdDict with
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
    # Add all modules to standards:
    standards.add_new_modules(modules)


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
            name='HeaterSetpointKelvin', ID=0x52, datatype=FswDataType.F64
        ),
        (0x53, 'HeaterWindow'): TelemetryChannel(
            name='HeaterWindow', ID=0x53, datatype=FswDataType.U16
        ),
        (0x54, 'HeaterWindowKelvin'): TelemetryChannel(
            name='HeaterWindowKelvin', ID=0x54, datatype=FswDataType.F64
        ),
        (0x55, 'HeaterPwmLimit'): TelemetryChannel(
            name='HeaterPwmLimit', ID=0x55, datatype=FswDataType.U16
        ),
        (0x56, 'WatchdogMode'): TelemetryChannel(
            name='WatchdogMode', ID=0x56, datatype=FswDataType.ENUM,
            enum=[
                EnumItem(
                    'INIT', 0, comment="The state that the state machine is in during initialization."),
                EnumItem('ENTERING_KEEP_ALIVE', 7,
                         comment="Performing all non-instant tasks to set up the appropriate system state for KEEP_ALIVE"),
                EnumItem(
                    'KEEP_ALIVE', 8, comment="In keep alive mode, used in transit to use little power but stay in communication"),
                EnumItem('ENTERING_SERVICE', 3,
                         comment="Performing all non-instant tasks to set up the appropriate system state for SERVICE"),
                EnumItem(
                    'SERVICE', 4, comment="In diagnostic service mode, which is also a precondition of entering MISSION"),
                EnumItem('ENTERING_MISSION', 15,
                         comment=" Performing all non-instant tasks to set up the appropriate system state for MISSION."),
                EnumItem(
                    'MISSION', 16, comment="In the fully-powered on mode to perform the mission."),
                EnumItem('ENTERING_STASIS', 31,
                         comment=" Performing all non-instant tasks to set up the appropriate system state for STASIS"),
                EnumItem('STASIS', 32,
                         comment="In mission, enter low-power stasis.")
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
            enum=[  # (cstat1 << 1) | cstat2
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
                    'RS_INIT', 0, comment="The state that the state machine is in during initialization."),
                EnumItem('RS_ENTERING_KEEP_ALIVE', 7,
                         comment="Performing all non-instant tasks to set up the appropriate system state for KEEP_ALIVE"),
                EnumItem(
                    'RS_KEEP_ALIVE', 8, comment="In keep alive mode, used in transit to use little power but stay in communication"),
                EnumItem('RS_ENTERING_SERVICE', 3,
                         comment="Performing all non-instant tasks to set up the appropriate system state for SERVICE"),
                EnumItem(
                    'RS_SERVICE', 4, comment="In diagnostic service mode, which is also a precondition of entering MISSION"),
                EnumItem('RS_ENTERING_MISSION', 15,
                         comment=" Performing all non-instant tasks to set up the appropriate system state for MISSION."),
                EnumItem(
                    'RS_MISSION', 16, comment="In the fully-powered on mode to perform the mission."),
                EnumItem('RS_ENTERING_STASIS', 31,
                         comment=" Performing all non-instant tasks to set up the appropriate system state for STASIS"),
                EnumItem('RS_STASIS', 32,
                         comment="In mission, enter low-power stasis.")
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
                fields=OrderedDict(([
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
                fields=OrderedDict(([
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
                fields=OrderedDict(([
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
                    ('RABI__V_SYS_ALL_OFF__RESET', 1),
                    ('RABI__V_SYS_ALL_ON__UNRESET', 1),
                    ('RABI__V_SYS_ALL_POWER_ON', 1),
                    ('RABI__V_SYS_ALL_POWER_OFF', 1),
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


# FPrime Enum of all BGAPI Error Codes (used for several Radio packets):
WF121_BGAPI_ERROR_CODES: Final[List[EnumItem]] = [
    EnumItem("NO_ERROR", 0x0000, comment="No error exists."),
    EnumItem("INVALID_PARAMETER", 0x0180,
             comment="This error code indicates that a command contained an invalid parameter"),
    EnumItem("DEVICE_WRONG_STATE", 0x0181,
             comment="This error code indicates that the device is in wrong state to accept commands."),
    EnumItem("OUT_OF_MEMORY", 0x0182,
             comment="This error indicates that the device has run out of memory."),
    EnumItem("FEATURE_NOT_IMPLEMENTED", 0x0183,
             comment="This error indicates that the feature in question has not been implemented."),
    EnumItem("COMMAND_NOT_RECOGNIZED", 0x0184,
             comment="This error indicates that the issued command was not recognized."),
    EnumItem("TIMEOUT", 0x0185,
             comment="This error indicates that a command or procedure failed due to timeout. This error code is generated e.g. if you send an incomplete command to the Wi-Fi module - after the timeout of 1 second this error code is sent to the host transported by the event .endpoint_syntax_errorThis error code is generated also e.g. when the maximum number of retry attempts (10) to try to connect to a wireless network have been executed. A typical example of such a case might be when the issued password is invalid, in which case the error code is transported by the event .wifi_evt_sme_connect_failed"),
    EnumItem("UNSPECIFIED_ERROR", 0x0186,
             comment="This error code is generated when an unspecified error is detected."),
    EnumItem("HARDWARE_FAILURE", 0x0187,
             comment="This error code is generated when a hardware failure is detected."),
    EnumItem("INTERNAL_BUFFER_FULL", 0x0188,
             comment="This error code is generated the a command was not accepted due to full internal buffers."),
    EnumItem("DISCONNECTED", 0x0189,
             comment="This error code is generated when a command or procedure has failed due to disconnection."),
    EnumItem("TOO_MANY_REQUEST", 0x018a,
             comment="This error code is generated when there are too many simultaneous requests."),
    EnumItem("ACCESS_POINT_NOT_IN_SCANLIST", 0x018b,
             comment="This error code is generated when the defined Access Point is not found from the scanlist."),
    EnumItem("INVALID_PASSWORD", 0x018c,
             comment="This error code is generated in the following cases:1) you try to connect to a secured network without setting the password or the password is too short2) you try to start a secured AP without setting the password or the password is too short3) you try to set a 64-character PSK with non-hex characters4) you try to set an invalid WEP key (invalid characters or invalid length)Notice that WPA does not contain any standard way for the Access Point to communicate to the station that the password is invalid. The Access Point just disconnects the client during authentication if the password is found invalid. Some stations take an educated guess that this probably means the password is incorrect. WF121 simply retries the authentication until it exceeds the maximum amount of retries (10) which then causes the 0x0185 Timeout Error."),
    EnumItem("AUTHENTICATION_FAILURE", 0x018d,
             comment="This error code is generated when the WPA/WPA2 authentication has failed."),
    EnumItem("OVERFLOW", 0x018e,
             comment="This error code is generated when an overflow has been detected."),
    EnumItem("MULTIPLE_PBC_SESSIONS", 0x018f,
             comment="This error code is generated when multiple PBC (Push Button Configuration) sessions have been detected."),
    EnumItem("ETHERNET_NOT_CONNECTED", 0x0190,
             comment="This error code is generated when the Ethernet cable is not connected"),
    EnumItem("ETHERNET_ROUTE_NOT_SET", 0x0191,
             comment="This error code is generated if the Ethernet route is not set"),
    EnumItem("WRONG_OPERATING_MODE", 0x0192,
             comment="This error code is generated if the operating mode is wrong for the issued command"),
    EnumItem("WIFI_NOT_FOUND", 0x0193,
             comment="This error code is generated if the requested resource was not found."),
    EnumItem("WIFI_NOT_ALREADY_EXIST", 0x0194,
             comment="This error is generated if the requested resource already exists."),
    EnumItem("WIFI_INVALID_CONFIGURATION", 0x0195,
             comment="This error code is generated if the current configuration is invalid."),
    EnumItem("WIFI_ACCESS_POINT_LOST", 0x0196,
             comment="This error code is generated if the connection to an Access Point is lost."),
    EnumItem("PS_STORE_FULL", 0x0301,
             comment="This error is generated if the Flash which is reserved for the PS (Persistent Store) is full."),
    EnumItem("PS_KEY_NOT_FOUND", 0x0302,
             comment="This error is generated if the PS key (Persistent Store key) was not found."),
    EnumItem("I2C_WRITE_ALREADY_IN_PROGRESS", 0x0303,
             comment="This error is generated if I2C transmission was initiated while a transmission was already in progress."),
    EnumItem("I2C_ACK_MISSING", 0x0304,
             comment="This error is generated if an acknowledgement for I2C was not received."),
    EnumItem("FLASH_WRITE_FAILED", 0x0308,
             comment="This error code is generated if writing to Flash failed."),
    EnumItem("FILE_NOT_OPENED", 0x0305,
             comment="This error code is generated if an access was attempted to an unopened file."),
    EnumItem("FILE_NOT_FOUND", 0x0306,
             comment="This error code is generated if the requested file was not found in the SD card."),
    EnumItem("DISK_ERROR", 0x0307,
             comment="This error code is generated if an SD card error was detected of if the SD card is full"),
    EnumItem("TCP_IP_SUCCESS", 0x0200,
             comment="This code indicates that no error was detected."),
    EnumItem("TCP_IP_OUT_OF_MEMORY", 0x0201,
             comment="This error code is generated when the system has run out of memory."),
    EnumItem("TCP_IP_BUFFER_ERROR", 0x0202,
             comment="This error code is generated when handling of buffers has failed"),
    EnumItem("TCP_IP_TIMEOUT", 0x0203,
             comment="This error code is generated when a timeout has been detected."),
    EnumItem("TCP_IP_ROUTING", 0x0204,
             comment="This error code is generated when a route could not be found."),
    EnumItem("TCP_IP_IN_PROGRESS", 0x0205,
             comment="This error code is generated when an operation is in progress"),
    EnumItem("TCP_IP_ILLEGAL_VALUE", 0x0206,
             comment="This error code is generated when the issued value is deemed illegal."),
    EnumItem("TCP_IP_WOULD_BLOCK", 0x0207,
             comment="This error code is generated when an operation blocks."),
    EnumItem("TCP_IP_ADDRESS_IN_USE", 0x0208,
             comment="This error code is generated when the issued address is already in use"),
    EnumItem("TCP_IP_ALREADY_CONNECTED", 0x0209,
             comment="This error code is generated when the Wi-Fi module is already connected"),
    EnumItem("TCP_IP_CONNECTION_ABORTED", 0x020a,
             comment="This error code is generated when a connection is aborted."),
    EnumItem("TCP_IP_CONNECTION_RESET", 0x020b,
             comment="This error code is generated when a connection has been reset"),
    EnumItem("TCP_IP_CONNECTION_CLOSED", 0x020c,
             comment="This error code is generated when a connection has been closed."),
    EnumItem("TCP_IP_NOT_CONNECTED", 0x020d,
             comment="This error code is generated when the Wi-Fi module is not connected."),
    EnumItem("TCP_IP_ILLEGAL_ARGUMENT", 0x020e,
             comment="This error code is generated if an illegal argument was issued."),
    EnumItem("TCP_IP_INTERFACE_LEVEL_ERROR", 0x020f,
             comment="This error code is generated if an interface error was detected"),
    EnumItem("TCP_IP_UNKNOWN_HOST", 0x0280,
             comment="This error is generated if an unknown host is detected."),
    EnumItem("SERVICE_NOT_RUNNING", 0x0210,
             comment="This error code is generated if the specified service is not running."),
    EnumItem("SERVICE_RUNNING", 0x0211,
             comment="This error code is generated if the specified service is already running"),
    EnumItem("HOSTNAME_NOT_SET", 0x0212,
             comment="This error code is generated if the hostname has not been set"),
    EnumItem("HOSTNAME_CONFLICT", 0x0213,
             comment="This error code is generated if a hostname conflict was detected."),
    EnumItem('OTHER__UNEXPECTED', 0xFFFF,
             comment="An unknown/unexpected BGAPI reason was given for this syntax_error. Perhaps the error code bytes were corrupted during downlink. See the raw bytes of the packet for the actual error code bytes.")
]

# Enum of all commands that can be sent from the Radio to Ground (used in a number of callbacks):
RADIO_GROUND_COMMAND_IDS: Final[List[EnumItem]] = [
    EnumItem("ECHO", 0x00, comment="Echo back the given bytes."),
    EnumItem("RESET_RADIO", 0xBB, comment="Reset the Radio."),
    EnumItem("ENTER_STASIS", 0xE5, comment="Enter stasis mode."),
    EnumItem("EXIT_STASIS", 0x5E, comment="Exit stasis mode."),
    EnumItem("WRITE_UART", 0xDB,
             comment="Write the given message to the Hercules UART."),
    EnumItem("HERCULES_DM", 0xDD,
             comment="Send the given data to Hercules as a Radio Direct Message."),
    EnumItem("CORRUPTED", 0xFF,
             comment="Given ID doesn't match any known IDs. Likely corrupted.")
]

# Enum of all Radio state abbreviations (used in a couple places):
RADIO_STATE_ABBRS_ENUM: Final[List[EnumItem]] = [
    EnumItem('BOOT', 0x00, comment="BOOT. Radio just booted."),
    EnumItem(
        'INIT', 0x01, comment="INITIALIZED. Trying to turn radio antenna on."),
    EnumItem(
        'WFON', 0x02, comment="WIFI_ON. Radio antenna is on. Trying to connect to the network."),
    EnumItem(
        'CONN', 0x03, comment="CONNECTED. Connected to WiFi network. Attempting to establish UDP connection."),
    EnumItem(
        'UDPC', 0x04, comment="UDP_CONNECTED. UDP connection established over WiFi. Everything is good to go."),
    EnumItem(
        'CRPT', 0xFF, comment="The abbreviation received didn't match any known Radio State Abbreviation. See raw data for actual bytes received.")
]

# Data exchanged directly between the Radio's internal MCU and Ground:
radio_ground: Module = Module(
    name="RadioGround",
    ID=0xEF00,
    commands=NameIdDict([
        nid_nic_entry(
            Command, 0x00, name='RadioGround_Echo', mnemonic='Echo', args=[
                Argument('text_to_echo', FswDataType.VARSTRING_255)
            ]
        ),
        nid_nic_entry(
            Command, 0xBB, name='RadioGround_ResetRadio', mnemonic='ResetRadio', args=[
                # Requires 'RESET' as a string to confirm this action:
                Argument('confirm_by_typing_RESET', FswDataType.STRING5)
            ]
        ),
        nid_nic_entry(
            Command, 0xE5, name='RadioGround_EnterStasis', mnemonic='EnterStasis', args=[
                # Requires 'STASIS' as a string to confirm this action:
                Argument('confirm_by_typing_STASIS', FswDataType.STRING6)
            ]
        ),
        nid_nic_entry(
            Command, 0x5E, name='RadioGround_ExitStasis', mnemonic='ExitStasis', args=[
                # Requires 'STASIS' as a string to confirm this action:
                Argument('confirm_by_typing_STASIS', FswDataType.STRING6)
            ]
        ),
        nid_nic_entry(
            Command, 0xDB, name='RadioGround_WriteUart', mnemonic='WriteUart', args=[
                Argument('text_to_echo', FswDataType.VARSTRING_255)
            ]
        ),
        nid_nic_entry(
            Command, 0xDD, name='RadioGround_HerculesDm', mnemonic='HerculesDm', args=[
                Argument('text_to_echo', FswDataType.VARSTRING_255)
            ]
        ),
    ]),
    events=NameIdDict([
        # Special one-off events:
        nid_nic_entry(
            # In addition to telem, emit a user-friendly event for each Radio
            # Heartbeat with the most important data:
            Event, 0x00, 'Heartbeat', severity_str='ACTIVITY_LO',
            format_string="Radio Heartbeat: %s with %d RSSI at %s.",
            args=[
                Argument('state_abbr', datatype=FswDataType.ENUM,
                         enum=[ei for ei in RADIO_STATE_ABBRS_ENUM]),
                Argument('rssi', datatype=FswDataType.I16),
                # HHh:MMm:SSs
                Argument('uptime_str', datatype=FswDataType.VARSTRING_255)
            ]
        ),
        nid_nic_entry(
            # Message sent when the Radio (re)connects to WiFi:
            Event, 0x01, 'Connected', severity_str='ACTIVITY_HI',
            format_string="Radio (Re)Connected: %s",
            args=[
                Argument("message", datatype=FswDataType.STRING39)
            ]
        ),
        nid_nic_entry(
            # Radio was told to echo back a given string. Technically the radio
            # will echo any number of characters you give it but we fix the
            # length to 24 chars to appease FPrime.
            Event, 0x02, 'Echo', severity_str='COMMAND',
            format_string="Radio Echoed: '%s'.",
            args=[
                Argument("echoed_text", datatype=FswDataType.VARSTRING_255)
            ]
        ),

        # Command response events:
        nid_nic_entry(
            Event, 0x10, 'GotCommand', severity_str='COMMAND',
            format_string="Radio got command '%s' with data: `%s`.",
            args=[
                Argument(
                    "command", datatype=FswDataType.ENUM,
                    enum=[ei for ei in RADIO_GROUND_COMMAND_IDS]
                ),
                Argument("string_of_command_data",
                         datatype=FswDataType.VARSTRING_255)
            ]
        ),
        nid_nic_entry(
            Event, 0x11, 'DidCommand', severity_str='COMMAND',
            format_string="Radio successfully executed command '%s' with data: `%s`.",
            args=[
                Argument(
                    "command", datatype=FswDataType.ENUM,
                    enum=[ei for ei in RADIO_GROUND_COMMAND_IDS]
                ),
                Argument("string_of_command_data",
                         datatype=FswDataType.VARSTRING_255)
            ]
        ),
        nid_nic_entry(
            Event, 0x1F, 'BadCommand', severity_str='COMMAND',
            format_string="Radio failed to execute a command '%s' with data: `%s`.",
            args=[
                Argument(
                    "command", datatype=FswDataType.ENUM,
                    enum=[ei for ei in RADIO_GROUND_COMMAND_IDS]
                ),
                Argument("string_of_command_data",
                         datatype=FswDataType.VARSTRING_255)
            ]
        ),

        # Anomaly handling events:
        nid_nic_entry(
            # Radio received a bad/garbled command from Hercules. Only really a problem if you get a bunch of these.
            Event, 0xF0, 'BadHerculesPacket', severity_str='WARNING_LO',
            format_string="Radio received a bad packet from Hercules. Issue: %s.",
            args=[
                Argument(
                    "issue", datatype=FswDataType.ENUM,
                    enum=[ei for ei in WF121_BGAPI_ERROR_CODES]
                )
            ]
        ),
        nid_nic_entry(
            Event, 0xF1, 'BadEndpointData', severity_str='WARNING_LO',
            format_string="One of the Radio's routing endpoints experienced an issue. Endpoint: %d, Issue: %s.",
            args=[
                Argument("endpoint", datatype=FswDataType.U8),
                Argument(
                    "issue", datatype=FswDataType.ENUM,
                    enum=[ei for ei in WF121_BGAPI_ERROR_CODES]
                )
            ]
        ),
        nid_nic_entry(
            # Radio received a UDP packet with incorrect metadata:
            Event, 0xF2, 'BadUdp', severity_str='WARNING_HI',
            format_string="Radio received %s in UDP packet with IP=%s, Port=%d.",
            args=[
                Argument(
                    "issue", datatype=FswDataType.ENUM,
                    enum=[
                        EnumItem('BAD_IP', 0b01),
                        EnumItem('BAD_PORT', 0b10),
                        EnumItem('BAD_IP_AND_PORT', 0b11),
                        EnumItem('NOTHING__THIS_SHOULD_NOT_HAPPEN', 0b00,
                                 comment="We should never get a BAD UDP message with 'NOTHING' as the issue. If we get this, there must have been a bitflip in the message.")
                    ]
                ),
                Argument("ip", datatype=FswDataType.STRING15),
                Argument("port", datatype=FswDataType.U16)
            ]
        ),
        nid_nic_entry(
            Event, 0xF3, 'CriticalReset', severity_str='FATAL',
            format_string="Radio had to reset itself due to a critical (unrecoverable) issue it discovered during self-monitoring: %s.",
            args=[
                Argument(
                    "issue", datatype=FswDataType.ENUM,
                    enum=[ei for ei in WF121_BGAPI_ERROR_CODES]
                )
            ]
        ),
        nid_nic_entry(  # This comes from evt_system_sw_exception.
            Event, 0xF4, 'CriticalResetDueToSoftwareException', severity_str='FATAL',
            format_string="Radio had to reset itself due to a critical software exception. The exception occurred at memory address: 0x%04X and has numerical type: %d.",
            args=[
                Argument("memory_address", datatype=FswDataType.U32),
                # there is nothing in the API docs or elsewhere about what values to expect for this exception type:
                Argument("exception_type", datatype=FswDataType.U8)
            ]
        )
    ]),
    telemetry=NameIdDict([
        # From Radio-Ground Heartbeat:
        nid_nic_entry(
            TelemetryChannel, 0x00, 'Rssi', datatype=FswDataType.I16
        ),
        nid_nic_entry(
            TelemetryChannel, 0x01, 'StateAbbr', datatype=FswDataType.ENUM,
            enum=[ei for ei in RADIO_STATE_ABBRS_ENUM]
        ),
        nid_nic_entry(
            # How long the radio has been up (since last reset), in timer ticks
            TelemetryChannel, 0x02, 'UptimeTicks', datatype=FswDataType.U32
        ),
        nid_nic_entry(
            # How long the radio has been up (since last reset), in milliseconds
            TelemetryChannel, 0x03, 'UptimeMs', datatype=FswDataType.U32  # calc
        ),
        nid_nic_entry(
            # How many UDP packets the Radio has received
            TelemetryChannel, 0x04, 'UdpRxPacketCount', datatype=FswDataType.U32
        ),
        nid_nic_entry(
            # How many times Hercules has given the Radio garbage BGAPI packets:
            TelemetryChannel, 0x05, 'BadHercPacketCount', datatype=FswDataType.U32
        ),
        nid_nic_entry(
            # Current value of the counting semaphore between Hercules and the Radio (should always be 0 if we're getting it):
            TelemetryChannel, 0x06, 'HerculesUdpInterlockCountingSemaphore', datatype=FswDataType.U8
        )
    ])
)


# Data we're supplied from Peregrine:
peregrine: Module = Module(
    name="Peregrine",
    ID=0xDF00,
    commands=NameIdDict(),
    events=NameIdDict(),
    telemetry=NameIdDict([
        nid_nic_entry(
            TelemetryChannel, 0x00, 'IrisOperationalEnabledFet', datatype=FswDataType.ENUM,
            enum=[
                EnumItem('DISABLED', 0x00,
                         comment="Iris Operational Power Circuit Disabled."
                         ),
                EnumItem('ENABLED', 0x01,
                         comment="Iris Operational Power Circuit Enabled."
                         )
            ]
        ),
        nid_nic_entry(
            TelemetryChannel, 0x01, 'IrisReleaseEnabledFet', datatype=FswDataType.ENUM,
            enum=[
                EnumItem('DISABLED', 0x00,
                         comment="Iris Release Power Circuit Disabled."
                         ),
                EnumItem('ENABLED', 0x01,
                         comment="Iris Release Power Circuit Enabled."
                         )
            ]
        ),
        nid_nic_entry(  # calc
            TelemetryChannel, 0xF2, 'IrisOperationalAvgPower', datatype=FswDataType.F64
        ),
        nid_nic_entry(
            TelemetryChannel, 0x02, 'IrisOperationalAvgCurrent', datatype=FswDataType.F64
        ),
        nid_nic_entry(
            TelemetryChannel, 0x03, 'IrisReleaseAvgCurrent', datatype=FswDataType.F64
        ),
        nid_nic_entry(
            TelemetryChannel, 0x04, 'IrisOperationalMaxCurrent', datatype=FswDataType.F64
        ),
        nid_nic_entry(
            TelemetryChannel, 0x05, 'IrisReleaseMaxCurrent', datatype=FswDataType.F64
        ),
        nid_nic_entry(
            TelemetryChannel, 0x06, 'SlipServiceStatus', datatype=FswDataType.ENUM,
            enum=[
                EnumItem('ACTIVE', 0x00,
                         comment='Service is currently active.'),
                EnumItem('RELOADING', 0x01,
                         comment='Service is currently reloading.'),
                EnumItem('INACTIVE', 0x02,
                         comment='Service is currently inactive.'),
                EnumItem('FAILED', 0x03,
                         comment='Service is currently failed.'),
                EnumItem('ACTIVATING', 0x04,
                         comment='Service is currently activating.'),
                EnumItem('DEACTIVATING', 0x05,
                         comment='Service is currently deactivating.')
            ]
        ),
        nid_nic_entry(
            TelemetryChannel, 0x07, 'WifiServiceStatus', datatype=FswDataType.ENUM,
            enum=[
                EnumItem('ACTIVE', 0x00,
                         comment='Service is currently active.'),
                EnumItem('RELOADING', 0x01,
                         comment='Service is currently reloading.'),
                EnumItem('INACTIVE', 0x02,
                         comment='Service is currently inactive.'),
                EnumItem('FAILED', 0x03,
                         comment='Service is currently failed.'),
                EnumItem('ACTIVATING', 0x04,
                         comment='Service is currently activating.'),
                EnumItem('DEACTIVATING', 0x05,
                         comment='Service is currently deactivating.')
            ]
        ),
        nid_nic_entry(
            TelemetryChannel, 0x08, 'DeckD1TempKelvin', datatype=FswDataType.F64
        ),
        nid_nic_entry(
            TelemetryChannel, 0x09, 'DeckD2TempKelvin', datatype=FswDataType.F64
        ),
        nid_nic_entry(
            TelemetryChannel, 0x0A, 'DeckD3TempKelvin', datatype=FswDataType.F64
        ),
    ])
)


def _packet_classes_to_events(
    specs: List[Tuple[str, Event.SeverityLevel]]
) -> List[_NID_NIC_ENTRY_T]:
    """
    Creates a list of name-id-containing NameIdDict entries specifying 
    events based on the given packet classes.
    See `gds_packet_event_mixin` for more details about why this exists.
    """
    return [
        nid_nic_entry(
            Event, i, c, severity_str=severity.name,
            format_string="%s",
            args=[
                Argument(
                    'msg',
                    datatype=FswDataType.VARSTRING_10K
                )
            ]
        )
        for i, (c, severity) in enumerate(specs)
    ]


# Registry of Packets which, if we receive them, represent an event:
gds_packets: Module = Module(
    name="GdsPackets",
    ID=0xCF00,
    commands=NameIdDict(),
    events=NameIdDict(
        # NOTE: To ensure backwards compatibility, only append to this list
        # and never reorder or remove (deprecate instead of removing):
        # Severity levels can be changed freely.
        _packet_classes_to_events([
            ('RadioBgApiPacket', Event.SeverityLevel.ACTIVITY_LO),
            ('RadioDirectMessagePacket', Event.SeverityLevel.ACTIVITY_HI),
            ('RadioUartBytePacket', Event.SeverityLevel.DIAGNOSTIC),
            ('UnsupportedPacket', Event.SeverityLevel.DIAGNOSTIC),
            ('WatchdogCommandResponsePacket', Event.SeverityLevel.COMMAND),
            ('WatchdogDebugPacket', Event.SeverityLevel.ACTIVITY_LO),
            ('WatchdogDebugImportantPacket', Event.SeverityLevel.ACTIVITY_HI),
            ('WatchdogHelloPacket', Event.SeverityLevel.COMMAND),
            ('WatchdogRadioDebugPacket', Event.SeverityLevel.ACTIVITY_HI),
            ('WatchdogResetSpecificAckPacket', Event.SeverityLevel.COMMAND),
            ('RadioDownlinkFlushPacket', Event.SeverityLevel.DIAGNOSTIC),
            ('HerculesRadioUplinkAckPacket', Event.SeverityLevel.ACTIVITY_LO)
        ])
    ),
    telemetry=NameIdDict()
)


# List of all special pre-built modules:
ALL_PREBUILT_MODULES: Final[List[Module]] = [
    watchdog_detailed_status_heartbeat,
    watchdog_heartbeat_tvac,
    watchdog_heartbeat,
    watchdog_command_response,
    radio_ground,
    peregrine,
    gds_packets
]
