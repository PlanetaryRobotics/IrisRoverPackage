from IrisBackendv3.codec.settings import set_codec_standards
from IrisBackendv3.data_standards.prebuilt import add_to_standards, ALL_PREBUILT_MODULES
from IrisBackendv3.data_standards.logging import (
    logger as DsLogger,
    logger_setConsoleLevel as DsLoggerLevel
)
from IrisBackendv3.data_standards import DataStandards
from IrisBackendv3.transceiver import SlipTransceiver, list_all_device_info
from IrisBackendv3.transceiver.endec import IpUdpEndec

from scripts.utils.__command_aliases import get_command
from scripts.utils.trans_tools import *

# List all available serial devices:
print("\nAll available serial devices:")
list_all_device_info(print_data=True)
print("\n\n")

# Re-build the `DataStandards` fresh from latest fprime and add in all prebuilt
# (WD Telem) Modules.
DsLoggerLevel('CRITICAL')
standards = DataStandards.build_standards()
add_to_standards(standards, ALL_PREBUILT_MODULES)
set_codec_standards(standards)

slip_xcvr = SlipTransceiver(
    device_sn='00000000',  # FTDI-232 debug dongle
    baud=115200,
    endecs=[IpUdpEndec()]
)
slip_xcvr.begin()
