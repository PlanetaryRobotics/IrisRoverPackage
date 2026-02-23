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
#include "ICD_MotorControl.h"

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
            ERR_BAD_REG_ADDR,
            ERR_BAD_STATE,
            ERR_WRITE_PROTECTED,
            ERR_GETTING_PARAMS,
            ERR_SETTING_PARAMS,
            UNKNOWN
        } MC_ERR_t;

        typedef enum
        {
            STATE_POWERED_OFF,
            STATE_IDLE,
            STATE_ENABLED,
            STATE_ARMED,
            STATE_RUNNING,
            STATE_WRITE_PROTECTED,
            STATE_FAULT,
            STATE_UNKNOWN
        }MC_STATE_t;

        typedef enum
        {
            NO_UPDATES = 0,
            UPDATE_TARGET_POS = 1,
            UPDATE_TARGET_SPEED = 2,
            UPDATE_CURRENT_P = 4,
            UPDATE_CURRENT_I = 8,
            UPDATE_SPEED_P = 16,
            UPDATE_SPEED_I = 32,
            UPDATE_ACC_RATE = 64,
            UPDATE_DEC_RATE = 128
        } UpdateMask;

        typedef uint8_t ControlRegister_t;
        typedef uint8_t StateRegister_t;
        typedef uint8_t FaultRegister_t;


        /**
         * @brief All important data related to the current motor controller state.
         */
        struct MotorControllerStruct
        {
            I2cSlaveAddress_t i2c_addr;
            
            // PROTECTED
            MC_ICD_RegStruct msp430_McRegStruct; // latest msp430 values
            MC_ICD_RegStruct herc_McRegStruct;   // expected values

            uint8_t updateConfigVals;      // request sent to update config vals at next opp
            MC_STATE_t currState;

            // TODO: Remove all below here

            uint8_t target_dir;

            // Expected Set Values
            int32_t target_pos;
            int16_t target_speed;

            int8_t current_p_val;
            int8_t current_i_val;
            int8_t speed_p_val;
            int8_t speed_i_val;
            int8_t acc_val;
            int8_t dec_val;

            McCtrlVal_t ctrlReg;
            McStateVal_t stateReg;
            McFaultMask_t faultReg;
        };

        void initMotorController(MotorControllerStruct *mc, uint8_t id);

        // NOT MUTEX SAFE
        MC_ERR_t readMcRegVal(MotorControllerStruct *mc, MC_ICD_RegAddr reg);
        MC_ERR_t writeMcRegVal(MotorControllerStruct *mc, MC_ICD_RegAddr reg);
//        MC_ERR_t readMcRegI2c(McI2cAddr_t i2c_addr, MC_ICD_RegAddr reg, uint32_t dataLen, void *_data);

        // MUTEX SAFE
        MC_ERR_t assertHercConfigState(MotorControllerStruct *mc);

        MC_ERR_t getMcRegAll(MotorControllerStruct *mc);
        MC_ERR_t setMcRegAll(MotorControllerStruct *mc);

        MC_ERR_t setMcParam(MotorControllerStruct *mc, MC_ICD_RegAddr param, uint32_t val);
        void setTargetPos(MotorControllerStruct *mc, int32_t target_pos);
        void setTargetSpeed(MotorControllerStruct *mc, int32_t target_speed);
        void setCurrentP(MotorControllerStruct *mc, int8_t current_p_val);
        void setCurrentI(MotorControllerStruct *mc, int8_t current_i_val);
        void setSpeedP(MotorControllerStruct *mc, int8_t speed_p_val);
        void setSpeedI(MotorControllerStruct *mc, int8_t speed_i_val);
        void setAccVal(MotorControllerStruct *mc, int8_t acc_val);
        void setDecVal(MotorControllerStruct *mc, int8_t dec_val);


        MC_ERR_t mcTestSetSpeed(MotorControllerStruct *mc);
        MC_ERR_t mcTestSetPos(MotorControllerStruct *mc);
        MC_ERR_t mcTestDrive(MotorControllerStruct *mc);
}


#endif /* CUBEROVER_MOTORCONTROL_MOTORCONTROLLER_HPP_ */
