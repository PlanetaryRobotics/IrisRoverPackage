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
    volatile uint16_t data[4];
    volatile BOOL sampleComplete;
} AdcValues;

/**
 * @brief Initialize ADC hardware.
 * Sets up the interrupts and whatnot for ADC.
 */
void adc_init(volatile uint16_t* watchdogFlags);

BOOL isAdcSampleDone(void);

BOOL setupAdcForLander(void);

BOOL setupAdcForMission(void);

BOOL adcCheckVoltageLevels(AdcValues* output);


#define ADC_2V5_LEVEL_IDX 0
#define ADC_2V8_LEVEL_IDX 2
#define ADC_BATT_LEVEL_IDX 3
#define ADC_LANDER_LEVEL_IDX 0
#define ADC_TEMP_IDX 1

#ifdef __cplusplus
} /* close extern "C" */
#endif

#endif /* __ADC_INC */
