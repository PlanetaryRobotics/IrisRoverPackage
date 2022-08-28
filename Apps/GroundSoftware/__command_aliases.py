"""
Shorthand aliases for common ground commands used during testing.
(A quick hack for EMI/EMC testing).

If you want to explore the Data Standards to build new commands, run:
`pyenv exec python datastandards_lookup.py`.

Created: 10/29/2021
Last Update: 08/28/2022
"""
from enum import Enum
from collections import OrderedDict
from typing import Any, Optional, Dict, cast

from IrisBackendv3.codec.payload import CommandPayload
from IrisBackendv3.codec.packet import IrisCommonPacket
from IrisBackendv3.codec.metadata import DataPathway, DataSource
from IrisBackendv3.codec.magic import Magic

source = DataSource.GENERATED

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

prepared_commands = {
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
    ), # Note: there's no way to do just the hercules (you can tell just the hercules but then it'll immediately make watch)
    'deploy-wifi': (
        DataPathway.WIRELESS,
        Magic.COMMAND, # "normal" command is for Hercules
        'WatchDogInterface_DisengageFromLander',
        OrderedDict(confirm='CONFIRM_DEPLOY'),
        DataPathway.WIRELESS
    ),
    'deploy-wd-only': ( # special command to tell only WD to release its interlock (in case Herc-WD comms are broken)
        DataPathway.WIRED,
        Magic.WATCHDOG_COMMAND,
        'WatchDogInterface_DisengageFromLander',
        OrderedDict(confirm='CONFIRM_DEPLOY'),
        DataPathway.WIRELESS
    ),
    'undeploy': (
        DataPathway.WIRED,
        Magic.COMMAND, # "normal" command is for Hercules
        'WatchDogInterface_ResetSpecific',
        OrderedDict(reset_value='HDRM_OFF'),
        DataPathway.WIRED
    ), # Note: Deploy2 (Herc deploy pin) does not undeploy, but WD does.
    'undeploy-2': (
        DataPathway.WIRED,
        Magic.COMMAND, # "normal" command is for Hercules
        'WatchDogInterface_EngageFromLander',
        OrderedDict(),
        DataPathway.WIRED
    ), # Note: Also does not turn off Deploy2
    'reset-herc': (
        DataPathway.WIRED,
        # intentionally telling the WD to tell Herc to tell the WD to enable heater control (same path as deployment command but a quick pretest)
        Magic.WATCHDOG_COMMAND,
        'WatchDogInterface_ResetSpecific',
        # Change this to whatever you want to reset.
        OrderedDict(reset_value='RESET_HERCULES'),
        DataPathway.WIRED
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
        Magic.WATCHDOG_COMMAND, # intentionally telling the WD to tell Herc to tell the WD to enable heater control (same path as deployment command but a quick pretest)
        'WatchDogInterface_ResetSpecific',
        OrderedDict(reset_value='EN_24_ON'), # Change this to whatever you want to reset.
        DataPathway.WIRED
    ),
    '24-off': (
        DataPathway.WIRED,
        Magic.WATCHDOG_COMMAND, # intentionally telling the WD to tell Herc to tell the WD to enable heater control (same path as deployment command but a quick pretest)
        'WatchDogInterface_ResetSpecific',
        OrderedDict(reset_value='EN_24_OFF'), # Change this to whatever you want to reset.
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
        Magic.WATCHDOG_COMMAND, # intentionally telling the WD to tell Herc to tell the WD to enable heater control (same path as deployment command but a quick pretest)
        'WatchDogInterface_ResetSpecific',
        OrderedDict(reset_value='RESET_ALL_MOTORS'), # Change this to whatever you want to reset.
        DataPathway.WIRED
    ),
    'power-on-motors': (
        DataPathway.WIRED,
        Magic.WATCHDOG_COMMAND, # intentionally telling the WD to tell Herc to tell the WD to enable heater control (same path as deployment command but a quick pretest)
        'WatchDogInterface_ResetSpecific',
        OrderedDict(reset_value='ALL_MOTORS_ON'), # Change this to whatever you want to reset.
        DataPathway.WIRED
    ),
    'power-off-motors': (
        DataPathway.WIRED,
        Magic.WATCHDOG_COMMAND, # intentionally telling the WD to tell Herc to tell the WD to enable heater control (same path as deployment command but a quick pretest)
        'WatchDogInterface_ResetSpecific',
        OrderedDict(reset_value='ALL_MOTORS_OFF'), # Change this to whatever you want to reset.
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
        OrderedDict(motor_id=0x00, raw_ticks=20000), # Change this to whatever motor you want to control (0 is all)
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
        OrderedDict(camera_num=0, callback_id=0), # Change this to whatever you want to reset.
        DataPathway.WIRED
    ),
    'take-image-1': (
        DataPathway.WIRED,
        Magic.COMMAND,
        'Camera_TakeImage',
        OrderedDict(camera_num=1, callback_id=0), # Change this to whatever you want to reset.
        DataPathway.WIRED
    ),
    'misc-test': (
        DataPathway.WIRED,
        # intentionally telling the WD to tell Herc to tell the WD to enable heater control (same path as deployment command but a quick pretest)
        Magic.WATCHDOG_COMMAND,
        'WatchDogInterface_ResetSpecific',
        # Change this to whatever you want to reset.
        OrderedDict(reset_value='BATTERY_START_CHARGE'),
        DataPathway.WIRED
    )
}

def get_command(alias: str, params: Optional[Any] = None):
    """
    Grabs command package data (pathway, type, and payload) given a short-hand 
    name (`alias`) and, optionally, a `param` that gets inserted as a command 
    argument where necessary.

    See docstring for `Parameter` for more details about how `params` pasting works.
    """
    pathway, magic, command_name, kwargs, telem_pathway = prepared_commands[alias]

    kwargs = cast(Dict, kwargs)

    # Paste the given parameters anywhere we're told:
    if not isinstance(params, list):
        params = [params]
    param_idx = 0
    for arg_name, arg_val in kwargs.items():
        if arg_val == Parameter.PASTE:
            kwargs[arg_name] = params[param_idx]
            param_idx = (param_idx + 1) % len(params)

    return (pathway, magic, command_name, kwargs, telem_pathway)
