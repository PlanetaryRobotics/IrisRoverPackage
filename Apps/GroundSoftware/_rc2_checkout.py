from IrisBackendv3.codec.settings import set_codec_standards
from IrisBackendv3.data_standards.prebuilt import add_to_standards, ALL_PREBUILT_MODULES
from IrisBackendv3.data_standards.logging import logger as DsLogger
from IrisBackendv3.data_standards import DataStandards
from IrisBackendv3.transceiver import SlipTransceiver
from IrisBackendv3.transceiver.endec import IpUdpEndec

from __command_aliases import get_command
from trans_tools import *

# Re-build the `DataStandards` fresh from latest fprime and add in all prebuilt
# (WD Telem) Modules.
# These `DataStandards` will serve as TMTC definitions be used by the `Codec`
# layer used by the `Transceiver` layer to interpret packets.
# TODO: Rebuild this once in the main IPC module, cache it, and load from cache in other IPC apps.
DsLogger.setLevel('CRITICAL')
standards = DataStandards.build_standards()
add_to_standards(standards, ALL_PREBUILT_MODULES)
set_codec_standards(standards)

# SETUP:
slip_xcvr = SlipTransceiver(
    device_sn='A7035PDL',  # Connects to the Lander harness
    # device_sn= 'AB0JRGV8', # Connects to J36-RS422 header on the SBC
    baud=57600,
    endecs=[IpUdpEndec()]
)
slip_xcvr.begin()

cmd, param = 'ReportStatus', True

# True - send command, # False - sniff only
send_data_packet_to_wd_before_sniffing = True
# send_data_packet_to_wd_before_sniffing = False

# overwriting = True

specific_param_override = True
specific_cmd_name_override = 'ReportStatus'

# specific_cmd_name_override = 'transit'
specific_cmd_name_override = 'setup'
specific_cmd_name_override = 'power-on'

# specific_cmd_name_override = 'vsa-off'
# specific_cmd_name_override = 'vsa-on'

# specific_cmd_name_override = '3v3-off'
# specific_cmd_name_override = '3v3-on'


# specific_cmd_name_override = 'power-off-herc'
# specific_cmd_name_override = 'power-off-radio'
# specific_cmd_name_override = 'power-off-fpga'
# specific_cmd_name_override = 'power-off-motors'

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

# blimp_inputs = [False, True, 'OFF', 'ON', 'FORCE_HIGH', 'FORCE_LOW', 'LATCH_BATT_OFF']
# specific_param_override = True

# specific_cmd_name_override = 'SetChargerEn'
# specific_cmd_name_override = 'setup'
# specific_cmd_name_override = 'power-on'
# specific_cmd_name_override = '24-on'
# specific_cmd_name_override = 'power-on-motors'
# specific_param_override = 'CHARGE_EN_ON'


# if overwriting:
cmd, param = specific_cmd_name_override, specific_param_override

print(cmd, '  :  ', param)

# Build command(s):
pathway, magic, command_name, kwargs, telem_pathway = get_command(cmd, param)
print(pathway, magic, command_name, kwargs)

command_payload_type = {
    Magic.WATCHDOG_COMMAND: WatchdogCommandPayload,
    Magic.RADIO_COMMAND: CommandPayload,
    Magic.COMMAND: CommandPayload
}[magic]

module, command = standards.global_command_lookup(command_name)

payloads = EnhancedPayloadCollection(
    CommandPayload=[
        command_payload_type(
            pathway=pathway,
            source=DataSource.GENERATED,
            magic=magic,
            module_id=module.ID,
            command_id=command.ID,
            args=kwargs
        )
    ]
)

# Send Command:
if send_data_packet_to_wd_before_sniffing:
    slip_xcvr.send_payloads(payloads)

# Stream Telemetry:
while True:
    packets = slip_xcvr.read()
# Check for wireless telemetry in Wireshark. For parsing help, run: `pyenv exec python parse_pcap.py --help`
