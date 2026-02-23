import asyncio
from typing import cast, Union

import numpy as np
import matplotlib.pyplot as plt  # type: ignore

from termcolor import cprint
import ulid
import time

from IrisBackendv3.codec.payload import FileBlockPayload
from scripts.tiny_apps.parse_pcap import parse_pcap

import attr

import operator as op
import numpy as np
import pandas as pd  # type: ignore
import requests  # type: ignore
import panel as pn  # type: ignore
import hvplot.pandas  # type: ignore
import hvplot.streamz  # type: ignore
import holoviews as hv  # type: ignore
from holoviews.element.tiles import EsriImagery  # type: ignore
from holoviews.selection import link_selections  # type: ignore
from datashader.utils import lnglat_to_meters  # type: ignore
from streamz import Stream  # type: ignore
from streamz.dataframe import DataFrame as StreamingDataFrame  # type: ignore

# Load fake data:


@attr.s(auto_attribs=True)
class PcapParseOpts:
    pcap_file: str = './test-data/Iris_210503_18_hours_of_telem.pcap'
    port: Union[str, int] = 'any'
    cache_dir: str = './out/pcap_logs/'
    cache_prefix: str = f'iris_pcap_{ulid.new()}'
    cache_telem: bool = True
    cli_log: bool = False
    plot: bool = False
    packetgap: int = 36000
    deadspace: int = 0
    log_level: str = 'INFO'


pcap_opts = PcapParseOpts()

pcap_data = parse_pcap(pcap_opts)

print("Setting up streams . . .")
# Build streams:
stream = Stream()
index = pd.DatetimeIndex([])
example = pd.DataFrame({'x': [], 'y': [], 'z': []},
                       columns=['x', 'y', 'z'], index=[])
df = StreamingDataFrame(stream, example=example)

# Setup plots:
plot_x = df.x.hvplot.line(title='XAcc', backlog=50)
plot_y = df.y.hvplot.line(title='YAcc', backlog=50)
plot_z = df.z.hvplot.line(title='ZAcc', backlog=50)


def update(i):
    df_new = pd.DataFrame({
        'x': [v for _, v in pcap_data['telemetry_streams']['Imu_XAcc']][:(i+1)*10],
        'y': [v for _, v in pcap_data['telemetry_streams']['Imu_YAcc']][:(i+1)*10],
        'z': [v for _, v in pcap_data['telemetry_streams']['Imu_ZAcc']][:(i+1)*10]
    }, columns=['x', 'y', 'z'], index=[pd.datetime.now() for _ in range((i+1)*10)])

    stream.emit(df_new)


pn_rt = pn.Column(
    pn.Row(
        plot_x,
        plot_y,
        plot_z
    )
)
pn_rt.servable()
pn_rt


async def main():
    for i in range(20):
        await asyncio.sleep(2)
        update(i)
        print(f'updated {i}')

asyncio.create_task(main())
