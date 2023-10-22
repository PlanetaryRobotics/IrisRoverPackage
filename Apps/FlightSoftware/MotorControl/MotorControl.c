/*
 * MotorControl.c
 *
 *  Created on: Oct 21, 2023
 *      Author: iris
 */

#include "MotorControl.h"

void initMotorControl(McI2cAddr_t i2cAddr)
{
    initMcRegStruct(&mcRegStruct, i2cAddr);
}

