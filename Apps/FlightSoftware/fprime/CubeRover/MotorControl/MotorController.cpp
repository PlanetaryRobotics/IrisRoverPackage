/*
 * MotorController.cpp
 *
 *  Created on: Oct 18, 2023
 *      Author: Raewyn
 */

#include "MotorController.hpp"
#include "FreeRTOS.h"
#include "os_task.h"

namespace CubeRover
{
    void initMotorController(MotorControllerStruct *mc, uint8_t id)
    {
        mc->i2c_addr = MC_SLAVE_I2C_ADDR_BASE + id;
        initMcRegStruct(&(mc->msp430_McRegStruct), mc->i2c_addr);
        initMcRegStruct(&(mc->herc_McRegStruct), MC_SLAVE_I2C_ADDR_BASE + id);

        mc->updateConfigVals = false;
        mc->currState = STATE_POWERED_OFF;

        mc->target_pos = 0;
        mc->target_speed = 0;
        mc->current_p_val = DEFAULT_CURRENT_P;
        mc->current_i_val = DEFAULT_CURRENT_I;
        mc->speed_p_val = DEFAULT_SPEED_P;
        mc->speed_i_val = DEFAULT_SPEED_I;
        mc->acc_val = 0;
        mc->dec_val = 0;
        mc->ctrlReg = MC_NO_CMD;
        mc->stateReg = MC_STATE_IDLE;
        mc->faultReg = MC_NO_FAULT;
    }


    // ----------------------------------------------------------------------
    // Motor Controller i2c Data Transfers
    // ----------------------------------------------------------------------

    MC_ERR_t readMcRegVal(MotorControllerStruct *mc, MC_ICD_RegAddr reg)
    {
        I2cSlaveAddress_t addr = mc->i2c_addr;
        uint8_t reg_buffer = static_cast<uint8_t>(reg);
        uint32_t dataLen = regSizeMap(reg);
        uint8_t *data;
        getReg(&(mc->msp430_McRegStruct), reg, data);

        if (data == NULL) {
            return ERR_GETTING_PARAMS;
        }

        MC_ERR_t err = NO_ERR;
        taskENTER_CRITICAL();
        if (!i2cMasterReadData(MOTOR_CONTROL_I2CREG,
                               addr, reg_buffer, dataLen, data))
        {
            err = ERR_i2c_READ;
        }
        taskEXIT_CRITICAL();
        return err;
    }

    MC_ERR_t writeMcRegVal(MotorControllerStruct *mc, MC_ICD_RegAddr reg)
    {
        if (reg == MC_REG_I2C_ADDRESS) {
            return ERR_WRITE_PROTECTED;
        }

        if (mc->currState != STATE_WRITE_PROTECTED &&
                (reg == MC_REG_CURRENT_POSITION ||
                 reg == MC_REG_CURRENT_SPEED ||
                 reg == MC_REG_MOTOR_CURRENT ||
                 reg == MC_REG_MC_STATUS ||
                 reg == MC_REG_MC_FAULT )) {
            return ERR_WRITE_PROTECTED;
        }

        I2cSlaveAddress_t addr = mc->i2c_addr;
        uint8_t reg_buffer = static_cast<uint8_t>(reg);
        uint32_t dataLen = regSizeMap(reg);;
        uint8_t *data;
        getReg(&(mc->herc_McRegStruct), reg, &data);

        if (data == NULL) {
            return ERR_GETTING_PARAMS;
        }

        MC_ERR_t err = NO_ERR;
        taskENTER_CRITICAL();
        if (!i2cMasterTransmit(MOTOR_CONTROL_I2CREG,
                               addr, reg_buffer, dataLen, data))
        {
            err = ERR_i2c_WRITE;
        }
        taskEXIT_CRITICAL();

        if(err == NO_ERR) {
            setReg(&(mc->msp430_McRegStruct), reg, &data);
        }

        return err;
    }


    // ----------------------------------------------------------------------
    // Safe Motor Controller Data Transfers/Updates
    // ----------------------------------------------------------------------

    

    MC_ERR_t getMcRegAll(MotorControllerStruct *mc)
    {
        MC_ERR_t err = NO_ERR;

        return err;
    }

    MC_ERR_t setMcRegAll(MotorControllerStruct *mc)
    {
        MC_ERR_t err = NO_ERR;
        int errs = 0;

        mc->herc_McRegStruct.mc_ctrlReg = MC_CMD_UPDATE_CONFIG;

        err = writeMcRegVal(mc, MC_REG_MC_CTRL);
        if (err != NO_ERR) {
            return err;
        }

        errs += writeMcRegVal(mc, MC_REG_TARGET_POSITION);
        errs += writeMcRegVal(mc, MC_REG_TARGET_SPEED);
        errs += writeMcRegVal(mc, MC_REG_P_CURRENT);
        errs += writeMcRegVal(mc, MC_REG_I_CURRENT);
        errs += writeMcRegVal(mc, MC_REG_P_SPEED);
        errs += writeMcRegVal(mc, MC_REG_I_SPEED);
        errs += writeMcRegVal(mc, MC_REG_ACC_RATE);
        errs += writeMcRegVal(mc, MC_REG_DEC_RATE);

        if (errs != 0)
        {
           err = ERR_SETTING_PARAMS;
        }

        return err;
    }


   // ----------------------------------------------------------------------
   // Set Herc MC Vals
   // ----------------------------------------------------------------------







   // ----------------------------------------------------------------------
   // Command handler implementations
   // ----------------------------------------------------------------------

   MC_ERR_t assertHercConfigState(MotorControllerStruct *mc)
    {
        MC_ERR_t err = NO_ERR;
        switch(mc->currState)
        {
        case STATE_POWERED_OFF:
        case STATE_IDLE:
        case STATE_WRITE_PROTECTED:
            err = NO_ERR;
            break;
        case STATE_ARMED:
        case STATE_RUNNING:
        case STATE_FAULT:
        case STATE_UNKNOWN:
        default:
            err = ERR_BAD_STATE;
            break;
        }
        return err;
    }

    MC_ERR_t assertMsp430ConfigAllowed(MotorControllerStruct *mc, MC_ICD_RegAddr reg)
    {
        MC_ERR_t err = ERR_WRITE_PROTECTED;
        uint8_t writePermission = checkRegWritePermission(reg);
        McStateVal_t msp430State = mc->msp430_McRegStruct.mc_stateReg;

        switch(msp430State)
        {
        case MC_STATE_ARMED:
        case MC_STATE_RUNNING:
        case MC_STATE_TARGET_REACHED:
        case MC_STATE_DISABLE:
        case MC_STATE_FAULT:
            if (writePermission > 3) {
                err = NO_ERR;
            }
            break;
        case MC_STATE_ENABLED:
            if (writePermission > 2) {
                err = NO_ERR;
            }
            break;
        case MC_STATE_IDLE:
            if (writePermission > 1) {
                err = NO_ERR;
            }
            break;
        case MC_STATE_WRITE_PROTECTED:
            if (writePermission > 0) {
                err = NO_ERR;
            }
            break;
        default:
            if (writePermission != 0) {
                err = UNKNOWN;
            }
            break;
        }
        return err;
    }


    /**
     * ----- UNSAFE TEST FUNCTIONS -----
     */

    /**
     * mcDrive - start movement
     */
    MC_ERR_t mcTestSetSpeed(MotorControllerStruct *mc)
    {
        MC_ERR_t err = NO_ERR;

        err = writeMcRegVal(mc, MC_REG_TARGET_SPEED);
//        uint8_t set_speed = mc->target_speed;
//        err = setMcRegVal(mc->i2c_addr, MC_REG_TARGET_SPEED, sizeof(set_speed), &set_speed);

        return err;
    }

    /**
     * mcDrive - start movement
     */
    MC_ERR_t mcTestSetPos(MotorControllerStruct *mc)
    {
        MC_ERR_t err = NO_ERR;


        err = writeMcRegVal(mc, MC_REG_TARGET_POSITION);
//        uint32_t set_pos = mc->target_pos;
//        err = setMcRegVal(mc->i2c_addr, MC_REG_TARGET_POSITION, sizeof(set_pos), &set_pos);

        return err;
    }

    /**
     * mcTestDrive - start movement
     */
    MC_ERR_t mcTestDrive(MotorControllerStruct *mc)
    {
        MC_ERR_t err = NO_ERR;


        err = writeMcRegVal(mc, MC_REG_MC_CTRL);
//        uint8_t start_drive = MC_CMD_EXECUTE_DRIVE;
//        err = setMcRegVal(mc->i2c_addr, MC_REG_MC_CTRL, sizeof(start_drive), &start_drive);

        return err;
    }


};
