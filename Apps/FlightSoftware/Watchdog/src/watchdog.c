#include <msp430.h>

/**
 * This file contains the code that does most of the watching of
 * the rest of the subsystems
 *
 * The way this file is laid out is that there are a number of ISRs. Some ISRs are taking in
 * kicks and setting a flag, and the other ISR is connected to a timer, which will check if the flags
 * are set, and clear them if they are.
 */

#include <stdint.h>
#include "include/flags.h"

volatile uint16_t watchdog_flags;

/**
 * Set up the ISRs for the watchdog
 */
int watchdog_init() {

    /* trigger on P1.0 Hi/lo edge */
    P1IE |= BIT0;                    // P1.0 interrupt enabled
    P1IES |= BIT0;                   // P1.0 Hi/lo edge
    P1IFG &= ~BIT0;                  // P1.0 IFG cleared initally

    /* timer setup */
    TA0CTL = TASSEL_1 | ID_0 | TAIE;           // ACLK/1, interrupt on overflow
    TA0EX0 = TAIDEX_0;                  // ??/1
    TA0CCR0 =  1000;                           // 12.5 Hz (pretty sure its not)
    TA0CTL |= MC_2;                 // Start timer in continuous mode

    return 0;
}

/**
 * Perform the watchdog monitoring steps here
 */
int watchdog_monitor() {
    /* temporarily disable interrupts */
    __bic_SR_register(GIE);

    /* check that kicks have been received */
    if (watchdog_flags & WDFLAG_RADIO_KICK) {
        /* radio kick received, all ok! */
        watchdog_flags ^= WDFLAG_RADIO_KICK;
    } else {
        /* MISSING radio kick! */
        // TODO: reset radio
    }

    if (watchdog_flags & WDFLAG_FPGA_KICK) {
        /* FPGA kick received, all ok! */
        watchdog_flags ^= WDFLAG_FPGA_KICK;
    } else {
        /* MISSING FPGA kick! */
        // TODO: reset FPGA
    }

    /* re-enable interrupts */
    __bis_SR_register(GIE);
    return 0;
}

// TODO: put ISR for things here
/*
 * Pins that need an ISR:
 *
 * Power:
 * - P2.7 is 1V2 PowerGood
 * - P4.4 is 1V8 PowerGood
 * - P4.5 is 3V3 PowerGood (which PowerGood? Watchdog's or main systems'?)
 * - P4.7 is 5V0 PowerGood
 *
 * Kicks:
 * - P1.0 is Radio Kick
 * - P1.3 is Watchdog Int (Kick?)
 * - P3.5 is FPGA Kick
 *
 *
 * Things that might want an ISR:
 * - Watchdog timer itself
 *      it is currently disabled. Do we want to use it?
 * - Misc. timer
 *      in LPM, we want to wake up periodically to check power systems, etc.
 *      only; this can be done with a timer to wake every X seconds
 */

/* Port 1 handler */
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=PORT1_VECTOR
__interrupt void port1_isr_handler(void) {
#elif defined(__GNUC__)
void __attribute__ ((interrupt(PORT1_VECTOR))) port1_isr_handler (void) {
#else
#error Compiler not supported!
#endif
    switch(__even_in_range(P1IV, P1IV__P1IFG7)) {
        case P1IV__P1IFG0: // P1.0: Radio Kick
            watchdog_flags |= WDFLAG_RADIO_KICK;
            break;
        default: // default: ignore
            break;
    }
}

/* Port 3 handler */
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=PORT3_VECTOR
__interrupt void port3_isr_handler(void) {
#elif defined(__GNUC__)
void __attribute__ ((interrupt(PORT3_VECTOR))) port3_isr_handler (void) {
#else
#error Compiler not supported!
#endif
    switch(__even_in_range(P3IV, P3IV__P3IFG7)) {
        case P3IV__P3IFG5: // P3.5: FPGA Kick
            watchdog_flags |= WDFLAG_FPGA_KICK;
            break;
        default: // default: ignore
            break;
    }
}


/* Timer 1 handler */
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = TIMER0_A1_VECTOR
__interrupt void Timer0_A1_ISR(void) {
#elif defined(__GNUC__)
void __attribute__ ((interrupt(TIMER0_A1_VECTOR))) Timer0_A1_ISR (void) {
#else
#error Compiler not supported!
#endif
    switch (__even_in_range(TA0IV, TAIV__TAIFG)) {
        case TAIV__TAIFG: /* timer tick overflowed */
            loop_flags |= FLAG_TIMER_TICK;
            break;
        default: break;
    }
}
