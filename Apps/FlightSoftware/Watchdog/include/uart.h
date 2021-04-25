/**
 * @file uart.h
 * @brief File for interfacing with UART protocol hardware module
 * File for interfacing with UART protocol hardware module. Also includes some
 * basic communications protocol parsing.
 * 
 * UART communication uses eUSCI_A0 (Hercules) and eUSCI_A1 (Lander).
 */
#ifndef __UART_INC
#define __UART_INC

#include "buffer.h"

/**
 * @brief Initialize UART hardware.
 * Sets up the interrupts and whatnot for UART.
 */
void clock_init();
void uart0_init();
void uart1_init();

#define UA1_NO_WRAPS 0
#define UA1_ADD_PKT_START 1
#define UA1_ADD_PKT_END 2
void uart0_tx_nonblocking(uint16_t length, unsigned char *buffer);
void uart1_tx_nonblocking(uint16_t length, unsigned char *buffer, uint8_t opts);

#endif /* __UART_INC */
