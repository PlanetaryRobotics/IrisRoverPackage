// ======================================================================
// \title  i2c.h
// \author cedric
// \edited by Jonathan
// \brief  header file for i2c interface between MSP430 motor controller
//         (this device) and the Hercules microcontroller
//
// ======================================================================

#ifndef I2C_H_
#define I2C_H_

// variables for i2c functionality
#define I2C_RX_BUFFER_MAX_SIZE      8
#define I2C_TX_BUFFER_MAX_SIZE      8
#define I2C_MAX_DATA_SIZE           4
#define I2C_PACKET_HEADER           0xAA

#include "driverlib.h"
#include "MotorControl.h"
#include "main.h"

// Register ids for i2c communication with Hercules
typedef enum I2cRegisterIds{
    I2C_ADDRESS = 0,
    TARGET_POSITION = 1,
    TARGET_SPEED = 2,
    CURRENT_POSITION = 3,
    CURRENT_SPEED = 4,
    MOTOR_CURRENT = 5,
    P_CURRENT = 6,
    I_CURRENT = 7,
    P_SPEED = 8,
    I_SPEED = 9,
    ACC_RATE = 10,
    DEC_RATE = 11,
    CONTROL_REGISTER = 12,
    STATUS_REGISTER = 13,
    FAULT_REGISTER = 14,
    CLEAR_FAULT_REGISTER = 15,
    MAX_NB_CMDS = 15
}I2cRegisterIds;

// Different modes for i2c functionality
//  _REG_ADDRESS_MODE -> receiving register id from Hercules that it wants to interact with
//  _DATA_MODE        -> ready to write or read bytes of data to/from Herules
typedef enum I2cMode{
    RX_REG_ADDRESS_MODE = 0,
    TX_DATA_MODE = 1,
    RX_DATA_MODE = 2
}I2cMode;

McI2cAddr_t initializeI2cModule();

#endif /* I2C_H_ */
