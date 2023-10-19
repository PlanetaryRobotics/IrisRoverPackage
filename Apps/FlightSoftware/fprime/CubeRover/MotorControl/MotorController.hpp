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
#include "Include/Shared_Libs/MotorControl/IrisMotorControlConfig.h"

#include <Os/Mutex.hpp>

#define MOTOR_CONTROL_I2CREG i2cREG1
//#define MC_SLAVE_I2C_ADDR_BASE 0x48
#define ALL_MOTOR_ID 0xFF


#define MAX_SPEED 100
#define MC_BUFFER_MAX_SIZE 16 // Maximum size of I2C buffer
#define START_MOTORS            32
#define NUM_MOTORS  4


namespace CubeRover
{
        MC_RegisterAddress test_regAddr;

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

        typedef enum
        {
            NO_UPDATES          = 0,
            UPDATE_TARGET_POS   = 1,
            UPDATE_TARGET_SPEED = 2,
            UPDATE_CURRENT_PI   = 4,
            UPDATE_VEL_PI       = 8,
            UPDATE_ACC          = 16
        } RegUpdateFlags;

        // --- MC i2c STATE Register Values
        typedef enum StateValue
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

        // TODO : REMOVE
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

        typedef uint8_t ControlRegister_t;
        typedef uint8_t StateRegister_t;
        typedef uint8_t FaultRegister_t;


        /**
         * @brief All important data related to the current motor controller state.
         */
        struct MotorControllerStruct
        {
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
            StateRegister_t state;
            FaultRegister_t fault;
        };

        void initMotorController(MotorControllerStruct *mc, uint8_t id);

        // NOT MUTEX SAFE
        MC_ERR_t getMcRegVal(I2cSlaveAddress_t i2c_addr, MC_RegisterAddress reg, uint32_t dataLen, void *_data);
        MC_ERR_t setMcRegVal(I2cSlaveAddress_t i2c_addr, MC_RegisterAddress reg, uint32_t dataLen, void *_data);

        // MUTEX SAFE
        MC_ERR_t getMcState(MotorControllerStruct *mc);
        MC_ERR_t getMcFault(MotorControllerStruct *mc);
        MC_ERR_t getMcAll(MotorControllerStruct *mc);

        MC_ERR_t setMcAll(MotorControllerStruct *mc);

        void setTargetPos(MotorControllerStruct *mc, int32_t target_pos);
        void setTargetVel(MotorControllerStruct *mc, int16_t target_vel);
        void setCurrentP(MotorControllerStruct *mc, int8_t current_p_val);
        void setCurrentI(MotorControllerStruct *mc, int8_t current_i_val);
        void setVelP(MotorControllerStruct *mc, int8_t vel_p_val);
        void setVelI(MotorControllerStruct *mc, int8_t vel_i_val);
        void setAccVal(MotorControllerStruct *mc, int8_t acc_val);
        void setDecVal(MotorControllerStruct *mc, int8_t dec_val);

        MC_ERR_t mcEnable(MotorControllerStruct *mc);
        MC_ERR_t mcArm(MotorControllerStruct *mc);
        MC_ERR_t mcDrive(MotorControllerStruct *mc);
        MC_ERR_t mcStop(MotorControllerStruct *mc);
        MC_ERR_t mcIdle(MotorControllerStruct *mc);
        MC_ERR_t mcClearFaults(MotorControllerStruct *mc);
        MC_ERR_t mcReset(MotorControllerStruct *mc);

}


#endif /* CUBEROVER_MOTORCONTROL_MOTORCONTROLLER_HPP_ */
