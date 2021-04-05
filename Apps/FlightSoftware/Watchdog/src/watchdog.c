/**
 * This file contains the code that does most of the watching of
 * the rest of the subsystems
 * <b>watchdog_monitor() should only ever be called in MISSION mode!</b>
 * There is no real watchdogging to do in lander connected mode.
 *
 * The way this file is laid out is that there are a number of ISRs. Some ISRs are taking in
 * kicks and setting a flag, and the other ISR is connected to a timer, which will check if the flags
 * are set, and clear them if they are.
 */

#include <msp430.h>
#include <stdint.h>
#include "include/flags.h"
#include "include/adc.h"
#include "include/uart.h"

volatile uint16_t watchdog_flags;

// for heater control
uint16_t Kp_heater = 500, PWM_limit = 0;
uint8_t heating = 0;

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

    /* Set up interrupt routine for heater control */
    TB0CCTL0 = CCIE;                        // TBCCR1 interrupt enabled
    // TODO: disbale timer interrupt when not in use

    TB0CCR0 = 10000;                        // ticks per duty cycle of heater PWM
    PWM_limit = 8500;                       // make sure PWM does not exceed ~90% to keep power use low
    TB0CCTL2 = OUTMOD_7; //CCR2 reset/set
    TB0CTL = TBSSEL__SMCLK | MC__UP | TBCLR; // SMCLK, continuous mode



    return 0;
}

/**
 * Perform the watchdog monitoring steps here
 *
 * Function called every ~5s
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

    /* unreset wifi chip */
    if (watchdog_flags & WDFLAG_UNRESET_RADIO2) {
        releaseRadioReset();
        watchdog_flags ^= WDFLAG_UNRESET_RADIO2;
    }

    /* unreset wifi chip */
    if (watchdog_flags & WDFLAG_UNRESET_RADIO1) {
        watchdog_flags |= WDFLAG_UNRESET_RADIO2;
        watchdog_flags ^= WDFLAG_UNRESET_RADIO1;
    }

    /* check ADC values */
    if (watchdog_flags & WDFLAG_ADC_READY) {
        /* ensure ADC values are in spec */
        watchdog_flags ^= WDFLAG_ADC_READY;
    }

    /* re-enable interrupts */
    __bis_SR_register(GIE);
    return 0;
}

/**
 * Function called whenever the watchdog needs to handle a byte from the hercules
 *
 * Assumes checksum valid.
 * @param buf: Pointer to first byte of header/payload (8+len bytes total)
 * @param max_l: Maximum length of bytes processes
 *
 * Returns number of bytes processed (includes header). 0 if more data bytes needed.
 */
unsigned int watchdog_handle_hercules(unsigned char *buf, uint16_t max_l) {
    unsigned int len = (buf[4]) | (buf[5] << 8);
    if (len != 0) {
        /* udp packet attached */
        if (len + 8 <= max_l) {
            /* echo back watchdog command header only */
            uart0_tx_nonblocking(8, buf);
            /* add this packet to the IP/UDP stack send buffer */
            ipudp_send_packet(buf, len);
            /* all done */
            return len + 8; // length + 8 = total bytes processed
        } else {
            /* need to wait for more bytes to come in */
            return 0;
        }
    } else {
        /* handle watchdog reset command */
        handle_watchdog_reset_cmd(buf[6]);
        /* check if we have UDP data to send back to hercules */
        if (hercbuf.used > 0) {
            /* write out our buffer */
            /* update header length first */
            buf[4] = hercbuf.used & 0xFF;
            buf[5] = (hercbuf.used >> 8) & 0xFF;
            /* recompute parity */
            uint8_t parity = 0xDC; /* sum of 0x21, 0xB0, and 0x0B */
            /* skip parity byte (i + 3) in summation */
            parity += buf[4] + buf[5];
            parity += buf[6] + buf[7];
            /* bitwise NOT to compute parity */
            parity = ~parity;
            /* write out new parity byte */
            buf[3] = parity;
            /* echo back watchdog command header updated version */
            uart0_tx_nonblocking(8, buf);
            /* send the udp packet */
            uart0_tx_nonblocking(hercbuf.used, hercbuf.buf);
            /* clear hercules send buffer */
            hercbuf.used = 0;
            hercbuf.idx = 0;
        } else {
            /* echo back watchdog command header */
//            buf[8] = 0xaa;
            uart0_tx_nonblocking(8, buf);
            /* also attach telemetry values */
            // TODO: telemetry send
            unsigned char telbuf[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
            uart0_tx_nonblocking(16, telbuf);
        }
        /* in this case, we always just processed 8 bytes header */
        return 8;
    }
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
            // exit LPM
            __bic_SR_register(DEFAULT_LPM);
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
            // exit LPM
            __bic_SR_register(DEFAULT_LPM);
            break;
        default: break;
    }
}


// Timer0_B0 interrupt service routine
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = TIMER0_B0_VECTOR
__interrupt void Timer0_B0_ISR (void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(TIMER0_B0_VECTOR))) Timer0_B0_ISR (void)
#else
#error Compiler not supported!
#endif
{
    // read thermistor value
    adc_sample();
//    while(watchdog_flags); //wait for conversion to complete

    // voltage, where LSB = 0.0008056640625V
    unsigned short therm_reading = adc_values[1];
    // iterate until reference voltage values until one is hit that is lower than measurement

    if(therm_reading > 3670){
        //   start heating when temperature drops below -5 C
        heating = 1;
    } else if(heating && therm_reading < 3352){
        //   stop heating when temperature reaches 0 C
        heating = 0;
    }

    uint16_t PWM_cycle = 0;
    // P controller output
    // setpoint is slightly above desired temp because otherwise it will get stuck slightly below it
    if(heating){
        PWM_cycle = Kp_heater * (therm_reading - 3325);
    }


    // cannot have duty cycle greater than clock
    if(PWM_cycle > PWM_limit){
        PWM_cycle = PWM_limit;
    }

    if(rovstate == RS_LANDER){
        TB0CCR2 = PWM_cycle;
    } else {
        // don't run heaters when not on lander
        TB0CCR2=0;
    }

}


