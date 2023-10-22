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

void updateMotorController();


McFaultMask_t g_mcIgnoredFaults;
uint8_t g_mcDefinedMaxSpeed;
//bool updateProtectedVar(MC_IDC_Protected_Var var, int32_t new_val);

#endif /* MOTORCONTROL_H_ */
