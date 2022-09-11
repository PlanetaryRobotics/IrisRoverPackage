# Runs a Transceiver Console:
import time
from trans_tools_console_process import start_console, settings

settings['SAVE_FILE_PREFIX'] = 'iris__radio_firmware_checkout__8_28_22_4' # this is the prefix on all log files.

# Serial numbers for each serial transceiver we use:
SERIAL_DEVICE_SNS = {
    'lander_harness': 'A7035PDL',  # Connects to the Lander harness
    'J36_debugger': 'AB0JRGV8' # Connects to J36-RS422 header on the SBC
}

import argparse

parser = argparse.ArgumentParser(
    description='IRIS Lunar Rover — Data Standards Lookup — CLI')


# Settings:
def get_opts():
    """
    Return settings wrapped in argparse.

    Returns
    -------
    opts : Settings wrapped in argparse.

    """
    default_baud: int = 9600
    parser.add_argument('-b', '--baud', type=int, default=default_baud, choices=[9600, 57600],
                        help=f'RS422 Baud Rate between Watchdog and Lander. Default: {default_baud}.')

    default_serial_device = [*SERIAL_DEVICE_SNS.keys()][0]
    parser.add_argument('-s', '--serial-device', type=str, default=default_serial_device, choices=list(SERIAL_DEVICE_SNS.keys()),
                        help=f'Which serial device is being used for RS422 comms. Default: {default_serial_device}.')

    opts = parser.parse_args()
    return opts

if __name__ == '__main__':
    opts = get_opts()
    print(
        f"Starting Iris Console with \t"
        f"SERIAL: baud: {opts.baud},  using: {opts.serial_device} (SN: {SERIAL_DEVICE_SNS[opts.serial_device]})"
        "\n"
        f"NOTE: On windows this will likely capture all keyboard inputs EVEN WHEN WINDOW IS NOT IN FOCUS (potentially leading to accidental command sends). So, be sure to close the window when not in use if using Windows."
        "\n"
    )
    input(f"Press ENTER to begin . . .")

    start_console(
        serial_device_sn = SERIAL_DEVICE_SNS[opts.serial_device],
        baud = opts.baud
    )