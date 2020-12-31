#include <msp430.h>

#include "include/buffer.h"
#include "include/uart.h"
#include "include/bsp.h"
#include "include/adc.h"
#include "include/flags.h"
#include "include/ip_udp.h"
#include "include/watchdog.h"

/* define all of the buffers used in other files */
__volatile struct buffer pbuf, uart0rx, uart0tx, uart1rx, uart1tx;
__volatile uint16_t loop_flags;

/**
 * main.c
 */

enum rover_state rovstate;

void enterMode(enum rover_state newstate) {
    switch (newstate) {
    case RS_LANDER:
        /* monitor only lander voltages */
        adc_setup_lander();
        break;
    case RS_MISSION:
        /* bootup process - enable all rails */
        enable3V3PowerRail();
        enable24VPowerRail();

        /* start monitoring only mission-relevant voltages */
        adc_setup_lander();

        /* power everything on and release resets */
        powerOnHercules();
        releaseHerculesReset();
        powerOnFpga();
        powerOnMotors();
        powerOnRadio();
        releaseRadioReset();
        releaseFPGAReset();
        releaseMotorsReset();
        /* TODO: do we want to do it this way? */

        break;
    case RS_FAULT:
        /* TODO: fault mode; enable everything in lander mode */
        break;
    }
    rovstate = newstate;
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
    watchdog_init();

    /* set up the ADC */
    adc_init();

    /* enter the lander mode */
    // TODO: do NOT enter mission mode right away...
    enterMode(RS_MISSION);

    // TODO: camera switch is for debugging only
    fpgaCameraSelectHi();

    __bis_SR_register(GIE); // Enable all interrupts

    // the core structure of this program is like an event loop
    while (1) {
        /* check if anything happened */
        if (!loop_flags) { /* nothing did */
            /* go back to low power mode */
            __bis_SR_register(GIE);
            continue;
        }

        /* a cool thing happened! now time to check what it was */
        if (loop_flags & FLAG_UART0_RX_PACKET) {
            // temporarily disable uart0 interrupt
            UCA0IE &= ~UCRXIE;
            int i = 0, len = 0;
            while (i + 4 <= uart0rx.idx) {
                /* stroke rx'd */
                if (uart0rx.buf[i] == 0x0 && uart0rx.buf[i + 1] == 0x0) {
                    // no special commands
                    // TODO: is this right? maybe want i + 3.
                    handle_watchdog_reset_cmd(uart0rx.buf[i + 2]);
                    // echo back watchdog command
                    uart0_tx_nonblocking(uart0rx.idx, uart0rx.buf);
                } else {
                    len = (uart0rx.buf[i]) | (uart0rx.buf[i + 1] << 8);
                    if (len + 4 > uart0rx.idx) {
                        // need to wait for more bytes to come in
                        break;
                    } else {
                        // TODO: actually parse udp instead of skipping it
                        i += len;
                    }
                }
                i += 4;
            }

            // leftovers
            if (i < uart0rx.idx) {
                // copy over leftovers to front of buffer
                memcpy(uart0rx.buf, uart0rx.buf + i, uart0rx.idx - i);
                uart0rx.idx = uart0rx.idx - i;
            } else {
                // no leftovers
                uart0rx.idx = 0;
            }

            // re-enable uart0 interrupt
            UCA0IE |= UCRXIE;

            /* clear event when done */
            loop_flags ^= FLAG_UART0_RX_PACKET;
        }
        if (loop_flags & FLAG_UART1_RX_PACKET) {
            // temporarily disable uart1 interrupt
            UCA1IE &= ~UCRXIE;
            /* copy over the bytes into a processing buffer */
            pbuf.used = uart1rx.idx;
            /* reset uart1rx */
            uart1rx.idx = 0;
            /* copy over uart1rx buffer into processing buffer */
            memcpy(pbuf.buf, uart1rx.buf, pbuf.used);
            pbuf.idx = 0;
            // re-enable uart1 interrupt
            UCA1IE |= UCRXIE;
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
            /* handle event for heartbeat */
            /* always sample the ADC for temperature and voltage levels */
            adc_sample();

            switch (rovstate) {
            case RS_LANDER:
                /* send heartbeat with collected data */
                send_earth_heartbeat();
                /* TODO: heater checks */
                break;
            case RS_MISSION:
                /* check for kicks from devices and reset misbehaving things */
                watchdog_monitor();
                break;
            case RS_FAULT:
                /* sad :( */
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
