## SETUP:

from trans_tools import *
from __command_aliases import get_command
from IrisBackendv3.utils.basic import bytearray_to_spaced_hex as hexstr

seq_num = 0x00


## SETTINGS:
serial_device_sn: str = 'A7035PDL' # Connects to the Lander harness
# serial_device_sn: str = 'AB0JRGV8' # Connects to J36-RS422 header on the SBC
baud = 57600
baud = 9600
ip="192.168.1.2"
port=8080

settings['SAVE_FILE_PREFIX'] = 'iris__radio_firmware_checkout__8_28_22_2' # this is the prefix on all log files.


cmd, param = 'ReportStatus', True

# True - send command, # False - sniff only
send_data_packet_to_wd_before_sniffing = True
#send_data_packet_to_wd_before_sniffing = False

# overwriting = True

specific_param_override = True
specific_cmd_name_override = 'ReportStatus'

# specific_cmd_name_override = 'transit'
specific_cmd_name_override = 'setup'
specific_cmd_name_override = 'power-on'
# specific_cmd_name_override = 'wifi-mode'

# specific_cmd_name_override = '24-off'
# specific_cmd_name_override = '24-on'

# specific_cmd_name_override = '3v3-off'
# specific_cmd_name_override = '3v3-on'


# specific_cmd_name_override = 'power-off-herc'
# specific_cmd_name_override = 'power-off-radio'
# specific_cmd_name_override = 'power-off-fpga'
# specific_cmd_name_override = 'power-off-motors'
# 
# specific_cmd_name_override = 'power-on-herc'
# specific_cmd_name_override = 'power-on-radio'
# specific_cmd_name_override = 'power-on-fpga'
# specific_cmd_name_override = 'power-on-motors'

# specific_cmd_name_override = 'reset-radio'
# specific_cmd_name_override = 'reset-herc'
# specific_cmd_name_override = 'reset-fpga'
# specific_cmd_name_override = 'reset-motors'


# specific_cmd_name_override = 'drive-fwd-200'

# REGE_set = False
# BE_set = False
# CE_set = 'OFF' # 'OFF' 'ON' 'FORCE_HIGH'
# BCE_set = 'OFF' # 'OFF' 'ON' 'FORCE_HIGH'
# BL_set = 'OFF' # 'OFF' 'ON' 'PULSE_HIGH'

# blimp_cmds: SetChargerEn , SetChargerPowerConnection , SetBatteryConnection
#             SetBatteryControlEnable , SetBatteryLatch
# specific_cmd_name_override = 'SetBatteryConnection'
# specific_cmd_name_override = 'SetBatteryLatch'

# blimp_inputs = [False, True, 'OFF', 'ON', 'FORCE_HIGH', 'FORCE_LOW', 'LATCH_BATT_OFF']
# specific_param_override = True
# specific_param_override = 'LATCH_BATT_EN_PULSE_HIGH'

# specific_cmd_name_override = 'SetChargerEn'
# specific_cmd_name_override = 'setup'
# specific_cmd_name_override = 'power-on'
# specific_cmd_name_override = '24-off'
# specific_cmd_name_override = 'power-on-motors'
# specific_param_override = 'CHARGE_EN_ON'


# if overwriting:
cmd, param = specific_cmd_name_override, specific_param_override

print(cmd,'  :  ',param) 

load_cache()
setup_logger(settings['SAVE_FILE_PREFIX']) # logs of stuff printed to console will be stored to `raw-console-logs`

# IF ERROR - update last number in FILE_PREFIX

from serial.tools import list_ports, list_ports_common
serial_device = [cp.device for cp in list_ports.comports() if cp.serial_number is not None and serial_device_sn in cp.serial_number][0]
serial_device

pathway, magic, command_name, kwargs, telem_pathway = get_command(cmd, param)
print(pathway, magic, command_name, kwargs)

packet = build_command_packet(seq_num, pathway, magic, command_name,kwargs)
packet_bytes = packet.encode()
print(hexstr(packet_bytes))

## Send Command:
connect_serial(device = serial_device, baud=baud)
if send_data_packet_to_wd_before_sniffing:
    send_packet(packet, pathway, ip, port)

## Stream Telemetry:
stream_data_ip_udp_serial(use_telem_dataview=True)
# Check for wireless telemetry in Wireshark. For parsing help, run: `pyenv exec python parse_pcap.py --help`