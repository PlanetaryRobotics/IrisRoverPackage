"""
Quick and dirty script to investigate the relationship between Deck (D2)
temperature and internal Iris Battery Temperature during Peregrine TVAC.

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
DB_FILE: Final[str] = 'int-tvac-data.h5'
PLOT_OUT_FILE: Final[str] = './out/databases/plots/__DECK_TO_ROVER_TEMP.png'
TDOT_OUT_FILE: Final[str] = './out/databases/plots/__DECK_TO_ROVER_TDOT.png'
DB_PATH: Final[str] = os.path.join(DB_DIR, DB_FILE)

print("Connecting to Local DB . . .")
db = pd.HDFStore(DB_PATH)

print("Loading Telem . . .")
telem = ptf.DataSet.load_from('telem', db)

print("Analyzing Data . . .")

# Collect all Iris on-board RT temperture readings and VBS readings:
battRTDegC = telem.data['WatchdogHeartbeat_BattAdcTempKelvin'] - 273.15
deckD2DegC = telem.data['Peregrine_DeckD2TempKelvin'] - 273.15

# Remove all NA values from each data Series (they're still time-stamped):
battRTDegC = battRTDegC[~battRTDegC.isna()]
deckD2DegC = deckD2DegC[~deckD2DegC.isna()]

# Since these telem items inherently come in at different times b/c they're in
# different packets, we need to perform correspondence...
# For every deck reading (the more frequent of these two telem series),
# grab the nearest voltage reading in time and use that reading.
# NOTE: interpolation yielded bad results here because it would fill in data
# for discontinuities poorly (i.e. add lines were there weren't any)
df = pd.merge_asof(deckD2DegC, battRTDegC, on='time', direction='nearest')

# Rename columns to make them easier to handle:
df.rename(columns={
    'Peregrine_DeckD2TempKelvin': 'deckD2DegC',
    'WatchdogHeartbeat_BattAdcTempKelvin': 'battRTDegC'
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
    "Iris Battery Temperature vs Nearest Deck Temperature"
)

df_plot = df.set_index('deckD2DegC', inplace=False)
df_plot['battRTDegC'].plot(
    figsize=fsize,
    grid=True,
    xlabel='Peregrine Deck D2 Temperature [°C]',
    ylabel='Battery Temperature (BattRT) [°C]',
    legend=False,
    color='red',
    marker='o',
    markersize=2,
    linestyle='none'  # just scatter plot these so we know where data is
)

# Plot the data:
print(f"Saving Results to {PLOT_OUT_FILE} . . .")
plt.savefig(PLOT_OUT_FILE, dpi=1200)
plt.close()


# Make a second plot for Tdot:
print("Plotting Time Derivative . . .")
# Compute time derivatives:
# (do this separately, before merging data so operation is performed directly
# on observations)


def compute_TDot(series):
    """Computes Tdot for series in degC/hr"""
    # Drop any rows where the value hasn't changed:
    series = series.loc[series.diff() != 0]
    dt_ms = np.diff(series.index.values).astype('timedelta64[ms]')
    diff = series.diff()
    diff = diff[~diff.isna()]
    df = pd.DataFrame({
        'time': series.index[1:],
        series.name: diff.values / dt_ms.astype('int64') * 3_600_000
    })
    return df


battRTDegC_Tdot = compute_TDot(battRTDegC)
deckD2DegC_Tdot = compute_TDot(deckD2DegC)
df_Tdot = pd.merge_asof(
    battRTDegC_Tdot, deckD2DegC_Tdot,
    on='time',
    direction='nearest'
)
df_Tdot.rename(columns={
    'Peregrine_DeckD2TempKelvin': 'deckD2DegC',
    'WatchdogHeartbeat_BattAdcTempKelvin': 'battRTDegC'
}, inplace=True)

# Use MADD as a quick and dirty way to remove major outliers (from time jumps
# and discontinuities):
df_Tdot.set_index('time', inplace=True)
mad = (df_Tdot - df_Tdot.median()).abs().median()
madd = (df_Tdot - df_Tdot.median()).abs() / mad
df_Tdot = df_Tdot[madd < 3].dropna(how='any')

fsize = (14, 8)
fig, axs = plt.subplots(
    nrows=1,
    figsize=fsize,
    dpi=1200,
    constrained_layout=True
)
fig.suptitle(
    "Iris Battery Temperature Rate vs Nearest Deck Temperature Rate"
)

df_Tdot.set_index('deckD2DegC', inplace=True)
df_Tdot['battRTDegC'].plot(
    figsize=fsize,
    grid=True,
    xlabel='Peregrine Deck D2 Temperature Rate [C°/hr]',
    ylabel='Battery Temperature Rate (BattRT) [C°/hr]',
    legend=False,
    color='orange',
    marker='o',
    markersize=2,
    linestyle='none'  # just scatter plot these so we know where data is
)

# Plot the data:
print(f"Saving TDot Results to {TDOT_OUT_FILE} . . .")
plt.savefig(TDOT_OUT_FILE, dpi=1200)
plt.close()

fsize = (14, 8)
fig, axs = plt.subplots(
    nrows=1,
    figsize=fsize,
    dpi=1200,
    constrained_layout=True
)
fig.suptitle(
    "Iris Battery Temperature Rate vs Nearest Deck Temperature Rate"
)
df.plot.hexbin(
    x='deckD2DegC',
    y='battRTDegC',
    gridsize=25,
    grid=True,
    xlabel='Peregrine Deck D2 Temperature Rate [C°/hr]',
    ylabel='Battery Temperature Rate (BattRT) [C°/hr]'
)
print(f"Saving TDot Hexbin Results to {TDOT_OUT_FILE}_HEXBIN.png . . .")
plt.savefig(f"{TDOT_OUT_FILE}_HEXBIN.png", dpi=1200)
plt.close()
