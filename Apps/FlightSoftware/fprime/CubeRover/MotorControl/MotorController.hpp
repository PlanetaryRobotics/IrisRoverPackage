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
            UPDATE_CURRENT_P    = 4,
            UPDATE_CURRENT_I    = 8,
            UPDATE_SPEED_P      = 16,
            UPDATE_SPEED_I      = 32,
            UPDATE_ACC_RATE     = 64,
            UPDATE_DEC_RATE     = 128
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
            int16_t target_speed;

            int32_t curr_pos; // encoder ticks
            int16_t curr_speed;
            uint32_t curr_current;

            int8_t current_p_val;
            int8_t current_i_val;
            int8_t speed_p_val;
            int8_t speed_i_val;
            int8_t acc_val;
            int8_t dec_val;

            ControlRegister_t ctrlReg;
            StateRegister_t stateReg;
            FaultRegister_t faultReg;
        };

        void initMotorController(MotorControllerStruct *mc, uint8_t id);


        // NOT MUTEX SAFE
        MC_ERR_t getMcRegVal(I2cSlaveAddress_t i2c_addr, MC_ICD_RegAddr reg, uint32_t dataLen, void *_data);
        MC_ERR_t setMcRegVal(I2cSlaveAddress_t i2c_addr, MC_ICD_RegAddr reg, uint32_t dataLen, void *_data);

        // MUTEX SAFE
        MC_ERR_t getMcState(MotorControllerStruct *mc);
        MC_ERR_t getMcFault(MotorControllerStruct *mc);
        MC_ERR_t getMcAll(MotorControllerStruct *mc);

        MC_ERR_t setMcAll(MotorControllerStruct *mc);


        inline void setTargetPos(MotorControllerStruct *mc, int32_t target_pos)
        {
            mc->target_pos = target_pos;
            mc->up_to_date |= UPDATE_TARGET_POS;
        }
        inline void setTargetSpeed(MotorControllerStruct *mc, int8_t target_speed)
        {
            mc->target_speed = target_speed;
            mc->up_to_date |= UPDATE_TARGET_SPEED;
        }
        inline void setCurrentP(MotorControllerStruct *mc, int8_t current_p_val)
        {
            mc->current_p_val = current_p_val;
            mc->up_to_date |= UPDATE_CURRENT_P;
        }
        inline void setCurrentI(MotorControllerStruct *mc, int8_t current_i_val)
        {
            mc->current_i_val = current_i_val;
            mc->up_to_date |= UPDATE_CURRENT_I;
        }
        inline void setSpeedP(MotorControllerStruct *mc, int8_t speed_p_val)
        {
            mc->speed_p_val = speed_p_val;
            mc->up_to_date |= UPDATE_SPEED_P;
        }
        inline void setSpeedI(MotorControllerStruct *mc, int8_t speed_i_val)
        {
            mc->speed_p_val = speed_i_val;
            mc->up_to_date |= UPDATE_SPEED_I;
        }
        inline void setAccVal(MotorControllerStruct *mc, int8_t acc_val)
        {
            mc->acc_val = acc_val;
            mc->up_to_date |= UPDATE_ACC_RATE;
        }
        inline void setDecVal(MotorControllerStruct *mc, int8_t dec_val)
        {
            mc->dec_val = dec_val;
            mc->up_to_date |= UPDATE_DEC_RATE;
        }

        MC_ERR_t mcEnable(MotorControllerStruct *mc);
        MC_ERR_t mcArm(MotorControllerStruct *mc);
        MC_ERR_t mcDrive(MotorControllerStruct *mc);
        MC_ERR_t mcStop(MotorControllerStruct *mc);
        MC_ERR_t mcIdle(MotorControllerStruct *mc);
        MC_ERR_t mcClearFaults(MotorControllerStruct *mc);
        MC_ERR_t mcReset(MotorControllerStruct *mc);

        MC_ERR_t mcTestSetSpeed(MotorControllerStruct *mc);
        MC_ERR_t mcTestSetPos(MotorControllerStruct *mc);
        MC_ERR_t mcTestDrive(MotorControllerStruct *mc);

}


#endif /* CUBEROVER_MOTORCONTROL_MOTORCONTROLLER_HPP_ */
