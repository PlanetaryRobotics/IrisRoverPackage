# Runs a Transceiver Console:
from trans_tools_console_process import start_console, settings

settings['SAVE_FILE_PREFIX'] = 'iris__radio_firmware_checkout__8_28_22_4' # this is the prefix on all log files.

start_console(
    serial_device_sn = 'A7035PDL',
    baud = 9600
)