import socket
import numpy as np
import struct


def compute_checksum(b):
    return ~np.uint8(sum(bytearray(b)) % 256)


ip = "192.168.1.2"
port = 8080

# FSW Header
seq = 1              # Starts at 1, increment manually
FSW_HEADER_FMT = 'B H B'
CMD_HEADER_FMT = 'I H'
CMD_MAGIC = 0x00bada55   # FSWCommand


def generate_command(component, opcode, struct_fmt, *args):
    """ Generate the binary string for a command.

    :param component: Component ID
    :param opcode: Command opcode
    :param struct_fmt: Format string for the command arguments to be fed into struct.pack
    :param *args: Command arguments
    :return: Command bytes
    """
    cmd_fmt = '<' + FSW_HEADER_FMT + CMD_HEADER_FMT + struct_fmt
    length = struct.calcsize('<' + CMD_HEADER_FMT + struct_fmt)
    the_id = component << 8 | opcode
    cmd = struct.pack(cmd_fmt,
                      seq, length, 0,        # FSW Packet Header
                      CMD_MAGIC, the_id, *args)
    checksum = compute_checksum(cmd)
    cmd = struct.pack(cmd_fmt,
                      seq, length, checksum,
                      CMD_MAGIC, the_id, *args)
    assert not compute_checksum(cmd)
    return cmd


cam_fmt = 'B H'
command = 0x01      # Take picture
camera_num = 1      # 0 or 1
callback_id = 42
cam_cmd = generate_command(0x11, command, cam_fmt, camera_num, callback_id)


nav_fmt = 'B B H'
# 0: Forward   1: Backward     2: Rotate Left (CCW)     3: Rotate Right (CW)
command = 0x00
distance = 5
speed = 5
callback_id = 42
nav_cmd = generate_command(0x0d, command, nav_fmt,
                           distance, speed, callback_id)


gi_fmt = 'I'
command = 0x00      # Set primary interface
interface = 0x01    # 0: Watchdog (Wired RS422)     1: WF121 (Wireless WiFi)
gi_cmd = generate_command(0x09, command, gi_fmt, interface)


the_cmd = gi_cmd
print("Message:", the_cmd)
print("UDP target IP:", ip)
print("UDP target port:", port)
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.sendto(the_cmd, (ip, port))
