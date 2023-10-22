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

#include "MotorControl.h"

/* ============================================
 *        Function/struct/enum definitions
 * ============================================
 */

void initMcParams();

bool read_driver_fault(); // function descriptions in main.c
void clear_driver_fault();
void updateStateMachine();


#endif /* MAIN_H_ */
