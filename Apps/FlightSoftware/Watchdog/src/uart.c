/*
 * Code for interfacing with UART protocol hardware module.
 *
 * UART communication uses eUSCI_A0 (Hercules) and eUSCI_A1 (Lander).
 *
 * 9600 baud, 1 stop bit, no parity
 */
#include <msp430.h>
#include "include/uart.h"
#include "include/flags.h"

/* end byte */
#define SLIP_END 0xC0
/* escape byte */
#define SLIP_ESC 0xDB
/* escaped end */
#define SLIP_ESC_END 0xDC
/* escaped escape */
#define SLIP_ESC_ESC 0xDD

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
    unsigned char rcv;

    /* two possibilities; rx or tx */
    switch(__even_in_range(UCA0IV, USCI_UART_UCTXCPTIFG)) {
    case USCI_UART_UCTXIFG: /* transmitted byte successfully */
        /* decrement the number of bytes used */
        uart0tx.used--;

        if (uart0tx.used == 0) {
            /* done sending after this byte; clear IFG */
            UCA0IE &= ~UCTXIE;
        }

        /* send the next byte */
        UCA0TXBUF = uart0tx.buf[uart0tx.idx++];
        if (uart0tx.idx >= BUFFER_SIZE) uart0tx.idx = 0;
        break;
    case USCI_UART_UCRXIFG: /* received new byte */
        /* get the received character */
        rcv = UCA0RXBUF;

        /* regular byte */

        uart0rx.buf[uart0rx.idx++] = rcv;

        /* note that we received a byte in main loop */
        loop_flags |= FLAG_UART0_RX_PACKET;

        /* clear UART_A0 receive flag */
        /* we are done here */
        UCA0IFG &= ~UCRXIFG;
        if (uart0rx.idx >= BUFFER_SIZE) uart0rx.idx -= BUFFER_SIZE;
        break;
    default: /* some other possibilities */
        break;
    }
}

__volatile int is_escaped = 0;
__volatile int has_started = 0;

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

    // TODO: buffer wraparound seems weird right now.

    /* two possibilities; rx or tx */
    switch(__even_in_range(UCA1IV, USCI_UART_UCTXCPTIFG)) {
    case USCI_UART_UCTXIFG: /* transmitted byte successfully */
        /* decrement the number of bytes used */
        uart1tx.used--;

        if (uart1tx.used == 0) {
            /* done sending after this byte; clear IFG */
            UCA1IE &= ~UCTXIE;
        }

        /* send the next byte */
        UCA1TXBUF = uart1tx.buf[uart1tx.idx++];
        if (uart1tx.idx >= BUFFER_SIZE) uart1tx.idx = 0;
        break;
    case USCI_UART_UCRXIFG: /* received new byte */
        /* get the received character */
        rcv = UCA1RXBUF;

        if (!has_started) {
            if (rcv == SLIP_END) {
                /* start now */
                has_started = 1;
                goto end_rx;
            } else {
                goto end_rx;
            }
        }

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
            /* done reading; skip storing the end byte, and signal to the main loop that we are done */
            loop_flags |= FLAG_UART1_RX_PACKET;
            has_started = 0;
            // exit LPM
            __bic_SR_register(DEFAULT_LPM);
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
        if (uart1rx.idx >= BUFFER_SIZE) {
            uart1rx.idx -= BUFFER_SIZE;
        }
        break;
    default: /* some other possibilities */
        break;
    }
}

/* =============================== main code ================================ */

/* init function */
void uart_init() {
    /* initally all buffers are empty */
    uart0tx.idx = 0;
    uart0tx.used = 0;
    uart0rx.idx = 0;
    uart0rx.used = 0;
    uart1tx.idx = 0;
    uart1tx.used = 0;
    uart1rx.idx = 0;
    uart1rx.used = 0;

    UCA0CTLW0 = UCSWRST;                    // Put eUSCI_A0 in reset
    UCA1CTLW0 = UCSWRST;                    // Put eUSCI_A1 in reset

    /* Setup for eUSCI_A0 and eUSCI_A1 */
    /* On the MSP430FR5994, pin P2.0 is used for TX and pin P2.1 is used for RX
     * (ref: pg 92 of datasheet) */
    /* P2.0 TX: x = 0; P2SEL1.x = 1, P2SEL0.x = 0 */
    /* P2.1 RX: x = 1; P2SEL1.x = 1, P2SEL0.x = 0 */
    /* On the MSP430FR5994, pin P2.5 is used for TX and pin P2.6 is used for RX
     * (ref: pg 95 of datasheet) */
    /* P2.5 TX: x = 5; P2SEL1.x = 1, P2SEL0.x = 0 */
    /* P2.6 RX: x = 6; P2SEL1.x = 1, P2SEL0.x = 0 */

    /* set P2SEL0.5, P2SEL0.6, P2SEL0.1, and P2SEL0.0 to 0 */
    P2SEL0 &= ~(BIT0 | BIT1 | BIT5 | BIT6);
    /* set P2SEL1.5, P2SEL1.6, P2SEL1.1, and P2SEL1.0 to 1 */
    P2SEL1 |= (BIT0 | BIT1 | BIT5 | BIT6);

    CSCTL0_H = CSKEY_H;                     // Unlock CS registers
    CSCTL1 = DCOFSEL_3 | DCORSEL;           // Set DCO to 8MHz
    CSCTL2 = SELA__VLOCLK | SELS__DCOCLK | SELM__DCOCLK;
    CSCTL3 = DIVA__1 | DIVS__1 | DIVM__1;   // Set all dividers
    CSCTL0_H = 0;                           // Lock CS registers

    UCA0CTLW0 |= UCSSEL__SMCLK;             // CLK = SMCLK
    UCA1CTLW0 |= UCSSEL__SMCLK;             // CLK = SMCLK
    // Baud Rate calculation
    // 8000000/(16*9600) = 52.083
    // Fractional portion = 0.083
    // User's Guide Table 21-4: UCBRSx = 0x04
    // UCBRFx = int ( (52.083-52)*16) = 1
    UCA0BRW = 52;                           // 8000000/16/9600
    UCA0MCTLW |= UCOS16 | UCBRF_1 | 0x4900; // ???
    UCA0CTLW0 &= ~UCSWRST;                  // Release eUSCI_A0 reset
    UCA0IE |= UCRXIE;                       // Enable USCI_A0 RX interrupt

    UCA1BRW = 52;                           // 8000000/16/9600
    UCA1MCTLW |= UCOS16 | UCBRF_1 | 0x4900; // ???
    UCA1CTLW0 &= ~UCSWRST;                  // Release eUSCI_A1 reset
    UCA1IE |= UCRXIE;                       // Enable USCI_A1 RX interrupt
}


void uart0_tx_nonblocking(uint16_t length, unsigned char *buffer) {
    uint16_t i=0;
    unsigned char b;
    uint16_t curr_idx = uart0tx.idx + uart0tx.used;

    for (i = 0; i < length; i++) {
        b = buffer[i];
        uart0tx.buf[curr_idx++] = b;
        if (curr_idx >= BUFFER_SIZE) curr_idx = 0;
        uart0tx.used++;
    }

    /* start interrupts for sending async */
    UCA0IE |= UCTXIE;
}

void uart1_tx_nonblocking(uint16_t length, unsigned char *buffer, uint8_t opts) {
    uint16_t i=0;
    unsigned char b;
    uint16_t curr_idx = uart1tx.idx + uart1tx.used;

    // disable interrupts to prevent race condition
    UCA1IE &= ~UCTXIE;

    // TODO: maybe we should do SLIP encoding in the interrupt handler instead,
    // so that we can have a deterministic MTU
    // ah wait, may not be possible, as we need to indicate packet boundaries
    // somehow, and this is how

    if (opts & UA1_ADD_PKT_START) {
        uart1tx.buf[curr_idx++] = SLIP_END;
        uart1tx.used += 1;
    }

    for (i = 0; i < length; i++) {
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

    if (opts & UA1_ADD_PKT_END) {
        uart1tx.buf[curr_idx++] = SLIP_END;
        uart1tx.used += 1;
    }

    /* start interrupts for sending async */
    UCA1IE |= UCTXIE;
}


