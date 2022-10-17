from trans_tools import *
from IrisBackendv3.codec.payload import CommandPayload
from IrisBackendv3.codec.packet import IrisCommonPacket
from IrisBackendv3.codec.metadata import DataPathway, DataSource
from IrisBackendv3.codec.magic import Magic
from IrisBackendv3.utils.basic import bytearray_to_spaced_hex as hexstr
connect_serial(device='/dev/tty.usbserial-A7035PDL')


def get_opts():
    """
    Return hyperparameters wrapped in argparse.

    Returns
    -------
    opts : Hyperparameters wrapped in argparse.

    """
    parser = argparse.ArgumentParser(
        description='IRIS Lunar Rover — GSW Backend — CLI')

    parser.add_argument('--mode', type=str, default='',
                        help='Source ("Lander") IP')

    # Wireless Parameters
    parser.add_argument('--ip-src', type=str, default='',
                        help='Source ("Lander") IP')
    parser.add_argument('--port-src', type=int, default=8080,
                        help='Source ("Lander") Port')
    parser.add_argument('--out-dir', type=str, default='.',
                        help='output folder')

    parser.add_argument('--serial-port', type=str, default='',
                        help='RS-422 Serial Port')

    # Visual words (requires tuning)
    parser.add_argument('--filter-scales', nargs='+', type=float,
                        default=[1, 2],
                        help='a list of scales for all the filters')
    parser.add_argument('--K', type=int, default=10,
                        help='# of words')
    parser.add_argument('--alpha', type=int, default=25,
                        help='Using only a subset of alpha pixels in each image')

    # Recognition system (requires tuning)
    parser.add_argument('--L', type=int, default=1,
                        help='# of layers in spatial pyramid matching (SPM)')

    # Additional options (add your own hyperparameters here)
    parser.add_argument('--custom-dict-name', type=str, default=None,
                        help='Custom name for custom dictionary')
    parser.add_argument('--custom-system-name', type=str, default=None,
                        help='Custom name for custom trained system')

    parser.add_argument('--rebuild-dictionary', type=bool, default=False,
                        help='Whether to rebuild the dictionary')
    parser.add_argument('--rebuild-recognition-system', type=bool, default=False,
                        help='Whether to relearn the recognition system')

    ##
    opts = parser.parse_args()
    return opts
