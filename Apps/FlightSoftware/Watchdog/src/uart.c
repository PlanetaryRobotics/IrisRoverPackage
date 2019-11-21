/*
 * Code for interfacing with UART protocol hardware module.
 *
 * UART communication uses eUSCI_A0 (Hercules) and eUSCI_A1 (Lander).
 */
#include <msp430.h>
#include "include/uart.h"

/* end byte */
#define SLIP_END 0xC0
/* escape byte */
#define SLIP_ESC 0xDB
/* escaped end */
#define SLIP_ESC_END 0xDC
/* escaped escape */
#define SLIP_ESC_ESC 0xDD

/* uart0 rx handler (will only be called after Hercules packet fully rx'd) */
void (*uart0_rx_handler)(uint16_t len, struct buffer *buf) = (void *)(0);

/* uart1 rx handler (will only be called after SLIP packet fully received) */
void (*uart1_rx_handler)(uint16_t len, struct buffer *buf) = (void *)(0);

void test_all_chars() {
    unsigned char i;

    for (i = 255; i >= 0; i--) {
        if (uart1rx.buf[i] != i) return;
    }

    P1OUT |= BIT1;
}

/* ========================== interrupt handlers =========================== */

/* uart0 rx interrupt handler (Hercules) */
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=EUSCI_A0_VECTOR
__interrupt void USCI_A0_ISR(void) {
#elif defined(__GNUC__)
void __attribute__ ((interrupt(EUSCI_A0_VECTOR))) USCI_A0_ISR (void) {
#else
#error Compiler not supported!
#endif
    UCA0IFG &= ~UCRXIFG;
}

__volatile int is_escaped = 0;

/* UCA1 interrupt handler (watchdog SLIP) */
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=EUSCI_A1_VECTOR
__interrupt void USCI_A1_ISR(void) {
#elif defined(__GNUC__)
void __attribute__ ((interrupt(EUSCI_A1_VECTOR))) USCI_A1_ISR (void) {
#else
#error Compiler not supported!
#endif

    unsigned char rcv;

    /* two possibilities; rx or tx */
    switch(__even_in_range(UCA1IV, USCI_UART_UCTXCPTIFG)) {
    case USCI_UART_UCTXIFG: /* transmitted byte successfully */
        if (uart1tx.used == 0) {
            /* done sending; clear IFG */
            UCA1IFG &= ~UCTXIFG;
        } else {
            /* still more bytes to send */
            /* decrement the number of bytes used */
            uart1tx.used--;

            /* send the next byte */
            UCA1TXBUF = uart1tx.buf[uart1tx.idx++];
            if (uart1tx.idx >= BUFFER_SIZE) uart1tx.idx = 0;
        }
        break;
    case USCI_UART_UCRXIFG: /* received new byte */
        /* get the received character */
        rcv = UCA1RXBUF;

        /* deal with escaped characters */
        if (is_escaped && rcv == SLIP_ESC_END) {
            /* push the escaped character */
            uart1rx.buf[uart1rx.idx++] = SLIP_END;
            is_escaped = 0;
            goto end_rx;
        } else if (is_escaped && rcv == SLIP_ESC_ESC) {
            /* push the escaped character */
            uart1rx.buf[uart1rx.idx++] = SLIP_ESC;
            is_escaped = 0;
            goto end_rx;
        }

        /* regular case (not escaped character) */
        switch (rcv) {
        case SLIP_END:
            /* done reading; skip storing the end byte */
            /* TODO: add code to call uart0_rx_handler() upon return */
            test_all_chars();
            break;
        case SLIP_ESC:
            /* about to start escape sequence; skip storing this byte */
            is_escaped = 1;
            break;
        default:
            /* regular byte */
            uart1rx.buf[uart1rx.idx++] = rcv;
            break;
        }

        /* clear UART_A1 receive flag */
        /* we are done here */
    end_rx:
        UCA1IFG &= ~UCRXIFG;
        break;
    default: /* some other possibilities */
        break;
    }
}

/* =============================== main code ================================ */

/* init function */
void uart_init() {
    /* Setup for eUSCI_A0 */
    /* On the MSP430FR5994, pin P2.0 is used for MOSI/SIMO, pin P2.1 is used for
     * MISO/SOMI (ref: pg 92 of datasheet), pin 1.4 is used for STE, and pin 1.5
     * is used for CLK (ref: pg 89 of datasheet) */
    /* P1.4 STE: x = 4; P1SEL1.x = 1, P1SEL0.x = 0 */
    /* P1.5 CLK: x = 5; P1SEL1.x = 1, P1SEL0.x = 0 */

    /* set P1SEL0.4 and P1SEL0.5 to 0 */
    P1SEL0 &= ~(0b00110000);
    /* set P1SEL1.4 and P1SEL1.5 to 1 */
    P1SEL1 |= (0b00110000);

    /* P2.0 MOSI/SIMO: x = 0; P2SEL1.x = 1, P2SEL0.x = 0 */
    /* P2.1 MISO/SOMI: x = 1; P2SEL1.x = 1, P2SEL0.x = 0 */
    /* see below (wrapped into one operation) */

    /* Setup for eUSCI_A1 */
    /* On the MSP430FR5994, pin P2.5 is used for TX and pin P2.6 is used for RX
     * (ref: pg 95 of datasheet) */
    /* P2.5 TX: x = 5; P2SEL1.x = 1, P2SEL0.x = 0 */
    /* P2.6 RX: x = 6; P2SEL1.x = 1, P2SEL0.x = 0 */

    /* set P2SEL0.5, P2SEL0.6, P2SEL0.1, and P2SEL0.0 to 0 */
    P2SEL0 &= ~(0b01100011);
    /* set P2SEL1.5, P2SEL1.6, P2SEL1.1, and P2SEL1.0 to 1 */
    P2SEL1 |= (0b01100011);

    CSCTL0_H = CSKEY_H;                     // Unlock CS registers
    CSCTL1 = DCOFSEL_3 | DCORSEL;           // Set DCO to 8MHz
    CSCTL2 = SELA__VLOCLK | SELS__DCOCLK | SELM__DCOCLK;
    CSCTL3 = DIVA__1 | DIVS__1 | DIVM__1;   // Set all dividers
    CSCTL0_H = 0;                           // Lock CS registers

    UCA1CTLW0 = UCSWRST;                    // Put eUSCI_A1 in reset
    UCA1CTLW0 |= UCSSEL__SMCLK;             // CLK = SMCLK
    // Baud Rate calculation
    // 8000000/(16*9600) = 52.083
    // Fractional portion = 0.083
    // User's Guide Table 21-4: UCBRSx = 0x04
    // UCBRFx = int ( (52.083-52)*16) = 1
    UCA1BRW = 52;                           // 8000000/16/9600
    UCA1MCTLW |= UCOS16 | UCBRF_1 | 0x4900; // ???
    UCA1CTLW0 &= ~UCSWRST;                  // Initialize eUSCI_A1
    UCA1IE |= UCRXIE;                       // Enable USCI_A1 RX interrupt
}

void uart1_tx_nonblocking(uint16_t length, unsigned char *buffer) {
    uint16_t i;
    unsigned char b;
    uint16_t curr_idx = uart1tx.idx + uart1tx.used;

    for (i = length; i > 0; i--) {
        b = buffer[i];
        /* check what byte this is */
        switch (b) {
        case SLIP_END:
            /* have to send 2 characters to escape END */
            uart1tx.buf[curr_idx++] = SLIP_ESC;
            if (curr_idx >= BUFFER_SIZE) curr_idx = 0;
            uart1tx.buf[curr_idx++] = SLIP_ESC_END;
            if (curr_idx >= BUFFER_SIZE) curr_idx = 0;
            uart1tx.used += 2;
            break;
        case SLIP_ESC:
            /* have to send 2 characters to escape ESC */
            uart1tx.buf[curr_idx++] = SLIP_ESC;
            if (curr_idx >= BUFFER_SIZE) curr_idx = 0;
            uart1tx.buf[curr_idx++] = SLIP_ESC_ESC;
            if (curr_idx >= BUFFER_SIZE) curr_idx = 0;
            uart1tx.used += 2;
            break;
        default:
            /* no need to escape or w/e */
            uart1tx.buf[curr_idx++] = b;
            if (curr_idx >= BUFFER_SIZE) curr_idx = 0;
            uart1tx.used++;
            break;
        }
    }
}
