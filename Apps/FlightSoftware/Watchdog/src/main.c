#include <msp430.h>
#include <stdlib.h>

#include "include/buffer.h"
#include "include/uart.h"
#include "include/bsp.h"
#include "include/adc.h"
#include "include/flags.h"
#include "include/i2c.h"
#include "include/ip_udp.h"
#include "include/watchdog.h"


/* define all of the buffers used in other files */
__volatile struct buffer pbuf, uart0rx, uart0tx, uart1rx, uart1tx, hercbuf;
__volatile uint16_t loop_flags;


/**
 * main.c
 */

enum rover_state rovstate;

void enterMode(enum rover_state newstate) {
    switch (newstate) {
    case RS_SLEEP:
        /* fallthrough to keepalive */
    case RS_SERVICE:
        /* service is same as keep alive */
        // TODO: what even is enableBatteries
        enableBatteries();
        /* service mode is a temporary step transition to mission mode */
    case RS_KEEPALIVE:
        /* power everything off and set resets */
        setRadioReset();
        setFPGAReset();
        setMotorsReset();
        setHerculesReset();
        powerOffFpga();
        powerOffMotors();
        powerOffRadio();
        powerOffHercules();
        fuelGaugeLowPower();
        /* TODO: do we want to do it in this order? */

        /* turn off voltage rails */
        disable3V3PowerRail();
        disable24VPowerRail();
        disableBatteries();
        /* monitor only lander voltages */
        adc_setup_lander();
        enableHeater();
        break;
    case RS_MISSION:
        /* bootup process - enable all rails */
        enable3V3PowerRail();
        enable24VPowerRail();
        enableBatteries();

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
        /* TODO: do we want to do it in this order? */

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

	// initialize buffers
	hercbuf.idx = 0;
	hercbuf.used = 0;

	/* initialize the board */
    initializeGpios();

    /* set up uart */
    uart_init();

    /* set up watchdog */
    watchdog_init();

    /* set up the ADC */
    adc_init();

    /* set up i2c */
    i2c_init();

    /* enter service mode */
//    enterMode(RS_SERVICE);
    // enter mission mode for justin debug
    enterMode(RS_MISSION);

    // TODO: camera switch is for debugging only
    fpgaCameraSelectHi();


    __bis_SR_register(GIE); // Enable all interrupts

    __delay_cycles(1000000); //pause for ~1/8 sec for fuel gauge i2c to init, TODO: may be able to remove this now
    initializeFuelGauge();

    ipudp_send_packet("hello, world!\r\n", 15);

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
            unsigned int i = 0, process_len = 0;
            i = 0;
            // header is 8 bytes long
            while (i + 8 <= uart0rx.idx) {

                /* check input value */

                if (uart0rx.buf[i] == 0x0B && uart0rx.buf[i + 1] == 0xB0 &&
                        uart0rx.buf[i + 2] == 0x21) {
                    /* magic value rx'd! check parity */
                    uint8_t parity = 0xDC; /* sum of 0x21, 0xB0, and 0x0B */
                    /* skip parity byte (i + 3) in summation */
                    parity += uart0rx.buf[i + 4] + uart0rx.buf[i + 5];
                    parity += uart0rx.buf[i + 6] + uart0rx.buf[i + 7];
                    /* bitwise NOT to compute parity */
                    parity = ~parity;

                    if (parity == uart0rx.buf[i + 3]) {
                        /* parity bytes match! */
                        process_len = watchdog_handle_hercules(uart0rx.buf, uart0rx.idx - i);
                        if (process_len == 0) {
                            //need more data
                            break;
                        } else {
                            // successfully handled a packet, now process the next one
                            i += process_len;
                            continue;
                        }
                    }
                }
                i++;
            }

            // leftovers
            if (i == 0) {
                // skip the null memcpy
            } else if (i < uart0rx.idx) {
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
            /* clear event */
            loop_flags ^= FLAG_UART1_RX_PACKET;
            // re-enable uart1 interrupt
            UCA1IE |= UCRXIE;
            /* parse the packet */
            parse_ground_cmd(pbuf);
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
            updateGaugeReadings(); // reads current measurements and tells fuel gauge to update them

            switch (rovstate) {
            case RS_SERVICE:
                send_earth_heartbeat();
                watchdog_monitor();
                break;
            case RS_KEEPALIVE:
                /* send heartbeat with collected data */
                send_earth_heartbeat();
                /* TODO: heater checks */
                heaterControl(); // calculate PWM duty cycle (if any) to apply to heater
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
            if (rovstate == RS_KEEPALIVE)
                /* we can only really enable the heaters if we're connected to the lander */
                enableHeater();

            /* clear event when done */
            loop_flags ^= FLAG_TEMP_LOW;
        }
        if (loop_flags & FLAG_TEMP_HIGH) {
            if (rovstate == RS_KEEPALIVE)
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
