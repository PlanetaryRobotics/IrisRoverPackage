# Test packets from WD:
from trans_tools import *
tp = [
    b"\xd5\x04\x08\x18\x00\x03\x03\n\x00\x18\x00\x00\x00\x00\x00`\x00B\x80\x0f\xc1\x00\xb8\xf4\x014!\xfd\x0cV\x0e\x18\r\x10'\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00",
    b"\xd5\x04\x08\x18\x00\x04\x03\n\x00\x18\x00\x00\x00\x00\x00`\x00B\x80\x0f\xc1\x00\xb8\xf4\x014!\xfd\x0cV\x0e\x18\r\x10'\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00",
    b"\xd5\x04\x08\x18\x00\x05\x03\n\x00\x18\x00\x00\x00\x00\x00`\x00B\x80\x0f\xc1\x00\xb8\xf4\x014!\xfd\x0cV\x0e\x18\r\x10'\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00",
    b"\xd5\x04\x08\x18\x00\x06\x03\n\x00\x18\x00\x00\x00\x00\x00\xe0\x00B\x80\x0f\xc1\x00\xb8\xf4\x014!\xfd\x0cV\x0e\x18\r\x10'\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
]

packet = parse_packet(tp[0])

log_print(packet)
