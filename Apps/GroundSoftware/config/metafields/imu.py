"""
Metafields pertaining to IMU sensor data processing.

Last Update: 01/15/2024
"""
from typing import Final, List, Tuple, Type

import numpy as np

from IrisBackendv3.data_standards.module import TelemetryChannel, Event
from IrisBackendv3.data_standards.fsw_data_type import FswDataType
from IrisBackendv3.codec.payload import (
    DownlinkedPayload, TelemetryPayload, EventPayload
)
from IrisBackendv3.meta.metafield import (
    MetaModule, MetaChannel, MetaChannelUpdateBehavior
)


def _imu_raw_to_m_s(raw_val: float) -> float:
    """Scales raw IMU readings to m/s^2.
    Calibrated based on readings averaged over an hour during RC12 while rover
    was static on lander in Pittsburgh, PA (local grav. of 9.80094m/s^2).

    ADXL312ACPZ accelerometer data sheet says there's a linear relationship
    from LSB to vector.
    """
    return raw_val * 9.80094 / float(np.sqrt(9.911**2 + 2.271**2 + 181.9**2))


def AccMS2Builder(
    acc_name: str
) -> Type[MetaChannel]:

    class AccMS2(MetaChannel):
        """Acceleration in m/s^2"""
        _PROTO = TelemetryChannel(f'{acc_name}_MS2', 0, FswDataType.F64)
        _UPDATE_BEHAVIOR = MetaChannelUpdateBehavior.ANY
        _WATCHING = [f'Imu_{acc_name}']

        def _calculate(self) -> Tuple[float, List[DownlinkedPayload]]:
            telem = self._get_t(f'Imu_{acc_name}')
            raw_val = telem.data
            ms2_val = _imu_raw_to_m_s(raw_val)

            return ms2_val, [telem]

    return AccMS2


# Gravitometry:


class GravityMagnitudeMS2(MetaChannel):
    """Magnitude of Acceleration Vector in m/s^2."""
    _PROTO = TelemetryChannel('GravityMagnitude_MS2', 0, FswDataType.F64)
    _UPDATE_BEHAVIOR = MetaChannelUpdateBehavior.ANY
    _WATCHING = [
        'MetaModImu_XAcc_MS2',
        'MetaModImu_YAcc_MS2',
        'MetaModImu_ZAcc_MS2'
    ]

    def _calculate(self) -> Tuple[float, List[DownlinkedPayload]]:
        x_acc_telem = self._get_t('MetaModImu_XAcc_MS2')
        y_acc_telem = self._get_t('MetaModImu_YAcc_MS2')
        z_acc_telem = self._get_t('MetaModImu_ZAcc_MS2')
        x_acc = x_acc_telem.data
        y_acc = y_acc_telem.data
        z_acc = z_acc_telem.data

        acc_mag = float(np.sqrt(x_acc**2 + y_acc**2 + z_acc**2))

        return acc_mag, [x_acc_telem, y_acc_telem, z_acc_telem]


class PitchAngleDeg(MetaChannel):
    """Calculates the rover pitch angle (about Rover axes, not IMU axes)."""
    _PROTO = TelemetryChannel('PitchAngleDeg', 0, FswDataType.F64)
    _UPDATE_BEHAVIOR = MetaChannelUpdateBehavior.ANY
    _WATCHING = [
        'MetaModImu_XAcc_MS2',
        'MetaModImu_YAcc_MS2',
        'MetaModImu_ZAcc_MS2'
    ]

    def _calculate(self) -> Tuple[float, List[DownlinkedPayload]]:
        x_acc_telem = self._get_t('MetaModImu_XAcc_MS2')
        y_acc_telem = self._get_t('MetaModImu_YAcc_MS2')
        z_acc_telem = self._get_t('MetaModImu_ZAcc_MS2')
        x_acc = x_acc_telem.data
        y_acc = y_acc_telem.data
        z_acc = z_acc_telem.data

        acc_mag = float(np.sqrt(x_acc**2 + y_acc**2 + z_acc**2))
        if acc_mag == 0:
            pitch_deg = 0
        else:
            pitch_deg = float(  # type: ignore
                np.arcsin(y_acc / acc_mag) * 180.0/np.pi
            )

        return pitch_deg, [x_acc_telem, y_acc_telem, z_acc_telem]


class RollAngleDeg(MetaChannel):
    """Calculates the rover pitch angle (about Rover axes, not IMU axes)."""
    _PROTO = TelemetryChannel('RollAngleDeg', 0, FswDataType.F64)
    _UPDATE_BEHAVIOR = MetaChannelUpdateBehavior.ANY
    _WATCHING = [
        'MetaModImu_XAcc_MS2',
        'MetaModImu_YAcc_MS2',
        'MetaModImu_ZAcc_MS2'
    ]

    def _calculate(self) -> Tuple[float, List[DownlinkedPayload]]:
        x_acc_telem = self._get_t('MetaModImu_XAcc_MS2')
        y_acc_telem = self._get_t('MetaModImu_YAcc_MS2')
        z_acc_telem = self._get_t('MetaModImu_ZAcc_MS2')
        x_acc = x_acc_telem.data
        y_acc = y_acc_telem.data
        z_acc = z_acc_telem.data

        pitch_deg = float(np.arctan2(x_acc, z_acc) * 180.0/np.pi) + 180
        # Wrap to -180 to +180
        if pitch_deg > 180:
            pitch_deg = pitch_deg - 360

        return pitch_deg, [x_acc_telem, y_acc_telem, z_acc_telem]


MOD_IMU = MetaModule(
    # Note: this gets prefixed by `MetaModule.UNIVERSAL_PREFIX` b/c it's a MetaModule:
    name="Imu",
    ID=0xA600,
    meta_channels=[
        # NOTE: Order Matters
        # To preserve backwards compatibility, don't delete, just deprecate.
        AccMS2Builder('XAcc')(),
        AccMS2Builder('YAcc')(),
        AccMS2Builder('ZAcc')(),
        GravityMagnitudeMS2(),
        PitchAngleDeg(),
        RollAngleDeg()
    ],
    meta_events=[]
)

ALL_META_MODULES: Final[List[MetaModule]] = [
    MOD_IMU
]
