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
//        // mc_mutex.lock();

        initMcRegStruct(mc->msp430_McRegStruct, MC_SLAVE_I2C_ADDR_BASE + id);
        initMcRegStruct(mc->herc_McRegStruct, MC_SLAVE_I2C_ADDR_BASE + id);

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

//        // mc_mutex.unLock();
    }


    // ----------------------------------------------------------------------
    // Motor Controller i2c Data Transfers
    // ----------------------------------------------------------------------

    MC_ERR_t readMcRegVal(MotorControllerStruct *mc, MC_ICD_RegAddr reg)
    {
        McI2cDataPkt mcRegPkt = makeMcI2cDataPkt(mc->msp430_McRegStruct, reg);

        if (mcRegPkt.dataLen == 0) {
            return ERR_GETTING_PARAMS;
        }

        MC_ERR_t err = NO_ERR;
        taskENTER_CRITICAL();
        if (!i2cMasterReadData(MOTOR_CONTROL_I2CREG,
                               mcRegPkt.addr, mcRegPkt.regID,
                               mcRegPkt.dataLen, mcRegPkt.data))
        {
            err = ERR_i2c_READ;
        }
        taskEXIT_CRITICAL();

        return err;
    }

//    MC_ERR_t readMcRegI2c(McI2cAddr_t i2c_addr, MC_ICD_RegAddr reg, uint16_t dataLen, void *_data)
//    {
//        MC_ERR_t err = NO_ERR;
//        uint8_t *data = static_cast<uint8_t *>(_data);
//        uint8_t reg_buffer = static_cast<uint8_t>(reg);
//
//        taskENTER_CRITICAL();
//        if (!i2cMasterReadData(MOTOR_CONTROL_I2CREG,
//                               i2c_addr, reg_buffer,
//                               dataLen, data))
//        {
//            err = ERR_i2c_READ;
//        }
//        taskEXIT_CRITICAL();
//
//        return err;
//    }

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

        McI2cDataPkt mcRegPkt = makeMcI2cDataPkt(mc->herc_McRegStruct, reg);
        if (mcRegPkt.dataLen == 0) {
            return ERR_GETTING_PARAMS;
        }

        MC_ERR_t err = NO_ERR;
        taskENTER_CRITICAL();
        if (!i2cMasterTransmit(MOTOR_CONTROL_I2CREG,
                               mcRegPkt.addr, mcRegPkt.regID,
                               mcRegPkt.dataLen, mcRegPkt.data))
        {
            err = ERR_i2c_WRITE;
        }
        taskEXIT_CRITICAL();


        return err;
    }


    // ----------------------------------------------------------------------
    // Safe Motor Controller Data Transfers/Updates
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
        McStateVal_t msp430State = mc->msp430_McRegStruct->mc_stateReg;

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

    MC_ERR_t getMcRegAll(MotorControllerStruct *mc)
    {
        MC_ERR_t err = NO_ERR;

        return err;
    }

    MC_ERR_t setMcRegAll(MotorControllerStruct *mc)
    {
        MC_ERR_t err = mcEnable(mc);
        if (err != NO_ERR) { return err; }

        if (mc->up_to_date & UPDATE_TARGET_POS)
        {
            if (setMcRegVal(mc->i2c_addr, MC_REG_TARGET_POSITION, sizeof(mc->target_pos), &(mc->target_pos)) == NO_ERR) {
                mc->up_to_date &= ~UPDATE_TARGET_POS;
            }
        }

        if (mc->up_to_date & UPDATE_TARGET_SPEED)
        {
            if (setMcRegVal(mc->i2c_addr, MC_REG_TARGET_SPEED, sizeof(mc->target_speed), &(mc->target_speed)) == NO_ERR) {
                mc->up_to_date &= ~UPDATE_TARGET_SPEED;
            }
        }

        if (mc->up_to_date & UPDATE_CURRENT_P)
        {
            if (setMcRegVal(mc->i2c_addr, MC_REG_P_CURRENT, sizeof(mc->current_p_val), &(mc->current_p_val)) == NO_ERR) {
                mc->up_to_date &= ~UPDATE_CURRENT_P;
            }
        }

        if (mc->up_to_date & UPDATE_CURRENT_I)
        {
            if (setMcRegVal(mc->i2c_addr, MC_REG_I_CURRENT, sizeof(mc->current_i_val), &(mc->current_i_val)) == NO_ERR) {
                mc->up_to_date &= ~UPDATE_CURRENT_I;
            }
        }

        if (mc->up_to_date & UPDATE_SPEED_P)
        {
            if (setMcRegVal(mc->i2c_addr, MC_REG_P_SPEED, sizeof(mc->speed_p_val), &(mc->speed_p_val)) == NO_ERR) {
                mc->up_to_date &= ~UPDATE_SPEED_P;
            }
        }

        if (mc->up_to_date & UPDATE_SPEED_I)
        {
            if (setMcRegVal(mc->i2c_addr, MC_REG_I_SPEED, sizeof(mc->speed_i_val), &(mc->speed_i_val)) == NO_ERR) {
                mc->up_to_date &= ~UPDATE_SPEED_I;
            }
        }

        if (mc->up_to_date & UPDATE_ACC_RATE)
        {
            if (setMcRegVal(mc->i2c_addr, MC_REG_ACC_RATE, sizeof(mc->acc_val), &(mc->acc_val)) == NO_ERR) {
                mc->up_to_date &= ~UPDATE_ACC_RATE;
            }
        }

        if (mc->up_to_date & UPDATE_DEC_RATE)
        {
            if (setMcRegVal(mc->i2c_addr, MC_REG_DEC_RATE, sizeof(mc->dec_val), &(mc->dec_val)) == NO_ERR) {
                mc->up_to_date &= ~UPDATE_DEC_RATE;
            }
        }

        if (mc->up_to_date != 0)
        {
            err = ERR_UPDATING_ALL_PARAMS;
        }

        return err;
    }


    // ----------------------------------------------------------------------
    // Set Herc MC Vals
    // ----------------------------------------------------------------------

    MC_ERR_t setMcParam(MotorControllerStruct *mc);








    // ----------------------------------------------------------------------
    // Command handler implementations
    // ----------------------------------------------------------------------

    /**
     * mcEnable - Allow parameter updates
     */
    MC_ERR_t mcEnable(MotorControllerStruct *mc)
    {
        MC_ERR_t err = NO_ERR;
        // mc_mutex.lock();

        // Get MC current state
        err = getMcRegVal(mc->i2c_addr, MC_REG_MC_STATUS, sizeof(mc->stateReg), &(mc->stateReg));

        // Send MC_CMD_UPDATE_CONFIG to go from IDLE to ENABLE
        if (err == NO_ERR && mc->stateReg == STATE_IDLE) {

            MC_ICD_CmdVal update_config = MC_CMD_UPDATE_CONFIG;
            err = setMcRegVal(mc->i2c_addr, MC_REG_MC_CTRL, sizeof(update_config), &update_config);
            if (err == NO_ERR) {
                mc->stateReg = STATE_ENABLED;
            }

        }

        if (err == NO_ERR && mc->stateReg != STATE_ENABLED) {
            err = ERR_BAD_STATE;
        }

        // mc_mutex.unLock();
        return err;
    }

    /**
     * mcArm - Wait for drive command
     */
    MC_ERR_t mcArm(MotorControllerStruct *mc)
    {
        MC_ERR_t err = NO_ERR;
        // mc_mutex.lock();

        // Get MC current state
        err = getMcRegVal(mc->i2c_addr, MC_REG_MC_STATUS, sizeof(mc->stateReg), &(mc->stateReg));

        if (err == NO_ERR && mc->stateReg == STATE_ENABLED) {
            MC_ICD_CmdVal enable_driver = MC_CMD_ENABLE_DRIVER;
            err = setMcRegVal(mc->i2c_addr, MC_REG_MC_CTRL, sizeof(enable_driver), &enable_driver);
            if (err == NO_ERR) {
                mc->stateReg = STATE_ARMED;
            }

        }

        if (err == NO_ERR && mc->stateReg != STATE_ARMED) {
            err = ERR_BAD_STATE;
        }

        // mc_mutex.unLock();
        return err;
    }

    /**
     * mcDrive - start movement
     */
    MC_ERR_t mcDrive(MotorControllerStruct *mc)
    {
        MC_ERR_t err = NO_ERR;
        // mc_mutex.lock();

        // Get MC current state
        err = getMcRegVal(mc->i2c_addr, MC_REG_MC_STATUS, sizeof(mc->stateReg), &(mc->stateReg));

        if (err == NO_ERR && mc->stateReg == STATE_ARMED) {
            MC_ICD_CmdVal start_drive = MC_CMD_EXECUTE_DRIVE;
            err = setMcRegVal(mc->i2c_addr, MC_REG_MC_CTRL, sizeof(start_drive), &start_drive);
            if (err == NO_ERR) {
                mc->stateReg = STATE_RUNNING;
            }

        }

        if (err == NO_ERR && mc->stateReg != STATE_RUNNING) {
            err = ERR_BAD_STATE;
        }

        // mc_mutex.unLock();
        return err;
    }

    /**
     * mcStop - stop movement
     */
    MC_ERR_t mcStop(MotorControllerStruct *mc)
    {
        MC_ERR_t err = NO_ERR;
        // mc_mutex.lock();

        MC_ICD_CmdVal stop = MC_CMD_DISABLE_DRIVER;
        err = setMcRegVal(mc->i2c_addr, MC_REG_MC_CTRL, sizeof(stop), &stop);
        if (err == NO_ERR) {
            mc->stateReg = STATE_DISABLED;
        }

        if (err == NO_ERR && mc->stateReg != STATE_DISABLED) {
            err = ERR_BAD_STATE;
        }

        // mc_mutex.unLock();
        return err;
    }

    /**
     * mcIdle - goto Idle
     */
    MC_ERR_t mcIdle(MotorControllerStruct *mc)
    {
        MC_ERR_t err = NO_ERR;

        // mc_mutex.lock();

        // Get MC current state
        err = getMcRegVal(mc->i2c_addr, MC_REG_MC_STATUS, sizeof(mc->stateReg), &(mc->stateReg));
        if(err != NO_ERR) {
            // mc_mutex.unLock();
            return err;
        }

        MC_ICD_CmdVal disable = MC_CMD_DISABLE_DRIVER;
        MC_ICD_CmdVal idle = MC_CMD_E_STOP;

        switch (mc->stateReg)
        {
        case STATE_ARMED:
        case STATE_TARGET_REACHED:
            err = setMcRegVal(mc->i2c_addr, MC_REG_MC_CTRL, sizeof(disable), &disable);
        case STATE_ENABLED:
        case STATE_DISABLED:
            err = setMcRegVal(mc->i2c_addr, MC_REG_MC_CTRL, sizeof(idle), &idle);
        case STATE_IDLE:
            break;
        default:
            err = ERR_BAD_STATE;
        }

        if (err == NO_ERR) {
            mc->stateReg = STATE_IDLE;
        }

        // mc_mutex.unLock();
        return err;
    }

    /**
     * mcClearFaults
     */
    MC_ERR_t mcClearFaults(MotorControllerStruct *mc)
    {
        MC_ERR_t err = getMcFault(mc);
        if (err != NO_ERR || mc->faultReg == MC_NO_FAULT) {
            return err;
        }
        // SEND MOTOR FAULTS EVENT

        // mc_mutex.lock();

        // Get MC current state
        err = getMcRegVal(mc->i2c_addr, MC_REG_MC_STATUS, sizeof(mc->stateReg), &(mc->stateReg));

        if (err == NO_ERR && mc->stateReg == STATE_IDLE) {
            MC_ICD_CmdVal clearFaults = MC_CMD_CLEAR_FAULTS;
            err = setMcRegVal(mc->i2c_addr, MC_REG_MC_CTRL, sizeof(clearFaults), &clearFaults);
        }
        if (err == NO_ERR) {
            mc->faultReg = MC_NO_FAULT;
        }

        // mc_mutex.unLock();
        return err;
    }

    /**
     * mcReset
     */
    MC_ERR_t mcReset(MotorControllerStruct *mc)
    {
        MC_ERR_t err = NO_ERR;
        // mc_mutex.lock();

        MC_ICD_CmdVal reset = MC_CMD_RESET_CONTROLLER;
        err = setMcRegVal(mc->i2c_addr, MC_REG_MC_CTRL, sizeof(reset), &reset);

        if (err == NO_ERR) {
            mc->stateReg = STATE_DISABLED;
        }

        // mc_mutex.unLock();
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

        uint8_t set_speed = mc->target_speed;
        err = setMcRegVal(mc->i2c_addr, MC_REG_TARGET_SPEED, sizeof(set_speed), &set_speed);

        return err;
    }

    /**
     * mcDrive - start movement
     */
    MC_ERR_t mcTestSetPos(MotorControllerStruct *mc)
    {
        MC_ERR_t err = NO_ERR;

        uint32_t set_pos = mc->target_pos;
        err = setMcRegVal(mc->i2c_addr, MC_REG_TARGET_POSITION, sizeof(set_pos), &set_pos);

        return err;
    }

    /**
     * mcTestDrive - start movement
     */
    MC_ERR_t mcTestDrive(MotorControllerStruct *mc)
    {
        MC_ERR_t err = NO_ERR;

        uint8_t start_drive = MC_CMD_EXECUTE_DRIVE;
        err = setMcRegVal(mc->i2c_addr, MC_REG_MC_CTRL, sizeof(start_drive), &start_drive);

        return err;
    }


};
