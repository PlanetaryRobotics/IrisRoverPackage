Module IrisBackendv3.codec.packet_classes.watchdog_detailed_status
==================================================================
Implementation of Watchdog Detailed Status Packet and its Interface.
This is a maximally detailed status packet that contains status info for every
sensor and interface the Watchdog has access to. Occasionally used as a
detailed replacement for Heartbeat.

@author: Connor W. Colombo (CMU)
@last-updated: 12/31/2022

Classes
-------

`WatchdogDetailedStatusPacket(custom_payload: Optional[CPCT] = None, payloads: Optional[EnhancedPayloadCollection] = None, raw: Optional[bytes] = None, endianness_code: str = '<')`
:   Superclass for a special **non**-Iris Common Packet packet
    (e.g. generated by the Watchdog) which contains items which would normally
    be in separate telemetry channels but instead are contained in one
    continuous `CustomPayload`. This class maps the extracts the values in the
    `CustomPayload` and stores them in traditional `TelemetryPayload`s linked to
    a custom `TelemetryChannel`.
    All of these telemetry channels must be found
    in a `DataStandards` module, typically a `prebuilt` one.
    
    **The subclass must specify a `PREBUILT_MODULE_NAME` which links the subclass
    to the `DataStandards` module it represents.**
    
    Members of the custom payload must have same names as the corresponding
    telemetry channels in the prebuilt `DataStandards` module.
    Note: an error will get thrown by `CustomPayloadPacket.__init__`
    when building `payloads` from the `custom_payload` if all the channels
    in the `PREBUILT_MODULE_NAME prebuilt module don't have a corresponding
    attr in this `CustomPayload`.
    
    *NOTE:* For this to work effectively:
    1. All fields, including computed properties, in the custom payload must
        match the names of the corresponding telemetry channels from the
        prebuilt module.
    2. Although the custom payload class can have fields that don't appear as
        telemetry channels in the linked datastandards module, all telemetry
        channels in the module must have matching fields in the custom payload
        class.
    3. All args to `__init__` of the custom payload must have corresponding
        channels in the linked datastandards module.
    4. The order of the args in `__init__` of the custom payload must
        match the order of the bytes/bits in the packet.
    
    Constructs an instance of this `CustomPayloadPacket`. If a
    `custom_payload` (`CPCT`) object is given (e.g. when decoding), it will
    be unpacked into a `EnhancedPayloadCollection`. If a `payloads`
    `EnhancedPayloadCollection` is given, it will just be loaded.
    NOTE: `custom_payload` XOR `payloads` must be given. Not neither, not
    both.

    ### Ancestors (in MRO)

    * IrisBackendv3.codec.packet_classes.watchdog_detailed_status.WatchdogDetailedStatusPacketInterface
    * IrisBackendv3.codec.packet_classes.custom_payload.CustomPayloadPacket
    * IrisBackendv3.codec.packet_classes.packet.Packet
    * IrisBackendv3.codec.container.ContainerCodec
    * typing.Generic
    * abc.ABC

    ### Class variables

    `BITFIELD_ALLOCATIONS: collections.OrderedDict`
    :

    `CUSTOM_PAYLOAD_CLASS: Optional[Type[IrisBackendv3.codec.packet_classes.watchdog_detailed_status.WatchdogDetailedStatusPacketInterface.CustomPayload]]`
    :   Core custom WatchdogDetailedStatus payload.
        Members must have same names as corresponding telemetry channels in the
        `WatchdogDetailedStatus` prebuilt module.
        Note: an error will get thrown by `WatchdogDetailedStatusPacket.__init__`
        when building `payloads` from the `custom_payload` if all the channels
        in the `WatchdogDetailedStatus` prebuilt module don't have a corresponding
        attr in this `CustomPayload`.
        
        *NOTE:* For this to work effectively, all fields, including computed
        properties, must match their names from the prebuilt module
        AND the order of the args in `__init__` must match the order of the
        bytes in the packet. [SEE THE NOTE IN `CustomPayloadPacket` FOR MORE
        DETAILS]

    `START_FLAG: bytes`
    :

    `WD_ADC_BITS: int`
    :

`WatchdogDetailedStatusPacketInterface(custom_payload: Optional[CPCT] = None, payloads: Optional[EnhancedPayloadCollection] = None, raw: Optional[bytes] = None, endianness_code: str = '<')`
:   Superclass for a special **non**-Iris Common Packet packet
    (e.g. generated by the Watchdog) which contains items which would normally
    be in separate telemetry channels but instead are contained in one
    continuous `CustomPayload`. This class maps the extracts the values in the
    `CustomPayload` and stores them in traditional `TelemetryPayload`s linked to
    a custom `TelemetryChannel`.
    All of these telemetry channels must be found
    in a `DataStandards` module, typically a `prebuilt` one.
    
    **The subclass must specify a `PREBUILT_MODULE_NAME` which links the subclass
    to the `DataStandards` module it represents.**
    
    Members of the custom payload must have same names as the corresponding
    telemetry channels in the prebuilt `DataStandards` module.
    Note: an error will get thrown by `CustomPayloadPacket.__init__`
    when building `payloads` from the `custom_payload` if all the channels
    in the `PREBUILT_MODULE_NAME prebuilt module don't have a corresponding
    attr in this `CustomPayload`.
    
    *NOTE:* For this to work effectively:
    1. All fields, including computed properties, in the custom payload must
        match the names of the corresponding telemetry channels from the
        prebuilt module.
    2. Although the custom payload class can have fields that don't appear as
        telemetry channels in the linked datastandards module, all telemetry
        channels in the module must have matching fields in the custom payload
        class.
    3. All args to `__init__` of the custom payload must have corresponding
        channels in the linked datastandards module.
    4. The order of the args in `__init__` of the custom payload must
        match the order of the bytes/bits in the packet.
    
    Constructs an instance of this `CustomPayloadPacket`. If a
    `custom_payload` (`CPCT`) object is given (e.g. when decoding), it will
    be unpacked into a `EnhancedPayloadCollection`. If a `payloads`
    `EnhancedPayloadCollection` is given, it will just be loaded.
    NOTE: `custom_payload` XOR `payloads` must be given. Not neither, not
    both.

    ### Ancestors (in MRO)

    * IrisBackendv3.codec.packet_classes.custom_payload.CustomPayloadPacket
    * IrisBackendv3.codec.packet_classes.packet.Packet
    * IrisBackendv3.codec.container.ContainerCodec
    * typing.Generic
    * abc.ABC

    ### Descendants

    * IrisBackendv3.codec.packet_classes.watchdog_detailed_status.WatchdogDetailedStatusPacket

    ### Class variables

    `CustomPayload`
    :   Core custom WatchdogDetailedStatus payload.
        Members must have same names as corresponding telemetry channels in the
        `WatchdogDetailedStatus` prebuilt module.
        Note: an error will get thrown by `WatchdogDetailedStatusPacket.__init__`
        when building `payloads` from the `custom_payload` if all the channels
        in the `WatchdogDetailedStatus` prebuilt module don't have a corresponding
        attr in this `CustomPayload`.
        
        *NOTE:* For this to work effectively, all fields, including computed
        properties, must match their names from the prebuilt module
        AND the order of the args in `__init__` must match the order of the
        bytes in the packet. [SEE THE NOTE IN `CustomPayloadPacket` FOR MORE
        DETAILS]

    `PREBUILT_MODULE_NAME: ClassVar[str]`
    :

`WDS_PI(custom_payload: Optional[CPCT] = None, payloads: Optional[EnhancedPayloadCollection] = None, raw: Optional[bytes] = None, endianness_code: str = '<')`
:   Superclass for a special **non**-Iris Common Packet packet
    (e.g. generated by the Watchdog) which contains items which would normally
    be in separate telemetry channels but instead are contained in one
    continuous `CustomPayload`. This class maps the extracts the values in the
    `CustomPayload` and stores them in traditional `TelemetryPayload`s linked to
    a custom `TelemetryChannel`.
    All of these telemetry channels must be found
    in a `DataStandards` module, typically a `prebuilt` one.
    
    **The subclass must specify a `PREBUILT_MODULE_NAME` which links the subclass
    to the `DataStandards` module it represents.**
    
    Members of the custom payload must have same names as the corresponding
    telemetry channels in the prebuilt `DataStandards` module.
    Note: an error will get thrown by `CustomPayloadPacket.__init__`
    when building `payloads` from the `custom_payload` if all the channels
    in the `PREBUILT_MODULE_NAME prebuilt module don't have a corresponding
    attr in this `CustomPayload`.
    
    *NOTE:* For this to work effectively:
    1. All fields, including computed properties, in the custom payload must
        match the names of the corresponding telemetry channels from the
        prebuilt module.
    2. Although the custom payload class can have fields that don't appear as
        telemetry channels in the linked datastandards module, all telemetry
        channels in the module must have matching fields in the custom payload
        class.
    3. All args to `__init__` of the custom payload must have corresponding
        channels in the linked datastandards module.
    4. The order of the args in `__init__` of the custom payload must
        match the order of the bytes/bits in the packet.
    
    Constructs an instance of this `CustomPayloadPacket`. If a
    `custom_payload` (`CPCT`) object is given (e.g. when decoding), it will
    be unpacked into a `EnhancedPayloadCollection`. If a `payloads`
    `EnhancedPayloadCollection` is given, it will just be loaded.
    NOTE: `custom_payload` XOR `payloads` must be given. Not neither, not
    both.

    ### Ancestors (in MRO)

    * IrisBackendv3.codec.packet_classes.custom_payload.CustomPayloadPacket
    * IrisBackendv3.codec.packet_classes.packet.Packet
    * IrisBackendv3.codec.container.ContainerCodec
    * typing.Generic
    * abc.ABC

    ### Descendants

    * IrisBackendv3.codec.packet_classes.watchdog_detailed_status.WatchdogDetailedStatusPacket

    ### Class variables

    `CustomPayload`
    :   Core custom WatchdogDetailedStatus payload.
        Members must have same names as corresponding telemetry channels in the
        `WatchdogDetailedStatus` prebuilt module.
        Note: an error will get thrown by `WatchdogDetailedStatusPacket.__init__`
        when building `payloads` from the `custom_payload` if all the channels
        in the `WatchdogDetailedStatus` prebuilt module don't have a corresponding
        attr in this `CustomPayload`.
        
        *NOTE:* For this to work effectively, all fields, including computed
        properties, must match their names from the prebuilt module
        AND the order of the args in `__init__` must match the order of the
        bytes in the packet. [SEE THE NOTE IN `CustomPayloadPacket` FOR MORE
        DETAILS]

    `PREBUILT_MODULE_NAME: ClassVar[str]`
    :

`WDS_CP(Io_ChargingStatus1: int, Io_ChargingStatus2: int, Io_BatteryConnectionStatus: Union[int, str], Io_BatteryLatchStatus: Union[int, str], Io_1V2PowerGood: Union[int, str], Io_1V8PowerGood: Union[int, str], Io_3V3PowerGood: Union[int, str], Io_5V0PowerGood: Union[int, str], Watchdog_State: Union[int, str], Watchdog_DeploymentStatus: Union[int, str], Watchdog_Uart0State: Union[int, str], Watchdog_Uart1State: Union[int, str], Adc_BatteryTempRaw: int, Watchdog_DetailedHeartbeatSequenceNumber: int, Watchdog_DigitalOutputStates: int, Watchdog_ResetLogs: int, Adc_LanderVoltageRaw: int, Adc_BatteryChargingTempRaw: int, Adc_FullSystemVoltageRaw: int, Adc_FullSystemCurrentRaw: int, Adc_SwitchedBatteryVoltageRaw: int, Adc_Vcc24VoltageRaw: int, Heater_ControlEnabled: Union[int, str], Heater_IsHeating: Union[int, str], Adc_2V5VoltageRaw: int, Adc_2V8VoltageRaw: int, Adc_Vcc28VoltageRaw: int, Heater_Kp: int, Heater_PwmLimit_DutyCycleCounter: int, Heater_SetpointValue: int, Heater_OnValue: int, Heater_OffValue: int, Heater_DutyCyclePeriodCycles: int, Heater_DutyCycleCounter: int, I2C_BatteryChargeRaw: int, I2C_BatteryVoltageRaw: int, I2C_BatteryCurrentRaw: int, I2C_FuelGaugeTempRaw: int, I2C_BatteryChargeTelemRaw: int, I2C_BatteryCurrentTelemRaw: int)`
:   Core custom WatchdogDetailedStatus payload.
    Members must have same names as corresponding telemetry channels in the
    `WatchdogDetailedStatus` prebuilt module.
    Note: an error will get thrown by `WatchdogDetailedStatusPacket.__init__`
    when building `payloads` from the `custom_payload` if all the channels
    in the `WatchdogDetailedStatus` prebuilt module don't have a corresponding
    attr in this `CustomPayload`.
    
    *NOTE:* For this to work effectively, all fields, including computed
    properties, must match their names from the prebuilt module
    AND the order of the args in `__init__` must match the order of the
    bytes in the packet. [SEE THE NOTE IN `CustomPayloadPacket` FOR MORE
    DETAILS]

    ### Class variables

    `BATT_5K_THERMISTOR_LOOKUP_TABLE`
    :

    `BATT_CHRG_10K_THERMISTOR_LOOKUP_TABLE`
    :

    `R_HEATER`
    :

    `VBS_ADC_FM1_CORRECTION_FACTOR`
    :

    `VCC28_ADC_FM1_CORRECTION_FACTOR`
    :

    `VL_ADC_FM1_CORRECTION_FACTOR`
    :

    `VSA_ADC_FM1_CORRECTION_FACTOR`
    :

    `V_HEATER_NOM`
    :

    `V_LANDER_MAX`
    :

    ### Static methods

    `battery_adc_reading_to_kelvin(adc_reading) ‑> float`
    :

    ### Instance variables

    `Adc_2V5Voltage: float`
    :

    `Adc_2V5VoltageRaw: int`
    :

    `Adc_2V8Voltage: float`
    :

    `Adc_2V8VoltageRaw: int`
    :

    `Adc_BatteryChargingTempKelvin: float`
    :

    `Adc_BatteryChargingTempRaw: int`
    :

    `Adc_BatteryChargingTempUncertaintyKelvin: float`
    :

    `Adc_BatteryTempKelvin: float`
    :

    `Adc_BatteryTempRaw: int`
    :

    `Adc_BatteryTempUncertaintyKelvin: float`
    :

    `Adc_DataFrame: pandas.core.frame.DataFrame`
    :   Creates a Pandas DataFrame that summarizes all ADC readings incl. 
        both their raw ADC readings and their conversions to human-readable 
        units.

    `Adc_FullSystemCurrent: float`
    :

    `Adc_FullSystemCurrentRaw: int`
    :

    `Adc_FullSystemVoltage: float`
    :

    `Adc_FullSystemVoltageRaw: int`
    :

    `Adc_LanderVoltage: float`
    :

    `Adc_LanderVoltageRaw: int`
    :

    `Adc_SwitchedBatteryVoltage: float`
    :

    `Adc_SwitchedBatteryVoltageRaw: int`
    :

    `Adc_Vcc24Voltage: float`
    :

    `Adc_Vcc24VoltageRaw: int`
    :

    `Adc_Vcc28Voltage: float`
    :

    `Adc_Vcc28VoltageRaw: int`
    :

    `Heater_ControlEnabled: int`
    :

    `Heater_DutyCycleCounter: int`
    :

    `Heater_DutyCyclePercent: float`
    :

    `Heater_DutyCyclePeriodCycles: int`
    :

    `Heater_DutyCyclePeriodMs: float`
    :

    `Heater_EffectivePower: float`
    :

    `Heater_EffectivePowerLimit: float`
    :

    `Heater_EffectiveVoltage: float`
    :

    `Heater_IsHeating: int`
    :

    `Heater_Kp: int`
    :

    `Heater_MaxPossiblePower: float`
    :

    `Heater_OffTempKelvin: float`
    :

    `Heater_OffValue: int`
    :

    `Heater_OnTempKelvin: float`
    :

    `Heater_OnValue: int`
    :

    `Heater_PwmLimit_DutyCycleCounter: int`
    :

    `Heater_PwmLimit_DutyCyclePercent: float`
    :

    `Heater_SetpointKelvin: float`
    :

    `Heater_SetpointValue: int`
    :

    `I2C_BatteryChargeMah: float`
    :

    `I2C_BatteryChargeRaw: int`
    :

    `I2C_BatteryChargeTelemMah: float`
    :

    `I2C_BatteryChargeTelemRaw: int`
    :

    `I2C_BatteryCurrent: float`
    :

    `I2C_BatteryCurrentRaw: int`
    :

    `I2C_BatteryCurrentTelemAmps: float`
    :

    `I2C_BatteryCurrentTelemRaw: int`
    :

    `I2C_BatteryVoltage: float`
    :

    `I2C_BatteryVoltageRaw: int`
    :

    `I2C_FuelGaugeTempKelvin: float`
    :

    `I2C_FuelGaugeTempRaw: int`
    :

    `Io_1V2PowerGood: int`
    :

    `Io_1V8PowerGood: int`
    :

    `Io_3V3PowerGood: int`
    :

    `Io_5V0PowerGood: int`
    :

    `Io_BatteryConnectionStatus: int`
    :

    `Io_BatteryLatchStatus: int`
    :

    `Io_BatteryState: int`
    :

    `Io_ChargerState: int`
    :

    `Io_ChargingStatus1: int`
    :

    `Io_ChargingStatus2: int`
    :

    `Watchdog_CombinedDigitalStates: int`
    :

    `Watchdog_CombinedDigitalStates_DataFrame: pandas.core.frame.DataFrame`
    :   Returns a Pandas DataFrame containing all fields from 
        `Watchdog_CombinedDigitalStates_Dict` using shorthand aliases for 
        each pin from `Watchdog_CombinedDigitalStates_Shorthand` for the 
        column headers.

    `Watchdog_CombinedDigitalStates_Dict: collections.OrderedDict[str, int]`
    :   Creates a dict of states for each pin where:
            0 = output driven low
            1 = output driven high
            2 = input (Hi-Z)

    `Watchdog_CombinedDigitalStates_Shorthand: collections.OrderedDict[str, int]`
    :   Returns a dict containing all fields from `Watchdog_CombinedDigitalStates_Dict`
        but using shorthand aliases for each pin (for easy printing):

    `Watchdog_DeploymentStatus: int`
    :

    `Watchdog_DetailedHeartbeatSequenceNumber: int`
    :

    `Watchdog_DigitalOutputStates: int`
    :

    `Watchdog_DigitalOutputStates_Dict: collections.OrderedDict[str, int]`
    :

    `Watchdog_ResetEventsList: List[str]`
    :   Returns a list of the names of all ResetLogs that have a non-zero 
        value (i.e. the event happened).

    `Watchdog_ResetLogs: int`
    :

    `Watchdog_ResetLogs_Dict: collections.OrderedDict[str, int]`
    :

    `Watchdog_ResetLogs_Dict_DataFrame: pandas.core.frame.DataFrame`
    :   Returns a Pandas DataFrame containing all fields from 
        `Watchdog_ResetLogs_Dict_Dict` using shorthand aliases for 
        each pin from `Watchdog_ResetLogs_Dict_Shorthand` for the 
        column headers.

    `Watchdog_ResetLogs_Dict_Shorthand: collections.OrderedDict[str, int]`
    :   Returns a dict containing all fields from `Watchdog_ResetLogs_Dict`
        but using shorthand aliases for each pin (for easy printing):

    `Watchdog_State: int`
    :

    `Watchdog_Uart0State: int`
    :

    `Watchdog_Uart1State: int`
    :

    `fused_est_lander_voltage: float`
    :   Fuses two sensor readings weighted based on their relative 
        uncertainties to estimated the true lander voltage.

    `mean_lander_voltage: float`
    :

    ### Methods

    `getEnumName(self, field_name) ‑> str`
    :   Gets the name associated with the int value of the enum with the
        given `field_name`.
        Example: `getEnumName(Watchdog_State)` loads the int value of
        `Watchdog_State` and converts it a string as given by the enum for
        the `Watchdog_State` `TelemetryChannel` in the `Module` associated
        with this packet. Works for computed enum channels too (e.g.
        `Io_ChargerState`).