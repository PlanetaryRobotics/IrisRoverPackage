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


        mc->i2c_addr = id;
        mc->i2c_addr = MC_SLAVE_I2C_ADDR_BASE + id;
        mc->up_to_date = NO_UPDATES;

        mc->target_pos = 0;
        mc->target_speed = 0;

        mc->curr_pos = 0;
        mc->curr_speed = 0;
        mc->curr_current = 0;

        mc->current_p_val = DEFAULT_CURRENT_P;
        mc->current_i_val = DEFAULT_CURRENT_I;
        mc->speed_p_val = DEFAULT_SPEED_P;
        mc->speed_i_val = DEFAULT_SPEED_I;
        mc->acc_val = 0;
        mc->dec_val = 0;

        mc->ctrlReg = MC_NO_CMD;
        mc->stateReg = STATE_IDLE;
        mc->faultReg = MC_NO_FAULT;

//        // mc_mutex.unLock();
    }



    MC_ERR_t getMcRegVal(I2cSlaveAddress_t i2c_addr, MC_ICD_RegAddr reg, uint32_t dataLen, void *_data)
    {
        MC_ERR_t err = NO_ERR;

        uint8_t *data = static_cast<uint8_t *>(_data);
        uint8_t reg_buffer = static_cast<uint8_t>(reg);

        taskENTER_CRITICAL();
        if (!i2cMasterReadData(MOTOR_CONTROL_I2CREG,
                               i2c_addr, reg_buffer,
                               dataLen, data))
        {
            err = ERR_i2c_READ;
        }
        taskEXIT_CRITICAL();

        return err;
    }

    MC_ERR_t setMcRegVal(I2cSlaveAddress_t i2c_addr, MC_ICD_RegAddr reg, uint32_t dataLen, void *_data)
    {
        switch (reg)
        {
        case MC_REG_I2C_ADDRESS:
        case MC_REG_CURRENT_POSITION:
        case MC_REG_CURRENT_SPEED:
        case MC_REG_MOTOR_CURRENT:
        case MC_REG_MC_STATUS:
        case MC_REG_MC_FAULT:
            return ERR_WRITE_PROTECTED;
        default:
        }

        MC_ERR_t err = NO_ERR;

        uint8_t *data = static_cast<uint8_t *>(_data);
        uint8_t reg_buffer = static_cast<uint8_t>(reg);

        taskENTER_CRITICAL();
        if (!i2cMasterTransmit(MOTOR_CONTROL_I2CREG,
                               i2c_addr, reg_buffer,
                               dataLen, data))
        {
            err = ERR_i2c_WRITE;
        }
        taskEXIT_CRITICAL();

        return err;
    }



    MC_ERR_t getMcState(MotorControllerStruct *mc)
    {
        MC_ERR_t err = NO_ERR;
        // mc_mutex.lock();

        err = getMcRegVal(mc->i2c_addr, MC_REG_MC_STATUS, sizeof(mc->stateReg), &(mc->stateReg));

        // mc_mutex.unLock();
        return err;
    }

    MC_ERR_t getMcFault(MotorControllerStruct *mc)
    {
        MC_ERR_t err = NO_ERR;
        // mc_mutex.lock();

        err = getMcRegVal(mc->i2c_addr, MC_REG_MC_FAULT, sizeof(mc->faultReg), &(mc->faultReg));

        // mc_mutex.unLock();
        return err;
    }

    MC_ERR_t getMcAll(MotorControllerStruct *mc)
    {
        MC_ERR_t err = NO_ERR;
        int err_cnt = 0;
        int mismatched = 0;
        // mc_mutex.lock();

        // Must be in IDLE or FAULT state before getting _all_ register data
        err = getMcRegVal(mc->i2c_addr, MC_REG_TARGET_POSITION, sizeof(mc->stateReg), &(mc->stateReg));
        if (err == NO_ERR && (mc->stateReg != STATE_IDLE || mc->stateReg != STATE_FAULT)) {
            err = ERR_BAD_STATE;
        }
        if (err != NO_ERR) {
            // mc_mutex.unLock();
            return err;
        }

        // Get ctrl, state, & fault regs
        if (getMcRegVal(mc->i2c_addr, MC_REG_MC_CTRL, sizeof(mc->ctrlReg), &(mc->ctrlReg)) != NO_ERR) {
            err_cnt = err_cnt + 1;
        }
        if (getMcRegVal(mc->i2c_addr, MC_REG_MC_STATUS, sizeof(mc->stateReg), &(mc->stateReg)) != NO_ERR) {
            err_cnt = err_cnt + 1;
        }
        if (getMcRegVal(mc->i2c_addr, MC_REG_MC_FAULT, sizeof(mc->faultReg), &(mc->faultReg)) != NO_ERR) {
            err_cnt = err_cnt + 1;
        }

        // Get all other vals
        if (getMcRegVal(mc->i2c_addr, MC_REG_TARGET_POSITION, sizeof(mc->target_pos), &(mc->target_pos)) != NO_ERR) {
            err_cnt = err_cnt + 1;
        }
        if (getMcRegVal(mc->i2c_addr, MC_REG_TARGET_SPEED, sizeof(mc->target_speed), &(mc->target_speed)) != NO_ERR) {
            err_cnt = err_cnt + 1;
        }
        if (getMcRegVal(mc->i2c_addr, MC_REG_CURRENT_POSITION, sizeof(mc->curr_pos), &(mc->curr_pos)) != NO_ERR) {
            err_cnt = err_cnt + 1;
        }
        if (getMcRegVal(mc->i2c_addr, MC_REG_CURRENT_SPEED, sizeof(mc->curr_speed), &(mc->curr_speed)) != NO_ERR) {
            err_cnt = err_cnt + 1;
        }
        if (getMcRegVal(mc->i2c_addr, MC_REG_MOTOR_CURRENT, sizeof(mc->curr_current), &(mc->curr_current)) != NO_ERR) {
            err_cnt = err_cnt + 1;
        }
        if (getMcRegVal(mc->i2c_addr, MC_REG_P_CURRENT, sizeof(mc->current_p_val), &(mc->current_p_val)) != NO_ERR) {
            err_cnt = err_cnt + 1;
        }
        if (getMcRegVal(mc->i2c_addr, MC_REG_I_CURRENT, sizeof(mc->current_i_val), &(mc->current_i_val)) != NO_ERR) {
            err_cnt = err_cnt + 1;
        }
        if (getMcRegVal(mc->i2c_addr, MC_REG_P_SPEED, sizeof(mc->speed_p_val), &(mc->speed_p_val)) != NO_ERR) {
            err_cnt = err_cnt + 1;
        }
        if (getMcRegVal(mc->i2c_addr, MC_REG_I_SPEED, sizeof(mc->speed_i_val), &(mc->speed_i_val)) != NO_ERR) {
            err_cnt = err_cnt + 1;
        }
        if (getMcRegVal(mc->i2c_addr, MC_REG_ACC_RATE, sizeof(mc->acc_val), &(mc->acc_val)) != NO_ERR) {
            err_cnt = err_cnt + 1;
        }
        if (getMcRegVal(mc->i2c_addr, MC_REG_DEC_RATE, sizeof(mc->dec_val), &(mc->dec_val)) != NO_ERR) {
            err_cnt = err_cnt + 1;
        }

        // mc_mutex.unLock();
        return err;
    }


    MC_ERR_t setMcAll(MotorControllerStruct *mc)
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


    /**
     * ----- STATE MACHINE TRANSITIONS -----
     */

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
