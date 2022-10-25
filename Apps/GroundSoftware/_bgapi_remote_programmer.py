#

from typing import Final

import zlib

import IrisBackendv3.codec.bgapi as bgapi


BGAPI_API: Final[str] = bgapi.BGAPI_WIFI_DEF
BGAPI_ENCODE: Final = bgapi.serdeser.Serializer(BGAPI_API)
BGAPI_DECODE: Final = bgapi.serdeser.Deserializer(BGAPI_API)


def send_bgapi_cmd_over_icp(bgcmd: bgapi.BGCommand) -> None:
    # Serialize BGAPI command (max 150B): # TODO: temp (need at least 128B to do radio programming)
    bg_data = BGAPI_ENCODE.command(bgcmd._device_name,
                                   bgcmd._class_name, bgcmd._msg_name, bgcmd)

    # Pack inside an Iris Common Packet:
    crc32 = zlib.crc32(bg_data)

    command_name = 'NetworkManager_ForwardBgApiCommand'
    command_args = {
        'crc32': crc32,
        'data': bg_data
    }

    module, command = standards.global_command_lookup(command_name)

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

    # Pack command:
    packet_bytes = b''
    try:
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
