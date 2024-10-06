"""
Watchdog Monitor telemetry fields.

(human-friendly versions of detailed status & keep alive packets as well as
Watchdog-Hercules health messages).

Last Update: 01/17/2024
"""
from typing import Final, List, Tuple, Type, Dict

import re

from IrisBackendv3.data_standards.module import TelemetryChannel, Event
from IrisBackendv3.data_standards.fsw_data_type import FswDataType
from IrisBackendv3.codec.payload import (
    DownlinkedPayload, TelemetryPayload, EventPayload
)
from IrisBackendv3.meta.metafield import (
    MetaModule, MetaChannel, MetaChannelUpdateBehavior
)
import IrisBackendv3.codec.packet_classes.watchdog_detailed_status as WDS


def CDS_Builder(
    field_abbr: str,
    field_name: str
) -> Type[MetaChannel]:
    """Breakout all switch telem into strings (for state plot)."""
    class CombinedDigitalState(MetaChannel):
        _PROTO = TelemetryChannel(field_name, 0, FswDataType.U8)
        _UPDATE_BEHAVIOR = MetaChannelUpdateBehavior.ANY
        _WATCHING = ['WatchdogDetailedStatus_Watchdog_CombinedDigitalStates']

        def _calculate(self) -> Tuple[int | None, List[DownlinkedPayload]]:
            t = self._get_t(
                'WatchdogDetailedStatus_Watchdog_CombinedDigitalStates'
            )
            channel = t.channel
            bitfields = channel.bitfields
            if bitfields is None:
                return None, []  # can't proceed, abort
            n_bytes = bitfields.total_bits // 8

            cds_int = int(t.data)
            cds_bytes = cds_int.to_bytes(n_bytes, 'big')
            raw_data = WDS.flip_all_bits_in_bytes(cds_bytes)
            fields = bitfields.unpack(raw_data)

            return fields[field_abbr], [t]
    return CombinedDigitalState


def StateEnumStringBuilder(
    telem_channel_name: str,
    state_map: Dict[int, str]
) -> Type[MetaChannel]:
    class StateEnumString(MetaChannel):
        _PROTO = TelemetryChannel(
            telem_channel_name+'String', 0, FswDataType.U8)
        _UPDATE_BEHAVIOR = MetaChannelUpdateBehavior.ANY
        _WATCHING = [telem_channel_name]

        def _calculate(self) -> Tuple[str, List[DownlinkedPayload]]:
            telem = self._get_t(telem_channel_name)
            int_val = telem.data

            return state_map.get(int_val, "UNKNOWN"), [telem]
    return StateEnumString


class WatchdogHerculesRoundtripTime_ms(MetaChannel):
    """Round-trip ping time (RTT), in milliseconds, of "stroking" protocol
    between Watchdog and Hercules. Good indicator of cumulative health of both
    chips.
    Normally this gets downlinked as a Debug packet, so we need to convert to
    telem channel.
    """
    _PROTO = TelemetryChannel(
        'WatchdogHerculesRoundtripTime_ms', 0, FswDataType.U16
    )
    _UPDATE_BEHAVIOR = MetaChannelUpdateBehavior.ANY
    _WATCHING = ['GdsPackets_WatchdogDebugPacket']

    def _calculate(self) -> Tuple[int | None, List[DownlinkedPayload]]:
        message_evt = self._get_e('GdsPackets_WatchdogDebugPacket')
        message_str = message_evt.formatted_string
        matches = re.search(r"RTT:\s*(\d*)\s*ms", message_str)
        rtt: int = 0
        if matches is None:
            return None, []
        else:
            try:
                rtt = int(matches.groups()[0])
            except ValueError:
                # Bad match (very unlikely but technically possible).
                # No telem here. Abort.
                return None, []
        return rtt, [message_evt]


class HerculesWatchdogRoundtripTime_ms(MetaChannel):
    """Round-trip ping time (RTT), in milliseconds, of "stroking" protocol
    from Hercules to Watchdog to Hercules.
    Good indicator of cumulative health of both chips.
    Normally this gets downlinked as a Debug packet, so we need to convert to
    telem channel.
    """
    _PROTO = TelemetryChannel(
        'HerculesWatchdogRoundtripTime_ms', 0, FswDataType.U16
    )
    _UPDATE_BEHAVIOR = MetaChannelUpdateBehavior.ANY
    _WATCHING = ['GdsPackets_WatchdogDebugPacket']

    _RE_PATTERN = re.compile(r"RTT:\s*(\d*)\s*ms")

    def _calculate(self) -> Tuple[int | None, List[DownlinkedPayload]]:
        message_evt = self._get_e('GdsPackets_WatchdogDebugPacket')
        message_str = message_evt.formatted_string
        matches = self._RE_PATTERN.search(message_str)
        rtt: int = 0
        if matches is None or len(matches.groups()) < 1:
            return None, []
        else:
            try:
                rtt = int(matches.groups()[0])
            except ValueError:
                # Bad match (very unlikely but technically possible).
                # No telem here. Abort.
                return None, []
        return rtt, [message_evt]


class WatchdogMissedStrokes(MetaChannel):
    """Number of times the Watchdog failed to respond to a stroke initiated by
    Hercules, as reported by Hercules (since Hercules boot).
    Good indicator of Watchdog health.
    Normally this gets downlinked as a Debug packet, so we need to convert to
    telem channel.
    """
    _PROTO = TelemetryChannel('WatchdogMissedStrokes', 0, FswDataType.U16)
    _UPDATE_BEHAVIOR = MetaChannelUpdateBehavior.ANY
    _WATCHING = ['GdsPackets_WatchdogDebugPacket']

    _RE_PATTERN = re.compile(
        r"Missed\s*responses:\s*(\d*).*skipped\s*sends:\s*(\d*)"
    )

    def _calculate(self) -> Tuple[int | None, List[DownlinkedPayload]]:
        message_evt = self._get_e('GdsPackets_WatchdogDebugPacket')
        message_str = message_evt.formatted_string
        matches = self._RE_PATTERN.search(message_str)
        misses: int = 0
        if matches is None or len(matches.groups()) < 1:
            return None, []
        else:
            try:
                misses = int(matches.groups()[0])
            except ValueError:
                # Bad match (very unlikely but technically possible).
                # No telem here. Abort.
                return None, []
        return misses, [message_evt]


class HerculesSkippedStrokes(MetaChannel):
    """Number of times the Hercules had to skip sending a stroke to the
    Watchdog (because a previous stroke was still pending), as reported by
    Hercules (since Hercules boot).
    Good indicator of Watchdog health.
    Normally this gets downlinked as a Debug packet, so we need to convert to
    telem channel.
    """
    _PROTO = TelemetryChannel('HerculesSkippedStrokes', 0, FswDataType.U16)
    _UPDATE_BEHAVIOR = MetaChannelUpdateBehavior.ANY
    _WATCHING = ['GdsPackets_WatchdogDebugPacket']

    _RE_PATTERN = re.compile(
        r"Missed\s*responses:\s*(\d*).*skipped\s*sends:\s*(\d*)"
    )

    def _calculate(self) -> Tuple[int | None, List[DownlinkedPayload]]:
        message_evt = self._get_e('GdsPackets_WatchdogDebugPacket')
        message_str = message_evt.formatted_string
        matches = self._RE_PATTERN.search(message_str)
        skips: int = 0
        if matches is None or len(matches.groups()) < 2:
            return None, []
        else:
            try:
                skips = int(matches.groups()[1])
            except ValueError:
                # Bad match (very unlikely but technically possible).
                # No telem here. Abort.
                return None, []
        return skips, [message_evt]


MOD_WATCHDOG_MON = MetaModule(
    # Note: this gets prefixed by `MetaModule.UNIVERSAL_PREFIX` b/c it's a MetaModule:
    name="WatchdogMonitor",
    ID=0xA700,
    meta_channels=[
        # NOTE: Order Matters
        # To preserve backwards compatibility, don't delete, just deprecate.
        # Break out Combined Digital States:
        CDS_Builder('V_LANDER_REG_EN', 'VLR_Enable')(),
        CDS_Builder('HEATER', 'Heater')(),
        CDS_Builder('DEPLOYMENT', 'Deployment')(),
        CDS_Builder('FPGA_KICK_AKA_CAM_SELECT', 'CamSelect')(),
        CDS_Builder('LATCH_BATT', 'LatchBatt')(),
        CDS_Builder('3V3_EN', '3V3_Enable')(),
        CDS_Builder('HERCULES_ON', 'HerculesOn')(),
        CDS_Builder('FPGA_ON', 'FpgaOn')(),
        CDS_Builder('CHRG_EN', 'Charger_Enable')(),
        CDS_Builder('BATTERY_EN', 'Battery_Enable')(),
        CDS_Builder('V_SYS_ALL_EN', 'VSA_Enable')(),
        CDS_Builder('HERCULES_N_RST', 'Hercules_NRST')(),
        CDS_Builder('HERCULES_N_PORRST', 'Hercules_NPORRST')(),
        CDS_Builder('FPGA_N_RST', 'FpgaNRst')(),
        CDS_Builder('RADIO_N_RST', 'RadioNRst')(),
        CDS_Builder('RADIO_ON', 'RadioOn')(),
        CDS_Builder('BMS_BOOT', 'BmsBoot')(),
        CDS_Builder('LATCH_SET', 'LatchSet')(),
        CDS_Builder('LATCH_RESET', 'LatchReset')(),
        CDS_Builder('BATT_STAT', 'BattStat')(),
        HerculesWatchdogRoundtripTime_ms(),
        WatchdogMissedStrokes(),
        HerculesSkippedStrokes()
    ],
    meta_events=[]
)

ALL_META_MODULES: Final[List[MetaModule]] = [
    MOD_WATCHDOG_MON
]
