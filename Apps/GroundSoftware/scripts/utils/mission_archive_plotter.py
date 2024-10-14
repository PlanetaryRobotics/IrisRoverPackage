"""
Makes plots from Mission archives made with the `IrisBackendv3.storage` layer,
e.g. `*.h5` or `*.parquet` (the same archive files that  can be used with
`archive_transceiver` and `archive_xcvr` for replay).

Author: Colombo, CMU
Last Updated: 10/14/2024
"""

import argparse
import IrisBackendv3 as IB3
import IrisBackendv3.ipc as ipc

import os  # noqa

import math

from IrisBackendv3.storage.logs import logger_setConsoleLevel as setStorageLogLevel
from IrisBackendv3.storage.settings import settings as STOR_SETTINGS
from IrisBackendv3.codec.logs import logger_setConsoleLevel as setCodecLogLevel

from IrisBackendv3.storage.dataset import DataSet

from IrisBackendv3.logs import VALID_LOG_LEVELS

from IrisBackendv3.storage.dataset import DataSet
from scripts.utils.plotting_tools import *
from scripts.utils.plotting_tools import _reset_colors
from scripts.utils.fm1_plot_formatting import *

from typing import Any, Final, Tuple, List, Dict, Optional, TypeAlias, cast
from datetime import datetime
import os.path

import argparse

from rich.progress import track

import numpy as np
import pandas as pd  # type: ignore
import time

import pickle


def get_opts(
    description: str = 'IRIS Lunar Rover — Mission YAMCS Export Archive Generator',
    default_log_level: str = 'DEBUG'
):
    """
    Return CLI options wrapped in argparse.
    """
    def str_to_log_level(s):
        if isinstance(s, str) and s.upper() in VALID_LOG_LEVELS:
            return s.upper()
        else:
            raise argparse.ArgumentTypeError(
                f'Valid log level expected. Log levels are: {VALID_LOG_LEVELS}'
            )

    parser = argparse.ArgumentParser(
        description=description
    )

    parser.add_argument('-f', '--telem-archive-file', type=str,
                        default='./out/fm1_data/yamcs_export/fm1_mission_archive.telem.parquet',
                        help=r'Path to the telemetry archive file.')
    parser.add_argument('-o', '--out-dir', type=str,  default='./out/fm1_data/plots/',
                        help=r'Output directory.')
    parser.add_argument('-t', '--plot-title', type=str,
                        # Need to use nargs+ to parse strings with spaces
                        nargs='+', default='Iris Lunar Rover'.split(' '),
                        help=r'Title for the plots.')
    parser.add_argument('-p', '--file-prefix', type=str, default='iris_plot',
                        help=r'Prefix for output files.')
    parser.add_argument('-e', '--plot-file-extensions', type=str,
                        nargs='+', default=['png'],
                        help=r'File extensions (and types) used for plot images.')
    parser.add_argument('-d', '--plot-dpis', type=float, nargs='+', default=[300],
                        help=r'DPIs (resolutions) for plots. 300 is normal. '
                        '600 is high quality. 1200 is *very* detailed. '
                        'NOTE: File size and generation time grow '
                        'exponentially with DPI.')

    parser.add_argument('--plot-start-time-utc', default="2020-01-01",
                        help="Format: `2024/1/15` or `2024/1/15T23:55:59`. "
                        "Earliest time to allow in "
                        "the plot (UTC) or `None` for no limits on start time. "
                        "Default is Jan. 1, 2020 (before start) because "
                        "some data in the archive was erroneously "
                        "labelled as being from 2001, so we need to not "
                        "plot that.")
    parser.add_argument('--plot-end-time-utc', default=None,
                        help="Format: `YYYY-MM-DD` or `YYYY-MM-DD HH:MM:SS`. "
                        "Latest time to allow in the plot (UTC).")

    parser.add_argument('-s', '--max-plot-time', default=None,
                        help="Format: `HH:MM:SS`. "
                        "Longest time range allowed per plot "
                        "(or `NONE` for no limit). "
                        "If time duration is longer than this, multiple "
                        "consecutive plots will be produced. ")

    parser.add_argument('--make-category-plots', default=True,
                        action=argparse.BooleanOptionalAction,
                        help="Make individual plots for each category."
                        )
    parser.add_argument('--skip-blank-categories', default=True,
                        action=argparse.BooleanOptionalAction,
                        help="Whether to skip making plots for categories "
                        "with no data during the plotted time window."
                        )
    parser.add_argument('--make-strip-charts', default=True,
                        action=argparse.BooleanOptionalAction,
                        help="Whether to make strip chart(s)."
                        )
    parser.add_argument('--make-diode-leakage-plots', default=True,
                        action=argparse.BooleanOptionalAction,
                        help="Whether to make diode leakage plot(s)."
                        )

    parser.add_argument('--plot-thermal', default=True,
                        action=argparse.BooleanOptionalAction,
                        help="Whether to make thermal plots."
                        )
    parser.add_argument('--plot-voltages', default=True,
                        action=argparse.BooleanOptionalAction,
                        help="Whether to make voltage plots."
                        )
    parser.add_argument('--plot-power', default=True,
                        action=argparse.BooleanOptionalAction,
                        help="Whether to make power plots."
                        )
    parser.add_argument('--plot-currents', default=True,
                        action=argparse.BooleanOptionalAction,
                        help="Whether to make current plots."
                        )
    parser.add_argument('--plot-battery', default=True,
                        action=argparse.BooleanOptionalAction,
                        help="Whether to make battery plots."
                        )
    parser.add_argument('--plot-states', default=True,
                        action=argparse.BooleanOptionalAction,
                        help="Whether to make states plots."
                        )
    parser.add_argument('--plot-imu', default=True,
                        action=argparse.BooleanOptionalAction,
                        help="Whether to make IMU (inertial) plots."
                        )
    parser.add_argument('--plot-packets', default=True,
                        action=argparse.BooleanOptionalAction,
                        help="Whether to make packets plots."
                        )
    parser.add_argument('--plot-data-use', default=True,
                        action=argparse.BooleanOptionalAction,
                        help="Whether to make plots of data utilization."
                        )

    parser.add_argument('--log-level', type=str_to_log_level, default=default_log_level,
                        help=('Logging level to be used (i.e. how annoying the logging '
                              'printouts should be). Only logs with importance greater '
                              'than or equal to the specified logging level are '
                              f'displayed. Valid logging levels are: {VALID_LOG_LEVELS}'))

    opts = parser.parse_args()
    opts.plot_title = ' '.join(opts.plot_title)
    return opts


def save_figure_wrapper(
    settings: PlotSettings,
    axs: List[Axis],
    save_dir_base: str,
    group_title: str,
    file_title: str,
    exts: List[str],
    dpis: List[int],
    dont_close: bool = False
) -> None:
    """Wrapper for `save_figure` that adds special features used for this
    plotting activity."""
    settings.title = file_title
    for ext in exts:
        not_vector = ext.lower() not in ['svg', 'pdf', 'eps']
        for dpi in (dpis if not_vector else [max(dpis)]):
            settings.plot_ext = ext
            settings.save_dir = os.path.join(
                save_dir_base,
                (f"{ext}__dpi{dpi}" if not_vector else ext),
                (f"{group_title}__dpi{dpi}" if not_vector else group_title)
            )
            done: bool = False
            while not done:
                try:
                    save_figure(
                        settings, axs, dont_close=True, ext_in_path=False,
                        dpi_override=dpi
                    )
                    done = True
                except ValueError as ve:
                    # Catch figure sizing errors b/c some plots are *very* big:
                    if 'pixels is too large' not in str(ve).lower():
                        # Not a sizing error:
                        raise ve
                    if dpi > 100:
                        next_dpi = max(dpi//2, dpi-100)
                        app.logger.warning(
                            f"\t > Figure `{settings.title}` with {ext=} "
                            f"too big to save at {dpi=} due to sizing "
                            f"constraint. "
                            f"Reducing from `{dpi=}` to `{next_dpi}` and "
                            f"retrying."
                        )
                        dpi = next_dpi
                    else:
                        app.logger.error(
                            f"Failed to save figure `{settings.title}` "
                            f"with {ext=}, {dpi=} due to sizing "
                            f"constraint, even after reducing to `{dpi=}`. "
                            f"Full error: `{ve!s}`. "
                            f"Skipping . . ."
                        )
                        done = True
    if not dont_close:
        plt.close()


def make_all_plots(opts, app: ipc.IpcAppHelper) -> None:
    TITLE: str = opts.plot_title
    FILES_TITLE: str = opts.file_prefix
    DPIS = [int(float(str(d))) for d in opts.plot_dpis]
    app.logger.notice(
        f"Making `{TITLE}` plots as `{FILES_TITLE}` with {DPIS=}."
    )

    start_time: datetime | None = None
    end_time: datetime | None = None

    if (
        opts.plot_start_time_utc is not None
        # check for text in case someone misread the help and typed it out:
        and str(opts.plot_start_time_utc).upper() != "NONE"
    ):
        start_time = pd.to_datetime(
            str(opts.plot_start_time_utc), utc=True).to_pydatetime()
    if (
        opts.plot_end_time_utc is not None
        # check for text in case someone misread the help and typed it out:
        and str(opts.plot_end_time_utc).upper() != "NONE"
    ):
        end_time = pd.to_datetime(
            str(opts.plot_end_time_utc), utc=True).to_pydatetime()

    TIME_SPLIT: timedelta | None = None
    if (
        opts.max_plot_time is not None
        # check for text in case someone misread the help and typed it out:
        and str(opts.max_plot_time).upper() != "NONE"
    ):
        pieces = opts.max_plot_time.strip().split(':')
        hours, minutes, seconds = 0.0, 0.0, 0.0
        if len(pieces) > 0:
            hours = abs(float(pieces[0]))
        if len(pieces) > 1:
            minutes = abs(float(pieces[1]))
        if len(pieces) > 2:
            seconds = abs(float(pieces[2]))
        if hours > 0 or minutes > 0 or seconds > 0:
            TIME_SPLIT = timedelta(
                hours=hours, minutes=minutes, seconds=seconds
            )

    app.logger.notice(
        f"Loading Mission Archive from {opts.telem_archive_file} . . ."
    )
    # Load Data Archive:
    full_telem_archive = DataSet.load_from_file(
        opts.telem_archive_file, STOR_SETTINGS['TELEM_KEY']
    )
    telem_full = full_telem_archive.data
    telem_full.shape

    app.logger.notice(
        f"Parsing Telemetry . . ."
    )
    data = RoverPlotData(telem_full, start_time=start_time, end_time=end_time)
    # Re-assess start and end times per actual data:
    start_time_data = cast(datetime, min(data.telem.index))
    end_time_data = cast(datetime, max(data.telem.index))

    app.logger.success("All data loaded!")

    full_time_window = end_time_data - start_time_data
    n_time_plots: int
    if TIME_SPLIT is not None:
        n_time_plots = math.ceil(full_time_window / TIME_SPLIT)
    else:
        n_time_plots = 1
        TIME_SPLIT = full_time_window

    exts = opts.plot_file_extensions
    app.logger.notice(f"Making Plots ({n_time_plots}) . . .")
    for time_idx in range(n_time_plots):
        plot_start = start_time_data + TIME_SPLIT*time_idx
        plot_end = start_time_data + TIME_SPLIT*(time_idx + 1)
        files_suffix: str = ""
        if n_time_plots > 1:
            files_suffix += str(time_idx+1)
        # Re-fetch data:
        data = RoverPlotData(
            telem_full, start_time=plot_start, end_time=plot_end
        )
        app.logger.info(f"Plotting from {plot_start} to {plot_end} . . .")
        # Default plot config:
        cfg = PlotSettings(
            TITLE,
            FILES_TITLE + files_suffix,
            dpi=max(DPIS),
            start_time=plot_start, end_time=plot_end,
            plot_ext=exts[0]
        )

        thermal_close_up_ignore: List[str] = [
            'TMR1', 'TMR2', 'TMR8', 'TMR9', 'TMR10',
            'Heater ON Thresh.', 'Heater OFF Thresh.'
        ]
        th2_ignore = [
            x for x in thermal_close_up_ignore if x in data.thermal.columns
        ]
        thermal_close = data.thermal.drop(columns=th2_ignore, inplace=False)

        def _plot_thermal_closeup(
            cfg: PlotSettings, axs: List[Axis], plot_idx: int
        ) -> None:
            PLOT_TH2: int = plot_idx
            AX_TH2 = data.plot_thermal(cfg, axs, PLOT_TH2)[0]
            # Remove columns after plotting all of them so colors match full thermal plot:
            hide_lines(cfg, axs[AX_TH2], th2_ignore)
            # Rescale y, ignoring hidden columns:
            set_ylim_from_df(axs[AX_TH2], thermal_close)
            add_degC_y_axis(axs, AX_TH2)
            add_legends(
                cfg, axs,
                position_overrides={PLOT_TH2: 'upper left'},
                ignore_labels={PLOT_TH2: th2_ignore}
            )

        voltage_close_up_ignore: List[str] = ['2V8', '2V5']
        v2_ignore = [
            x for x in voltage_close_up_ignore if x in data.voltages.columns
        ]
        voltage_close = data.voltages.drop(
            columns=v2_ignore, inplace=False)
        for c in voltage_close.columns:
            voltage_close[c] = voltage_close[c].where(
                voltage_close[c] > 20)

        def _plot_voltage_closeup(
            cfg: PlotSettings, axs: List[Axis], plot_idx: int
        ) -> None:
            PLOT_V2: int = plot_idx
            AX_V2 = data.plot_voltages(cfg, axs, PLOT_V2)[0]
            # Remove columns after plotting all of them so colors match full thermal plot:
            hide_lines(cfg, axs[AX_V2], v2_ignore)
            # Rescale y, ignoring hidden columns:
            set_ylim_from_df(axs[AX_V2], voltage_close)
            add_legends(
                cfg, axs,
                position_overrides={PLOT_V2: 'upper left'},
                ignore_labels={PLOT_V2: v2_ignore}
            )

        if opts.make_category_plots:
            cat_plot_configs: List[Tuple[
                str, bool, List[pd.DataFrame],
                Callable[[PlotSettings, List[Axis], int, Figure], Any], bool
            ]] = [
                (
                    "Thermal", opts.plot_thermal, [data.thermal],
                    lambda c, a, i, f: data.plot_thermal(c, a, i), True
                ),
                (
                    "Thermal_Closeup", opts.plot_thermal, [thermal_close],
                    lambda c, a, i, f: _plot_thermal_closeup(c, a, i), False
                ),
                (
                    "Power", opts.plot_power, [data.power],
                    lambda c, a, i, f: data.plot_power(c, a, i), True
                ),
                (
                    "Current", opts.plot_currents, [data.currents],
                    lambda c, a, i, f: data.plot_currents(c, a, i), True
                ),
                (
                    "Voltage", opts.plot_voltages, [data.voltages],
                    lambda c, a, i, f: data.plot_voltages(c, a, i), True
                ),
                (
                    "Voltage_Closeup", opts.plot_voltages, [voltage_close],
                    lambda c, a, i, f: _plot_voltage_closeup(c, a, i), False
                ),
                (
                    "Battery", opts.plot_battery, [data.battery],
                    lambda c, a, i, f: data.plot_battery(c, a, i), True
                ),
                (
                    "State", opts.plot_states, [data.states_bound],
                    lambda c, a, i, f: data.plot_states(c, a, i, f), True
                ),
                (
                    "Packet", opts.plot_packets,
                    [data.ul_packets, data.dl_packets],
                    lambda c, a, i, f: data.plot_packets(c, a, i), True
                ),
                (
                    "Data_Utilization", opts.plot_data_use,
                    [data.data, data.data_rate],
                    lambda c, a, i, f: data.plot_data_fields(c, a, i), True
                ),
                (
                    "IMU", opts.plot_imu,
                    [data.inertial, data.angle],
                    lambda c, a, i, f: data.plot_inertial(c, a, i), True
                ),
            ]
            for name, make, dfs, func, incl_legend in track(
                cat_plot_configs, "Categorical Plots"
            ):
                _reset_colors(cfg)
                app.logger.verbose(f"\t > Plotting {name} . . .")
                app.logger.debug(f"{name} has {[df.size for df in dfs]}")
                if (
                    make and not (
                        opts.skip_blank_categories and
                        all(df.size == 0 for df in dfs)
                    )
                ):
                    cfg.suptitle = f"{TITLE} {files_suffix} - {name} Data"
                    fig, axs = create_figure(cfg)
                    func(cfg, axs, 0, fig)
                    if incl_legend:
                        add_legends(cfg, axs)
                    # y is handled by plotter
                    tidy_figure(cfg, axs, skip_y_align=True)
                    save_figure_wrapper(
                        cfg, axs, opts.out_dir,
                        (FILES_TITLE+files_suffix),
                        f"{FILES_TITLE}{files_suffix}__{name.lower()}",
                        exts, DPIS
                    )
                    plt.close()

        if opts.make_diode_leakage_plots:
            app.logger.verbose("Making Diode Leakage Plot . . .")
            dlp = make_diode_leakage_plot(
                data,
                f"{FILES_TITLE}{files_suffix}__diode_leakage",
                start_time=start_time,
                end_time=end_time,
                dpi=max(DPIS)
            )
            if dlp is None:
                app.logger.debug(" > No data to plot.")
            else:
                dlp_cfg, fig, axs = dlp
                save_figure_wrapper(
                    dlp_cfg, axs, opts.out_dir,
                    (FILES_TITLE+files_suffix),
                    f"{FILES_TITLE}{files_suffix}__diode_leakage",
                    exts, DPIS
                )
                plt.close()

        if opts.make_strip_charts:
            # Make two strip plots, one with and one without min. plots
            # Min plots will be easier to read. Full plots will be able to be
            # lined up in a row.
            def _make_strip_chart_wrapper(
                dpi: int, suffix: str, skip_empty_plots: bool
            ) -> Tuple[PlotSettings, Figure, List[Axis]] | None:
                try:
                    return make_strip_chart(
                        data,
                        TITLE + files_suffix,
                        FILES_TITLE + files_suffix + suffix,
                        start_time=start_time,
                        end_time=end_time,
                        skip_empty_plots=skip_empty_plots,
                        plot_thermal=opts.plot_thermal,
                        plot_voltages=opts.plot_voltages,
                        plot_power=opts.plot_power,
                        plot_currents=opts.plot_currents,
                        plot_battery=opts.plot_battery,
                        plot_states=opts.plot_states,
                        plot_imu=opts.plot_imu,
                        plot_packets=opts.plot_packets,
                        plot_data=opts.plot_data_use,
                        dpi=dpi
                    )
                except ValueError as ve:
                    # Catch figure sizing errors b/c this is one big boy.
                    if 'pixels is too large' not in str(ve).lower():
                        # Not a sizing error:
                        raise ve
                    if dpi > 100:
                        next_dpi = max(dpi//2, dpi-100)
                        app.logger.warning(
                            f"\t > Strip figure too large to plot. "
                            f"Reducing from `{dpi=}` to `{next_dpi}` and retrying."
                        )
                        return _make_strip_chart_wrapper(
                            next_dpi, suffix, skip_empty_plots
                        )
                    else:
                        app.logger.error(
                            f"Failed to make strip plot due to sizing "
                            f"constraint, even after reducing to `{dpi=}`. "
                            f"Full error: `{ve!s}`. "
                            f"Skipping . . ."
                        )
                        return None
            app.logger.verbose("Making Full Strip Plot . . .")
            strip = _make_strip_chart_wrapper(
                max(DPIS), "__strip", skip_empty_plots=False
            )
            if strip is None:
                app.logger.verbose(" > No strips to plot.")
            else:
                strip_cfg, fig, axs = strip
                save_figure_wrapper(
                    strip_cfg, axs, opts.out_dir,
                    (FILES_TITLE+files_suffix),
                    (FILES_TITLE + files_suffix + "__strip"),
                    exts, DPIS
                )
                plt.close()

            if opts.skip_blank_categories:
                app.logger.verbose("Making Min Strip Plot . . .")
                strip = _make_strip_chart_wrapper(
                    max(DPIS), "__strip_min", skip_empty_plots=True
                )
                if strip is None:
                    app.logger.debug(" > No strips to plot.")
                else:
                    strip_cfg, fig, axs = strip
                    save_figure_wrapper(
                        strip_cfg, axs, opts.out_dir,
                        (FILES_TITLE+files_suffix),
                        (FILES_TITLE + files_suffix + "__strip_min"),
                        exts, DPIS
                    )
                    plt.close()

        app.logger.success(
            f"Plots from {plot_start} to {plot_end} "
            f"saved to `{opts.out_dir}`."
        )

    app.logger.success("All plots complete!")


if __name__ == "__main__":
    opts = get_opts()
    app = ipc.IpcAppHelper("MissionArchivePlotter")
    app.setLogLevel(opts.log_level)
    setStorageLogLevel(opts.log_level)
    # Set Codec Logger to Error+ b/c there will be a glut of warnings for the
    # infrequent corrupted packets:
    setCodecLogLevel('ERROR')

    app.logger.notice(f"Making all plots:")
    make_all_plots(opts, app)
