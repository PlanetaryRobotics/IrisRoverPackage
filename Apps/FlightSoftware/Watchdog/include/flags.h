/**
 * @file flags.h
 * @brief File outlining the flags that are used to communicate with the main loop
 */
#ifndef __FLAGS_INC
#define __FLAGS_INC

#include "common.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define FLAG_UART0_RX_PACKET 0x1
#define FLAG_UART1_RX_PACKET 0x2
#define FLAG_TIMER_TICK 0x8
#define FLAG_TEMP_LOW 0x10
#define FLAG_TEMP_HIGH 0x20
#define FLAG_POWER_ISSUE 0x40
#define FLAG_I2C_GAUGE_READING_ACTIVE 0x80

// All flags need to be 1-hot:
#define WDFLAG_RADIO_KICK 0x0000'0001
#define WDFLAG_ADC_READY 0x0000'0002
#define WDFLAG_UNRESET_RADIO1 0x0000'0004
#define WDFLAG_UNRESET_RADIO2 0x0000'0008
#define WDFLAG_UNRESET_HERCULES 0x0000'0010
#define WDFLAG_UNRESET_MOTOR1 0x0000'0020
#define WDFLAG_UNRESET_MOTOR2 0x0000'0040
#define WDFLAG_UNRESET_MOTOR3 0x0000'0080
#define WDFLAG_UNRESET_MOTOR4 0x0000'0100
#define WDFLAG_UNRESET_FPGA 0x0000'0200
#define WDFLAG_UNRESET_3V3 0x0000'0400
#define WDFLAG_POWER_ON_V_SYS_ALL 0x0000'0800
#define WDFLAG_HERCULES_KICK 0x0000'1000
#define WDFLAG_WAITING_FOR_IO_EXPANDER_WRITE 0x0000'2000
#define WDFLAG_POWER_ON_HERCULES 0x0000'4000
#define WDFLAG_FULL_POWER_REBOOT 0x0000'8000
#define WDFLAG_SAFETY_TIMER_KICK 0x0001'0000
// Require two non-adjacent bits (A and B) to be set to trigger full power
// reboot stages (more bitflip resilient):
#define WDFLAG_SAFETY_TIMER__PWR_OFF_1A 0x0002'0000
#define WDFLAG_SAFETY_TIMER__PWR_OFF_2A 0x0004'0000
#define WDFLAG_SAFETY_TIMER__PWR_ON_1A 0x0008'0000
#define WDFLAG_SAFETY_TIMER__PWR_ON_2A 0x0010'0000
#define WDFLAG_SAFETY_TIMER__PWR_ON_3A 0x0020'0000
#define WDFLAG_SAFETY_TIMER__PWR_ON_4A 0x0040'0000
#define WDFLAG_SAFETY_TIMER__PWR_OFF_1B 0x0080'0000
#define WDFLAG_SAFETY_TIMER__PWR_OFF_2B 0x0100'0000
#define WDFLAG_SAFETY_TIMER__PWR_ON_1B 0x0200'0000
#define WDFLAG_SAFETY_TIMER__PWR_ON_2B 0x0400'0000
#define WDFLAG_SAFETY_TIMER__PWR_ON_3B 0x0800'0000
#define WDFLAG_SAFETY_TIMER__PWR_ON_4B 0x1000'0000

#define WDOPT_MONITOR_HERCULES 0x0001

#define ENTER_DEFAULT_LPM LPM1
#define EXIT_DEFAULT_LPM LPM1_EXIT

        //* Safety Timer:
        // If GND doesn't check in with us (WD) through the whole comms pipeline
        // by sending an ACK command every X minutes (reasonably large number),
        // WD assumes something bad has happened (locked Radio, Herc, etc),
        // possibly severing contact with GND and that GND isn't able to
        // recover it. So, to recover, WD gracefully powers everything but the
        // batteries down and then boots back up into a safe state with all
        // non-communication peripherals (camera, motors) OFF.

        typedef enum SafetyTimer_RebootControlValue
        {
                // Only allow two values so we can detect a bitflip here:
                SAFETY_TIMER__REBOOT_CONTROL_OFF = 0x00,
                SAFETY_TIMER__REBOOT_CONTROL_ON = 0xFF
        } SafetyTimer_RebootControlValue;

        // ! NOTE: The number of centiseconds that fit in a U16 is not enough
        // (only 10 mins) and bumping the timer to a long (U32) isn't advisable
        // (would screw w timing of all time checks), so we need to just count
        // the number of expirations before a trigger. 5 would be sufficient
        // but 10 makes the math easier on operators, so cutoffs values are now
        // "tenths of a cutoff"
        typedef struct SafetyTimerParams
        {
                // Is timer allowed to fully reboot the rover when the cutoff is reached:
                // (if an invalid enum value, there's been a bitflip and value is reset to ON).
                SafetyTimer_RebootControlValue timerRebootControlOn;
                // Centisecond threshold where the safety timer fully reboots
                // the rover if it hasn't heard from us (received an ACK):
                uint16_t timerRebootCutoffCentisecondsTenth;
                // Number of times the timer has expired (needs to expire 10
                // times to trigger a reset) - see note above for why.
                // We need this to get the timeout to a reasonable value.
                // We'll also emit a warning every time this expires.
                uint8_t tenthTimerExpirationCount;
                // System time in centiseconds at the last time we received an
                // ACK from Ground (or when a tenth-timer expired).
                // Tops out at 10.9mins.
                // Make sure to check this in a rollover-safe way:
                // (now-last) > cutoff.
                uint16_t centisecondsAtLastEvent;
        } SafetyTimerParams;

// Number of times the timer needs to expire to trigger a reboot:
// CAN'T BE <2.
#define SAFETY_TIMER__TENTH_TIMER_EXPIRATION_COUNT_TRIGGER 10
// Default safety timer cutoff/10 in centiseconds (4 mins -> 40min timer):
#define SAFETY_TIMER__DEFAULT_CUTOFF_TENTH_CS 24000
// How much to increment or decrement the safety timer/10 cutoff by when told to (0.5min. -> 5min. timer inc.):
#define SAFETY_TIMER__CUTOFF_TENTH_INCREMENT_CS 3000
// How soon [in cs] before the final expiration before reboot to start emitting a warning message every tick (~5s):
// Should be at least 2x roundtrip delay (set to 5min)
#define SAFETY_TIMER__FINAL_COUNTDOWN_START_TIME_CS 30000
// Maximum value the safety timer/10 is allowed to have (~10.9min.):
#define SAFETY_TIMER__CUTOFF_TENTH_MAX_VAL_CS 0xFFFE
// Minimum value the safety timer/10 is allowed to have (0.5min. -> 5min. timer):
#define SAFETY_TIMER__CUTOFF_TENTH_MIN_VAL_CS 3000

        typedef enum Heater_ForceState
        {
                // force the heater to always be on or off (at whatever duty)
                // - using specific enums to make it hard to be bitflipped
                // into this state.
                HEATER_FORCE_ALWAYS_ON = 0xAA,  // heater always ON
                HEATER_FORCE_ALWAYS_OFF = 0x55, // heater always OFF
                HEATER_FORCE_NOTHING = 0xFF     // let heater control do its thing
        } Heater_ForceState;

        typedef enum HeaterControl_InputSource
        {
                // force the heater to always be on or off (at whatever duty)
                // - using specific enums to make it hard to be bitflipped
                // into this state.
                HEATER_CONTROL_INPUT_BATT_RT = 0xAA, // Use Normal Battery RT Thermistor (Default)
                HEATER_CONTROL_INPUT_CHARGER = 0x55, // Power up the Charging IC, Don't Charge, and use the Charging Thermistor (only as a last ditch effort)
        } HeaterControl_InputSource;

        // Core heater settings that we want to be persistent (e.g. we DO want
        // setpoints to persist but we DON'T want flags to persist - good for
        // those to reset on reboot in case they somehow cause a crash):
        // ... basically only persist variables that are changed *directly*
        // in response to a command.
        typedef struct HeaterParamsPersistent
        {
                uint16_t m_kpHeater;
                uint16_t m_pwmLimit;
                uint16_t m_heaterSetpoint;
                uint16_t m_heaterWindow;
                uint16_t m_heaterOnVal;  // heater transitions to ON when T ADC < this value
                uint16_t m_heaterOffVal; // heater transitions to OFF when T ADC > this value
                BOOL m_heatingControlEnabled;
                uint16_t m_heaterDutyCyclePeriod;
                uint16_t m_heaterDutyCycle;
        } HeaterParamsPersistent;

        typedef struct HeaterParams
        {
                HeaterParamsPersistent *persistent;
                BOOL m_heating;
                BOOL m_thresholdsChanged; // flag that the on or off thresholds have changed since they were last checked
                // Intentionally don't persist input source and force state b/c
                // it's potentially dangerous and we want this to be a last
                // resort measure, so it's okay for this to be manually
                // activated after every reboot:
                Heater_ForceState m_forceState;          // force the heater to always be on or off (at whatever duty) - using specific enums to make it hard to be bitflipped into this state.
                HeaterControl_InputSource m_inputSource; // what sensor should drive the heater controller (note, changing this should be accompanied by changing thresholds)
        } HeaterParams;

#define DEFAULT_KP_HEATER 500        // deprecated
#define DEFAULT_PWM_LIMIT 9999       // deprecated
#define DEFAULT_HEATER_SETPOINT 3325 // deprecated
#define DEFAULT_HEATER_WINDOW 60     // deprecated
// 2540 is the 0 deg C thermistor voltage ADC reading - heater transitions to ON when T ADC < this value:
#define DEFAULT_HEATER_ON_VAL 2540
// 2042 is the 10 deg C thermistor voltage ADC reading - heater transitions to OFF when T ADC > this value:
#define DEFAULT_HEATER_OFF_VAL 2040
#define DEFAULT_HEATING_CONTROL_ENABLED TRUE
#define DEFAULT_HEATER_DUTY_CYCLE_PERIOD 10000
#define DEFAULT_HEATER_DUTY_CYCLE 9998

#define GENERIC_BIT_INDEX_TO_TYPE_MASK(type, index) (((type)1) << ((type)(index)))
#define GENERIC_BIT_INDEX_TO_UINT8_MASK(index) GENERIC_BIT_INDEX_TO_TYPE_MASK(uint8_t, index)
#define GENERIC_BIT_INDEX_TO_UINT16_MASK(index) GENERIC_BIT_INDEX_TO_TYPE_MASK(uint16_t, index)
#define GENERIC_BIT_INDEX_TO_UINT32_MASK(index) GENERIC_BIT_INDEX_TO_TYPE_MASK(uint32_t, index)
#define GENERIC_BIT_INDEX_TO_UINT64_MASK(index) GENERIC_BIT_INDEX_TO_TYPE_MASK(uint64_t, index)

#define GENERIC_SET_BIT_IN_UINT8(uintVal, index) \
        uintVal |= ((uint8_t)GENERIC_BIT_INDEX_TO_UINT8_MASK(index))
#define GENERIC_SET_BIT_IN_UINT16(uintVal, index) \
        uintVal |= ((uint16_t)GENERIC_BIT_INDEX_TO_UINT16_MASK(index))
#define GENERIC_SET_BIT_IN_UINT32(uintVal, index) \
        uintVal |= ((uint32_t)GENERIC_BIT_INDEX_TO_UINT32_MASK(index))
#define GENERIC_SET_BIT_IN_UINT64(uintVal, index) \
        uintVal |= ((uint64_t)GENERIC_BIT_INDEX_TO_UINT64_MASK(index))

#define GENERIC_CLEAR_BIT_IN_UINT8(uintVal, index) \
        uintVal &= ~((uint8_t)GENERIC_BIT_INDEX_TO_UINT8_MASK(index))
#define GENERIC_CLEAR_BIT_IN_UINT16(uintVal, index) \
        uintVal &= ~((uint16_t)GENERIC_BIT_INDEX_TO_UINT16_MASK(index))
#define GENERIC_CLEAR_BIT_IN_UINT32(uintVal, index) \
        uintVal &= ~((uint32_t)GENERIC_BIT_INDEX_TO_UINT32_MASK(index))
#define GENERIC_CLEAR_BIT_IN_UINT64(uintVal, index) \
        uintVal &= ~((uint64_t)GENERIC_BIT_INDEX_TO_UINT64_MASK(index))

        typedef enum OutputPinStatusBitIndices
        {
                // These are digital outputs. If the corresponding bit is set, these should be high
                OPSBI__V_LANDER_REG_EN = 0,
                OPSBI__HEATER,
                OPSBI__DEPLOYMENT,
                OPSBI__FPGA_KICK_AKA_CAM_SELECT,
                OPSBI__LATCH_BATT,
                OPSBI__3V3_EN,
                OPSBI__HERCULES_ON,
                OPSBI__FPGA_ON,
                OPSBI__MOTOR_ON,
                OPSBI__CHRG_EN,
                OPSBI__CHRG_EN_FORCE_HIGH,
                OPSBI__BATTERY_EN,
                OPSBI__V_SYS_ALL_EN,
                OPSBI__V_SYS_ALL_EN_FORCE_LOW,
                OPSBI__HERCULES_N_RST,
                OPSBI__HERCULES_N_PORRST,
                OPSBI__FPGA_N_RST,
                OPSBI__RADIO_N_RST,
                OPSBI__RADIO_ON,
                OPSBI__BMS_BOOT,
                OPSBI__LATCH_SET,
                OPSBI__LATCH_RESET,
                OPSBI__BATT_STAT,

                OPSBI__RADIO_N_RESET_IS_INPUT,
                OPSBI__HERCULES_N_RST_IS_INPUT,
                OPSBI__HERCULES_N_PORRST_IS_INPUT,
                OPSBI__FPGA_N_RST_IS_INPUT,
                OPSBI__LATCH_SET_IS_INPUT,
                OPSBI__LATCH_RESET_IS_INPUT,
                OPSBI__BATT_STAT_IS_INPUT,
        } OutputPinStatusBitIndices;

#define OPSBI_MASK(opsbi_index_enum) GENERIC_BIT_INDEX_TO_UINT32_MASK(opsbi_index_enum)
#define SET_OPSBI_IN_UINT(opsbiUint, opsbi_index_enum) GENERIC_SET_BIT_IN_UINT32(opsbiUint, opsbi_index_enum)
#define SET_OPSBI_IN_UINT_PTR(opsbiUintPtr, opsbi_index_enum) SET_OPSBI_IN_UINT(*opsbiUintPtr, opsbi_index_enum)
#define CLEAR_OPSBI_IN_UINT(opsbiUint, opsbi_index_enum) GENERIC_CLEAR_BIT_IN_UINT32(opsbiUint, opsbi_index_enum)
#define CLEAR_OPSBI_IN_UINT_PTR(opsbiUintPtr, opsbi_index_enum) CLEAR_OPSBI_IN_UINT(*opsbiUintPtr, opsbi_index_enum)

        typedef enum InputPinAndStateBitIndices
        {
                // State related things
                IPASBI__UART0_INITIALIZED = 0,
                IPASBI__UART1_INITIALIZED,
                IPASBI__DEPLOYED,
                IPASBI__DEPLOYING,

                // These are digital inputs. If the corresponding bit is set, then the input reads high
                IPASBI__CHARGE_STAT1,
                IPASBI__CHARGE_STAT2,
                IPASBI__BATT_STAT,
                IPASBI__LATCH_STAT,
                IPASBI__PG12,
                IPASBI__PG18,
                IPASBI__PG33,
                IPASBI__PG50,
        } InputPinAndStateBitIndices;

#define IPASBI_MASK(ipasbi_index_enum) GENERIC_BIT_INDEX_TO_UINT16_MASK(ipasbi_index_enum)
#define SET_IPASBI_IN_UINT(ipasbiUint, ipasbi_index_enum) GENERIC_SET_BIT_IN_UINT16(ipasbiUint, ipasbi_index_enum)
#define SET_IPASBI_IN_UINT_PTR(ipasbiUintPtr, ipasbi_index_enum) SET_IPASBI_IN_UINT(*ipasbiUintPtr, ipasbi_index_enum)
#define CLEAR_IPASBI_IN_UINT(ipasbiUint, ipasbi_index_enum) GENERIC_CLEAR_BIT_IN_UINT16(ipasbiUint, ipasbi_index_enum)
#define CLEAR_IPASBI_IN_UINT_PTR(ipasbiUintPtr, ipasbi_index_enum) CLEAR_IPASBI_IN_UINT(*ipasbiUintPtr, ipasbi_index_enum)

        typedef enum ResetActionBitIndices
        {
                RABI__NO_RESET = 0,
                RABI__HERCULES_RESET,
                RABI__HERCULES_UNRESET,
                RABI__HERCULES_POWER_ON,
                RABI__HERCULES_POWER_OFF,
                RABI__RADIO_RESET,
                RABI__RADIO_UNRESET,
                RABI__RADIO_POWER_ON,
                RABI__RADIO_POWER_OFF,
                RABI__CAM_FPGA_RESET,
                RABI__CAM_FPGA_UNRESET,
                RABI__CAM_FPGA_POWER_ON,
                RABI__CAM_FPGA_POWER_OFF,
                RABI__ALL_MOTORS_POWER_ON,
                RABI__ALL_MOTORS_POWER_OFF,
                RABI__3V3_EN_RESET,
                RABI__3V3_EN_UNRESET,
                RABI__3V3_EN_POWER_ON,
                RABI__3V3_EN_POWER_OFF,
                RABI__V_SYS_ALL_OFF__RESET,
                RABI__V_SYS_ALL_ON__UNRESET,
                RABI__V_SYS_ALL_POWER_ON,
                RABI__V_SYS_ALL_POWER_OFF,
                RABI__HDRM_DEPLOY_SIGNAL_POWER_OFF,
                RABI__FPGA_CAM_0_SELECT,
                RABI__FPGA_CAM_1_SELECT,
                RABI__BATTERY_CHARGE_START,
                RABI__BATTERY_CHARGE_STOP,
                RABI__RS422_UART_ENABLE,
                RABI__RS422_UART_DISABLE,
                RABI__AUTO_HEATER_CONTROLLER_ENABLE,
                RABI__AUTO_HEATER_CONTROLLER_DISABLE,
                RABI__HERCULES_WATCHDOG_ENABLE,
                RABI__HERCULES_WATCHDOG_DISABLE,
                RABI__BATTERIES_ENABLE,
                RABI__BATTERIES_DISABLE,
                RABI__HDRM_DEPLOY_SIGNAL_POWER_ON,
                RABI__HERCULES_WATCHDOG_RESET
        } ResetActionBitIndices;

#define RABI_MASK(rabi_index_enum) GENERIC_BIT_INDEX_TO_UINT64_MASK(rabi_index_enum)
#define SET_RABI_IN_UINT(rabiUint, rabi_index_enum) GENERIC_SET_BIT_IN_UINT64(rabiUint, rabi_index_enum)
#define SET_RABI_IN_UINT_PTR(rabiUintPtr, rabi_index_enum) SET_RABI_IN_UINT(*rabiUintPtr, rabi_index_enum)
#define CLEAR_RABI_IN_UINT(rabiUint, rabi_index_enum) GENERIC_CLEAR_BIT_IN_UINT64(rabiUint, rabi_index_enum)
#define CLEAR_RABI_IN_UINT_PTR(rabiUintPtr, rabi_index_enum) CLEAR_RABI_IN_UINT(*rabiUintPtr, rabi_index_enum)

        typedef struct WatchdogStateDetails
        {
                uint32_t m_outputPinBits;
                uint16_t m_inputPinAndStateBits;
                uint64_t m_resetActionBits;
                uint8_t m_stateAsUint;
                HeaterParams m_hParams;
                SafetyTimerParams m_safetyTimerParams;

        } WatchdogStateDetails;

#ifdef __cplusplus
} /* close extern "C" */
#endif

#endif /* __FLAGS_INC */
