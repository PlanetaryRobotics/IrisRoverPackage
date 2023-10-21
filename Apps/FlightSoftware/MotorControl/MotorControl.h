/*
 * MotorControl.h
 *
 *  Created on: Oct 21, 2023
 *      Author: iris
 */

#ifndef MOTORCONTROL_H_
#define MOTORCONTROL_H_

#include "ICD_MotorControl.h"

#include "IQmathLib.h"
#include <msp430.h>

#include "driverlib.h"
#include "bsp.h"
#include "i2c.h"
#include "hallsensor.h"
#include "pi.h"
#include "mod6_cnt.h"
#include "impulse.h"



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



// Local Target Variables
int8_t g_targetDirection;       // TODO: Maybe pass via Reg?
volatile int32_t g_targetPosition;
volatile uint8_t g_targetSpeed;

volatile uint16_t g_accelRate;  // TODO: UNUSED
volatile uint16_t g_decelRate;  // TODO: UNUSED

// Loop Control Variables
bool g_targetReached;
volatile int32_t g_currentPosition;
int32_t g_oldPosition;

volatile _iq g_currentSpeed;

bool g_closedLoop;
_iq g_openLoopTorque;

// Maybe for Open Loop timing ?
uint16_t g_controlPrescaler;

// State Variables Theoretically...
volatile StateMachine g_state;
volatile CmdState g_cmdState;

uint8_t g_statusRegister;
volatile uint8_t g_controlRegister;
volatile uint8_t g_faultRegister;

// Error Counter Variables
volatile uint32_t g_drivingTimeoutCtr;
uint8_t g_errorCounter = 0; // incremented every time inner control loop is reached and motor is acting strange
                            // if it exceeds ERROR_ITERATION_THRESHOLD then motor is stopped



MC_ICD_RegStruct mcRegStruct;


void initMotorControl(McI2cAddr_t i2cAddr);
void initSensorVariables();
void initializeSoftwareControlVariables();



float uintToIq(uint16_t intVal)
{
    float iqSetVal = (float)intVal / (float)PI_PARAM_TO_IQ;
    return iqSetVal;
}


/** -----------------------------------------------
 *  ------- TODO: DEPRICATE
 *  -----------------------------------------------
 */

#endif /* MOTORCONTROL_H_ */
