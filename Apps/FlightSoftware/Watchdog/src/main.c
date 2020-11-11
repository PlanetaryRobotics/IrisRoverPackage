#include <msp430.h>

#include "include/buffer.h"
#include "include/uart.h"
#include "include/bsp.h"
#include "include/adc.h"
#include "include/flags.h"

/* define all of the buffers used in other files */
__volatile struct buffer uart0rx, uart0tx, uart1rx, uart1tx, i2crx, i2ctx;
__volatile uint16_t loop_flags;

/**
 * main.c
 */

enum rover_state {
    RS_LANDER,
    RS_MISSION,
    RS_FAULT
} rovstate;

void enterMode(enum rover_state newstate) {
    switch (newstate) {
    case RS_LANDER:
        /* TODO: initial mode; enable timer, all comms lines, and temperature checks, but don't turn on power rails */
        break;
    case RS_MISSION:
        /* bootup process - enable all rails */
        enable3V3PowerRail();
        enable24VPowerRail();

        /* TODO: mission mode; enable everything in lander, but also power on the 24V and 3V3 rails */
        break;
    case RS_FAULT:
        /* TODO: fault mode; enable everything in lander mode */
        break;
    }
    rovstate = newstate;
}

void DEBUG_SETUP() {
    enable24VPowerRail();
    enable3V3PowerRail();
    powerOnHercules();
    releaseHerculesReset();
    powerOnFpga();
    powerOnMotors();
    powerOnRadio();
    releaseRadioReset();
    releaseFPGAReset();
    releaseMotorsReset();
}


int main(void) {
    /* stop watchdog timer */
	WDTCTL = WDTPW | WDTHOLD;

	/* unlock changes to registers/ports, etc. */
	PM5CTL0 &= ~LOCKLPM5;

	/* initialize the board */
    initializeGpios();

    /* set up uart */
    uart_init();

    /* set up watchdog */
    //watchdog_init();

    /* set up the ADC */
    adc_init();

    /* debug setup */
    DEBUG_SETUP();

    __bis_SR_register(GIE); // Enable all interrupts

    // the core structure of this program is like an event loop
    while (1) {
        /* check if anything happened */
        if (!loop_flags) { /* nothing did */
            /* go back to low power mode */
            __bis_SR_register(GIE);//LPM0_bits + GIE);
            continue;
        }

        /* a cool thing happened! now time to check what it was */
        if (loop_flags & FLAG_UART0_RX_PACKET) {
            /* TODO: handle event for packet from lander */
            /* clear event when done */
            loop_flags ^= FLAG_UART0_RX_PACKET;
        }
        if (loop_flags & FLAG_UART1_RX_PACKET) {
            /* TODO: handle event for packet from hercules */
            P1OUT ^= BIT1;
            /* clear event when done */
            loop_flags ^= FLAG_UART1_RX_PACKET;
        }
        if (loop_flags & FLAG_I2C_RX_PACKET) {
            /* TODO: handle event for power system message */
            /* clear event when done */
            loop_flags ^= FLAG_I2C_RX_PACKET;
        }
        if (loop_flags & FLAG_TIMER_TICK) {
            /* TODO: handle event for heartbeat & kicks */
            switch (rovstate) {
            case RS_LANDER:
                /* TODO: send heartbeat */
                /* TODO: check temperatures */
                break;
            case RS_MISSION:
                /* TODO: send kicks to devices */
                /* check power levels */
                adc_sample();
                /* act on any kicks/flags that were set for the watchdog */
                watchdog_monitor();
                break;
            case RS_FAULT:
                /* TODO: wait for boot-back-up message */
                break;
            }

            /* clear event when done */
            loop_flags ^= FLAG_TIMER_TICK;
        }
        if (loop_flags & FLAG_TEMP_LOW) {
            if (rovstate == RS_LANDER)
                /* we can only really enable the heaters if we're connected to the lander */
                enableHeater();
            /* clear event when done */
            loop_flags ^= FLAG_TEMP_LOW;
        }
        if (loop_flags & FLAG_TEMP_HIGH) {
            if (rovstate == RS_LANDER)
                /* it only makes sense to disable the heaters if we're connected to the lander */
                disableHeater();
            /* clear event when done */
            loop_flags ^= FLAG_TEMP_HIGH;
        }
        if (loop_flags & FLAG_POWER_ISSUE) {
            if (rovstate == RS_MISSION) {
                /* TODO: turn off various power lines & enter fault mode */
            }
            /* clear event when done */
            loop_flags ^= FLAG_POWER_ISSUE;
        }
    }

	return 0;
}
