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
#include <include/motor.h>
#include <msp430.h>

#include "include/i2c.h"

#include "include/pi.h"
#include "include/utils.h"
#include "include/registers.h"

/* ============================================
 *          Constants
 * ============================================
 */

// TO INCREASE MAX SPEED:
//   - increase 'PI_OUTPUT_BOUNDS'
//   - increase 'MAX_TARGET_SPEED'
// change g_targetPosition ~2000 (top of void main()
// change g_controlRegister to 32

// timing constants (control rate groups, delays, number of iterations for flags)
#define ERROR_ITERATION_THRESHOLD       10      // how many iterations motor performance must be funky before driving is stopped
#define PI_SPD_CONTROL_PRESCALER        1000 // 15.6 Hz, speed control
#define OPEN_LOOP_SPEED                 3   // estimate of rotational distance (in hall sensor ticks) covered in 1/15.6 [sec] by motor when in open loop
#define DRIVING_TIMEOUT_THRESHOLD       1872 // how much time in 1/15.6 [sec] before stopping driving (1872 = ~120 seconds)
#define POSITION_CONVERGENCE_THRESHOLD  100 // how many motor ticks within target position motor needs to be to stop driving (absolute value)

#define OPEN_LOOP_TORQUE        0.15       // for kick-starting into closed loop (normalized to 1.0, 1.0 being maximum current system can produce)
#define PERIOD_IMPULSE          150       //   ^ also used to kick-start, see impulse.h for details
#define CLOSE_LOOP_THRESHOLD    0.01      // Close loop threshold from open to close loop
                                          //   -> threshold for current speed
#define OPEN_LOOP_TICKS         10       // distance in hall sensor ticks motor covers in open loop mode per 1/15.6 sec
#define FULLY_OPEN_LOOP_PWM     0.3      // PWM duty cycle ( 30% ) as decimal
#define MAX_TARGET_SPEED                   100U // used to initialize g_maxSpeed

/* ============================================
 *        Function/struct/enum definitions
 * ============================================
 */

bool read_driver_fault(); //function descriptions in main.c
void clear_driver_fault();
void updateStateMachine();

#endif /* MAIN_H_ */
