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

    // configure the ADC module
    // Relevant page numbers: pg. 890 of user manual
    // ADC12SHT0_2 = 16 ADC12CLK cycles for registers ADC12MEM0 to ADC12MEM7 and ADC12MEM24 to ADC12MEM31
    // ADC12SHT1_2 = 16 ADC12CLK cycles for registers ADC12MEM8 to ADC12MEM23
    // ADC12MSC = multiple samples
    // ADC12ON = ADC12 on (but not necessarily reading)
    // implicitly disables the ADC readings
    ADC12CTL0 = ADC12SHT0_2 | ADC12SHT1_2 | ADC12MSC | ADC12ON;

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

    // MEM0: P1.4 == V_LANDER_SENS == A4. Use VR+ = VeRef+ buffered, VR- = VeRef-
    ADC12MCTL0 = ADC12INCH_4 | ADC12VRSEL_15;

    // MEM1: P2.4 == BATT_TEMP == A7. Use VR+ = VeRef+ buffered, VR- = VeRef-
    ADC12MCTL1 = ADC12INCH_7 | ADC12VRSEL_15;

    // MEM2: P3.0 == BATT_RT == A12. Use VR+ = VeRef+ buffered, VR- = VeRef-
    ADC12MCTL2 = ADC12INCH_12 | ADC12VRSEL_15;

    // MEM3: P3.1 == V_SYS_ALL_SENS == A13. Use VR+ = VeRef+ buffered, VR- = VeRef-
    ADC12MCTL3 = ADC12INCH_13 | ADC12VRSEL_15;

    // MEM4: P3.2 == I_SYS_ALL_SENS == A14. Use VR+ = VeRef+ buffered, VR- = VeRef-
    ADC12MCTL4 = ADC12INCH_14 | ADC12VRSEL_15;

    // MEM5: P3.3 == V_BATT_SENS == A15. Use VR+ = VeRef+ buffered, VR- = VeRef-
    ADC12MCTL5 = ADC12INCH_15 | ADC12VRSEL_15;

    // MEM6: P4.0 == Vcc_2.5 == A8. Use VR+ = VeRef+ buffered, VR- = VeRef-
    ADC12MCTL6 = ADC12INCH_8 | ADC12VRSEL_15;

    // MEM7: P4.1 == Vcc_2.8 == A9. Use VR+ = VeRef+ buffered, VR- = VeRef-
    ADC12MCTL7 = ADC12INCH_9 | ADC12VRSEL_15;

    // MEM8: P4.2 == Vcc_28 == A10. Use VR+ = VeRef+ buffered, VR- = VeRef-
    ADC12MCTL8 = ADC12INCH_10 | ADC12VRSEL_15;

    // MEM9: P4.3 == Vcc_24 == A11. Use VR+ = VeRef+ buffered, VR- = VeRef-. Also, this is the end of the sequence.
    ADC12MCTL9 = ADC12INCH_11 | ADC12VRSEL_15 | ADC12EOS;

    // enable interrupts only on last reading
    ADC12IER0 = ADC12IE9;
}

BOOL isAdcSampleDone(void)
{
    return (!(ADC12CTL1 & ADC12BUSY)) ? TRUE : FALSE;
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
        case ADC12IV_ADC12IFG9: // ADC12IE9 interrupt
            outputValues->vLanderSense = ADC12MEM0;
            outputValues->battTemp = ADC12MEM1;
            outputValues->battRT = ADC12MEM2;
            outputValues->vSysAllSense = ADC12MEM3;
            outputValues->iSysAllSense = ADC12MEM4;
            outputValues->vBattSense = ADC12MEM5;
            outputValues->vcc2Point5 = ADC12MEM6;
            outputValues->vcc2Point8 = ADC12MEM7;
            outputValues->vcc28 = ADC12MEM8;
            outputValues->vcc24 = ADC12MEM9;
            outputValues->sampleComplete = TRUE; // update output struct to signal sample is complete.

            *watchdogFlagsPtr |= WDFLAG_ADC_READY; // signal ready to main loop
            break;
        default:
            break;
    }
}




