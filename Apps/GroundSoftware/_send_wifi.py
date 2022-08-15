# Send data to the Radio over Wifi:
import scapy.all as scp
import socket
import numpy as np
import struct

def send_wifi(data: bytes, ip="192.168.150.3", port=42000, src_ip="192.168.10.105", src_port=43531) -> None:
    # Craft UDP packet (incl. using the correct src_ip, even if that's not our own):
    full_packet = scp.IP(dst=ip, src=src_ip) / scp.UDP(sport=src_port, dport=port)/scp.Raw(load=data)
    # printraw(scp.raw(scp.IP(scp.raw(full_packet))))
    # printraw(scp.raw(full_packet))
    full_packet_data = scp.raw(full_packet)

    # Open raw socket and send:
    try:
        sock = socket.socket(socket.AF_INET, socket.SOCK_RAW, socket.IPPROTO_RAW)
        # Tell kernel not to add headers (we took care of that above):
        sock.setsockopt(socket.IPPROTO_IP, socket.IP_HDRINCL, 1)
        sock.sendto(full_packet_data, (ip, 0))
        sock.close()
    except PermissionError as pe:
        print("Failed to open socket for sending WiFi packets due to PermissionError. You should be running this as `sudo` - are you?")

def send_radio_only_command(command_id: int, command_data: bytes, *args, **kwargs) -> None:
    ## Build Radio ICP VLP:
    radio_only_type_magic = b'\xD0\x09\xAD\x10'
    vlp = radio_only_type_magic + command_id.to_bytes(1, 'little') + command_data
    
    ## Build the Radio ICP CPH packet:
    # Start with null seq_num
    seq_num = b'\x00'
    # Tack on VLP length:
    vlp_len = len(vlp).to_bytes(2, 'little')
    # Add a null ICP checksum (for now):
    checksum_bytes = b'\x00'
    # Make the CPH:
    cph = seq_num + vlp_len + checksum_bytes

    # Add on the VLP to make the ICP (draft, no checksum):
    data = cph + vlp

    # Compute checksum of packet with null checksum:
    checksum = ~np.uint8(sum(bytearray(data)) % 256)
    checksum_bytes = struct.pack('<B', checksum)
    
    # Lastly, rebuild CPH and then ICP packet with correct checksum:
    cph = seq_num + vlp_len + checksum_bytes
    data = cph + vlp

    ## Send the data:
    send_wifi(data, *args, **kwargs)


radio_only_cmd_ids = {
    # Echo the given bytes back over wifi:
    'ECHO': 0x00,
    # Reset the radio (requires data to be 'RESET' or it won't do anything)
    'RESET_RADIO': 0xBB,
    # Send the given bytes directly to Hercules UART (instead of through the BGAPI) - For use in case something went wrong and we need to place specific bytes on the Radio-UART line:
    'WRITE_UART': 0xDB,
    # Send the given ASCII message to Hercules using the Radio-Hercules Direct Message protocol:
    'HERCULES_DM': 0xDD
}


def send_radio_only_cmd__echo(data_to_echo: bytes, *args, **kwargs):
    send_radio_only_command(radio_only_cmd_ids['ECHO'], data_to_echo, *args, **kwargs)

send_radio_only_cmd__echo(b'HELLO')
send_radio_only_command(radio_only_cmd_ids['WRITE_UART'], b'TESTING...')
send_radio_only_command(0xFF, b'This-is-a-bad-cmd-id')
send_radio_only_command(radio_only_cmd_ids['HERCULES_DM'], b'faked:STATE')
send_radio_only_command(radio_only_cmd_ids['RESET_RADIO'], b'nada')
send_radio_only_command(radio_only_cmd_ids['RESET_RADIO'], b'')
# send_radio_only_command(radio_only_cmd_ids['RESET_RADIO'], b'RESET')
