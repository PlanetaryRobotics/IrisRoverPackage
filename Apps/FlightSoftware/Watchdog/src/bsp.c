#include "include/bsp.h"
#include "include/i2c_sensors.h"
#include "include/blimp.h"

// TODO uncomment to program MC
//#define PROGRAM_MOTOR_CONTROLLERS

#define PORT1_ENABLED 1
#define PORT2_ENABLED 1
#define PORT3_ENABLED 1
#define PORT4_ENABLED 1
#define PORTJ_ENABLED 1

uint8_t heaterStatus;
uint8_t hasDeployed;

static uint8_t ioExpanderPort0OutputValues = 0x00u;
static uint8_t ioExpanderPort1OutputValues = 0x00u;

/**
 * @brief      Initializes the gpios.
 */
void initializeGpios()
{
    //########################################################################
    // P1 configuration.
    //########################################################################
#if PORT1_ENABLED
    // Start with all as GPIO output (which is the recommended default config for unused pins)
    P1DIR = 0xFFu;
    P1OUT = 0x00u;
    P1SEL0 = 0x00u;
    P1SEL1 = 0x00u;

    // P1.0 is connected to ground, and is used as VeREF-. VeREF- is an input for a negative reference voltage to the
    // ADC. This is the tertiary function (SEL0 and SEL1 are 1).
    // Per MSP430FR599x datasheet (https://www.ti.com/lit/ds/symlink/msp430fr5994.pdf) Table 9-20,
    // for VeREF- P1DIR is don't care.
    P1SEL0 |= BIT0;
    P1SEL1 |= BIT0;

    // P1.1 is connected to 3.3V rail, and is used as VeREF+. VeREF+ is an input for a positive reference voltage to the
    // ADC. This is the tertiary function (SEL1 and SEL0 are 1).
    // Per MSP430FR599x datasheet (https://www.ti.com/lit/ds/symlink/msp430fr5994.pdf) Table 9-20,
    // for VeREF+ P1DIR is don't care.
    P1SEL0 |= BIT1;
    P1SEL1 |= BIT1;

    // P1.2 is connected to the CHARGE_STAT1 signal (i.e. STAT1 pin of BQ24650RVAR charge controller), and is used as a
    // GPIO input.
    P1DIR &= ~BIT2;

    // P1.3 is connected to the WD_INT signal and is used as a GPIO input.
    //!< @todo What is WD_INT actually useful for, if anything? If nothing, make this GPIO output.
    P1DIR &= ~BIT3;

    // P1.4 is connected to the V_LANDER_SENS signal (output of voltage divider for measuring lander voltage being
    // supplied to us), and is used as an ADC analog input (specifically it is ADC analog input A4). This is the
    // tertiary function (SEL1 and SEL0 are 1).
    // Per MSP430FR599x datasheet (https://www.ti.com/lit/ds/symlink/msp430fr5994.pdf) Table 9-21,
    // for A4 P1DIR is don't care.
    P1SEL0 |= BIT4;
    P1SEL1 |= BIT4;

    // ? TODO: Why is this here twice? Repeat of above?
    // P1.4 is connected to the V_LANDER_SENS signal (output of voltage divider for measuring lander voltage being
    // supplied to us), and is used as an ADC analog input (specifically it is ADC analog input A4). This is the
    // tertiary function (SEL1 and SEL0 are 1).
    // Per MSP430FR599x datasheet (https://www.ti.com/lit/ds/symlink/msp430fr5994.pdf) Table 9-21,
    // for A4 P1DIR is don't care.
    P1SEL0 |= BIT4;
    P1SEL1 |= BIT4;

    // P1.5 is connected to the V_LANDER_REG_EN signal (control signal for MOSFET to enable voltage regulator that
    // regulates lander voltage down to the voltage to be used as battery charger input voltage), and is used as a GPIO
    // output that is initially low.
    P1OUT &= ~BIT5;

    // P1.6 is used as the I2C (UCB0) data line (SDA). This is the secondary function (SEL1 is 1 and SEL0 is 0).
    // Per MSP430FR599x datasheet (https://www.ti.com/lit/ds/symlink/msp430fr5994.pdf) Table 9-22,
    // for UCB0SDA P1DIR is don't care.
    P1SEL1 |= BIT6;

    // P1.7 is used as the I2C (UCB0) clock line (SCL). This is the secondary function (SEL1 is 1 and SEL0 is 0).
    // Per MSP430FR599x datasheet (https://www.ti.com/lit/ds/symlink/msp430fr5994.pdf) Table 9-22,
    // for UCB0SCL P1DIR is don't care.
    P1SEL1 |= BIT7;

    //P1OUT &= ~(BIT4 | BIT5); // Initially everything is off

#ifndef PROGRAM_MOTOR_CONTROLLERS
    //P1DIR |= BIT4;  // P1.4 output Motor control reset B
    //P1DIR |= BIT5;  // P1.5 output Motor control reset C
#endif
#endif // PORT1_ENABLED
    //########################################################################
    // P2 configuration.
    //########################################################################
#if PORT2_ENABLED
    // Start with all as GPIO output (which is the recommended default config for unused pins)
    P2DIR = 0x00u; // is now input, was 0xFFu
    P2OUT = 0x00u;
    P2SEL0 = 0x00u;
    P2SEL1 = 0x00u;

    // P2.0 is used as the UART0 (UCA0) TX data line (TXD). This is the secondary function (SEL1 is 1 and SEL0 is 0).
    // Per MSP430FR599x datasheet (https://www.ti.com/lit/ds/symlink/msp430fr5994.pdf) Table 9-23,
    // for UCA0TXD P2DIR is don't care.
    P2SEL1 |= BIT0; // comment out if UART line killing things

    // P2.1 is used as the UART0 (UCA0) RX data line (RXD). This is the secondary function (SEL1 is 1 and SEL0 is 0).
    // Per MSP430FR599x datasheet (https://www.ti.com/lit/ds/symlink/msp430fr5994.pdf) Table 9-23,
    // for UCA0RXD P2DIR is don't care.
    P2SEL1 |= BIT1;

    // P2.2 is connected to the Heater signal (control signal for MOSFET to enable heater), and is used as a GPIO
    // output with an initially low value.
    P2DIR |= BIT2;
    P2OUT &= ~BIT2;

    // P2.3 is connected to the BATT_CTRL_EN signal (control signal for MOSFET to enable battery controller circuit),
    // and is used as a GPIO output. This line has an external HW pull up so it is preferred to go to high-Z state
    // rather than pulling the line high. From BLiMP schematics:
    //
    // "For safety reasons (so the WD doesn't accidentally reset it on the surface if there's a
    // persistent memory fault), WD should *always* boot with BATT_CTRL_EN as Hi-Z and only set it LOW (to reduce power
    // draw from lander) by a command and *not* save this state persistently. Any time WD reboots, BATT_CTRL_EN should
    // go Hi-Z or HIGH."
    //
    // Even though this pin is essentially an output, to get high-Z state we configure it as input with pullup/pulldown
    // resistors disabled.
    P2DIR &= ~BIT3;
    P2REN &= ~BIT3;

    // P2.4 is connected to the BATT_TEMP signal (output of voltage divider for battery pack thermistor), and is used
    // as an ADC analog input (specifically it is ADC analog input A7). This is the tertiary function (SEL1 and SEL0
    // are 1).
    // Per MSP430FR599x datasheet (https://www.ti.com/lit/ds/symlink/msp430fr5994.pdf) Table 9-24,
    // for A7 P2DIR is don't care.
    P2SEL0 |= BIT4;
    P2SEL1 |= BIT4;

    // P2.5 is used as the UART1 (UCA1) TX data line (TXD). This is the secondary function (SEL1 is 1 and SEL0 is 0).
    // Per MSP430FR599x datasheet (https://www.ti.com/lit/ds/symlink/msp430fr5994.pdf) Table 9-25,
    // for UCA1TXD P2DIR is don't care.
    P2SEL1 |= BIT5;

    // P2.6 is used as the UART1 (UCA1) RX data line (RXD). This is the secondary function (SEL1 is 1 and SEL0 is 0).
    // Per MSP430FR599x datasheet (https://www.ti.com/lit/ds/symlink/msp430fr5994.pdf) Table 9-25,
    // for UCA1RXD P2DIR is don't care.
    P2SEL1 |= BIT6;

    // P2.7 is connected to the PG_1.2 signal (1.2V "power good"), and is used as a GPIO input. The "power good"
    // is normally pulled up to 3.3V by an external pullup resistor, but when the 1.2V rail power exits a "good" state
    // (e.g. goes to high or low) then the regulator chip will pull the signal down. We want to interrupt on that
    // falling edge so that we can react appropriately.
    //!< @todo Make sure interrupt is eventually enabled and implemented.
    P2DIR &= ~BIT7;
    P2REN &= ~BIT7;

    // P2 configuration
    //P2DIR &= 0x00;  // All bits as input
    //P2OUT &= ~(BIT2 | BIT3 | BIT4); // Initially everything is off
    //P2DIR |= BIT2;  // P2.2 output Heater

#ifndef PROGRAM_MOTOR_CONTROLLERS
    //P2DIR |= BIT3;  // P2.3 output Motor control reset A
#endif

    //P2DIR |= BIT4;  // P2.4 output Radio ON
    //P2DIR &= ~BIT7;  // P2.7 input Power good 1V2

    //P2SEL0 |= BIT2; // P2.2 heater output, PWM mode
#endif // PORT2_ENABLED
    //########################################################################
    // P3 configuration.
    //########################################################################
#if PORT3_ENABLED
    // Start with all as GPIO output (which is the recommended default config for unused pins)
    P3DIR = 0xFFu;
    P3OUT = 0x00u;
    P3SEL0 = 0x00u;
    P3SEL1 = 0x00u;

    // P3.0 is connected to the BATT_RT signal (output of battery temperature thermistor), and is used
    // as an ADC analog input (specifically it is ADC analog input A12). This is the tertiary function (SEL1 and SEL0
    // are 1).
    // Per MSP430FR599x datasheet (https://www.ti.com/lit/ds/symlink/msp430fr5994.pdf) Table 9-27,
    // for A12 P3DIR is don't care.
    P3SEL0 |= BIT0;
    P3SEL1 |= BIT0;

    // P3.1 is connected to the V_SYS_ALL_SENS signal (output of voltage divider based on V_SYS_ALL on the BLiMP), and
    // is used as an ADC analog input (specifically it is ADC analog input A13). This is the tertiary function (SEL1 and
    // SEL0 are 1).
    // Per MSP430FR599x datasheet (https://www.ti.com/lit/ds/symlink/msp430fr5994.pdf) Table 9-27,
    // for A13 P3DIR is don't care.
    P3SEL0 |= BIT1;
    P3SEL1 |= BIT1;

    // P3.2 is connected to the I_SYS_ALL_SENS signal (output of voltage divider that allows a measure of the current
    // by all circuitry running on V_SYS_ALL on the BLiMP), and is used as an ADC analog input (specifically it is ADC
    // analog input A14). This is the tertiary function (SEL1 and SEL0 are 1).
    // Per MSP430FR599x datasheet (https://www.ti.com/lit/ds/symlink/msp430fr5994.pdf) Table 9-27,
    // for A14 P3DIR is don't care.
    P3SEL0 |= BIT2;
    P3SEL1 |= BIT2;

    // P3.3 is connected to the V_BATT_SENS signal (output of voltage divider that based on V_BATT on the BLiMP), and
    // is used as an ADC analog input (specifically it is ADC analog input A15). This is the tertiary function (SEL1 and
    // SEL0 are 1).
    // Per MSP430FR599x datasheet (https://www.ti.com/lit/ds/symlink/msp430fr5994.pdf) Table 9-27,
    // for A15 P3DIR is don't care.
    P3SEL0 |= BIT3;
    P3SEL1 |= BIT3;

    // P3.4 is connected to the Deployment signal (control signal for MOSFET to enable HDRM), and is used as a GPIO
    // output with an initially low value.
    P3OUT &= ~BIT4;

    // P3.5 is connected to the FPGA_Kick signal and is used as a GPIO input.
    P3DIR &= ~BIT5;

    // ! TODO: Not deployment (anymore on Rev I - not sure if it was on rev H). Is `LATCH_BATT` now.
    // P3.6 is connected to the Deployment signal (control signal for MOSFET to enable HDRM), and is used as a GPIO
    // output with an initially low value.
    P3OUT &= ~BIT6;

    // P3.7 is connected to the 3V3_EN signal (control signal for MOSFET to enable HDRM), and is used as a GPIO
    // output with an initially low value.
    P3OUT &= ~BIT7;

    // P3 configuration
    //P3DIR &= 0x00;
    //P3OUT &= ~(BIT1 | BIT2 | BIT3 | BIT4 | BIT5 | BIT6 | BIT7); // Initially everything is off
    //P3DIR &= ~BIT0;  // P3.0 input battery thermistor
    //P3DIR |= BIT1; // P3.1 output Hercules POR
    //P3DIR |= BIT2; // P3.2 output Hercules Reset
    //P3DIR |= BIT3; // P3.3 output Radio Reset
    //P3DIR |= BIT4; // P3.4 output deployment
    //P3DIR |= BIT5; // P3.5 output FPGA camera select
    //P3DIR |= BIT6; // P3.6 output FPGA reset
    //P3DIR |= BIT7; // P3.7 output 3V3 enable
#endif // PORT3_ENABLED
    //########################################################################
    // P4 configuration.
    //########################################################################
#if PORT4_ENABLED
    // Start with all as GPIO output (which is the recommended default config for unused pins)
    P4DIR = 0xFFu;
    P4OUT = 0x00u;
    P4SEL0 = 0x00u;
    P4SEL1 = 0x00u;

    // P4.0 is connected to the VCC_2.5 signal (2.5V rail), and is used as an ADC analog input (specifically it is ADC
    // analog input A8). This is the tertiary function (SEL1 and SEL0 are 1).
    // Per MSP430FR599x datasheet (https://www.ti.com/lit/ds/symlink/msp430fr5994.pdf) Table 9-29,
    // for A8 P4DIR is don't care.
    P4SEL0 |= BIT0;
    P4SEL1 |= BIT0;

    // P4.1 is connected to the VCC_2.8 signal (2.8V rail), and is used as an ADC analog input (specifically it is ADC
    // analog input A9). This is the tertiary function (SEL1 and SEL0 are 1).
    // Per MSP430FR599x datasheet (https://www.ti.com/lit/ds/symlink/msp430fr5994.pdf) Table 9-29,
    // for A9 P4DIR is don't care.
    P4SEL0 |= BIT1;
    P4SEL1 |= BIT1;

    // ! TODO: 24V and 28V sense are backwards. P4.2 is 28V. P4.3 is 24V.
    // P4.2 is connected to a voltage divider on the 24V rail, and is used as an ADC analog input (specifically it is
    // ADC analog input A10). This is the tertiary function (SEL1 and SEL0 are 1).
    // Per MSP430FR599x datasheet (https://www.ti.com/lit/ds/symlink/msp430fr5994.pdf) Table 9-29,
    // for A10 P4DIR is don't care.
    P4SEL0 |= BIT2;
    P4SEL1 |= BIT2;

    // ! TODO: 24V and 28V sense are backwards. P4.2 is 28V. P4.3 is 24V.
    // P4.3 is connected to a voltage divider on the 28V rail, and is used as an ADC analog input (specifically it is
    // ADC analog input A11). This is the tertiary function (SEL1 and SEL0 are 1).
    // Per MSP430FR599x datasheet (https://www.ti.com/lit/ds/symlink/msp430fr5994.pdf) Table 9-29,
    // for A11 P4DIR is don't care.
    P4SEL0 |= BIT3;
    P4SEL1 |= BIT3;

    // P4.4 is connected to the PG_1.8 signal (1.8V "power good"), and is used as a GPIO input. The "power good"
    // is normally pulled up to 3.3V by an external pullup resistor, but when the 1.8V rail power exits a "good" state
    // (e.g. goes to high or low) then the regulator chip will pull the signal down. We want to interrupt on that
    // falling edge so that we can react appropriately.
    //!< @todo Make sure interrupt is eventually enabled and implemented.
    P4DIR &= ~BIT4;
    P4REN &= ~BIT4;

    // P4.5 is connected to the PG_3.3 signal (3.3V "power good"), and is used as a GPIO input. The "power good"
    // is normally pulled up to 3.3V by an external pullup resistor, but when the 3.3V rail power exits a "good" state
    // (e.g. goes to high or low) then the regulator chip will pull the signal down. We want to interrupt on that
    // falling edge so that we can react appropriately.
    //!< @todo Make sure interrupt is eventually enabled and implemented.
    P4DIR &= ~BIT5;
    P4REN &= ~BIT5;

    // P4.6 is connected to the BMS_ALRT signal (battery monitoring system alert), and is used as a GPIO input.
    //!< @todo Determine how BMS_ALRT behaves.
    //!< @todo Make sure interrupt is eventually enabled and implemented.
    P4DIR &= ~BIT6;
    P4REN &= ~BIT6;

    // P4.7 is connected to the PG_5.0 signal (5.0V "power good"), and is used as a GPIO input. The "power good"
    // is normally pulled up to 5.0V by an external pullup resistor, but when the 5.0V rail power exits a "good" state
    // (e.g. goes to high or low) then the regulator chip will pull the signal down. We want to interrupt on that
    // falling edge so that we can react appropriately.
    //!< @todo Make sure interrupt is eventually enabled and implemented.
    P4DIR &= ~BIT7;
    P4REN &= ~BIT7;
#endif // PORT4_ENABLED
    /*
    // P4 configuration
    P4DIR &= 0x00;
    P4OUT |= BIT6; // Initially everything is off (NOTE: 24V0 OFF = HI)
    P4DIR &= ~BIT4; // P4.4 input power good input (1V8)
    P4DIR &= ~BIT5; // P4.5 input power good input (3V3)
    P4DIR |= BIT6; // P4.6 output 24V0 enable
    P4DIR &= ~BIT7; // P4.7 input power good input (5V0)

    P4SEL0 |= BIT0; // P4.0 Analog input 8 (VCC 2V5)
    P4SEL1 |= BIT0; // P4.0 Analog input 8 (VCC 2V5)

    P4SEL0 |= BIT1; // P4.1 Analog input 9 (VCC 2V8)
    P4SEL1 |= BIT1; // P4.1 Analog input 9 (VCC 2V8)

    P4SEL0 |= BIT2; // P4.2 Analog input 10 (VCC 28V0)
    P4SEL1 |= BIT2; // P4.2 Analog input 10 (VCC 28V0)

    P4SEL0 |= BIT3; // P4.3 Analog input 11 (VCC 24V0)
    P4SEL1 |= BIT3; // P4.3 Analog input 11 (VCC 24V0)

    P3SEL0 |= BIT0; // P3.0 Analog input 12 (BATT_RT (battery temperature))
    P3SEL1 |= BIT0; // P3.0 Analog input 12 (BATT_RT (battery temperature))
    // Analog input configuration done in adc_init()
    */

    // RAD TODO - PJ config?
    //########################################################################
    // P4 configuration.
    //########################################################################
#if PORTJ_ENABLED
    // Start with all as GPIO output (which is the recommended default config for unused pins)
    PJDIR = 0xFFu;
    PJOUT = 0x00u;
    PJSEL0 = 0x00u;
    PJSEL1 = 0x00u;

    // PJ.0 is connected to the Hercules_ON signal and is used as a GPIO output with an initially low value.
    PJOUT &= ~BIT0;

    // PJ.1 is connected to the FPGA_ON signal and is used as a GPIO output with an initially low value.
    PJOUT &= ~BIT1;

    // PJ.2 is connected to the Motor_ON signal and is used as a GPIO output with an initially low value.
    PJOUT &= ~BIT2;

    // PJ.3 is connected to the CHRG_EN signal and is used as a GPIO output with an initially low value.
    // TODO: CHRG_EN should be High-Z or low always.
    PJDIR &= ~BIT3;
    PJREN &= ~BIT3;

    // TODO: Looks like this is still being set as an output though it should be an input.
    // PJ.4 is connected to the Radio_Kick signal and is used as a GPIO input.
    PJDIR &= ~BIT4;

    // PJ.5 is connected to the BATTERY_EN signal and is used as a GPIO output with an initially low value.
    PJOUT &= ~BIT5;

    // TODO: Looks like this is still being set as an output though it should be an input.
    // ^- TODO: looks like this has been fixed?
    // PJ.6 is connected to the BATT_STAT signal and is used as a GPIO input.
    PJDIR &= ~BIT6;

    // PJ.7 is connected to the V_SYS_ALL_EN signal and is used as a GPIO output with an initially low value.
    // TODO: VSAE has an external pull-down and VSA switch seems to draw more current when VSAE is driven low (maybe? - retest this), so consider making VSAE always HiZ or High.
    PJOUT &= ~BIT7;
#endif // PORTJ_ENABLED
    /*
    // PJ configuration
    PJOUT &= ~(BIT0 | BIT1 | BIT2 | BIT3 | BIT4 | BIT5); // Initially everything is off
    PJDIR &= 0x00;
    PJDIR |= BIT0; // PJ.0 output Hercules ON
    PJDIR |= BIT1; // PJ.1 output FPGA ON
    PJDIR |= BIT2; // PJ.2 output MOTORS ON
    PJDIR |= BIT3; // PJ.3 CHRG output 0 = not charging
#ifndef PROGRAM_MOTOR_CONTROLLERS
    PJDIR |= BIT4; // PJ.4 output Motor control reset D
#endif
    PJDIR |= BIT5; // PJ.5 output BATTERY
    */

    // RAD TODO - what are all these? only pins left I see on schematic that weren't set above are DVCC, etc
    // Initialize all unused ports to as GPIO output to prevent floating pins. (Per slau367p section 12.3.2)
    P5SEL0 = 0x00u;
    P5SEL1 = 0x00u;
    P5DIR = 0xFFu;
    P5OUT = 0x00u;

    P6SEL0 = 0x00u;
    P6SEL1 = 0x00u;
    P6DIR = 0xFFu;
    P6OUT = 0x00u;

    P7SEL0 = 0x00u;
    P7SEL1 = 0x00u;
    P7DIR = 0xFFu;
    P7OUT = 0x00u;

    P8SEL0 = 0x00u;
    P8SEL1 = 0x00u;
    P8DIR = 0xFFu;
    P8OUT = 0x00u;

    // Initial statuses
    heaterStatus = 0;
    hasDeployed = 0;
}

/**
 * @brief      Enables the heater. (HI = ON)
 *      TB0CCTL2 is register that toggles PWM output on heater pin
 */
inline void enableHeater()
{
    P2OUT |= BIT2;
    heaterStatus = 1;
}

/**
 * @brief      Disables the heater. (LO = OFF)
 */
inline void disableHeater()
{
    P2OUT &= ~BIT2;
    heaterStatus = 0;
}

/**
 * @brief      Enables the 3.3 v power rail. (HI = ON)
 */
inline void enable3V3PowerRail()
{
    P3OUT |= BIT7;
}

/**
 * @brief      Disables the 3.3 v power rail. (LOW = OFF)
 */
inline void disable3V3PowerRail()
{
    P3OUT &= ~BIT7;
}

// RAD TODO - this is for V_SYS_ALL_EN now (24v w/Motor_ON - PJ.2)
/**
 * @brief      Enables the 24 v power rail. (high = ON)
 */
inline void enable24VPowerRail()
{
    PJOUT |= BIT7;
}

/**
 * @brief      Disables the 24 v power rail. (LOW = OFF)
 */
inline void disable24VPowerRail()
{
    PJOUT &= ~BIT7;
}

/**
 * @brief      Releases a hercules reset. (HI = NORMAL)
 */
inline void releaseHerculesReset()
{
    //!< @todo CHANGE THIS HIGH PRIORITY! This is controlled through I/O expander now
    //P3OUT |= BIT1 | BIT2;
    ioExpanderPort0OutputValues |= I2C_SENSORS__IOE_P0_BIT__N_HERUCLES_RST;
    ioExpanderPort0OutputValues |= I2C_SENSORS__IOE_P0_BIT__N_HERCULES_PORRST;
}

/**
 * @brief      Sets the hercules reset. (LO = RESET)
 */
inline void setHerculesReset()
{
    //!< @todo CHANGE THIS HIGH PRIORITY! This is controlled through I/O expander now
    //P3OUT &= ~(BIT1 | BIT2);
    ioExpanderPort0OutputValues &= ~I2C_SENSORS__IOE_P0_BIT__N_HERUCLES_RST;
    ioExpanderPort0OutputValues &= ~I2C_SENSORS__IOE_P0_BIT__N_HERCULES_PORRST;
}

/**
 * @brief      Releases a radio reset. (HI = NORMAL)
 */
inline void releaseRadioReset()
{
    //!< @todo CHANGE THIS HIGH PRIORITY! This is controlled through I/O expander now
    //P3OUT |= BIT3;
    ioExpanderPort1OutputValues |= I2C_SENSORS__IOE_P1_BIT__N_RADIO_RST;
}

/**
 * @brief      Sets the radio reset. (LO = RESET)
 */
inline void setRadioReset()
{
    //!< @todo CHANGE THIS HIGH PRIORITY! This is controlled through I/O expander now
    //P3OUT &= ~BIT3;
    ioExpanderPort1OutputValues &= ~I2C_SENSORS__IOE_P1_BIT__N_RADIO_RST;
}

/**
 * @brief      Releases a fpga reset. (HI = NORMAL)
 */
inline void releaseFPGAReset()
{
    //!< @todo CHANGE THIS HIGH PRIORITY! This is controlled through I/O expander now
    //P3OUT |= BIT6;
    ioExpanderPort0OutputValues |= I2C_SENSORS__IOE_P0_BIT__N_FPGA_RST;
}

/**
 * @brief      Sets the fpga reset. (LO = RESET)
 */
inline void setFPGAReset()
{
    //!< @todo CHANGE THIS HIGH PRIORITY! This is controlled through I/O expander now
    //P3OUT &= ~BIT6;
    ioExpanderPort0OutputValues &= ~I2C_SENSORS__IOE_P0_BIT__N_FPGA_RST;
}

/**
 * @brief      Select camera 1 in FPGA
 */
inline void fpgaCameraSelectHi()
{
    //!< @todo This was going to the line labeled FPGA_Kick which appears to be a watchdog input, not output... wrong?
    //P3OUT |= BIT5;
}

/**
 * @brief      Select camera 0 in FPGA
 */
inline void fpgaCameraSelectLo()
{
    //!< @todo This was going to the line labeled FPGA_Kick which appears to be a watchdog input, not output... wrong?
    //P3OUT &= ~BIT5;
}

/**
 * @brief      Releases the motor resets. (HI = NORMAL)
 */
inline void releaseMotor1Reset()
{
#ifndef PROGRAM_MOTOR_CONTROLLERS
    // RAD TODO - remove these ifndef I think
    //!< @todo CHANGE THIS HIGH PRIORITY! This is controlled through I/O expander now
    //P2OUT |= BIT3;
    ioExpanderPort0OutputValues |= I2C_SENSORS__IOE_P0_BIT__MC_RST_A;
#endif
}
inline void releaseMotor2Reset()
{
#ifndef PROGRAM_MOTOR_CONTROLLERS
    //!< @todo CHANGE THIS HIGH PRIORITY! This is controlled through I/O expander now
    //P1OUT |= BIT4;
    ioExpanderPort0OutputValues |= I2C_SENSORS__IOE_P0_BIT__MC_RST_B;
#endif
}
inline void releaseMotor3Reset()
{
#ifndef PROGRAM_MOTOR_CONTROLLERS
    //!< @todo CHANGE THIS HIGH PRIORITY! This is controlled through I/O expander now
    //P1OUT |= BIT5;
    ioExpanderPort0OutputValues |= I2C_SENSORS__IOE_P0_BIT__MC_RST_C;
#endif
}
inline void releaseMotor4Reset()
{
#ifndef PROGRAM_MOTOR_CONTROLLERS
    //!< @todo CHANGE THIS HIGH PRIORITY! This is controlled through I/O expander now
    //PJOUT |= BIT4;
    ioExpanderPort0OutputValues |= I2C_SENSORS__IOE_P0_BIT__MC_RST_D;
#endif
}
inline void releaseMotorsReset()
{
#ifndef PROGRAM_MOTOR_CONTROLLERS
    //!< @todo CHANGE THIS HIGH PRIORITY! This is controlled through I/O expander now
    //P1OUT |= BIT4 | BIT5;
    //P2OUT |= BIT3;
    //PJOUT |= BIT4;
    ioExpanderPort0OutputValues |= I2C_SENSORS__IOE_P0_BIT__MC_RST_A;
    ioExpanderPort0OutputValues |= I2C_SENSORS__IOE_P0_BIT__MC_RST_B;
    ioExpanderPort0OutputValues |= I2C_SENSORS__IOE_P0_BIT__MC_RST_C;
    ioExpanderPort0OutputValues |= I2C_SENSORS__IOE_P0_BIT__MC_RST_D;
#endif
}

/**
 * @brief      Sets the motors to reset. (LO = RESET)
 * @todo CONFIRM THIS IS ACTUALLY NEGATED (i.e. reset is active when low)
 */
inline void setMotor1Reset()
{
#ifndef PROGRAM_MOTOR_CONTROLLERS
    //!< @todo CHANGE THIS HIGH PRIORITY! This is controlled through I/O expander now
    //P2OUT &= ~BIT3;
    ioExpanderPort0OutputValues &= ~I2C_SENSORS__IOE_P0_BIT__MC_RST_A;
#endif
}
inline void setMotor2Reset()
{
#ifndef PROGRAM_MOTOR_CONTROLLERS
    //!< @todo CHANGE THIS HIGH PRIORITY! This is controlled through I/O expander now
    //P1OUT &= ~BIT4;
    ioExpanderPort0OutputValues &= ~I2C_SENSORS__IOE_P0_BIT__MC_RST_B;
#endif
}
inline void setMotor3Reset()
{
#ifndef PROGRAM_MOTOR_CONTROLLERS
    //!< @todo CHANGE THIS HIGH PRIORITY! This is controlled through I/O expander now
    //P1OUT &= ~BIT5;
    ioExpanderPort0OutputValues &= ~I2C_SENSORS__IOE_P0_BIT__MC_RST_C;
#endif
}
inline void setMotor4Reset()
{
#ifndef PROGRAM_MOTOR_CONTROLLERS
    //!< @todo CHANGE THIS HIGH PRIORITY! This is controlled through I/O expander now
    //PJOUT &= ~BIT4;
    ioExpanderPort0OutputValues &= ~I2C_SENSORS__IOE_P0_BIT__MC_RST_D;
#endif
}
inline void setMotorsReset()
{
#ifndef PROGRAM_MOTOR_CONTROLLERS
    //!< @todo CHANGE THIS HIGH PRIORITY! This is controlled through I/O expander now
    //P1OUT &= ~(BIT4 | BIT5);
    //P2OUT &= ~BIT3;
    //PJOUT &= ~BIT4;
    ioExpanderPort0OutputValues &= ~I2C_SENSORS__IOE_P0_BIT__MC_RST_A;
    ioExpanderPort0OutputValues &= ~I2C_SENSORS__IOE_P0_BIT__MC_RST_B;
    ioExpanderPort0OutputValues &= ~I2C_SENSORS__IOE_P0_BIT__MC_RST_C;
    ioExpanderPort0OutputValues &= ~I2C_SENSORS__IOE_P0_BIT__MC_RST_D;
#endif
}
/**
 * @brief      Power the hercules MCU (HI = ON)
 */
inline void powerOnHercules()
{
    PJOUT |= BIT0;
}

/**
 * @brief      Power off the hercules MCU (LO = OFF)
 */
inline void powerOffHercules()
{
    PJOUT &= ~BIT0;
}

/**
 * @brief      Power on the Radio (HI = ON)
 */
inline void powerOnRadio()
{
    //!< @todo CHANGE THIS HIGH PRIORITY! This is controlled through I/O expander now
    //P2OUT |= BIT4;
    ioExpanderPort1OutputValues |= I2C_SENSORS__IOE_P1_BIT__RADIO_ON; // ! TODO - KEEP
//    ioExpanderPort1OutputValues = 0x00; // TODO - REMOVE
//    ioExpanderPort0OutputValues = 0x00; // TODO - REMOVE
}

/**
 * @brief      Power off the Radio (LO = OFF)
 */
inline void powerOffRadio()
{
    //!< @todo CHANGE THIS HIGH PRIORITY! This is controlled through I/O expander now
    //P2OUT &= ~BIT4;
    ioExpanderPort1OutputValues &= ~I2C_SENSORS__IOE_P1_BIT__RADIO_ON;
}

/**
 * @brief      Power on the FPGA (HI = ON)
 */
inline void powerOnFpga()
{
    PJOUT |= BIT1;
}

/**
 * @brief      Power off the FPGA (LO = OFF)
 */
inline void powerOffFpga()
{
    PJOUT &= ~BIT1;
}

/**
 * @brief      Power on the motors (HI = ON)
 */
inline void powerOnMotors()
{
    PJOUT |= BIT2;
}

/**
 * @brief      Power off the motors (LO = OFF)
 */
inline void powerOffMotors()
{
    PJOUT &= ~BIT2;
}

/**
 * @brief      Enable the batteries
 */
inline void enableBatteries()
{
    // Turn on battery enable:
    blimp_battEnOn();
    // Make the latch absorb the BE state:
    blimp_latchBattUpdate();
}

/**
 * @brief      Disable the batteries
 */
inline void disableBatteries()
{
    // Turn off battery enable:
    blimp_battEnOff();
    // Make the latch absorb the BE state:
    blimp_latchBattUpdate();
}

/**
 * @brief      Deploy the rover from the lander
 */
inline void setDeploy()
{
    hasDeployed = 1;
    P3OUT |= BIT4;
}

/**
 * @brief      Un-set deploy from lander
 */
inline void unsetDeploy()
{
    P3OUT &= ~BIT4;
}

/**
 * @brief      Start charging batteries from lander power
 */
inline void startChargingBatteries()
{
    // Turn on batteries before charging if not on yet:
    // (not safe to start charging on an open circuit)
    if(!blimp_batteryState()){
        blimp_battEnOn();
    }
    // Enable the charging regulator:
    blimp_regEnOn();
    // Start charging:
    blimp_chargerEnOn();
}

/**
 * @brief      Stop charging batteries
 */
inline void stopChargingBatteries()
{
    // Stop charging:
    blimp_chargerEnOff();
    // Disable the charging regulator:
    blimp_regEnOff();
}

inline uint8_t getIOExpanderPort0OutputValue()
{
    return ioExpanderPort0OutputValues;
}

inline uint8_t getIOExpanderPort1OutputValue()
{
    return ioExpanderPort1OutputValues;
}
