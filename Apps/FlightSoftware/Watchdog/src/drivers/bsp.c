#include "drivers/bsp.h"
#include "comms/i2c_sensors.h"
#include "flags.h"

#include <stddef.h>

// uncomment to program MC
//#define PROGRAM_MOTOR_CONTROLLERS

#define PORT1_ENABLED 1
#define PORT2_ENABLED 1
#define PORT3_ENABLED 1
#define PORT4_ENABLED 1
#define PORTJ_ENABLED 1

static uint8_t ioExpanderPort0OutputValues = 0;
static uint8_t ioExpanderPort1OutputValues = 0;

static PersistentState* persistentStatePtr = NULL;

/**
 * @brief      Initializes the gpios.
 */
void initializeGpios(PersistentState* pState)
{
    DEBUG_LOG_NULL_CHECK_RETURN(pState, "Parameter is NULL", /* nothing, b/c void return */);
    persistentStatePtr = pState;

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

    // P1.5 is connected to the V_LANDER_REG_EN signal (control signal for MOSFET to enable voltage regulator that
    // regulates lander voltage down to the voltage to be used as battery charger input voltage), and is used as a GPIO
    // output that is initially low (unless persistent status says it should be high)
    if (persistentStatePtr->m_statusBits & PSBI_MASK(PSBI__V_LANDER_REG_EN)) {
        P1OUT |= BIT5;
    } else {
        P1OUT &= ~BIT5;
    }

    // P1.6 is used as the I2C (UCB0) data line (SDA). This is the secondary function (SEL1 is 1 and SEL0 is 0).
    // Per MSP430FR599x datasheet (https://www.ti.com/lit/ds/symlink/msp430fr5994.pdf) Table 9-22,
    // for UCB0SDA P1DIR is don't care.
    P1SEL1 |= BIT6;

    // P1.7 is used as the I2C (UCB0) clock line (SCL). This is the secondary function (SEL1 is 1 and SEL0 is 0).
    // Per MSP430FR599x datasheet (https://www.ti.com/lit/ds/symlink/msp430fr5994.pdf) Table 9-22,
    // for UCB0SCL P1DIR is don't care.
    P1SEL1 |= BIT7;

#endif // PORT1_ENABLED
    //########################################################################
    // P2 configuration.
    //########################################################################
#if PORT2_ENABLED
    // Start with all as GPIO input. Unused pins are supposed to be configured as outputs, but due to current draw
    // issues with P2.0, we initialize all as inputs so that they are high-z and don't allow current draw. We configure
    // all other pins in Port2 below anyway, so this default doesn't really matter for anything other than P2.0, which
    // we currently don't set.
    P2DIR = 0x00u;
    P2OUT = 0x00u;
    P2SEL0 = 0x00u;
    P2SEL1 = 0x00u;

    // P2.0 is used as the UART0 (UCA0) TX data line (TXD). This is the secondary function (SEL1 is 1 and SEL0 is 0).
    // Per MSP430FR599x datasheet (https://www.ti.com/lit/ds/symlink/msp430fr5994.pdf) Table 9-23,
    // for UCA0TXD P2DIR is don't care.
    //
    // TODO >> Don't set this here, as we see large current draw when this mode is set and Hercules is off. <<
    //
    //P2SEL1 |= BIT0;

    // P2.1 is used as the UART0 (UCA0) RX data line (RXD). This is the secondary function (SEL1 is 1 and SEL0 is 0).
    // Per MSP430FR599x datasheet (https://www.ti.com/lit/ds/symlink/msp430fr5994.pdf) Table 9-23,
    // for UCA0RXD P2DIR is don't care.
    P2SEL1 |= BIT1;

    // P2.2 is connected to the Heater signal (control signal for MOSFET to enable heater), and is used as a GPIO
    // output with an initially low value. We always set this low here, and handle persisting the heater state after
    // GPIO initialization
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
    // Even though this pin is essentially an ouptut, to get high-Z state we configure it as input with pullup/pulldown
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
    // output with an initially low value unless persistent state was high
    if (persistentStatePtr->m_statusBits & PSBI_MASK(PSBI__DEPLOYMENT)) {
        P3OUT |= BIT4;
    } else {
        P3OUT &= ~BIT4;
    }

    // P3.5 is connected to the FPGA_Kick signal and is used as a GPIO input.
    P3DIR &= ~BIT5;

    // P3.6 is connected to the LATCH_BATT signal, and is used as a GPIO output with an initially low value
    P3OUT &= ~BIT6;

    // P3.7 is connected to the 3V3_EN signal (control signal for MOSFET to enable HDRM), and is used as a GPIO
    // output with an initially low value (unless persistent state was high)
    if (persistentStatePtr->m_statusBits & PSBI_MASK(PSBI__3V3_EN)) {
        P3OUT |= BIT7;
    } else {
        P3OUT &= ~BIT7;
    }

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

    // P4.2 is connected to a voltage divider on the 28V rail, and is used as an ADC analog input (specifically it is
    // ADC analog input A10). This is the tertiary function (SEL1 and SEL0 are 1).
    // Per MSP430FR599x datasheet (https://www.ti.com/lit/ds/symlink/msp430fr5994.pdf) Table 9-29,
    // for A10 P4DIR is don't care.
    P4SEL0 |= BIT2;
    P4SEL1 |= BIT2;

    // P4.3 is connected to a voltage divider on the 24V rail, and is used as an ADC analog input (specifically it is
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
    //########################################################################
    // PJ configuration.
    //########################################################################
#if PORTJ_ENABLED
    // Start with all as GPIO output (which is the recommended default config for unused pins)
    PJDIR = 0xFFu;
    PJOUT = 0x00u;
    PJSEL0 = 0x00u;
    PJSEL1 = 0x00u;

    // PJ.0 is connected to the Hercules_ON signal and is used as a GPIO output with an initially low value (unless
    // persistent state was high)
    if (persistentStatePtr->m_statusBits & PSBI_MASK(PSBI__HERCULES_ON)) {
        PJOUT |= BIT0;
    } else {
        PJOUT &= ~BIT0;
    }
    // PJ.1 is connected to the FPGA_ON signal and is used as a GPIO output with an initially low value (unless
    // persistent state was high)
    if (persistentStatePtr->m_statusBits & PSBI_MASK(PSBI__FPGA_ON)) {
        PJOUT |= BIT1;
    } else {
        PJOUT &= ~BIT1;
    }

    // PJ.2 is connected to the Motor_ON signal and is used as a GPIO output with an initially low value (unless
    // persistent state was high)
    if (persistentStatePtr->m_statusBits & PSBI_MASK(PSBI__MOTOR_ON)) {
        PJOUT |= BIT2;
    } else {
        PJOUT &= ~BIT2;
    }

    // PJ.3 is connected to the CHRG_EN signal and is used as a GPIO output with an initially high-Z value (unless
    // persistent state was low). CHRG_EN should always be high-Z (when charging is enabled) or low.
    if (persistentStatePtr->m_statusBits & PSBI_MASK(PSBI__CHRG_EN_FORCE_HIGH)) {
        PJDIR |= BIT3;
        PJOUT |= BIT3;
    } else if (persistentStatePtr->m_statusBits & PSBI_MASK(PSBI__CHRG_EN)) {
        PJDIR &= ~BIT3;
        PJREN &= ~BIT3;
    } else {
        PJDIR |= BIT3;
        PJOUT &= ~BIT3;
    }

    // PJ.4 is connected to the Radio_Kick signal and is used as a GPIO input.
    PJDIR &= ~BIT4;

    // PJ.5 is connected to the BATTERY_EN signal and is used as a GPIO output with an initially low value (unless
    // persistent state was high)
    if (persistentStatePtr->m_statusBits & PSBI_MASK(PSBI__BATTERY_EN)) {
        PJOUT |= BIT5;
    } else {
        PJOUT &= ~BIT5;
    }

    // PJ.6 is connected to the BATT_STAT signal and is used as a GPIO input.
    PJDIR &= ~BIT6;

    // PJ.7 is connected to the V_SYS_ALL_EN signal and is used as a GPIO output with an initially high-Z value (unless
    // persistent state was high). V_SYS_ALL_EN should always be high-Z (when V_SYS_ALL disabled) or high.
    if (persistentStatePtr->m_statusBits & PSBI_MASK(PSBI__V_SYS_ALL_EN_FORCE_LOW)) {
        PJDIR |= BIT7;
        PJOUT &= ~BIT7;
    } else if (persistentStatePtr->m_statusBits & PSBI_MASK(PSBI__V_SYS_ALL_EN)) {
        PJDIR |= BIT7;
        PJOUT |= BIT7;
    } else {
        PJDIR &= ~BIT7;
        PJREN &= ~BIT7;
    }

#endif // PORTJ_ENABLED

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
}

void enableUart0Pins(void)
{
    // P2.0 is used as the UART0 (UCA0) TX data line (TXD). This is the secondary function (SEL1 is 1 and SEL0 is 0).
    // Per MSP430FR599x datasheet (https://www.ti.com/lit/ds/symlink/msp430fr5994.pdf) Table 9-23,
    // for UCA0TXD P2DIR is don't care.
    P2SEL1 |= BIT0;

    // P2.1 is used as the UART0 (UCA0) RX data line (RXD). This is the secondary function (SEL1 is 1 and SEL0 is 0).
    // Per MSP430FR599x datasheet (https://www.ti.com/lit/ds/symlink/msp430fr5994.pdf) Table 9-23,
    // for UCA0RXD P2DIR is don't care.
    P2SEL1 |= BIT1;
}

void disableUart0Pins(void)
{
    // To disable the UART0 pins (P2.0 and P2.1) we want them both set as GPIO inputs in order to prevent current flow

    // Set both pins as GPIO simultaneously
    P2SEL1 &= ~(BIT0 | BIT1);
    // This shouldn't be necessary as these should always be zero already, but just to be extra sure I clear these bits
    // too. With this, the pins are GPIO without a shadow of a doubt.
    P2SEL0 &= ~(BIT0 | BIT1);
    // Now that they're GPIO, we need to set them to inputs
    P2DIR &= ~(BIT0 | BIT1);
}

void enableUart1Pins(void)
{
    // P2.5 is used as the UART1 (UCA1) TX data line (TXD). This is the secondary function (SEL1 is 1 and SEL0 is 0).
    // Per MSP430FR599x datasheet (https://www.ti.com/lit/ds/symlink/msp430fr5994.pdf) Table 9-25,
    // for UCA1TXD P2DIR is don't care.
    P2SEL1 |= BIT5;

    // P2.6 is used as the UART1 (UCA1) RX data line (RXD). This is the secondary function (SEL1 is 1 and SEL0 is 0).
    // Per MSP430FR599x datasheet (https://www.ti.com/lit/ds/symlink/msp430fr5994.pdf) Table 9-25,
    // for UCA1RXD P2DIR is don't care.
    P2SEL1 |= BIT6;
}

void disableUart1Pins(void)
{
    // To disable the UART1 pins (P2.6 and P2.6) we want them both set as GPIO inputs in order to prevent current flow

    // Set both pins as GPIO simultaneously
    P2SEL1 &= ~(BIT5 | BIT6);
    // This shouldn't be necessary as these should always be zero already, but just to be extra sure I clear these bits
    // too. With this, the pins are GPIO without a shadow of a doubt.
    P2SEL0 &= ~(BIT5 | BIT6);
    // Now that they're GPIO, we need to set them to inputs
    P2DIR &= ~(BIT5 | BIT6);

}

/**
 * Initialize clocks for UART. necessary and should only be called once, at boot.
 */
void clockInit(void)
{
    // Unlock all CS registers
    CSCTL0_H = CSKEY_H;

    // Sets DCO Range select to high speed
    CSCTL1 = DCORSEL;

    // Sets DCO frequency to 8 MHz.
    // It is 8 MHz because DCORSEL was set; if DCORSEL was not set, this (a DCOFSEL of 3) would set DCO to 4 MHz.
    // Note that DCOFSEL_3 means setting the DCOFSEL bits to the decimal value 3, i.e. DCOFSEL is 011b
    CSCTL1 |= DCOFSEL_3;

    // Sets the source of ACLK to VLOCLK.
    // Per Section 8.12.3.4 (pg 42) of the datasheet, this has a typical frequency of 9.4 kHz
    //    (source: https://www.ti.com/lit/ds/symlink/msp430fr5994.pdf)
    CSCTL2 = SELA__VLOCLK;

    // Sets the source of SMCLK to DCOCLK.
    CSCTL2 |= SELS__DCOCLK;

    // Sets the source of MCLK to DCOCLK.
    CSCTL2 |= SELM__DCOCLK;

    // Sets an input divider of 1 (i.e. no division) for ACLK
    // Note that unlike DCOFSEL_3, DIVA_1 does NOT mean setting the DIVA bits to the decimal value of 1,
    //    but instead means that the input divider is 1 (which means setting the DIVA bits to the decimal
    //    value of 0).
    CSCTL3 = DIVA__1;

    // Sets an input divider of 1 (i.e. no division) for SMCLK
    // Note that unlike DCOFSEL_3, DIVS_1 does NOT mean setting the DIVS bits to the decimal value of 1,
    //    but instead means that the input divider is 1 (which means setting the DIVS bits to the decimal
    //    value of 0).
    CSCTL3 |= DIVS__1;

    // Sets an input divider of 1 (i.e. no division) for MCLK
    // Note that unlike DCOFSEL_3, DIVM_1 does NOT mean setting the DIVM bits to the decimal value of 1,
    //    but instead means that the input divider is 1 (which means setting the DIVM bits to the decimal
    //    value of 0).
    CSCTL3 |= DIVM__1;

    // Lock all CS registers
    CSCTL0_H = 0;
}

/**
 * @brief      Enables the heater. (HI = ON)
 *      TB0CCTL2 is register that toggles PWM output on heater pin
 */
inline void enableHeater(void)
{
    P2OUT |= BIT2;
    persistentStatePtr->m_heaterParams.m_heating = TRUE;
}

/**
 * @brief      Disables the heater. (LO = OFF)
 */
inline void disableHeater(void)
{
    P2OUT &= ~BIT2;
    persistentStatePtr->m_heaterParams.m_heating = FALSE;
}

/**
 * @brief      Enables the 3.3 v power rail. (HI = ON)
 */
inline void enable3V3PowerRail(void)
{
    P3OUT |= BIT7;
    SET_PSBI_IN_STATE_PTR(persistentStatePtr, PSBI__3V3_EN);
}

/**
 * @brief      Disables the 3.3 v power rail. (LOW = OFF)
 */
inline void disable3V3PowerRail(void)
{
    P3OUT &= ~BIT7;
    CLEAR_PSBI_IN_STATE_PTR(persistentStatePtr, PSBI__3V3_EN);
}

// RAD TODO - this is for V_SYS_ALL_EN now (24v w/Motor_ON - PJ.2)
/**
 * @brief      Enables the 24 v power rail. (high = ON)
 */
inline void enable24VPowerRail(void)
{
    PJDIR |= BIT7;
    PJOUT |= BIT7;
    SET_PSBI_IN_STATE_PTR(persistentStatePtr, PSBI__V_SYS_ALL_EN);
}

/**
 * @brief      Disables the 24 v power rail. (LOW = OFF)
 */
inline void disable24VPowerRail(void)
{
    PJDIR &= ~BIT7;
    PJREN &= ~BIT7;
    CLEAR_PSBI_IN_STATE_PTR(persistentStatePtr, PSBI__V_SYS_ALL_EN);
}

/**
 * @brief      Releases a hercules reset. (HI = NORMAL)
 */
inline void releaseHerculesReset(void)
{
    ioExpanderPort0OutputValues |= I2C_SENSORS__IOE_P0_BIT__N_HERUCLES_RST;
    ioExpanderPort0OutputValues |= I2C_SENSORS__IOE_P0_BIT__N_HERCULES_PORRST;

    // Technically this isn't set yet because the I/O expander hasn't been written, but we'll save the state here
    // because calling this means we had the intention of setting these states
    SET_PSBI_IN_STATE_PTR(persistentStatePtr, PSBI__HERCULES_N_RST);
    SET_PSBI_IN_STATE_PTR(persistentStatePtr, PSBI__HERCULES_N_PORRST);
}

/**
 * @brief      Sets the hercules reset. (LO = RESET)
 */
inline void setHerculesReset(void)
{
    ioExpanderPort0OutputValues &= ~I2C_SENSORS__IOE_P0_BIT__N_HERUCLES_RST;
    ioExpanderPort0OutputValues &= ~I2C_SENSORS__IOE_P0_BIT__N_HERCULES_PORRST;

    // Technically this isn't set yet because the I/O expander hasn't been written, but we'll save the state here
    // because calling this means we had the intention of setting these states
    CLEAR_PSBI_IN_STATE_PTR(persistentStatePtr, PSBI__HERCULES_N_RST);
    CLEAR_PSBI_IN_STATE_PTR(persistentStatePtr, PSBI__HERCULES_N_PORRST);
}

/**
 * @brief      Releases a radio reset. (HI = NORMAL)
 */
inline void releaseRadioReset(void)
{
    ioExpanderPort1OutputValues |= I2C_SENSORS__IOE_P1_BIT__N_RADIO_RST;

    // Technically this isn't set yet because the I/O expander hasn't been written, but we'll save the state here
    // because calling this means we had the intention of setting this state
    SET_PSBI_IN_STATE_PTR(persistentStatePtr, PSBI__RADIO_N_RST);
}

/**
 * @brief      Sets the radio reset. (LO = RESET)
 */
inline void setRadioReset(void)
{
    ioExpanderPort1OutputValues &= ~I2C_SENSORS__IOE_P1_BIT__N_RADIO_RST;

    // Technically this isn't set yet because the I/O expander hasn't been written, but we'll save the state here
    // because calling this means we had the intention of setting this state
    CLEAR_PSBI_IN_STATE_PTR(persistentStatePtr, PSBI__RADIO_N_RST);
}

/**
 * @brief      Releases a fpga reset. (HI = NORMAL)
 */
inline void releaseFPGAReset(void)
{
    ioExpanderPort0OutputValues |= I2C_SENSORS__IOE_P0_BIT__N_FPGA_RST;

    // Technically this isn't set yet because the I/O expander hasn't been written, but we'll save the state here
    // because calling this means we had the intention of setting this state
    SET_PSBI_IN_STATE_PTR(persistentStatePtr, PSBI__FPGA_N_RST);
}

/**
 * @brief      Sets the fpga reset. (LO = RESET)
 */
inline void setFPGAReset(void)
{
    ioExpanderPort0OutputValues &= ~I2C_SENSORS__IOE_P0_BIT__N_FPGA_RST;

    // Technically this isn't set yet because the I/O expander hasn't been written, but we'll save the state here
    // because calling this means we had the intention of setting this state
    CLEAR_PSBI_IN_STATE_PTR(persistentStatePtr, PSBI__FPGA_N_RST);
}

/**
 * @brief      Select camera 1 in FPGA
 */
inline void fpgaCameraSelectHi(void)
{
    //!< @todo This was going to the line labeled FPGA_Kick which appears to be a watchdog input, not output... wrong?
    //P3OUT |= BIT5;
}

/**
 * @brief      Select camera 0 in FPGA
 */
inline void fpgaCameraSelectLo(void)
{
    //!< @todo This was going to the line labeled FPGA_Kick which appears to be a watchdog input, not output... wrong?
    //P3OUT &= ~BIT5;
}

/**
 * @brief      Releases the motor resets. (HI = NORMAL)
 */
inline void releaseMotor1Reset(void)
{
#ifndef PROGRAM_MOTOR_CONTROLLERS
    ioExpanderPort0OutputValues |= I2C_SENSORS__IOE_P0_BIT__MC_RST_A;

    // Technically this isn't set yet because the I/O expander hasn't been written, but we'll save the state here
    // because calling this means we had the intention of setting this state
    SET_PSBI_IN_STATE_PTR(persistentStatePtr, PSBI__MC_RST_A);
#endif
}
inline void releaseMotor2Reset(void)
{
#ifndef PROGRAM_MOTOR_CONTROLLERS
    ioExpanderPort0OutputValues |= I2C_SENSORS__IOE_P0_BIT__MC_RST_B;

    // Technically this isn't set yet because the I/O expander hasn't been written, but we'll save the state here
    // because calling this means we had the intention of setting this state
    SET_PSBI_IN_STATE_PTR(persistentStatePtr, PSBI__MC_RST_B);
#endif
}
inline void releaseMotor3Reset(void)
{
#ifndef PROGRAM_MOTOR_CONTROLLERS
    ioExpanderPort0OutputValues |= I2C_SENSORS__IOE_P0_BIT__MC_RST_C;

    // Technically this isn't set yet because the I/O expander hasn't been written, but we'll save the state here
    // because calling this means we had the intention of setting this state
    SET_PSBI_IN_STATE_PTR(persistentStatePtr, PSBI__MC_RST_C);
#endif
}
inline void releaseMotor4Reset(void)
{
#ifndef PROGRAM_MOTOR_CONTROLLERS
    ioExpanderPort0OutputValues |= I2C_SENSORS__IOE_P0_BIT__MC_RST_D;

    // Technically this isn't set yet because the I/O expander hasn't been written, but we'll save the state here
    // because calling this means we had the intention of setting this state
    SET_PSBI_IN_STATE_PTR(persistentStatePtr, PSBI__MC_RST_D);
#endif
}
inline void releaseMotorsReset(void)
{
#ifndef PROGRAM_MOTOR_CONTROLLERS
    ioExpanderPort0OutputValues |= I2C_SENSORS__IOE_P0_BIT__MC_RST_A;
    ioExpanderPort0OutputValues |= I2C_SENSORS__IOE_P0_BIT__MC_RST_B;
    ioExpanderPort0OutputValues |= I2C_SENSORS__IOE_P0_BIT__MC_RST_C;
    ioExpanderPort0OutputValues |= I2C_SENSORS__IOE_P0_BIT__MC_RST_D;

    // Technically this isn't set yet because the I/O expander hasn't been written, but we'll save the state here
    // because calling this means we had the intention of setting this state
    SET_PSBI_IN_STATE_PTR(persistentStatePtr, PSBI__MC_RST_A);
    SET_PSBI_IN_STATE_PTR(persistentStatePtr, PSBI__MC_RST_B);
    SET_PSBI_IN_STATE_PTR(persistentStatePtr, PSBI__MC_RST_C);
    SET_PSBI_IN_STATE_PTR(persistentStatePtr, PSBI__MC_RST_D);
#endif
}

/**
 * @brief      Sets the motors to reset. (LO = RESET)
 * @todo CONFIRM THIS IS ACTUALLY NEGATED (i.e. reset is active when low)
 */
inline void setMotor1Reset(void)
{
#ifndef PROGRAM_MOTOR_CONTROLLERS
    ioExpanderPort0OutputValues &= ~I2C_SENSORS__IOE_P0_BIT__MC_RST_A;

    // Technically this isn't set yet because the I/O expander hasn't been written, but we'll save the state here
    // because calling this means we had the intention of setting this state
    CLEAR_PSBI_IN_STATE_PTR(persistentStatePtr, PSBI__MC_RST_A);
#endif
}
inline void setMotor2Reset(void)
{
#ifndef PROGRAM_MOTOR_CONTROLLERS
    ioExpanderPort0OutputValues &= ~I2C_SENSORS__IOE_P0_BIT__MC_RST_B;

    // Technically this isn't set yet because the I/O expander hasn't been written, but we'll save the state here
    // because calling this means we had the intention of setting this state
    CLEAR_PSBI_IN_STATE_PTR(persistentStatePtr, PSBI__MC_RST_B);
#endif
}
inline void setMotor3Reset(void)
{
#ifndef PROGRAM_MOTOR_CONTROLLERS
    ioExpanderPort0OutputValues &= ~I2C_SENSORS__IOE_P0_BIT__MC_RST_C;

    // Technically this isn't set yet because the I/O expander hasn't been written, but we'll save the state here
    // because calling this means we had the intention of setting this state
    CLEAR_PSBI_IN_STATE_PTR(persistentStatePtr, PSBI__MC_RST_C);
#endif
}
inline void setMotor4Reset(void)
{
#ifndef PROGRAM_MOTOR_CONTROLLERS
    ioExpanderPort0OutputValues &= ~I2C_SENSORS__IOE_P0_BIT__MC_RST_D;

    // Technically this isn't set yet because the I/O expander hasn't been written, but we'll save the state here
    // because calling this means we had the intention of setting this state
    CLEAR_PSBI_IN_STATE_PTR(persistentStatePtr, PSBI__MC_RST_D);
#endif
}
inline void setMotorsReset(void)
{
#ifndef PROGRAM_MOTOR_CONTROLLERS
    ioExpanderPort0OutputValues &= ~I2C_SENSORS__IOE_P0_BIT__MC_RST_A;
    ioExpanderPort0OutputValues &= ~I2C_SENSORS__IOE_P0_BIT__MC_RST_B;
    ioExpanderPort0OutputValues &= ~I2C_SENSORS__IOE_P0_BIT__MC_RST_C;
    ioExpanderPort0OutputValues &= ~I2C_SENSORS__IOE_P0_BIT__MC_RST_D;

    // Technically this isn't set yet because the I/O expander hasn't been written, but we'll save the state here
    // because calling this means we had the intention of setting this state
    CLEAR_PSBI_IN_STATE_PTR(persistentStatePtr, PSBI__MC_RST_A);
    CLEAR_PSBI_IN_STATE_PTR(persistentStatePtr, PSBI__MC_RST_B);
    CLEAR_PSBI_IN_STATE_PTR(persistentStatePtr, PSBI__MC_RST_C);
    CLEAR_PSBI_IN_STATE_PTR(persistentStatePtr, PSBI__MC_RST_D);
#endif
}
/**
 * @brief      Power the hercules MCU (HI = ON)
 */
inline void powerOnHercules(void)
{
    PJOUT |= BIT0;
    SET_PSBI_IN_STATE_PTR(persistentStatePtr, PSBI__HERCULES_ON);
}

/**
 * @brief      Power off the hercules MCU (LO = OFF)
 */
inline void powerOffHercules(void)
{
    PJOUT &= ~BIT0;
    CLEAR_PSBI_IN_STATE_PTR(persistentStatePtr, PSBI__HERCULES_ON);
}

/**
 * @brief      Power on the Radio (HI = ON)
 */
inline void powerOnRadio(void)
{
    ioExpanderPort1OutputValues |= I2C_SENSORS__IOE_P1_BIT__RADIO_ON;

    // Technically this isn't set yet because the I/O expander hasn't been written, but we'll save the state here
    // because calling this means we had the intention of setting this state
    SET_PSBI_IN_STATE_PTR(persistentStatePtr, PSBI__RADIO_ON);
}

/**
 * @brief      Power off the Radio (LO = OFF)
 */
inline void powerOffRadio(void)
{
    ioExpanderPort1OutputValues &= ~I2C_SENSORS__IOE_P1_BIT__RADIO_ON;

    // Technically this isn't set yet because the I/O expander hasn't been written, but we'll save the state here
    // because calling this means we had the intention of setting this state
    CLEAR_PSBI_IN_STATE_PTR(persistentStatePtr, PSBI__RADIO_ON);
}

/**
 * @brief      Power on the FPGA (HI = ON)
 */
inline void powerOnFpga(void)
{
    PJOUT |= BIT1;
    SET_PSBI_IN_STATE_PTR(persistentStatePtr, PSBI__FPGA_ON);
}

/**
 * @brief      Power off the FPGA (LO = OFF)
 */
inline void powerOffFpga(void)
{
    PJOUT &= ~BIT1;
    CLEAR_PSBI_IN_STATE_PTR(persistentStatePtr, PSBI__FPGA_ON);
}

/**
 * @brief      Power on the motors (HI = ON)
 */
inline void powerOnMotors(void)
{
    PJOUT |= BIT2;
    SET_PSBI_IN_STATE_PTR(persistentStatePtr, PSBI__MOTOR_ON);
}

/**
 * @brief      Power off the motors (LO = OFF)
 */
inline void powerOffMotors(void)
{
    PJOUT &= ~BIT2;
    CLEAR_PSBI_IN_STATE_PTR(persistentStatePtr, PSBI__MOTOR_ON);
}

/**
 * @brief      Enable the batteries (HI = ON)
 */
inline void enableBatteries(void)
{
    // Turn on battery enable:
    blimp_battEnOn();
    // Make the latch absorb the BE state:
    blimp_latchBattUpdate();
}

/**
 * @brief      Disable the batteries (LO = OFF)
 */
inline void disableBatteries(void)
{
    // Turn off battery enable:
    blimp_battEnOff();
    // Make the latch absorb the BE state:
    blimp_latchBattUpdate();
}

/**
 * @brief      Deploy the rover from the lander
 */
inline void setDeploy(void)
{
    P3OUT |= BIT4;
    SET_PSBI_IN_STATE_PTR(persistentStatePtr, PSBI__DEPLOYMENT);
}

/**
 * @brief      Un-set deploy from lander
 */
inline void unsetDeploy(void)
{
    P3OUT &= ~BIT4;
    CLEAR_PSBI_IN_STATE_PTR(persistentStatePtr, PSBI__DEPLOYMENT);
}

/**
 * @brief      Start charging batteries from lander power
 */
inline void startChargingBatteries(void)
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

void setLatchSetAndResetHigh(void)
{
    ioExpanderPort0OutputValues |= I2C_SENSORS__IOE_P0_BIT__LATCH_RST;
    ioExpanderPort1OutputValues |= I2C_SENSORS__IOE_P1_BIT__LATCH_SET;
}

/**
 * @brief      Stop charging batteries
 */
inline void stopChargingBatteries(void)
{
    // Stop charging:
    blimp_chargerEnOff();
    // Disable the charging regulator:
    blimp_regEnOff();
}

/**
 * @brief     Releases BATT_CTRL_EN to enable the 3V3_LATCH regulator and power the battery control (battery latching
 *            and battery switch) circuitry.
 */
inline void blimp_bctrlEnOn(){
    // Go HiZ (input with no pulls) to let external pull-up to VIN do the work.
    P2DIR &= ~BIT3;
    P2REN &= ~BIT3;
}

/**
 * @brief      Forces BATT_CTRL_EN high to enable the 3V3_LATCH regulator and power the battery control (battery
 *             latching and battery switch) circuitry at the expense of a couple mA of current at 3V3 draining through
 *             the WD.
 *
 * NOTE: Normally you'd not want to do this because there's an external pull-up to V_LANDER/V_BATT that controls
 * this but if we can't talk to latching circuitry, it might be because one of the resistors vibed loose, in which
 * case this can be helpful (and possibly save our mission).
 */
inline void blimp_bctrlEnForceHigh(){
    // Set as output and drive high
    P2DIR |= BIT3;
    P2OUT |= BIT3;
}

/**
 * @brief     Drives BATT_CTRL_EN low to disable the 3V3_LATCH regulator and power off the battery control (battery
 *            latching and battery switch) circuitry.
 */
inline void blimp_bctrlEnOff(){
    // Set as output and drive low
    P2DIR |= BIT3;
    P2OUT &= ~BIT3;
}

/**
 * @brief     Sets the clock signal to the D-Latch high
 *            (note: by itself, this doesn't latch the batteries - see: blimp_latchBattUpdate)
 */
inline void blimp_latchBattOn(){
    P3OUT |= BIT6;
}

/**
 * @brief     Sets the clock signal to the D-Latch low
 *            (note: by itself, this doesn't un-latch the batteries - see:blimp_latchBattUpdate)
 */
inline void blimp_latchBattOff(){
    P3OUT &= ~BIT6;
}

/**
 * @brief     Pulses the clock signal to the D-Latch so the latch absorbs the state of BATT_EN.
 */
void blimp_latchBattUpdate(){
    // Pulse LB low-high-low to have the latch absorb the state of BE
    // delay should be at least 2us
    P3OUT &= ~BIT6; // set low first in case LB stayed driven high due to error or became high due to cosmic radiation
    __delay_cycles(400);
    P3OUT |= BIT6;
    __delay_cycles(400);
    P3OUT &= ~BIT6;
}

/**
 * @brief      Enables the charging IC. NOTE: You'll also need to turn on REGE (28V charging regulator) before you can
 *             actually charge. Do this first.
 */
inline void blimp_chargerEnOn(){
    // Go HiZ (input with no pulls) to let external pull-up to VIN do the work.
    PJDIR &= ~BIT3;
    PJREN &= ~BIT3;
    SET_PSBI_IN_STATE_PTR(persistentStatePtr, PSBI__CHRG_EN);
    CLEAR_PSBI_IN_STATE_PTR(persistentStatePtr, PSBI__CHRG_EN_FORCE_HIGH);
}

/**
 * @brief      Forces the charging IC input high.
 * NOTE: Normally you'd not want to do this because there's an external voltage divider that controls
 * this but if we can't enable charging, it might be because one of the resistors vibed loose, in which
 * case this can be helpful (and possibly save our mission).
 */
inline void blimp_chargerEnForceHigh(){
    // Set as output and drive high
    PJDIR |= BIT3;
    PJOUT |= BIT3;
    CLEAR_PSBI_IN_STATE_PTR(persistentStatePtr, PSBI__CHRG_EN);
    SET_PSBI_IN_STATE_PTR(persistentStatePtr, PSBI__CHRG_EN_FORCE_HIGH);
}

/**
 * @brief      Disables the charging IC. NOTE: If you're no longer charging, you'll also want to turn off REGE (28V
 *             charging regulator) to not waste power.
 */
inline void blimp_chargerEnOff(){
    // Set as output and drive low
    PJDIR |= BIT3;
    PJOUT &= ~BIT3;
    CLEAR_PSBI_IN_STATE_PTR(persistentStatePtr, PSBI__CHRG_EN);
    CLEAR_PSBI_IN_STATE_PTR(persistentStatePtr, PSBI__CHRG_EN_FORCE_HIGH);
}

/**
 * @brief      Turns on the 28V lander power regulator used by the charging IC (REGE).
 */
inline void blimp_regEnOn()
{
    P1OUT |= BIT5;
    SET_PSBI_IN_STATE_PTR(persistentStatePtr, PSBI__V_LANDER_REG_EN);
}

/**
 * @brief      Turns off the 28V lander power regulator used by the charging IC (REGE).
 */
inline void blimp_regEnOff()
{
    P1OUT &= ~BIT5;
    CLEAR_PSBI_IN_STATE_PTR(persistentStatePtr, PSBI__V_LANDER_REG_EN);
}

/**
 * @brief      Enables the battery enable override. NOTE: (by design) this won't latch the batteries on. You'll need to
 *             pulse LBATT for the latch absorbs the state of BATT_EN if you want the state to be persistent.
 */
void blimp_battEnOn(){
    PJOUT |= BIT5;
    SET_PSBI_IN_STATE_PTR(persistentStatePtr, PSBI__BATTERY_EN);
}

/**
 * @brief      Disables the battery enable override. NOTE: (by design) this won't turn off the batteries if they are
 *             also latched (LSTAT=1). To turn off the latch, you'll need to pulse LBATT so the latch absorbs the state
 *             of BATT_EN.
 */
void blimp_battEnOff(){
    PJOUT &= ~BIT5;
    CLEAR_PSBI_IN_STATE_PTR(persistentStatePtr, PSBI__BATTERY_EN);
}

/**
 * @brief      Enables the V_SYS_ALL switch which connects V_SYS_ALL to V_SYS and powers everything beside the WD
 *             (which is already powered by V_SYS).
 */
void blimp_vSysAllEnOn(){
    PJDIR |= BIT7;
    PJOUT |= BIT7;
    SET_PSBI_IN_STATE_PTR(persistentStatePtr, PSBI__V_SYS_ALL_EN);
    CLEAR_PSBI_IN_STATE_PTR(persistentStatePtr, PSBI__V_SYS_ALL_EN_FORCE_LOW);
}

/**
 * @brief      Disables the V_SYS_ALL switch which connects V_SYS_ALL to V_SYS and powers everything beside the WD
 *             (which is already powered by V_SYS).
 */
void blimp_vSysAllEnOff(){
    PJDIR &= ~BIT7;
    PJREN &= ~BIT7;
    CLEAR_PSBI_IN_STATE_PTR(persistentStatePtr, PSBI__V_SYS_ALL_EN);
    CLEAR_PSBI_IN_STATE_PTR(persistentStatePtr, PSBI__V_SYS_ALL_EN_FORCE_LOW);
}

void blimp_vSysAllEnForceLow()
{
    PJDIR |= BIT7;
    PJOUT &= ~BIT7;
    CLEAR_PSBI_IN_STATE_PTR(persistentStatePtr, PSBI__V_SYS_ALL_EN);
    SET_PSBI_IN_STATE_PTR(persistentStatePtr, PSBI__V_SYS_ALL_EN_FORCE_LOW);
}

/**
 * @brief      Reads STAT1 from BQ24650 charging IC. If H and STAT2 is H then fault. If L and STAT2 is H, then charging.
 */
inline BOOL blimp_cstat1(){
    return P1IN & BIT2;
}

/**
 * @brief      Reads the present state of the battery switch (BSTAT). ON is HIGH.
 */
inline BOOL blimp_bstat(){
    return PJIN & BIT6;
}


/**
 * @brief      Reads the present state of the battery switch (BSTAT). ON is HIGH.
 */
inline BOOL blimp_batteryState(){
    // Alias for blimp_bstat();
    return blimp_bstat();
}

inline uint8_t getIOExpanderPort0OutputValue(void)
{
    return ioExpanderPort0OutputValues;
}

inline uint8_t getIOExpanderPort1OutputValue(void)
{
    return ioExpanderPort1OutputValues;
}
