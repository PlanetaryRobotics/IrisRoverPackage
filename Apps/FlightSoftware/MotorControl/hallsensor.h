/*
 * hallsensor.h
 *
 *  Created on: Oct 21, 2023
 *      Author: iris
 */

#ifndef HALLSENSOR_H_
#define HALLSENSOR_H_

#include "IQmathLib.h"

typedef struct HallSensor
{
    uint8_t Pattern;
    uint8_t OldPattern;
    bool Event;
    bool Error;
} HallSensor;

// Hall Sensor Variables
HallSensor g_hallSensor;
uint8_t g_hallMap[8];
uint8_t g_commState;
uint8_t g_oldCommState;
bool g_readSensors;

_iq g_currentPhaseA;
_iq g_currentPhaseB;
_iq g_currentPhaseC;

// Used for calibration
_iq g_currentOffsetPhaseA;
_iq g_currentOffsetPhaseB;
_iq g_currentOffsetPhaseC;

#endif /* HALLSENSOR_H_ */
