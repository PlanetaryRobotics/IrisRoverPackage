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

/* baud rate configurations - be sure to use a uint32_t!! */
#define UART0_BAUD 115200
#define UART1_BAUD 115200

#include "buffer.h"

extern void (*uart0_rx_handler)(uint16_t len, struct buffer *buf);
extern void (*uart1_rx_handler)(uint16_t len, struct buffer *buf);

/**
 * @brief Initialize UART hardware.
 * Sets up the interrupts and whatnot for UART.
 */
void uart_init();

void uart1_tx_nonblocking(uint16_t length, unsigned char *buffer);

#endif /* __UART_INC */
