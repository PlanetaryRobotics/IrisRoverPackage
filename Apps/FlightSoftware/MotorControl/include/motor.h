/*
 * state_machine.h
 *
 *  Created on: Mar 13, 2022
 *      Author: jonbs
 */

#ifndef INCLUDE_MOTOR_H_
#define INCLUDE_MOTOR_H_

#include <msp430.h>
#include "include/sensors.h"
#include "include/bsp.h"
#include "include/registers.h"
#include "include/utils.h"

// internal state machine for motor
typedef enum {
    IDLE,           // motor driver turned off, target & current position reset to 0
    RUNNING         // actively trying to converge to target positions
}State;

// possible transition options for internal state machine
typedef enum {
    RUN,            // if in IDLE, switch to RUNNING
    DISABLE,        // if in RUNNING, switch to IDLE
    NO_CMD          // don't change state
}CmdState;

typedef struct STATE_MACHINE{
    State state;
    CmdState command;
}STATE_MACHINE;


typedef struct MOTOR{
    STATE_MACHINE state_machine;
    int32_t current_position;
    int32_t target_position;
    int8_t target_direction;
    int32_t last_position;

    _iq current_speed;
    bool closed_loop;
    bool target_reached;

    bool read_sensors;
    HALL_SENSOR hall_sensor;
    CURRENT_SENSOR current_sensor;

    uint32_t driving_timeout_ctr;
    REGISTERS registers;
}MOTOR;

void disable(volatile MOTOR* motor);
void run(volatile MOTOR* motor);
void updateStateMachine(volatile MOTOR* motor);
void handleMotorTimeout(volatile MOTOR* motor);
void initializeHallInterface(volatile MOTOR* motor);
void initializeSensorVariables(volatile MOTOR* motor);
void currentOffsetCalibration(volatile MOTOR* motor);
inline void readHallSensor(volatile MOTOR* motor);
_iq getSpeed(volatile MOTOR* motor);
//void readSensors(volatile MOTOR* motor);

#endif /* INCLUDE_MOTOR_H_ */
