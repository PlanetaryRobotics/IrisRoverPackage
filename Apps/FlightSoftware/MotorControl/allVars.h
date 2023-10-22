/*
 * allVars.h
 *
 *  Created on: Oct 21, 2023
 *      Author: iris
 */

#ifndef ALLVARS_H_
#define ALLVARS_H_

#include "ICD_MotorControl.h"

/* Include the IQmath header file. */
#define GLOBAL_IQ 15
#include "IQmathLib.h"

#include "driverlib.h"
#include "bsp.h"
#include "i2c.h"
#include "pi.h"
#include "mod6_cnt.h"
#include "impulse.h"
#include "MotorControl.h"

#include <msp430.h>

/* ============================================
 *          Bits of Registers
 * ============================================
 */
// bits of control register (g_controlRegister)
#define DRIVE_OPEN_LOOP 1    // first bit of control reg; drive only in open loop if set to 1
#define CLEAR_DRIVER_FAULT 2 // second bit indicates request to try to clear fault in motor driver
#define STATE_MACHINE_DISABLE 4
#define STATE_MACHINE_RUN 8
#define OVERRIDE_FAULT_DETECTION 16  // don't reset current & desired position if abnormal behavior detected
#define EXECUTE_COMMAND 32           // actually drive to command
#define OPEN_LOOP_TORQUE_OVERRIDE 64 // use g_maxSpeed/MAX_TARGET_SPEED to calculate open loop PWM cycle (instead of constant at 0.3)

// bits of status register (shares 1,2,4 with control register)
#define POSITION_CONVERGED 8
#define CONTROLLER_ERROR 16 // indicates something has gone awry with position controller; will not converge

// bits of fault register
#define DRIVER_FAULT 1            // for if there is a fault in the DRV8304 motor drivers
#define POSITION_NO_CHANGE 2      // for if position is not changing at all; could be dead hall sensors
#define DRIVING_WRONG_DIRECTION 4 // for if motor is driving in wrong direction
#define DRIVING_TIMEOUT 8         // for if motor does not converge in time defined by DRIVING_TIMEOUT_THRESHOLD


/* ============================================
 *          Constants
 * ============================================
 */

// TO INCREASE MAX SPEED:
//   - increase 'PI_OUTPUT_BOUNDS'
//   - increase 'MAX_TARGET_SPEED'
// What about FULLY_OPEN_LOOP_PWM? <- no, we need this low. set OPEN_LOOP_TORQUE_OVERRIDE, set g_maxSpeed,
// change g_targetPosition ~2000 (top of void main()
// change g_controlRegister to 32 (or 33 for torque override)

// timing constants (control rate groups, delays, number of iterations for flags)
#define ERROR_ITERATION_THRESHOLD 10       // how many iterations motor performance must be funky before driving is stopped
#define PI_SPD_CONTROL_PRESCALER 1000      // 15.6 Hz, speed control
#define OPEN_LOOP_SPEED 3                  // estimate of rotational distance (in hall sensor ticks) covered in 1/15.6 [sec] by motor when in open loop
#define DRIVING_TIMEOUT_THRESHOLD 1872     // how much time in 1/15.6 [sec] before stopping driving (1872 = ~120 seconds)
#define POSITION_CONVERGENCE_THRESHOLD 100 // how many motor ticks within target position motor needs to be to stop driving (absolute value)

#define PI_OUTPUT_BOUNDS 1.0
#define PI_OUTPUT_BOUNDS_IQ     0x8000  // 1.0
#define OPEN_LOOP_TORQUE 0.15   // for kick-starting into closed loop (normalized to 1.0, 1.0 being maximum current system can produce)
#define OPEN_LOOP_TORQUE_IQ     0x1333  // 0.15
#define PERIOD_IMPULSE 150        //   ^ also used to kick-start, see impulse.h for details

#define CLOSE_LOOP_THRESHOLD 0.01   // Close loop threshold from open to close loop -> threshold for current speed
#define CLOSE_LOOP_THRESHOLD_IQ 0x0CCC  // 0.01
#define OPEN_LOOP_TICKS 10        // distance in hall sensor ticks motor covers in open loop mode per 1/15.6 sec

#define PI_CURRENT_IL 0.5
#define PI_CURRENT_IL_IQ        0x4000  // 0.5
#define FULLY_OPEN_LOOP_PWM 0.3   // PWM duty cycle ( 30% ) as decimal
#define FULLY_OPEN_LOOP_PWM_IQ  0x2666  // 0.3

typedef struct HallSensor
{
    uint8_t Pattern;
    uint8_t OldPattern;
    bool Event;
    bool Error;
} HallSensor;


// State Variables Theoretically...
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




_iq g_currentPhaseA;
_iq g_currentPhaseB;
_iq g_currentPhaseC;
_iq g_feedforwardFW;

// Used for calibration
_iq g_currentOffsetPhaseA;
_iq g_currentOffsetPhaseB;
_iq g_currentOffsetPhaseC;
//volatile _iq g_currentSpeed;
_iq g_currentSpeed;
_iq g_openLoopTorque;

_iq g_closeLoopThreshold;

// Hall Sensor Variables
uint8_t g_commState;
uint8_t g_oldCommState;
HallSensor g_hallSensor;
uint8_t g_hallMap[8];


volatile int32_t g_currentPosition;
volatile int32_t g_targetPosition;
int32_t g_oldPosition;

// Maybe for Open Loop timing ?
uint16_t g_controlPrescaler;

volatile PI_CONTROLLER g_piSpd;
volatile PI_CONTROLLER g_piCur;
volatile MOD6CNT g_mod6cnt;
volatile IMPULSE g_impulse;

bool g_closedLoop;
bool g_targetReached;
volatile uint8_t g_targetSpeed;

volatile StateMachine g_state;
volatile CmdState g_cmdState;

volatile uint16_t g_accelRate;  // TODO: UNUSED
volatile uint16_t g_decelRate;  // TODO: UNUSED

int8_t g_targetDirection;       // TODO: Maybe pass via Reg?

uint8_t g_statusRegister;
volatile uint8_t g_controlRegister;
volatile uint8_t g_faultRegister;
volatile uint32_t g_drivingTimeoutCtr;
//uint8_t g_errorCounter; // incremented every time inner control loop is reached and motor is acting strange
//                            // if it exceeds ERROR_ITERATION_THRESHOLD then motor is stopped
//bool g_readSensors;



//void initAllVars();
void initializeSensorVariables(void);
void initializeSoftwareControlVariables(void);
void initializeControllerVariables(void);
void initializeHallInterface(void);

#endif /* ALLVARS_H_ */
