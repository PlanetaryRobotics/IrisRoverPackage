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
#include "include/pi.h"
#include "include/adc.h"

volatile uint16_t watchdog_flags, battery_temp;

// BELOW heater_on_min, heater turns on. ABOVE heater_off_max, heater turns off.
uint16_t heater_on_min = 0x600, heater_off_max = 0x900;

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
    TB0CCTL0 = CCIE;                        // TBCCR0 interrupt enabled
    TB0CCR0 = 10000;                        // set timer B frequency also to ~120 Hz
    TB0CTL = TBSSEL__SMCLK | MC__CONTINOUS; // SMCLK, continuous mode

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
    unsigned short therm_reading = adc_values[3];
    // iterate until reference voltage values until one is hit that is lower than measurement
    uint8_t i;
    for( i = 0; i<thermistor_ref_len; i++){
        if(therm_reading > thermistor_ref_voltages[i]){
            if(i==0){
                // temperature below readable range, set to lowest possible value
                battery_temp = 0;
                break;
            } else {
                // find change in temp [C] per step between two nearest ref voltages
                uint16_t ref_gap_steps = (thermistor_ref_voltages[i-1] - thermistor_ref_voltages[i]) / 5;

                // interpolate battery temperature from reference values
                //    battery temp scales such that 0 = -40C, 65534 = 125C
                battery_temp = i*2114 + ref_gap_steps * (thermistor_ref_voltages[i-1] - therm_reading);
                break;
            }
        }

        if(i == thermistor_ref_len-1){
            // exceeded readable range, set to highest possible value
            battery_temp = 65535;
            break;
        }

    }

    uint16_t PWM_cycle = 0;
    // calculate P controller output
    if(battery_temp > battery_target_temp){
        // don't use heater if we are over desired battery temperature
    } else{
        PWM_cycle = Kp_heater * (battery_target_temp - battery_temp);
    }

    // cannot have duty cycle greater than clock
    if(PWM_cycle > TB0CCR0){
        PWM_cycle = TB0CCR0;
    }

    TB0CCR2 = PWM_cycle; // apply duty cycle

}


