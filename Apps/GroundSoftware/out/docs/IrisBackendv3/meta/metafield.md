Module IrisBackendv3.meta.metafield
===================================
Creates system for defining metachannels and events, i.e. telemetry channels
and events that are calculated on the Ground based on the values of other
telemetry and events.

Essentially, this provides MetaModules, MetaChannels, and MetaEvents that wrap
standard Modules, Channels, and Events and mirror these standard non-"Meta"
counterparts in behavior, except that they aren't downlinked, they're computed
based on other fields and those computation functions are included in the
"Meta" definitions.

@author: Connor W. Colombo (CMU)
@last-updated: 10/02/2024

Functions
---------

    
`add_metamodules_to_standards(standards: IrisBackendv3.data_standards.data_standards.DataStandards, meta_modules: Union[IrisBackendv3.meta.metafield.MetaModule, List[IrisBackendv3.meta.metafield.MetaModule]]) ‑> None`
:   Adds the given MetaModules to the given data standards so long as it
    doesn't override anything.

    
`process_payloads_for_meta_modules(modules: List[IrisBackendv3.meta.metafield.MetaModule], payloads: List[IrisBackendv3.codec.payload.DownlinkedPayload], MAX_DEPTH: int = 10) ‑> List[IrisBackendv3.codec.payload.DownlinkedPayload]`
:   Produces all meta-payloads that can be produced given the input payloads.
    
    That is, this runs all the given payloads over all the modules and collects
    any generated payloads.
    
    NOTE: Since some metachannels may depend on other metachannels, this is
    done cyclically until no new payloads are produced or `MAX_DEPTH` is
    reached.

Classes
-------

`MetaChannelUpdateBehavior(value, names=None, *, module=None, qualname=None, type=None, start=1)`
:   Enum defining when a metachannel should recompute it's value.

    ### Ancestors (in MRO)

    * enum.Enum

    ### Class variables

    `ALL`
    :

    `ANY`
    :

    `TIME_ONLY`
    :

`MetaField()`
:   Definition for a meta-channel or meta-event, alongside the processing
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

    ### Ancestors (in MRO)

    * typing.Generic
    * abc.ABC

    ### Descendants

    * config.metafields.imu.AccMS2Builder.<locals>.AccMS2
    * config.metafields.imu.AccMS2Builder.<locals>.AccMS2
    * config.metafields.imu.AccMS2Builder.<locals>.AccMS2
    * config.metafields.imu.GravityMagnitudeMS2
    * config.metafields.imu.PitchAngleDeg
    * config.metafields.imu.RollAngleDeg
    * config.metafields.latency._DownlinkTimesLatencySec
    * config.metafields.peregrine.DeckD2TempKelvin
    * config.metafields.peregrine.OperationalCorrectedCurrentAvg_mA
    * config.metafields.peregrine.OperationalCorrectedCurrentMax_mA
    * config.metafields.peregrine.OperationalPowerAvg_W
    * config.metafields.peregrine.OperationalPowerMax_W
    * config.metafields.peregrine.ReleaseCurrentAvg_mA
    * config.metafields.peregrine.ReleaseCurrentMax_mA
    * config.metafields.peregrine.ReleasePowerAvg_W
    * config.metafields.peregrine.ReleasePowerMax_W
    * config.metafields.rover_power.BatteryParameter.<locals>.BatteryParam
    * config.metafields.rover_power.BatteryParameter.<locals>.BatteryParam
    * config.metafields.rover_power.BatteryParameter.<locals>.BatteryParam
    * config.metafields.rover_power.BatteryParameter.<locals>.BatteryParam
    * config.metafields.rover_power.BatteryParameter.<locals>.BatteryParam
    * config.metafields.rover_power.CurrentReading1V2F
    * config.metafields.rover_power.CurrentReading1V2H
    * config.metafields.rover_power.CurrentReading3V3
    * config.metafields.rover_power.CurrentReading3V3F
    * config.metafields.rover_power.CurrentReading3V3H
    * config.metafields.rover_power.CurrentReading3V3R
    * config.metafields.rover_power.CurrentReadingMotors
    * config.metafields.rover_power.FullSystemCurrent_mA
    * config.metafields.rover_power.FullSystemSwitch_FaultState
    * config.metafields.rover_power.LanderVoltage_FusedEst
    * config.metafields.temps.BatteryTempAvgKelvin
    * config.metafields.temps.BoardThermBuilder.<locals>.BoardTherm
    * config.metafields.temps.BoardThermBuilder.<locals>.BoardTherm
    * config.metafields.temps.BoardThermBuilder.<locals>.BoardTherm
    * config.metafields.temps.BoardThermBuilder.<locals>.BoardTherm
    * config.metafields.temps.BoardThermBuilder.<locals>.BoardTherm
    * config.metafields.temps.BoardThermBuilder.<locals>.BoardTherm
    * config.metafields.temps.RoverThermBuilder.<locals>.BoardTherm
    * config.metafields.temps.RoverThermBuilder.<locals>.BoardTherm
    * config.metafields.temps.RoverThermBuilder.<locals>.BoardTherm
    * config.metafields.temps.RoverThermBuilder.<locals>.BoardTherm
    * config.metafields.temps.RoverThermBuilder.<locals>.BoardTherm
    * config.metafields.temps.RoverThermBuilder.<locals>.BoardTherm
    * config.metafields.temps.RoverThermBuilder.<locals>.BoardTherm
    * config.metafields.temps.RoverThermBuilder.<locals>.BoardTherm
    * config.metafields.temps.RoverThermBuilder.<locals>.BoardTherm
    * config.metafields.temps.RoverThermBuilder.<locals>.BoardTherm
    * config.metafields.transit_power.VHeaterLive
    * config.metafields.watchdog_monitor.CDS_Builder.<locals>.CombinedDigitalState
    * config.metafields.watchdog_monitor.CDS_Builder.<locals>.CombinedDigitalState
    * config.metafields.watchdog_monitor.CDS_Builder.<locals>.CombinedDigitalState
    * config.metafields.watchdog_monitor.CDS_Builder.<locals>.CombinedDigitalState
    * config.metafields.watchdog_monitor.CDS_Builder.<locals>.CombinedDigitalState
    * config.metafields.watchdog_monitor.CDS_Builder.<locals>.CombinedDigitalState
    * config.metafields.watchdog_monitor.CDS_Builder.<locals>.CombinedDigitalState
    * config.metafields.watchdog_monitor.CDS_Builder.<locals>.CombinedDigitalState
    * config.metafields.watchdog_monitor.CDS_Builder.<locals>.CombinedDigitalState
    * config.metafields.watchdog_monitor.CDS_Builder.<locals>.CombinedDigitalState
    * config.metafields.watchdog_monitor.CDS_Builder.<locals>.CombinedDigitalState
    * config.metafields.watchdog_monitor.CDS_Builder.<locals>.CombinedDigitalState
    * config.metafields.watchdog_monitor.CDS_Builder.<locals>.CombinedDigitalState
    * config.metafields.watchdog_monitor.CDS_Builder.<locals>.CombinedDigitalState
    * config.metafields.watchdog_monitor.CDS_Builder.<locals>.CombinedDigitalState
    * config.metafields.watchdog_monitor.CDS_Builder.<locals>.CombinedDigitalState
    * config.metafields.watchdog_monitor.CDS_Builder.<locals>.CombinedDigitalState
    * config.metafields.watchdog_monitor.CDS_Builder.<locals>.CombinedDigitalState
    * config.metafields.watchdog_monitor.CDS_Builder.<locals>.CombinedDigitalState
    * config.metafields.watchdog_monitor.CDS_Builder.<locals>.CombinedDigitalState
    * config.metafields.watchdog_monitor.HerculesSkippedStrokes
    * config.metafields.watchdog_monitor.HerculesWatchdogRoundtripTime_ms
    * config.metafields.watchdog_monitor.WatchdogHerculesRoundtripTime_ms
    * config.metafields.watchdog_monitor.WatchdogMissedStrokes

    ### Static methods

    `build_ds_definition(id: int) ‑> ~_MF_DEF_T`
    :   Builds the definition that should go in the DataStandards for this
        module.

    ### Instance variables

    `name: str`
    :

    ### Methods

    `process(self, payload: IrisBackendv3.codec.payload.DownlinkedPayload) ‑> Tuple[Optional[~_MF_CALC_T], List[IrisBackendv3.codec.payload.DownlinkedPayload]]`
    :   Processes the given the given payload and returns a value if this
        value triggers a value to be output from this MetaField
        (or `None` otherwise).
        
        Returns a tuple of:
            - the calculated value (if any)
            - all the payloads used to derive this value (if any)

    `reset(self) ‑> None`
    :   Full state reset.

`MetaModule(name: str, ID: int, meta_channels: List[IrisBackendv3.meta.metafield.MetaField[IrisBackendv3.data_standards.module.TelemetryChannel, Any]], meta_events: List[IrisBackendv3.meta.metafield.MetaField[IrisBackendv3.data_standards.module.Event, Dict[str, Any]]], UNIVERSAL_PREFIX: Final[str] = 'MetaMod', MAX_NUM_FIELDS: Final[int] = 255)`
:   Container for meta-channels and meta-events.
    
    Automatically builds a module for the DataStandards based on the given
    MetaChannel Definitions.

    ### Class variables

    `ID: int`
    :

    `MAX_NUM_FIELDS: Final[int]`
    :

    `UNIVERSAL_PREFIX: Final[str]`
    :

    `meta_channels: List[IrisBackendv3.meta.metafield.MetaField[IrisBackendv3.data_standards.module.TelemetryChannel, Any]]`
    :

    `meta_events: List[IrisBackendv3.meta.metafield.MetaField[IrisBackendv3.data_standards.module.Event, Dict[str, Any]]]`
    :

    `name: str`
    :

    ### Instance variables

    `module: IrisBackendv3.data_standards.module.Module`
    :

    ### Methods

    `process(self, in_payload: IrisBackendv3.codec.payload.DownlinkedPayload) ‑> List[IrisBackendv3.codec.payload.DownlinkedPayload]`
    :   Processes the given the given payload and returns any derived
        payloads generated by receiving this payload.
        
        NOTE: This assumes the input `payload` has been timestamped with SCET
        already.