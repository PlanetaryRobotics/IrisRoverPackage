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
#include "include/blimp.h"


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
//#pragma PERSISTENT(rovstate)
enum rover_state rovstate = RS_KEEPALIVE;

void enterMode(enum rover_state newstate) {
    switch (newstate) {
    case RS_SLEEP:
        /* sleep mode is basically the same thing as keep alive */
    case RS_SERVICE:
        /* service is same as keep alive */
        /* service mode is a temporary step transition to mission mode */
        /***
         * ! TODO: Check Service vs KeepAlive distinction. Service should have
         *  higher bandwidth (more detailed and more frequent heartbeats) and
         *  only Service should allow power-on commands and enabling UART0 to
         *  Hercules.
         */
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

        disable3V3PowerRail(); // Comment out for programming Motor Controllers
//        enable3V3PowerRail(); // TESTING - for Programming Motor Controllers!
//        powerOnMotors(); // TESTING [added] - for Programming Motor Controllers!

        /* monitor only lander voltages */
        adc_setup_lander();
//        enableHeater();
        disableHeater();

        //!< @todo Check return statuses
        I2C_Sensors__initializeIOExpanderBlocking();
        I2C_Sensors__writeIOExpanderOutputsBlocking(getIOExpanderPort0OutputValue(), getIOExpanderPort1OutputValue());

        blimp_normalBoot(); // must happen after IO expander initialization
//        blimp_safeBoot(); // TODO: only for testing.

        // BLiMP controls that must happen after BLiMP boot function (blimp_safeBoot() or blimp_normalBoot()):
//        startChargingBatteries();
        stopChargingBatteries();

        // Disable all system power (VSA) switch:
        blimp_vSysAllEnOff();

        //TODO - enable/disable24VPowerRail() sets V_SYS_ALL_EN
        //       24V power actually set by powerOnMotors()
        //       V_SYS_ALL used to power everything besides Heater & WD + peripherals
//        disable24VPowerRail();
        // TODO: **ONLY** disable batteries on boot **IF** we haven't deployed (lander voltage present).
        disableBatteries();

//        uint8_t chargeStat2 = 0;
//        uint8_t latchStat = 0;
//        I2C_Sensors__Status stat = I2C_Sensors__readIOExpanderBlocking(&chargeStat2, &latchStat);

//        fpgaCameraSelectLo();
//#define MOTOR_TEST
#ifdef MOTOR_TEST
        blimp_normalBoot();
        blimp_vSysAllEnOn();
        enable3V3PowerRail();
        powerOnMotors();
//        setHerculesReset();
//        releaseHerculesReset();
//        releaseRadioReset();
//        powerOnRadio();
        I2C_Sensors__writeIOExpanderOutputsBlocking(getIOExpanderPort0OutputValue(), getIOExpanderPort1OutputValue());
#endif

//#define HERC_TEST
#ifdef HERC_TEST
        enable3V3PowerRail();
        powerOnHercules();
//        setHerculesReset();
        releaseHerculesReset();
//        releaseRadioReset();
//        powerOnRadio();
//        I2C_Sensors__writeIOExpanderOutputsBlocking(getIOExpanderPort0OutputValue(), getIOExpanderPort1OutputValue());
#endif

        // TODO: remove this block
//#define WD_GPO_TEST
#ifdef WD_GPO_TEST
        __delay_cycles(12345678); // arbitrary. lets I2C ISRs happen for initialization
        blimp_vSysAllEnOn();
//        PJOUT |= BIT7; // VSAE ON
        blimp_battEnOn();
//        PJOUT |= BIT5; // BE ON
        blimp_chargerEnForceHigh();
//        PJOUT |= BIT3; // To Check out SBC
//        PJOUT |= BIT3; // CE ON
        blimp_bctrlEnForceHigh();
//        P2DIR |= BIT3; // To Check out SBC
//        P2OUT |= BIT3; // BCTRLE ON
        blimp_latchBattOn();
//        P3OUT |= BIT6; // Latch_batt ON

        P3DIR |= BIT5; // FPGA_KICK OUTPUT
        P3OUT |= BIT5; // FPGA_KICK ON

        P3OUT |= BIT4; // DEPLOY_1 ON

        enableHeater();
        blimp_latchSetHigh();
        blimp_latchResetHigh();
        blimp_regEnOn();
        I2C_Sensors__writeIOExpanderOutputsBlocking(getIOExpanderPort0OutputValue(), getIOExpanderPort1OutputValue());
#endif

//#define BLiMP
#ifdef BLiMP
        // *** vSysAllEn
//        blimp_vSysAllEnOn();
        blimp_vSysAllEnOff();

        // *** SetChargerEn
//        blimp_chargerEnOn();
//        blimp_chargerEnForceHigh();
        blimp_chargerEnOff();

        // *** SetChargerPowerConnection
//        blimp_regEnOn();
        blimp_regEnOff();

        // *** SetBatteryConnection
//        blimp_battEnOn();
        blimp_battEnOff();

        // *** SetBatteryControlEnable
//        blimp_bctrlEnOn();
//        blimp_bctrlEnForceHigh();
        blimp_bctrlEnOff();

        // *** SetBatteryLatch
//        blimp_latchBattOn();blimp_latchBattOn();
        blimp_latchBattOff();
//        blimp_latchBattUpdate();

#endif


        break;
    default:
    case RS_MISSION:
        //TODO - will need V_SYS_ALL_EN before anything else can work
        // ! TODO - changed this case a lot to disable for testing

        blimp_normalBoot(); // must happen after IO expander initialization

        // Turn on all system power (VSA) switch:
        blimp_vSysAllEnOn();

        /* bootup process - enable all rails */
        enable3V3PowerRail();
//        enable24VPowerRail();
////        disable24VPowerRail(); // TESTING
//        enableBatteries();
        disableBatteries(); // TESTING
        disableHeater();
        unsetDeploy();
//        setDeploy(); // TESTING


        /* enable hercules uart */
        uart0_init();

        /* power everything on and release resets */
        releaseRadioReset();
//        setRadioReset(); // TESTING
        setFPGAReset(); // TESTING
//        releaseFPGAReset();

        /* start monitoring only mission-relevant voltages */
        adc_setup_mission();

        /* power stuff on */
        powerOffFpga(); // TESTING
//        powerOnFpga();
        powerOffMotors(); // TESTING
//        powerOnMotors();
        powerOnRadio();
//        powerOffRadio(); // TESTING

        stopChargingBatteries();

        // !TODO : release RadioReset() and powerOnRadio happen at exact same time... This is a useless spot to set the outputs...
        // Wondering if it's why sometimes the power goes weird when entering mission but power off herc, power off radio, power on radio, power on herc works...
        I2C_Sensors__writeIOExpanderOutputsBlocking(getIOExpanderPort0OutputValue(), getIOExpanderPort1OutputValue());

        __delay_cycles(12345678); //give fuel gauge [50 ms] & wifi [~750 ms] time to start up
        I2C_Sensors__initializeFuelGaugeBlocking();

        powerOnHercules();
//        powerOffHercules(); // TESTING

        setMotorsReset(); // TESTING
//        releaseMotorsReset();

        releaseHerculesReset();
//        setHerculesReset(); // TESTING

        I2C_Sensors__writeIOExpanderOutputsBlocking(getIOExpanderPort0OutputValue(), getIOExpanderPort1OutputValue());

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

    /* initialize the board */
    initializeGpios();

	/* unlock changes to registers/ports, etc. */
	PM5CTL0 &= ~LOCKLPM5;

	// initialize buffers
	hercbuf.idx = 0;
	hercbuf.used = 0;
	ticks = 0;

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

    /* boot into default mode */
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
                I2C_Sensors__initiateGaugeReadings();
                loop_flags |= FLAG_I2C_GAUGE_READING_ACTIVE;

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
