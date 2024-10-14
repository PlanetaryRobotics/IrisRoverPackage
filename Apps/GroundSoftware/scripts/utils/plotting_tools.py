"""
Tools for creating uniform plots of Rover data.

@author: Connor Colombo (CMU)
@last-updated: 10/13/2024
"""

import numpy as np
import pandas as pd  # type: ignore

from typing import Any, Tuple, List, Dict, Iterable
from datetime import datetime, timezone, timedelta
from collections import deque, OrderedDict
from dataclasses import dataclass, field
import os.path

import ulid
from matplotlib import pyplot as plt  # type: ignore
import matplotlib.colors  # type: ignore
from matplotlib import dates as mdates  # type: ignore
from matplotlib.figure import Figure  # type: ignore
from matplotlib.axes import Axes  # type: ignore
from matplotlib.axis import Axis  # type: ignore
from matplotlib import units as munits  # type: ignore
from matplotlib import ticker as mticker  # type: ignore
import seaborn as sns  # type: ignore
plt.rcParams['text.usetex'] = False
# plt.rcParams["text.latex.preamble"] = (r'\usepackage{xfrac}')
sns.set()  # Initialize Seaborn. Use default theme.

# Plotting Helper Functions:
# colors_set = ['#1f77b4', '#ff7f0e', '#2ca02c', '#d62728', '#9467bd',
#                 '#8c564b', '#e377c2', '#7f7f7f', '#bcbd22', '#17becf']
_c_tab20 = plt.get_cmap('tab20')(np.arange(20, dtype=int))
colors_set = [matplotlib.colors.to_hex(c[:3]) for c in _c_tab20]


@dataclass
class PlotSettings:
    """Configuration & Context Container for Plots"""
    suptitle: str = "Iris Lunar Rover"
    title: str = ""
    num_subplots: int = 1
    # Which axes (axis indices) are on which plots (plot indices):
    plot_axes: OrderedDict[int, List[int]] = field(
        default_factory=lambda: OrderedDict([(0, [0])])
    )
    start_time: datetime | None = None
    end_time: datetime | None = None
    save_dir: str = "./out/fm1_data/plots"
    plot_ext: str = "png"
    dpi: int = 1200
    figsize: Tuple[float, float] = (14, 8)
    colors_set = colors_set
    colors = deque(colors_set)  # active order


# Set up date formatting:
time_locator = mdates.AutoDateLocator()
time_formatter = mdates.ConciseDateFormatter(time_locator)
time_formatter.formats = ['%y',       # ticks are mostly years
                          '%b',       # ticks are mostly months
                          '%d',       # ticks are mostly days
                          '%H:%M',    # hrs
                          '%H:%M',    # min
                          '%S.%f', ]  # secs
# these are mostly just the level above...
time_formatter.zero_formats = [''] + time_formatter.formats[:-1]
# ...except for ticks that are mostly hours, then it is nice to have
# month-day:
time_formatter.zero_formats[3] = '%d-%b'
time_formatter.offset_formats = ['UTC%z',
                                 '%Y UTC%z',
                                 '%b %Y UTC%z',
                                 '%d %b %Y UTC%z',
                                 '%d %b %Y UTC%z',
                                 '%d %b %Y %H:%M UTC%z']

# And add some data formatters:


def _datasize_func(x, pos):
    i = int(np.log(abs(x))/np.log(1000)) if x != 0 else 0
    if i == 0:
        return f"{x/(1000**i):3.0f} bytes"
    suffix = ['bytes', 'kB', 'MB', 'GB', 'TB'][i]
    return f"{x/(1000**i):3.2f} {suffix}"


datasize_formatter = mticker.FuncFormatter(_datasize_func)


def _datarate_func(x, pos):
    i = int(np.log(abs(x))/np.log(1000)) if x != 0 else 0
    suffix = ['bps', 'kbps', 'Mbps'][i]
    return f"{x/(1000**i):3.2f} {suffix}"


datarate_formatter = mticker.FuncFormatter(_datarate_func)


def create_figure(settings: PlotSettings) -> Tuple[Figure, List[Axis]]:
    fig, axs = plt.subplots(
        nrows=settings.num_subplots,
        figsize=settings.figsize,
        dpi=settings.dpi,
        constrained_layout=True
    )
    fig.suptitle(settings.suptitle)
    if isinstance(axs, np.ndarray):
        axs = axs.tolist()  # convert from numpy array
    elif isinstance(axs, Iterable):
        axs = [x for x in axs]
    else:
        axs = [axs]
    # Assign default axes to each plot (by default):
    for plot_idx in range(settings.num_subplots):
        settings.plot_axes[plot_idx] = [plot_idx]
    return fig, axs


def add_second_y_axis(
    axs: List[Axis],
    idx: int,
    num_extra_y_axes: int = 1
) -> List[int]:
    """Returns a list of the indices of the new axes"""
    axis = axs[idx]
    new_axes: List[int] = []
    for i in range(num_extra_y_axes):
        axs.append(axis.twinx())
        axs[-1].spines['right'].set_position(('axes', 1.0 + i * 0.07))
        new_axes.append(len(axs)-1)
    return new_axes


def _degK_to_degCLabel(tempK: float, *_) -> str:
    return f"{(tempK - 273.15):.1f}"


def add_degC_y_axis(axs: List[Axis], idx: int) -> None:
    # Adds a secondary axis for displaying temps in degC to the axis with the
    # given index
    ax_degK = axs[idx]
    ax_degK.set_ylabel('Temperature [K]')
    # Create a clone secondary axis (unity conversion functions) so that the
    # grid lines & ticks remain in exactly the same place.
    ax_degC = ax_degK.secondary_yaxis(
        'right',
        functions=(lambda T: T, lambda T: T)
    )
    # Then just use a label formatting function to change what we display:
    ax_degC.yaxis.set_major_formatter(
        mticker.FuncFormatter(_degK_to_degCLabel))
    ax_degC.set_ylabel('Temperature [°C]')


def _get_timebound_df(
    settings: PlotSettings,
    df: pd.DataFrame
) -> pd.DataFrame:
    if settings.start_time is not None:
        df = df[(df.index >= pd.to_datetime(settings.start_time, utc=True))]
    if settings.end_time is not None:
        df = df[(df.index <= pd.to_datetime(settings.end_time, utc=True))]
    return df


def _preprocess_plot_data(
    settings: PlotSettings,
    df: pd.DataFrame,
    skip_timebound: bool = False
) -> pd.DataFrame:
    if not skip_timebound:
        df = _get_timebound_df(settings, df)
    # Remove all-nan columns:
    nan_less = df.dropna(axis=1, how='all')
    # Only use nan-less if we still have data left, so we don't end up with an
    # unlabelled empty plot:
    if len(nan_less.columns) == 0:
        return df
    else:
        return nan_less


def plot_interp(
    settings: PlotSettings,
    axis: Axis,
    xy_label: Tuple[str, str],
    df: pd.DataFrame,
    **kwargs
) -> None:
    """DRY definition of how to handle interpolated background plots:"""
    df = _preprocess_plot_data(settings, df)
    if len(df.columns) == 0:
        return  # nothing to do
    df_interp = df.interpolate(method='linear', inplace=False)
    df_interp.plot(
        figsize=settings.figsize,
        grid=True,
        ax=axis,
        xlabel=xy_label[0],
        ylabel=xy_label[1],
        legend=False,
        color=[*settings.colors],
        **{
            **dict(
                marker='.',
                markersize=1,
                linestyle='none'
            ),
            **kwargs
        }
    )
    settings.colors.rotate(-len(df.columns))  # advance color cycle


def plot_main(
    settings: PlotSettings,
    axis: Axis,
    xy_label: Tuple[str, str],
    df: pd.DataFrame,
    **kwargs
) -> None:
    """DRY definition of how to handle main plots:"""
    df = _preprocess_plot_data(settings, df)
    if len(df.columns) == 0:
        return  # nothing to do
    df.plot(
        figsize=settings.figsize,
        grid=True,
        ax=axis,
        xlabel=xy_label[0],
        ylabel=xy_label[1],
        legend=False,
        color=[*settings.colors],
        **{
            **dict(
                marker='o',
                markersize=2,
                linestyle='none'
            ),
            **kwargs
        }
    )

    settings.colors.rotate(-len(df.columns))  # advance color cycle


def _flag_nan_runs(
    series: pd.Series,
    threshold: timedelta,
    flag='RUN-FLAG',
    inplace: bool = False
) -> pd.Series:
    """
    Detects runs of consecutive uninterrupted `NaN` values in the given
    `series` that last longer specified `threshold` timedelta.

    A series is returned where the first and last value of each run are
    replaced with `flag`. The original series is unaltered unless
    `inplace=True`.
    """
    if not inplace:
        series = series.copy(deep=True)

    # Find the boundaries of runs:
    is_nan = series.isna()
    nan_start = is_nan & (~is_nan.shift(1, fill_value=False))
    nan_start_times = series.index[nan_start]
    nan_end = is_nan & (~is_nan.shift(-1, fill_value=False))
    nan_end_times = series.index[nan_end]

    # Filter runs >= threshold:
    long_run_mask = (nan_end_times - nan_start_times) >= threshold

    # Flag starts and ends:
    series.loc[nan_start_times[long_run_mask]] = flag
    series.loc[nan_end_times[long_run_mask]] = flag

    return series


def plot_state_timeline(
    settings: PlotSettings,
    fig: Figure,
    axis: Axis,
    df: pd.DataFrame,
    blank_interval: timedelta = timedelta(minutes=45),
    bar_height: float = 1.0,
    label_fit_factor: float = 33.3,
    label_props=dict(
        ha='center',
        va='center',
        color='white',
        fontsize=8,
        fontweight='bold'
    )
) -> None:
    """Plots a Categorical Grafana-Style State Timeline w/ state labels."""
    df = _preprocess_plot_data(settings, df, skip_timebound=True)

    # Grab data from slightly outside the specified time window so we can fill
    # from outside the window in case the last state update was outside the
    # window:
    # (the biggest time delta that will matter is the `blank_interval`)
    if settings.start_time is not None:
        start_time = settings.start_time - 1.01*blank_interval
        df = df[(df.index >= pd.to_datetime(start_time, utc=True))]
    if settings.end_time is not None:
        end_time = settings.end_time + 1.01*blank_interval
        df = df[(df.index <= pd.to_datetime(end_time, utc=True))]

    # Add a blank entry to the start and end of any interval longer than
    # `blank_interval` (flagging happens later w/ `_flag_nan_runs`):
    BLANK_KEY = '--plot_state_timeline--NODATA'  # very unlikely to be in data

    # Collect all unique values, in order of appearance, column-by-column:
    col_uniques = [df[c].unique() for c in df.columns]
    unique_values = [*{
        x: None for c in col_uniques for x in c
        if x is not None and not (isinstance(x, np.floating) and np.isnan(x))
    }.keys()]

    # Map a color set:
    n_repeats: int = int(
        np.ceil(len(unique_values) / len(settings.colors_set)))
    colors = (settings.colors_set * n_repeats)[:len(unique_values)]

    # Define the colors for each state:
    state_colors = {v: c for v, c in zip(unique_values, colors)}

    # Apply special colors for boolean ints:
    state_colors[BLANK_KEY] = 'white'
    state_colors[False] = 'rosybrown'
    state_colors[0.0] = 'rosybrown'
    state_colors[True] = 'mediumspringgreen'
    state_colors[1.0] = 'mediumspringgreen'
    state_colors[2.0] = 'darkslategrey'

    # Iterate over each row name in the plot (column in df) to create bars:
    for row_idx, name in enumerate(df.columns):
        # Flag runs:
        col_data = _flag_nan_runs(df[name], blank_interval, BLANK_KEY)
        # Filter out null values
        col_data = col_data.dropna()
        # Only consider state changes (remove unchanged-values):
        # (but make sure the last value is always kept):
        col_data_end = col_data[-1:]
        col_data = col_data[:-1]
        col_data = col_data[col_data != col_data.shift(periods=1)]
        col_data = pd.concat((col_data, col_data_end)).squeeze(axis=0)

        for data_idx in range(len(col_data) - 1):
            state = col_data.iloc[data_idx]
            if state == BLANK_KEY:
                continue
            start_time = col_data.index[data_idx]
            end_time = col_data.index[data_idx + 1]
            axis.barh(
                y=name,
                width=end_time - start_time,
                left=start_time,
                height=bar_height,
                color=state_colors.get(state, 'white'),
                label=state if state != BLANK_KEY else "(No Data)",
                align='center'
            )

            # Create an invisible text object to calculate its width:
            text_label = state if state != BLANK_KEY else ""
            test_text = axis.text(
                start_time + (end_time - start_time) / 2,
                row_idx,
                text_label,
                **label_props,
                alpha=0
            )
            bbox = test_text.get_window_extent(
                renderer=fig.canvas.get_renderer())
            text_width_in_pixels = bbox.width

            # Convert bar width to pixels using date2num for datetime conversion
            start_num = mdates.date2num(start_time)
            end_num = mdates.date2num(end_time)
            bar_width_in_pixels = (
                axis.transData.transform((end_num, 0))[0]
                - axis.transData.transform((start_num, 0))[0]
            )

            # Add label only if text width fits within the bar width:
            test_text.remove()
            if text_width_in_pixels <= bar_width_in_pixels * label_fit_factor:
                axis.text(
                    start_time + (end_time - start_time) / 2,
                    row_idx,
                    text_label,
                    **label_props
                )

    # Make sure X is temporal:
    axis.xaxis_date()

    # Clip X to make sure it's now not outside of the target time bounds:
    _align_x_axes(settings, [axis])


def add_legends(
    settings: PlotSettings,
    axs: List[Axis],
    position_overrides: Dict[int, str] | None = None,
    ignore_labels: Dict[int, List[str]] | None = None,
    ignore_plots: List[int] | None = None
) -> None:
    # Build and apply legends to each plot:
    for plot_idx, ax_idxs in settings.plot_axes.items():
        if ignore_plots is not None and plot_idx in ignore_plots:
            continue
        handles, legends = [], []
        for ax_idx in ax_idxs:
            hdl, leg = axs[ax_idx].get_legend_handles_labels()
            handles.extend(hdl)
            legends.extend(leg)
        # Only keep the last appearance of each legend name and its
        # corresponding handle:
        # (there will be duplicates if interp plots are added behind):
        prune = [x in legends[i + 1:] for i, x in enumerate(legends)]
        handles = [x for i, x in enumerate(handles) if not prune[i]]
        legends = [x for i, x in enumerate(legends) if not prune[i]]

        if ignore_labels is not None and plot_idx in ignore_labels:
            reformatted = [*zip(*[
                (h, l) for h, l in zip(handles, legends)
                if l not in ignore_labels[plot_idx]
            ])]
            if len(reformatted) > 0:
                handles, legends = reformatted
            else:
                handles, legends = [], []

        if position_overrides is not None and plot_idx in position_overrides:
            loc = position_overrides[plot_idx]
        else:
            loc = 'best'

        # Apply to *last* pair of axes on the plot (so it's on top):
        axs[ax_idxs[-1]].legend(
            handles, legends,
            loc=loc,
            markerscale=3.5,
            shadow=False,
            framealpha=0.85
        )


def hide_lines(
    settings: PlotSettings,
    axis: Axis,
    line_labels: List[str]
) -> None:
    """Hides lines with the given labels, after plotting.
    (useful for having two plots have matching colors but different visibilities.)
    """
    for line in axis.lines:
        if line.get_label() in line_labels:
            line.set_linestyle('none')
            line.set_marker('none')


def set_ylim_from_df(axis: Axis, df: pd.DataFrame) -> None:
    if df.size == 0:
        return  # Nothing we can do
    min_val = df.min().min()
    max_val = df.max().max()
    val_range = max_val - min_val
    axis.set_ylim(
        [min_val - 0.05*val_range, max_val + 0.05*val_range],
        auto=False
    )


def _reset_colors(settings: PlotSettings) -> None:
    # reset color order between subplots:
    settings.colors = deque(settings.colors_set)


def prep_new_subplot(settings: PlotSettings, plot_idx: int) -> None:
    _reset_colors(settings)


def _axis_has_data(axis: Axis) -> bool:
    """Check if any data has actually been plotted on these axes
    (as a line or a scatter).
    NOTE: Checking for lines isn't sufficient since DataFrame.plot will still
    place empty lines there if the DataFrame is empty.
    """
    return (
        sum(len(line.get_xdata()) for line in axis.lines) > 0 or
        len(axis.collections) > 0
    )


def _align_x_axes(
    settings: PlotSettings,
    axs: List[Axis],
    start_time_override: datetime | None = None,
    end_time_override: datetime | None = None
) -> None:
    # Make sure axes have the same xlims so they're comparable (choose the
    # widest range):
    xlims = [ax.get_xlim() for ax in axs if _axis_has_data(ax)]
    if len(xlims) == 0:
        return  # can't do anything
    xlim_wide = [min(a for a, _ in xlims), max(b for _, b in xlims)]
    # Make sure the xlims still conform to the time bounds:
    if start_time_override is None:
        start_time = settings.start_time
    else:
        start_time = start_time_override
    if end_time_override is None:
        end_time = settings.end_time
    else:
        end_time = end_time_override
    if start_time is not None:
        start_bound = pd.to_datetime(start_time, utc=True)
        start_bound = mdates.date2num(start_bound)
        xlim_wide[0] = max(xlim_wide[0], start_bound)
    if end_time is not None:
        end_bound = pd.to_datetime(end_time, utc=True)
        end_bound = mdates.date2num(end_bound)
        xlim_wide[1] = min(xlim_wide[1], end_bound)
    # Apply limits:
    for ax in axs:
        ax.set_xlim(xlim_wide)


def _align_plot_y_axes(
    settings: PlotSettings,
    axs: List[Axis],
    plot_idx: int,
    str_format: str | None = None,
    force_integer: bool = False,
    secondary_grid_colors: List[str] = [
        'olive', 'gainsboro', 'antiquewhite', 'lemonchiffon', 'grey',
    ]
) -> None:
    # Don't do anything if just 1 pair of axes:
    if len(settings.plot_axes[plot_idx]) < 2:
        return
    if str_format is None:
        if force_integer:
            str_format = '%.0f'
        else:
            str_format = '%.3f'
    n_ticks = max(
        len(axs[i].get_yticks()) for i in settings.plot_axes[plot_idx]
    )
    for idx_in_plot, i in enumerate(settings.plot_axes[plot_idx]):
        axs[i].yaxis.set_major_locator(mticker.LinearLocator(n_ticks))
        if force_integer:
            axs[i].yaxis.set_major_locator(
                mticker.MaxNLocator(n_ticks, integer=force_integer)
            )
        else:
            axs[i].yaxis.set_major_locator(mticker.LinearLocator(n_ticks))
        axs[i].yaxis.set_major_formatter(
            mticker.FormatStrFormatter(str_format)
        )
        # Make sure ticks don't extend beyond the range of the data:
        axs[i].autoscale(enable=True, axis='y', tight=True)
        # Save the new tick values:
        ticks = axs[i].get_yticks()
        tick_labels = axs[i].get_yticklabels()
        # Add some margin to the y limits *after* autoscaling:
        tight_ylim = axs[i].get_ylim()
        ylim_range = tight_ylim[1] - tight_ylim[0]
        new_ylim = [tight_ylim[0] - 0.05 * ylim_range,
                    tight_ylim[1] + 0.05 * ylim_range]
        axs[i].set_ylim(new_ylim, auto=False)
        # Reapply tick labels:
        axs[i].set_yticks(ticks)
        axs[i].set_yticklabels(tick_labels)
        # If we had to force integers, it isn't guaranteed that the second
        # y-axis will align, so we should make those secondary lines a
        # different color:
        if force_integer and idx_in_plot > 0:
            axs[i].grid(
                color=secondary_grid_colors[i % len(secondary_grid_colors)]
            )


def _apply_time_formattings(
    settings: PlotSettings,
    axs: List[Axis]
) -> None:
    # Apply time formatting to x axes:
    for ax in axs:
        ax.xaxis.set_major_locator(time_locator)
        ax.xaxis.set_major_formatter(time_formatter)


def tidy_figure(
    settings: PlotSettings,
    axs: List[Axis],
    str_format: str | None = None,
    skip_x_align: bool = False,
    skip_time: bool = False,
    skip_y_align: bool = False,
    only_plots: List[int] | None = None
) -> None:
    if only_plots is not None:
        axs_iso = [axs[i] for pi in only_plots for i in settings.plot_axes[pi]]
    if not skip_x_align:
        axs_x = axs if only_plots is None else axs_iso
        _align_x_axes(settings, axs_x)
    if not skip_time:
        axs_t = axs if only_plots is None else axs_iso
        _apply_time_formattings(settings, axs_t)
    if not skip_y_align:
        if only_plots is None:
            plots = [*settings.plot_axes.keys()]
        else:
            plots = only_plots
        for plot_idx in plots:
            _align_plot_y_axes(settings, axs, plot_idx, str_format)


def save_figure(
    settings: PlotSettings,
    axs: List[Axis],
    name_override: str | None = None,
    dont_close: bool = False,
    ext_in_path: bool = False,
    dpi_override: int | None = None
) -> None:
    if name_override is None:
        name = settings.title
    else:
        name = name_override
    plots_ulid = ulid.new()
    if ext_in_path:
        out_dir = os.path.join(settings.save_dir, settings.plot_ext)
    else:
        out_dir = settings.save_dir
    out_file = os.path.join(
        out_dir, f"{name}_{plots_ulid!s}.{settings.plot_ext}"
    )
    # Make sure directory exists:
    if not os.path.exists(out_dir):
        os.makedirs(out_dir)
    plt.savefig(out_file, dpi=(dpi_override or settings.dpi))
    if not dont_close:
        plt.close()
    print(f"Results saved to: {out_file}")
