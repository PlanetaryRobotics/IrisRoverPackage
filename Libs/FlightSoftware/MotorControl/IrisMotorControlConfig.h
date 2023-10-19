/*
 * mc.h
 *
 *  Created on: Oct 19, 2023
 *      Author: Raewyn
 */

#ifndef IRISMOTORCONTROLCONFIG_H_
#define IRISMOTORCONTROLCONFIG_H_

#include <stdint.h>

#define I2C_RX_BUFFER_MAX_SIZE      8
#define I2C_TX_BUFFER_MAX_SIZE      8
#define I2C_MAX_DATA_SIZE           4

#define MC_SLAVE_I2C_ADDR_BASE      0x48


#define DEFAULT_TARGET_POS  20000
#define DEFAULT_TARGET_VEL  70
#define MAX_TARGET_VEL      100

#define DEFAULT_VEL_P       15000   // e-4 (1.5000) (0x3A98)
#define DEFAULT_VEL_I       9       // e-4 (0.0009) (0x0009)
#define DEFAULT_CURRENT_P   9500    // e-4 (0.9500) (0x005F)
#define DEFAULT_CURRENT_I   20      // e-4 (0.0020) (0x0014)


#ifdef FPRIME_BUILD
namespace CubeRover
{
#endif

    // --- MC i2c Register
    typedef enum MC_RegisterAddress
    {
        MC_REG_I2C_ADDRESS         = 0,    // RO - 1 Byte
        MC_REG_TARGET_POSITION     = 1,    // RW - 4 Bytes
        MC_REG_TARGET_SPEED        = 2,    // RW - 1 Byte
        MC_REG_CURRENT_POSITION    = 3,    // RO - 4 Bytes
        MC_REG_CURRENT_SPEED       = 4,    // RO - 1 Bytes
        MC_REG_MOTOR_CURRENT       = 5,    // RO - 4 Bytes
        MC_REG_P_CURRENT           = 6,    // RW - 2 Bytes
        MC_REG_I_CURRENT           = 7,    // RW - 2 Bytes
        MC_REG_P_SPEED             = 8,    // RW - 2 Bytes
        MC_REG_I_SPEED             = 9,    // RW - 2 Bytes
        MC_REG_ACC_RATE            = 10,   // RW - 2 Bytes
        MC_REG_DEC_RATE            = 11,   // RW - 2 Bytes
        MC_REG_MC_CTRL             = 12,   // RW - 1 Byte
        MC_REG_MC_STATUS           = 13,   // RO - 1 Byte
        MC_REG_MC_FAULT            = 14,   // RO - 1 Byte
        MC_REG_MAX                 = 16
    } MC_RegisterAddress;

    //uint8_t mc_i2cCmdLen[MC_REG_MAX] = {
    //                                 1, // MC_REG_I2C_ADDRESS
    //                                 4, // MC_REG_TARGET_POSITION
    //                                 1, // MC_REG_TARGET_SPEED
    //                                 4, // MC_REG_CURRENT_POSITION
    //                                 1, // MC_REG_CURRENT_SPEED
    //                                 2, // MC_REG_MOTOR_CURRENT
    //                                 2, // MC_REG_P_CURRENT
    //                                 2, // MC_REG_I_CURRENT
    //                                 2, // MC_REG_P_VEL
    //                                 2, // MC_REG_I_VEL
    //                                 2, // MC_REG_ACC_RATE
    //                                 2, // MC_REG_DEC_RATE
    //                                 1, // MC_REG_MC_CTRL
    //                                 1, // MC_REG_MC_STATUS
    //                                 1, // MC_REG_MC_FAULT
    //                                 0  // ...err
    //};

    // --- MC i2c COMMAND Register Values
    typedef enum MC_CommandValue
    {
        MC_NO_CMD                  = 0,
        MC_CMD_UPDATE_CONFIG       = 1,    // if STATE_ENABLED : goto STATE_IDLE
        MC_CMD_CLEAR_FAULTS        = 2,    // if STATE_FAULT : Clear Faults , goto STATE_IDLE
        MC_CMD_ENABLE_DRIVER       = 4,    // if STATE_IDLE & params set : goto STATE_ENABLED
        MC_CMD_DISABLE_DRIVER      = 8,    // Disable Drivers , goto STATE_IDLE
        MC_CMD_RESET_CONTROLLER    = 16,   // Reset Controller
        MC_CMD_EXECUTE_DRIVE       = 32,   // if STATE_ENABLED : goto STATE_RUNNING
        MC_CMD_OVERRIDE_PROTECTED  = 64,   // ...?
        MC_CMD_E_STOP              = 128   // Disable Drivers , set Targets = 0 , goto STATE_IDLE
    } MC_CommandValue;

    // --- MC i2c STATE Register Values
    typedef enum MC_StateValue
    {
        MC_STATE_IDLE              = 0,    // 0: Driver Disabled
                                        //    Updating Parameters
        MC_STATE_ENABLED           = 1,    // 1: Driver Enabled
                                        //    Waiting for MC_CMD_EXECUTE_DRIVE
        MC_STATE_RUNNING           = 2,    // (2: FAULT_UNEXPECTED_STATE ?)
                                        // 3: Driving
        MC_STATE_TARGET_REACHED    = 4,    // 4: Target reached (Driver Disabled)
                                        // (5,6,7: FAULT_UNEXPECTED_STATE ?)
        MC_STATE_FAULT             = 128,  // 128: Fault Detected
    } MC_StateValue;

    // --- MC i2c FAULT Register Values
    typedef enum MC_FaultValue
    {
        MC_NO_FAULT                = 0,
        MC_FAULT_I2C_ERROR         = 1,
        MC_FAULT_BAD_CONFIG_VAL    = 2,
        MC_FAULT_UNEXPECTED_STATE  = 4,
        MC_FAULT_OVERCURRENT       = 8,
        MC_FAULT_STALL             = 16,
        MC_FAULT_POS_SENSOR_FAULT  = 32,
        MC_FAULT_MC_WATCHDOG       = 64,
        MC_FAULT_OTHER             = 128
    } MC_FaultValue;

    typedef struct MC_Instance
    {
        uint8_t up_to_date;

        uint8_t i2c_addr;

        int32_t target_pos; // ticks
        uint8_t target_vel; // 0-100%

        int32_t curr_pos; // ticks
        uint8_t curr_vel; // 0-100%
        int16_t curr_current; // mA

        uint16_t current_p_val; // Linear Format
        uint16_t current_i_val;
        uint16_t vel_p_val;
        uint16_t vel_i_val;
        uint16_t acc_val; // ticks*s-2
        uint16_t dec_val;

        uint8_t ctrl;
        uint8_t state;
        uint8_t fault;
    } MC_Instance;

#ifdef FPRIME_BUILD
}
#endif

#endif /* IRISMOTORCONTROLCONFIG_H_ */
