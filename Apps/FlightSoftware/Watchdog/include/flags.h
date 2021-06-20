/**
 * @file flags.h
 * @brief File outlining the flags that are used to communicate with the main loop
 */
#ifndef __FLAGS_INC
#define __FLAGS_INC

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

#define DEFAULT_LPM LPM2_bits

__volatile extern uint16_t watchdog_flags;

enum rover_state {
    RS_SLEEP = 0x02, // about to lose power
    RS_SERVICE = 0x04, // temporary state to act as a buffer between mission and service
    RS_KEEPALIVE = 0x08, // connected to the lander 28V, send heartbeats
    RS_MISSION = 0x10, // power everything up. woot!
    RS_FAULT = 0x20 // oops
};
extern enum rover_state rovstate;

// enter a rover state
void enterMode(enum rover_state newstate);

#ifdef __cplusplus
} /* close extern "C" */
#endif

#endif /* __FLAGS_INC */
