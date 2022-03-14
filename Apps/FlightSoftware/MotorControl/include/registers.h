/*
 * registers.h
 *
 *  Created on: Mar 13, 2022
 *      Author: jonbs
 */

#ifndef INCLUDE_REGISTERS_H_
#define INCLUDE_REGISTERS_H_

// bits of control register
enum CONTROL_REGISTER_BITS {
    DRIVE_OPEN_LOOP = 1,       // first bit of control reg; drive only in open loop if set to 1
    CLEAR_DRIVER_FAULT = 2,       // second bit indicates request to try to clear fault in motor driver
    STATE_MACHINE_DISABLE = 4,
    STATE_MACHINE_RUN = 8,
    OVERRIDE_FAULT_DETECTION = 16,       // don't reset current & desired position if abnormal behavior detected
    EXECUTE_COMMAND = 32,       // actually drive to command
    OPEN_LOOP_TORQUE_OVERRIDE = 64       // use g_maxSpeed/MAX_TARGET_SPEED to calculate open loop PWM cycle (instead of constant at 0.3)
};

// bits of status register (shares 1,2,4 with control register)
enum STATUS_REGISTER_BITS {
    POSITION_CONVERGED = 8,
    CONTROLLER_ERROR = 16          // indicates something has gone awry with position controller; will not converge
};

// bits of fault register
enum FAULT_REGISTER_BITS {
    DRIVER_FAULT = 1,               // for if there is a fault in the DRV8304 motor drivers
    POSITION_NO_CHANGE = 2,         // for if position is not changing at all; could be dead hall sensors
    DRIVING_WRONG_DIRECTION = 4,    // for if motor is driving in wrong direction
    DRIVING_TIMEOUT = 8           // for if motor does not converge in time defined by DRIVING_TIMEOUT_THRESHOLD
};

typedef struct REGISTERS{
    uint8_t status_register;
    uint8_t control_register;
    uint8_t fault_register;
}REGISTERS;

#endif /* INCLUDE_REGISTERS_H_ */
