"""
Aliases for Iris diagnostic commands.

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
    GI_module = standards.modules['GroundInterface']

    return {
        'herc-wired-noop': PreparedCommand(
            'CommandDispatcher_Cmdnoop',
            OrderedDict(),
            DataPathway.WIRED,
            Magic.COMMAND,
            comment="Diagnostic command to tell Herc to do nothing (and return a cmd resp)."
        ),
        # Test FPrime Echo Command (a CommandDispatcher No-op):
        # (should see this echoed back in a log - good for testing bidirectional
        # FPrime string encoding/decoding):
        'herc-cmd-echo': PreparedCommand(
            'CommandDispatcher_Cmdnoopstring',
            OrderedDict(arg_1='Hello Command Dispatcher!'),
            DataPathway.WIRED,
            Magic.COMMAND,
            comment="Tell Herc CmdDisp to echo 'Hello Command Dispatcher!'"
        ),
        'wd-echo-hi-watchdog': PreparedCommand(
            'WatchDogInterface_Echo',
            OrderedDict(length='10', message="HiWatchdog"),
            DataPathway.WIRED,
            Magic.WATCHDOG_COMMAND,
            comment="Command to tell the WD to Echo a string (might be broken)."
        ),

        # Suppress (filter out) GiDownlinkedPacket logs (ID <0x0903>):
        'active-logger-suppress-GiDownlinkedPacket': PreparedCommand(
            'ActiveLogger_Alogsetidfilter',
            OrderedDict(
                id=(GI_module.ID | GI_module.events['GiDownlinkedPacket'].ID),
                id_filter_enable='IDENABLED'  # filter enabled = log suppressed
            ),
            DataPathway.WIRED,
            Magic.COMMAND,
            comment="Test comment to tell ActiveLogger to suppress GiDownlinkedPacket logs."
        ),
        # Re-enable (don't filter out) GiDownlinkedPacket logs (ID <0x0903>):
        'active-logger-enable-GiDownlinkedPacket': PreparedCommand(
            'ActiveLogger_Alogsetidfilter',
            OrderedDict(
                id=(GI_module.ID | GI_module.events['GiDownlinkedPacket'].ID),
                id_filter_enable='IDDISABLED'  # filter disabled = log allowed
            ),
            DataPathway.WIRED,
            Magic.COMMAND,
            comment="Test comment to tell ActiveLogger to suppress GiDownlinkedPacket logs."
        ),

        'active-logger-dump-filter-state': PreparedCommand(
            'ActiveLogger_Alogdumpfilterstate',
            OrderedDict(),
            DataPathway.WIRED,
            Magic.COMMAND,
            comment="Tell ActiveLogger to dump the state of its filters."
        ),

        # Turn off WARNING_HI logs (e.g. ImuAngleWarning spamming us)
        'active-logger-turn-off-warning-hi': PreparedCommand(
            'ActiveLogger_Alogseteventreportfilter',
            OrderedDict(filter_level='INPUTWARNINGHI',
                        filter_enable='INPUTDISABLED'),
            DataPathway.WIRED,
            Magic.COMMAND,
            comment="Tell ActiveLogger to suppress all WARNINGHI logs (in case of spam)."
        )
    }
