from scripts.utils.trans_tools import *

connect_serial(device='/dev/ttyUSB0')
# this the prefix on all log files. make it something unique.
settings['SAVE_FILE_PREFIX'] = 'iris_firing_test_trial0'

stream_data_ip_udp_serial()
