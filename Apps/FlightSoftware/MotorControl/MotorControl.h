/*
 * MotorControl.h
 *
 *  Created on: Oct 21, 2023
 *      Author: iris
 */

#ifndef MOTORCONTROL_H_
#define MOTORCONTROL_H_


#include "allVars.h"


MC_ICD_RegStruct mcRegStruct;


void initMotorControl(McI2cAddr_t i2cAddr);


/** -----------------------------------------------
 *  ------- TODO: DEPRICATE
 *  -----------------------------------------------
 */

#endif /* MOTORCONTROL_H_ */
