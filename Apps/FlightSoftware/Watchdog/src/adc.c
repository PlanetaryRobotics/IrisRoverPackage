#include <msp430.h>
#include "include/adc.h"
#include "include/flags.h"

/*
 * File for interfacing with ADC hardware module
 *
 * ADC pins are as follows:
 * P4.0, P4.1, P4.2, P4.3, P3.0 are all analog inputs
 *
 * Port | Expected voltage             | Expected reading (12-bit) |
 * -----|------------------------------|---------------------------|
 * P4.0 | 2.50V                        | 3103                      |
 * P4.1 | 2.80V                        | 3475                      |
 * P4.2 | 2.55V (1:11 divider of 28V)  | 3165                      |
 * P4.3 | 2.99V (~1:8 divider of 24V)  | 3711                      |
 * P3.0 | ???                          | Manually calibrated       |
 *
 * "The ADC12_B supports 8-bit, 10-bit, and 12-bit resolution modes,
 * and the ADC12RES bits select the current mode. The analog-to-digital
 * conversion requires 10, 12, and 14 ADC12CLK cycles, respectively."
 *
 * 8-bit: LSB = 0.012890625V
 * 10-bit: LSB = 0.00322265625V
 * 12-bit: LSB = 0.0008056640625V
 */

void adc_init() {
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
    ADC12CTL1 = ADC12SHP | ADC12CONSEQ_1; //TODO: commented this out | ADC12SSEL_3;

    // ADC12RES_2 = 12-bit resolution
    // Implicitly sets data encoding mode to unsigned binary and disables low power mode
    ADC12CTL2 = ADC12RES_2;

    // Implicitly start reading at MEM0
    ADC12CTL3 = 0;

    // enable interrupts only on last reading
    ADC12IER0 = ADC12IE4;

    //ADC12SSELx
    // SMCLK, MCLK, ACLK, and the MODCLK are the possible ADC12CLK sources
    // The ADC12PDIV bits set the initial divider on the input clock (1, 4, 32, or 64),
    // and then ADC12DIV bits set an additional divider of 1 to 8

    // CSTARTADDx
    // The CSTARTADDx bits define the first ADC12MCTLx used for any conversion. If the conversion mode is single-channel or repeat-single-channel, the CSTARTADDx points to the single ADC12MCTLx to be used.
    // If the conversion mode selected is either sequence-of-channels or repeat-sequence-of-channels, CSTARTADDx points to the first ADC12MCTLx location to be used in a sequence. A pointer, not visible to software, is incremented automatically to the next ADC12MCTLx in sequence when each conversion completes. The sequence continues until an ADC12EOS bit in ADC12MCTLx is processed; this is, the last control byte processed.

    // I think we want ADC12DF = 0 with ADC12DIF = 0

    // Sequence-of-Channels Mode = ADC12MSC

    // set up each pin
    /* note that using equals signs implicitly disable comparators, disable
     * differential mode, and uses VCC (3V3) and VSS (0V) as references
     */
    ADC12MCTL0 = ADC12INCH_8; // A8 = P4.0 stored in MEM0
    ADC12MCTL1 = ADC12INCH_9; // A9 = P4.1 stored in MEM1
    ADC12MCTL2 = ADC12INCH_10 | ADC12VRSEL_1; // A10 = P4.2 stored in MEM2
    ADC12MCTL3 = ADC12INCH_11 | ADC12VRSEL_1; // A11 = P4.3 stored in MEM3
    ADC12MCTL4 = ADC12INCH_12 | ADC12VRSEL_1 | ADC12EOS; // A12 = P3.0 stored in MEM4 (also, EOS)
}

volatile unsigned uint8_t adc_values_n = 0;

void adc_setup_lander() {
    // set up the ADC for the lander state

}

/**
 * @brief take one sample of the ADC
 */
inline void adc_sample() {
    // wait until existing sample done
    while (ADC12CTL1 & ADC12BUSY) { __no_operation(); }
    // take one sample of the ADC
    ADC12CTL0 |= ADC12SC | ADC12ENC;
}

volatile unsigned short adc_values[5] = {0,0,0,0,0};

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
    switch (__even_in_range(ADC12IV, ADC12IV_ADC12RDYIFG))
    {
        case ADC12IV_ADC12IFG4: // ADC12IE4 interrupt
            adc_values[0] = ADC12MEM0; // Save MEM0
            adc_values[1] = ADC12MEM1; // Save MEM1
            adc_values[2] = ADC12MEM2; // Save MEM2
            adc_values[3] = ADC12MEM3; // Save MEM3
            adc_values[4] = ADC12MEM4; // Save MEM4
            watchdog_flags |= WDFLAG_ADC_READY;
            break;
        default: break;
    }
}




