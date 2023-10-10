<title>WatchDogInterface Component Dictionary</title>
# WatchDogInterface Component Dictionary


## Command List

|Mnemonic|ID|Description|Arg Name|Arg Type|Comment
|---|---|---|---|---|---|
|Reset_Specific|0 (0x0)|Command to reset the specific parts of rover| | |
| | | |reset_value|reset_values_possible|
                    	ENUM Value that specifies which components or hardware need to be reset
                    |
|Prepare_For_Deployment|1 (0x1)|Command to send signal to MSP430 to prepare for deploying (may not be needed)| | |
| | | |confirm|confirm_prepare_for_deploy||
|Disengage_From_Lander|2 (0x2)|Command to send signal to MSP430 that it should send a signal to lander to disengage, sets disengage pin high| | |
| | | |confirm|confirm_disengage||
|Engage_From_Lander|3 (0x3)|Command to send signal to MSP430 that it should send a signal to lander to engage, sets disengage pin low. Impl. only exists inside Hercules.| | |
|Switch_Connection_Mode|4 (0x4)|Command to send signal to MSP430 that we switch the current connection mode. NOTE: This is currently deprecated behavior. Watchdog now sends data to all available and active interfaces in any given state.| | |
| | | |mode|watchdog_connection_mode||
|Set_Debug_Comms_State|170 (0xaa)|Turn Watchdog DEBUG comms messages ON or OFF (should default to ON).| | |
| | | |confirm| confirm_change_debug||
| | | |state|debug_comms_state||
|Set_Auto_Heater_On_Value|171 (0xab)|Set the ON threshold for the auto heater controller on the Watchdog.| | |
| | | |on|U16||
|Set_Auto_Heater_Off_Value|172 (0xac)|Set the OFF threshold for the auto heater controller on the Watchdog.| | |
| | | |off|U16||
|Set_Heater_Duty_Cycle|173 (0xad)|Set the PWM duty cycle of the auto heater controller on the Watchdog.| | |
| | | |duty|U16||
|Set_Heater_Duty_Cycle_Period|174 (0xae)|Set the PWM period of the auto heater controller on the Watchdog.| | |
| | | |period|U16||
|Set_VSAE_State|218 (0xda)|Tells the Watchdog to manually set the state for the V_SYS_ALL_ENABLE line on the BLiMP.| | |
| | | |confirm| confirm_vsae_change_enum||
| | | |state|vsae_state||
|Switch_to_Sleep_Mode|234 (0xea)|Command to send signal to MSP430 that it should go into Sleep Mode| | |
| | | |confirm|confirm_sleep_mode||
|Switch_to_Keep_Alive_Mode|235 (0xeb)|Command to send signal to MSP430 that it should go into Keep Alive Mode| | |
| | | |confirm|confirm_alive_mode||
|Switch_to_Service_Mode|236 (0xec)|Command to send signal to MSP430 that it should go into Service Mode| | |
| | | |confirm|confirm_service_mode||
|Clear_Reset_Memory|244 (0xf4)|Clear the reset memory used in the Watchdog's Detailed Status Report.| | |
| | | |confirm_1|confirm_clear_reset_memory_1||
| | | |confirm_2|confirm_clear_reset_memory_2||
|DANGEROUS_Force_Battery_State_DANGEROUS|245 (0xf5)|**DANGEROUS**: Tells the Watchdog to set the BLiMP's BSTAT pin (normally an input) to OUTPUT the given value. This is designed to be used as a last ditch effort to connect the batteries in case any of the components driving BSTAT die. If they aren't dead, this will have the effect of blowing up the BSTAT circuitry and maybe a port on the Watchdog if not the whole Watchdog. This is **ONLY** to be used if the Mission will be over if you don't. You've got to be really sure you want to do this.| | |
| | | |confirm_1|dangerous_confirm_force_bstat_enum_1||
| | | |confirm_2|dangerous_confirm_force_bstat_enum_2||
| | | |state|bstat_state||
|Request_Status_Report|246 (0xf6)|Request the Watchdog to send a Detailed Status Report.| | |
| | | |confirm|confirm_status_request||
|Set_Charger_Enable|247 (0xf7)|Manually set charging IC enable state: CE. (normally you should just use the start and stop charging commands in reset specific.)| | |
| | | |charge_en|charge_en_states||
|Set_Charger_Power_Connection|248 (0xf8)|Manually set charging power 28V regulator enable state: REGE. (normally you should just use the start and stop charging commands in reset specific.)| | |
| | | |v_lander_reg_en|bool||
|Set_Battery_Connection|249 (0xf9)|Manually set battery connection state: BE. (normally you should just use the batteries enable/disable command in reset specific.)| | |
| | | |batt_en|bool||
|Set_Battery_Control_Enable|250 (0xfa)|Manually set the state of the battery control circuitry: BCTRLE. On Iris FM1 this line (should be) disconnected so this *should effectively be a no-op. To be used if the engineers believe this connection may have reformed somehow.| | |
| | | |batt_ctrl_en|batt_ctrl_en_states||
|Set_Battery_Latch|251 (0xfb)|Manually set battery latch state: LB. (normally you should just use the batteries enable/disable command in reset specific.)| | |
| | | |latch_batt|latch_batt_states||
|Set_Latch_Set|252 (0xfc)|Control the battery latch "SET" override. This line *should* be severed on Iris FM1, so this *should* effectively be a no-op.| | |
| | | |latch_set|latch_set_states||
|Set_Latch_Reset|253 (0xfd)|Control the battery latch "RESET" override. This line *should* be severed on Iris FM1, so this *should* effectively be a no-op.| | |
| | | |latch_reset|latch_reset_states||
|Echo|255 (0xff)|Echo the given fixed length string (technically can send a string of any length up to the max length).| | |
| | | |length|U8||
| | | |message|Fw::CmdStringArg|Message for the Watchdog to Echo back to us.|

## Telemetry Channel List

|Channel Name|ID|Type|Description|
|---|---|---|---|
|VOLTAGE_28V|5 (0x5)|int16_t|WD ADC reading for Voltage from 28V line from Watchdog|
|BATTERY_THERMISTOR|16 (0x10)|U8|Battery Thermistor reading from Watchdog|
|THERM_0|27 (0x1b)|U16|12 bit ADC Reading for On-SBC Thermistor RT1.|
|THERM_1|28 (0x1c)|U16|12 bit ADC Reading for On-SBC Thermistor RT2.|
|THERM_2|29 (0x1d)|U16|12 bit ADC Reading for On-SBC Thermistor RT3.|
|THERM_3|30 (0x1e)|U16|12 bit ADC Reading for On-SBC Thermistor RT4.|
|THERM_4|31 (0x1f)|U16|12 bit ADC Reading for On-SBC Thermistor RT5.|
|THERM_5|32 (0x20)|U16|12 bit ADC Reading for On-SBC Thermistor RT6.|
|THERM_6|33 (0x21)|U16|12 bit ADC Reading for off-board temperature sensor 1 (TMR1).|
|THERM_7|34 (0x22)|U16|12 bit ADC Reading for off-board temperature sensor 2 (TMR2).|
|THERM_8|35 (0x23)|U16|12 bit ADC Reading for off-board temperature sensor 3 (TMR3).|
|THERM_9|36 (0x24)|U16|12 bit ADC Reading for off-board temperature sensor 4 (TMR4).|
|THERM_10|37 (0x25)|U16|12 bit ADC Reading for off-board temperature sensor 5 (TMR5).|
|THERM_11|38 (0x26)|U16|12 bit ADC Reading for off-board temperature sensor 6 (TMR6).|
|THERM_12|39 (0x27)|U16|12 bit ADC Reading for off-board temperature sensor 7 (TMR7).|
|THERM_13|40 (0x28)|U16|12 bit ADC Reading for off-board temperature sensor 8 (TMR8).|
|THERM_14|41 (0x29)|U16|12 bit ADC Reading for off-board temperature sensor 9 (TMR9).|
|THERM_15|42 (0x2a)|U16|12 bit ADC Reading for off-board temperature sensor 10 (TMR10).|

## Event List

|Event Name|ID|Description|Arg Name|Arg Type|Arg Size|Description
|---|---|---|---|---|---|---|
|WatchDogIncorrectResp|0 (0x0)|Warning that the WatchDog MSP430 sent back a response different than what was sent to it| | | | |
| | | |error|U8||The watchdog error value (reference to documentation)|
|WatchDogTimedOut|1 (0x1)|Warning that a WatchDog MSP430 watchdog timer went off| | | | |
|WatchDogCmdReceived|2 (0x2)|Notification that watchdog interface received a command from Cmd_Dispatcher| | | | |
| | | |Cmd|Fw::LogStringArg&|50|The cmd that watchdog interface processed|
|WatchDogCommError|3 (0x3)|Warning that a WatchDog MSP430 error has occured.| | | | |
| | | |error|U32||The watchdog error value (reference to documentation)|
|ADCThermistorError|4 (0x4)|Warning that less than 16 thermistors had conversions or conversion never completed meaning no new tempurature values are being generated.| | | | |
|WatchDogIncorrectResetValue|5 (0x5)|Warning that an incorrect reset value has been sent from ground| | | | |
|AdcCurrentSensorReadingsReport|6 (0x6)|Report containing all of the currents measured using the ADC. Delivered as an event since we ran out of space for new telem.
             GSW GDS will split this out into meta Telemetry Channels on Ground so all Ground assets receive this as telemetry. 
             GDS will also handle ADC->mA conversion.
             This is acceptable b/c these are lower priority ("bonus") telem that we weren't counting on, so it's okay if this isn't delivered
             regularly or sometimes gets dropped. 
             Also, it's not too unreasonable to package all of these together since they're all read together at once using ADC2 Group1.| | | | |
| | | |CURRENT_3V3_FPGA|U16||Raw ADC reading from the INA190A5 of the current being drawn by the FPGA's 3.3V rail. GDS will convert this to mA.|
| | | |CURRENT_3V3_RADIO|U16||Raw ADC reading from the INA190A5 of the current being drawn by the Radio's 3.3V rail. GDS will convert this to mA.|
| | | |CURRENT_3V3|U16||Raw ADC reading from the INA190A5 of the current being drawn by the entire 3.3V rail (and all of its derived rails). GDS will convert this to mA.|
| | | |CURRENT_3V3_HERCULES|U16||Raw ADC reading from the INA190A5 of the current being drawn by the Hercules' 3.3V rail. GDS will convert this to mA.|
| | | |CURRENT_1V2_HERCULES|U16||Raw ADC reading from the INA190A5 of the current being drawn by the Hercules' 1.2V rail. GDS will convert this to mA.|
| | | |CURRENT_1V2_FPGA|U16||Raw ADC reading from the INA190A5 of the current being drawn by the FPGA's 1.2V rail. GDS will convert this to mA.|
| | | |CURRENT_24V|U16||Raw ADC reading from the INA190A5 of the current being drawn by the entire 24V rail. GDS will convert this to mA.|
|ADCCurrentError|7 (0x7)|Warning that less than 7 current sensors had conversions or conversion never completed meaning no new current values are being generated.| | | | |
