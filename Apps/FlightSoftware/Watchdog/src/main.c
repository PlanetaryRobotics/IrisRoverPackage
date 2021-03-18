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
__volatile struct buffer pbuf, uart0rx, uart0tx, uart1rx, uart1tx;
__volatile struct small_buffer i2crx, i2ctx;
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
        // [DEBUG] CONNOR/WHOMEVER ELSE - THIS LINE RIGHT HERE, IF COMMENTED OUT, WILL KEEP THE MOTOR CONTROLLERS DISABLED
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

void DEBUG_SETUP() {
    enableBatteries();
    enable3V3PowerRail(); // we added this
    enable24VPowerRail(); // we added this
    //disable24VPowerRail();
    //enable3V3PowerRail();
    powerOnHercules(); // we turned this on (uncommented)
    //releaseHerculesReset();
    //powerOnFpga();
    powerOnMotors();// we turned this on (uncommented) (that's all ;)
    // not really sure what version this is tbh
    // this is an old version; can i swtich workspaces real quick?
    //
    //powerOffMotors();
    //powerOnRadio();
    //releaseRadioReset();
    //releaseFPGAReset();
    //releaseMotorsReset();
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


    /* set up i2c */
    i2c_init();
    __delay_cycles(1000000); //pause for ~1/8 sec for fuel gauge i2c to init
    initializeFuelGauge();

// [DEBUG] from fuel gauge, all set now but leaving until it's fully integrated -J
    //    readBatteryVoltage();

//    while(1){
//        readBatteryVoltage();
//        readBatteryCurrent();
//        readGaugeTemp();
//        readBatteryCharge();
//        __delay_cycles(1234567);
//    }


    // the core structure of this program is like an event loop
    while (1) {

            // camera hack
                 P3OUT |= BIT5; // P3.5 output camera select


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
            unsigned int i = 0, len = 0;
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
                        len = (uart0rx.buf[i + 4]) | (uart0rx.buf[i + 5] << 8);
                        if (len) {
                            /* udp packet */
                            if (len + 8 > uart0rx.idx) {
                                // TODO: parse UDP
                                /* copy over the bytes into a processing buffer */
                                pbuf.used = len;
                                /* copy over uart1rx buffer into processing buffer */
                                memcpy(pbuf.buf, uart0rx.buf + 8, pbuf.used);
                                pbuf.idx = 0;
                                i += len + 8;
                                /* echo back watchdog command */
                                uart0_tx_nonblocking(uart0rx.idx, uart0rx.buf);
                            } else {
                                /* need to wait for more bytes to come in */
                                break;
                            }
                        } else {
                            /* handle watchdog reset command */
                            handle_watchdog_reset_cmd(uart0rx.buf[i + 6]);
                            /* echo back watchdog command */
                            uart0_tx_nonblocking(8, uart0rx.buf + i);
                            /* skip past the width of a watchdog command */
                            i += 8;
                            /* also send telemetry */
                            //uart0_tx_nonblocking();
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
