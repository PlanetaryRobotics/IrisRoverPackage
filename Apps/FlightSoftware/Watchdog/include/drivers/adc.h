/**
 * @file adc.h
 * @brief File for interfacing with ADC hardware module
 * File for interfacing with Analog-to-Digital (ADC) hardware module.
 * Used to monitor power levels
 */
#ifndef __ADC_INC
#define __ADC_INC

#include "common.h"

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct AdcValues
{
    volatile uint16_t vLanderSense;
    volatile uint16_t battTemp;
    volatile uint16_t battRT;
    volatile uint16_t vSysAllSense;
    volatile uint16_t iSysAllSense;
    volatile uint16_t vBattSense;
    volatile uint16_t vcc2Point5;
    volatile uint16_t vcc2Point8;
    volatile uint16_t vcc28;
    volatile uint16_t vcc24;
    volatile BOOL sampleComplete;
} AdcValues;

/**
 * @brief Initialize ADC hardware.
 * Sets up the interrupts and whatnot for ADC.
 */
void adc_init(volatile uint16_t* watchdogFlags);

BOOL isAdcSampleDone(void);

BOOL adcCheckVoltageLevels(AdcValues* output);

#ifdef __cplusplus
} /* close extern "C" */
#endif

#endif /* __ADC_INC */
