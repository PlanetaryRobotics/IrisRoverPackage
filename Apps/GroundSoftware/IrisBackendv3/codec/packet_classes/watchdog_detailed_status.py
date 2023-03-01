"""
Implementation of Watchdog Detailed Status Packet and its Interface.
This is a maximally detailed status packet that contains status info for every
sensor and interface the Watchdog has access to. Occassionally used as a
detailed replacement for Heartbeat.

@author: Connor W. Colombo (CMU)
@last-updated: 12/31/2022
"""
from __future__ import annotations  # Activate postponed annotations (for using classes as return type in their own methods)

from .packet import CT
from .custom_payload import CustomPayloadPacket, CPCT

from typing import List, Any, Optional, ClassVar, cast, Union, Type
from collections import OrderedDict

import numpy as np  # type: ignore
from scapy.utils import hexstr  # type: ignore
from pandas import DataFrame  # type: ignore

from ..payload_collection import EnhancedPayloadCollection

from ..settings import ENDIANNESS_CODE
from ..exceptions import PacketDecodingException

from IrisBackendv3.utils.basic import flip_all_bits_in_bytes
from IrisBackendv3.data_standards.module import Module


class WatchdogDetailedStatusPacketInterface(CustomPayloadPacket[CT, CPCT]):
    # Name of the corresponding prebuilt `Module` used for mapping this packet's
    # data to telemetry streams:
    PREBUILT_MODULE_NAME: ClassVar[str] = 'WatchdogDetailedStatus'

    # Empty __slots__ allows super's __slots__ to not turn into __dict__:
    __slots__: List[str] = []

    class CustomPayload():
        """
        Core custom WatchdogDetailedStatus payload.
        Members must have same names as corresponding telemetry channels in the
        `WatchdogDetailedStatus` prebuilt module.
        Note: an error will get thrown by `WatchdogDetailedStatusPacket.__init__`
        when building `payloads` from the `custom_payload` if all the channels
        in the `WatchdogDetailedStatus` prebuilt module don't have a corresponding
        attr in this `CustomPayload`.

        *NOTE:* For this to work effectively, all fields, including computed
        properties, must match their names from the prebuilt module
        AND the order of the args in `__init__` must match the order of the
        bytes in the packet. [SEE THE NOTE IN `CustomPayloadPacket` FOR MORE
        DETAILS]
        """
        #! TODO: Consider moving these (and even V divider values etc) into some common GSW "SYSTEM PROPERTIES" struct somewhere
        # TODO: 5k table taken from old Avionics conversion sheet. not yet checked/verified. (check it)
        BATT_5K_THERMISTOR_LOOKUP_TABLE = {  # for 5k thermistor: https://www.tdk-electronics.tdk.com/inf/50/db/ntc/NTC_Mini_sensors_S863.pdf
            'degC': np.asarray([-55, -50, -45, -40, -35, -30, -25, -20, -15, -10, -5, 0, 5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 65, 70, 75, 80, 85, 90, 95, 100, 105, 110, 115, 120, 125, 130, 135, 140, 145, 150, 155]),
            'adc': np.asarray([4012, 3977, 3929, 3866, 3784, 3680, 3551, 3395, 3214, 3008, 2781, 2540, 2291, 2042, 1801, 1574, 1365, 1176, 1008, 861, 734, 625, 532, 453, 386, 329, 282, 242, 208, 179, 155, 134, 116, 102, 89, 78, 68, 60, 53, 47, 42, 37, 33])
        }
        BATT_CHRG_10K_THERMISTOR_LOOKUP_TABLE = {  # for 10k thermistor (NTC10k_B57863S0103F040)
            'degC': np.asarray([-55, -50, -45, -40, -35, -30, -25, -20, -15, -10, -5, 0, 5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 65, 70, 75, 80, 85, 90, 95, 100, 105, 110, 115, 120, 125, 130, 135, 140, 145, 150, 155]),
            'RTH_R25': np.asarray([96.3, 67.01, 47.17, 33.65, 24.26, 17.7, 13.04, 9.707, 7.293, 5.533, 4.232, 3.265, 2.539, 1.99, 1.571, 1.249, 1.0000, 0.8057, 0.6531, 0.5327, 0.4369, 0.3603, 0.2986, 0.2488, 0.2083, 0.1752, 0.1481, 0.1258, 0.1072, 0.09177, 0.07885, 0.068, 0.05886, 0.05112, 0.04454, 0.03893, 0.03417, 0.03009, 0.02654, 0.02348, 0.02083, 0.01853, 0.01653])
        }
        V_LANDER_MAX = 1.10*28.0  # Maximum possible lander voltage (allowed)
        V_HEATER_NOM = 28.0  # NOMINAL Heater Voltage
        R_HEATER = 628.245  # Heater resistance (ohms)

        # Correction factor for Vcc28 ADC conversion on FM1 in the cleanroom
        # during RC6 at 28.0V input:
        # upper 6bits = +-64ADC = +-0.567V
        # At 28.0V, an ADC reading of 3158.6777 would be expected. Since only
        # upper 6b are being sent, this needs to be rounded to nearest 64: 3136.
        VCC28_ADC_FM1_CORRECTION_FACTOR = 3136 / 2944
        # Correction factor for VL ADC conversion on FM1 in the cleanroom
        # during RC6 at 28.0V input:
        # upper 7bits = +-32ADC = +-0.248V
        # At 28.0V, an ADC reading of 3611.5347 would be expected. Since only
        # upper 7b are being sent, this needs to be rounded to nearest 32: 3616.
        VL_ADC_FM1_CORRECTION_FACTOR = 3616 / 3104

        # Correction factor for VSA ADC conversion on FM1 in the cleanroom
        # during RC6 at 28.0V input:
        # upper 5bits = +-128ADC = +-0.992V
        # At 28.0V, an ADC reading of 3611.5347 would be expected. Since only
        # upper 5b are being sent, this needs to be rounded to nearest 128: 3584.
        # yes, 2944 was observed here too (large jumps b/c so few bits)
        VSA_ADC_FM1_CORRECTION_FACTOR = 3584 / 2944

        # Correction factor for VBS ADC conversion on FM1 in the cleanroom
        # during RC5 at 23.80V VBS:
        # upper 9bits = +-8ADC = +-0.054V
        # At 23.80V, an ADC reading of 3558.5824 would be expected. Since only
        # upper 9b are being sent, this needs to be rounded to nearest 8: 3560.
        VBS_ADC_FM1_CORRECTION_FACTOR = 3560 / 2928

        __slots__: List[str] = [
            '_Io_ChargingStatus1',
            '_Io_ChargingStatus2',
            '_Io_BatteryConnectionStatus',
            '_Io_BatteryLatchStatus',
            '_Io_1V2PowerGood',
            '_Io_1V8PowerGood',
            '_Io_3V3PowerGood',
            '_Io_5V0PowerGood',
            '_Watchdog_State',
            '_Watchdog_DeploymentStatus',
            '_Watchdog_Uart0State',
            '_Watchdog_Uart1State',
            '_Adc_BatteryTempRaw',
            '_Watchdog_DetailedHeartbeatSequenceNumber',
            '_Watchdog_DigitalOutputStates',
            '_Watchdog_ResetLogs',
            '_Adc_LanderVoltageRaw',
            '_Adc_BatteryChargingTempRaw',
            '_Adc_FullSystemVoltageRaw',
            '_Adc_FullSystemCurrentRaw',
            '_Adc_SwitchedBatteryVoltageRaw',
            '_Adc_Vcc24VoltageRaw',
            '_Heater_ControlEnabled',
            '_Heater_IsHeating',
            '_Adc_2V5VoltageRaw',
            '_Adc_2V8VoltageRaw',
            '_Adc_Vcc28VoltageRaw',
            '_Heater_Kp',
            '_Heater_PwmLimit_DutyCycleCounter',
            '_Heater_SetpointValue',
            '_Heater_OnValue',
            '_Heater_OffValue',
            '_Heater_DutyCyclePeriodCycles',
            '_Heater_DutyCycleCounter',
            '_I2C_BatteryChargeRaw',
            '_I2C_BatteryVoltageRaw',
            '_I2C_BatteryCurrentRaw',
            '_I2C_FuelGaugeTempRaw',
            '_I2C_BatteryChargeTelemRaw',
            '_I2C_BatteryCurrentTelemRaw'
        ]

        _Io_ChargingStatus1: int
        _Io_ChargingStatus2: int
        _Io_BatteryConnectionStatus: int
        _Io_BatteryLatchStatus: int
        _Io_1V2PowerGood: int
        _Io_1V8PowerGood: int
        _Io_3V3PowerGood: int
        _Io_5V0PowerGood: int
        _Watchdog_State: int
        _Watchdog_DeploymentStatus: int
        _Watchdog_Uart0State: int
        _Watchdog_Uart1State: int
        _Adc_BatteryTempRaw: int
        _Watchdog_DetailedHeartbeatSequenceNumber: int
        _Watchdog_DigitalOutputStates: int
        _Watchdog_ResetLogs: int
        _Adc_LanderVoltageRaw: int
        _Adc_BatteryChargingTempRaw: int
        _Adc_FullSystemVoltageRaw: int
        _Adc_FullSystemCurrentRaw: int
        _Adc_SwitchedBatteryVoltageRaw: int
        _Adc_Vcc24VoltageRaw: int
        _Heater_ControlEnabled: int
        _Heater_IsHeating: int
        _Adc_2V5VoltageRaw: int
        _Adc_2V8VoltageRaw: int
        _Adc_Vcc28VoltageRaw: int
        _Heater_Kp: int
        _Heater_PwmLimit_DutyCycleCounter: int
        _Heater_SetpointValue: int
        _Heater_OnValue: int
        _Heater_OffValue: int
        _Heater_DutyCyclePeriodCycles: int
        _Heater_DutyCycleCounter: int
        _I2C_BatteryChargeRaw: int
        _I2C_BatteryVoltageRaw: int
        _I2C_BatteryCurrentRaw: int
        _I2C_FuelGaugeTempRaw: int
        _I2C_BatteryChargeTelemRaw: int
        _I2C_BatteryCurrentTelemRaw: int

        @property
        def _module(self) -> Module:
            return WatchdogDetailedStatusPacketInterface.get_ds_module()

        # Helpful computed properties (which don't end up in Computed TelemetryChannels):
        @property
        def mean_lander_voltage(self) -> float:
            return self.Adc_LanderVoltage / 2.0 + self.Adc_Vcc28Voltage / 2.0

        @property
        def fused_est_lander_voltage(self) -> float:
            """
            Fuses two sensor readings weighted based on their relative 
            uncertainties to estimated the true lander voltage.
            """
            # TODO: Update these weights from empirical measurements of their
            # accuracies and uncertainties
            dLander = 0.25  # [Volts] (uncertainty in LanderVoltage reading)
            dVcc28 = 0.5  # [Volts] (uncertainty in Vcc28Voltage reading)

            VLander = self.Adc_LanderVoltage
            Vcc28 = self.Adc_Vcc28Voltage

            # Guard against 0:
            if max(abs(VLander), abs(Vcc28)) == 0:
                return 0

            # If a significant difference exists (i.e. one sensor is likely faulty)...:
            if (abs(Vcc28 - VLander) / max(abs(VLander), abs(Vcc28))) > 0.5:
                # If one of them is significantly greater (50%) than the max
                # possible lander voltage (i.e. way to large -- failed high),
                # take the other one:
                if (VLander > 1.5 * self.V_LANDER_MAX) and (Vcc28 <= 1.5 * self.V_LANDER_MAX):
                    return Vcc28
                if (Vcc28 > 1.5 * self.V_LANDER_MAX) and (VLander <= 1.5 * self.V_LANDER_MAX):
                    return VLander
                # otherwise, just use the larger of the two (since the lower
                # one likely failed low):
                return max(VLander, Vcc28)

            # Guard against poor uncertainty settings:
            if (dLander+dVcc28) == 0:
                return 0

            # Both sensors have consistent values, so fuse the results:
            return VLander * (1.0-dLander/(dLander+dVcc28)) + Vcc28 * (1.0-dVcc28/(dLander+dVcc28))

        # Computed properties for Computed Telemetry Channels:
        @property
        def Io_ChargerState(self) -> int:
            return self.Io_ChargingStatus1 << 1 | self.Io_ChargingStatus2

        @property
        def Io_BatteryState(self) -> int:
            return self.Io_BatteryConnectionStatus << 1 | self.Io_BatteryLatchStatus

        @property
        def Watchdog_DigitalOutputStates_Dict(self) -> OrderedDict[str, int]:
            data = self.Watchdog_DigitalOutputStates.to_bytes(4, 'big')
            # unfortunately we can't just feed `to_bytes(4, 'little')` into
            # `bitstruct` because it expects the order of the bits in each byte
            # to be the left to right but MSP fills right to left. So, we have
            # to either convert to little then flip the order of the bits in
            # each byte or convert to big and flip the order of all the bits:
            data = flip_all_bits_in_bytes(data)

            bitfields = self._module.telemetry['Watchdog_DigitalOutputStates'].bitfields

            if bitfields is not None:
                return bitfields.unpack(data)
            else:
                return OrderedDict()

        @property
        def Watchdog_CombinedDigitalStates_Dict(self) -> OrderedDict[str, int]:
            """
            Creates a dict of states for each pin where:
                0 = output driven low
                1 = output driven high
                2 = input (Hi-Z)
            """
            # Make a new dict with all entries present in bitfield definition:
            bitfields = self._module.telemetry['Watchdog_CombinedDigitalStates'].bitfields
            if bitfields is not None:
                fields = bitfields.fields.copy()

            # Init all entries with a value indicating they haven't been processed yet:
            for k in fields.keys():
                fields[k] = -1

            # Grab states from Watchdog_DigitalOutputStates:
            out_states = self.Watchdog_DigitalOutputStates_Dict
            # Transfer over all states that exist in both:
            for k in fields.keys():
                fields[k] = out_states['OPSBI__'+k]

            # Build special (input capable) states:
            fields['CHRG_EN'] = 1 if out_states['OPSBI__CHRG_EN_FORCE_HIGH'] else out_states['OPSBI__CHRG_EN'] * 2
            if out_states['OPSBI__V_SYS_ALL_EN_FORCE_LOW']:
                fields['V_SYS_ALL_EN'] = 0
            else:
                fields['V_SYS_ALL_EN'] = 1 if out_states['OPSBI__V_SYS_ALL_EN'] else 2

            fields['RADIO_N_RST'] = 2 if out_states['OPSBI__RADIO_N_RESET_IS_INPUT'] else out_states['OPSBI__RADIO_N_RST']
            fields['HERCULES_N_RST'] = 2 if out_states['OPSBI__HERCULES_N_RST_IS_INPUT'] else out_states['OPSBI__HERCULES_N_RST']
            fields['HERCULES_N_PORRST'] = 2 if out_states['OPSBI__HERCULES_N_PORRST_IS_INPUT'] else out_states['OPSBI__HERCULES_N_PORRST']
            fields['FPGA_N_RST'] = 2 if out_states['OPSBI__FPGA_N_RST_IS_INPUT'] else out_states['OPSBI__FPGA_N_RST']
            fields['LATCH_SET'] = 2 if out_states['OPSBI__LATCH_SET_IS_INPUT'] else out_states['OPSBI__LATCH_SET']
            fields['LATCH_RESET'] = 2 if out_states['OPSBI__LATCH_RESET_IS_INPUT'] else out_states['OPSBI__LATCH_RESET']
            fields['BATT_STAT'] = 2 if out_states['OPSBI__BATT_STAT_IS_INPUT'] else out_states['OPSBI__BATT_STAT']

            # Check for any unprocessed fields:
            if any(v < 0 for v in fields.values()):
                raise ValueError(
                    "In `WatchdogDetailedStatus`, not all fields in "
                    "`Watchdog_CombinedDigitalStates` have a matching field in "
                    "`Watchdog_DigitalOutputStates`. "
                    "`-1` indicates unmatched in the following: "
                    f"Watchdog_CombinedDigitalStates: {fields}, "
                    f"Watchdog_DigitalOutputStates: {out_states} ."
                )

            return fields

        @property
        def Watchdog_CombinedDigitalStates(self) -> int:
            bitfields = self._module.telemetry['Watchdog_CombinedDigitalStates'].bitfields
            if bitfields is not None:
                raw_data = bitfields.pack(
                    self.Watchdog_CombinedDigitalStates_Dict)
                # flip order of all bits to be consistent with `Watchdog_DigitalOutputStates_Dict` decoding:
                raw_data = flip_all_bits_in_bytes(raw_data)
                return int.from_bytes(raw_data, 'big', signed=False)
            else:
                return 0

        @property
        def Watchdog_CombinedDigitalStates_Shorthand(self) -> OrderedDict[str, int]:
            """
            Returns a dict containing all fields from `Watchdog_CombinedDigitalStates_Dict`
            but using shorthand aliases for each pin (for easy printing):
            """
            long_form = self.Watchdog_CombinedDigitalStates_Dict
            return OrderedDict([
                ('HEAT', long_form['HEATER']),
                ('DEP', long_form['DEPLOYMENT']),

                ('BSTAT', long_form['BATT_STAT']),

                ('BE', long_form['BATTERY_EN']),
                ('LB', long_form['LATCH_BATT']),
                ('LS', long_form['LATCH_SET']),
                ('LR', long_form['LATCH_RESET']),

                ('CE', long_form['CHRG_EN']),
                ('REGE', long_form['V_LANDER_REG_EN']),

                ('VSAE', long_form['V_SYS_ALL_EN']),

                ('3V3_EN', long_form['3V3_EN']),

                ('H_ON', long_form['HERCULES_ON']),
                ('R_ON', long_form['RADIO_ON']),
                ('F_ON', long_form['FPGA_ON']),
                ('M_ON', long_form['MOTOR_ON']),

                ('H_RST', long_form['HERCULES_N_RST']),
                ('H_N_PRST', long_form['HERCULES_N_PORRST']),
                ('F_N_RST', long_form['FPGA_N_RST']),
                ('R_N_RST', long_form['RADIO_N_RST']),

                ('CAMSEL', long_form['FPGA_KICK_AKA_CAM_SELECT']),
                ('BMSB', long_form['BMS_BOOT'])
            ])

        @property
        def Watchdog_CombinedDigitalStates_DataFrame(self) -> DataFrame:
            """
            Returns a Pandas DataFrame containing all fields from 
            `Watchdog_CombinedDigitalStates_Dict` using shorthand aliases for 
            each pin from `Watchdog_CombinedDigitalStates_Shorthand` for the 
            column headers.
            """
            return DataFrame(self.Watchdog_CombinedDigitalStates_Shorthand, index=['GPIO (2=HiZ)'])

        @property
        def Watchdog_ResetLogs_Dict(self) -> OrderedDict[str, int]:
            data = self.Watchdog_ResetLogs.to_bytes(40//8, 'big')
            # unfortunately we can't just feed `to_bytes(5, 'little')` into
            # `bitstruct` because it expects the order of the bits in each byte
            # to be the left to right but MSP fills right to left. So, we have
            # to either convert to little then flip the order of the bits in
            # each byte or convert to big and flip the order of all the bits:
            data = flip_all_bits_in_bytes(data)

            bitfields = self._module.telemetry['Watchdog_ResetLogs'].bitfields
            if bitfields is not None:
                return bitfields.unpack(data)
            else:
                return OrderedDict()

        @property
        def Watchdog_ResetLogs_Dict_Shorthand(self) -> OrderedDict[str, int]:
            """
            Returns a dict containing all fields from `Watchdog_ResetLogs_Dict`
            but using shorthand aliases for each pin (for easy printing):
            """
            long_form = self.Watchdog_ResetLogs_Dict
            return OrderedDict([
                ('NONE', long_form['RABI__NO_RESET']),

                ('HDRM_SIG_ON', long_form['RABI__HDRM_DEPLOY_SIGNAL_POWER_ON']),  # noqa (don't wrap this line around)
                ('HDRM_SIG_OFF', long_form['RABI__HDRM_DEPLOY_SIGNAL_POWER_OFF']),  # noqa (don't wrap this line around)

                ('BATT_EN', long_form['RABI__BATTERIES_ENABLE']),
                ('BATT_DIS', long_form['RABI__BATTERIES_DISABLE']),
                ('CHARGE_START', long_form['RABI__BATTERY_CHARGE_START']),
                ('CHARGE_STOP', long_form['RABI__BATTERY_CHARGE_STOP']),

                ('HEAT_CTRL_EN', long_form['RABI__AUTO_HEATER_CONTROLLER_ENABLE']),  # noqa (don't wrap this line around)
                ('HEAT_CTRL_DIS', long_form['RABI__AUTO_HEATER_CONTROLLER_DISABLE']),  # noqa (don't wrap this line around)

                ('RS422_EN', long_form['RABI__RS422_UART_ENABLE']),
                ('RS422_DIS', long_form['RABI__RS422_UART_DISABLE']),

                ('3V3_EN_RST', long_form['RABI__3V3_EN_RESET']),
                ('3V3_EN_UNRST', long_form['RABI__3V3_EN_UNRESET']),
                ('3V3_EN_ON', long_form['RABI__3V3_EN_POWER_ON']),
                ('3V3_EN_OFF', long_form['RABI__3V3_EN_POWER_OFF']),

                ('H_RST', long_form['RABI__HERCULES_RESET']),
                ('H_UNRST', long_form['RABI__HERCULES_UNRESET']),
                ('H_ON', long_form['RABI__HERCULES_POWER_ON']),
                ('H_OFF', long_form['RABI__HERCULES_POWER_OFF']),
                ('H_WD_EN', long_form['RABI__HERCULES_WATCHDOG_ENABLE']),
                ('H_WD_DIS', long_form['RABI__HERCULES_WATCHDOG_DISABLE']),
                ('H_WD_RST', long_form['RABI__HERCULES_WATCHDOG_RESET']),

                ('R_RST', long_form['RABI__RADIO_RESET']),
                ('R_UNRST', long_form['RABI__RADIO_UNRESET']),
                ('R_ON', long_form['RABI__RADIO_POWER_ON']),
                ('R_OFF', long_form['RABI__RADIO_POWER_OFF']),

                ('F_RST', long_form['RABI__CAM_FPGA_RESET']),
                ('F_UNRST', long_form['RABI__CAM_FPGA_UNRESET']),
                ('F_ON', long_form['RABI__CAM_FPGA_POWER_ON']),
                ('F_OFF', long_form['RABI__CAM_FPGA_POWER_OFF']),

                ('CAM0_SEL', long_form['RABI__FPGA_CAM_0_SELECT']),
                ('CAM1_SEL', long_form['RABI__FPGA_CAM_1_SELECT']),

                ('ALL_MOT_ON', long_form['RABI__ALL_MOTORS_POWER_ON']),
                ('ALL_MOT_OFF', long_form['RABI__ALL_MOTORS_POWER_OFF']),

                ('VSA_RST', long_form['RABI__V_SYS_ALL_OFF__RESET']),
                ('VSA_UNRST', long_form['RABI__V_SYS_ALL_ON__UNRESET']),
                ('VSA_ON', long_form['RABI__V_SYS_ALL_POWER_ON']),
                ('VSA_OFF', long_form['RABI__V_SYS_ALL_POWER_OFF'])
            ])

        @property
        def Watchdog_ResetLogs_Dict_DataFrame(self) -> DataFrame:
            """
            Returns a Pandas DataFrame containing all fields from 
            `Watchdog_ResetLogs_Dict_Dict` using shorthand aliases for 
            each pin from `Watchdog_ResetLogs_Dict_Shorthand` for the 
            column headers.
            """
            return DataFrame(self.Watchdog_ResetLogs_Dict_Shorthand, index=['Flag'])

        @property
        def Watchdog_ResetEventsList(self) -> List[str]:
            """
            Returns a list of the names of all ResetLogs that have a non-zero 
            value (i.e. the event happened).
            """
            return [name.replace('RABI__', '') for name, val in self.Watchdog_ResetLogs_Dict.items() if val != 0]  # noqa (don't wrap this line around)

        @property
        def Adc_LanderVoltage(self) -> float:
            return self.VL_ADC_FM1_CORRECTION_FACTOR * float(self.Adc_LanderVoltageRaw) / 4095.0 * 3.3 * (232.0+2000.0)/232.0

        @property
        def Adc_BatteryChargingTempKelvin(self) -> float:
            R25 = 10e3
            RT1 = 4320
            RT2 = 19100
            if self.Adc_BatteryChargingTempRaw > 0:
                V_ADC = float(self.Adc_BatteryChargingTempRaw) / 4095.0 * 3.3
                R_TH = 1.0 / ((3.3 - V_ADC) / (V_ADC * RT1) - 1.0 / RT2)
                return float(np.interp(
                    R_TH/R25,
                    self.BATT_CHRG_10K_THERMISTOR_LOOKUP_TABLE['RTH_R25'][::-1],
                    self.BATT_CHRG_10K_THERMISTOR_LOOKUP_TABLE['degC'][::-1]
                )) + 273.15
            else:
                return 0

        @property
        def Adc_BatteryChargingTempUncertaintyKelvin(self) -> float:
            # ! TODO: (pull from BACK HAL code + supporting samsung notes)
            return 0.0

        @classmethod
        def battery_adc_reading_to_kelvin(cls, adc_reading) -> float:
            return float(np.interp(
                float(adc_reading),
                cls.BATT_5K_THERMISTOR_LOOKUP_TABLE['adc'][::-1],
                cls.BATT_5K_THERMISTOR_LOOKUP_TABLE['degC'][::-1]
            )) + 273.15

        @property
        def Adc_BatteryTempKelvin(self) -> float:
            full_adc_reading = self.Adc_BatteryTempRaw  # already 12b
            return self.battery_adc_reading_to_kelvin(full_adc_reading)

        @property
        def Adc_BatteryTempUncertaintyKelvin(self) -> float:
            # ! TODO: calculate. Use `Adc_BatteryChargingTempUncertaintyKelvin` for reference
            return 0.0

        @property
        def Adc_FullSystemVoltage(self) -> float:
            return self.VSA_ADC_FM1_CORRECTION_FACTOR * float(self.Adc_FullSystemVoltageRaw) / 4095.0 * 3.3 * (232.0+2000.0)/232.0

        @property
        def Adc_FullSystemCurrent(self) -> float:
            # uses *bottom* 9b, so we don't have to shift it, it just saturates earlier:
            full_adc_reading = self.Adc_FullSystemCurrentRaw
            return float(full_adc_reading) / 4095 * 3.3 * 4600/1000

        @property
        def Adc_SwitchedBatteryVoltage(self) -> float:
            return self.VBS_ADC_FM1_CORRECTION_FACTOR * float(self.Adc_SwitchedBatteryVoltageRaw) / 4095.0 * 3.3 * (274.0+2000.0)/274.0

        @property
        def Adc_2V5Voltage(self) -> float:
            # no divider (or protection resistor) used, we read it directly:
            return float(self.Adc_2V5VoltageRaw) / 4095.0 * 3.3

        @property
        def Adc_2V8Voltage(self) -> float:
            # no divider (or protection resistor) used, we read it directly:
            return float(self.Adc_2V8VoltageRaw) / 4095.0 * 3.3

        @property
        def Adc_Vcc28Voltage(self) -> float:
            return self.VCC28_ADC_FM1_CORRECTION_FACTOR * float(self.Adc_Vcc28VoltageRaw) / 4095.0 * 3.3 * (47.0+470.0)/47.0

        @property
        def Adc_Vcc24Voltage(self) -> float:
            return float(self.Adc_Vcc24VoltageRaw) / 4095.0 * 3.3 * (47.0+330.0)/47.0

        @property
        def Adc_DataFrame(self) -> DataFrame:
            """
            Creates a Pandas DataFrame that summarizes all ADC readings incl. 
            both their raw ADC readings and their conversions to human-readable 
            units.
            """
            return DataFrame({
                '2V5': [self.Adc_2V5VoltageRaw, f"{self.Adc_2V5Voltage:.2f}V"],
                '2V8': [self.Adc_2V8VoltageRaw, f"{self.Adc_2V8Voltage:.2f}V"],
                '24V': [self.Adc_Vcc24VoltageRaw, f"{self.Adc_Vcc24Voltage:.2f}V"],
                'VBm': [self.Adc_SwitchedBatteryVoltageRaw, f"{self.Adc_SwitchedBatteryVoltage:.2f}V"],
                'VL': [self.Adc_LanderVoltageRaw, f"{self.Adc_LanderVoltage:.2f}V"],
                'Vcc28': [self.Adc_Vcc28VoltageRaw, f"{self.Adc_Vcc28Voltage:.2f}V"],
                'VSA': [self.Adc_FullSystemVoltageRaw, f"{self.Adc_FullSystemVoltage:.2f}V"],
                'ISA': [self.Adc_FullSystemCurrentRaw, f"{self.Adc_FullSystemCurrent*1000:.1f}mA"],
                'Tbatt': [self.Adc_BatteryTempRaw, f"{self.Adc_BatteryTempKelvin:.1f}±{self.Adc_BatteryTempUncertaintyKelvin:.1f}K"],
                'Tchrg': [self.Adc_BatteryChargingTempRaw, f"{self.Adc_BatteryChargingTempKelvin:.1f}±{self.Adc_BatteryChargingTempUncertaintyKelvin:.1f}K"]
            }, index=['ADC', 'val'])

        @property
        def Heater_PwmLimit_DutyCyclePercent(self) -> float:
            # NOTE: PwmLimit is deprecated
            if self.Heater_DutyCyclePeriodCycles == 0:
                return float('Inf')
            else:
                return 100.0 * float(self.Heater_PwmLimit_DutyCycleCounter) / float(self.Heater_DutyCyclePeriodCycles)

        @property
        def Heater_EffectivePowerLimit(self) -> float:
            # NOTE: PwmLimit is deprecated
            max_avail_voltage = self.fused_est_lander_voltage * \
                self.Heater_PwmLimit_DutyCyclePercent / 100.0
            if self.R_HEATER == 0:
                return float('Inf')
            else:
                return max_avail_voltage**2 / self.R_HEATER

        @property
        def Heater_MaxPossiblePower(self) -> float:
            # Maximum possible heater power
            if self.R_HEATER == 0:
                return float('Inf')
            else:
                return self.fused_est_lander_voltage**2 / self.R_HEATER

        @property
        def Heater_SetpointKelvin(self) -> float:
            # ! TODO: Why are we sending 16b if Adc maxes out at 12b... Strongly consider revising.
            # already 12b (16b sent)
            full_adc_reading = self.Heater_SetpointValue
            return self.battery_adc_reading_to_kelvin(full_adc_reading)

        @property
        def Heater_OnTempKelvin(self) -> float:
            # ! TODO: Why are we sending 16b if Adc maxes out at 12b... Strongly consider revising.
            full_adc_reading = self.Heater_OnValue  # already 12b (16b sent)
            return self.battery_adc_reading_to_kelvin(full_adc_reading)

        @property
        def Heater_OffTempKelvin(self) -> float:
            # ! TODO: Why are we sending 16b if Adc maxes out at 12b... Strongly consider revising.
            full_adc_reading = self.Heater_OffValue  # already 12b (16b sent)
            return self.battery_adc_reading_to_kelvin(full_adc_reading)

        @property
        def Heater_DutyCyclePeriodMs(self) -> float:
            wd_pwm_clock_freq: float = 8e6
            return 1000.0 * float(self.Heater_DutyCyclePeriodCycles) / wd_pwm_clock_freq

        @property
        def Heater_DutyCyclePercent(self) -> float:
            if self.Heater_DutyCyclePeriodCycles == 0:
                return float('Inf')
            else:
                return 100.0 * float(self.Heater_DutyCycleCounter) / float(self.Heater_DutyCyclePeriodCycles)

        @property
        def Heater_EffectiveVoltage(self) -> float:
            return self.fused_est_lander_voltage * self.Heater_DutyCyclePercent / 100.0

        @property
        def Heater_EffectivePower(self) -> float:
            if self.R_HEATER == 0:
                return float('Inf')
            else:
                return self.Heater_EffectiveVoltage**2 / self.R_HEATER

        @property
        def I2C_BatteryChargeMah(self) -> float:
            # ! TODO: Impl. me! Conversion currently unknown
            return 0.0

        @property
        def I2C_BatteryVoltage(self) -> float:
            # ! TODO: Verify this conversion. Was just pulled from old WD code.
            return self.I2C_BatteryVoltageRaw * 0.00108033875

        @property
        def I2C_BatteryCurrent(self) -> float:
            # ! TODO: Verify this conversion. Was just pulled from old WD code.
            return 0.0000390636921 * self.I2C_BatteryCurrentRaw

        @property
        def I2C_FuelGaugeTempKelvin(self) -> float:
            # ! TODO: Verify this conversion. Was just pulled from old WD code.
            return 0.00778210117 * self.I2C_FuelGaugeTempRaw

        @property
        def I2C_BatteryChargeTelemMah(self) -> float:
            # ! TODO: Impl. me! Conversion currently unknown
            # TODO: Do we even need to send this? (isn't it covered by the above I2C readings?)
            return 0.0

        @property
        def I2C_BatteryCurrentTelemAmps(self) -> float:
            # ! TODO: Impl. me! Conversion currently unknown
            # TODO: Do we even need to send this? (isn't it covered by the above I2C readings?)
            return 0.0

        # Make public get, private set to signal that you can freely use these values
        # but modifying them directly can yield undefined behavior (specifically
        # `raw` not syncing up with whatever other data is in the container)

        @property
        def Io_ChargingStatus1(self) -> int: return self._Io_ChargingStatus1
        @property
        def Io_ChargingStatus2(self) -> int: return self._Io_ChargingStatus2

        @property
        def Io_BatteryConnectionStatus(self) -> int:
            return self._Io_BatteryConnectionStatus

        @property
        def Io_BatteryLatchStatus(self) -> int:
            return self._Io_BatteryLatchStatus

        @property
        def Io_1V2PowerGood(self) -> int: return self._Io_1V2PowerGood
        @property
        def Io_1V8PowerGood(self) -> int: return self._Io_1V8PowerGood
        @property
        def Io_3V3PowerGood(self) -> int: return self._Io_3V3PowerGood
        @property
        def Io_5V0PowerGood(self) -> int: return self._Io_5V0PowerGood
        @property
        def Watchdog_State(self) -> int: return self._Watchdog_State

        @property
        def Watchdog_DeploymentStatus(self) -> int:
            return self._Watchdog_DeploymentStatus

        @property
        def Watchdog_Uart0State(self) -> int: return self._Watchdog_Uart0State
        @property
        def Watchdog_Uart1State(self) -> int: return self._Watchdog_Uart1State
        @property
        def Adc_BatteryTempRaw(self) -> int: return self._Adc_BatteryTempRaw

        @property
        def Watchdog_DetailedHeartbeatSequenceNumber(self) -> int:
            return self._Watchdog_DetailedHeartbeatSequenceNumber

        @property
        def Watchdog_DigitalOutputStates(self) -> int:
            return self._Watchdog_DigitalOutputStates

        @property
        def Watchdog_ResetLogs(self) -> int: return self._Watchdog_ResetLogs

        @property
        def Adc_LanderVoltageRaw(self) -> int:
            return self._Adc_LanderVoltageRaw

        @property
        def Adc_BatteryChargingTempRaw(self) -> int:
            return self._Adc_BatteryChargingTempRaw

        @property
        def Adc_FullSystemVoltageRaw(self) -> int:
            return self._Adc_FullSystemVoltageRaw

        @property
        def Adc_FullSystemCurrentRaw(self) -> int:
            return self._Adc_FullSystemCurrentRaw

        @property
        def Adc_SwitchedBatteryVoltageRaw(self) -> int:
            return self._Adc_SwitchedBatteryVoltageRaw

        @property
        def Adc_Vcc24VoltageRaw(self) -> int: return self._Adc_Vcc24VoltageRaw

        @property
        def Heater_ControlEnabled(self) -> int:
            return self._Heater_ControlEnabled

        @property
        def Heater_IsHeating(self) -> int: return self._Heater_IsHeating
        @property
        def Adc_2V5VoltageRaw(self) -> int: return self._Adc_2V5VoltageRaw
        @property
        def Adc_2V8VoltageRaw(self) -> int: return self._Adc_2V8VoltageRaw
        @property
        def Adc_Vcc28VoltageRaw(self) -> int: return self._Adc_Vcc28VoltageRaw
        @property
        def Heater_Kp(self) -> int: return self._Heater_Kp

        @property
        def Heater_PwmLimit_DutyCycleCounter(self) -> int:
            return self._Heater_PwmLimit_DutyCycleCounter

        @property
        def Heater_SetpointValue(self) -> int:
            return self._Heater_SetpointValue

        @property
        def Heater_OnValue(self) -> int: return self._Heater_OnValue
        @property
        def Heater_OffValue(self) -> int: return self._Heater_OffValue

        @property
        def Heater_DutyCyclePeriodCycles(self) -> int:
            return self._Heater_DutyCyclePeriodCycles

        @property
        def Heater_DutyCycleCounter(self) -> int:
            return self._Heater_DutyCycleCounter

        @property
        def I2C_BatteryChargeRaw(self) -> int:
            return self._I2C_BatteryChargeRaw

        @property
        def I2C_BatteryVoltageRaw(self) -> int:
            return self._I2C_BatteryVoltageRaw

        @property
        def I2C_BatteryCurrentRaw(self) -> int:
            return self._I2C_BatteryCurrentRaw

        @property
        def I2C_FuelGaugeTempRaw(self) -> int:
            return self._I2C_FuelGaugeTempRaw

        @property
        def I2C_BatteryChargeTelemRaw(
            self) -> int: return self._I2C_BatteryChargeTelemRaw

        @property
        def I2C_BatteryCurrentTelemRaw(self) -> int:
            return self._I2C_BatteryCurrentTelemRaw

        # Name accessors for all enum-related params:
        def getEnumName(self, field_name) -> str:
            """
            Gets the name associated with the int value of the enum with the
            given `field_name`.
            Example: `getEnumName(Watchdog_State)` loads the int value of
            `Watchdog_State` and converts it a string as given by the enum for
            the `Watchdog_State` `TelemetryChannel` in the `Module` associated
            with this packet. Works for computed enum channels too (e.g.
            `Io_ChargerState`).
            """
            return cast(str, self._module.telemetry[field_name].get_enum_name(
                getattr(self, field_name)
            ))

        def __init__(self,
                     Io_ChargingStatus1: int,
                     Io_ChargingStatus2: int,
                     Io_BatteryConnectionStatus: Union[int, str],
                     Io_BatteryLatchStatus: Union[int, str],
                     Io_1V2PowerGood: Union[int, str],
                     Io_1V8PowerGood: Union[int, str],
                     Io_3V3PowerGood: Union[int, str],
                     Io_5V0PowerGood: Union[int, str],
                     Watchdog_State: Union[int, str],
                     Watchdog_DeploymentStatus: Union[int, str],
                     Watchdog_Uart0State: Union[int, str],
                     Watchdog_Uart1State: Union[int, str],
                     Adc_BatteryTempRaw: int,
                     Watchdog_DetailedHeartbeatSequenceNumber: int,
                     Watchdog_DigitalOutputStates: int,
                     Watchdog_ResetLogs: int,
                     Adc_LanderVoltageRaw: int,
                     Adc_BatteryChargingTempRaw: int,
                     Adc_FullSystemVoltageRaw: int,
                     Adc_FullSystemCurrentRaw: int,
                     Adc_SwitchedBatteryVoltageRaw: int,
                     Adc_Vcc24VoltageRaw: int,
                     Heater_ControlEnabled: Union[int, str],
                     Heater_IsHeating: Union[int, str],
                     Adc_2V5VoltageRaw: int,
                     Adc_2V8VoltageRaw: int,
                     Adc_Vcc28VoltageRaw: int,
                     Heater_Kp: int,
                     Heater_PwmLimit_DutyCycleCounter: int,
                     Heater_SetpointValue: int,
                     Heater_OnValue: int,
                     Heater_OffValue: int,
                     Heater_DutyCyclePeriodCycles: int,
                     Heater_DutyCycleCounter: int,
                     I2C_BatteryChargeRaw: int,
                     I2C_BatteryVoltageRaw: int,
                     I2C_BatteryCurrentRaw: int,
                     I2C_FuelGaugeTempRaw: int,
                     I2C_BatteryChargeTelemRaw: int,
                     I2C_BatteryCurrentTelemRaw: int
                     ) -> None:
            # TODO: Put all endianness flips and bitshifts here:

            self._Io_ChargingStatus1 = Io_ChargingStatus1
            self._Io_ChargingStatus2 = Io_ChargingStatus2
            self._Adc_BatteryTempRaw = Adc_BatteryTempRaw
            self._Watchdog_DetailedHeartbeatSequenceNumber = Watchdog_DetailedHeartbeatSequenceNumber
            self._Watchdog_DigitalOutputStates = Watchdog_DigitalOutputStates
            self._Watchdog_ResetLogs = Watchdog_ResetLogs
            self._Adc_LanderVoltageRaw = Adc_LanderVoltageRaw
            self._Adc_BatteryChargingTempRaw = Adc_BatteryChargingTempRaw
            self._Adc_FullSystemVoltageRaw = Adc_FullSystemVoltageRaw
            self._Adc_FullSystemCurrentRaw = Adc_FullSystemCurrentRaw
            self._Adc_SwitchedBatteryVoltageRaw = Adc_SwitchedBatteryVoltageRaw
            self._Adc_Vcc24VoltageRaw = Adc_Vcc24VoltageRaw
            self._Adc_2V5VoltageRaw = Adc_2V5VoltageRaw
            self._Adc_2V8VoltageRaw = Adc_2V8VoltageRaw
            self._Adc_Vcc28VoltageRaw = Adc_Vcc28VoltageRaw
            self._Heater_Kp = Heater_Kp
            self._Heater_PwmLimit_DutyCycleCounter = Heater_PwmLimit_DutyCycleCounter
            self._Heater_SetpointValue = Heater_SetpointValue
            self._Heater_OnValue = Heater_OnValue
            self._Heater_OffValue = Heater_OffValue
            self._Heater_DutyCyclePeriodCycles = Heater_DutyCyclePeriodCycles
            self._Heater_DutyCycleCounter = Heater_DutyCycleCounter
            self._I2C_BatteryChargeRaw = I2C_BatteryChargeRaw
            self._I2C_BatteryVoltageRaw = I2C_BatteryVoltageRaw
            self._I2C_BatteryCurrentRaw = I2C_BatteryCurrentRaw
            self._I2C_FuelGaugeTempRaw = I2C_FuelGaugeTempRaw
            self._I2C_BatteryChargeTelemRaw = I2C_BatteryChargeTelemRaw
            self._I2C_BatteryCurrentTelemRaw = I2C_BatteryCurrentTelemRaw

            def load_enum(field_name: str, val: Union[int, str]) -> None:
                """
                Allows an Enum field value to be given as an `int` of an 
                EnumItem value or `str` of an EnumItem name and ensures that the 
                `int` value is what's stored.
                """
                if isinstance(val, int):
                    setattr(self, '_'+field_name, val)
                else:
                    intval = cast(int, self._module.telemetry[field_name].get_enum_value(
                        cast(str, val)
                    ))
                    setattr(self, '_'+field_name, intval)

            load_enum('Io_BatteryConnectionStatus', Io_BatteryConnectionStatus)
            load_enum('Io_BatteryLatchStatus', Io_BatteryLatchStatus)
            load_enum('Io_1V2PowerGood', Io_1V2PowerGood)
            load_enum('Io_1V8PowerGood', Io_1V8PowerGood)
            load_enum('Io_3V3PowerGood', Io_3V3PowerGood)
            load_enum('Io_5V0PowerGood', Io_5V0PowerGood)
            load_enum('Watchdog_State', Watchdog_State)
            load_enum('Watchdog_DeploymentStatus', Watchdog_DeploymentStatus)
            load_enum('Watchdog_Uart0State', Watchdog_Uart0State)
            load_enum('Watchdog_Uart1State', Watchdog_Uart1State)
            load_enum('Heater_ControlEnabled', Heater_ControlEnabled)
            load_enum('Heater_IsHeating', Heater_IsHeating)

        def __repr__(self) -> str:
            return (
                f"#{self.Watchdog_DetailedHeartbeatSequenceNumber} "
                f"[{self.getEnumName('Watchdog_State')} + {self.getEnumName('Watchdog_DeploymentStatus')}] \t"
                f"CHARGER: {self.getEnumName('Io_ChargerState')} \t"
                f"BATTERIES: {self.getEnumName('Io_BatteryState')} \t"
                f"Herc-UART0: {self.getEnumName('Watchdog_Uart0State')} \t"
                f"Lander-UART1: {self.getEnumName('Watchdog_Uart1State')}"
                "\n"
                f"1V2: {self.getEnumName('Io_1V2PowerGood')} \t"
                f"1V8: {self.getEnumName('Io_1V8PowerGood')} \t"
                f"2V5: {self.Adc_2V5Voltage:.1f} \t"
                f"2V8: {self.Adc_2V8Voltage:.1f} \t"
                f"3V3: {self.getEnumName('Io_3V3PowerGood')} \t"
                f"5V0: {self.getEnumName('Io_5V0PowerGood')} \t"
                f"24V: {self.Adc_Vcc24Voltage:.2f}"
                "\n"
                f"VBm: {self.Adc_SwitchedBatteryVoltage:.2f}V \t"
                f"VL: {self.Adc_LanderVoltage:.2f}V \t"
                f"Vcc28: {self.Adc_Vcc28Voltage:.2f}V \t"
                f"VSA: {self.Adc_FullSystemVoltage:.2f}V \t"
                f"ISA: {self.Adc_FullSystemCurrent*1000:.1f}mA"
                "\n"
                "HEATER "
                f"[↑{self.Heater_OnTempKelvin:.0f}K ({self.Heater_OnValue}) | "
                f"↓{self.Heater_OffTempKelvin:.0f}K ({self.Heater_OffValue})] \t"
                f" is {self.getEnumName('Heater_IsHeating')}, "
                f"Control: {self.getEnumName('Heater_ControlEnabled')} \t"
                f"@ {self.Heater_EffectivePower:.3f}W / {self.Heater_MaxPossiblePower:.3f}W \t"
                f"Period: {self.Heater_DutyCyclePeriodMs:.2f}ms"
                "\n"
                f"Tbatt: {self.Adc_BatteryTempKelvin:.1f}K ± {self.Adc_BatteryTempUncertaintyKelvin}K \t"
                f"Tchrg: {self.Adc_BatteryChargingTempKelvin:.1f}K ± {self.Adc_BatteryChargingTempUncertaintyKelvin}K \t"
                f"Tblimp: {self.I2C_FuelGaugeTempKelvin:.1f}K"
                "\n"
                "BATTERY MONITOR: "
                f"{self.I2C_BatteryVoltage:.2f}V \t"
                f"{self.I2C_BatteryCurrent*1000:.1f}mA \t"
                f"{self.I2C_BatteryChargeMah:.0f}mAh \t"
                f"\n{self.Adc_DataFrame.to_string(float_format=lambda x: '{:.2f}'.format(x))}"
                f"\n{self.Watchdog_CombinedDigitalStates_DataFrame.to_string()}"
                # f"GPIO (2=HiZ): {dict(**self.Watchdog_CombinedDigitalStates_Dict)} \n"
                f"\nReset/Flag Events: [{', '.join(self.Watchdog_ResetEventsList)}]"
                # f"\n{dict(**self.Watchdog_ResetLogs_Dict_Shorthand)}"
                # "\n"
                "\n"
            )


# Some useful type alias that make the subsequent class more readable:
WDS_PI = WatchdogDetailedStatusPacketInterface
WDS_CP = WDS_PI.CustomPayload


class WatchdogDetailedStatusPacket(WDS_PI[WDS_PI, WDS_CP]):
    # Properties (r-only class variables):
    START_FLAG: bytes = b'\xD5'  # Required start flag
    WD_ADC_BITS: int = 12
    # Specify the `CUSTOM_PAYLOAD_CLASS` used by `CustomPayloadPacket` superclass (weird type signature is used to match the `Optional[Type[CPCT]]` used in the superclass):
    CUSTOM_PAYLOAD_CLASS: Optional[Type[WDS_CP]] = WDS_CP

    # Bitfield Struct Allocations (represents the order and number of bits
    # assigned to all data in the message's bitfield struct):
    # Each field has the following entries: (n_bits, is_adc_val, n_upper_adc_bits, n_lower_adc_bits)
    BITFIELD_ALLOCATIONS: OrderedDict = OrderedDict([
        ('Io_ChargingStatus1', (1, False, 0, 0)),
        ('Io_ChargingStatus2', (1, False, 0, 0)),
        ('Io_BatteryConnectionStatus', (1, False, 0, 0)),
        ('Io_BatteryLatchStatus', (1, False, 0, 0)),
        ('Io_1V2PowerGood', (1, False, 0, 0)),
        ('Io_1V8PowerGood', (1, False, 0, 0)),
        ('Io_3V3PowerGood', (1, False, 0, 0)),
        ('Io_5V0PowerGood', (1, False, 0, 0)),
        ('Watchdog_State', (8, False, 0, 0)),
        ('Watchdog_DeploymentStatus', (2, False, 0, 0)),
        ('Watchdog_Uart0State', (1, False, 0, 0)),
        ('Watchdog_Uart1State', (1, False, 0, 0)),
        ('Adc_BatteryTempRaw', (12, True, WD_ADC_BITS, WD_ADC_BITS)),
        ('Watchdog_DetailedHeartbeatSequenceNumber', (8, False, 0, 0)),
        ('Watchdog_DigitalOutputStates', (32, False, 0, 0)),
        ('Watchdog_ResetLogs', (40, False, 0, 0)),
        ('Adc_LanderVoltageRaw', (7, True, 7, WD_ADC_BITS)),
        ('Adc_BatteryChargingTempRaw', (9, True, 9, WD_ADC_BITS)),
        ('Adc_FullSystemVoltageRaw', (5, True, 5, WD_ADC_BITS)),
        ('Adc_FullSystemCurrentRaw', (9, True, WD_ADC_BITS, 9)),
        ('Adc_SwitchedBatteryVoltageRaw', (9, True, 9, WD_ADC_BITS)),
        ('Adc_Vcc24VoltageRaw', (7, True, 7, WD_ADC_BITS)),
        ('Heater_ControlEnabled', (1, False, 0, 0)),
        ('Heater_IsHeating', (1, False, 0, 0)),
        ('Adc_2V5VoltageRaw', (5, True, 5, WD_ADC_BITS)),
        ('Adc_2V8VoltageRaw', (5, True, 5, WD_ADC_BITS)),
        ('Adc_Vcc28VoltageRaw', (6, True, 6, WD_ADC_BITS)),
        ('Heater_Kp', (16, False, 0, 0)),  # Deprecated
        ('Heater_PwmLimit_DutyCycleCounter', (16, False, 0, 0)),  # Deprecated
        ('Heater_SetpointValue', (16, False, 0, 0)),  # Deprecated
        ('Heater_OnValue', (16, False, 0, 0)),
        ('Heater_OffValue', (16, False, 0, 0)),
        ('Heater_DutyCyclePeriodCycles', (16, False, 0, 0)),
        ('Heater_DutyCycleCounter', (16, False, 0, 0)),
        ('I2C_BatteryChargeRaw', (16, False, 0, 0)),
        ('I2C_BatteryVoltageRaw', (16, False, 0, 0)),
        ('I2C_BatteryCurrentRaw', (16, False, 0, 0)),
        ('I2C_FuelGaugeTempRaw', (16, False, 0, 0)),
        ('I2C_BatteryChargeTelemRaw', (8, False, 0, 0)),
        ('I2C_BatteryCurrentTelemRaw', (8, False, 0, 0))
    ])

    # Empty __slots__ allows super's __slots__ to not turn into __dict__:
    __slots__: List[str] = []

    def __repr__(self) -> str:
        return self.custom_payload.__repr__() + f' {hexstr(self.raw)}'

    @ classmethod
    def decode(cls,
               data: bytes,
               endianness_code: str = ENDIANNESS_CODE
               ) -> WatchdogDetailedStatusPacket:
        flag, core_data = data[:1], data[1:]
        if cls.START_FLAG != flag:
            raise PacketDecodingException(
                data,
                "Start flag for `WatchdogDetailedStatusPacket` was invalid. "  # type: ignore
                f"Expected {cls.START_FLAG}, Got: {flag} ."
            )

        # Create empty dict to contain extracted values for later use in destructuring in constructor call:
        named_bitfield_entries = dict([
            (k, 0) for k in cls.BITFIELD_ALLOCATIONS.keys()
        ])

        # Encode message as int and handle endianness:
        data_int: int = int.from_bytes(core_data, 'little', signed=False)

        # Extract all fields from `data_int` and shift adc fields as required:
        idx_bit_head: int = 0
        for field_name, (n_bits, is_adc_val, n_upper_adc_bits, _) in cls.BITFIELD_ALLOCATIONS.items():
            # TODO: consider storing the mask and idx_bit_head for each value after first run for faster processing (build once and store)
            field_mask = ((1 << n_bits) - 1) << idx_bit_head
            field_val = (data_int & field_mask) >> idx_bit_head

            if is_adc_val:
                # Shift back out to 12 bits (if only the upper bits were sent):
                field_val = field_val << (cls.WD_ADC_BITS - n_upper_adc_bits)

            named_bitfield_entries[field_name] = field_val
            idx_bit_head += n_bits

        def flip_endianness(name: str, n_bytes: int) -> None:
            """
            Flips the endianness of the field with the given name whose size is 
            `n_bytes` bytes.
            """
            nonlocal named_bitfield_entries
            old_val = named_bitfield_entries[name]
            new_val = int.from_bytes(
                old_val.to_bytes(n_bytes, 'big'), 'little')
            named_bitfield_entries[name] = new_val

        # TODO: Check if flipping is right thing to do for FuelGauge I2C vals (since they're technically a byte array):
        flip_endianness('I2C_BatteryChargeRaw', 2)
        flip_endianness('I2C_BatteryVoltageRaw', 2)
        flip_endianness('I2C_BatteryCurrentRaw', 2)
        flip_endianness('I2C_FuelGaugeTempRaw', 2)

        custom_payload = WatchdogDetailedStatusPacket.CustomPayload(
            **named_bitfield_entries
        )
        return WatchdogDetailedStatusPacket(
            custom_payload=custom_payload,
            raw=data,
            endianness_code=endianness_code
        )

    def encode(self, **kwargs: Any) -> bytes:
        #! TODO (not really a typical use case so not super necessary besides for completeness)
        #!! TODO: IS NECESSARY FOR IPC (OR JUST ENCODE THAT STUFF IN A STATE) <- Not with new `Packet`-specific `__reduce__` strategy
        raise NotImplementedError()

    @ classmethod
    def is_valid(cls, data: bytes, endianness_code: str = ENDIANNESS_CODE) -> bool:
        """
        Determines whether the given bytes constitute a valid packet of this type.
        """
        correct_start = len(data) > 0 and data[:1] == cls.START_FLAG
        correct_length = len(data) == 47  # Bytes

        return correct_start and correct_length

    @classmethod
    def build_minimum_packet(
        cls,
        payloads: EnhancedPayloadCollection,
        raw: Optional[bytes],
        endianness_code: str
    ) -> WatchdogDetailedStatusPacket:
        """ Builds a minimum representation of this Packet (before any
        additional elements from `__getstate__` are added back). Used by
        `Packet.__reduce__` for unpacking serialized data.
        """
        return cls(
            payloads=payloads,
            raw=raw,
            endianness_code=endianness_code
        )
