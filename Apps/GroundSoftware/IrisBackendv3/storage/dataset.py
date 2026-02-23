"""
Collection for representing and storing archived data from an entire activity
/ mission.

Presently, this is handled as a light wrapper for a DataFrame so all the data
processing tools that come with pandas can be used here too.

TODO: Migrate all Iris-specific table generation (specifically, for Excel) out
to a `config` or `ipc_app` file.

@author: Colombo, CMU
@last-updated: 10/06/2024
"""
# Activate postponed annotations:
from __future__ import annotations

import IrisBackendv3 as IB3
from IrisBackendv3.data_standards.fsw_data_type import (
    Category as FswDataCategory
)
from IrisBackendv3.data_standards.logs import (
    logger as DsLogger,
    logger_setConsoleLevel as DsLoggerLevel
)

from IrisBackendv3.storage.formatting import (
    bytes_to_str, format_time, reformat_event_str
)
from IrisBackendv3.storage.settings import settings
from IrisBackendv3.storage.logs import logger
from IrisBackendv3.storage.utils import sorted_natural_ints

from typing import Any, Final, Tuple, List, Dict, Iterable, Type, cast
from dataclasses import dataclass
from datetime import datetime, timedelta, timezone
import os.path

from dataclasses import dataclass

import ulid
import numpy as np
import pandas as pd  # type: ignore
import scapy.all as scp  # type: ignore

from rich.progress import Progress, TaskID


@dataclass
class DataSeries:
    """Series of timestamped data-points
    (DataFrame with a TimeSeries Datetimeindex of times and a column of values)
    """
    data: pd.DataFrame

    def __post_init__(self) -> None:
        """Make sure DataFrame formatting is correct."""
        # Make sure index is datetime:
        self.data.index = format_time(self.data.index)
        # Make sure index is sorted by ascending time:
        self.data.sort_index(ascending=True, inplace=True)

    @classmethod
    def from_lists(
        cls,
        name: str,
        times: List[datetime],
        values: List[Any]
    ) -> DataSeries:
        # Create basic data series:
        val_types = set(type(v) for v in values)
        if bytes in val_types:
            # Serialize any bytes to strings for better storage (pandas compat):
            values = [
                bytes_to_str(v) if isinstance(v, bytes) else v
                for v in values
            ]
            val_types.remove(bytes)
            val_types.add(str)

        dtype: Type
        if len(val_types) != 1:
            dtype = object
        else:
            val_type = val_types.pop()
            dtype = {
                str: str,
                int: int,
                float: float
            }[val_type]

        df = pd.DataFrame(
            data={name: values},
            index=format_time(times),
            dtype=dtype
        )
        df.index.name = settings['TIME_COL_NAME']

        return DataSeries(df)

    @classmethod
    def from_tuples_list(
        cls,
        name: str,
        data: List[Tuple[datetime, Any]]
    ) -> DataSeries:
        # Unzip tuples into two lists:
        if len(data) > 0:
            times, values = [*zip(*data)]  # type: ignore
            return cls.from_lists(name, times, values)
        else:
            return cls.from_lists(name, [], [])


def _is_bool_obj_column(series: pd.Series, sample_limit: int = 1000) -> bool:
    """
    Helper function to determine if a column's data is boolean-only.
    """
    if pd.api.types.is_bool_dtype(series.dtype):
        return True

    # Drop missing values (np.nan and None)
    series = series.dropna()
    series = series[series != None]

    if series.empty:
        return False  # Can't determine

    # Sample the data to reduce computation:
    sample = series.head(sample_limit)

    return sample.map(lambda x: isinstance(x, bool)).all()  # type: ignore


def _is_datetime_only_column(series: pd.Series, sample_limit: int = 1000) -> bool:
    """
    Helper function to determine if a column's data is a datetime-only, even if
    stored in a non-datetime column.
    """
    if pd.api.types.is_datetime64_any_dtype(series.dtype):
        return True

    # Drop missing values (np.nan and None)
    series = series.dropna()
    series = series[series != None]

    if series.empty:
        return False  # Can't determine

    # Sample the data to reduce computation:
    sample = series.head(sample_limit)

    return sample.map(lambda x: isinstance(x, datetime)).all()  # type: ignore


def _is_int_only_column(series: pd.Series, sample_limit: int = 1000) -> bool:
    """
    Helper function to determine if a column's data is an int-only, even if
    stored in a non-int column.
    """
    if pd.api.types.is_integer_dtype(series.dtype):
        return True

    # Drop missing values (np.nan and None)
    series = series.dropna()
    series = series[series != None]

    if series.empty:
        return False  # Can't determine

    # Sample the data to reduce computation:
    sample = series.head(sample_limit)

    def _is_int_invariant(x: Any) -> bool:
        try:
            return cast(bool, int(x) == x)
        except:
            # Couldn't convert to int, so not eligible:
            return False

    return sample.map(  # type: ignore
        lambda x: isinstance(x, int) or _is_int_invariant(x)
    ).all()


def _convert_bool_obj_cols_to_float(df: pd.DataFrame) -> pd.DataFrame:
    """
    Helper function to determine if a column is boolean-only object column.
    NOTE: This performs the operation in-place.
    """
    bool_cols = [
        c for c in df.select_dtypes(include=['object']).columns
        if _is_bool_obj_column(df[c])
    ]
    df[bool_cols] = df[bool_cols].astype(float)
    return df


def _delocalize_datetime_columns(df: pd.DataFrame) -> pd.DataFrame:
    """
    Remove localization from all datetime columns (for Excel compat):
    NOTE: This performs the operation in-place.
    """
    dt_cols = [c for c in df.columns if _is_datetime_only_column(df[c])]
    for c in dt_cols:
        df[c] = df[c].apply(
            lambda t: t.tz_convert(None) if isinstance(t, pd.Timestamp) else t
        )
        # Should be datetime dtype now:
        if df[c].dt.tz is not None:
            df[c] = df[c].dt.tz_convert(None)
    return df


def _isolate_event_columns(cols: Iterable[str]) -> List[str]:
    """Returns a filtered list of column names containing only those columns
    which correspond to Events."""
    standards = IB3.get_codec_standards()
    e_cols: List[str] = []
    for c in cols:
        parts = c.split('_')
        m_name = parts[0]
        item_name = '_'.join(parts[1:])
        if m_name not in standards.modules.names:
            continue
        module = standards.modules[m_name]
        if item_name in module.events.names:
            e_cols.append(c)
    return e_cols


def _is_integer_telem_column(c: str) -> bool:
    standards = IB3.get_codec_standards()
    parts = c.split('_')
    m_name = parts[0]
    item_name = '_'.join(parts[1:])
    if m_name not in standards.modules.names:
        return False
    module = standards.modules[m_name]
    if item_name in module.telemetry.names:
        channel = module.telemetry[item_name]
        if channel.datatype.python_type == int:
            return True
    return False


def _is_non_numeric_telem_column(c: str) -> bool:
    standards = IB3.get_codec_standards()
    parts = c.split('_')
    m_name = parts[0]
    item_name = '_'.join(parts[1:])
    if m_name not in standards.modules.names:
        return False
    module = standards.modules[m_name]
    if item_name in module.telemetry.names:
        channel = module.telemetry[item_name]
        if channel.datatype.category != FswDataCategory.NUMBER:
            return True
    return False


def _isolate_non_numeric_telem_columns(cols: Iterable[str]) -> List[str]:
    """Returns a filtered list of column names containing only those columns
    which correspond to Telemetry of non-numeric data:"""
    return [c for c in cols if _is_non_numeric_telem_column(c)]


def _type_aware_telem_interpolate(
    df: pd.DataFrame,
    limit: int = 200  # empirically determined, >120 good to fill gaps
) -> pd.DataFrame:
    # Just ffill any columns containing boolean or integer data
    # (even if represented as a number) or strings:
    cols = [*df.columns]
    # Use set to make sure there are no duplicate names:
    fwd_cols = list(set([
        *_isolate_event_columns(cols),
        *_isolate_non_numeric_telem_columns(cols),
        *[c for c in cols if (
            not pd.api.types.is_numeric_dtype(df[c].dtype)
            or _is_int_only_column(df[c])
        )],
    ]))
    df[fwd_cols] = df[fwd_cols].ffill(limit=limit)

    # Interpolate the rest:
    numeric_cols = list(set(cols) - set(fwd_cols))
    df[numeric_cols] = df[numeric_cols].interpolate(method='time', limit=limit)
    return df


def _df_to_excel_sheet(
    writer: pd.ExcelWriter,
    df: pd.DataFrame,
    sheet_name: str,
    rotate_headers: bool = True
) -> None:
    """Uses `writer` to write the given DataFrame to an excel sheet with the
    given name, with standardized styling."""
    # Write everything but the header:
    df.to_excel(writer, sheet_name=sheet_name, startrow=1, header=False)

    sheet = writer.sheets[sheet_name]

    # Add a header formats:
    header_style = {
        'align': 'center',
        'valign': 'bottom',
        'text_wrap': False,
        'bold': True,
        'border': 1
    }
    fmt_header = writer.book.add_format(header_style)
    if rotate_headers:
        fmt_header_rotated = writer.book.add_format({
            'rotation': 90, **header_style  # type: ignore
        })
    else:
        fmt_header_rotated = fmt_header

    num_format_int = writer.book.add_format({'num_format': '0'})
    num_format_f1 = writer.book.add_format({'num_format': '0.0'})
    num_format_f3 = writer.book.add_format({'num_format': '0.000'})

    # Write the column headers with the defined format.
    for col_num, col_name in enumerate(df.columns):
        idx = col_num + 1
        sheet.write(0, idx, col_name, fmt_header_rotated)
        # Optionally, apply column format depending on contents:
        if any(x in col_name.upper() for x in [
            'CURRENT', 'MILLIAMPS', 'PERCENT', 'TEMP', 'CELSIUS', 'KELVIN'
        ]):
            sheet.set_column(idx, idx, None, num_format_f1)
        elif _is_integer_telem_column(col_name) or _is_int_only_column(df[col_name]):
            sheet.set_column(idx, idx, None, num_format_int)
        elif not _is_non_numeric_telem_column(col_name):
            # All other numerics get at most 3 decimal places:
            sheet.set_column(idx, idx, None, num_format_f3)

    # Write the index header with the defined format:
    sheet.write(0, 0, df.index.name, fmt_header)


@dataclass
class DataSet:
    """Collection of data containing multiple named DataSeries comprised of a
    multiple timestamped data points
    (basically just a way for merging a bunch of Timeseries DataFrames into
    one).
    """
    data: pd.DataFrame

    def __post_init__(self) -> None:
        """Make sure DataFrame formatting is correct."""
        # Make sure index is datetime and named properly:
        self.data.index = format_time(self.data.index)
        self.data.index.name = settings['TIME_COL_NAME']
        # Make sure index is sorted by ascending time:
        self.data.sort_index(ascending=True, inplace=True)
        # Sort columns alphanumerically:
        self.data = self.data.reindex(
            sorted_natural_ints(self.data.columns), axis=1
        )
        # Fix strings:
        self._nullify_null_strings()

    def _nullify_null_strings(self) -> None:
        """ For any string columns, replace 'nan' and '' with np.nan (so empty
        # detection works)."""
        def _not_null_string(x: Any) -> bool:
            if not isinstance(x, str):
                return True
            return x != '' and x.upper() != "NAN"
        for c in self.data.columns:
            if pd.api.types.is_string_dtype(self.data[c].dtype):
                self.data[c] = self.data[c].where(
                    self.data[c].apply(_not_null_string)
                )

    def stack(self, other: DataSet) -> DataSet:
        """Stacks this `DataSet` vertically on top of an `other` `DataSet`
        (stacking, here, is in the sense of `numpy.vstack`, rows from `other`
        will be added below the rows of this DataSet. Any columns that `other`
        has but this doesn't have will be added)."""
        df = pd.concat(
            [self.data, other.data],
            axis=0,
            ignore_index=False
        )
        return DataSet(df)

    def hstack(self, other: DataSet) -> DataSet:
        """Stacks this `DataSet` horizontally to the left of `DataSet` `other`
        (stacking, here, is in the sense of `numpy.hstack`, columns from
        `other` will be added to the right of the columns of this DataSet.
        Any rows that `other` has but this doesn't have will be added)."""
        df = pd.concat(
            [self.data, other.data],
            axis=1,
            ignore_index=False
        )
        return DataSet(df)

    def _save_telem_tables_to_excel(
        self,
        df: pd.DataFrame,
        writer: pd.ExcelWriter,
        progress: Progress,
        task: TaskID
    ) -> None:
        # Create blank sheets in desired order to force order:
        blank = pd.DataFrame()
        # Special Sheets first:
        for sn in [
            '_IrisOnly', '_Thermal', '_ThermalInterp', '_Power', '_PowerInterp',
            '_Packet', '_Packet-IrisOnly', '_Events', '_Commands', '_Credits'
        ]:
            blank.to_excel(writer, sn)

        # Pre-process packet strings to make them human readable:
        df['*Packet_str'] = df['*Packet_str'].apply(
            lambda x: reformat_event_str(x) if isinstance(x, str) else x
        )

        # Save a sheet of just Iris telem:
        # Exclude Peregrine Modules:
        progress.update(task, description="Isolated Iris Telem")
        cols = [*df.columns]
        iris_data = df.filter([
            c for c in cols if 'PEREGRINE' not in c.upper()
        ])
        # Exclude rows of Peregrine packets:
        packet_strs = iris_data['*Packet_str'].str.upper()
        iris_only = packet_strs.apply(
            lambda x: isinstance(x, str) and 'PEREGRINE' not in x
        )
        iris_data = iris_data[iris_only]
        iris_data = iris_data.dropna(axis=0, how='all')
        _df_to_excel_sheet(writer, iris_data, "_IrisOnly")
        progress.advance(task)

        # Create Module-by-Module sheets that drop na rows:
        progress.update(task, description="Module Sheets")
        modules = [*set(c.split('_')[0] for c in cols if '_' in c)]
        modules = [m for m in modules if not m.startswith('*Command')]
        modules = sorted_natural_ints(modules)  # sort for natural sheet order
        mod_task = progress.add_task("Modules", True, 0, len(modules)+1)
        for m in modules:
            progress.update(mod_task, description=m)
            subset = df.filter([c for c in cols if c.startswith(m+'_')])
            subset = subset.dropna(axis=0, how='all')
            # Ensure appropriate sheet name formatting:
            sheet_name = m
            if len(sheet_name) > 31:
                # Split off any prefixes like "ActiveRateGroup-" then
                # truncate to get under length:
                sheet_name = sheet_name.split('-')[-1][-31:]
            for c in r'[]:*?/\\\'':
                sheet_name = sheet_name.replace(c, '')
            if m != '*Packet':
                _df_to_excel_sheet(writer, subset, sheet_name)
                progress.advance(mod_task)
            else:
                # For Packets, also create one for just Iris Packets
                _df_to_excel_sheet(
                    writer, subset, '_Packet',
                    rotate_headers=False
                )
                progress.advance(mod_task)
                packet_strs = subset['*Packet_str'].str.upper()
                iris_only = packet_strs.apply(lambda x: (
                    isinstance(x, str) and 'PEREGRINE' not in x
                ))
                iris_packets = subset[iris_only]
                _df_to_excel_sheet(
                    writer, iris_packets, '_Packet-IrisOnly',
                    rotate_headers=False
                )
                progress.advance(mod_task)
        progress.update(mod_task,
                        description="Module sheets complete.",
                        total=1, completed=1
                        )
        progress.remove_task(mod_task)
        progress.advance(task)

        # Create Stream of Event columns (incl. packet strings):
        progress.update(task, description="Events")
        event_cols = _isolate_event_columns(df.columns)
        # Add in command streams (first):
        event_cols = ['*Command_command', *event_cols]
        # Add in packet strings (last):
        event_cols.extend(['*Packet_str', '*Packet_type'])

        # Remove spam events from stream:
        event_cols = [c for c in event_cols if c not in [
            'WatchDogInterface_AdcCurrentSensorReadingsReport',
            'GroundInterface_GiDownlinkedPacket',
            'RadioDownlinkFlushPacket',
            'GdsPackets_UnsupportedPacket'
        ]]

        events = df[event_cols]
        events = events.dropna(axis=0, how='all')

        # Reformat commands:
        events['*Command_command'] = events['*Command_command'].apply(
            lambda x: ' '*12 + '[UPLINK] > ' + x if isinstance(x, str) else x
        )

        # Remove spam entries:
        events['GdsPackets_WatchdogDebugPacket_filtered'] = events['GdsPackets_WatchdogDebugPacket']
        events = events.drop(columns=['GdsPackets_WatchdogDebugPacket'])
        spam_debugs = events['GdsPackets_WatchdogDebugPacket_filtered'].apply(
            lambda x: isinstance(x, str) and any(xx in x for xx in [
                'RTT: ', ', skipped sends: '
            ])
        )
        events.loc[spam_debugs,
                   'GdsPackets_WatchdogDebugPacket_filtered'] = np.nan

        # Only keep packet strings of key packets:
        events['*Packet_str_filtered'] = events['*Packet_str']
        key_packets = events['*Packet_type'].isin([
            'WatchdogHeartbeatPacket',
            'WatchdogTvacHeartbeatPacket',
            'WatchdogDetailedStatusPacket'
        ])
        events.loc[~key_packets, '*Packet_str_filtered'] = np.nan
        events = events.drop(columns=['*Packet_str', '*Packet_type'])

        # Re-drop nan-rows now that we've probably made more:
        events = events.dropna(axis=0, how='all')

        # Create a singular column of all events and key messages in 1
        # chronological stream:
        events.insert(
            0, '_MessageStream',
            events.apply(lambda r: r.str.cat(sep=',    '), axis=1)
        )
        _df_to_excel_sheet(writer, events, '_Events', rotate_headers=False)
        progress.advance(task)

        # Collect all Command and Command Response Fields:
        progress.update(task, description="Commands")

        command_cols = [
            '*Command_Time1_queued',
            '*Command_Time2_approved',
            '*Command_Time3_radiated',
            '*Command_Time4_onLander',
            '*Command_alias',
            '*Command_command',
            '*Command_command_type',
            '*Command_data_size',
            '*Command_pathway',
            '*Command_source',
            'GroundInterface_GiCommandReceived',
            'GroundInterface_GiUplinkedPacketError',
            'GdsPackets_WatchdogCommandResponsePacket',
            'GdsPackets_WatchdogResetSpecificAckPacket',
            'GdsPackets_WatchdogSafetyTimerPacket',
            'CommandDispatcher_OpCodeDispatched',
            'CommandDispatcher_OpCodeCompleted',
            'NetworkManager_PktRecv',
            'GroundInterface_GiCmdsUplinked',
            'GroundInterface_GiCmdsSent',
            'GroundInterface_GiUplinkPktErrs',
            'GroundInterface_GiUplinkSeqNum',
            '*Command_yamcs_Name',
            '*Command_yamcs_Queue',
            '*Command_yamcs_User',
            '*Command_yamcs_comment',
            'CommandDispatcher_NoOpStringReceived',
            'WatchdogCommandResponse_CommandId',
            'WatchdogCommandResponse_ErrorFlag',
            '*Command_data'
        ]
        command_cols = [x for x in command_cols if x in df.columns]
        # Supplemental data, handled differently:
        suppl_cols = [
            'NetworkManager_PktRecv',
            'GroundInterface_GiCmdsUplinked',
            'GroundInterface_GiCmdsSent',
            'GroundInterface_GiUplinkPktErrs',
            'GroundInterface_GiUplinkSeqNum'
        ]
        suppl_cols = [x for x in suppl_cols if x in df.columns]
        command = df[[x for x in command_cols if x not in suppl_cols]]
        command = command.dropna(axis=0, how='all')

        # Select supplemental telemetry (not event) fields that can indicate an
        # ack:
        suppl = df[suppl_cols]
        suppl = suppl.dropna(axis=0, how='all')
        # Only keep rows where 1 reported value is different from it's last
        # reported value:

        def _change_indicator(ds: pd.Series):
            # Find which rows contain any changes by forward-filling NaN values:
            dsf = ds.ffill()
            dsf_shifted = dsf.shift(1)
            # Identify changes where the current value differs from the
            # previous non-NaN value:
            change = (dsf != dsf_shifted) & ds.notna()
            # Also capture the first non-NaN value:
            first_non_na = dsf_shifted.isna() & ds.notna()
            return change | first_non_na

        # Determine if any column has a change in each row
        any_change = suppl.apply(_change_indicator).any(axis=1)
        suppl = suppl[any_change]
        # Join back in:
        command = command.join(suppl, how='outer')
        # Enforce column order, with suppl joined in:
        command = command[command_cols]
        _df_to_excel_sheet(writer, command, '_Commands')
        progress.advance(task)

        # Collect Credits:
        progress.update(task, description="Credits")
        credits = df.filter([
            c for c in df.columns if (
                c in [
                    'GroundInterface_BroughtToYouBy',
                    'GroundInterface_SpecialMessage',
                    'CommandDispatcher_NoOpStringReceived'
                ]
            )
        ])
        credits = credits.dropna(axis=0, how='all')
        _df_to_excel_sheet(writer, credits, '_Credits', rotate_headers=False)
        progress.advance(task)

        # Create a special sheet of all thermal data:
        progress.update(task, description="Thermal")
        thermal_cols = [
            'WatchdogHeartbeatTvac_WatchdogMode',
            'MetaModPeregrine_DeckD2TempKelvin',
            'MetaModTemps_BatteryTempAvgKelvin',
            'WatchdogHeartbeat_BattAdcTempKelvin',
            'WatchdogHeartbeatTvac_AdcTempKelvin',
            'MetaModTemps_BoardTherm0_RT1Kelvin',
            'MetaModTemps_BoardTherm1_RT2Kelvin',
            'MetaModTemps_BoardTherm2_RT3Kelvin',
            'MetaModTemps_BoardTherm3_RT4Kelvin',
            'MetaModTemps_BoardTherm4_RT5Kelvin',
            'MetaModTemps_BoardTherm5_RT6Kelvin',
            'MetaModTemps_RoverTherm6_TMR1Kelvin',
            'MetaModTemps_RoverTherm7_TMR2Kelvin',
            'MetaModTemps_RoverTherm8_TMR3Kelvin',
            'MetaModTemps_RoverTherm9_TMR4Kelvin',
            'MetaModTemps_RoverTherm10_TMR5Kelvin',
            'MetaModTemps_RoverTherm11_TMR6Kelvin',
            'MetaModTemps_RoverTherm12_TMR7Kelvin',
            'MetaModTemps_RoverTherm13_TMR8Kelvin',
            'MetaModTemps_RoverTherm14_TMR9Kelvin',
            'MetaModTemps_RoverTherm15_TMR10Kelvin',
            'WatchdogDetailedStatus_Heater_OffTempKelvin',
            'WatchdogDetailedStatus_Heater_OnTempKelvin',
            'MetaModWatchdogMonitor_Heater',
            'WatchdogHeartbeat_HeaterStatus',
            'WatchdogHeartbeatTvac_HeaterStatus',
            'WatchdogHeartbeatTvac_HeatingControlEnabled',
            'WatchdogDetailedStatus_Heater_ControlEnabled',
            'WatchdogDetailedStatus_Heater_IsHeating',
            'WatchdogDetailedStatus_Heater_EffectivePower',
            'WatchdogDetailedStatus_Heater_EffectivePowerLimit',
            'MetaModPeregrine_OperationalPowerAvg_W',
            'MetaModPeregrine_OperationalPowerMax_W',
            'MetaModPeregrine_ReleasePowerAvg_W',
            'MetaModPeregrine_ReleasePowerMax_W',
            'WatchdogDetailedStatus_Heater_EffectiveVoltage',
            'MetaModTransitPower_VHeaterLive',
            'WatchdogDetailedStatus_Heater_DutyCycleCounter',
            'WatchdogDetailedStatus_Heater_DutyCyclePercent',
            'WatchdogDetailedStatus_Heater_DutyCyclePeriodCycles',
            'WatchdogDetailedStatus_Heater_DutyCyclePeriodMs',
            'WatchdogDetailedStatus_Heater_PwmLimit_DutyCycleCounter',
            'WatchdogDetailedStatus_Heater_PwmLimit_DutyCyclePercent',
            'WatchdogHeartbeatTvac_HeaterPwmDutyCycle',
            'WatchdogHeartbeatTvac_HeaterPwmLimit'
        ]
        thermal_cols = [x for x in thermal_cols if x in df.columns]
        thermal = df[thermal_cols]
        thermal = thermal.dropna(axis=0, how='all')
        # Convert boolean-only columns to numeric:
        thermal = _convert_bool_obj_cols_to_float(thermal)
        _df_to_excel_sheet(writer, thermal, '_Thermal')
        progress.advance(task)
        # Also create one with missing values interpolated:
        thermal_interp = _type_aware_telem_interpolate(thermal)
        _df_to_excel_sheet(writer, thermal_interp, '_ThermalInterp')
        progress.advance(task)

        # Create a special sheet of all power data:
        progress.update(task, description="Power")
        power_cols = [
            'GdsPackets_WatchdogSafetyTimerPacket',
            'WatchdogHeartbeatTvac_WatchdogMode',
            'Peregrine_IrisOperationalEnabledFet',
            'Peregrine_IrisReleaseEnabledFet',
            'WatchdogDetailedStatus_Io_BatteryState',
            'MetaModRoverPower_BatteryChargeAh',
            'MetaModRoverPower_BatteryEnergyWh',
            'MetaModRoverPower_BatteryStateOfCharge',
            'MetaModRoverPower_BatteryStateOfCharge_Capacity',
            'MetaModRoverPower_BatteryStateOfCharge_Energy',
            'MetaModTemps_BatteryTempAvgKelvin',
            'MetaModRoverPower_FullSystemPowerEst_W',
            'MetaModRoverPower_MotorPowerEst_W',
            'MetaModRoverPower_Power3V3Est_W',
            'WatchdogDetailedStatus_Heater_EffectivePower',
            'MetaModTransitPower_HeaterPowerLive_W',
            'WatchdogDetailedStatus_Heater_EffectivePowerLimit',
            'MetaModPeregrine_OperationalPowerAvg_W',
            'MetaModPeregrine_OperationalPowerMax_W',
            'MetaModPeregrine_ReleasePowerAvg_W',
            'MetaModPeregrine_ReleasePowerMax_W',
            'MetaModPeregrine_OperationalCorrectedCurrentAvg_mA',
            'MetaModPeregrine_OperationalCorrectedCurrentMax_mA',
            'MetaModPeregrine_ReleaseCurrentAvg_mA',
            'MetaModPeregrine_ReleaseCurrentMax_mA',
            'MetaModRoverPower_FullSystemCurrent_mA',
            'MetaModRoverPower_Fpga1V2Current_mA',
            'MetaModRoverPower_Fpga3V3Current_mA',
            'MetaModRoverPower_Full3V3Current_mA',
            'MetaModRoverPower_Hercules1V2Current_mA',
            'MetaModRoverPower_Hercules3V3Current_mA',
            'MetaModRoverPower_MotorBusCurrent_mA',
            'MetaModRoverPower_Radio3V3Current_mA',
            'MetaModWatchdogMonitor_VLR_Enable',
            'MetaModWatchdogMonitor_VSA_Enable',
            'MetaModWatchdogMonitor_3V3_Enable',
            'MetaModWatchdogMonitor_BattStat',
            'MetaModWatchdogMonitor_Battery_Enable',
            'MetaModWatchdogMonitor_LatchBatt',
            'MetaModWatchdogMonitor_LatchReset',
            'MetaModWatchdogMonitor_LatchSet',
            'MetaModWatchdogMonitor_BmsBoot',
            'MetaModWatchdogMonitor_CamSelect',
            'MetaModWatchdogMonitor_Charger_Enable',
            'MetaModWatchdogMonitor_Deployment',
            'MetaModWatchdogMonitor_FpgaNRst',
            'MetaModWatchdogMonitor_FpgaOn',
            'MetaModWatchdogMonitor_Heater',
            'MetaModWatchdogMonitor_HerculesOn',
            'MetaModWatchdogMonitor_Hercules_NPORRST',
            'MetaModWatchdogMonitor_Hercules_NRST',
            'MetaModWatchdogMonitor_RadioNRst',
            'MetaModWatchdogMonitor_RadioOn',
            'WatchdogDetailedStatus_Io_ChargingStatus1',
            'WatchdogDetailedStatus_Io_ChargingStatus2',
            'WatchdogHeartbeatTvac_Voltage',
            'WatchdogHeartbeat_BatteryVoltageOk',
            'WatchdogDetailedStatus_Io_BatteryConnectionStatus',
            'WatchdogDetailedStatus_Io_BatteryLatchStatus',
            'WatchdogDetailedStatus_Io_ChargerState',
            'WatchdogDetailedStatus_Io_5V0PowerGood',
            'WatchdogDetailedStatus_Io_3V3PowerGood',
            'WatchdogDetailedStatus_Io_1V8PowerGood',
            'WatchdogDetailedStatus_Io_1V2PowerGood',
            'WatchdogDetailedStatus_Adc_Vcc28Voltage',
            'WatchdogDetailedStatus_Adc_LanderVoltage',
            'MetaModRoverPower_HerculesLanderVoltage',
            'MetaModRoverPower_LanderVoltage_FusedEst',
            'WatchdogDetailedStatus_Heater_EffectiveVoltage',
            'MetaModTransitPower_VHeaterLive',
            'WatchdogDetailedStatus_Adc_SwitchedBatteryVoltage',
            'WatchdogDetailedStatus_Adc_FullSystemVoltage',
            'WatchdogDetailedStatus_Adc_Vcc24Voltage',
            'WatchdogDetailedStatus_Adc_2V8Voltage',
            'WatchdogDetailedStatus_Adc_2V5Voltage',
            'WatchdogHeartbeat_ChargePercent',
            'WatchdogHeartbeat_CurrentMilliamps'
        ]
        power_cols = [x for x in power_cols if x in df.columns]
        power = df[power_cols]
        power = power.dropna(axis=0, how='all')
        # Convert boolean-only columns to numeric:
        power = _convert_bool_obj_cols_to_float(power)
        _df_to_excel_sheet(writer, power, '_Power')
        progress.advance(task)
        # Also create one with missing values interpolated:
        power_interp = _type_aware_telem_interpolate(power)
        _df_to_excel_sheet(writer, power_interp, '_PowerInterp')
        progress.advance(task)

    def save_to_excel(
        self,
        file_path: str,
        generate_telem_tables: bool = False,
        work_on_copy: bool = True
    ) -> None:
        """Save this DataSet as an excel file.
        This operation modifies the dataset (changing column formats, etc), so
        it works on a copy and doesn't make any changes to the source data
        unless `work_on_copy=False`. Only do this if the dataset is
        prohibitively large to copy.

        If `generate_telem_tables`, this will assume it's working on a
        telemetry dataset and build numerous telemetry tables.
        """
        # We'll need to modify the dataset to write, so just create a deep copy
        # so we don't modify the original:
        # Temporarily convert time index to Excel-friendly format:
        if work_on_copy:
            logger.debug("Creating copy of data for Excel ...")
            df = self.data.copy(deep=True)
        else:
            logger.debug("Modifying original dataset to fit Excel format ...")
            df = self.data

        orig_index = df.index
        df.index = orig_index.tz_convert(None)

        # Reformat datetimes to allow them to be saved in Excel:
        df = _delocalize_datetime_columns(df)

        logger.debug("Building & saving Excel sheets . . .")
        with Progress() as progress:
            task = progress.add_task("", True, 0, 10)
            try:
                writer = pd.ExcelWriter(
                    file_path,
                    engine='xlsxwriter', mode='w'
                )
                # Save the all the data:
                progress.update(task, description="Full table")
                _df_to_excel_sheet(writer, df, "_All")
                progress.advance(task)

                if generate_telem_tables:
                    self._save_telem_tables_to_excel(
                        df, writer, progress, task)

                # Freeze and rotate all top rows of all sheets for visibility:
                logger.debug("Formatting all Excel Sheets . . .")
                for sheet in writer.sheets.values():
                    sheet.freeze_panes(1, 0)  # Freeze the first row.
                    # Autofit all column widths:
                    sheet.autofit()

                # Make sure the full telem sheet is default when opened:
                writer.sheets['_All'].activate()

                writer.close()
                progress.update(task, total=1, completed=1)
            finally:
                # Make sure to reset time index:
                df.index = orig_index

    def save_to_parquet(self, file_path: str) -> None:
        """Saves to the given parquet file."""
        self.data.to_parquet(
            file_path,
            compression='gzip',
            index=True
        )

    @staticmethod
    def _preprocess_df_from_hdf(df: pd.DataFrame) -> pd.DataFrame:
        """Re-standardize data coming in from and HDF that was formatted with
        the inverse of this function: `_postprocess_df_for_hdf`."""
        for column in df.columns:
            # Check the type of the column to determine which sparse dtype to use
            if pd.api.types.is_string_dtype(df[column].dtype):
                df[column] = df[column].where(df[column] != "")
        return df

    @staticmethod
    def _postprocess_df_for_hdf(df: pd.DataFrame) -> pd.DataFrame:
        """Remove/reformat any DataFrame aspects (like NA and extension types)
        unsupported by HDFStore.

        Inverse is `_preprocess_data_from_hdf`."""
        # pytables' HDFStore doesn't support pd.NA in columns or extension types like
        # Sparse, so we just have to convert NAs to magics and convert back on decode:
        for column in df.columns:
            # Check the type of the column to determine which sparse dtype to use
            if pd.api.types.is_string_dtype(df[column].dtype):
                df[column].fillna("", inplace=True)
            elif pd.api.types.is_numeric_dtype(df[column].dtype):
                df[column].fillna(np.nan, inplace=True)
            else:
                # No other option supported by HDFStore, need to nan here too.
                # This shouldn't typically occur since, at time of writing, all
                # possible types will end up as string or float64, though others are
                # possible.
                df[column].fillna(np.nan, inplace=True)
        return df

    def _store_in_hdf(self, name: str, store: pd.HDFStore) -> None:
        """ Save in the given `HDFStore` with the given name.
        Wrapped in case we want to add any pre-save logic.

        NOTE: This requires an instance of HDFStore to be maintained by
        surrounding code. Example:
        ```
        db = pd.HDFStore(h5_file_path)
        yamcs_archive = DataSet._store_in_hdf(settings['YAMCS_KEY'], db)
        db.close()
        ```
        """
        hdf_data = self.data.copy(deep=True)  # don't alter original
        hdf_data = self.__class__._postprocess_df_for_hdf(hdf_data)
        store[name] = hdf_data

    def save_to_h5(self, file_path: str, key: str) -> None:
        """Saves to the given key in an HDFStore in an `h5` file, including
        managing the lifecycle of the DB accessor."""
        db = pd.HDFStore(file_path)
        self._store_in_hdf(key, db)
        db.close()

    @classmethod
    def load_from_h5(cls, file_path: str, key: str) -> DataSet:
        """Loads from the given key in an HDFStore in an `h5` file, including
        managing the lifecycle of the DB accessor."""
        db = pd.HDFStore(file_path)
        archive = cls._load_from_hdf(key, db)
        db.close()
        return archive

    @classmethod
    def _load_from_hdf(cls, name: str, store: pd.HDFStore) -> DataSet:
        """ Load from the given `HDFStore` with the given name.
        Wrapped so we can perform any post-load logic.

        NOTE: This requires an instance of HDFStore to be maintained by
        surrounding code. Example:
        ```
        db = pd.HDFStore(h5_file_path)
        yamcs_archive = DataSet._load_from_hdf(settings['YAMCS_KEY'], db)
        db.close()
        ```
        """
        df: pd.DataFrame
        if name in store:
            df = cls._preprocess_df_from_hdf(store[name])
        else:
            logger.warning(
                f"Key `{name}` not found in HDFStore with keys {[*store.keys()]}. "
                f"Creating blank DataFrame but consider this may have been a typo."
            )
            df = pd.DataFrame()  # just create a DataSet around an empty df
        return cls(df)

    @classmethod
    def load_from_parquet(cls, file_path: str) -> DataSet:
        """Loads archive from the given parquet file."""
        return cls(pd.read_parquet(
            file_path
        ))

    @classmethod
    def load_from_file(cls, file_path: str, key: str) -> DataSet:
        """General archive file loader. Loads archive from the given archive
        file. Accesses using the given key if the archive is keyed.
        Supports `h5` and `parquet`.
        """
        ext = file_path.split('.')[-1]
        if ext.lower() == 'parquet':
            return cls.load_from_parquet(file_path)
        elif ext.lower() == 'h5':
            return cls.load_from_h5(file_path, key)
        else:
            raise ValueError(
                f"Cannot load Archive DataSet from `{file_path}` with `{key=}`. "
                f"File extension `{ext}` is unsupported."
            )

    @classmethod
    def from_dataseries(
        cls,
        datas: List[DataSeries],
        time_tolerance: pd.Timedelta = pd.Timedelta('1us')
    ) -> DataSet:
        df = pd.DataFrame()
        if len(datas) == 0:
            df = pd.DataFrame()
        elif len(datas) == 1:
            df = datas[0]
        else:
            # Slice off the first dataset so it can serve as our base:
            df, *remaining_datas = datas
            df = df.data
            # Round time index to time_tolerance before merging:
            df.index = df.index.round(time_tolerance)
            # Successively merge in each data series by time (index), ensuring
            # all rows from both datasets are preserved but merging rows if
            # they have the same time value:
            for ds in remaining_datas:
                df2 = ds.data
                df2.index = df2.index.round(time_tolerance)
                df = df.join(df2, how='outer')

        return cls(df)

    @classmethod
    def from_tuples_dict(
        cls,
        data: Dict[str, List[Tuple[datetime, Any]]],
        time_tolerance: pd.Timedelta = pd.Timedelta('1us')
    ) -> DataSet:
        """
        Create from a dictionary that maps field names to lists of (time, data)
        tuples.
        When merging data series, events that happened within `time_tolerance`
        of each other will be considered to have happened at the same time (and
        thus be listed on the same row).
        """
        # Create a properly formatted DataSeries from each entry:
        datas = [
            DataSeries.from_tuples_list(name, points)
            for name, points in data.items()
            if len(points) > 0
        ]
        # Build DataSet:
        return cls.from_dataseries(datas, time_tolerance=time_tolerance)
