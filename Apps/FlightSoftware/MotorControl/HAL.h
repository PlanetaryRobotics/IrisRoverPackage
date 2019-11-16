/*
 * Hardware Abstraction Layer for Motor Controller.
 *
 * Author: Connor W. Colombo, CMU
 * Some SMBus functions taken from or based off of example code in MSPWare by Texas Instruments Incorporated, BSD Copyright (c) 2015.
 */

#ifndef _CR_MOT_HAL_H__
#define _CR_MOT_HAL_H__

//
// Include files
//
#include <stdint.h>
#include <stdbool.h>
#include "msp430.h"

//
// Function prototypes
//
extern void HAL_MCU_Init(void);
extern void HAL_I2C_InitGPIO(void);
extern uint32_t HAL_I2C_GetBaseAddr(void);

//
// Events (called by HAL, declared in Application)
//
extern bool HAL_I2C_Event(void);
#ifdef __MSP430_HAS_USCI__
extern bool HAL_Timeout_Event(void);
#endif

#endif //_CR_MOT_HAL_H__
