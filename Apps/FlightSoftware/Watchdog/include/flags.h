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

__volatile extern uint16_t loop_flags;

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
} HeaterParams;

#define DEFAULT_KP_HEATER 500
#define DEFAULT_PWM_LIMIT 8500
#define DEFAULT_HEATER_SETPOINT 3325
#define DEFAULT_HEATER_WINDOW 60
// 3670 is the -5 deg C thermistor voltage ADC reading
#define DEFAULT_HEATER_ON_VAL 3670
// 3670 is the 0 deg C thermistor voltage ADC reading
#define DEFAULT_HEATER_OFF_VAL 3352
#define DEFAULT_HEATING 0
#define DEFAULT_HEATING_CONTROL_ENABLED 1

#ifdef __cplusplus
} /* close extern "C" */
#endif

#endif /* __FLAGS_INC */
