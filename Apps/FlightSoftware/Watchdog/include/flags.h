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
#define FLAG_TIMER_TICK      0x8
#define FLAG_TEMP_LOW        0x10
#define FLAG_TEMP_HIGH       0x20
#define FLAG_POWER_ISSUE     0x40
#define FLAG_I2C_GAUGE_READING_ACTIVE 0x80

#define WDFLAG_RADIO_KICK 0x0001
#define WDFLAG_ADC_READY 0x0002
#define WDFLAG_UNRESET_RADIO1 0x0004
#define WDFLAG_UNRESET_RADIO2 0x0008
#define WDFLAG_UNRESET_HERCULES 0x0010
#define WDFLAG_UNRESET_MOTOR1 0x0020
#define WDFLAG_UNRESET_MOTOR2 0x0040
#define WDFLAG_UNRESET_MOTOR3 0x0080
#define WDFLAG_UNRESET_MOTOR4 0x0100
#define WDFLAG_UNRESET_FPGA   0x0200
#define WDFLAG_UNRESET_3V3    0x0400
#define WDFLAG_UNRESET_24V    0x0800
#define WDFLAG_HERCULES_KICK  0x1000

#define WDOPT_MONITOR_HERCULES  0x0001

#define ENTER_DEFAULT_LPM LPM3
#define EXIT_DEFAULT_LPM LPM3_EXIT

typedef struct HeaterParams
{
    uint16_t m_kpHeater;
    uint16_t m_pwmLimit;
    uint16_t m_heaterSetpoint;
    uint16_t m_heaterWindow;
    uint16_t m_heaterOnVal;
    uint16_t m_heaterOffVal;
    BOOL m_heating;
    BOOL m_heatingControlEnabled;
    uint16_t m_heaterDutyCyclePeriod;
    uint16_t m_heaterDutyCycle;
} HeaterParams;

#define DEFAULT_KP_HEATER 500
#define DEFAULT_PWM_LIMIT 8500
#define DEFAULT_HEATER_SETPOINT 3325
#define DEFAULT_HEATER_WINDOW 60
// 3670 is the -5 deg C thermistor voltage ADC reading
#define DEFAULT_HEATER_ON_VAL 3670
// 3670 is the 0 deg C thermistor voltage ADC reading
#define DEFAULT_HEATER_OFF_VAL 3352
#define DEFAULT_HEATING FALSE
#define DEFAULT_HEATING_CONTROL_ENABLED TRUE
#define DEFAULT_HEATER_DUTY_CYCLE_PERIOD 10000
#define DEFAULT_HEATER_DUTY_CYCLE 0

typedef enum PersistentStatusBitIndices
{
    // State related things
    PSBI__I2C_INITIALIZED = 0,
    PSBI__UART0_INITIALIZED,
    PSBI__UART1_INITIALIZED,
    PSBI__DEPLOYED,

    // These are digital outputs. If the corresponding bit is set, these should be high
    PSBI__V_LANDER_REG_EN,
    // The state of BATT_CTRL_EN is not saved persistently by design (for safety reasons)
    PSBI__DEPLOYMENT,
    // The state of LATCH_BATT is not saved persistently by design (it should only ever be pulsed)
    PSBI__3V3_EN,
    PSBI__HERCULES_ON,
    PSBI__FPGA_ON,
    PSBI__MOTOR_ON,
    PSBI__CHRG_EN,
    PSBI__CHRG_EN_FORCE_HIGH,
    PSBI__BATTERY_EN,
    PSBI__V_SYS_ALL_EN,
    PSBI__V_SYS_ALL_EN_FORCE_LOW,
    PSBI__MC_RST_A,
    PSBI__MC_RST_B,
    PSBI__MC_RST_C,
    PSBI__MC_RST_D,
    PSBI__HERCULES_N_RST,
    PSBI__HERCULES_N_PORRST,
    PSBI__FPGA_N_RST,
    PSBI__LATCH_RST,
    PSBI__RADIO_N_RST,
    PSBI__LATCH_SET,
    PSBI__RADIO_ON,
    PSBI__BMS_BOOT
} PersistentStatusBitIndices;

#define PSBI_MASK(psbi_index_enum) (((uint32_t) 1) << ((uint32_t) (psbi_index_enum)))

#define SET_PSBI_IN_STATE(state, psbi_index_enum) \
    /*(state).m_statusBits |= ((uint32_t) PSBI_MASK(psbi_index_enum))*/

#define SET_PSBI_IN_STATE_PTR(statePtr, psbi_index_enum) SET_PSBI_IN_STATE(*statePtr, psbi_index_enum)

#define CLEAR_PSBI_IN_STATE(state, psbi_index_enum) \
    /*(state).m_statusBits &= ~((uint32_t) PSBI_MASK(psbi_index_enum))*/

#define CLEAR_PSBI_IN_STATE_PTR(statePtr, psbi_index_enum) CLEAR_PSBI_IN_STATE(*statePtr, psbi_index_enum)

typedef struct PersistentState
{
    uint32_t m_statusBits;
    HeaterParams m_heaterParams;
    uint8_t m_stateAsUint;
} PersistentState;

#ifdef __cplusplus
} /* close extern "C" */
#endif

#endif /* __FLAGS_INC */
