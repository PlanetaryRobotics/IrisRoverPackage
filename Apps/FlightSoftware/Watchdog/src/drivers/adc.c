#include <msp430.h>
#include "drivers/adc.h"
#include "flags.h"
#include "event/event_queue.h"

/*
 * File for interfacing with ADC hardware module
 *
 * ADC pins are as follows:
 * P4.0, P4.1, P4.2, P4.3, P3.0 are all analog inputs
 *
 * Vref = 2.5 V (originally thought it would be 3.3V)
 *
 * Port | Expected voltage             | Expected reading (12-bit) |
 * -----|------------------------------|---------------------------|
 * P4.0 | 2.50V                        | 4095                      |
 * P4.1 | 2.80V                        | 4095                      |
 * P4.2 | 2.55V (1:11 divider of 28V)  | 4095                      |
 * P4.3 | 2.99V (~1:8 divider of 24V)  | 4095                      |
 * P3.0 | see document below           | Manually calibrated       |
 *
 * Thermistor (P3.0) expected voltages given temp:
 *          https://drive.google.com/file/d/1rxdHpPYH3jQ4o-HlOHyc0wqWvTNbUub2/view?usp=sharing
 * "The ADC12_B supports 8-bit, 10-bit, and 12-bit resolution modes,
 * and the ADC12RES bits select the current mode. The analog-to-digital
 * conversion requires 10, 12, and 14 ADC12CLK cycles, respectively."
 *
 * 8-bit: LSB = 0.012890625V
 * 10-bit: LSB = 0.00322265625V
 * 12-bit: LSB = 0.0008056640625V
 */

static AdcValues* outputValues = NULL;
static volatile uint16_t* watchdogFlagsPtr = NULL;

void adc_init(volatile uint16_t* watchdogFlags) {
    watchdogFlagsPtr = watchdogFlags;

    // Set the correct settings for various inputs
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

    // configure the ADC module
    // Relevant page numbers: pg. 890 of user manual
    // ADC12SHT0_2 = 16 ADC12CLK cycles
    // ADC12MSC = multiple samples
    // ADC12ON = ADC12 on (but not necessarily reading)
    // implicitly disables the ADC readings
    ADC12CTL0 = ADC12SHT0_2 | ADC12MSC | ADC12ON;

    // ADC12SHP = SAMPCON signal is sourced from the sampling timer
    // ADC12CONSEQ_1 = use "sequence-of-channels" mode to read
    // ADC12SSEL3 = source SMCLK as a clock (0=ADC12OSC, 1=ACLK, 2=MCLK)
    // Implicitly sets clock dividers to 1
    ADC12CTL1 = ADC12SHP | ADC12CONSEQ_1 | ADC12SSEL_3;

    // ADC12RES_2 = 12-bit resolution
    // Implicitly sets data encoding mode to unsigned binary and disables low power mode
    ADC12CTL2 = ADC12RES_2;

    // Implicitly start reading at MEM0
    ADC12CTL3 = 0;

    while(REFCTL0 & REFGENBUSY); // Wait for internal ref generator to be ready
    REFCTL0 |= REFVSEL_2|REFON;  // choose 2.5V internal reference
    while(!(REFCTL0 & REFGENRDY)); //wait for reference generator to settle
}

BOOL isAdcSampleDone(void)
{
    return (!(ADC12CTL1 & ADC12BUSY)) ? TRUE : FALSE;
}

BOOL setupAdcForLander(void)
{ // set up adc to read values for lander mode
    if (NULL == watchdogFlagsPtr) {
        return FALSE;
    }

    // If existing sample isn't done, then we can't do the setup
    if (!isAdcSampleDone()) {
        return FALSE;
    }

    // enable interrupts only on last reading
    ADC12IER0 = ADC12IE1;

    // set up the ADC for the lander state
    ADC12MCTL0 = ADC12INCH_10 | ADC12VRSEL_1; // A10 = P4.2 stored in MEM0
    ADC12MCTL1 = ADC12INCH_12 | ADC12VRSEL_1 | ADC12EOS; // A12 = P3.0 stored in MEM1 (also, EOS)

    // clear sample ready, if set
    *watchdogFlagsPtr &= ~WDFLAG_ADC_READY;

    return TRUE;
}

BOOL setupAdcForMission(void)
{ // set up adc to read values for mission mode (voltage rails)
    if (NULL == watchdogFlagsPtr) {
        return FALSE;
    }

    // If existing sample isn't done, then we can't do the setup
    if (!isAdcSampleDone()) {
        return FALSE;
    }

    // enable interrupts only on last reading
    ADC12IER0 = ADC12IE2;

    // set up the ADC for the mission state
    ADC12MCTL0 = ADC12INCH_8 | ADC12VRSEL_1; // A8 = P4.0 stored in MEM0 (Vcc 2.5V)
    ADC12MCTL1 = ADC12INCH_12 | ADC12VRSEL_1; // A12 = P3.0 stored in MEM1 (temperature)
    ADC12MCTL2 = ADC12INCH_9 | ADC12VRSEL_1; // A9 = P4.1 stored in MEM2 (Vcc 2.8V)
    ADC12MCTL3 = ADC12INCH_11 | ADC12VRSEL_1 | ADC12EOS; // A11 = P4.3 stored in MEM2 (Vcc 24V divided down)

    // clear sample ready, if set
    *watchdogFlagsPtr &= ~WDFLAG_ADC_READY;

    return TRUE;
}

/**
 * @brief take one sample of the ADC
 */
BOOL adcCheckVoltageLevels(AdcValues* output)
{
    if (NULL == output) {
        return FALSE;
    }

    // If existing sample isn't done, then we can't trigger a new sample
    if (!isAdcSampleDone()) {
        return FALSE;
    }

    // Update the
    outputValues = output;
    outputValues->sampleComplete = FALSE;

    // take one sample of the ADC
    ADC12CTL0 |= ADC12SC | ADC12ENC;

    return TRUE;
}

// Interrupt handler for when the ADC has completed a reading
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = ADC12_VECTOR
__interrupt void ADC12_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(ADC12_VECTOR))) ADC12_ISR (void)
#else
#error Compiler not supported!
#endif
{
    if (NULL == outputValues) {
        return;
    }

    switch (__even_in_range(ADC12IV, ADC12IV_ADC12RDYIFG)) {
        case ADC12IV_ADC12IFG3: // ADC12IE3 interrupt
            outputValues->data[3] = ADC12MEM3; // Save MEM3
            outputValues->data[2] = ADC12MEM2; // Save MEM2
            // fall through to ADC12IE1 to save MEM1 & MEM0 and signal to main loop
            /* no break */
        case ADC12IV_ADC12IFG1: // ADC12IE1 interrupt
            outputValues->data[1] = ADC12MEM1; // Save MEM1
            outputValues->data[0] = ADC12MEM0; // Save MEM0
            outputValues->sampleComplete = TRUE; // update output struct to signal sample is complete.

            *watchdogFlagsPtr |= WDFLAG_ADC_READY; // signal ready to main loop
            //!< @todo
            break;
        default:
            break;
    }
}




