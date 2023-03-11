"""
Aliases for Iris commands between GSW and the Radio.

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
        'radio-echo-hello': PreparedCommand(
            'RadioGround_Echo',
            OrderedDict(text_to_echo='HELLO'),
            DataPathway.WIRELESS,
            Magic.RADIO_COMMAND,
            comment="Directly tell Radio to echo 'HELLO'."
        ),
        'radio-reset-self': PreparedCommand(
            'RadioGround_ResetRadio',
            OrderedDict(confirm_by_typing_RESET='RESET'),
            DataPathway.WIRELESS,
            Magic.RADIO_COMMAND,
            comment="Directly tell Radio to reset itself."
        ),
        # 'radio-enter-stasis': PreparedCommand(
        #     'RadioGround_EnterStasis',
        #     OrderedDict(confirm_by_typing_STASIS='STASIS'),
        #     DataPathway.WIRELESS,
        #     Magic.RADIO_COMMAND,
        #     comment="Directly tell Radio to enter STASIS mode (DEPRECATED)."
        # ),
        # 'radio-exit-stasis': PreparedCommand(
        #     'RadioGround_ExitStasis',
        #     OrderedDict(confirm_by_typing_STASIS='STASIS'),
        #     DataPathway.WIRELESS,
        #     Magic.RADIO_COMMAND,
        #     comment="Directly tell Radio to exit STASIS mode (DEPRECATED)."
        # ),
        'radio-write-testing-to-uart': PreparedCommand(
            'RadioGround_WriteUart',
            OrderedDict(text_to_echo='TESTING...'),
            DataPathway.WIRELESS,
            Magic.RADIO_COMMAND,
            comment="Directly tell Radio to write 'TESTING...' to the Hercules-Radio UART line."
        ),
        'radio-write-send-bad-dm': PreparedCommand(
            'RadioGround_HerculesDm',
            OrderedDict(text_to_echo='faked:STATE'),
            DataPathway.WIRELESS,
            Magic.RADIO_COMMAND,
            comment="Directly tell Radio to write a bad `DirectMessage` to the Hercules-Radio UART line."
        ),
        'radio-write-send-disconnected-dm': PreparedCommand(
            'RadioGround_HerculesDm',
            OrderedDict(text_to_echo='state:INIT'),
            DataPathway.WIRELESS,
            Magic.RADIO_COMMAND,
            comment="Directly tell Radio to write a `state:INIT` `DirectMessage` to the Hercules-Radio UART line (pretending that the radio reset)."
        )
    }
