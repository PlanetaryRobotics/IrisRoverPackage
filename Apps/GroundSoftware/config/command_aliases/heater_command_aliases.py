"""
Aliases for Iris heater commands.

Last Update: 03/10/2023
"""
from typing import Final
from collections import OrderedDict
from IrisBackendv3.codec.magic import Magic
from IrisBackendv3.codec.metadata import DataPathway
from IrisBackendv3.data_standards.data_standards import DataStandards
from IrisBackendv3.config.command_aliases import PreparedCommand, CommandAliases, CommandAliasesBuilder


def power_level_to_duty_cycle(P: float) -> float:
    """Converts a power percentage `P` to a duty cycle."""
    # TODO: Move this to Iris system config?
    # NOTE: this is actually setting the TB0CCR2 on counter.
    # Percentage Voltage is calculated as DutyCycle/DutyCyclePeriod (TB0CCR0),
    # which this assumes is fixed at 10000 (1.25ms).
    # Note, b/c P = V^2/R, P/Pmax = (V/Vmax)^2, therefore 10% power = 32% voltage = 3162/10000
    duty = round(10000*(P/100)**0.5)
    # Make sure duty stays between >=0 and <=9999
    # (DON'T set to the full value of 10000 (since cutoff is 1 less))
    return min(max(duty, 0), 9999)


@CommandAliasesBuilder
def ALIASES(standards: DataStandards) -> CommandAliases:
    return {
        'disable-heater-control': PreparedCommand(
            'WatchDogInterface_ResetSpecific',
            OrderedDict(reset_value='AUTO_HEATER_CONTROLLER_DISABLE'),
            DataPathway.WIRED,
            Magic.WATCHDOG_COMMAND,
            comment="Turn the heater controller OFF."
        ),
        'enable-heater-control': PreparedCommand(
            'WatchDogInterface_ResetSpecific',
            OrderedDict(reset_value='AUTO_HEATER_CONTROLLER_ENABLE'),
            DataPathway.WIRED,
            Magic.WATCHDOG_COMMAND,
            comment="Turn the heater controller ON."
        ),

        # NOTE: Power levels below 25% (besides 0%) aren't offered by default because:
        # https://github.com/PlanetaryRobotics/IrisRoverPackage/issues/196#issuecomment-1368383399
        # NOTE: this is actually setting the TB0CCR2 on counter.
        # Percentage Voltage is calculated as DutyCycle/DutyCyclePeriod (TB0CCR0),
        **{
            f'set-heater-to-power-{P}': PreparedCommand(
                'WatchDogInterface_SetHeaterDutyCycle',
                OrderedDict(duty=power_level_to_duty_cycle(P)),
                DataPathway.WIRED,
                Magic.WATCHDOG_COMMAND,
                comment=(
                    "Set heater power to {P}% "
                    f"({round(100*power_level_to_duty_cycle(P)/10000):d}%V"
                    f"={power_level_to_duty_cycle(P)})."
                )
            )
            for P in [0, 25, 30, 40, 50, 60, 70, 75, 80, 90, 95, 100]
        },

        # Set the heater's "ON" power level back to its default (8500 duty, which
        # was used during EM3 TVAC). This equates to 72.2% max power or 0.889W at
        # 27.80V.
        'set-heater-to-power-default-em3': PreparedCommand(
            'WatchDogInterface_SetHeaterDutyCycle',
            OrderedDict(duty=8500),
            DataPathway.WIRED,
            Magic.WATCHDOG_COMMAND,
            comment="Set heater power level to default from EM3 TVAC (72%)."
        ),

        'heater-force-off': PreparedCommand(
            'WatchDogInterface_ResetSpecific',
            OrderedDict(reset_value='HEATER_FORCE_OFF'),
            DataPathway.WIRED,
            Magic.WATCHDOG_COMMAND,
            comment="Force the heater to be OFF always no matter what."
        ),
        'heater-force-on': PreparedCommand(
            'WatchDogInterface_ResetSpecific',
            OrderedDict(reset_value='HEATER_FORCE_ON'),
            DataPathway.WIRED,
            Magic.WATCHDOG_COMMAND,
            comment="Force the heater to be ON always no matter what."
        ),
        'heater-force-nothing': PreparedCommand(
            'WatchDogInterface_ResetSpecific',
            OrderedDict(reset_value='HEATER_FORCE_NOTHING'),
            DataPathway.WIRED,
            Magic.WATCHDOG_COMMAND,
            comment="Don't force the heater state (let the controller do it)."
        ),

        'set-heater-on-level': PreparedCommand(
            'WatchDogInterface_SetAutoHeaterOnValue',
            OrderedDict(on=1000),
            DataPathway.WIRED,
            Magic.WATCHDOG_COMMAND,
            comment="Set the heater controller's OFF->ON transition threshold (ADC of NTC)."
        ),
        'set-heater-off-level': PreparedCommand(
            'WatchDogInterface_SetAutoHeaterOffValue',
            OrderedDict(off=500),
            DataPathway.WIRED,
            Magic.WATCHDOG_COMMAND,
            comment="Set the heater controller's ON->OFF transition threshold (ADC of NTC)."
        )
    }
