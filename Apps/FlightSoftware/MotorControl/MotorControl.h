/*
 * MotorControl.h
 *
 *  Created on: Oct 21, 2023
 *      Author: iris
 */

#ifndef MOTORCONTROL_H_
#define MOTORCONTROL_H_


/* Include the IQmath header file. */
#define GLOBAL_IQ 15
#include "IQmathLib.h"

#include "ICD_MotorControl.h"
#include "allVars.h"

#include "driverlib.h"
#include "bsp.h"
#include "i2c.h"
#include "hallsensor.h"
#include "pi.h"
#include "mod6_cnt.h"
#include "impulse.h"

#include <msp430.h>

// internal state machine for motor
typedef enum StateMachine
{
    IDLE,   // motor driver turned off, target & current position reset to 0
    RUNNING // actively trying to converge to target positions
} StateMachine;

// possible transition options for internal state machine
typedef enum CmdState
{
    RUN,     // if in IDLE, switch to RUNNING
    DISABLE, // if in RUNNING, switch to IDLE
    NO_CMD   // don't change state
} CmdState;





MC_ICD_RegStruct mcRegStruct;


void initMotorControl(McI2cAddr_t i2cAddr);
void initSensorVariables();


/** -----------------------------------------------
 *  ------- TODO: DEPRICATE
 *  -----------------------------------------------
 */

#endif /* MOTORCONTROL_H_ */
