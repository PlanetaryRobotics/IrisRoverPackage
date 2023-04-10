"""
Shorthand aliases for common ground commands used during testing.
(A quick hack for EMI/EMC testing).

If you want to explore the Data Standards to build new commands, run:
`pyenv exec python datastandards_lookup.py`.

NOTE: THIS FILE IS DEPRECATED AS OF 03/10/2023. Don't use for new code. Use
`IrisBackendv3/config/command_aliases`.

Created: 10/29/2021
Last Update: 01/01/2023
"""
from __future__ import annotations  # Support things like OrderedDict[A,B]
from enum import Enum
from typing import Any, Optional, Final, Dict, Tuple
from collections import OrderedDict

from IrisBackendv3.codec.payload import CommandPayload
from IrisBackendv3.codec.packet_classes.iris_common import IrisCommonPacket
from IrisBackendv3.codec.metadata import DataPathway, DataSource
from IrisBackendv3.codec.magic import Magic
import IrisBackendv3.codec.bgapi as bgapi
from IrisBackendv3.utils.crc import crc32_fsw

source = DataSource.GENERATED

# Test BGAPI Passthrough data:
BGAPI_GET_MAC_CMD: Final = bgapi.build_command(
    bgapi.BGAPI_WIFI_API, 'config', 'get_mac', {'hw_interface': 0})
BGAPI_GET_MAC_BYTES: Final[bytes] = bgapi.encode_command(
    bgapi.BGAPI_WIFI_ENCODER, BGAPI_GET_MAC_CMD)

BGAPI_WIFI_ON_CMD: Final = bgapi.build_command(
    bgapi.BGAPI_WIFI_API, 'sme', 'wifi_on', dict())
BGAPI_WIFI_ON_BYTES: Final[bytes] = bgapi.encode_command(
    bgapi.BGAPI_WIFI_ENCODER, BGAPI_WIFI_ON_CMD)

BGAPI_START_SCAN_CMD: Final = bgapi.build_command(
    bgapi.BGAPI_WIFI_API, 'sme', 'start_scan', {'hw_interface': 0, 'chList': b''})
# bgapi.BGAPI_WIFI_API, 'sme', 'start_scan', {'hw_interface': 0, 'chList': b'\x0b'})
BGAPI_START_SCAN_BYTES: Final[bytes] = bgapi.encode_command(
    bgapi.BGAPI_WIFI_ENCODER, BGAPI_START_SCAN_CMD)

BGAPI_STOP_SCAN_CMD: Final = bgapi.build_command(
    bgapi.BGAPI_WIFI_API, 'sme', 'stop_scan', dict())
BGAPI_STOP_SCAN_BYTES: Final[bytes] = bgapi.encode_command(
    bgapi.BGAPI_WIFI_ENCODER, BGAPI_STOP_SCAN_CMD)

BGAPI_DFU_ON_CMD: Final = bgapi.build_command(
    bgapi.BGAPI_WIFI_API, 'dfu', 'reset', {'dfu': 1})
BGAPI_DFU_ON_BYTES: Final[bytes] = bgapi.encode_command(
    bgapi.BGAPI_WIFI_ENCODER, BGAPI_DFU_ON_CMD)

BGAPI_DFU_OFF_CMD: Final = bgapi.build_command(
    bgapi.BGAPI_WIFI_API, 'dfu', 'reset', {'dfu': 0})
BGAPI_DFU_OFF_BYTES: Final[bytes] = bgapi.encode_command(
    bgapi.BGAPI_WIFI_ENCODER, BGAPI_DFU_OFF_CMD)

BGAPI_STS10_CMD: Final = bgapi.build_command(
    bgapi.BGAPI_WIFI_API, 'endpoint', 'set_transmit_size', dict(endpoint=3, size=10))
BGAPI_STS10_BYTES: Final[bytes] = bgapi.encode_command(
    bgapi.BGAPI_WIFI_ENCODER, BGAPI_STS10_CMD)

BGAPI_STS0_CMD: Final = bgapi.build_command(
    bgapi.BGAPI_WIFI_API, 'endpoint', 'set_transmit_size', dict(endpoint=3, size=0))
BGAPI_STS0_BYTES: Final[bytes] = bgapi.encode_command(
    bgapi.BGAPI_WIFI_ENCODER, BGAPI_STS0_CMD)

BGAPI_DL5_CMD: Final = bgapi.build_command(
    bgapi.BGAPI_WIFI_API, 'endpoint', 'send', dict(endpoint=3, data=b'hElLo'))
BGAPI_DL5_BYTES: Final[bytes] = bgapi.encode_command(
    bgapi.BGAPI_WIFI_ENCODER, BGAPI_DL5_CMD)


class Parameter(Enum):
    """
    Enum used to indicate that a parameter should be pasted in the
    given argument slot.

    Better said with an example:
    If a command alias has kwargs:
        ```
        OrderedDict(
            'arg1': 42,
            'arg2': Parameter.PASTE,
            'arg3': 'something else'
            'arg4': PARAMETER.PASTE
            'arg5': 0xBEEF
        )
        ```
    and `get_command` is called with `params=[1, 2]` then the command will be built with:
        ```
        args={'arg1': 42,
        'arg2': 1,
        'arg3': 'something else'
        'arg4': 2
        'arg5': 0xBEEF}
        ```
    """
    PASTE = 0


PreparedCommandType = Tuple[
    DataPathway,
    Magic,
    str,
    'OrderedDict[str, Any]',
    DataPathway
]

prepared_commands: Dict[str, PreparedCommandType] = {
    'radio-bgapi-set-transmit-size-10': (
        DataPathway.WIRED,
        Magic.COMMAND,
        'NetworkManager_SendBgApiCommand',
        OrderedDict(
            crc_32=crc32_fsw(BGAPI_STS10_BYTES),
            packet_id=123,
            expect_response='NM_BGAPI_CMD_DONTEXPECTRESPONSE' if BGAPI_STS10_CMD.no_response else 'NM_BGAPI_CMD_EXPECTRESPONSE',
            bgapi_packet=BGAPI_STS10_BYTES
        ),
        DataPathway.WIRED
    ),
    'radio-bgapi-set-transmit-size-0': (
        DataPathway.WIRED,
        Magic.COMMAND,
        'NetworkManager_SendBgApiCommand',
        OrderedDict(
            crc_32=crc32_fsw(BGAPI_STS0_BYTES),
            packet_id=124,
            expect_response='NM_BGAPI_CMD_DONTEXPECTRESPONSE' if BGAPI_STS0_CMD.no_response else 'NM_BGAPI_CMD_EXPECTRESPONSE',
            bgapi_packet=BGAPI_STS0_BYTES
        ),
        DataPathway.WIRED
    ),
    'radio-bgapi-downlink-5-bytes': (
        DataPathway.WIRED,
        Magic.COMMAND,
        'NetworkManager_SendBgApiCommand',
        OrderedDict(
            crc_32=crc32_fsw(BGAPI_DL5_BYTES),
            packet_id=125,
            expect_response='NM_BGAPI_CMD_DONTEXPECTRESPONSE' if BGAPI_DL5_CMD.no_response else 'NM_BGAPI_CMD_EXPECTRESPONSE',
            bgapi_packet=BGAPI_DL5_BYTES
        ),
        DataPathway.WIRED
    ),

    'monitor-herc-on': (  # Tell the Watchdog to switch into service mode
        DataPathway.WIRED,
        Magic.WATCHDOG_COMMAND,
        'WatchDogInterface_ResetSpecific',
        OrderedDict(reset_value='HERCULES_WATCHDOG_ENABLE'),
        DataPathway.WIRED
    ),
    'monitor-herc-off': (
        DataPathway.WIRED,
        Magic.WATCHDOG_COMMAND,
        'WatchDogInterface_ResetSpecific',
        OrderedDict(reset_value='HERCULES_WATCHDOG_DISABLE'),
        DataPathway.WIRED
    ),

    'batt-on': (
        DataPathway.WIRED,
        Magic.WATCHDOG_COMMAND,
        'WatchDogInterface_ResetSpecific',
        OrderedDict(reset_value='ENABLE_BATTERIES'),
        DataPathway.WIRED
    ),
    'batt-off': (
        DataPathway.WIRED,
        Magic.WATCHDOG_COMMAND,
        'WatchDogInterface_ResetSpecific',
        OrderedDict(reset_value='DISABLE_BATTERIES'),
        DataPathway.WIRED
    ),

    'transit': (  # Tell the Watchdog to switch into service mode
        DataPathway.WIRED,
        Magic.WATCHDOG_COMMAND,
        'WatchDogInterface_SwitchToKeepAliveMode',
        OrderedDict(confirm='CONFIRM_ALIVE'),
        DataPathway.WIRED
    ),

    'setup': (  # Tell the Watchdog to switch into service mode
        DataPathway.WIRED,
        Magic.WATCHDOG_COMMAND,
        'WatchDogInterface_SwitchToServiceMode',
        OrderedDict(confirm='CONFIRM_SERVICE'),
        DataPathway.WIRED
    ),
    'power-on': (  # Tell the Watchdog to switch into mission mode
        DataPathway.WIRED,
        Magic.WATCHDOG_COMMAND,
        'WatchDogInterface_PrepareForDeployment',
        OrderedDict(confirm='CONFIRM_PREP'),
        DataPathway.WIRED
    ),
    'wifi-mode': (  # Command herc to send telem over wifi
        DataPathway.WIRELESS,
        Magic.COMMAND,  # "normal" command is for Hercules
        'GroundInterface_SetPrimaryInterface',
        OrderedDict(primary_interface='WF_121'),
        DataPathway.WIRELESS
    ),
    'wifi-mode-wired-send': (  # Command herc to send telem over wifi using a wired command
        DataPathway.WIRED,
        Magic.COMMAND,  # "normal" command is for Hercules
        'GroundInterface_SetPrimaryInterface',
        OrderedDict(primary_interface='WF_121'),
        DataPathway.WIRELESS
    ),
    'wired-mode': (  # Command herc to send telem over uart
        DataPathway.WIRED,
        Magic.COMMAND,  # "normal" command is for Hercules
        'GroundInterface_SetPrimaryInterface',
        OrderedDict(primary_interface='WATCHDOG'),
        DataPathway.WIRELESS
    ),
    'auto-switch-on': (
        DataPathway.WIRED,
        Magic.COMMAND,
        'GroundInterface_SetInterfaceAutoSwitch',
        OrderedDict(on=True),
        DataPathway.WIRED
    ),
    'auto-switch-off': (
        DataPathway.WIRED,
        Magic.COMMAND,
        'GroundInterface_SetInterfaceAutoSwitch',
        OrderedDict(on=False),
        DataPathway.WIRED
    ),
    'deploy': (
        DataPathway.WIRED,
        Magic.COMMAND,  # "normal" command is for Hercules
        'WatchDogInterface_DisengageFromLander',
        OrderedDict(confirm='CONFIRM_DEPLOY'),
        DataPathway.WIRED
    ),  # Note: there's no way to do just the hercules (you can tell just the hercules but then it'll immediately make watch)
    'deploy-wifi': (
        DataPathway.WIRELESS,
        Magic.COMMAND,  # "normal" command is for Hercules
        'WatchDogInterface_DisengageFromLander',
        OrderedDict(confirm='CONFIRM_DEPLOY'),
        DataPathway.WIRELESS
    ),
    'deploy-wd-only': (  # special command to tell only WD to release its interlock (in case Herc-WD comms are broken)
        DataPathway.WIRED,
        Magic.WATCHDOG_COMMAND,
        'WatchDogInterface_DisengageFromLander',
        OrderedDict(confirm='CONFIRM_DEPLOY'),
        DataPathway.WIRELESS
    ),
    'undeploy': (
        DataPathway.WIRED,
        Magic.COMMAND,  # "normal" command is for Hercules
        'WatchDogInterface_ResetSpecific',
        OrderedDict(reset_value='HDRM_OFF'),
        DataPathway.WIRED
    ),  # Note: Deploy2 (Herc deploy pin) does not undeploy, but WD does.
    'undeploy-2': (
        DataPathway.WIRED,
        Magic.COMMAND,  # "normal" command is for Hercules
        'WatchDogInterface_EngageFromLander',
        OrderedDict(),
        DataPathway.WIRED
    ),  # Note: Also does not turn off Deploy2
    'power-on-system-vsa': (  # VSA ON (designed for use in SERVICE)
        DataPathway.WIRED,
        Magic.WATCHDOG_COMMAND,
        'WatchDogInterface_ResetSpecific',
        OrderedDict(reset_value='SYSTEM_ON'),
        DataPathway.WIRED
    ),
    'power-off-system-vsa': (  # VSA OFF (designed for use in SERVICE)
        DataPathway.WIRED,
        Magic.WATCHDOG_COMMAND,
        'WatchDogInterface_ResetSpecific',
        OrderedDict(reset_value='SYSTEM_OFF'),
        DataPathway.WIRED
    ),
    'reset-herc': (
        DataPathway.WIRED,
        # intentionally telling the WD to tell Herc to tell the WD to enable heater control (same path as deployment command but a quick pretest)
        Magic.WATCHDOG_COMMAND,
        'WatchDogInterface_ResetSpecific',
        # Change this to whatever you want to reset.
        OrderedDict(reset_value='RESET_HERCULES'),
        DataPathway.WIRED
    ),
    'reset-herc-wireless': (
        DataPathway.WIRELESS,
        Magic.COMMAND,
        'WatchDogInterface_ResetSpecific',
        # Change this to whatever you want to reset.
        OrderedDict(reset_value='RESET_HERCULES'),
        DataPathway.WIRELESS
    ),
    'power-on-herc': (
        DataPathway.WIRED,
        # intentionally telling the WD to tell Herc to tell the WD to enable heater control (same path as deployment command but a quick pretest)
        Magic.WATCHDOG_COMMAND,
        'WatchDogInterface_ResetSpecific',
        # Change this to whatever you want to reset.
        OrderedDict(reset_value='HERCULES_POWER_ON'),
        DataPathway.WIRED
    ),
    'power-off-herc': (
        DataPathway.WIRED,
        # intentionally telling the WD to tell Herc to tell the WD to enable heater control (same path as deployment command but a quick pretest)
        Magic.WATCHDOG_COMMAND,
        'WatchDogInterface_ResetSpecific',
        # Change this to whatever you want to reset.
        OrderedDict(reset_value='HERCULES_POWER_OFF'),
        DataPathway.WIRED
    ),
    'reset-radio': (
        DataPathway.WIRED,
        # intentionally telling the WD to tell Herc to tell the WD to enable heater control (same path as deployment command but a quick pretest)
        Magic.WATCHDOG_COMMAND,
        'WatchDogInterface_ResetSpecific',
        # Change this to whatever you want to reset.
        OrderedDict(reset_value='RESET_RADIO'),
        DataPathway.WIRED
    ),
    'power-on-radio': (
        DataPathway.WIRED,
        # intentionally telling the WD to tell Herc to tell the WD to enable heater control (same path as deployment command but a quick pretest)
        Magic.WATCHDOG_COMMAND,
        'WatchDogInterface_ResetSpecific',
        # Change this to whatever you want to reset.
        OrderedDict(reset_value='RADIO_POWER_ON'),
        DataPathway.WIRED
    ),
    'power-off-radio': (
        DataPathway.WIRED,
        # intentionally telling the WD to tell Herc to tell the WD to enable heater control (same path as deployment command but a quick pretest)
        Magic.WATCHDOG_COMMAND,
        'WatchDogInterface_ResetSpecific',
        # Change this to whatever you want to reset.
        OrderedDict(reset_value='RADIO_POWER_OFF'),
        DataPathway.WIRED
    ),
    'reset-fpga': (
        DataPathway.WIRED,
        # intentionally telling the WD to tell Herc to tell the WD to enable heater control (same path as deployment command but a quick pretest)
        Magic.WATCHDOG_COMMAND,
        'WatchDogInterface_ResetSpecific',
        # Change this to whatever you want to reset.
        OrderedDict(reset_value='RESET_FPGA'),
        DataPathway.WIRED
    ),
    '3v3-on': (
        DataPathway.WIRED,
        # intentionally telling the WD to tell Herc to tell the WD to enable heater control (same path as deployment command but a quick pretest)
        Magic.WATCHDOG_COMMAND,
        'WatchDogInterface_ResetSpecific',
        # Change this to whatever you want to reset.
        OrderedDict(reset_value='EN_3_3_POWER_ON'),
        DataPathway.WIRED
    ),
    '3v3-off': (
        DataPathway.WIRED,
        # intentionally telling the WD to tell Herc to tell the WD to enable heater control (same path as deployment command but a quick pretest)
        Magic.WATCHDOG_COMMAND,
        'WatchDogInterface_ResetSpecific',
        # Change this to whatever you want to reset.
        OrderedDict(reset_value='EN_3_3_POWER_OFF'),
        DataPathway.WIRED
    ),
    '24-on': (
        DataPathway.WIRED,
        # intentionally telling the WD to tell Herc to tell the WD to enable heater control (same path as deployment command but a quick pretest)
        Magic.WATCHDOG_COMMAND,
        'WatchDogInterface_ResetSpecific',
        # Change this to whatever you want to reset.
        OrderedDict(reset_value='EN_24_ON'),
        DataPathway.WIRED
    ),
    '24-off': (
        DataPathway.WIRED,
        # intentionally telling the WD to tell Herc to tell the WD to enable heater control (same path as deployment command but a quick pretest)
        Magic.WATCHDOG_COMMAND,
        'WatchDogInterface_ResetSpecific',
        # Change this to whatever you want to reset.
        OrderedDict(reset_value='EN_24_OFF'),
        DataPathway.WIRED
    ),
    'SetChargerEn': (
        DataPathway.WIRED,
        Magic.WATCHDOG_COMMAND,
        'WatchDogInterface_SetChargerEnable',
        OrderedDict(charge_en=Parameter.PASTE),
        DataPathway.WIRED
    ),
    'SetChargerPowerConnection': (
        DataPathway.WIRED,
        Magic.WATCHDOG_COMMAND,
        'WatchDogInterface_SetChargerPowerConnection',
        OrderedDict(v_lander_reg_en=Parameter.PASTE),
        DataPathway.WIRED
    ),
    'SetBatteryConnection': (
        DataPathway.WIRED,
        Magic.WATCHDOG_COMMAND,
        'WatchDogInterface_SetBatteryConnection',
        OrderedDict(batt_en=Parameter.PASTE),
        DataPathway.WIRED
    ),
    'batt-en-on': (
        DataPathway.WIRED,
        Magic.WATCHDOG_COMMAND,
        'WatchDogInterface_SetBatteryConnection',
        OrderedDict(batt_en=True),
        DataPathway.WIRED
    ),
    'batt-en-off': (
        DataPathway.WIRED,
        Magic.WATCHDOG_COMMAND,
        'WatchDogInterface_SetBatteryConnection',
        OrderedDict(batt_en=False),
        DataPathway.WIRED
    ),
    'SetBatteryControlEnable': (
        DataPathway.WIRED,
        Magic.WATCHDOG_COMMAND,
        'WatchDogInterface_SetBatteryControlEnable',
        OrderedDict(batt_ctrl_en=Parameter.PASTE),
        DataPathway.WIRED
    ),
    'SetBatteryLatch': (
        DataPathway.WIRED,
        Magic.WATCHDOG_COMMAND,
        'WatchDogInterface_SetBatteryLatch',
        OrderedDict(latch_batt=Parameter.PASTE),
        DataPathway.WIRED
    ),
    'SetLatchSet': (
        DataPathway.WIRED,
        Magic.WATCHDOG_COMMAND,
        'WatchDogInterface_SetLatchSet',
        OrderedDict(latch_set=Parameter.PASTE),
        DataPathway.WIRED
    ),
    'SetLatchReset': (
        DataPathway.WIRED,
        Magic.WATCHDOG_COMMAND,
        'WatchDogInterface_SetLatchReset',
        OrderedDict(latch_reset=Parameter.PASTE),
        DataPathway.WIRED
    ),
    'ReportStatus': (
        DataPathway.WIRED,
        Magic.WATCHDOG_COMMAND,
        'WatchDogInterface_RequestStatusReport',
        OrderedDict(confirm='CONFIRM_REQUEST'),
        DataPathway.WIRED
    ),
    'disable-heater-control-via-herc': (
        DataPathway.WIRED,
        # intentionally telling the WD to tell Herc to tell the WD to enable heater control (same path as deployment command but a quick pretest)
        Magic.COMMAND,
        'WatchDogInterface_ResetSpecific',
        OrderedDict(reset_value='AUTO_HEATER_CONTROLLER_DISABLE'),
        DataPathway.WIRED
    ),
    'disable-heater-control': (
        DataPathway.WIRED,
        Magic.WATCHDOG_COMMAND,
        'WatchDogInterface_ResetSpecific',
        OrderedDict(reset_value='AUTO_HEATER_CONTROLLER_DISABLE'),
        DataPathway.WIRED
    ),
    'enable-heater-control': (
        DataPathway.WIRED,
        Magic.WATCHDOG_COMMAND,
        'WatchDogInterface_ResetSpecific',
        OrderedDict(reset_value='AUTO_HEATER_CONTROLLER_ENABLE'),
        DataPathway.WIRED
    ),

    # Set the heater's "ON" power level to be 10%:
    # NOTE: this is actually setting the TB0CCR2 on counter.
    # Percentage Voltage is calculated as DutyCycle/DutyCyclePeriod (TB0CCR0),
    # which this assumes is fixed at 10000 (1.25ms).
    # Note, b/c P = V^2/R, P/Pmax = (V/Vmax)^2, therefore 10% power = 32% voltage = 3162/10000
    'set-heater-to-power-10': (
        DataPathway.WIRED,
        Magic.WATCHDOG_COMMAND,
        'WatchDogInterface_SetHeaterDutyCycle',
        OrderedDict(duty=3162),
        DataPathway.WIRED
    ),
    # Set the heater's "ON" power level to be 20%:
    # b/c P/Pmax = (V/Vmax)^2, 20% power = 45% voltage = 4472/10000
    'set-heater-to-power-20': (
        DataPathway.WIRED,
        Magic.WATCHDOG_COMMAND,
        'WatchDogInterface_SetHeaterDutyCycle',
        OrderedDict(duty=4472),
        DataPathway.WIRED
    ),
    # Set the heater's "ON" power level to be 25%:
    # b/c P/Pmax = (V/Vmax)^2, 25% power = 50% voltage = 5000/10000
    'set-heater-to-power-25': (
        DataPathway.WIRED,
        Magic.WATCHDOG_COMMAND,
        'WatchDogInterface_SetHeaterDutyCycle',
        OrderedDict(duty=5000),
        DataPathway.WIRED
    ),
    # Set the heater's "ON" power level to be 30%:
    # b/c P/Pmax = (V/Vmax)^2, 30% power = 55% voltage = 5477/10000
    'set-heater-to-power-30': (
        DataPathway.WIRED,
        Magic.WATCHDOG_COMMAND,
        'WatchDogInterface_SetHeaterDutyCycle',
        OrderedDict(duty=5477),
        DataPathway.WIRED
    ),
    # Set the heater's "ON" power level to be 40%:
    # b/c P/Pmax = (V/Vmax)^2, 40% power = 63% voltage = 6325/10000
    'set-heater-to-power-40': (
        DataPathway.WIRED,
        Magic.WATCHDOG_COMMAND,
        'WatchDogInterface_SetHeaterDutyCycle',
        OrderedDict(duty=6325),
        DataPathway.WIRED
    ),
    # Set the heater's "ON" power level to be 50%:
    # b/c P/Pmax = (V/Vmax)^2, 50% power = 71% voltage = 7071/10000
    'set-heater-to-power-50': (
        DataPathway.WIRED,
        Magic.WATCHDOG_COMMAND,
        'WatchDogInterface_SetHeaterDutyCycle',
        OrderedDict(duty=7071),
        DataPathway.WIRED
    ),
    # Set the heater's "ON" power level to be 60%:
    # b/c P/Pmax = (V/Vmax)^2, 60% power = 77% voltage = 7746/10000
    'set-heater-to-power-60': (
        DataPathway.WIRED,
        Magic.WATCHDOG_COMMAND,
        'WatchDogInterface_SetHeaterDutyCycle',
        OrderedDict(duty=7746),
        DataPathway.WIRED
    ),
    # Set the heater's "ON" power level to be 70%:
    # b/c P/Pmax = (V/Vmax)^2, 70% power = 84% voltage = 8367/10000
    'set-heater-to-power-70': (
        DataPathway.WIRED,
        Magic.WATCHDOG_COMMAND,
        'WatchDogInterface_SetHeaterDutyCycle',
        OrderedDict(duty=8367),
        DataPathway.WIRED
    ),
    # Set the heater's "ON" power level to be 75%:
    # b/c P/Pmax = (V/Vmax)^2, 75% power = 87% voltage = 8660/10000
    'set-heater-to-power-75': (
        DataPathway.WIRED,
        Magic.WATCHDOG_COMMAND,
        'WatchDogInterface_SetHeaterDutyCycle',
        OrderedDict(duty=8660),
        DataPathway.WIRED
    ),
    # Set the heater's "ON" power level to be 80%:
    # b/c P/Pmax = (V/Vmax)^2, 80% power = 89% voltage = 8944/10000
    'set-heater-to-power-80': (
        DataPathway.WIRED,
        Magic.WATCHDOG_COMMAND,
        'WatchDogInterface_SetHeaterDutyCycle',
        OrderedDict(duty=8944),
        DataPathway.WIRED
    ),
    # Set the heater's "ON" power level to be 90%:
    # b/c P/Pmax = (V/Vmax)^2, 90% power = 89% voltage = 9486/10000
    'set-heater-to-power-90': (
        DataPathway.WIRED,
        Magic.WATCHDOG_COMMAND,
        'WatchDogInterface_SetHeaterDutyCycle',
        OrderedDict(duty=9486),
        DataPathway.WIRED
    ),
    # Set the heater's "ON" power level to be 95%:
    # b/c P/Pmax = (V/Vmax)^2, 95% power = 97% voltage = 9746/10000
    'set-heater-to-power-95': (
        DataPathway.WIRED,
        Magic.WATCHDOG_COMMAND,
        'WatchDogInterface_SetHeaterDutyCycle',
        OrderedDict(duty=9746),
        DataPathway.WIRED
    ),
    # Set the heater's "ON" power level to be 100%:
    'set-heater-to-power-100': (
        DataPathway.WIRED,
        Magic.WATCHDOG_COMMAND,
        'WatchDogInterface_SetHeaterDutyCycle',
        # DON'T set to the full value of 10000 (since cutoff is 1 less)
        OrderedDict(duty=9999),
        DataPathway.WIRED
    ),
    # Set the heater's "ON" power level to be 0% (basically, always off no
    # matter what):
    'set-heater-to-power-0': (
        DataPathway.WIRED,
        Magic.WATCHDOG_COMMAND,
        'WatchDogInterface_SetHeaterDutyCycle',
        # DON'T set to the full value of 10000 (since cutoff is 1 less)
        OrderedDict(duty=0),
        DataPathway.WIRED
    ),
    # Set the heater's "ON" power level back to its default (8500 duty, which
    # was used during EM3 TVAC). This equates to 72.2% max power or 0.889W at
    # 27.80V.
    'set-heater-to-power-default': (
        DataPathway.WIRED,
        Magic.WATCHDOG_COMMAND,
        'WatchDogInterface_SetHeaterDutyCycle',
        # DON'T set to the full value of 10000 (since cutoff is 1 less)
        OrderedDict(duty=8500),
        DataPathway.WIRED
    ),

    'heater-force-off': (
        DataPathway.WIRED,
        Magic.WATCHDOG_COMMAND,
        'WatchDogInterface_ResetSpecific',
        OrderedDict(reset_value='HEATER_FORCE_OFF'),
        DataPathway.WIRED
    ),
    'heater-force-on': (
        DataPathway.WIRED,
        Magic.WATCHDOG_COMMAND,
        'WatchDogInterface_ResetSpecific',
        OrderedDict(reset_value='HEATER_FORCE_ON'),
        DataPathway.WIRED
    ),
    'heater-force-nothing': (
        DataPathway.WIRED,
        Magic.WATCHDOG_COMMAND,
        'WatchDogInterface_ResetSpecific',
        OrderedDict(reset_value='HEATER_FORCE_NOTHING'),
        DataPathway.WIRED
    ),

    'set-heater-on-level': (
        DataPathway.WIRED,
        Magic.WATCHDOG_COMMAND,
        'WatchDogInterface_SetAutoHeaterOnValue',
        OrderedDict(on=1000),
        DataPathway.WIRED
    ),
    'set-heater-off-level': (
        DataPathway.WIRED,
        Magic.WATCHDOG_COMMAND,
        'WatchDogInterface_SetAutoHeaterOffValue',
        OrderedDict(off=500),
        DataPathway.WIRED
    ),

    'reset-motors': (
        DataPathway.WIRED,
        Magic.WATCHDOG_COMMAND,
        'WatchDogInterface_ResetSpecific',
        OrderedDict(reset_value='RESET_ALL_MOTORS'),
        DataPathway.WIRED
    ),
    'power-on-motors': (
        DataPathway.WIRED,
        Magic.WATCHDOG_COMMAND,
        'WatchDogInterface_ResetSpecific',
        OrderedDict(reset_value='ALL_MOTORS_ON'),
        DataPathway.WIRED
    ),
    'power-off-motors': (
        DataPathway.WIRED,
        Magic.WATCHDOG_COMMAND,
        'WatchDogInterface_ResetSpecific',
        OrderedDict(reset_value='ALL_MOTORS_OFF'),
        DataPathway.WIRED
    ),

    'reset-motors-herc': (
        DataPathway.WIRED,
        # intentionally telling the WD to tell Herc to tell the WD
        Magic.COMMAND,
        'WatchDogInterface_ResetSpecific',
        OrderedDict(reset_value='RESET_ALL_MOTORS'),
        DataPathway.WIRED
    ),
    'power-on-motors-herc': (
        DataPathway.WIRED,
        # intentionally telling the WD to tell Herc to tell the WD
        Magic.COMMAND,
        'WatchDogInterface_ResetSpecific',
        OrderedDict(reset_value='ALL_MOTORS_ON'),
        DataPathway.WIRED
    ),
    'power-off-motors-herc': (
        DataPathway.WIRED,
        # intentionally telling the WD to tell Herc to tell the WD
        Magic.COMMAND,
        'WatchDogInterface_ResetSpecific',
        OrderedDict(reset_value='ALL_MOTORS_OFF'),
        DataPathway.WIRED
    ),

    'reset-motors-wifi': (
        DataPathway.WIRELESS,
        Magic.COMMAND,
        'WatchDogInterface_ResetSpecific',
        OrderedDict(reset_value='RESET_ALL_MOTORS'),
        DataPathway.WIRELESS
    ),
    'power-on-motors-wifi': (
        DataPathway.WIRELESS,
        Magic.COMMAND,
        'WatchDogInterface_ResetSpecific',
        OrderedDict(reset_value='ALL_MOTORS_ON'),
        DataPathway.WIRELESS
    ),
    'power-off-motors-wifi': (
        DataPathway.WIRELESS,
        Magic.COMMAND,
        'WatchDogInterface_ResetSpecific',
        OrderedDict(reset_value='ALL_MOTORS_OFF'),
        DataPathway.WIRELESS
    ),


    # Navigation_NavDriveForward[distance: uint8, speed: uint8, callback_id: uint16]
    'drive-fwd-200': (
        DataPathway.WIRED,
        Magic.COMMAND,
        'Navigation_NavDriveForward',
        OrderedDict(distance=200, speed=100, callback_id=0xBEEF),
        DataPathway.WIRED
    ),
    # Navigation_NavDriveForward[distance: uint8, speed: uint8, callback_id: uint16]
    'motor-control-get-telem': (
        DataPathway.WIRED,
        Magic.COMMAND,
        'MotorControl_McUpdateTelemetry',
        OrderedDict(),
        DataPathway.WIRED
    ),
    # Navigation_NavDriveForward[distance: uint8, speed: uint8, callback_id: uint16]
    'motor-control-spin-all': (
        DataPathway.WIRED,
        Magic.COMMAND,
        'MotorControl_McSpin',
        # Change this to whatever motor you want to control (0 is all)
        OrderedDict(motor_id=0x00, raw_ticks=20000),
        DataPathway.WIRED
    ),
    'herc-wired-noop': (
        DataPathway.WIRED,
        Magic.COMMAND,
        'CommandDispatcher_Cmdnoop',
        OrderedDict(),
        DataPathway.WIRED
    ),
    # WD Select Camera
    'select-cam-0': (
        DataPathway.WIRED,
        Magic.WATCHDOG_COMMAND,
        'WatchDogInterface_ResetSpecific',
        OrderedDict(reset_value='FPGA_CAM_0'),
        DataPathway.WIRED
    ),
    'select-cam-1': (
        DataPathway.WIRED,
        Magic.WATCHDOG_COMMAND,
        'WatchDogInterface_ResetSpecific',
        OrderedDict(reset_value='FPGA_CAM_1'),
        DataPathway.WIRED
    ),
    # Herc Take Image
    'take-image-0': (
        DataPathway.WIRED,
        Magic.COMMAND,
        'Camera_TakeImage',
        # Change this to whatever you want to reset.
        OrderedDict(camera_num=0, callback_id=0),
        DataPathway.WIRED
    ),
    'take-image-1': (
        DataPathway.WIRED,
        Magic.COMMAND,
        'Camera_TakeImage',
        # Change this to whatever you want to reset.
        OrderedDict(camera_num=1, callback_id=0),
        DataPathway.WIRED
    ),
    'wd-echo-hi-watchdog': (
        DataPathway.WIRED,
        Magic.WATCHDOG_COMMAND,
        'WatchDogInterface_Echo',
        # Change this to whatever you want to reset.
        OrderedDict(length='10', message="HiWatchdog"),
        DataPathway.WIRED
    ),
    'misc-test': (
        DataPathway.WIRED,
        Magic.WATCHDOG_COMMAND,
        'WatchDogInterface_ResetSpecific',
        # Change this to whatever you want to reset.
        OrderedDict(reset_value='BATTERY_START_CHARGE'),
        DataPathway.WIRED
    ),

    # Test FPrime Echo Command (a CommandDispatcher No-op):
    # (should see this echoed back in a log - good for testing bidirectional
    # FPrime string encoding/decoding):
    'herc-cmd-echo': (
        DataPathway.WIRED,
        Magic.COMMAND,
        'CommandDispatcher_Cmdnoopstring',
        # enable filter entry to disable that log ID:
        OrderedDict(arg_1='Hello Command Dispatcher!'),
        DataPathway.WIRED
    ),

    # Suppress (filter out) GiDownlinkedPacket logs (ID <0x0903>):
    'active-logger-suppress-GiDownlinkedPacket': (
        DataPathway.WIRED,
        Magic.COMMAND,
        'ActiveLogger_Alogsetidfilter',
        # enable filter entry to disable that log ID:
        OrderedDict(id=0x0903, id_filter_enable='IDENABLED'),
        DataPathway.WIRED
    ),
    # Re-enable (don't filter out) GiDownlinkedPacket logs (ID <0x0903>):
    'active-logger-enable-GiDownlinkedPacket': (
        DataPathway.WIRED,
        Magic.COMMAND,
        'ActiveLogger_Alogsetidfilter',
        # disable/remove filter entry to re-enable that log ID:
        OrderedDict(id=0x0903, id_filter_enable='IDDISABLED'),
        DataPathway.WIRED
    ),

    # Re-enable (don't filter out) GiDownlinkedPacket logs (ID <0x0903>):
    'active-logger-dump-filter-state': (
        DataPathway.WIRED,
        Magic.COMMAND,
        'ActiveLogger_Alogdumpfilterstate',
        OrderedDict(),
        DataPathway.WIRED
    ),

    'radio-bgapi-passthru-on': (
        DataPathway.WIRED,
        Magic.COMMAND,
        'NetworkManager_SetRadioBgApiPassthrough',
        OrderedDict(passthrough=True),
        DataPathway.WIRED
    ),
    'radio-bgapi-passthru-off': (
        DataPathway.WIRED,
        Magic.COMMAND,
        'NetworkManager_SetRadioBgApiPassthrough',
        OrderedDict(passthrough=False),
        DataPathway.WIRED
    ),
    # Triggers a `RadioBgApiCommandRecords` event to see what BgApi
    # packets have been processed recently and what the outcomes were.
    'radio-bgapi-passthru-cmd-dump': (
        DataPathway.WIRED,
        Magic.COMMAND,
        'NetworkManager_DownlinkBgApiCommandRecords',
        OrderedDict(),
        DataPathway.WIRED
    ),

    # Sends a BGAPI command to the Radio to get the MAC address (basic test
    # command):
    'radio-bgapi-passthru-get-mac': (
        DataPathway.WIRED,
        Magic.COMMAND,
        'NetworkManager_SendBgApiCommand',
        OrderedDict(
            crc_32=crc32_fsw(BGAPI_GET_MAC_BYTES),
            packet_id=1234,
            expect_response='NM_BGAPI_CMD_DONTEXPECTRESPONSE' if BGAPI_GET_MAC_CMD.no_response else 'NM_BGAPI_CMD_EXPECTRESPONSE',
            bgapi_packet=BGAPI_GET_MAC_BYTES
        ),
        DataPathway.WIRED
    ),
    # Sends a BGAPI command to the Radio to get the MAC address (basic test
    # command) but with an intentionally incorrect CRC:
    'radio-bgapi-passthru-get-mac--bad-crc': (
        DataPathway.WIRED,
        Magic.COMMAND,
        'NetworkManager_SendBgApiCommand',
        OrderedDict(
            crc_32=crc32_fsw(BGAPI_GET_MAC_BYTES)-1,
            packet_id=1234,
            expect_response='NM_BGAPI_CMD_DONTEXPECTRESPONSE' if BGAPI_GET_MAC_CMD.no_response else 'NM_BGAPI_CMD_EXPECTRESPONSE',
            bgapi_packet=BGAPI_GET_MAC_BYTES
        ),
        DataPathway.WIRED
    ),

    # Some basic diagnostic Radio passthru commands:
    'radio-bgapi-passthru-wifi-on': (
        DataPathway.WIRED,
        Magic.COMMAND,
        'NetworkManager_SendBgApiCommand',
        OrderedDict(
            crc_32=crc32_fsw(BGAPI_WIFI_ON_BYTES),
            packet_id=1235,
            expect_response='NM_BGAPI_CMD_DONTEXPECTRESPONSE' if BGAPI_WIFI_ON_CMD.no_response else 'NM_BGAPI_CMD_EXPECTRESPONSE',
            bgapi_packet=BGAPI_WIFI_ON_BYTES
        ),
        DataPathway.WIRED
    ),

    'radio-bgapi-passthru-scan': (
        DataPathway.WIRED,
        Magic.COMMAND,
        'NetworkManager_SendBgApiCommand',
        OrderedDict(
            crc_32=crc32_fsw(BGAPI_START_SCAN_BYTES),
            packet_id=1236,
            expect_response='NM_BGAPI_CMD_DONTEXPECTRESPONSE' if BGAPI_START_SCAN_CMD.no_response else 'NM_BGAPI_CMD_EXPECTRESPONSE',
            bgapi_packet=BGAPI_START_SCAN_BYTES
        ),
        DataPathway.WIRED
    ),
    'radio-bgapi-passthru-scan-stop': (
        DataPathway.WIRED,
        Magic.COMMAND,
        'NetworkManager_SendBgApiCommand',
        OrderedDict(
            crc_32=crc32_fsw(BGAPI_STOP_SCAN_BYTES),
            packet_id=1237,
            expect_response='NM_BGAPI_CMD_DONTEXPECTRESPONSE' if BGAPI_STOP_SCAN_CMD.no_response else 'NM_BGAPI_CMD_EXPECTRESPONSE',
            bgapi_packet=BGAPI_STOP_SCAN_BYTES
        ),
        DataPathway.WIRED
    ),


    'radio-bgapi-passthru-dfu-on': (
        DataPathway.WIRED,
        Magic.COMMAND,
        'NetworkManager_SendBgApiCommand',
        OrderedDict(
            crc_32=crc32_fsw(BGAPI_DFU_ON_BYTES),
            packet_id=4321,
            expect_response='NM_BGAPI_CMD_DONTEXPECTRESPONSE' if BGAPI_DFU_ON_CMD.no_response else 'NM_BGAPI_CMD_EXPECTRESPONSE',
            bgapi_packet=BGAPI_DFU_ON_BYTES
        ),
        DataPathway.WIRED
    ),
    'radio-bgapi-passthru-dfu-off': (
        DataPathway.WIRED,
        Magic.COMMAND,
        'NetworkManager_SendBgApiCommand',
        OrderedDict(
            crc_32=crc32_fsw(BGAPI_DFU_OFF_BYTES),
            packet_id=4320,
            expect_response='NM_BGAPI_CMD_DONTEXPECTRESPONSE' if BGAPI_DFU_OFF_CMD.no_response else 'NM_BGAPI_CMD_EXPECTRESPONSE',
            bgapi_packet=BGAPI_DFU_OFF_BYTES
        ),
        DataPathway.WIRED
    ),


    # Turn off WARNING_HI logs (e.g. ImuAngleWarning spamming us)
    'active-logger-turn-off-warning-hi': (
        DataPathway.WIRED,
        Magic.COMMAND,
        'ActiveLogger_Alogseteventreportfilter',
        OrderedDict(filter_level='INPUTWARNINGHI',
                    filter_enable='INPUTDISABLED'),
        DataPathway.WIRED
    ),

    # Turn off Hercules-Radio UART (inside the Hercules) so an external device
    # can use it (for Radio diagnostics and programming).
    'radio-comms-ext-mode': (
        DataPathway.WIRED,
        Magic.COMMAND,
        'NetworkManager_SetRadioCommunicationsMode',
        OrderedDict(mode='EXTERNAL'),
        DataPathway.WIRED
    ),
    # Turn on Hercules-Radio UART (inside the Hercules) so Hercules can use it
    # (normal operation). If this doesn't work, just do a `reset-herc`.
    'radio-comms-normal-mode': (
        DataPathway.WIRED,
        Magic.COMMAND,
        'NetworkManager_SetRadioCommunicationsMode',
        OrderedDict(mode='HERCULES'),
        DataPathway.WIRED
    ),
    'radio-echo-hello': (
        DataPathway.WIRELESS,
        Magic.RADIO_COMMAND,
        'RadioGround_Echo',
        OrderedDict(text_to_echo='HELLO'),
        DataPathway.WIRELESS
    ),
    'radio-reset-self': (
        DataPathway.WIRELESS,
        Magic.RADIO_COMMAND,
        'RadioGround_ResetRadio',
        OrderedDict(confirm_by_typing_RESET='RESET'),
        DataPathway.WIRELESS
    ),
    'radio-enter-stasis': (
        DataPathway.WIRELESS,
        Magic.RADIO_COMMAND,
        'RadioGround_EnterStasis',
        OrderedDict(confirm_by_typing_STASIS='STASIS'),
        DataPathway.WIRELESS
    ),
    'radio-exit-stasis': (
        DataPathway.WIRELESS,
        Magic.RADIO_COMMAND,
        'RadioGround_ExitStasis',
        OrderedDict(confirm_by_typing_STASIS='STASIS'),
        DataPathway.WIRELESS
    ),
    'radio-write-testing-to-uart': (
        DataPathway.WIRELESS,
        Magic.RADIO_COMMAND,
        'RadioGround_WriteUart',
        OrderedDict(text_to_echo='TESTING...'),
        DataPathway.WIRELESS
    ),
    'radio-write-send-bad-dm': (
        DataPathway.WIRELESS,
        Magic.RADIO_COMMAND,
        'RadioGround_HerculesDm',
        OrderedDict(text_to_echo='faked:STATE'),
        DataPathway.WIRELESS
    ),
    'radio-write-send-disconnected-dm': (
        DataPathway.WIRELESS,
        Magic.RADIO_COMMAND,
        'RadioGround_HerculesDm',
        OrderedDict(text_to_echo='state:INIT'),
        DataPathway.WIRELESS
    ),

    'roll-credits': (
        DataPathway.WIRED,
        Magic.COMMAND,
        'GroundInterface_RollCredits',
        OrderedDict(on=True),
        DataPathway.WIRED
    ),
    'names-start': (
        DataPathway.WIRED,
        Magic.COMMAND,
        'GroundInterface_RollCredits',
        OrderedDict(on=True),
        DataPathway.WIRED
    ),
    'names-stop': (
        DataPathway.WIRED,
        Magic.COMMAND,
        'GroundInterface_RollCredits',
        OrderedDict(on=False),
        DataPathway.WIRED
    ),
    'names-1s': (
        DataPathway.WIRED,
        Magic.COMMAND,
        'GroundInterface_SetNameAndMessagePeriod',
        OrderedDict(seconds=1),
        DataPathway.WIRED
    ),
    'names-5s': (
        DataPathway.WIRED,
        Magic.COMMAND,
        'GroundInterface_SetNameAndMessagePeriod',
        OrderedDict(seconds=5),
        DataPathway.WIRED
    ),
    'names-10s': (
        DataPathway.WIRED,
        Magic.COMMAND,
        'GroundInterface_SetNameAndMessagePeriod',
        OrderedDict(seconds=10),
        DataPathway.WIRED
    )
}


def get_command(alias: str, params: Optional[Any] = None) -> PreparedCommandType:
    """
    Grabs command package data (pathway, type, and payload) given a short-hand
    name (`alias`) and, optionally, a `param` that gets inserted as a command
    argument where necessary.

    See docstring for `Parameter` for more details about how `params` pasting works.
    """
    pathway, magic, command_name, kwargs, telem_pathway = prepared_commands[alias]

    # Paste the given parameters anywhere we're told:
    if not isinstance(params, list):
        params = [params]
    param_idx = 0
    for arg_name, arg_val in kwargs.items():
        if arg_val == Parameter.PASTE:
            kwargs[arg_name] = params[param_idx]
            param_idx = (param_idx + 1) % len(params)

    return (pathway, magic, command_name, kwargs, telem_pathway)
