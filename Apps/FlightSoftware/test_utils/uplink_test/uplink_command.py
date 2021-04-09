import socket
import numpy as np
import struct

def compute_checksum(b):
    return ~np.uint8(sum(bytearray(b)) % 256)
        

ip = "192.168.1.2"
port = 8080

# FSW Header
seq = 4              # Starts at 1, increment manually

# FSWCommand
magic = 0x00bada55

# Camera
length = 0x000e     # Update manually if there are more arguments
component = 0x11    # Camera
opcode = 0x01       # Take picture
the_id = component << 8 | opcode
camera_num = 0x01   # Argument 1
callback_id = 0x0042  # Argument 2

fsw_header_fmt = 'B H B'
cam_fmt = 'I H B H'
fmt = '<' + fsw_header_fmt + cam_fmt
length = struct.calcsize('<' + cam_fmt)
cam_cmd = struct.pack(fmt,
                      seq, length, 0,        # FSW Packet Header
                      magic, the_id, camera_num, callback_id)
checksum = compute_checksum(cam_cmd)
cam_cmd = struct.pack(fmt,
                      seq, length, checksum,
                      magic, the_id, camera_num, callback_id)
assert not compute_checksum(cam_cmd)


"""
# Navigation
length = hton('000f')   # Update manually if there are more arguments
component = '0d'
opcode = '00'  # 0: Forward   1: Backward     2: Rotate Left (CCW)     3: Rotate Right (CW)
distance = '05'
speed = '05'
callback_id = hton('0042')
command = seq + length + checksum + magic + component + opcode + distance + speed + callback_id
"""

print("Message:", cam_cmd)

print("UDP target IP:", ip)
print("UDP target port:", port)

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.sendto(cam_cmd, (ip, port))

