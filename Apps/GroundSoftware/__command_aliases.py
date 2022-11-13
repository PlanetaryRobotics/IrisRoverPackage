"""
Shorthand aliases for common ground commands used during testing.
(A quick hack for EMI/EMC testing).

If you want to explore the Data Standards to build new commands, run:
`pyenv exec python datastandards_lookup.py`.

Created: 10/29/2021
Last Update: 11/12/2022
"""
from __future__ import annotations  # Support things like OrderedDict[A,B]
from enum import Enum
from typing import Any, Optional, Final, Dict, Tuple
from collections import OrderedDict

from IrisBackendv3.codec.payload import CommandPayload
from IrisBackendv3.codec.packet import IrisCommonPacket
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
    'disable-heater-control': (
        DataPathway.WIRED,
        # intentionally telling the WD to tell Herc to tell the WD to enable heater control (same path as deployment command but a quick pretest)
        Magic.WATCHDOG_COMMAND,
        'WatchDogInterface_ResetSpecific',
        # Change this to whatever you want to reset.
        OrderedDict(reset_value='DISABLE_HEATER_CONTROL'),
        DataPathway.WIRED
    ),

    'reset-motors': (
        DataPathway.WIRED,
        # intentionally telling the WD to tell Herc to tell the WD to enable heater control (same path as deployment command but a quick pretest)
        Magic.WATCHDOG_COMMAND,
        'WatchDogInterface_ResetSpecific',
        # Change this to whatever you want to reset.
        OrderedDict(reset_value='RESET_ALL_MOTORS'),
        DataPathway.WIRED
    ),
    'power-on-motors': (
        DataPathway.WIRED,
        # intentionally telling the WD to tell Herc to tell the WD to enable heater control (same path as deployment command but a quick pretest)
        Magic.WATCHDOG_COMMAND,
        'WatchDogInterface_ResetSpecific',
        # Change this to whatever you want to reset.
        OrderedDict(reset_value='ALL_MOTORS_ON'),
        DataPathway.WIRED
    ),
    'power-off-motors': (
        DataPathway.WIRED,
        # intentionally telling the WD to tell Herc to tell the WD to enable heater control (same path as deployment command but a quick pretest)
        Magic.WATCHDOG_COMMAND,
        'WatchDogInterface_ResetSpecific',
        # Change this to whatever you want to reset.
        OrderedDict(reset_value='ALL_MOTORS_OFF'),
        DataPathway.WIRED
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
