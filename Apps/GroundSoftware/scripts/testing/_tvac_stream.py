from scripts.utils.tvac_tools import *

connect_serial(device = '/dev/ttyUSB1')
settings['SAVE_FILE_PREFIX'] = 'psu_iris_tvac_' # this the prefix on all log files. make it something unique.

send_command('WatchDogInterface_SwitchToKeepAliveMode', confirm='YES')

stream_data()