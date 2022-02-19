#include <msp430.h>
#include <stdlib.h>
#include <string.h>

#include "include/buffer.h"
#include "include/uart.h"
#include "include/bsp.h"
#include "include/adc.h"
#include "include/flags.h"
#include "include/ip_udp.h"
#include "include/watchdog.h"
#include "include/i2c_sensors.h"
#include "include/ground_cmd.h"


/* define all of the buffers used in other files */
__volatile struct buffer uart0rx, uart0tx, uart1rx, uart1tx;
struct buffer hercbuf;
__volatile uint16_t loop_flags;
extern uint8_t heating;
uint8_t lastHeater = 0;
extern uint8_t heatingControlEnabled;

/**
 * main.c
 */
#pragma PERSISTENT(rovstate)
enum rover_state rovstate = RS_MISSION;

void enterMode(enum rover_state newstate) {
    switch (newstate) {
    case RS_SLEEP:
        /* sleep mode is basically the same thing as keep alive */
    case RS_SERVICE:
        /* service is same as keep alive */
        /* service mode is a temporary step transition to mission mode */
    case RS_KEEPALIVE:
        /* power everything off and set resets */
        powerOffFpga();
        powerOffMotors();
        powerOffRadio();
        powerOffHercules();
        setRadioReset();
        setFPGAReset();
        setMotorsReset();
        setHerculesReset();
        unsetDeploy();

        /* turn off voltage rails */
        disable3V3PowerRail();
        disable24VPowerRail();
        disableBatteries();

        /* monitor only lander voltages */
        adc_setup_lander();
        enableHeater();
        startChargingBatteries();

        break;
    default:
    case RS_MISSION:
        /* bootup process - enable all rails */
        enable3V3PowerRail();
        enable24VPowerRail();
        enableBatteries();
        disableHeater();
        unsetDeploy();

        /* enable hercules uart */
        uart0_init();

        /* power everything on and release resets */
        releaseRadioReset();
        releaseFPGAReset();

        /* start monitoring only mission-relevant voltages */
        adc_setup_mission();

        /* power stuff on */
        powerOnFpga();
        powerOnMotors();
        powerOnRadio();
        stopChargingBatteries();

        __delay_cycles(12345678); //give fuel gauge [50 ms] & wifi [~750 ms] time to start up
//        I2C_Sensors__initializeFuelGaugeBlocking();
        powerOnHercules();
        releaseMotorsReset();
        releaseHerculesReset();

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

    /* set up i2c to read from fuel gauge*/
    I2C_Sensors__init();

    /* enter keepalive mode */
    enterMode(rovstate);

    __bis_SR_register(GIE); // Enable all interrupts

    ipudp_send_packet("hello, world!\r\n", 15); // @suppress("Invalid arguments")

    I2C_Sensors__Readings i2cReadings = { 0 }; // will hold i2c data

    // the core structure of this program is like an event loop
    while (1) {
        ticks++;
        /* watchdog timer setup - need to stroke every ~1s */
        /* basically, we limit the execution of each loop to ~1s or else reset */
        WDTCTL = WDT_ARST_1000;

        /* check if anything happened */
        if (!loop_flags) { /* nothing did */
            /* go back to low power mode */
            __bis_SR_register(GIE);
            continue;
        }

        /* a cool thing happened! now time to check what it was */
        if (loop_flags & FLAG_UART0_RX_PACKET) {
            watchdog_handle_hercules(&i2cReadings); // @suppress("Invalid arguments")

            /* clear event when done */
            loop_flags ^= FLAG_UART0_RX_PACKET;
        }

        if (loop_flags & FLAG_UART1_RX_PACKET) {
            // temporarily disable uart1 rx interrupt
            UCA1IE &= ~UCRXIE;
            uart1rx.used = uart1rx.idx;
            uart1rx.idx = 0;
            parse_ground_cmd(&uart1rx); // @suppress("Invalid arguments")
            loop_flags ^= FLAG_UART1_RX_PACKET;
            // re-enable uart1 interrupt
            UCA1IE |= UCRXIE;
            /* parse the packet */
        }
        if (loop_flags & FLAG_TIMER_TICK) {
            /* handle event for heartbeat */
            /* always sample the ADC for temperature and voltage levels */
            adc_sample();

            switch (rovstate) {
            case RS_SERVICE:
                send_earth_heartbeat(&i2cReadings);
                if (heatingControlEnabled) heaterControl();
                watchdog_monitor();
                break;
            case RS_KEEPALIVE:
                /* send heartbeat with collected data */
                send_earth_heartbeat(&i2cReadings);
                if (heatingControlEnabled) heaterControl(); // calculate PWM duty cycle (if any) to apply to heater
                break;
            case RS_MISSION:
                /* check for kicks from devices and reset misbehaving things */
                send_earth_heartbeat(&i2cReadings);
                watchdog_monitor();

                // Initiate gauge readings here, the rest of the actions to do in this state every state
                // will be done after gauge readings complete, which is monitored in the
                // FLAG_I2C_GAUGE_READING_ACTIVE loop_flags block below.
//                I2C_Sensors__initiateGaugeReadings();
//                loop_flags |= FLAG_I2C_GAUGE_READING_ACTIVE;

                break;
            case RS_FAULT:
                /* sad :( */
                break;
            }

            /* clear event when done */
            loop_flags ^= FLAG_TIMER_TICK;
        }

        // NOTE: will never enter this conditional with:
        //         " loop_flags |= FLAG_I2C_GAUGE_READING_ACTIVE; "
        //       commented out
        if (loop_flags & FLAG_I2C_GAUGE_READING_ACTIVE) {
            I2C_Sensors__spinOnce();

            I2C_Sensors__Status stat = I2C_Sensors__getGaugeReadingStatus(&i2cReadings);

            int done = (I2C_SENSORS__STATUS__INCOMPLETE != stat);

            if (done) {
                loop_flags ^= FLAG_I2C_GAUGE_READING_ACTIVE;
            }
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
