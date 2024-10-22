"""
Aliases for Iris driving commands.

Last Update: 03/10/2023
"""
from typing import Final
from collections import OrderedDict
from IrisBackendv3.codec.magic import Magic
from IrisBackendv3.codec.metadata import DataPathway
from IrisBackendv3.data_standards.data_standards import DataStandards
from IrisBackendv3.config.command_aliases import PreparedCommand, CommandAliases, CommandAliasesBuilder


@CommandAliasesBuilder
def ALIASES(standards: DataStandards) -> CommandAliases:
    return {
        # Nav deprecated
        # 'drive-fwd-200': PreparedCommand(
        #     'Navigation_NavDriveForward',
        #     OrderedDict(distance=200, speed=100, callback_id=0xBEEF),
        #     DataPathway.WIRED,
        #     Magic.COMMAND,
        #     comment="Drive forward 200cm."
        # ),

        # These are changing:
        # 'motor-control-get-telem': PreparedCommand(
        #     'MotorControl_McUpdateTelemetry',
        #     OrderedDict(),
        #     DataPathway.WIRED,
        #     Magic.COMMAND
        # ),
        # # Navigation_NavDriveForward[distance: uint8, speed: uint8, callback_id: uint16]
        # 'motor-control-spin-all': PreparedCommand(
        #     'MotorControl_McSpin',
        #     OrderedDict(motor_id=0x00, raw_ticks=20000),
        #     DataPathway.WIRED,
        #     Magic.COMMAND,
        #     comment="Low-level command to tell all drivers to spin their motors."
        # )
        # New Configurable Motor Control Driving Commands:
        'mc-turn-left-10k': PreparedCommand(  # 'motor-control-spin-configured-left'
            'MotorControl_McSpinConfigured',
            # Bitmask of motor_ids
            # MotorA: 0x01, MotorB: 0x02, MotorC: 0x04, MotorD: 0x08
            # All Motors: 0x0F
            OrderedDict(motor_id=0b1111, dir=0b0000,
                        raw_ticks=10000, percent_speed=100),
            DataPathway.WIRELESS,
            Magic.COMMAND,
            comment="Turn Left for 10k ticks."
        ),
        'mc-turn-right-10k': PreparedCommand(  # 'motor-control-spin-configured-left'
            'MotorControl_McSpinConfigured',
            # Bitmask of motor_ids
            # MotorA: 0x01, MotorB: 0x02, MotorC: 0x04, MotorD: 0x08
            # All Motors: 0x0F
            OrderedDict(motor_id=0b1111, dir=0b1111,
                        raw_ticks=10000, percent_speed=100),
            DataPathway.WIRELESS,
            Magic.COMMAND,
            comment="Turn Right for 10k ticks."
        ),
        'mc-drive-fwd-10k': PreparedCommand(  # motor-control-spin-configured-ad-0-bc-1
            'MotorControl_McSpinConfigured',
            # Bitmask of motor_ids
            # MotorA: 0x01, MotorB: 0x02, MotorC: 0x04, MotorD: 0x08
            # All Motors: 0x0F
            OrderedDict(motor_id=0b1111, dir=0b0110,
                        raw_ticks=10000, percent_speed=100),
            DataPathway.WIRELESS,
            Magic.COMMAND,
            comment="Drive Forward for 10k ticks. All-wheel drive."
        ),
        'mc-drive-fwd-front-drive-10k': PreparedCommand(  # motor-control-spin-configured-ad-0-bc-1
            'MotorControl_McSpinConfigured',
            # Bitmask of motor_ids
            # MotorA: 0x01, MotorB: 0x02, MotorC: 0x04, MotorD: 0x08
            # All Motors: 0x0F
            OrderedDict(motor_id=0b0011, dir=0b0110,
                        raw_ticks=10000, percent_speed=100),
            DataPathway.WIRELESS,
            Magic.COMMAND,
            comment="Drive Forward for 10k ticks. Use only front wheels (if power limiting)."
        ),
        'mc-drive-fwd-rear-drive-10k': PreparedCommand(  # motor-control-spin-configured-ad-0-bc-1
            'MotorControl_McSpinConfigured',
            # Bitmask of motor_ids
            # MotorA: 0x01, MotorB: 0x02, MotorC: 0x04, MotorD: 0x08
            # All Motors: 0x0F
            OrderedDict(motor_id=0b1100, dir=0b0110,
                        raw_ticks=10000, percent_speed=100),
            DataPathway.WIRELESS,
            Magic.COMMAND,
            comment="Drive Forward for 10k ticks. Use only front wheels (if power limiting)."
        ),
        'mc-drive-back-10k': PreparedCommand(  # motor-control-spin-configured-ad-0-bc-1
            'MotorControl_McSpinConfigured',
            # Bitmask of motor_ids
            # MotorA: 0x01, MotorB: 0x02, MotorC: 0x04, MotorD: 0x08
            # All Motors: 0x0F
            OrderedDict(motor_id=0b1111, dir=0b1001,
                        raw_ticks=10000, percent_speed=100),
            DataPathway.WIRELESS,
            Magic.COMMAND,
            comment="Drive Backward for 10k ticks. All-wheel drive."
        ),
        'mc-spin-a-10k': PreparedCommand(  # motor-control-spin-configured-ad-0-bc-1
            'MotorControl_McSpinConfigured',
            # Bitmask of motor_ids
            # MotorA: 0x01, MotorB: 0x02, MotorC: 0x04, MotorD: 0x08
            # All Motors: 0x0F
            OrderedDict(motor_id=0b0001, dir=0b1111,
                        raw_ticks=10000, percent_speed=100),
            DataPathway.WIRELESS,
            Magic.COMMAND,
            comment="Spin Motor A for 10k ticks (in dir=1)."
        )
    }
