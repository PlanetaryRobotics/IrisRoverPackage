# Runs a Transceiver Console:
import time
from trans_tools_console_process import start_console, settings

settings['SAVE_FILE_PREFIX'] = 'iris__radio_firmware_checkout__8_28_22_4' # this is the prefix on all log files.

# Serial numbers for each serial transceiver we use:
SERIAL_DEVICE_SNS = {
    'lander_harness': 'A7035PDL',  # Connects to the Lander harness
    'J36_debugger': 'AB0JRGV8' # Connects to J36-RS422 header on the SBC
}

# Settings (TODO: make the opt. CLI args):
SERIAL_DEVICE: str = 'lander_harness' # key in SERIAL_DEVICE_SNS
BAUD: int = 9600

if __name__ == '__main__':
    print(
        f"Starting Iris Console with \t"
        f"SERIAL: baud: {BAUD},  using: {SERIAL_DEVICE} (SN: {SERIAL_DEVICE_SNS[SERIAL_DEVICE]})"
        "\n"
        f"NOTE: On windows this will likely capture all keyboard inputs EVEN WHEN WINDOW IS NOT IN FOCUS (potentially leading to accidental command sends). So, be sure to close the window when not in use if using Windows."
        "\n"
    )
    input(f"Press ENTER to begin . . .")

    start_console(
        serial_device_sn = 'A7035PDL',
        baud = 9600
    )