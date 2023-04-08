"""
Quick and dirty script to investigate the backwards current leak on the OR gate
at "high" temperatures discovered during Peregrine TVAC.

NOTE: If this gives protobuf warnings, either resolve them or run:
- `PROTOCOL_BUFFERS_PYTHON_IMPLEMENTATION=python ./run-script.sh ...`
- instead of: `./run-script.sh ...`
"""
# Activate postponed annotations:
from __future__ import annotations

if __name__ == "__main__":
    # Loading dependencies can take a bit (builds DataStandards as part of
    # this). Acknowledge that it's doing something if running as main:
    print('Booting . . .')

from typing import Any, Final, Tuple, List, Dict
from dataclasses import dataclass
from datetime import datetime, timedelta
import os.path

from termcolor import colored

import scripts.testing._peregrine_tvac_fetcher as ptf

import numpy as np
import pandas as pd
import scapy.all as scp  # type: ignore

from matplotlib import pyplot as plt
from matplotlib import dates as mdates
from matplotlib import units as munits
from matplotlib import ticker as mticker
import seaborn as sns
plt.rcParams['text.usetex'] = False
sns.set()  # Initialize Seaborn. Use default theme.

DB_DIR: Final[str] = './out/databases'
# DB_FILE: Final[str] = 'int-tvac-data__backup_01GPYZVH3PEHC23MVEPW4A92JV.h5'
# PLOT_OUT_FILE: Final[str] = './out/databases/plots/__DIODE_LEAKAGE_PLOT.png'
DB_FILE: Final[str] = 'int-tvac-data2.h5'
PLOT_OUT_FILE: Final[str] = './out/databases/plots/__DIODE_LEAKAGE_PLOT_2.png'
DB_PATH: Final[str] = os.path.join(DB_DIR, DB_FILE)

print("Connecting to Local DB . . .")
db = pd.HDFStore(DB_PATH)

print("Loading Telem . . .")
telem = ptf.DataSet.load_from('telem', db)

print("Analyzing Data . . .")

# Collect all Iris on-board RT temperture readings and VBS readings:
battRTCelsius = telem.data['WatchdogHeartbeat_BattAdcTempKelvin'] - 273.15
vbs = telem.data['WatchdogDetailedStatus_Adc_SwitchedBatteryVoltage']

# Remove all NA values from each data Series (their still time-stamped):
battRTCelsius = battRTCelsius[~battRTCelsius.isna()]
vbs = vbs[~vbs.isna()]

# Since these telem items inherently come in at different times b/c they're in
# different packets, we need to perform correspondence...
# For every temperature reading (the more frequent of these two telem series),
# grab the nearest voltage reading in time and use that reading.
# NOTE: interpolation yielded bad results here because it would fill in data
# for discontinuities poorly (i.e. add lines were there weren't any)
df = pd.merge_asof(battRTCelsius, vbs, on='time', direction='nearest')

# Rename columns to make them easier to handle:
df.rename(columns={
    'WatchdogDetailedStatus_Adc_SwitchedBatteryVoltage': 'VBS',
    'WatchdogHeartbeat_BattAdcTempKelvin': 'battRTCelsius'
}, inplace=True)

# Plot the data:
print("Plotting Results . . .")

fsize = (14, 8)
fig, axs = plt.subplots(
    nrows=1,
    figsize=fsize,
    dpi=1200,
    constrained_layout=True
)
fig.suptitle(
    "Iris Diode-OR Leakage at High Temperature\n"
    "(Voltage at Battery Switch Output with Switch Off)"
)

df.set_index('battRTCelsius', inplace=True)
df['VBS'].plot(
    figsize=fsize,
    grid=True,
    xlabel='Battery Temperature (BattRT) [°C]',
    ylabel=(
        'Switch Output Voltage [V]\n'
        '(note: poor calibration at low voltages)'
    ),
    legend=False,
    marker='o',
    markersize=2,
    linestyle='none'  # just scatter plot these so we know where data is
)

# Plot the data:
print(f"Saving Results to {PLOT_OUT_FILE} . . .")
plt.savefig(PLOT_OUT_FILE, dpi=1200)
plt.close()
