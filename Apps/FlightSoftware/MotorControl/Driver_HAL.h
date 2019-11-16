/*
 * Hardware Abstraction Layer for the Motor Driver (and related hardware).
 *
 * TODO: Implement (pull over the EVM test code).
 *
 * Author: Connor W. Colombo, CMU
 */

#ifndef _CR_MOT_DRIVER_HAL_H__
#define _CR_MOT_DRIVER_HAL_H__

//
// Include files
//
#include <stdint.h>
#include <stdbool.h>

extern void setPWM(uint16_t duty);

extern int16_t getCurrentA(void);
extern int16_t getCurrentB(void);
extern int16_t getCurrentC(void);
extern int16_t getCurrentNorm(void);

extern uint8_t getHall1State(void);
extern uint8_t getHall2State(void);
extern uint8_t getHall3State(void);

extern int32_t getPosition(void);

extern uint32_t getTotalTimeDelta(void);

//
// Events (called by HAL, declared in Application)
//
extern bool Encoder_Update_Event(void);

#endif //_CR_MOT_DRIVER_HAL_H__
