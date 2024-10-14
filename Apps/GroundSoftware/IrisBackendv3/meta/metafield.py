"""
Creates system for defining metachannels and events, i.e. telemetry channels
and events that are calculated on the Ground based on the values of other
telemetry and events.

Essentially, this provides MetaModules, MetaChannels, and MetaEvents that wrap
standard Modules, Channels, and Events and mirror these standard non-"Meta"
counterparts in behavior, except that they aren't downlinked, they're computed
based on other fields and those computation functions are included in the
"Meta" definitions.

@author: Connor W. Colombo (CMU)
@last-updated: 10/09/2024
"""
from typing import Any, Final, List, TypeVar, Generic, TypeAlias, Dict, Deque, Set, overload, cast, Tuple, Callable
from enum import Enum, auto
from abc import ABC, abstractmethod
from dataclasses import dataclass, field
from datetime import datetime, timedelta, timezone
from collections import deque, OrderedDict
from itertools import islice

import traceback

import attrs

import math
from numbers import Real

from IrisBackendv3.data_standards.logs import logger

from IrisBackendv3.data_standards.module import (
    Module, TelemetryChannel, Event
)
from IrisBackendv3.utils.nameiddict import NameIdDict
from IrisBackendv3.data_standards.data_standards import DataStandards
from IrisBackendv3.data_standards.exceptions import (
    StandardsFormattingException
)
from IrisBackendv3.codec.payload import (
    Payload, DownlinkedPayload,
    TelemetryPayload, EventPayload
)
from IrisBackendv3.codec.metadata import DataPathway, DataSource, DownlinkTimes

_MF_DEF_T = TypeVar('_MF_DEF_T', TelemetryChannel, Event)
_MF_CALC_T = TypeVar('_MF_CALC_T')


class MetaChannelUpdateBehavior(Enum):
    """Enum defining when a metachannel should recompute it's value."""
    # A new value for ANY watched value will trigger an output:
    ANY = auto()
    # ALL watched values must receive a new value since the last output before
    # a new output is generated:
    ALL = auto()
    # No conditions. Once there's enough data, this will always be emitted, no
    # faster than `_MIN_INTERVAL`:
    TIME_ONLY = auto()


class MetaField(Generic[_MF_DEF_T, _MF_CALC_T], ABC):
    """Definition for a meta-channel or meta-event, alongside the processing
    function/or that processes inputs to create a meta-field value.
    The actual logic of this process is handled by the
    abstractmethod (`_calculate`).

    Any necessary state information or helper functions for the functor can be
    stored inside the functor subclass.

    Each metafield will be given a (typ. singleton) instance of a subclass of
    this class.

    NOTE: `_calculate` is only called once at least `_N_MIN_VALS` values have
    been received for EACH field being  watched (i.e. only once we've gotten
    the min amount of data for each field). Regardless of `_UPDATE_BEHAVIOR`,
    no data will be output by this field until this condition is met.
    """
    # # CLASS VARIABLES TO OVERRIDE IN SUBCLASS:
    # [REQUIRED]:
    # Prototype for the datastandards definition for this field (all key
    # information, *EXCEPT* ID, that's resolved by the MetaModule).
    # When initializing, just set ID=0 (or any int).
    # Having a proto matters more for events than telem.
    _PROTO: _MF_DEF_T
    # String names of the fields (telemetry & events) this should watch
    # (what this value is derived from):
    _WATCHING: List[str]

    # [OPTIONAL]:
    # How many values to store in the FIFO:
    _CACHE_DEPTH: int = 1
    # How many values do we need to have received in order to be able to
    # calculate something (an example of where this is not 1 would be for
    # computing acceleration, which takes a second derivative, thus this would
    # need >=3 values):
    _N_MIN_VALS: int = 1
    # When an update should be triggered:
    _UPDATE_BEHAVIOR: MetaChannelUpdateBehavior = MetaChannelUpdateBehavior.ANY
    # MIN amount of time between updates (used for debouncing). Also, if update
    # behavior is set to TIME_ONLY, a value will be emitted at approximately this
    # interval. When `process` is called, a value is only output if more than
    # this interval has elapsed since the last time a value was emitted.
    # NOTE: As an interval, only the MIN is guaranteed here (the interval will
    # never be less than this interval but may be significantly more depending
    # on when data gets ingested).
    _MIN_INTERVAL: timedelta | None = None
    # Whether this should be DEPRECATED (if deprecated, the field definition
    # remains in place (so old records can still be read) but the processor
    # will never emit anything):
    _DEPRECATED: bool = False

    # # Internal state variables:
    # Deque for each watched field containing the last `_CACHE_DEPTH` entries
    # received:
    # NOTE: New values are `appendleft` to this deque, so the first n items in
    # the deque are the n most recent (more efficient for slicing).
    _deques: Dict[str, Deque[DownlinkedPayload]]
    # Which fields that this watches that have been received since the last
    # time `_calculate` was called:
    _received_since_last_calc: Set[str]
    # Last time this method was called:
    _time_of_last_calc: datetime

    @property
    def name(self) -> str: return self._PROTO.name

    @classmethod
    def build_ds_definition(cls, id: int) -> _MF_DEF_T:
        """Builds the definition that should go in the DataStandards for this
        module."""
        # Adopt ID determined by MetaModule:
        cls._PROTO.ID = id
        return cls._PROTO

    def __init__(self) -> None:
        # Validate config:
        if (
            self._UPDATE_BEHAVIOR == MetaChannelUpdateBehavior.TIME_ONLY
            and self._MIN_INTERVAL is None
        ):
            raise ValueError(
                f"MetaField `{self.__class__.name}` is set to `TIME_ONLY` but "
                "has no `_MIN_INTERVAL`. This must not be `None` in timed mode."
            )

        self.reset()

    def reset(self) -> None:
        """Full state reset."""
        self._create_deques()
        self._received_since_last_calc = set()
        self._time_of_last_calc = datetime.now(timezone.utc)

    def _create_deques(self) -> None:
        self._deques = {
            field_name: deque([], maxlen=self._CACHE_DEPTH)
            for field_name in self._WATCHING
        }

    def _field_has_data(self, field: str, amount: int = 1) -> bool:
        """Checks that the `field` with the given name has at least the given
        `amount` of entries in its FIFO. Most needed when `_N_MIN_VALS=0`,
        meaning data isn't guaranteed."""
        return len(self._deques[field]) >= amount

    @overload
    def _get(self, field: str) -> DownlinkedPayload: ...
    @overload
    def _get(self, field: str, last_n: int) -> List[DownlinkedPayload]: ...

    def _get(
        self,
        field: str,
        last_n: int = 1
    ) -> DownlinkedPayload | List[DownlinkedPayload]:
        """Gets the last `last_n` fields from the internal `deque`
        corresponding to the watched field with the given name."""
        if field not in self._WATCHING:
            raise ValueError(
                f"`{field=}` requested using `_get` but it's invalid for "
                f"MetaField `{self.__class__.name}` which only watches "
                f"`{self._WATCHING}`."
            )

        if last_n == 1:
            # Just grab the last item if that's all that's needed:
            return self._deques[field][-1]

        # If we need more than 1 item, we need to slice:
        return [*islice(self._deques[field], last_n)]

    # Wrappers with built-in narrowing casts since we'll pretty much always
    # known which kind of DownlinkedPayload we're getting:
    @overload
    def _get_t(self, field: str) -> TelemetryPayload: ...
    @overload
    def _get_t(self, field: str, last_n: int) -> List[TelemetryPayload]: ...

    def _get_t(
        self,
        field: str,
        last_n: int = 1
    ) -> TelemetryPayload | List[TelemetryPayload]:
        return cast(TelemetryPayload, self._get(field, last_n))

    @overload
    def _get_e(self, field: str) -> EventPayload: ...
    @overload
    def _get_e(self, field: str, last_n: int) -> List[EventPayload]: ...

    def _get_e(
        self,
        field: str,
        last_n: int = 1
    ) -> EventPayload | List[EventPayload]:
        return cast(EventPayload, self._get(field, last_n))

    def _get_last_n(
        self,
        last_n: int,
        time_window: timedelta | None = None,
        payload_filter: Callable[[DownlinkedPayload], bool] | None = None
    ) -> List[Tuple[str, DownlinkedPayload]]:
        """Returns the `last_n` values received from any field this MetaField
        is watching, sorted by `scet_est`.

        If `time_window` is given, only times from within this time window from
        the latest value will be considered.

        If a `payload_filter` is given, values will only considered if
        `payload_filter` returns `True` for them. Values will also be ignored
        if they have no `downlink_times.scet_est`.

        NOTE: If fewer than `last_n` values have been received overall,
        the returned collection will be shorter than `last_n`.

        Returns a list of (field_name, field_value) tuples, sorted by `scet_est`.
        """
        if payload_filter is None:
            payload_filter = (lambda x: True)
        all_values: List[Tuple[str, DownlinkedPayload]] = [
            (k, x) for k, deq in self._deques.items() for x in deq
            if (
                x.scet_est is not None
                and payload_filter(x)
                and not (isinstance(x, TelemetryPayload) and (
                    x.data is None
                    or (isinstance(x.data, Real) and math.isnan(x.data))
                ))
            )
        ]
        all_values = cast(List[Tuple[str, DownlinkedPayload]], sorted(
            all_values,
            key=lambda e: e[1].scet_est  # type: ignore
        ))
        if time_window is not None:
            t_last: datetime
            t_last = all_values[-1][1].scet_est  # type: ignore
            all_values = [
                (k, x) for k, x in all_values
                if cast(datetime, x.scet_est) > (t_last-time_window)
            ]

        return all_values[-last_n:]

    def _get_last_n_dict(
        self,
        last_n: int,
        time_window: timedelta | None = None,
        payload_filter: Callable[[DownlinkedPayload], bool] | None = None
    ) -> Dict[str, List[DownlinkedPayload]]:
        """Returns the `last_n` values received from any field this MetaField
        is watching, sorted by `scet_est`.

        If `time_window` is given, only times from within this time window from
        the latest value will be considered.

        If a `payload_filter` is given, values will only considered if
        `payload_filter` returns `True` for them. Values will also be ignored
        if they have no `downlink_times.scet_est`.

        NOTE: If fewer than `last_n` values have been received overall,
        the returned collection will be shorter than `last_n`.

        Returns a dict where these items are split by field name. Watched
        fields which haven't been in the `last_n` received values won't have an
        entry in the dictionary.
        Returned values are sorted by `scet_est`.
        """
        last_values = self._get_last_n(last_n, time_window, payload_filter)
        split: Dict[str, List[DownlinkedPayload]] = dict()
        for k, v in last_values:
            if k not in split:
                split[k] = []
            split[k].append(v)
        return split

    def _get_time_weighted_avg(
        self,
        last_n: int | None = None,
        time_window: timedelta | None = None,
        payload_filter: Callable[[TelemetryPayload], bool] | None = None,
        extra_weights: Dict[str, float] | None = None
    ) -> Tuple[float | None, List[TelemetryPayload]]:
        """
        Returns the time weighted average of all queued values received for
        this fields this MetaField watches, or just up to the `last_n` if
        `last_n` is given.

        If `time_window` is given, only times from within this time window from
        the latest value will be considered.

        If an `extra_weights` dictionary is given, these will be applied to
        values from the corresponding channels. Extra weights MUST have a key
        for every watched field if given.

        If a `payload_filter` is given, values will only considered if
        `payload_filter` returns `True` for them. Values will also be ignored
        if they have no `downlink_times.scet_est`.

        Only works on `TelemetryPayloads`, ignores any `EventPayloads`.
        See `_get_last_n` for more details on how filtering works.

        Returns the average and all payloads used in that calculation
        """
        if extra_weights is not None:
            if any(k not in extra_weights for k in self._WATCHING):
                raise ValueError(
                    f"`{extra_weights=}` must have an entry for every "
                    f"watched field in `{self._WATCHING=}`!"
                )

        if last_n is None:
            last_n = sum(len(x) for x in self._deques.values())

        # Isolate `TelemetryPayloads`:
        def base_filter(x: DownlinkedPayload) -> bool:
            return isinstance(x, TelemetryPayload)

        total_filter: Callable[[DownlinkedPayload], bool]
        if payload_filter is None:
            total_filter = base_filter
        else:
            def total_filter(x: DownlinkedPayload) -> bool:
                pf = cast(Callable[[TelemetryPayload], bool], payload_filter)
                return base_filter(x) and pf(cast(TelemetryPayload, x))

        # Our `base_filter` guarantees that the results are TelemetryPayloads:
        recents = cast(List[Tuple[str, TelemetryPayload]],
                       self._get_last_n(last_n, time_window, total_filter))

        if len(recents) == 0:
            return None, []

        if len(recents) == 1:
            return recents[0][1].data, [recents[0][1]]

        if len(recents) == 2:
            avg = (recents[0][1].data + recents[1][1].data) / 2.0
            return avg, [recents[0][1], recents[1][1]]

        # Get duration:
        # NOTE: `_get_last_n` guarantees sort by time and that all payloads
        # have `downlink_times` and `scet_est`
        t_start = cast(datetime, recents[0][1].scet_est)
        t_end = cast(datetime, recents[-1][1].scet_est)
        total_duration = (t_end - t_start).total_seconds()

        # Perform weighted avg:
        total_extra_weights: float = 0.0
        weighted_sum: float = 0.0
        for i, (k, p) in enumerate(recents):
            v = p.data
            t = cast(datetime, p.scet_est)
            t_next: datetime
            if (i+1) < len(recents):
                t_next = cast(datetime, recents[i+1][1].scet_est)
            else:
                t_next = t_end
            duration = (t_next - t).total_seconds()
            contribution = v * duration
            if extra_weights is not None:
                contribution *= extra_weights[k]
                total_extra_weights += extra_weights[k]
            weighted_sum += contribution

        twa: float = 0.0
        if total_duration != 0.0:
            twa = weighted_sum / total_duration
        if total_extra_weights != 0.0:
            twa = twa / total_extra_weights

        return twa, [p for _, p in recents]

    def process(
        self,
        payload: DownlinkedPayload
    ) -> Tuple[_MF_CALC_T | None, List[DownlinkedPayload]]:
        """Processes the given the given payload and returns a value if this
        value triggers a value to be output from this MetaField
        (or `None` otherwise).

        Returns a tuple of:
            - the calculated value (if any)
            - all the payloads used to derive this value (if any)
        """
        # Check what we got:
        name: str
        if isinstance(payload, TelemetryPayload):
            name = payload.module.name + "_" + payload.channel.name
        elif isinstance(payload, EventPayload):
            name = payload.module.name + "_" + payload.event.name
        else:
            # Not something we can handle. Log it. Ignore it.
            logger.warning(
                "Non-telem/event payload passed to `MetaField.process`. "
                "The fact that this is happening is probably a smell but not "
                f"an issue. Payload will be ignored. `{payload=}`"
            )
            return None, []

        # Absorb the data if it's something we're watching:
        if name in self._WATCHING:
            self._deques[name].appendleft(payload)
            # Acknowledge that we've received this:
            self._received_since_last_calc.add(name)

        # Check if we're even able to process the data (we've received at
        # least 1 value for each watched field):
        able = all(len(d) >= self._N_MIN_VALS for d in self._deques.values())
        if not able:
            return None, []

        # Check if we're ready to calculate and emit a result:
        b = self._UPDATE_BEHAVIOR
        n_recv = len(self._received_since_last_calc)
        ready = (
            b == MetaChannelUpdateBehavior.ANY and n_recv > 0
            or b == MetaChannelUpdateBehavior.ALL and n_recv == len(self._WATCHING)
        )
        if ready and self._MIN_INTERVAL is not None:
            ready &= (datetime.now(timezone.utc) -
                      self._time_of_last_calc) > self._MIN_INTERVAL
        if not ready:
            return None, []

        # If we're here, we're ready to calculate:
        try:
            val, factors = self._calculate()
        except Exception as e:
            trace = traceback.format_exc()
            logger.error(
                f"Failed to calculate MetaField: {self} (name={self.name}) "
                f"b/c `{e}`. Traceback: `{trace}`.\n"
            )
            val, factors = None, []
            # Still proceed and reset timers so we don't get stuck here.

        # Reset counters and timers:
        self._received_since_last_calc = set()
        self._time_of_last_calc = datetime.now(timezone.utc)

        return val, factors

    @ abstractmethod
    def _calculate(self) -> Tuple[_MF_CALC_T | None, List[DownlinkedPayload]]:
        """Implementation of the actual calculation this field performs.
        Only called once enough data has been received for all fields and all
        update conditions have been met.

        Has full access to the internal state of this field processor.

        Can access the latest values for watched fields using `_get`.

        For telem, this calculates the data value.
        For events, this calculates a dictionary of all the necessary arg values.

        Returns a tuple of:
            - the calculated value or `None` if no value should be emitted.
            - all the payloads used to derive this value (if any)
        """
        raise NotImplementedError()


# Convenient Shorthands:
MetaChannel: TypeAlias = MetaField[TelemetryChannel, Any]
MetaEvent: TypeAlias = MetaField[Event, Dict[str, Any]]


@ dataclass
class MetaModule:
    """Container for meta-channels and meta-events.

    Automatically builds a module for the DataStandards based on the given
    MetaChannel Definitions.
    """
    # MetaModule Name:
    name: str
    # Module ID:
    # Manually assign so it's fixed and tracked (instead of being influenced by
    # out-of-file data)
    ID: int
    # Channels in this Module:
    meta_channels: List[MetaChannel]
    # Events in this Module:
    meta_events: List[MetaEvent]

    # Datastandards module represented here:
    _module: Module = field(init=False)

    # Prefix applied to all MetaModules:
    UNIVERSAL_PREFIX: Final[str] = "MetaMod"

    # Maximum number of fields (max # events and max # channels).
    # Based on op-code system inherited from FPrime:
    MAX_NUM_FIELDS: Final[int] = 0xFF

    @ property
    def module(self) -> Module: return self._module

    def __post_init__(self) -> None:
        # Make sure there aren't too many entries in telemetry or events lists:
        if len(self.meta_channels) > self.MAX_NUM_FIELDS:
            raise StandardsFormattingException(
                __file__,
                f"`{len(self.meta_channels)=}` is greater than the max number "
                f"of telemetry channels allowed (`{self.MAX_NUM_FIELDS=}`) "
                f"in a module. Can't build MetaModule."
            )
        if len(self.meta_events) > self.MAX_NUM_FIELDS:
            raise StandardsFormattingException(
                __file__,
                f"`{len(self.meta_events)=}` is greater than the max number "
                f"of event definitions allowed (`{self.MAX_NUM_FIELDS=}`). "
                f"in a module. Can't build MetaModule."
            )

        # Build the Module based on the parameters:
        self._module = Module(
            name=self.UNIVERSAL_PREFIX + self.name,
            ID=self.ID & 0xFF00,
            commands=NameIdDict(),
            events=NameIdDict({
                (i, me.name): me.build_ds_definition(id=i)
                for i, me in enumerate(self.meta_events)
            }),
            telemetry=NameIdDict({
                (i, mc.name): mc.build_ds_definition(id=i)
                for i, mc in enumerate(self.meta_channels)
            })
        )

    @ staticmethod
    def _determine_downlink_times(factors: List[DownlinkedPayload]) -> DownlinkTimes:
        """Determines the appropriate DownlinkTimes for the payload of a
        MetaField derived from (computed using) `factors`.

        Pull effective DownlinkTimes from whichever factor has the latest
        SCET (as if this had been generated on the s/c at the same time):
        If none of them have SCET, fall back to the next in the pipe:
        lander_rx, then amcc_rx, then pmcc_rx.
        """
        counts = OrderedDict([
            ('scet_est', 0),
            ('lander_rx', 0),
            ('amcc_rx', 0),
            ('pmcc_rx', 0)
        ])
        for f in factors:
            times_dict = attrs.asdict(f.downlink_times)
            for k in counts.keys():
                if times_dict[k] is not None:
                    counts[k] += 1
        # Take the first (earliest-in-pipe) key that has >1 entry:
        valid_time_keys = [k for k, v in counts.items() if v > 0]
        if len(valid_time_keys) == 0:
            # Nothing we can do here. Just say it was received by PMCC now and
            # call it:
            now = datetime.now(timezone.utc)
            return DownlinkTimes(
                scet_est=now,
                pmcc_rx=now
            )
        time_key = valid_time_keys[0]
        # Judge the newest factor based on our time key:
        most_recent_factor: DownlinkedPayload = factors[0]
        most_recent_time = datetime(1, 1, 1, tzinfo=timezone.utc)
        for f in factors:
            t = attrs.asdict(f.downlink_times)[time_key]
            if t is not None and t > most_recent_time:
                most_recent_time = t
                most_recent_factor = f
        # Set all times based on those of the `most_recent_factor` (as if
        # this had been emitted at the same time):
        return attrs.evolve(
            most_recent_factor.downlink_times or DownlinkTimes()
        )

    def process(self, in_payload: DownlinkedPayload) -> List[DownlinkedPayload]:
        """Processes the given the given payload and returns any derived
        payloads generated by receiving this payload.

        NOTE: This assumes the input `payload` has been timestamped with SCET
        already.
        """
        meta_fields: List[MetaField] = [*self.meta_channels, *self.meta_events]
        payloads: List[DownlinkedPayload] = []
        for field in meta_fields:
            # See if anything pops out of this field:
            result, factors = field.process(in_payload)

            # Move on if we didn't get anything:
            if result is None:
                continue

            # We got something, build the payload:
            payload: DownlinkedPayload
            is_telem = isinstance(field._PROTO, TelemetryChannel)
            if is_telem:
                payload = TelemetryPayload(
                    module_id=self.ID,
                    channel_id=field._PROTO.ID,
                    timestamp=0,  # doesn't apply here
                    data=result
                )
            else:
                payload = EventPayload(
                    module_id=self.ID,
                    event_id=field._PROTO.ID,
                    timestamp=0,  # doesn't apply here
                    args=result
                )
            # Add metadata:
            payload.pathway = DataPathway.NONE  # doesn't apply here
            payload.source = DataSource.GENERATED
            payload.downlink_times = self._determine_downlink_times(factors)
            payloads.append(payload)

        return payloads


def process_payloads_for_meta_modules(
    modules: List[MetaModule],
    payloads: List[DownlinkedPayload],
    MAX_DEPTH: int = 10
) -> List[DownlinkedPayload]:
    """
    Produces all meta-payloads that can be produced given the input payloads.

    That is, this runs all the given payloads over all the modules and collects
    any generated payloads.

    NOTE: Since some metachannels may depend on other metachannels, this is
    done cyclically until no new payloads are produced or `MAX_DEPTH` is
    reached.
    """
    generated_payloads: List[DownlinkedPayload] = []

    if MAX_DEPTH == 0:
        logger.error(
            "Maximum recursion depth reached when processing meta-payloads "
            f"for {payloads} (final step). "
            f"Perhaps this depth is too low or perhaps there's a cyclic "
            f"dependency."
        )
    else:
        # Grab only telem and events:
        telem_n_events = [
            p for p in payloads
            if isinstance(p, TelemetryPayload) or isinstance(p, EventPayload)
        ]

        for payload in telem_n_events:
            for module in modules:
                generated_payloads.extend(module.process(payload))

    if len(generated_payloads) == 0:
        # Nothing more can be derived:
        return generated_payloads
    else:
        # Attempt to derive more based on the new data just generated:
        return (
            generated_payloads +
            process_payloads_for_meta_modules(
                modules, generated_payloads, MAX_DEPTH-1)
        )


def add_metamodules_to_standards(
    standards: DataStandards,
    meta_modules: MetaModule | List[MetaModule]
) -> None:
    """
    Adds the given MetaModules to the given data standards so long as it
    doesn't override anything.
    """
    # Support being given just a single module to add:
    if (
        not isinstance(meta_modules, list)
        and isinstance(meta_modules, MetaModule)
    ):
        meta_modules = [meta_modules]
    # Create modules:
    modules = [mm.module for mm in meta_modules]
    # Add all modules to standards:
    standards.add_new_modules(modules)


# TODO: Also, add data rate (inst, 1 min, 1hr avg) telem to GDS dl_processor
