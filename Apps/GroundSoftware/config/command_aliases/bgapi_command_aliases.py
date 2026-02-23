"""
Aliases for Iris BGAPI commands. These control the Radio's configuration and
go from Hercules to the Radio, not to the Radio directly. Only to be used with
BGAPI passthrough ON.

Last Update: 03/10/2023
"""
from typing import Final
from collections import OrderedDict
from IrisBackendv3.codec.magic import Magic
from IrisBackendv3.codec.metadata import DataPathway
from IrisBackendv3.data_standards.data_standards import DataStandards
from IrisBackendv3.config.command_aliases import PreparedCommand, CommandAliases, CommandAliasesBuilder

import IrisBackendv3.codec.bgapi as bgapi
from IrisBackendv3.utils.crc import crc32_fsw

# Test BGAPI Passthrough data:
BGAPI_GET_MAC_CMD: Final = bgapi.build_command(
    bgapi.BGAPI_WIFI_API, 'config', 'get_mac', {'hw_interface': 0}
)
BGAPI_GET_MAC_BYTES: Final[bytes] = bgapi.encode_command(
    bgapi.BGAPI_WIFI_ENCODER, BGAPI_GET_MAC_CMD
)

BGAPI_WIFI_ON_CMD: Final = bgapi.build_command(
    bgapi.BGAPI_WIFI_API, 'sme', 'wifi_on', dict()
)

BGAPI_START_SCAN_CMD: Final = bgapi.build_command(
    bgapi.BGAPI_WIFI_API, 'sme', 'start_scan', {
        'hw_interface': 0, 'chList': b''}
)
# bgapi.BGAPI_WIFI_API, 'sme', 'start_scan', {'hw_interface': 0, 'chList': b'\x0b'})

BGAPI_STOP_SCAN_CMD: Final = bgapi.build_command(
    bgapi.BGAPI_WIFI_API, 'sme', 'stop_scan', dict()
)

BGAPI_DFU_ON_CMD: Final = bgapi.build_command(
    bgapi.BGAPI_WIFI_API, 'dfu', 'reset', {'dfu': 1}
)

BGAPI_DFU_OFF_CMD: Final = bgapi.build_command(
    bgapi.BGAPI_WIFI_API, 'dfu', 'reset', {'dfu': 0}
)


def bgcmd_to_expect_response(bgcmd) -> str:
    """Returns the value for `expect_response` in
    `NetworkManager_SendBgApiCommand` needed for the given BGAPI `BGCommand`
    """
    return 'NM_BGAPI_CMD_DONTEXPECTRESPONSE' if bgcmd.no_response else 'NM_BGAPI_CMD_EXPECTRESPONSE'


def bgcmd_to_prepared_command(
    bgcmd,
    packet_id: int
) -> PreparedCommand:
    """
    Builds a `PreparedCommand` for the given `BGCommand`
    **ASSUMING IT SHOULD WORK** (some test commands require an intentionally
    bad command to make sure it'll be rejected)
    """
    raw_data: Final[bytes] = bgapi.encode_command(
        bgapi.BGAPI_WIFI_ENCODER, bgcmd
    )
    return PreparedCommand(
        'NetworkManager_SendBgApiCommand',
        OrderedDict(
            crc_32=crc32_fsw(raw_data),
            packet_id=packet_id,
            expect_response=bgcmd_to_expect_response(bgcmd),
            bgapi_packet=raw_data
        ),
        DataPathway.WIRED,
        Magic.COMMAND,
        comment=f"Send BGAPI command: `{bgcmd!s}`."
    )


@CommandAliasesBuilder
def ALIASES(standards: DataStandards) -> CommandAliases:
    return {
        'radio-bgapi-passthru-on': PreparedCommand(
            'NetworkManager_SetRadioBgApiPassthrough',
            OrderedDict(passthrough=True),
            DataPathway.WIRED,
            Magic.COMMAND,
            comment="Tell NM to turn ON BGAPI forwarding."
        ),
        'radio-bgapi-passthru-off': PreparedCommand(
            'NetworkManager_SetRadioBgApiPassthrough',
            OrderedDict(passthrough=False),
            DataPathway.WIRED,
            Magic.COMMAND,
            comment="Tell NM to turn OFF BGAPI forwarding."
        ),
        # Triggers a `RadioBgApiCommandRecords` event to see what BgApi
        # packets have been processed recently and what the outcomes were.
        'radio-bgapi-passthru-cmd-dump': PreparedCommand(
            'NetworkManager_DownlinkBgApiCommandRecords',
            OrderedDict(),
            DataPathway.WIRED,
            Magic.COMMAND,
            comment="Tells NM to log what BGAPI packets have been processed recently and what the outcomes were."
        ),

        # Sends a BGAPI command to the Radio to get the MAC address (basic test
        # command):
        'radio-bgapi-passthru-get-mac': PreparedCommand(
            'NetworkManager_SendBgApiCommand',
            OrderedDict(
                crc_32=crc32_fsw(BGAPI_GET_MAC_BYTES),
                packet_id=1234,
                expect_response='NM_BGAPI_CMD_DONTEXPECTRESPONSE' if BGAPI_GET_MAC_CMD.no_response else 'NM_BGAPI_CMD_EXPECTRESPONSE',
                bgapi_packet=BGAPI_GET_MAC_BYTES
            ),
            DataPathway.WIRED,
            Magic.COMMAND,
            comment="BGAPI test command to get the MAC address (tell WD to tell Herc to tell the Radio to tell Herc to tell WD to tell us its MAC address)."
        ),

        # Sends a BGAPI command to the Radio to get the MAC address (basic test
        # command) but with an intentionally incorrect CRC:
        'radio-bgapi-passthru-get-mac--bad-crc': PreparedCommand(
            'NetworkManager_SendBgApiCommand',
            OrderedDict(
                crc_32=crc32_fsw(BGAPI_GET_MAC_BYTES)-1,
                packet_id=1234,
                expect_response='NM_BGAPI_CMD_DONTEXPECTRESPONSE' if BGAPI_GET_MAC_CMD.no_response else 'NM_BGAPI_CMD_EXPECTRESPONSE',
                bgapi_packet=BGAPI_GET_MAC_BYTES
            ),
            DataPathway.WIRED,
            Magic.COMMAND,
            comment="Command to verify BGAPI CRC checking (get MAC command but with bad CRC - SHOULD BE REJECTED)."
        ),

        # Some basic diagnostic Radio passthru commands:
        'radio-bgapi-passthru-wifi-on': bgcmd_to_prepared_command(
            BGAPI_WIFI_ON_CMD,
            packet_id=1235
        ),

        'radio-bgapi-passthru-scan': bgcmd_to_prepared_command(
            BGAPI_START_SCAN_CMD,
            packet_id=1236
        ),
        'radio-bgapi-passthru-scan-stop': bgcmd_to_prepared_command(
            BGAPI_STOP_SCAN_CMD,
            packet_id=1237
        ),

        'radio-bgapi-passthru-dfu-on': bgcmd_to_prepared_command(
            BGAPI_DFU_ON_CMD,
            packet_id=4321
        ),
        'radio-bgapi-passthru-dfu-off': bgcmd_to_prepared_command(
            BGAPI_DFU_OFF_CMD,
            packet_id=4320
        ),

        # Turn off Hercules-Radio UART (inside the Hercules) so an external device
        # can use it (for Radio diagnostics and programming).
        'radio-comms-ext-mode': PreparedCommand(
            'NetworkManager_SetRadioCommunicationsMode',
            OrderedDict(mode='EXTERNAL'),
            DataPathway.WIRED,
            Magic.COMMAND,
            comment="Turn OFF Hercules-Radio UART so an external device can use it (for Radio diagnostics and programming)."
        ),

        # Turn on Hercules-Radio UART (inside the Hercules) so Hercules can use it
        # (normal operation). If this doesn't work, just do a `reset-herc`.
        'radio-comms-normal-mode': PreparedCommand(
            'NetworkManager_SetRadioCommunicationsMode',
            OrderedDict(mode='HERCULES'),
            DataPathway.WIRED,
            Magic.COMMAND,
            comment="Turn ON Hercules-Radio UART so an external device can use it (for Radio diagnostics and programming)."
        )
    }
