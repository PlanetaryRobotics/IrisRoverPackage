"""
Converts exported YAMCS mission data from a pickle of YAMCS objects to the
HDFStore database format used for storing YAMCS data from previous exercises
(like iTVAC). This archive file can be used with `archive_transceiver` and
`archive_xcvr` to replay mission data into the Ground Software.

Author: Colombo, CMU
Last Updated: 10/02/2024
"""

import argparse
import IrisBackendv3 as IB3
import IrisBackendv3.ipc as ipc

import os  # noqa
os.environ["PROTOCOL_BUFFERS_PYTHON_IMPLEMENTATION"] = "python"  # noqa

from IrisBackendv3.transceiver.archive_transceiver import ArchiveParseOpts
from IrisBackendv3.storage.telemetry import yamcs_dataset_to_telem
from IrisBackendv3.storage.commands import yamcs_history_to_commands
from IrisBackendv3.storage.logs import logger_setConsoleLevel as setStorageLogLevel
from IrisBackendv3.codec.logs import logger_setConsoleLevel as setCodecLogLevel

from IrisBackendv3.storage.dataset import DataSet

from IrisBackendv3.logs import VALID_LOG_LEVELS

from IrisBackendv3.transceiver.yamcs_helper import (
    ALL_YAMCS_PARAMS
)
from yamcs.tmtc.model import ParameterValue, CommandHistory  # type: ignore

from typing import Any, Final, Tuple, List, Dict, Optional, TypeAlias
from datetime import datetime
import os.path

import argparse

from rich.progress import track

import numpy as np
import pandas as pd  # type: ignore
import time

import pickle

IB3.init_from_latest()


TimeDict: TypeAlias = Dict[datetime, List[ParameterValue]]


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

    parser.add_argument('-d', '--data-dir', default='./out/fm1_data/yamcs_export/',
                        help=r'Directory where all data files are found and will be saved.')
    parser.add_argument('-p', '--params-file', default='parameter_histories.pickle',
                        help=r'File name (in data-dir) of pickle of Parameter Histories export from YAMCS.')
    parser.add_argument('-c', '--commands-file', default='command_history.pickle',
                        help=r'File name (in data-dir) of pickle of Command History export from YAMCS.')
    parser.add_argument('-n', '--name', default='fm1_mission_archive',
                        help=r'Name of export files.')
    parser.add_argument('--rx-is-generation-time', default=True,
                        action=argparse.BooleanOptionalAction,
                        help="Whether the reception_time (amcc_rx, pmcc_rx) should "
                        "be generation_time  (lander_rx), if this data is "
                        "being replayed, or 'now' if the data came in recently "
                        "and we want it to sync up with the current time "
                        "(rarely useful). In most cases you'll want this to be "
                        "`True`."
                        )

    parser.add_argument('--log-level', type=str_to_log_level, default=default_log_level,
                        help=('Logging level to be used (i.e. how annoying the logging '
                              'printouts should be). Only logs with importance greater '
                              'than or equal to the specified logging level are '
                              f'displayed. Valid logging levels are: {VALID_LOG_LEVELS}'))

    opts = parser.parse_args()
    return opts


def clump_param_history_by_time(
    app: ipc.IpcAppHelper,
    param_reload: Dict[str, List[ParameterValue]]
) -> TimeDict:
    # Group all entries by generation time & sort (so they replay correctly):
    app.logger.notice(
        f"Recovered {len([*param_reload.keys()])} YAMCS parameters with {(n_pts:=sum(len(vs) for vs in param_reload.values()))} total values."
    )

    # Clump by common generation_time:
    time_dict: Dict[datetime, List[ParameterValue]] = dict()
    for name, entries in param_reload.items():
        for entry in entries:
            if entry.generation_time not in time_dict:
                time_dict[entry.generation_time] = []
            time_dict[entry.generation_time].append(entry)

    # Sort by generation time:
    time_dict = dict(sorted(time_dict.items()))

    def log_time_dict_stats(time_dict: Dict[datetime, List[ParameterValue]]):
        app.logger.info(
            f"Collected {sum(len(entries) for entries in time_dict.values())}"
            f"... clumped into {len(time_dict)} distinct time points from "
            f"{[*time_dict.keys()][0]:%Y-%m-%d %H:%M:%S%z} to "
            f"{[*time_dict.keys()][-1]:%Y-%m-%d %H:%M:%S%z}."
        )

        # Check for wrong "2000" bug with YAMCS timestamps on PM1
        # (no timestamp given and just assigned to some time close to 2000):
        app.logger.warning(
            f"Of the {(nt:=len(time_dict))} time points recovered, "
            f"{(ntb:=len([k for k in time_dict.keys() if k.year < 2020]))} ({100*ntb/nt:.3f}%) "
            f"are from before 2020 and are likely victim to the '2000' bug. "
            f"These {(n_bad:=sum(len(vs) for k, vs in time_dict.items() if k.year < 2024))} "
            f"datapoints of the {n_pts} total datapoints ({100*n_bad/n_pts:.3f}%) will be ignored. "
            f"The parameters affected are: {set(v.name for k,vs in time_dict.items() if k.year < 2020 for v in vs)}"
        )
    log_time_dict_stats(time_dict)

    # For some parameters duplicate identical entries (same time, same value) were
    # fetched from YAMCS. Find and remove these:
    n_entries_raw = sum(len(entries) for entries in time_dict.values())
    for time in track(time_dict.keys(), "Deduplicating entries"):
        # Can't directly do ParameterValue comparison, but can compare their
        # `__dict__` s which contain all the core data we care about:
        unique_dicts: List[Dict] = []
        unique_entries: List[ParameterValue] = []
        for entry in time_dict[time]:
            if entry.__dict__ not in unique_dicts:
                unique_dicts.append(entry.__dict__)
                unique_entries.append(entry)
        time_dict[time] = unique_entries
    n_entries_dedup = sum(len(entries) for entries in time_dict.values())
    n_removed = n_entries_raw - n_entries_dedup
    app.logger.notice(
        f"Duplicate Removal Complete. "
        f"Removed {n_removed} ({n_removed/n_entries_raw*100:.1f}%, <=70-80% expected). "
        f"Num. entries {n_entries_raw} -> {n_entries_dedup}. After removal:"
    )
    log_time_dict_stats(time_dict)
    return time_dict


def yamcs_time_dict_to_dataset(yamcs_time_dict: TimeDict) -> DataSet:
    """Generates a YAMCS DataSet from a TimeDict of YAMCS data:"""
    # AMCC doesn't provide generation time, so this really is all the meaningful
    # data we get through YAMCS:
    yamcs_data_dict: Dict[str, List[Tuple[datetime, Any]]] = dict()
    for params in yamcs_time_dict.values():
        for p in params:
            # Ignore params not in list (MO/timeline) since we can't handle those anyway:
            if p.name not in ALL_YAMCS_PARAMS:
                continue
            if p.name not in yamcs_data_dict:
                yamcs_data_dict[p.name] = []
            yamcs_data_dict[p.name].append((p.generation_time, p.eng_value))

    # Return as DataSet:
    dataset = DataSet.from_tuples_dict(
        yamcs_data_dict,
        time_tolerance=pd.Timedelta('1ms')
    )
    return dataset


if __name__ == "__main__":
    opts = get_opts()
    app = ipc.IpcAppHelper("YamcsMissionExportConverter")
    app.setLogLevel(opts.log_level)
    setStorageLogLevel(opts.log_level)
    # Set Codec Logger to Error+ b/c there will be a glut of warnings for the
    # infrequent corrupted packets:
    setCodecLogLevel('ERROR')

    DATA_DIR: Final[str] = opts.data_dir
    PARAM_HISTORY_FILE: Final[str] = opts.params_file
    CMD_HISTORY_FILE: Final[str] = opts.commands_file
    EXPORT_NAME: Final[str] = opts.name

    param_reload: Dict[str, List[ParameterValue]]
    cmd_reload: List[CommandHistory]

    archive_opts = ArchiveParseOpts(
        name=EXPORT_NAME,
        ext='parquet',
        rx_is_generation_time=opts.rx_is_generation_time
    )

    app.logger.notice("Loading Mission Data Exports . . .")
    PARAM_HISTORY_PATH = os.path.join(DATA_DIR, PARAM_HISTORY_FILE)
    CMD_HISTORY_PATH = os.path.join(DATA_DIR, CMD_HISTORY_FILE)
    app.logger.info(
        f"\t> Loading Parameter History from {PARAM_HISTORY_PATH} . . .")
    with open(PARAM_HISTORY_PATH, 'rb') as f:
        param_reload = pickle.load(f)
    app.logger.info(
        f"\t> Loading Command History from {CMD_HISTORY_PATH} . . .")
    with open(CMD_HISTORY_PATH, 'rb') as f:
        cmd_reload = pickle.load(f)

    app.logger.success("Mission exports loaded!")

    app.logger.notice(f"Recovered {len(cmd_reload)} Command records.")
    app.logger.notice(
        f"Recovered {len([*param_reload.keys()])} YAMCS parameters "
        f"with {(n_pts:=sum(len(vs) for vs in param_reload.values()))} total "
        "values."
    )

    app.logger.notice("Processing command history . . .")
    commands = yamcs_history_to_commands(cmd_reload)

    app.logger.notice("Saving Command History Table . . .")
    pqt_path = os.path.join(DATA_DIR, EXPORT_NAME+".command.parquet")
    app.logger.info(f"\t ... to parquet: {pqt_path} . . .")
    commands.save_to_parquet(pqt_path)

    excel_path = os.path.join(DATA_DIR, EXPORT_NAME+".command.xlsx")
    app.logger.info(f"\t ... to Excel: {excel_path} . . .")
    commands.save_to_excel(excel_path)
    app.logger.success("Commands History Table Saved!")

    app.logger.info("Processing parameter history . . .")
    yamcs_time_dict = clump_param_history_by_time(app, param_reload)

    # Convert to the Archive:
    app.logger.notice("Generating DataSet from YAMCS entries. . .")
    yamcs_dataset = yamcs_time_dict_to_dataset(yamcs_time_dict)
    # Filter out '2000'/'2001' bug entries that lost their timestamp in YAMCS:
    yamcs_dataset.data = yamcs_dataset.data[yamcs_dataset.data.index.year > 2020]
    # Join on command data for cross-ref:
    cmds_prefixed = commands.data.rename(columns={
        c: '*Command_'+c
        for c in commands.data.columns
    })
    yamcs_dataset.data = yamcs_dataset.data.join(cmds_prefixed, how='outer')
    yamcs_dataset = DataSet(yamcs_dataset.data)  # reapply df formatting
    app.logger.success(
        f"YAMCS DataSet generated with shape {yamcs_dataset.data.shape}"
    )

    # Save YAMCS DataSet in Archive:
    app.logger.notice("Saving YAMCS DataSet . . .")
    pqt_path = os.path.join(DATA_DIR, EXPORT_NAME+".yamcs.parquet")
    app.logger.info(f"\t ... to parquet: {pqt_path} . . .")
    yamcs_dataset.save_to_parquet(pqt_path)

    excel_path = os.path.join(DATA_DIR, EXPORT_NAME+".yamcs.xlsx")
    app.logger.info(f"\t ... to Excel: {excel_path} . . .")
    yamcs_dataset.save_to_excel(excel_path)
    app.logger.success("YAMCS DataSet Saved!")

    # Generate Full Telemetry DataSet:
    app.logger.notice("Generating Full Telemetry DataSet: . . .")
    telem_dataset = yamcs_dataset_to_telem(
        yamcs_dataset,
        rx_is_generation_time=archive_opts.rx_is_generation_time
    )
    # Join on command data for cross-ref:
    telem_dataset.data = telem_dataset.data.join(cmds_prefixed, how='outer')
    telem_dataset = DataSet(telem_dataset.data)  # reapply df formatting
    app.logger.notice(
        f"Telemetry DataSet generated with shape {telem_dataset.data.shape}"
    )

    # Save Telemetry DataSet in Archive:
    app.logger.notice("Saving Telemetry DataSet . . .")
    pqt_path = os.path.join(DATA_DIR, EXPORT_NAME+".telem.parquet")
    app.logger.info(f"\t ... to parquet: {pqt_path} . . .")
    telem_dataset.save_to_parquet(pqt_path)

    excel_path = os.path.join(DATA_DIR, EXPORT_NAME+".telem.xlsx")
    app.logger.info(f"\t ... to Excel: {excel_path} . . .")
    telem_dataset.save_to_excel(excel_path, generate_telem_tables=True)
    app.logger.success("Telemetry DataSet Saved!")
