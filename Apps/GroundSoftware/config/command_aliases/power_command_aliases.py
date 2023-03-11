"""
Aliases for Iris reset and power commands.

Last Update: 03/10/2023
"""
from collections import OrderedDict
from IrisBackendv3.codec.magic import Magic
from IrisBackendv3.codec.metadata import DataPathway
from IrisBackendv3.data_standards.data_standards import DataStandards
from IrisBackendv3.config.command_aliases import PreparedCommand, CommandAliases, CommandAliasesBuilder


@CommandAliasesBuilder
def ALIASES(standards: DataStandards) -> CommandAliases:
    return {
        'power-cycle-vsa': PreparedCommand(
            'WatchDogInterface_ResetSpecific',
            OrderedDict(reset_value='POWER_CYCLE_SYSTEM'),
            DataPathway.WIRED,
            Magic.WATCHDOG_COMMAND,
            comment="Tell WD to turn off VSA then turn it back on."
        ),
        'power-on-system-vsa': PreparedCommand(
            'WatchDogInterface_ResetSpecific',
            OrderedDict(reset_value='SYSTEM_ON'),
            DataPathway.WIRED,
            Magic.WATCHDOG_COMMAND,
            comment="Turn ON VSA (for use in SERVICE)."
        ),
        'power-off-system-vsa': PreparedCommand(
            'WatchDogInterface_ResetSpecific',
            OrderedDict(reset_value='SYSTEM_OFF'),
            DataPathway.WIRED,
            Magic.WATCHDOG_COMMAND,
            comment="Turn OFF VSA (for use in SERVICE)."
        ),
        'reset-herc': PreparedCommand(
            'WatchDogInterface_ResetSpecific',
            OrderedDict(reset_value='RESET_HERCULES'),
            DataPathway.WIRED,
            Magic.WATCHDOG_COMMAND,
            comment="Tell WD to reset Herc."
        ),
        'reset-herc-wireless': PreparedCommand(
            'WatchDogInterface_ResetSpecific',
            OrderedDict(reset_value='RESET_HERCULES'),
            DataPathway.WIRELESS,
            Magic.COMMAND,
            comment="Tell Radio to tell Herc to tell WD to reset Herc."
        ),
        'power-on-herc': PreparedCommand(
            'WatchDogInterface_ResetSpecific',
            OrderedDict(reset_value='HERCULES_POWER_ON'),
            DataPathway.WIRED,
            Magic.WATCHDOG_COMMAND,
            comment="Tell WD to power ON Herc."
        ),
        'power-off-herc': PreparedCommand(
            'WatchDogInterface_ResetSpecific',
            OrderedDict(reset_value='HERCULES_POWER_OFF'),
            DataPathway.WIRED,
            Magic.WATCHDOG_COMMAND,
            comment="Tell WD to power OFF Herc."
        ),
        'reset-radio': PreparedCommand(
            'WatchDogInterface_ResetSpecific',
            OrderedDict(reset_value='RESET_RADIO'),
            DataPathway.WIRED,
            Magic.WATCHDOG_COMMAND,
            comment="Tell WD to reset the Radio."
        ),
        'power-on-radio': PreparedCommand(
            'WatchDogInterface_ResetSpecific',
            OrderedDict(reset_value='RADIO_POWER_ON'),
            DataPathway.WIRED,
            Magic.WATCHDOG_COMMAND,
            comment="Tell WD to power ON the Radio."
        ),
        'power-off-radio': PreparedCommand(
            'WatchDogInterface_ResetSpecific',
            OrderedDict(reset_value='RADIO_POWER_OFF'),
            DataPathway.WIRED,
            Magic.WATCHDOG_COMMAND,
            comment="Tell WD to power OFF the Radio."
        ),
        'reset-fpga': PreparedCommand(
            'WatchDogInterface_ResetSpecific',
            OrderedDict(reset_value='RESET_FPGA'),
            DataPathway.WIRED,
            Magic.WATCHDOG_COMMAND,
            comment="Tell WD to reset the FPGA."
        ),
        'power-on-fpga': PreparedCommand(
            'WatchDogInterface_ResetSpecific',
            OrderedDict(reset_value='FPGA_POWER_ON'),
            DataPathway.WIRED,
            Magic.WATCHDOG_COMMAND,
            comment="Tell WD to power ON the FPGA."
        ),
        'power-off-fpga': PreparedCommand(
            'WatchDogInterface_ResetSpecific',
            OrderedDict(reset_value='FPGA_POWER_OFF'),
            DataPathway.WIRED,
            Magic.WATCHDOG_COMMAND,
            comment="Tell WD to power OFF the FPGA."
        ),
        '3v3-on': PreparedCommand(
            'WatchDogInterface_ResetSpecific',
            OrderedDict(reset_value='EN_3_3_POWER_ON'),
            DataPathway.WIRED,
            Magic.WATCHDOG_COMMAND,
            comment="Tell WD to power ON the 3V3 bus."
        ),
        '3v3-off': PreparedCommand(
            'WatchDogInterface_ResetSpecific',
            OrderedDict(reset_value='EN_3_3_POWER_OFF'),
            DataPathway.WIRED,
            Magic.WATCHDOG_COMMAND,
            comment="Tell WD to power OFF the 3V3 bus."
        ),
        'motors-on': PreparedCommand(
            'WatchDogInterface_ResetSpecific',
            OrderedDict(reset_value='ALL_MOTORS_ON'),
            DataPathway.WIRED,
            Magic.WATCHDOG_COMMAND,
            comment="Tell WD to power ON the 5V and 24V bus for motors & drivers."
        ),
        'motors-off': PreparedCommand(
            'WatchDogInterface_ResetSpecific',
            OrderedDict(reset_value='ALL_MOTORS_OFF'),
            DataPathway.WIRED,
            Magic.WATCHDOG_COMMAND,
            comment="Tell WD to power OFF the 5V and 24V bus for motors & drivers."
        ),
        **{
            f'reset-motor-{i+1}': PreparedCommand(
                'WatchDogInterface_ResetSpecific',
                OrderedDict(reset_value=f'RESET_MOTOR_{i+1}'),
                DataPathway.WIRED,
                Magic.WATCHDOG_COMMAND,
                comment=f"Tell WD to reset motor controller {i+1}."
            )
            for i in range(0, 4)
        },
        'reset-motors-all': PreparedCommand(
            'WatchDogInterface_ResetSpecific',
            OrderedDict(reset_value='RESET_ALL_MOTORS'),
            DataPathway.WIRED,
            Magic.WATCHDOG_COMMAND,
            comment="Tell WD to reset all 4 motor controllers."
        )
    }
