/*
 * echo_test.c
 *
 *  Created on: Nov 27, 2020
 *      Author: ohnx
 */

#include "msp430.h"
#include "include/uart.h"

void echo_test_handler() {
    uart1_tx_nonblocking(uart1rx.idx, uart1rx.buf);
    uart1rx.idx = 0;
}

void echo_test() {
    P1DIR |= BIT1;
    P1DIR |= BIT0;
    // red LED starts on
    P1OUT |= BIT0;
    // green LED  starts off
    P1OUT &= ~(BIT1);
}


