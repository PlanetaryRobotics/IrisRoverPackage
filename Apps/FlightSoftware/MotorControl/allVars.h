/*
 * allVars.h
 *
 *  Created on: Oct 21, 2023
 *      Author: iris
 */

#ifndef ALLVARS_H_
#define ALLVARS_H_


/* ============================================
 *          Bits of Registers
 * ============================================
 */
// bits of control register (g_controlRegister)
#define DRIVE_OPEN_LOOP 1    // first bit of control reg; drive only in open loop if set to 1
#define STATE_MACHINE_DISABLE 4
#define STATE_MACHINE_RUN 8
#define OVERRIDE_FAULT_DETECTION 16  // don't reset current & desired position if abnormal behavior detected
#define OPEN_LOOP_TORQUE_OVERRIDE 64 // use g_maxSpeed/MAX_TARGET_SPEED to calculate open loop PWM cycle (instead of constant at 0.3)

/* ============================================
 *          Constants
 * ============================================
 */
#define DRIVE_ON_BOOT_START_DELAY (20 * DELAY_100_ms) /*2 sec*/

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

#define PERIOD_IMPULSE 150        //   ^ also used to kick-start, see impulse.h for details
#define OPEN_LOOP_TICKS 10        // distance in hall sensor ticks motor covers in open loop mode per 1/15.6 sec



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
uint8_t g_errorCounter; // incremented every time inner control loop is reached and motor is acting strange
                            // if it exceeds ERROR_ITERATION_THRESHOLD then motor is stopped

void initAllVars();





#endif /* ALLVARS_H_ */
