#include <msp430.h>
#include <stdlib.h>
#include <string.h>

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
extern uint8_t heating;
uint8_t lastHeater = 0;
uint8_t heatingControlEnabled = 1;

// @suppress("Invalid arguments")

/* function definitions in ground_cmd.c */
void parse_ground_cmd(struct buffer *pp);
void send_earth_heartbeat();

/**
 * main.c
 */

enum rover_state rovstate;

void uart1_disable();
void uart0_init();

void enterMode(enum rover_state newstate) {
    switch (newstate) {
    case RS_SLEEP:
        /* fallthrough to keepalive */
    case RS_SERVICE:
        /* service is same as keep alive */
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
//        fuelGaugeLowPower();
        /* TODO: do we want to do it in this order? */

        /* turn off voltage rails */
        disable3V3PowerRail();
        disable24VPowerRail();
//        disableBatteries();
        /* monitor only lander voltages */
        adc_setup_lander();
        enableBatteries();      // need enable batteries to read from fuel gauge
        enableHeater();
        break;
    case RS_MISSION:
        /* bootup process - enable all rails */
        enable3V3PowerRail();
        enable24VPowerRail();
        enableBatteries();

        /* swap uart's */
//        uart1_disable(); // enabled: 0.0467
        uart0_init();

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

//        __delay_cycles(1234567); //give fuel gauge time to start up
//        initializeFuelGauge();
        releaseMotorsReset();
        /* TODO: do we want to do it in this order? */

        break;
    case RS_FAULT:
        /* TODO: fault mode; enable everything in lander mode */
        break;
    }
    rovstate = newstate;
}

uint16_t ticks;

int main(void) {
    /* stop watchdog timer */
	WDTCTL = WDTPW | WDTHOLD;

	/* unlock changes to registers/ports, etc. */
	PM5CTL0 &= ~LOCKLPM5;

	// initialize buffers
	hercbuf.idx = 0;
	hercbuf.used = 0;
	ticks = 0;

	/* initialize the board */
    initializeGpios();

    /* set up uart clock */
    clock_init();

    /* set up uart */
    //uart0_init();
    uart1_init();

    /* set up watchdog */
    watchdog_init();

    /* set up the ADC */
    adc_init();

    /* set up i2c */
    i2c_init();

    /* enter keepalive mode */
    enterMode(RS_KEEPALIVE);

    // TODO: camera switch is for debugging only
    fpgaCameraSelectHi();

    __bis_SR_register(GIE); // Enable all interrupts

    // TODO: debug
    ipudp_send_packet("hello, world!\r\n", 15); // @suppress("Invalid arguments")

    // the core structure of this program is like an event loop
    while (1) {
        ticks++;
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
                        process_len = watchdog_handle_hercules(uart0rx.buf, uart0rx.idx - i);  // @suppress("Invalid arguments")
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
                memcpy(uart0rx.buf, uart0rx.buf + i, uart0rx.idx - i);  // @suppress("Invalid arguments")
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
            memcpy(pbuf.buf, uart1rx.buf, pbuf.used);  // @suppress("Invalid arguments")
            pbuf.idx = 0;
            /* clear event */
            loop_flags ^= FLAG_UART1_RX_PACKET;
            // re-enable uart1 interrupt
            UCA1IE |= UCRXIE;
            /* parse the packet */
            parse_ground_cmd(&pbuf); // @suppress("Invalid arguments")
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
            case RS_SERVICE:
                send_earth_heartbeat();
                if (heatingControlEnabled) heaterControl();
                watchdog_monitor();
                break;
            case RS_KEEPALIVE:
                /* send heartbeat with collected data */
                send_earth_heartbeat();
                if (heatingControlEnabled) heaterControl(); // calculate PWM duty cycle (if any) to apply to heater
                break;
            case RS_MISSION:
                /* check for kicks from devices and reset misbehaving things */
//                updateGaugeReadings();
                send_earth_heartbeat();
                watchdog_monitor();
                break;
            case RS_FAULT:
                /* sad :( */
                break;
            }

            /* clear event when done */
            loop_flags ^= FLAG_TIMER_TICK;
        }
        if (lastHeater ^ heating) {
            if (heating) {
                enableHeater();
            } else {
                disableHeater();
            }
        }
        lastHeater = heating;
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
