/*
 * MotorController.cpp
 *
 *  Created on: Oct 18, 2023
 *      Author: iris
 */

#include "MotorController.hpp"

namespace CubeRover
{
    void initMotorController(MotorController *mc, uint8_t id)
    {
        mc->mutex.lock();

        mc->i2c_addr = id + MC_SLAVE_I2C_ADDR_BASE;
        mc->up_to_date = false;

        mc->target_pos = 0;
        mc->target_vel = 0;

        mc->curr_pos = 0;
        mc->curr_vel = 0;
        mc->curr_current = 0;

        mc->current_p_val = 0;
        mc->current_i_val = 0;
        mc->vel_p_val = 0;
        mc->vel_i_val = 0;
        mc->acc_val = 0;
        mc->dec_val = 0;

        mc->ctrl = CMD_UNKNOWN;
        mc->state = STATE_IDLE;
        mc->fault = NO_FAULT;

        mc->mutex.unLock();
    }



    MC_ERR_t getMcRegVal(I2cSlaveAddress_t i2c_addr, RegisterAddress_t reg, uint32_t dataLen, void *_data)
    {
        MC_ERR_t err = NO_ERR;

        uint8_t *data = static_cast<uint8_t *>(_data);
        uint8_t reg_buffer = static_cast<uint8_t>(reg);

        taskENTER_CRITICAL();
        if (!i2cMasterReadData(MOTOR_CONTROL_I2CREG,
                               i2c_addr, reg_buffer,
                               dataLen, data))
        {
            err = ERR_i2c_WRITE;
        }
        taskEXIT_CRITICAL();

        return err;
    }

    MC_ERR_t setMcRegVal(I2cSlaveAddress_t i2c_addr, RegisterAddress_t reg, uint32_t dataLen, void *_data)
    {
        switch (reg)
        {
        case REG_I2C_ADDRESS:
        case REG_CURRENT_POSITION:
        case REG_CURRENT_SPEED:
        case REG_MOTOR_CURRENT:
        case REG_MC_STATUS:
        case REG_MC_FAULT:
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



    MC_ERR_t getMotorState(MotorController *mc)
    {
        MC_ERR_t err = NO_ERR;
        mc->mutex.lock();

        err = getMcRegVal(mc->i2c_addr, REG_MC_STATUS, sizeof(mc->state), &(mc->state));

        mc->mutex.unLock();
        return err;
    }

    MC_ERR_t getMotorFault(MotorController *mc)
    {
        MC_ERR_t err = NO_ERR;

        mc->mutex.lock();

        err = getMcRegVal(mc->i2c_addr, REG_MC_FAULT, sizeof(mc->fault), &(mc->fault));

        mc->mutex.unLock();

        return err;
    }



    /**
     * ----- STATE MACHINE TRANSITIONS -----
     */

    /**
     * mcEnable - Allow parameter updates
     */
    MC_ERR_t mcEnable(MotorController *mc)
    {
        MC_ERR_t err = NO_ERR;
        mc->mutex.lock();

        // Get MC current state
        err = getMcRegVal(mc->i2c_addr, REG_MC_STATUS, sizeof(mc->state), &(mc->state));

        // Send CMD_UPDATE_CONFIG to go from IDLE to ENABLE
        if (err == NO_ERR && mc->state == STATE_IDLE) {

            ControlValue update_config = CMD_UPDATE_CONFIG;
            err = setMcRegVal(mc->i2c_addr, REG_MC_CTRL, sizeof(update_config), &update_config);
            if (err == NO_ERR) {
                mc->state = STATE_ENABLED;
            }

        }

        if (err == NO_ERR && mc->state != STATE_ENABLED) {
            err = ERR_BAD_STATE;
        }

        mc->mutex.unLock();
        return err;
    }

    /**
     * mcArm - Wait for drive command
     */
    MC_ERR_t mcArm(MotorController *mc)
    {
        MC_ERR_t err = NO_ERR;
        mc->mutex.lock();

        // Get MC current state
        err = getMcRegVal(mc->i2c_addr, REG_MC_STATUS, sizeof(mc->state), &(mc->state));

        if (err == NO_ERR && mc->state == STATE_ENABLED) {
            ControlValue enable_driver = CMD_ENABLE_DRIVER;
            err = setMcRegVal(mc->i2c_addr, REG_MC_CTRL, sizeof(enable_driver), &enable_driver);
            if (err == NO_ERR) {
                mc->state = STATE_ARMED;
            }

        }

        if (err == NO_ERR && mc->state != STATE_ARMED) {
            err = ERR_BAD_STATE;
        }

        mc->mutex.unLock();
        return err;
    }

    /**
     * mcDrive - start movement
     */
    MC_ERR_t mcDrive(MotorController *mc)
    {
        MC_ERR_t err = NO_ERR;
        mc->mutex.lock();

        // Get MC current state
        err = getMcRegVal(mc->i2c_addr, REG_MC_STATUS, sizeof(mc->state), &(mc->state));

        if (err == NO_ERR && mc->state == STATE_ARMED) {
            ControlValue start_drive = CMD_EXECUTE_DRIVE;
            err = setMcRegVal(mc->i2c_addr, REG_MC_CTRL, sizeof(start_drive), &start_drive);
            if (err == NO_ERR) {
                mc->state = STATE_RUNNING;
            }

        }

        if (err == NO_ERR && mc->state != STATE_RUNNING) {
            err = ERR_BAD_STATE;
        }

        mc->mutex.unLock();
        return err;
    }

    /**
     * mcStop - stop movement
     */
    MC_ERR_t mcStop(MotorController *mc)
    {
        MC_ERR_t err = NO_ERR;
        mc->mutex.lock();

        ControlValue stop = CMD_DISABLE_DRIVER;
        err = setMcRegVal(mc->i2c_addr, REG_MC_CTRL, sizeof(stop), &stop);
        if (err == NO_ERR) {
            mc->state = STATE_DISABLED;
        }

        if (err == NO_ERR && mc->state != STATE_DISABLED) {
            err = ERR_BAD_STATE;
        }

        mc->mutex.unLock();
        return err;
    }

    /**
     * mcIdle - goto Idle
     */
    MC_ERR_t mcIdle(MotorController *mc)
    {
        MC_ERR_t err = NO_ERR;

        mc->mutex.lock();

        // Get MC current state
        err = getMcRegVal(mc->i2c_addr, REG_MC_STATUS, sizeof(mc->state), &(mc->state));
        if(err != NO_ERR) {
            mc->mutex.unLock();
            return err;
        }

        ControlValue disable = CMD_DISABLE_DRIVER;
        ControlValue idle = CMD_IDLE;

        switch (mc->state)
        {
        case STATE_ARMED:
        case STATE_TARGET_REACHED:
            err = setMcRegVal(mc->i2c_addr, REG_MC_CTRL, sizeof(disable), &disable);
        case STATE_ENABLED:
        case STATE_DISABLED:
            err = setMcRegVal(mc->i2c_addr, REG_MC_CTRL, sizeof(idle), &idle);
        case STATE_IDLE:
            break;
        default:
            err = ERR_BAD_STATE;
        }

        if (err == NO_ERR) {
            mc->state = STATE_IDLE;
        }

        mc->mutex.unLock();
        return err;
    }

    /**
     * mcClearFaults
     */
    MC_ERR_t mcClearFaults(MotorController *mc)
    {
        MC_ERR_t err = getMotorFault(mc);
        if (err != NO_ERR || mc->fault == NO_FAULT) {
            return err;
        }
        // SEND MOTOR FAULTS EVENT

        mc->mutex.lock();

        // Get MC current state
        err = getMcRegVal(mc->i2c_addr, REG_MC_STATUS, sizeof(mc->state), &(mc->state));

        if (err == NO_ERR && mc->state == STATE_IDLE) {
            ControlValue clearFaults = CMD_CLEAR_FAULTS;
            err = setMcRegVal(mc->i2c_addr, REG_MC_CTRL, sizeof(clearFaults), &clearFaults);
        }
        if (err == NO_ERR) {
            mc->fault = NO_FAULT;
        }

        mc->mutex.unLock();
        return err;
    }

    /**
     * mcReset
     */
    MC_ERR_t mcReset(MotorController *mc)
    {
        MC_ERR_t err = NO_ERR;
        mc->mutex.lock();

        ControlValue reset = CMD_RESET_CONTROLLER;
        err = setMcRegVal(mc->i2c_addr, REG_MC_CTRL, sizeof(reset), &reset);

        if (err == NO_ERR) {
            mc->state = STATE_DISABLED;
        }

        mc->mutex.unLock();
        return err;
    }





    MC_ERR_t setMotorTargetPos(MotorController *mc, uint32_t target_pos)
    {
        MC_ERR_t err = NO_ERR;
        mc->mutex.lock();

        if (mc->fault != NO_FAULT || mc->state != STATE_ENABLED)
        {
            mc->mutex.unLock();
            return ERR_BAD_STATE;
        }

        err = setMcRegVal(mc->i2c_addr, REG_TARGET_POSITION, sizeof(target_pos), &target_pos);
        if (err != NO_ERR) {
            mc->mutex.unLock();
            return err;
        }
        mc->target_pos = target_pos;

        mc->mutex.unLock();
        return err;
    }


    MC_ERR_t setAllMotorParams(MotorController *mc)
    {
        MC_ERR_t err = NO_ERR;
        mc->mutex.lock();

        if (mc->state != STATE_IDLE || mc->fault != NO_FAULT)
        {
            mc->mutex.unLock();
            return ERR_BAD_STATE;
        }

        mc->ctrl = CMD_UPDATE_CONFIG;
        err = setMcRegVal(mc->i2c_addr, REG_MC_CTRL, sizeof(mc->ctrl), &mc->ctrl);
        if (err != NO_ERR) {
            mc->mutex.unLock();
            return err;
        }

        err = setMcRegVal(mc->i2c_addr, REG_MC_CTRL, sizeof(mc->ctrl), &mc->ctrl);

        mc->mutex.unLock();
        return err;
    }

};
