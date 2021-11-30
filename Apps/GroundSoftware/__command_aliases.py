"""
Shorthand aliases for common ground commands used during testing.
(A quick hack for EMI/EMC testing).

If you want to explore the Data Standards to build new commands, run:
`pyenv exec python datastandards_lookup.py`.

Created: 10/29/2021
Last Update: 10/29/2021
"""
from typing import Optional
from IrisBackendv3.codec.payload import CommandPayload
from IrisBackendv3.codec.packet import IrisCommonPacket
from IrisBackendv3.codec.metadata import DataPathway, DataSource
from IrisBackendv3.codec.magic import Magic

source = DataSource.GENERATED


def get_command(alias: str, param: Optional[str] = None):
    """
    Grabs command package data (pathway, type, and payload) given a short-hand 
    name (`alias`) and, optionally, a `param` that gets inserted as a command 
    argument where necessary.
    """
    prepared_commands = {
        'setup': (  # Tell the Watchdog to switch into service mode
            DataPathway.WIRED,
            Magic.WATCHDOG_COMMAND,
            'WatchDogInterface_SwitchToServiceMode',
            dict(confirm='CONFIRM_SERVICE'),
            DataPathway.WIRED
        ),
        'power-on': (  # Tell the Watchdog to switch into mission mode
            DataPathway.WIRED,
            Magic.WATCHDOG_COMMAND,
            'WatchDogInterface_PrepareForDeployment',
            dict(confirm='CONFIRM_PREP'),
            DataPathway.WIRED
        ),
        'wifi-mode': (  # Command herc to send telem over wifi
            DataPathway.WIRED,
            Magic.COMMAND,  # "normal" command is for Hercules
            'GroundInterface_SetPrimaryInterface',
            dict(primary_interface='WF_121'),
            DataPathway.WIRELESS
        ),
        'wired-mode': (  # Command herc to send telem over uart
            DataPathway.WIRED,
            Magic.COMMAND,  # "normal" command is for Hercules
            'GroundInterface_SetPrimaryInterface',
            dict(primary_interface='WATCHDOG'),
            DataPathway.WIRELESS
        ),
        'deploy': (
            DataPathway.WIRED,
            Magic.COMMAND,  # "normal" command is for Hercules
            'WatchDogInterface_DisengageFromLander',
            dict(confirm='CONFIRM_DEPLOY'),
            DataPathway.WIRED
        ),
        'reset-herc': (
            DataPathway.WIRED,
            # intentionally telling the WD to tell Herc to tell the WD to enable heater control (same path as deployment command but a quick pretest)
            Magic.WATCHDOG_COMMAND,
            'WatchDogInterface_ResetSpecific',
            # Change this to whatever you want to reset.
            dict(reset_value='RESET_HERCULES'),
            DataPathway.WIRED
        ),
        'power-on-herc': (
            DataPathway.WIRED,
            # intentionally telling the WD to tell Herc to tell the WD to enable heater control (same path as deployment command but a quick pretest)
            Magic.WATCHDOG_COMMAND,
            'WatchDogInterface_ResetSpecific',
            # Change this to whatever you want to reset.
            dict(reset_value='HERCULES_POWER_ON'),
            DataPathway.WIRED
        ),
        'power-off-herc': (
            DataPathway.WIRED,
            # intentionally telling the WD to tell Herc to tell the WD to enable heater control (same path as deployment command but a quick pretest)
            Magic.WATCHDOG_COMMAND,
            'WatchDogInterface_ResetSpecific',
            # Change this to whatever you want to reset.
            dict(reset_value='HERCULES_POWER_OFF'),
            DataPathway.WIRED
        ),
        'reset-radio': (
            DataPathway.WIRED,
            # intentionally telling the WD to tell Herc to tell the WD to enable heater control (same path as deployment command but a quick pretest)
            Magic.WATCHDOG_COMMAND,
            'WatchDogInterface_ResetSpecific',
            # Change this to whatever you want to reset.
            dict(reset_value='RESET_RADIO'),
            DataPathway.WIRED
        ),
        'power-on-radio': (
            DataPathway.WIRED,
            # intentionally telling the WD to tell Herc to tell the WD to enable heater control (same path as deployment command but a quick pretest)
            Magic.WATCHDOG_COMMAND,
            'WatchDogInterface_ResetSpecific',
            # Change this to whatever you want to reset.
            dict(reset_value='RADIO_POWER_ON'),
            DataPathway.WIRED
        ),
        'power-off-radio': (
            DataPathway.WIRED,
            # intentionally telling the WD to tell Herc to tell the WD to enable heater control (same path as deployment command but a quick pretest)
            Magic.WATCHDOG_COMMAND,
            'WatchDogInterface_ResetSpecific',
            # Change this to whatever you want to reset.
            dict(reset_value='RADIO_POWER_OFF'),
            DataPathway.WIRED
        ),
        'reset-fpga': (
            DataPathway.WIRED,
            # intentionally telling the WD to tell Herc to tell the WD to enable heater control (same path as deployment command but a quick pretest)
            Magic.WATCHDOG_COMMAND,
            'WatchDogInterface_ResetSpecific',
            # Change this to whatever you want to reset.
            dict(reset_value='RESET_FPGA'),
            DataPathway.WIRED
        ),
        'power-on-fpga': (
            DataPathway.WIRED,
            # intentionally telling the WD to tell Herc to tell the WD to enable heater control (same path as deployment command but a quick pretest)
            Magic.WATCHDOG_COMMAND,
            'WatchDogInterface_ResetSpecific',
            # Change this to whatever you want to reset.
            dict(reset_value='FPGA_POWER_ON'),
            DataPathway.WIRED
        ),
        'power-off-fpga': (
            DataPathway.WIRED,
            # intentionally telling the WD to tell Herc to tell the WD to enable heater control (same path as deployment command but a quick pretest)
            Magic.WATCHDOG_COMMAND,
            'WatchDogInterface_ResetSpecific',
            # Change this to whatever you want to reset.
            dict(reset_value='FPGA_POWER_OFF'),
            DataPathway.WIRED
        ),
        'reset-motors': (
            DataPathway.WIRED,
            # intentionally telling the WD to tell Herc to tell the WD to enable heater control (same path as deployment command but a quick pretest)
            Magic.WATCHDOG_COMMAND,
            'WatchDogInterface_ResetSpecific',
            # Change this to whatever you want to reset.
            dict(reset_value='RESET_ALL_MOTORS'),
            DataPathway.WIRED
        ),
        'power-on-motors': (
            DataPathway.WIRED,
            # intentionally telling the WD to tell Herc to tell the WD to enable heater control (same path as deployment command but a quick pretest)
            Magic.WATCHDOG_COMMAND,
            'WatchDogInterface_ResetSpecific',
            # Change this to whatever you want to reset.
            dict(reset_value='ALL_MOTORS_ON'),
            DataPathway.WIRED
        ),
        'power-off-motors': (
            DataPathway.WIRED,
            # intentionally telling the WD to tell Herc to tell the WD to enable heater control (same path as deployment command but a quick pretest)
            Magic.WATCHDOG_COMMAND,
            'WatchDogInterface_ResetSpecific',
            # Change this to whatever you want to reset.
            dict(reset_value='ALL_MOTORS_OFF'),
            DataPathway.WIRED
        ),
        '3v3-on': (
            DataPathway.WIRED,
            # intentionally telling the WD to tell Herc to tell the WD to enable heater control (same path as deployment command but a quick pretest)
            Magic.WATCHDOG_COMMAND,
            'WatchDogInterface_ResetSpecific',
            # Change this to whatever you want to reset.
            dict(reset_value='EN_3_3_POWER_ON'),
            DataPathway.WIRED
        ),
        '3v3-off': (
            DataPathway.WIRED,
            # intentionally telling the WD to tell Herc to tell the WD to enable heater control (same path as deployment command but a quick pretest)
            Magic.WATCHDOG_COMMAND,
            'WatchDogInterface_ResetSpecific',
            # Change this to whatever you want to reset.
            dict(reset_value='EN_3_3_POWER_OFF'),
            DataPathway.WIRED
        ),
        'vsa-on': (
            DataPathway.WIRED,
            Magic.WATCHDOG_COMMAND,
            'WatchDogInterface_ResetSpecific',
            # Change this to whatever you want to reset.
            dict(reset_value='EN_24_ON'),
            DataPathway.WIRED
        ),
        'vsa-off': (
            DataPathway.WIRED,
            Magic.WATCHDOG_COMMAND,
            'WatchDogInterface_ResetSpecific',
            # Change this to whatever you want to reset.
            dict(reset_value='EN_24_OFF'),
            DataPathway.WIRED
        ),
        'SetChargerEn': (
            DataPathway.WIRED,
            Magic.WATCHDOG_COMMAND,
            'WatchDogInterface_SetChargerEn',
            dict(charge_en=param),
            DataPathway.WIRED
        ),
        'SetChargerPowerConnection': (
            DataPathway.WIRED,
            Magic.WATCHDOG_COMMAND,
            'WatchDogInterface_SetChargerPowerConnection',
            dict(v_lander_reg_en=param),
            DataPathway.WIRED
        ),
        'SetBatteryConnection': (
            DataPathway.WIRED,
            Magic.WATCHDOG_COMMAND,
            'WatchDogInterface_SetBatteryConnection',
            dict(batt_en=param),
            DataPathway.WIRED
        ),
        'SetBatteryControlEnable': (
            DataPathway.WIRED,
            Magic.WATCHDOG_COMMAND,
            'WatchDogInterface_SetBatteryControlEnable',
            dict(batt_ctrl_en=param),
            DataPathway.WIRED
        ),
        'SetBatteryLatch': (
            DataPathway.WIRED,
            Magic.WATCHDOG_COMMAND,
            'WatchDogInterface_SetBatteryLatch',
            dict(latch_batt=param),
            DataPathway.WIRED
        ),
        'SetLatchSet': (
            DataPathway.WIRED,
            Magic.WATCHDOG_COMMAND,
            'WatchDogInterface_SetLatchSet',
            dict(latch_set=param),
            DataPathway.WIRED
        ),
        'SetLatchReset': (
            DataPathway.WIRED,
            Magic.WATCHDOG_COMMAND,
            'WatchDogInterface_SetLatchReset',
            dict(latch_reset=param),
            DataPathway.WIRED
        ),
        'ReportStatus': (
            DataPathway.WIRED,
            Magic.WATCHDOG_COMMAND,
            'WatchDogInterface_RequestStatusReport',
            dict(confirm='CONFIRM_REQUEST'),
            DataPathway.WIRED
        ),
        # Navigation_NavDriveForward[distance: uint8, speed: uint8, callback_id: uint16]
        'drive-fwd-200': (
            DataPathway.WIRED,
            Magic.COMMAND,
            'Navigation_NavDriveForward',
            dict(distance=200, speed=100, callback_id=0xBEEF), # Change this to whatever you want to reset.
            DataPathway.WIRED
        ),
        'misc-test': (
            DataPathway.WIRED,
            # intentionally telling the WD to tell Herc to tell the WD to enable heater control (same path as deployment command but a quick pretest)
            Magic.WATCHDOG_COMMAND,
            'WatchDogInterface_ResetSpecific',
            # Change this to whatever you want to reset.
            dict(reset_value='BATTERY_START_CHARGE'),
            DataPathway.WIRED
        )
    }

    pathway, magic, command_name, kwargs, telem_pathway = prepared_commands[alias]
    return (pathway, magic, command_name, kwargs, telem_pathway)
