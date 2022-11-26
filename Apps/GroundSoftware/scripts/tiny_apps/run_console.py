# Runs a Transceiver Console:
import argparse
import time
from scripts.utils.trans_tools_console_process import start_console, settings

# Serial numbers for each serial transceiver we use:
SERIAL_DEVICE_SNS = {
    'lander_harness': 'A7035PDL',  # Connects to the Lander harness
    'J36_debugger': 'AB0JRGV8'  # Connects to J36-RS422 header on the SBC
}


parser = argparse.ArgumentParser(
    description='IRIS Lunar Rover — Diagnostic Console (Iris Console)'
)


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

    default_test_name: str = 'generic_test'
    parser.add_argument('-n', '--test-name', type=str, default=default_test_name,
                        help=f'Name of this test (for logging purposes). Default: {default_test_name}.')

    opts = parser.parse_args()
    return opts


def main():
    opts = get_opts()

    # this is the prefix on all log files.
    settings['SAVE_FILE_PREFIX'] = opts.test_name

    print(
        f"Starting Iris Console with \t"
        f"SERIAL: baud: {opts.baud},  using: {opts.serial_device} (SN: {SERIAL_DEVICE_SNS[opts.serial_device]})"
        "\n"
        f"NOTE: On Windows and MacOS this will likely capture all keyboard inputs EVEN WHEN WINDOW IS NOT IN FOCUS (potentially leading to accidental command sends). So, be sure to close the window when not in use if using Windows."
        "\n"
    )
    input(f"Press ENTER to begin . . .")

    start_console(
        serial_device_sn=SERIAL_DEVICE_SNS[opts.serial_device],
        baud=opts.baud
    )
    

if __name__ == '__main__':
    main()
