/**
 * @file flags.h
 * @brief File outlining the flags that are used to communicate with the main loop
 */
#ifndef __FLAGS_INC
#define __FLAGS_INC

#include <stdint.h>

#define FLAG_UART0_RX_PACKET 0x1
#define FLAG_UART1_RX_PACKET 0x2
#define FLAG_I2C_RX_PACKET   0x4
#define FLAG_TIMER_TICK      0x8
#define FLAG_TEMP_LOW        0x10
#define FLAG_TEMP_HIGH       0x20
#define FLAG_POWER_ISSUE     0x40

__volatile extern uint16_t loop_flags;

#define WDFLAG_RADIO_KICK 0x1
#define WDFLAG_FPGA_KICK 0x2


__volatile extern uint16_t watchdog_flags;

#endif /* __FLAGS_INC */
