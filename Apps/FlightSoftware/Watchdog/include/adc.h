/**
 * @file adc.h
 * @brief File for interfacing with ADC hardware module
 * File for interfacing with Analog-to-Digital (ADC) hardware module.
 * Used to monitor power levels
 */
#ifndef __ADC_INC
#define __ADC_INC

#include "buffer.h"

/**
 * @brief Initialize ADC hardware.
 * Sets up the interrupts and whatnot for ADC.
 */
void adc_init();

#endif /* __ADC_INC */
