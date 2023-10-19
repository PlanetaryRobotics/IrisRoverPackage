/*
 * MotorController.hpp
 *
 *  Created on: Oct 18, 2023
 *      Author: Raewyn
 */

#ifndef CUBEROVER_MOTORCONTROL_MOTORCONTROLLER_HPP_
#define CUBEROVER_MOTORCONTROL_MOTORCONTROLLER_HPP_


#include "Fw/Types/BasicTypes.hpp"

#include "CubeRover/MotorControl/MotorControlComponentAc.hpp"
#include "MotorController_i2c.h"

#include <Os/Mutex.hpp>

#define MOTOR_CONTROL_I2CREG i2cREG1
#define MC_SLAVE_I2C_ADDR_BASE 0x48

#define DEFAULT_TARG_POS    20000
#define DEFAULT_TARG_VEL    80

#define DEFAULT_VEL_P       1.5
#define DEFAULT_VEL_I       0.0009
#define DEFAULT_CURRENT_P   0.95
#define DEFAULT_CURRENT_I   0.002



// STATUS Register Values


// FAULT Register Values

namespace CubeRover
{
        typedef enum
        {
            NO_ERR,
            ERR_i2c_READ,
            ERR_i2c_WRITE,
            ERR_BAD_STATE,
            ERR_WRITE_PROTECTED,
            ERR_UPDATING_ALL_PARAMS,
            UNKNOWN
        } MC_ERR_t;

        // --- MC i2c Register
        typedef enum
        {
            REG_I2C_ADDRESS = 0,        // Read-only  - 1Byte
            REG_TARGET_POSITION = 1,    // Write-only - 4Bytes
            REG_TARGET_SPEED = 2,       // Write-only - 1Byte
            REG_CURRENT_POSITION = 3,   // Read-only  - 4Bytes
            REG_CURRENT_SPEED = 4,      // DEPRICATED - 2Bytes
            REG_MOTOR_CURRENT = 5,      // Read-only  - 4Bytes
            REG_P_CURRENT = 6,          // Write-only - 2Bytes
            REG_I_CURRENT = 7,          // Write-only - 2Bytes
            REG_P_SPEED = 8,            // Write-only - 2Bytes
            REG_I_SPEED = 9,            // Write-only - 2Bytes
            REG_ACC_RATE = 10,          // Write-only - 2Bytes
            REG_DEC_RATE = 11,          // Write-only - 2Bytes
            REG_MC_CTRL = 12,           // Write-only - 1Byte
            REG_MC_STATUS = 13,         // Read-only  - 1Byte
            REG_MC_FAULT = 14,          // Read-only  - 1Byte
        } RegisterAddress_t;

        typedef enum
        {
            NO_UPDATES          = 0,
            UPDATE_TARGET_POS   = 1,
            UPDATE_TARGET_SPEED = 2,
            UPDATE_CURRENT_PI   = 4,
            UPDATE_VEL_PI       = 8,
            UPDATE_ACC          = 16
        } RegUpdateFlags;

        // --- MC i2c COMMAND Register Values
        typedef enum
        {
            CMD_UNKNOWN             = 0,
            CMD_UPDATE_CONFIG       = 1,
            CMD_EXECUTE_DRIVE       = 2,
            CMD_ENABLE_DRIVER       = 4,
            CMD_DISABLE_DRIVER      = 8,
            CMD_RESET_CONTROLLER    = 16,
            CMD_CLEAR_FAULTS        = 32,
            CMD_OVERRIDE_PROTECTED  = 64,
            CMD_IDLE                = 128
        } CommandValue;

        typedef uint8_t ControlRegister_t;

        // --- MC i2c STATE Register Values
        typedef enum
        {
            STATE_UNKNOWN           = 0,
            STATE_FAULT             = 1,
            STATE_IDLE              = 2,
            STATE_ENABLED           = 4,
            STATE_ARMED             = 8,
            STATE_RUNNING           = 16,
            STATE_TARGET_REACHED    = 32,
            STATE_DISABLED          = 64
        } StateValue;

        typedef uint8_t StateRegister_t;

        // --- MC i2c FAULT Register Values
        typedef enum
        {
            NO_FAULT                = 0,
            FAULT_I2C_ERROR         = 1,
            FAULT_BAD_CONFIG_VAL    = 2,
            FAULT_UNEXPECTED_STATE  = 4,
            FAULT_OVERCURRENT       = 8,
            FAULT_STALL             = 16,
            FAULT_POS_SENSOR_FAULT  = 32,
            FAULT_MC_WATCHDOG       = 64,
            FAULT_OTHER             = 128
        } FaultValue;

        typedef uint8_t FaultRegister_t;

//        // TODO : REMOVE
//        typedef union
//        {
//            uint8_t value;
//            struct
//            {
//                uint8_t open_loop : 1;
//                uint8_t clear_fault : 1;
//                uint8_t fsm_disable : 1;
//                uint8_t fsm_run : 1;
//                uint8_t override_fault_detection : 1;
//                uint8_t unused : 3;
//            } bits;
//        } ControlRegister_t;

        typedef union
        {
            uint8_t value;
            struct
            {
                uint8_t open_loop : 1;
                uint8_t clear_fault : 1;
                uint8_t fsm_disable : 1;
                uint8_t position_converged : 1;
                uint8_t controller_error : 1;
                uint8_t unused : 3;
            } bits;
        } StatusRegister_t;
//
//        typedef union
//        {
//            uint8_t value;
//            struct
//            {
//                uint8_t driver_fault : 1;
//                uint8_t position_no_change : 1;
//                uint8_t driving_wrong_direction : 1;
//                uint8_t unused : 5;
//            } bits;
//        } FaultRegister_t;


        /**
         * @brief All important data related to the current motor controller state.
         */
        struct MotorController
        {
            // Mutex that should be locked anytime the status is read or modified:
            ::Os::Mutex mc_mutex;
            uint8_t up_to_date;

            I2cSlaveAddress_t i2c_addr;

            int32_t target_pos;
            int16_t target_vel;

            int32_t curr_pos; // encoder ticks
            int16_t curr_vel;
            uint32_t curr_current;

            int8_t current_p_val;
            int8_t current_i_val;
            int8_t vel_p_val;
            int8_t vel_i_val;
            int8_t acc_val;
            int8_t dec_val;

            ControlRegister_t ctrl;
            StatusRegister_t state;
            FaultRegister_t fault;
        };

//        void initMotorController(MotorController *mc, uint8_t id);
//
//        // NOT MUTEX SAFE
//        MC_ERR_t getMcRegVal(I2cSlaveAddress_t i2c_addr, RegisterAddress_t reg, uint32_t dataLen, void *_data);
//        MC_ERR_t setMcRegVal(I2cSlaveAddress_t i2c_addr, RegisterAddress_t reg, uint32_t dataLen, void *_data);
//
//        // MUTEX SAFE
//        MC_ERR_t getMcState(MotorController *mc);
//        MC_ERR_t getMcFault(MotorController *mc);
//        MC_ERR_t getMcAll(MotorController *mc);
//
//        MC_ERR_t setMcAll(MotorController *mc);
//
//        MC_ERR_t mcEnable(MotorController *mc);
//        MC_ERR_t mcArm(MotorController *mc);
//        MC_ERR_t mcDrive(MotorController *mc);
//        MC_ERR_t mcStop(MotorController *mc);
//        MC_ERR_t mcIdle(MotorController *mc);
//        MC_ERR_t mcClearFaults(MotorController *mc);
//        MC_ERR_t mcReset(MotorController *mc);
//
//        void setTargetPos(MotorController *mc, int32_t target_pos);
//        void setTargetVel(MotorController *mc, int16_t target_vel);
//        void setCurrentP(MotorController *mc, int8_t current_p_val);
//        void setCurrentI(MotorController *mc, int8_t current_i_val);
//        void setVelP(MotorController *mc, int8_t vel_p_val);
//        void setVelI(MotorController *mc, int8_t vel_i_val);
//        void setAccVal(MotorController *mc, int8_t acc_val);
//        void setDecVal(MotorController *mc, int8_t dec_val);


}


#endif /* CUBEROVER_MOTORCONTROL_MOTORCONTROLLER_HPP_ */
