#

from typing import Final

from collections import OrderedDict

import IrisBackendv3.codec.bgapi as bgapi
from IrisBackendv3.utils.crc import crc32_fsw

from IrisBackendv3.codec.settings import set_codec_standards
from IrisBackendv3.data_standards.prebuilt import add_to_standards, ALL_PREBUILT_MODULES
from IrisBackendv3.data_standards.logging import logger as DsLogger
from IrisBackendv3.data_standards import DataStandards

from trans_tools import *

BGAPI_API: Final = bgapi.BGAPI_WIFI_API
BGAPI_ENCODE: Final = bgapi.serdeser.Serializer(BGAPI_API)
BGAPI_DECODE: Final = bgapi.serdeser.Deserializer(BGAPI_API)


# Re-build the `DataStandards` fresh from latest fprime and add in all prebuilt
# (WD Telem) Modules.
# These `DataStandards` will serve as TMTC definitions be used by the `Codec`
# layer used by the `Transceiver` layer to interpret packets.
# TODO: Rebuild this once in the main IPC module, cache it, and load from cache in other IPC apps.
DsLogger.setLevel('CRITICAL')
standards = DataStandards.build_standards()
add_to_standards(standards, ALL_PREBUILT_MODULES)
set_codec_standards(standards)


def compile_bgapi_cmd_as_icp(packet_id: int, bgcmd: bgapi.BGCommand) -> bytes:
    """
    Compiles the given BGAPI Command inside an Iris Common Packet (ICP) as a
    BGAPI-Passthrough Command to the Hercules. This Hercules command is tagged
    with the given packet_id (uint32_t).
    Returns the raw packet bytes to be sent to the Rover.
    """
    # Serialize BGAPI command (max 134B - checked by ICP encoder automatically):
    bg_data = bgapi.encode_command(BGAPI_ENCODE, bgcmd)

    # Compute args:
    command_args = OrderedDict(
        crc_32=crc32_fsw(bg_data),
        packet_id=packet_id,
        expect_response='NM_BGAPI_CMD_DONTEXPECTRESPONSE' if bgcmd.no_response else 'NM_BGAPI_CMD_EXPECTRESPONSE',
        bgapi_packet=bg_data
    ),

    # Pack inside an Iris Common Packet:
    command_name = 'NetworkManager_SendBgApiCommand'
    module, command = standards.global_command_lookup(command_name)

    # Pack command:
    # Make ICP payload:
    command_payload = CommandPayload(
        pathway=DataPathway.WIRED,
        source=DataSource.GENERATED,
        magic=Magic.COMMAND,
        module_id=module.ID,
        command_id=command.ID,
        args=command_args
    )
    # Load payload into ICP packet:
    payloads = EnhancedPayloadCollection(CommandPayload=[command_payload])
    packet = IrisCommonPacket(
        seq_num=0x00,
        payloads=payloads
    )
    packet_bytes = packet.encode()

    return packet_bytes

    """
    <command kind="sync" opcode="0x01" mnemonic="Start_Radio_Programming">
        <comment>
            Tell the Radio to enter Device Firmware Updating (DFU) mode for programming the Radio over UART.
            This will: 
                1. Send a BGAPI `wifi_cmd_dfu_reset` command to boot device to DFU mode with Reset command.
                2. Wait for a BGAPI `wifi_evt_dfu_boot` event.
                3. Send a `wifi_cmd_dfu_flash_set_address(address=0x0000)` to start the firmware update.
                3. Send command Flash Set Address to start the firmware update.
            Will generate a `Start_Radio_DfuMode_Executed` event when done (with a success or fail arg).
        </comment>
    </command>
    <command kind="sync" opcode="0x02" mnemonic="Stop_Radio_DfuMode">
        <comment>
            Tell the Radio to exit Device Firmware Updating (DFU) mode for programming the Radio over UART.
            This will: 
                1. Send a BGAPI `wifi_cmd_dfu_reset` command to boot device to DFU mode with Reset command.
                . Send Flash Upload Finish to when all the data has been uploaded.
                6. Finalize the DFU firmware update with Reset command.
            Will generate a `Stop_Radio_DfuMode_Executed` event when done (with a success or fail arg).
        </comment>
    </command>
    """


def extract_bgapi_response(packet: IrisCommonPacket):
    # TODO (passed directly via Wd debugprintf)


if __name__ == "__main__":

    # Build BGAPI Command:
    bgcmd = bgapi.BGCommand()  # TODO

    self.response_queue = queue.Queue(maxsize=1)
    self.waiting_response = threading.Event()

    def send_command(self, bgcmd, response_timeout):
        try:
            # Send command
            if not bgcmd.no_response:
                self.waiting_response.set()
            try:
                self.conn.write()
