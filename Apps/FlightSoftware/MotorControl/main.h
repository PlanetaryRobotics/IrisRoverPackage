// ======================================================================
// \title  i2c.h
// \author cedric
// \edited by Jonathan
// \brief  Includes necessary header files and declares functions used by
//         other files (really just i2c.c)
//
// ======================================================================
#ifndef MAIN_H_
#define MAIN_H_

/* Include the IQmath header file. */
#define GLOBAL_IQ 15
#include "IQmathLib.h"


#include "MotorControl.h"

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
// control related constants (gains, thresholds, setpoints, default values)
#define KP_SPD 1.5
#define KI_SPD 0.0009
#define KP_CUR 0.95
#define KI_CUR 0.002
#define PI_OUTPUT_BOUNDS 1.0 // will be converted to _IQ

#define OPEN_LOOP_TORQUE 0.15     // for kick-starting into closed loop (normalized to 1.0, 1.0 being maximum current system can produce)
#define PERIOD_IMPULSE 150        //   ^ also used to kick-start, see impulse.h for details
#define CLOSE_LOOP_THRESHOLD 0.01 // Close loop threshold from open to close loop
                                  //   -> threshold for current speed
#define OPEN_LOOP_TICKS 10        // distance in hall sensor ticks motor covers in open loop mode per 1/15.6 sec
#define FULLY_OPEN_LOOP_PWM 0.3   // PWM duty cycle ( 30% ) as decimal


/* ============================================
 *        Function/struct/enum definitions
 * ============================================
 */

void initMcParams();

bool read_driver_fault(); // function descriptions in main.c
void clear_driver_fault();
void updateStateMachine();


#endif /* MAIN_H_ */
