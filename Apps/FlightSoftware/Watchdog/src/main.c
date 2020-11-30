#include <msp430.h>

#include "include/buffer.h"
#include "include/uart.h"
#include "include/bsp.h"
#include "include/adc.h"
#include "include/flags.h"
#include "include/ip_udp.h"

// Generated by Justin N. on 11/30/2020 using the following command and a little bit of Vim magic
// od -An -tx1 HerculesTlmPacket.bin > HerculesTlmPacket.h
uint8_t packet[504] = {0x45, 0x00, 0x01, 0xf8, 0x00, 0x01, 0x00, 0x00, 0xff, 0x11, 0x6a, 0x9f, 0xc0, 0xa8, 0x67, 0x02,
                       0xc0, 0xa8, 0x67, 0x01, 0xa4, 0x10, 0xa4, 0x10, 0x01, 0xe4, 0x4e, 0x0f, 0x00, 0x00, 0x00, 0x00,
                       0x00, 0xc0, 0x00, 0x10, 0x00, 0xc5, 0x03, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                       0x00, 0x00, 0x00, 0xc0, 0x00, 0x10, 0xff, 0x00, 0xc9, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03,
                       0xe8, 0xda, 0xc0, 0x00, 0x10, 0xff, 0x00, 0xca, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xe8,
                       0x19, 0xc0, 0x00, 0x10, 0xff, 0x00, 0xcb, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xe8, 0x00,
                       0x00, 0x00, 0x00, 0xc0, 0x00, 0x10, 0xff, 0x00, 0xcc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03,
                       0xe8, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x00, 0x10, 0xff, 0x00, 0xb6, 0x00, 0x00, 0x00, 0x00, 0x00,
                       0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x00, 0x10, 0xff, 0x00, 0xb5, 0x00, 0x00, 0x00,
                       0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x00, 0x10, 0xff, 0x00, 0xb7, 0x00,
                       0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xc0, 0x00, 0x10, 0xff, 0x00,
                       0xb8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xc0, 0x00, 0x10,
                       0xff, 0x00, 0x3d, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xe8, 0x00, 0x00, 0x03, 0xe8, 0xc0,
                       0x00, 0x10, 0xff, 0x00, 0x51, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xe8, 0x00, 0x00, 0x03,
                       0xe8, 0xc0, 0x00, 0x10, 0xff, 0x00, 0xa1, 0x00, 0x00, 0x00, 0x02, 0x00, 0x1e, 0xda, 0x70, 0x00,
                       0xc0, 0x00, 0x10, 0xff, 0x00, 0xa2, 0x00, 0x00, 0x00, 0x02, 0x00, 0x1e, 0xda, 0x70, 0x00, 0xc0,
                       0x00, 0x10, 0xff, 0x00, 0xa3, 0x00, 0x00, 0x00, 0x02, 0x00, 0x1e, 0xda, 0x70, 0x00, 0x00, 0x00,
                       0x00, 0xc0, 0x00, 0x10, 0xff, 0x00, 0xa4, 0x00, 0x00, 0x00, 0x02, 0x00, 0x1e, 0xda, 0x70, 0x00,
                       0x00, 0x00, 0x00, 0xc0, 0x00, 0x10, 0xff, 0x00, 0xa5, 0x00, 0x00, 0x00, 0x02, 0x00, 0x1e, 0xda,
                       0x70, 0x00, 0x00, 0x00, 0x0b, 0xc0, 0x00, 0x10, 0xff, 0x00, 0xa6, 0x00, 0x00, 0x00, 0x02, 0x00,
                       0x1e, 0xda, 0x70, 0x00, 0x00, 0x00, 0x0b, 0xc0, 0x00, 0x10, 0xff, 0x00, 0xa7, 0x00, 0x00, 0x00,
                       0x02, 0x00, 0x1e, 0xda, 0x70, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x00, 0x10, 0xff, 0x00, 0xa8, 0x00,
                       0x00, 0x00, 0x02, 0x00, 0x1e, 0xda, 0x70, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x00, 0x10, 0xff, 0x00,
                       0xa9, 0x00, 0x00, 0x00, 0x02, 0x00, 0x1e, 0xda, 0x70, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x00, 0x10,
                       0xff, 0x00, 0xaa, 0x00, 0x00, 0x00, 0x02, 0x00, 0x1e, 0xda, 0x70, 0x00, 0x00, 0x00, 0x00, 0xc0,
                       0x00, 0x10, 0xff, 0x00, 0xab, 0x00, 0x00, 0x00, 0x02, 0x00, 0x1e, 0xda, 0x70, 0x00, 0x00, 0x00,
                       0x00, 0xc0, 0x00, 0x10, 0xff, 0x00, 0xac, 0x00, 0x00, 0x00, 0x02, 0x00, 0x1e, 0xda, 0x70, 0x00,
                       0x00, 0x00, 0x00, 0xc0, 0x00, 0x10, 0xff, 0x00, 0xad, 0x00, 0x00, 0x00, 0x02, 0x00, 0x1e, 0xda,
                       0x70, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x00, 0x10, 0xff, 0x00, 0x29, 0x00, 0x00, 0x00, 0x02, 0x00,
                       0x1e, 0xda, 0x70, 0x00, 0x00, 0x0b, 0xb8, 0xc0, 0x00, 0x10, 0xff, 0x00, 0x29, 0x00, 0x00, 0x00,
                       0x04, 0x00, 0x3d, 0xad, 0x10, 0x00, 0x00, 0x0f, 0xa0, 0xc0, 0x00, 0x10, 0xff, 0x00, 0xa1, 0x00,
                       0x00, 0x00, 0x04, 0x00, 0x3d, 0xa9, 0x28, 0x00};


/* define all of the buffers used in other files */
__volatile struct buffer pbuf, uart0rx, uart0tx, uart1rx, uart1tx;
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

extern void echo_test();

int main(void) {
    /* stop watchdog timer */
	WDTCTL = WDTPW | WDTHOLD;

	/* unlock changes to registers/ports, etc. */
	PM5CTL0 &= ~LOCKLPM5;

	/* initialize the board */
    initializeGpios();

    /* set up uart */
    echo_test();
    uart_init();

    /* set up watchdog */
    watchdog_init();

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
            /* TODO: handle event for packet from hercules */
            /* clear event when done */
            loop_flags ^= FLAG_UART0_RX_PACKET;
        }
        if (loop_flags & FLAG_UART1_RX_PACKET) {
            /* copy over the bytes into a processing buffer */
            pbuf.used = uart1rx.idx;
            /* reset uart1rx */
            /* TODO: potential race conditions here. is it possible for more bytes to come in while we copy old ones? */
            uart1rx.idx = 0;
            memcpy(pbuf.buf, uart1rx.buf, pbuf.used);
            pbuf.idx = 0;
            /* parse the packet */
            parse_ground_cmd(pbuf);
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
            P1OUT ^= BIT0;
            uart1_tx_nonblocking(sizeof(packet), packet);
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
