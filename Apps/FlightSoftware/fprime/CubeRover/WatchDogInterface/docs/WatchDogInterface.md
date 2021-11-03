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
|Switch_Connection_Mode|3 (0x3)|Command to send signal to MSP430 that we switch the current connection mode| | |
|Engage_From_Lander|4 (0x4)|Command to send signal to MSP430 that it should send a signal to lander to engage, sets disengage pin low| | |
|Set_Kp_Specific|170 (0xaa)|Command to send signal to MSP430 that it should set Kp to a specific value| | |
| | | |value|U16||
|Set_Heater_Duty_Cycle_Max|173 (0xad)|Command to send signal to MSP430 that it should set the max possible Duty Cycle value for the heater| | |
|Set_Heater_Duty_Cycle_Period|174 (0xae)|Command to send signal to MSP430 that it should set the period the Duty Cycle for the heater is at| | |
| | | |period|U16||
|Set_Heater_Window|175 (0xaf)|Set the Half-Width of the heater on/off deadband window around the setpoint (in thermistor ADC values). Between setpoint - half_width and setpoint + half_width, heater is off.| | |
| | | |adc_half_width|U16||
|Set_Heater_Setpoint|218 (0xda)|Command to send signal to MSP430 that it should set V to a specific value| | |
| | | |adc_setpoint|U16||
|Switch_to_Sleep_Mode|234 (0xea)|Command to send signal to MSP430 that it should go into Sleep Mode| | |
| | | |confirm|confirm_sleep_mode||
|Switch_to_Keep_Alive_Mode|235 (0xeb)|Command to send signal to MSP430 that it should go into Keep Alive Mode| | |
| | | |confirm|confirm_alive_mode||
|Switch_to_Service_Mode|236 (0xec)|Command to send signal to MSP430 that it should go into Service Mode| | |
| | | |confirm|confirm_service_mode||

## Telemetry Channel List

|Channel Name|ID|Type|Description|
|---|---|---|---|
|VOLTAGE_2_5V|2 (0x2)|int16_t|Voltage from 2.5V line from Watchdog|
|VOLTAGE_2_8V|3 (0x3)|int16_t|Voltage from 2.8V line from Watchdog|
|VOLTAGE_24V|4 (0x4)|int16_t|Voltage from 24V line from Watchdog|
|VOLTAGE_28V|5 (0x5)|int16_t|Voltage from 28V line from Watchdog|
|BATTERY_THERMISTOR|16 (0x10)|U8|Battery Charge left in mAH|
|SYSTEM_STATUS|23 (0x17)|int8_t|Boolean for Heater On/Off from Watchdog|
|BATTERY_LEVEL|24 (0x18)|U16|Value of Charge in Battery|
|BATTERY_CURRENT|25 (0x19)|U16|Value of Current Used in Battery|
|BATTERY_VOLTAGE|26 (0x1a)|U16|Value of Voltage Used in Battery|
|THERM_0|27 (0x1b)|U16|12 bit Value for Thermistor value|
|THERM_1|28 (0x1c)|U16|12 bit Value for Thermistor value|
|THERM_2|29 (0x1d)|U16|12 bit Value for Thermistor value|
|THERM_3|30 (0x1e)|U16|12 bit Value for Thermistor value|
|THERM_4|31 (0x1f)|U16|12 bit Value for Thermistor value|
|THERM_5|32 (0x20)|U16|12 bit Value for Thermistor value|

## Event List

|Event Name|ID|Description|Arg Name|Arg Type|Arg Size|Description
|---|---|---|---|---|---|---|
|WatchDogIncorrectResp|0 (0x0)|Warning that the WatchDog MSP430 sent back a response different than what was sent to it| | | | |
| | | |error|U8||The watchdog error value (reference to documentation)|
|WatchDogTimedOut|1 (0x1)|Warning that a WatchDog MSP430 watchdog timer went off| | | | |
|WatchDogCmdReceived|2 (0x2)|Notification that watchdog interface recieved a command from Cmd_Dispatcher| | | | |
| | | |Cmd|Fw::LogStringArg&|50|The cmd that watchdog interface processed|
|WatchDogCommError|3 (0x3)|Warning that a WatchDog MSP430 error has occured.| | | | |
| | | |error|U32||The watchdog error value (reference to documentation)|
|ADCThermistorError|4 (0x4)|Warning that less than 16 thermistors had conversions or conversion never completed meaning no new tempurature values are being generated| | | | |
|WatchDogIncorrectResetValue|5 (0x5)|Warning that an incorrect reset value has been sent from ground| | | | |
