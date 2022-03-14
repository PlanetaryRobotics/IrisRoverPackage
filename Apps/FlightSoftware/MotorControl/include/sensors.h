/*
 * sensors.h
 *
 *  Created on: Mar 13, 2022
 *      Author: jonbs
 */

#ifndef INCLUDE_SENSORS_H_
#define INCLUDE_SENSORS_H_

#include <stdint.h>
#include "driverlib.h"
#include "IQmathLib.h"
#define GLOBAL_IQ       15


typedef struct HALL_SENSOR{
    uint8_t pattern;
    uint8_t last_pattern;
    uint8_t comm_state;
    uint8_t last_comm_state;
    uint8_t comm_cycle_counter;

    uint8_t HALL_MAP[8];

    bool Event;
    bool Error;
}HALL_SENSOR;


typedef struct CURRENT_SENSOR{
    // Used for calibration
    _iq current_offset_phase_A;
    _iq current_offset_phase_B;
    _iq current_offset_phase_C;

    bool read_sensors;

}CURRENT_SENSOR;


void initializeHallInterface(volatile MOTOR* motor);
void initializeSensorVariables(volatile MOTOR* motor);
void currentOffsetCalibration(volatile MOTOR* motor);
inline void readHallSensor(volatile MOTOR* motor);
_iq getSpeed(volatile MOTOR* motor);
void readSensors(volatile MOTOR* motor);
#endif /* INCLUDE_SENSORS_H_ */
