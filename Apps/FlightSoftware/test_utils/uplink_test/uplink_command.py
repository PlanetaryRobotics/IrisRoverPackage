import socket

def hton(s):
    s_network = ''
    for i in range(len(s) // 2 - 1, -1, -1):
        s_network += s[i*2:i*2+2]
    return s_network
        

ip = "192.168.1.2"
port = 8080

# FSW Header
seq = '01'              # Starts at 1, increment manually
length = hton('000e')   # Update manually if there are more arguments
checksum = hton('0000')

# FSWCommand
magic = '00bada55'
component = '11'    # Camera
opcode = '01'       # Take picture
camera_num = '01'   # Argument 1
callback_id = hton('0042')  # Argument 2

command = seq + length + checksum + magic + component + opcode + camera_num + callback_id
print(command)
command = bytearray.fromhex(command)


print("UDP target IP:", ip)
print("UDP target port:", port)
print("message:", command)

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.sendto(command, (ip, port))

