"""
Tools for achieving consistent formatting of plots of FM1 data.

@author: Connor Colombo (CMU)
@last-updated: 10/13/2024
"""

import numpy as np
import pandas as pd  # type: ignore

from typing import Any, Tuple, List, Dict, Iterable, Callable
from datetime import datetime, timezone, timedelta
from collections import deque, OrderedDict
from dataclasses import dataclass, field

import matplotlib.pyplot as plt  # type: ignore
from matplotlib.figure import Figure  # type: ignore
from matplotlib.axes import Axes  # type: ignore
from matplotlib.axis import Axis  # type: ignore

from matplotlib import units as munits  # type: ignore
from matplotlib import ticker as mticker  # type: ignore

from scripts.utils.plotting_tools import (
    PlotSettings,
    create_figure,
    plot_main, plot_interp, plot_state_timeline,
    add_degC_y_axis, add_second_y_axis, set_ylim_from_df, _align_plot_y_axes,
    _align_x_axes,
    add_legends, prep_new_subplot,
    datasize_formatter, datarate_formatter,
    hide_lines, tidy_figure,
    save_figure
)


@dataclass
class RoverPlotData:
    """Container for key mission data by category and methods for extracting it."""
    # All mission data (e.g. from a telemetry archive `.parquet`):
    telem_full: pd.DataFrame

    # Whether or not to modify the source data field:
    inplace: bool = False

    start_time: datetime | None = None
    end_time: datetime | None = None

    # List out all columns we care about across all categories we care about:
    # Key is telemetry channel name, value is key to use in legend:
    thermal_cols: Dict[str, str] = field(default_factory=lambda: {
        'MetaModTemps_BoardTherm0_RT1Kelvin': 'SBC-RT1',
        'MetaModTemps_BoardTherm1_RT2Kelvin': 'SBC-RT2',
        'MetaModTemps_BoardTherm2_RT3Kelvin': 'SBC-RT3',
        'MetaModTemps_BoardTherm3_RT4Kelvin': 'SBC-RT4',
        'MetaModTemps_BoardTherm4_RT5Kelvin': 'SBC-RT5',
        'MetaModTemps_BoardTherm5_RT6Kelvin': 'SBC-RT6',
        'MetaModTemps_RoverTherm6_TMR1Kelvin': 'TMR1',
        'MetaModTemps_RoverTherm7_TMR2Kelvin': 'TMR2',
        'MetaModTemps_RoverTherm8_TMR3Kelvin': 'TMR3',
        'MetaModTemps_RoverTherm9_TMR4Kelvin': 'TMR4',
        'MetaModTemps_RoverTherm10_TMR5Kelvin': 'TMR5',
        'MetaModTemps_RoverTherm11_TMR6Kelvin': 'TMR6',
        'MetaModTemps_RoverTherm12_TMR7Kelvin': 'TMR7',
        'MetaModTemps_RoverTherm13_TMR8Kelvin': 'TMR8',
        'MetaModTemps_RoverTherm14_TMR9Kelvin': 'TMR9',
        'MetaModTemps_RoverTherm15_TMR10Kelvin': 'TMR10',
        'MetaModTemps_BatteryTempAvgKelvin': 'Battery (Avg.)',
        'MetaModPeregrine_DeckD2TempKelvin': 'Lander Deck (D2)',
        'WatchdogDetailedStatus_Heater_OffTempKelvin': 'Heater OFF Thresh.',
        'WatchdogDetailedStatus_Heater_OnTempKelvin': 'Heater ON Thresh.'
    })
    current_cols: Dict[str, str] = field(default_factory=lambda: {
        'MetaModPeregrine_OperationalCorrectedCurrentAvg_mA': 'From Lander (Avg)',
        'MetaModRoverPower_FullSystemCurrent_mA': 'Full-System (VSA)',
        'MetaModRoverPower_MotorBusCurrent_mA': 'Motors (24V)',
        'MetaModRoverPower_Full3V3Current_mA': '3V3',
        'MetaModRoverPower_Fpga3V3Current_mA': '3V3F',
        'MetaModRoverPower_Fpga1V2Current_mA': '1V2F',
        'MetaModRoverPower_Hercules3V3Current_mA': '3V3H',
        'MetaModRoverPower_Hercules1V2Current_mA': '1V2H',
        'MetaModRoverPower_Radio3V3Current_mA': '3V3R'
    })
    power_cols: Dict[str, str] = field(default_factory=lambda: {
        'MetaModTransitPower_HeaterPowerLive_W': 'Heater',
        'WatchdogDetailedStatus_Heater_EffectivePowerLimit': 'Heater (Limit)',
        'MetaModPeregrine_OperationalPowerAvg_W': 'From Lander (Avg)',
        'MetaModPeregrine_OperationalPowerMax_W': 'From Lander (Max)',
        'MetaModRoverPower_FullSystemPowerEst_W': 'Full-System (VSA)',
        'MetaModRoverPower_MotorPowerEst_W': 'Motors (24V)',
        'MetaModRoverPower_Power3V3Est_W': '3V3'
    })
    voltage_cols: Dict[str, str] = field(default_factory=lambda: {
        'MetaModRoverPower_LanderVoltage_FusedEst': 'Lander (28V)',
        'MetaModTransitPower_VHeaterLive': 'Heater',
        'WatchdogDetailedStatus_Adc_SwitchedBatteryVoltage': 'Battery (VBS)',
        'WatchdogDetailedStatus_Adc_FullSystemVoltage': 'Full-System (VSA)',
        'WatchdogDetailedStatus_Adc_Vcc24Voltage': 'Motor Bus (24V)',
        'WatchdogDetailedStatus_Adc_2V8Voltage': '2V8',
        'WatchdogDetailedStatus_Adc_2V5Voltage': '2V5'
    })
    battery_cols: Dict[str, str] = field(default_factory=lambda: {
        'MetaModRoverPower_BatteryChargeAh': 'Charge Available [Ah]',
        'MetaModRoverPower_BatteryEnergyWh': 'Energy Available [Wh]',
        'MetaModRoverPower_BatteryStateOfCharge': r'State of Charge [% Energy]'
    })
    state_cols: Dict[str, str] = field(default_factory=lambda: {
        'Peregrine_IrisOperationalEnabledFet': 'Operational Circuit',
        'Peregrine_IrisReleaseEnabledFet': 'Release Circuit',
        'Peregrine_SlipServiceStatus': 'Wired Comms',
        'Peregrine_WifiServiceStatus': 'Wireless Comms',

        'WatchdogDetailedStatus_Watchdog_State': 'Watchdog',
        'WatchdogDetailedStatus_Heater_ControlEnabled': 'Heating Control',
        'WatchdogDetailedStatus_Heater_IsHeating': 'Heater',
        'WatchdogDetailedStatus_Watchdog_DeploymentStatus': 'Deployment',
        'WatchdogDetailedStatus_Io_BatteryState': 'Battery',
        'WatchdogDetailedStatus_Io_ChargerState': 'Charger',
        'WatchdogDetailedStatus_Watchdog_Uart0State': 'WD-UART0',
        'WatchdogDetailedStatus_Watchdog_Uart1State': 'WD-UART1',

        'MetaModWatchdogMonitor_Deployment': 'WD-DEP',
        'MetaModWatchdogMonitor_BattStat': 'BSTAT',
        'MetaModWatchdogMonitor_Battery_Enable': 'BE',
        'MetaModWatchdogMonitor_LatchBatt': 'LB',
        'MetaModWatchdogMonitor_LatchReset': 'LR',
        'MetaModWatchdogMonitor_LatchSet': 'LS',
        'MetaModWatchdogMonitor_Charger_Enable': 'CE',

        'MetaModWatchdogMonitor_VLR_Enable': 'EN-VLR',
        'MetaModWatchdogMonitor_VSA_Enable': 'EN-VSA',

        'MetaModWatchdogMonitor_3V3_Enable': 'EN-3V3',

        'MetaModWatchdogMonitor_HerculesOn': 'HERC-ON',
        'MetaModWatchdogMonitor_Hercules_NPORRST': 'HERC-NPRST',
        'MetaModWatchdogMonitor_Hercules_NRST': 'HERC-NRST',

        'MetaModWatchdogMonitor_RadioOn': 'WF121-ON',
        'MetaModWatchdogMonitor_RadioNRst': 'WF121-NRST',
        'MetaModWatchdogMonitor_FpgaOn': 'FPGA-ON',

        'MetaModWatchdogMonitor_FpgaNRst': 'FPGA-NRST',
        'MetaModWatchdogMonitor_CamSelect': 'CAM-SEL',
        # Computed Columns (added in __post_init__):
        'Motor Bus': 'Motor Bus'
    })
    inertial_cols: Dict[str, str] = field(default_factory=lambda: {
        'MetaModImu_XAcc_MS2': r'Acceleration-X [$^{m}/_{s^2}$]',
        'MetaModImu_YAcc_MS2': r'Acceleration-Y [$^{m}/_{s^2}$]',
        'MetaModImu_ZAcc_MS2': r'Acceleration-Z [$^{m}/_{s^2}$]',
        'MetaModImu_GravityMagnitude_MS2': r'Vector Magnitude [$^{m}/_{s^2}$]'
    })
    angle_cols: Dict[str, str] = field(default_factory=lambda: {
        'MetaModImu_PitchAngleDeg': 'Pitch Angle Est. [deg]',
        'MetaModImu_RollAngleDeg': 'Roll Angle Est. [deg]'
    })
    ul_packet_cols: Dict[str, str] = field(default_factory=lambda: {
        'GroundInterface_GiCmdsUplinked': 'Uplinked Commands Received (GI)',
        'GroundInterface_GiPacketsReceived': 'Uplinked Packets Received (GI)',
        # Computed Columns (added in __post_init__):
        '*Data_Iris_Command_Count': 'Packets Uplinked (GSW)'
    })
    dl_packet_cols: Dict[str, str] = field(default_factory=lambda: {
        'GroundInterface_GiPacketsTransmitted': 'Packets Downlinked (GI)',
        # 'GroundInterface_GiTlmItemsReceived': 'Telemetry Count (GI)',
        # 'GroundInterface_GiTlmItemsDownlinked': 'Telemetry Downlink Count (GI)',
        # Computed Columns (added in __post_init__):
        '*Data_Iris_Packet_Count': 'Downlinked Packets Received (GSW)'
    })
    data_cols: Dict[str, str] = field(default_factory=lambda: {
        # Computed Columns (added in __post_init__):
        '*Data_Iris_Volume_Bytes': 'Mission Data Volume (Bytes)',
        '*Data_Iris_Volume_Bytes_w_CCSDS': 'Mission Data Volume, w/Overhead Penalty (Bytes)'
    })
    data_rate_cols: Dict[str, str] = field(default_factory=lambda: {
        # Computed Columns (added in __post_init__):
        '*Data_Iris_Rate_bps': 'Avg. Data Rate',
        '*Data_Iris_Rate_bps_w_CCSDS': 'Avg. Data Rate, w/Overhead Penalty'
    })

    thermal: pd.DataFrame = field(init=False)
    currents: pd.DataFrame = field(init=False)
    power: pd.DataFrame = field(init=False)
    voltages: pd.DataFrame = field(init=False)
    battery: pd.DataFrame = field(init=False)
    states: pd.DataFrame = field(init=False)
    inertial: pd.DataFrame = field(init=False)
    angle: pd.DataFrame = field(init=False)
    ul_packets: pd.DataFrame = field(init=False)
    dl_packets: pd.DataFrame = field(init=False)
    data: pd.DataFrame = field(init=False)
    data_rate: pd.DataFrame = field(init=False)

    @staticmethod
    def _get_cols(
        df: pd.DataFrame,
        cols_map: Dict[str, str]
    ) -> pd.DataFrame:
        """Gets the columns with the given names (dict keys) from the given
        DataFrame and then renames them to the given dict values (for more
        readable legends).
        NOTE: Columns are only renamed in the output, not the source DataFrame,
        though the two are still connected.
        """
        # Get columns:
        df2 = df[[*cols_map.keys()]]
        # Remove any rows that now don't contain any data
        # (b/c their data was only in other cols):
        df2 = df2.dropna(axis=0, how='all')
        # Rename columns:
        df2 = df2.rename(columns=cols_map)
        return df2

    @property
    def _active_telem(self) -> pd.DataFrame:
        if hasattr(self, 'telem') and self.telem is not None:
            return self.telem
        return self.telem_full

    def _ensure_cols(self, cols: List[str], df: pd.DataFrame | None = None) -> bool:
        if df is None:
            df = self._active_telem
        return all(x in df.columns for x in cols)

    def _ensure_cols_with_data(self, cols: List[str], df: pd.DataFrame | None = None) -> bool:
        if df is None:
            df = self._active_telem
        if not self._ensure_cols(cols, df):
            return False
        return all(df[c].size > 0 for c in cols)

    def _add_computed_packet_fields(
        self,
        N_CCSDS_overhead_bytes=14,
        max_packet_interval_s=50.0,
        interval_padding_factor=2.5,  # should be sized to land you between samples
        longest_packet_interval_s=491
    ) -> None:
        if not self._ensure_cols_with_data([
            '*Command_data_size',
            '*Packet_size'
        ]):
            return  # can't continue
        # NOTE: Needs to operate on `telem_full` since it needs to sum from
        # mission start.
        self.telem_full['*Data_Iris_Command_Count'] = (
            self.telem_full['*Command_data_size'] > 0
        ).cumsum()
        self.telem_full['*Data_Iris_Packet_Count'] = (
            self.telem_full['*Packet_size'] > 0
        ).cumsum()
        self.telem_full['*Data_Iris_Volume_Bytes'] = self.telem_full['*Packet_size'].cumsum()
        self.telem_full['*Data_Iris_Volume_Bytes_w_CCSDS'] = (
            self.telem_full['*Packet_size'] + N_CCSDS_overhead_bytes
        ).cumsum()

        # Compute Data Rate:
        # Average rate over maximum typ. packet interval during slowest telemetering:
        T_WIN = interval_padding_factor * max_packet_interval_s
        self.telem_full['*Data_Iris_Rate_bps'] = (
            (8*self.telem_full['*Packet_size']
             ).rolling(timedelta(seconds=T_WIN)).sum() / T_WIN
        )
        self.telem_full['*Data_Iris_Rate_bps_w_CCSDS'] = (
            (8*(self.telem_full['*Packet_size'] + N_CCSDS_overhead_bytes))
            .rolling(timedelta(seconds=T_WIN)).sum() / T_WIN
        )
        # Smooth over longest packet interval to remove impulses:
        self.telem_full['*Data_Iris_Rate_bps'] = self.telem_full['*Data_Iris_Rate_bps'].rolling(
            timedelta(seconds=longest_packet_interval_s)
        ).mean()
        self.telem_full['*Data_Iris_Rate_bps_w_CCSDS'] = self.telem_full['*Data_Iris_Rate_bps_w_CCSDS'].rolling(
            timedelta(seconds=longest_packet_interval_s)
        ).mean()

    def _add_computed_state_fields(self) -> None:
        if not self._ensure_cols_with_data([
            'WatchdogDetailedStatus_Adc_Vcc24Voltage'
        ], self._active_telem):
            return  # can't continue
        self._active_telem['Motor Bus'] = self._active_telem['WatchdogDetailedStatus_Adc_Vcc24Voltage']
        self._active_telem.loc[
            self._active_telem['WatchdogDetailedStatus_Adc_Vcc24Voltage'].notna()
            & (self._active_telem['WatchdogDetailedStatus_Adc_Vcc24Voltage'] <= 12),
            'Motor Bus'
        ] = 'OFF'
        self._active_telem.loc[
            self._active_telem['WatchdogDetailedStatus_Adc_Vcc24Voltage'].notna()
            & (self._active_telem['WatchdogDetailedStatus_Adc_Vcc24Voltage'] > 12),
            'Motor Bus'
        ] = 'ON'

    def _add_computed(self) -> None:
        # Create Computed Data Columns:
        self._add_computed_packet_fields()
        self._add_computed_state_fields()

    def _prune_junk_thermals(
        self,
        T_FLOOR: int = 200
    ) -> None:
        # Remove Junk Temperature Values:
        for c in self.thermal_cols.keys():
            T = self._active_telem[c]
            # Remove obviously failed samples:
            self._active_telem.loc[T < 1] = np.nan
            # Set a floor for likely bad data:
            self._active_telem.loc[T < T_FLOOR] = T_FLOOR

    def _prune_junk(self) -> None:
        # Prune any junk data:
        self._prune_junk_thermals()

    def _build_thermal(
        self,
        averaging_interval: timedelta = timedelta(minutes=1)
    ) -> None:
        self.thermal = self._get_cols(self._active_telem, self.thermal_cols)
        if 'Battery (Avg.)' not in [*self.thermal_cols.values()]:
            return  # can't continue
        # Smooth out noisy data:
        self.thermal['Battery (Avg.)'] = self.thermal['Battery (Avg.)'].rolling(
            averaging_interval
        ).mean()

    def plot_thermal(
        self,
        settings: PlotSettings,
        axs: List[Axis],
        plot_idx: int
    ) -> List[int]:
        plot_interp(settings, axs[plot_idx],
                    ('', 'Temperature [K]'), self.thermal)
        plot_main(settings, axs[plot_idx],
                  ('', 'Temperature [K]'), self.thermal)

        add_degC_y_axis(axs, plot_idx)
        # Tidy up just this plot:
        tidy_figure(settings, axs, only_plots=[plot_idx])
        return [plot_idx]

    def _build_currents(
        self,
        averaging_interval: timedelta = timedelta(seconds=90)
    ) -> None:
        self.currents = self._get_cols(self._active_telem, self.current_cols)
        # Smooth out noisy data:
        self.currents = self.currents.rolling(averaging_interval).mean()

    def plot_currents(
        self,
        settings: PlotSettings,
        axs: List[Axis],
        plot_idx: int
    ) -> List[int]:
        plot_main(
            settings, axs[plot_idx], ("", "Current (approx.) [mA]"),
            self.currents,
            linestyle='solid'
        )
        # plot_interp(settings, axs[plot_idx], ('', 'Current [mA]'), self.currents)
        # plot_main(settings, axs[plot_idx], ('', 'Current [mA]'), self.currents)
        # Tidy up just this plot:
        tidy_figure(settings, axs, only_plots=[plot_idx])
        return [plot_idx]

    def _build_power(
        self,
        averaging_interval: timedelta = timedelta(seconds=90)
    ) -> None:
        self.power = self._get_cols(self._active_telem, self.power_cols)
        self.power = self.power.rolling(averaging_interval).mean()

    def plot_power(
        self,
        settings: PlotSettings,
        axs: List[Axis],
        plot_idx: int
    ) -> List[int]:
        plot_main(
            settings, axs[plot_idx], ("", "Power [W]"), self.power, linestyle='solid'
        )
        # plot_interp(settings, axs[plot_idx], ('', 'Power [W]'), self.power)
        # plot_main(settings, axs[plot_idx], ('', 'Power [W]'), self.power)
        # Tidy up just this plot:
        tidy_figure(settings, axs, only_plots=[plot_idx])
        return [plot_idx]

    def _build_voltages(
        self,
        averaging_interval: timedelta = timedelta(seconds=30)
    ) -> None:
        self.voltages = self._get_cols(self._active_telem, self.voltage_cols)
        # Smooth out noisy data:
        self.voltages = self.voltages.rolling(averaging_interval).mean()

    def plot_voltages(
        self,
        settings: PlotSettings,
        axs: List[Axis],
        plot_idx: int
    ) -> List[int]:
        plot_interp(
            settings, axs[plot_idx], ("", "Voltage [V]"), self.voltages,
            linestyle='solid'
        )
        # plot_interp(settings, axs[plot_idx], ('', 'Voltage [V]'), self.voltages)
        # plot_main(settings, axs[plot_idx], ('', 'Voltage [V]'), self.voltages)
        # Tidy up just this plot:
        tidy_figure(settings, axs, only_plots=[plot_idx])
        return [plot_idx]

    def _build_battery(
        self,
        averaging_interval: timedelta = timedelta(minutes=1)
    ) -> None:
        self.battery = self._get_cols(self._active_telem, self.battery_cols)
        # Smooth out noisy data:
        self.battery = self.battery.rolling(averaging_interval).mean()

    def plot_battery(
        self,
        settings: PlotSettings,
        axs: List[Axis],
        plot_idx: int
    ) -> List[int]:
        AX_BATT_SOC = plot_idx
        AX_BAT_CH, AX_BATT_EN = add_second_y_axis(axs, AX_BATT_SOC, 2)
        plot_interp(
            settings, axs[AX_BATT_SOC],
            ('', r'State of Charge (Temperature-Compensated) [% of Energy]'),
            100*self.battery[[r'State of Charge [% Energy]']]
        )
        plot_main(
            settings, axs[AX_BATT_SOC],
            ('', r'State of Charge (Temperature-Compensated) [% of Energy]'),
            100*self.battery[[r'State of Charge [% Energy]']]
        )
        plot_interp(
            settings, axs[AX_BAT_CH],
            ('', 'Charge (Temperature-Compensated) [Ah]'),
            self.battery[['Charge Available [Ah]']])
        plot_main(
            settings, axs[AX_BAT_CH],
            ('', 'Charge (Temperature-Compensated) [Ah]'),
            self.battery[['Charge Available [Ah]']]
        )
        plot_interp(
            settings, axs[AX_BATT_EN],
            ('', 'Energy (Temperature-Compensated) [Wh]'),
            self.battery[['Energy Available [Wh]']]
        )
        plot_main(
            settings, axs[AX_BATT_EN],
            ('', 'Energy (Temperature-Compensated) [Wh]'),
            self.battery[['Energy Available [Wh]']]
        )

        settings.plot_axes[plot_idx] = [AX_BATT_SOC, AX_BAT_CH, AX_BATT_EN]
        # Tidy up just this plot:
        tidy_figure(settings, axs, only_plots=[plot_idx])
        # After tidying, re-format y-axes:
        axs[AX_BATT_SOC].yaxis.set_major_formatter(
            mticker.PercentFormatter(decimals=1)
        )
        axs[AX_BATT_EN].yaxis.set_major_formatter(
            mticker.FormatStrFormatter('%.2f')
        )
        return settings.plot_axes[plot_idx]

    def _build_state(self) -> None:
        # Have to use full_telem for states so we can forward fill from outside
        # of the time window:
        self.states = self._get_cols(self.telem_full, self.state_cols)

    @property
    def states_bound(self) -> pd.DataFrame:
        """Returns states table, bound by time."""
        if not hasattr(self, 'states'):
            return pd.DataFrame()
        else:
            return self._isolate_time_range(
                self.states, self.start_time, self.end_time
            )

    def plot_states(
        self,
        settings: PlotSettings,
        axs: List[Axis],
        plot_idx: int,
        fig: Figure,
        label_fit_factor=1.1,
        blank_interval=timedelta(minutes=15)
    ) -> List[int]:
        # Clip X to make sure it's not now outside of the target time bounds
        # (b/c we fetch states outside of there for fill):
        # (do this beforehand so we don't waste time plotting then cropping
        # extra data):
        og_start_time = settings.start_time
        og_end_time = settings.end_time
        if self.start_time is not None:
            if settings.start_time is None:
                settings.start_time = self.start_time
            else:
                # Get the tightest bounds of the two:
                settings.start_time = max(settings.start_time, self.start_time)
        if self.end_time is not None:
            if settings.end_time is None:
                settings.end_time = self.end_time
            else:
                # Get the tightest bounds of the two:
                settings.end_time = min(settings.end_time, self.end_time)

        plot_state_timeline(
            settings,
            fig, axs[plot_idx],
            self.states,
            label_fit_factor=label_fit_factor,
            blank_interval=blank_interval
        )

        # Restore original bounds:
        settings.start_time = og_start_time
        settings.end_time = og_end_time

        # Tidy up just this plot:
        tidy_figure(settings, axs, only_plots=[plot_idx])
        return [plot_idx]

    def _build_inertial(
        self,
        averaging_interval: timedelta = timedelta(minutes=10)
    ) -> None:
        self.inertial = self._get_cols(self._active_telem, self.inertial_cols)
        self.angle = self._get_cols(self._active_telem, self.angle_cols)
        # Smooth out noisy data:
        self.inertial = self.inertial.rolling(averaging_interval).mean()
        self.angle = self.angle.rolling(averaging_interval).mean()

    def plot_inertial(
        self,
        settings: PlotSettings,
        axs: List[Axis],
        plot_idx: int
    ) -> List[int]:
        AX_ACC = plot_idx
        AX_ANG = add_second_y_axis(axs, AX_ACC, 1)[0]
        plot_main(
            settings, axs[AX_ACC], ("", r"Acceleration [$^{m}/_{s^2}$]"),
            self.inertial, linestyle='solid'
        )
        plot_main(
            settings, axs[AX_ANG], ("", r"Angle [deg]"),
            self.angle, linestyle='solid'
        )
        settings.plot_axes[plot_idx] = [AX_ACC, AX_ANG]
        # Tidy up just this plot:
        tidy_figure(settings, axs, only_plots=[plot_idx])
        return settings.plot_axes[plot_idx]

    def _build_packets(self) -> None:
        self.dl_packets = self._get_cols(
            self._active_telem, self.dl_packet_cols
        )
        self.ul_packets = self._get_cols(
            self._active_telem, self.ul_packet_cols
        )

    def plot_packets(
        self,
        settings: PlotSettings,
        axs: List[Axis],
        plot_idx: int
    ) -> List[int]:
        AX_DL = plot_idx
        AX_UL = add_second_y_axis(axs, AX_DL, 1)[0]
        plot_main(
            settings, axs[AX_DL], ("", "Downlink Count"),
            self.dl_packets, linestyle='solid'
        )
        plot_main(
            settings, axs[AX_UL], ("", "Uplink Count"),
            self.ul_packets, linestyle='solid'
        )
        settings.plot_axes[plot_idx] = [AX_DL, AX_UL]
        # Tidy up just this plot, skipping y:
        tidy_figure(settings, axs, only_plots=[plot_idx], skip_y_align=True)
        # After tidying, align y-axes, ensuring they're integers:
        _align_plot_y_axes(settings, axs, plot_idx, force_integer=True)
        return settings.plot_axes[plot_idx]

    def _build_data_fields(self) -> None:
        self.data = self._get_cols(self._active_telem, self.data_cols)
        self.data_rate = self._get_cols(
            self._active_telem, self.data_rate_cols
        )

    def plot_data_fields(
        self,
        settings: PlotSettings,
        axs: List[Axis],
        plot_idx: int
    ) -> List[int]:
        AX_DATA = plot_idx
        AX_RATE = add_second_y_axis(axs, AX_DATA, 1)[0]
        plot_interp(
            settings, axs[AX_DATA],
            ("", "Data Volume [bytes]"),
            self.data, linestyle='solid'
        )
        plot_interp(
            settings, axs[AX_RATE],
            ("", r"Data Rate [$^{\mathrm{bits}}/_{s}$]"),
            self.data_rate, linestyle='solid'
        )
        settings.plot_axes[plot_idx] = [AX_DATA, AX_RATE]
        # Tidy up just this plot:
        tidy_figure(settings, axs, only_plots=[plot_idx])
        # After tidying, re-format y-axes:
        axs[AX_DATA].yaxis.set_major_formatter(datasize_formatter)
        axs[AX_RATE].yaxis.set_major_formatter(datarate_formatter)
        return settings.plot_axes[plot_idx]

    def _build_all(self) -> None:
        self._build_thermal()
        self._build_currents()
        self._build_power()
        self._build_voltages()
        self._build_battery()
        self._build_state()
        self._build_inertial()
        self._build_packets()
        self._build_data_fields()

    @staticmethod
    def _isolate_time_range(
        tlm: pd.DataFrame,
        start_time: datetime | None,
        end_time: datetime | None
    ) -> pd.DataFrame:
        if start_time is not None:
            tlm = tlm[(tlm.index >= start_time)]
        if end_time is not None:
            tlm = tlm[(tlm.index <= end_time)]
        return tlm.copy(deep=True)

    def _filter_columns(
        self,
        cols: Dict[str, str],
        inplace: bool = True
    ) -> Dict[str, str]:
        if not inplace:
            return {k: v for k, v in cols.items() if k in self.telem_full.columns}
        else:
            for k in [*cols.keys()]:
                if k not in self.telem_full.columns:
                    cols.pop(k)
            return cols

    def _filter_all_columns(self) -> None:
        for cols in self.all_cols.values():
            self._filter_columns(cols, inplace=True)

    @property
    def all_cols(self) -> Dict[str, Dict[str, str]]:
        """Returns a dictionary of all categorical column dictionaries."""
        return {
            'thermal_cols': self.thermal_cols,
            'current_cols': self.current_cols,
            'power_cols': self.power_cols,
            'voltage_cols': self.voltage_cols,
            'battery_cols': self.battery_cols,
            'state_cols': self.state_cols,
            'inertial_cols': self.inertial_cols,
            'angle_cols': self.angle_cols,
            'ul_packet_cols': self.ul_packet_cols,
            'dl_packet_cols': self.dl_packet_cols,
            'data_cols': self.data_cols,
            'data_rate_cols': self.data_rate_cols
        }

    def __post_init__(self) -> None:
        if not self.inplace:
            self.telem_full = self.telem_full.copy(deep=True)
        self._add_computed()
        self.telem = self._isolate_time_range(
            self.telem_full, self.start_time, self.end_time
        )
        self._filter_all_columns()
        self._prune_junk()
        self._build_all()

    def strip_plot(self) -> None:
        pass


def make_strip_chart(
    data: RoverPlotData,
    title: str = "",
    files_title: str = "rover_strip",
    dpi: int = 300,  # 300  # (300 is fast, 1200 is very high res)
    start_time: datetime | None = None,
    end_time: datetime | None = None,
    skip_empty_plots: bool = True,
    plot_thermal: bool = True,
    plot_voltages: bool = True,
    plot_power: bool = True,
    plot_currents: bool = True,
    plot_battery: bool = True,
    plot_states: bool = True,
    plot_imu: bool = True,
    plot_packets: bool = True,
    plot_data: bool = True,
    thermal_close_up_ignore: List[str] = [
        'TMR1', 'TMR2', 'TMR8', 'TMR9', 'TMR10',
        'Heater ON Thresh.', 'Heater OFF Thresh.'
    ],
    voltage_close_up_ignore: List[str] = [
        '2V8', '2V5'
    ],
    **plot_settings_kwargs
) -> Tuple[PlotSettings, Figure, List[Axis]] | None:
    """Stack all plots and make them share a common X-axis."""
    fig: Figure

    # Special function to build thermal plots:
    PLOT_TH: int = 0

    def _plot_thermal_main(
        cfg: PlotSettings, axs: List[Axis], plot_idx: int
    ) -> None:
        nonlocal PLOT_TH
        PLOT_TH = plot_idx
        data.plot_thermal(cfg, axs, PLOT_TH)

    # Thermal Close-up (remove static, outlier columns):
    PLOT_TH2: int = 0
    th2_ignore = [
        x for x in thermal_close_up_ignore if x in data.thermal.columns
    ]
    thermal_close = data.thermal.drop(columns=th2_ignore, inplace=False)

    def _plot_thermal_closeup(
        cfg: PlotSettings, axs: List[Axis], plot_idx: int
    ) -> None:
        nonlocal PLOT_TH2
        PLOT_TH2 = plot_idx
        AX_TH2 = data.plot_thermal(cfg, axs, PLOT_TH2)[0]
        # Remove columns after plotting all of them so colors match full thermal plot:
        hide_lines(cfg, axs[AX_TH2], th2_ignore)
        # Rescale y, ignoring hidden columns:
        set_ylim_from_df(axs[AX_TH2], thermal_close)
        add_degC_y_axis(axs, AX_TH2)

    # Voltage Close-up (remove low-voltage columns):
    PLOT_V2: int = 0
    v2_ignore = [
        x for x in voltage_close_up_ignore if x in data.voltages.columns
    ]
    voltage_close = data.voltages.drop(columns=v2_ignore, inplace=False)
    for c in voltage_close.columns:
        voltage_close[c] = voltage_close[c].where(voltage_close[c] > 20)

    def _plot_voltage_closeup(
        cfg: PlotSettings, axs: List[Axis], plot_idx: int
    ) -> None:
        nonlocal PLOT_V2
        PLOT_V2 = plot_idx
        AX_V2 = data.plot_voltages(cfg, axs, PLOT_V2)[0]
        # Remove columns after plotting all of them so colors match full thermal plot:
        hide_lines(cfg, axs[AX_V2], v2_ignore)
        # Rescale y, ignoring hidden columns:
        set_ylim_from_df(axs[AX_V2], voltage_close)

    # Special plotting function for states:
    def _plot_states(
        cfg: PlotSettings, axs: List[Axis], plot_idx: int
    ) -> None:
        data.plot_states(cfg, axs, plot_idx, fig)

    # Define all plots we want to make with Tuples of: `tables to check for data`,
    # `function to build plot`:
    PLOT_DEFS: List[Tuple[
        List[pd.DataFrame],
        Callable[[PlotSettings, List[Axis], int], Any]
    ]] = []
    if plot_thermal:
        PLOT_DEFS.append(([data.thermal], _plot_thermal_main))
        PLOT_DEFS.append(([thermal_close], _plot_thermal_closeup))
    if plot_voltages:
        PLOT_DEFS.append(([data.voltages], data.plot_voltages))
        PLOT_DEFS.append(([voltage_close], _plot_voltage_closeup))
    if plot_power:
        PLOT_DEFS.append(([data.power], data.plot_power))
    if plot_currents:
        PLOT_DEFS.append(([data.currents], data.plot_currents))
    if plot_battery:
        PLOT_DEFS.append(([data.battery], data.plot_battery))
    if plot_states:
        PLOT_DEFS.append(([data.states_bound], _plot_states))
    if plot_imu:
        PLOT_DEFS.append(([data.inertial, data.angle], data.plot_inertial))
    if plot_packets:
        PLOT_DEFS.append(
            ([data.ul_packets, data.dl_packets], data.plot_packets))
    if plot_data:
        PLOT_DEFS.append(([data.data, data.data_rate], data.plot_data_fields))

    # Plot all column sets with plots:
    N_PLOTS: int
    if skip_empty_plots:
        N_PLOTS = sum(sum(df.size for df in dfs) > 0 for dfs, _ in PLOT_DEFS)
    else:
        N_PLOTS = len(PLOT_DEFS)

    if N_PLOTS == 0:
        return None   # nothing to do

    cfg = PlotSettings(
        suptitle=title,
        title=files_title,
        start_time=start_time,
        end_time=end_time,
        num_subplots=N_PLOTS,
        figsize=(14, 15*N_PLOTS//2+1),
        dpi=dpi,
        **plot_settings_kwargs
    )
    fig, axs = create_figure(cfg)

    # Make all plots:
    plot_idx: int = 0
    for dfs, plot_func in PLOT_DEFS:
        if not skip_empty_plots or sum(df.size for df in dfs) > 0:
            prep_new_subplot(cfg, plot_idx)
            plot_func(cfg, axs, plot_idx)
            plot_idx += 1

    # Finish up:
    add_legends(
        cfg, axs,
        position_overrides={
            PLOT_TH2: 'upper left'
        },
        ignore_labels={
            PLOT_TH2: th2_ignore,
            PLOT_V2: v2_ignore
        },
        ignore_plots=[
            # PLOT_STATE
        ]
    )

    # Uniform x-formatting & align everything:
    # (skip y because that's handled by each plot and could be screwed up here):
    tidy_figure(cfg, axs, skip_y_align=True)

    return cfg, fig, axs


def make_diode_leakage_plot(
    data: RoverPlotData,
    files_title: str = "rover_strip",
    dpi: int = 300,  # 300  # (300 is fast, 1200 is very high res)
    start_time: datetime | None = None,
    end_time: datetime | None = None,
    averaging_interval: timedelta = timedelta(seconds=120),
    **plot_settings_kwargs
) -> Tuple[PlotSettings, Figure, List[Axis]] | None:
    title = (
        "Iris Diode-OR Leakage at High Temperature\n"
        "(Voltage at Battery Switch Output with Switch Off)"
    )
    cfg = PlotSettings(
        suptitle=title,
        title=files_title,
        start_time=start_time,
        end_time=end_time,
        figsize=(14, 8),
        dpi=dpi,
        **plot_settings_kwargs
    )
    fig, axs = create_figure(cfg)

    # Collect all Iris on-board RT temperAture readings and VBS readings:
    battRTCelsius = data.telem_full['MetaModTemps_BatteryTempAvgKelvin'] - 273.15
    vbs = data.telem_full['WatchdogDetailedStatus_Adc_SwitchedBatteryVoltage']
    batt_off = data.telem_full['WatchdogDetailedStatus_Io_BatteryState'].ffill(
    ) == 'DISCONNECTED'

    # Prune obviously junk data:
    battRTCelsius.loc[battRTCelsius < (200-273.15)] = np.nan

    # Smooth out noisy data:
    battRTCelsius = battRTCelsius.rolling(averaging_interval).mean()
    vbs = vbs.rolling(averaging_interval).mean()

    # Only grab data when battery is off:
    battRTCelsius = battRTCelsius[batt_off]
    vbs = vbs[batt_off]

    # Isolate specified time range:
    # (have to do this *after* battery state selection in case the last battery
    # state is outside the specified time window)
    if data.start_time is not None:
        battRTCelsius = battRTCelsius[
            (battRTCelsius.index >= pd.to_datetime(data.start_time, utc=True))
        ]
        vbs = vbs[(vbs.index >= pd.to_datetime(data.start_time, utc=True))]
    if data.end_time is not None:
        battRTCelsius = battRTCelsius[
            (battRTCelsius.index <= pd.to_datetime(data.end_time, utc=True))
        ]
        vbs = vbs[(vbs.index <= pd.to_datetime(data.end_time, utc=True))]

    # Remove all NA values from each data Series (their still time-stamped):
    battRTCelsius = battRTCelsius[~battRTCelsius.isna()]
    vbs = vbs[~vbs.isna()]

    # Since these telem items inherently come in at different times b/c they're in
    # different packets, we need to perform correspondence...
    # For every temperature reading (the more frequent of these two telem series),
    # grab the nearest voltage reading in time and use that reading.
    # NOTE: interpolation yielded bad results here because it would fill in data
    # for discontinuities poorly (i.e. add lines were there weren't any)
    df = pd.merge_asof(battRTCelsius, vbs,
                       on=vbs.index.name, direction='nearest')

    # Rename columns to make them easier to handle:
    df.rename(columns={
        'WatchdogDetailedStatus_Adc_SwitchedBatteryVoltage': 'VBS',
        'MetaModTemps_BatteryTempAvgKelvin': 'battCelsius'
    }, inplace=True)

    df.set_index('battCelsius', inplace=True)
    df = df.drop(columns=[data.telem_full.index.name])

    if df.size == 0:
        return None  # Nothing to plot

    df.plot(
        figsize=cfg.figsize,
        ax=axs[0],
        grid=True,
        xlabel='Avg. Battery Temperature [°C]',
        ylabel=(
            'Battery Switch Output Voltage (VBS) w/ Switch OFF [V]\n'
            '(note: poor accuracy at low voltages)'
        ),
        legend=False,
        marker='o',
        markersize=2,
        linestyle='none'  # just scatter plot these so we know where data is
    )

    return cfg, fig, axs
