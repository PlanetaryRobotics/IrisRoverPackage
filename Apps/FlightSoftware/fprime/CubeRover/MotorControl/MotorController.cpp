/*
 * MotorController.cpp
 *
 *  Created on: Oct 18, 2023
 *      Author: iris
 */

#include "MotorController.hpp"
#include "FreeRTOS.h"
#include "os_task.h"

namespace CubeRover
{
//    void initMotorController(MotorController *mc, uint8_t id)
//    {
//        mc->mc_mutex.lock();
//
//        mc->i2c_addr = id + MC_SLAVE_I2C_ADDR_BASE;
//        mc->up_to_date = NO_UPDATES;
//
//        mc->target_pos = 0;
//        mc->target_vel = 0;
//
//        mc->curr_pos = 0;
//        mc->curr_vel = 0;
//        mc->curr_current = 0;
//
//        mc->current_p_val = 0;
//        mc->current_i_val = 0;
//        mc->vel_p_val = 0;
//        mc->vel_i_val = 0;
//        mc->acc_val = 0;
//        mc->dec_val = 0;
//
//        mc->ctrl = CMD_UNKNOWN;
//        mc->state = STATE_IDLE;
//        mc->fault = NO_FAULT;
//
//        mc->mc_mutex.unLock();
//    }
//
//
//
//    MC_ERR_t getMcRegVal(I2cSlaveAddress_t i2c_addr, RegisterAddress_t reg, uint32_t dataLen, void *_data)
//    {
//        MC_ERR_t err = NO_ERR;
//
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
//
//    MC_ERR_t setMcRegVal(I2cSlaveAddress_t i2c_addr, RegisterAddress_t reg, uint32_t dataLen, void *_data)
//    {
//        switch (reg)
//        {
//        case REG_I2C_ADDRESS:
//        case REG_CURRENT_POSITION:
//        case REG_CURRENT_SPEED:
//        case REG_MOTOR_CURRENT:
//        case REG_MC_STATUS:
//        case REG_MC_FAULT:
//            return ERR_WRITE_PROTECTED;
//        default:
//        }
//
//        MC_ERR_t err = NO_ERR;
//
//        uint8_t *data = static_cast<uint8_t *>(_data);
//        uint8_t reg_buffer = static_cast<uint8_t>(reg);
//
//        taskENTER_CRITICAL();
//        if (!i2cMasterTransmit(MOTOR_CONTROL_I2CREG,
//                               i2c_addr, reg_buffer,
//                               dataLen, data))
//        {
//            err = ERR_i2c_WRITE;
//        }
//        taskEXIT_CRITICAL();
//
//        return err;
//    }
//
//
//
//    MC_ERR_t getMcState(MotorController *mc)
//    {
//        MC_ERR_t err = NO_ERR;
//        mc->mc_mutex.lock();
//
//        err = getMcRegVal(mc->i2c_addr, REG_MC_STATUS, sizeof(mc->state), &(mc->state));
//
//        mc->mc_mutex.unLock();
//        return err;
//    }
//
//    MC_ERR_t getMcFault(MotorController *mc)
//    {
//        MC_ERR_t err = NO_ERR;
//        mc->mc_mutex.lock();
//
//        err = getMcRegVal(mc->i2c_addr, REG_MC_FAULT, sizeof(mc->fault), &(mc->fault));
//
//        mc->mc_mutex.unLock();
//        return err;
//    }
//
//    MC_ERR_t getMcAll(MotorController *mc)
//    {
//        MC_ERR_t err = NO_ERR;
//        int err_cnt = 0;
//        int mismatched = 0;
//        mc->mc_mutex.lock();
//
//        // Must be in IDLE or FAULT state before getting _all_ register data
//        err = getMcRegVal(mc->i2c_addr, REG_TARGET_POSITION, sizeof(mc->state), &(mc->state));
//        if (err == NO_ERR && (mc->state != STATE_IDLE || mc->state != STATE_FAULT)) {
//            err = ERR_BAD_STATE;
//        }
//        if (err != NO_ERR) {
//            mc->mc_mutex.unLock();
//            return err;
//        }
//
//        // Get ctrl & fault regs
//        if (getMcRegVal(mc->i2c_addr, REG_TARGET_POSITION, sizeof(mc->ctrl), &(mc->ctrl)) != NO_ERR) {
//            err_cnt = err_cnt + 1;
//        }
//        if (getMcRegVal(mc->i2c_addr, REG_TARGET_POSITION, sizeof(mc->fault), &(mc->fault)) != NO_ERR) {
//            err_cnt = err_cnt + 1;
//        }
//
//        // Get all other vals
//        if (getMcRegVal(mc->i2c_addr, REG_TARGET_POSITION, sizeof(mc->target_pos), &(mc->target_pos)) != NO_ERR) {
//            err_cnt = err_cnt + 1;
//        }
//        if (getMcRegVal(mc->i2c_addr, REG_TARGET_POSITION, sizeof(mc->target_vel), &(mc->target_vel)) != NO_ERR) {
//            err_cnt = err_cnt + 1;
//        }
//        if (getMcRegVal(mc->i2c_addr, REG_TARGET_POSITION, sizeof(mc->curr_pos), &(mc->curr_pos)) != NO_ERR) {
//            err_cnt = err_cnt + 1;
//        }
//        if (getMcRegVal(mc->i2c_addr, REG_TARGET_POSITION, sizeof(mc->curr_vel), &(mc->curr_vel)) != NO_ERR) {
//            err_cnt = err_cnt + 1;
//        }
//        if (getMcRegVal(mc->i2c_addr, REG_TARGET_POSITION, sizeof(mc->curr_current), &(mc->curr_current)) != NO_ERR) {
//            err_cnt = err_cnt + 1;
//        }
//        if (getMcRegVal(mc->i2c_addr, REG_TARGET_POSITION, sizeof(mc->current_p_val), &(mc->current_p_val)) != NO_ERR) {
//            err_cnt = err_cnt + 1;
//        }
//        if (getMcRegVal(mc->i2c_addr, REG_TARGET_POSITION, sizeof(mc->current_i_val), &(mc->current_i_val)) != NO_ERR) {
//            err_cnt = err_cnt + 1;
//        }
//        if (getMcRegVal(mc->i2c_addr, REG_TARGET_POSITION, sizeof(mc->vel_p_val), &(mc->vel_p_val)) != NO_ERR) {
//            err_cnt = err_cnt + 1;
//        }
//        if (getMcRegVal(mc->i2c_addr, REG_TARGET_POSITION, sizeof(mc->vel_i_val), &(mc->vel_i_val)) != NO_ERR) {
//            err_cnt = err_cnt + 1;
//        }
//        if (getMcRegVal(mc->i2c_addr, REG_TARGET_POSITION, sizeof(mc->acc_val), &(mc->acc_val)) != NO_ERR) {
//            err_cnt = err_cnt + 1;
//        }
//        if (getMcRegVal(mc->i2c_addr, REG_TARGET_POSITION, sizeof(mc->dec_val), &(mc->dec_val)) != NO_ERR) {
//            err_cnt = err_cnt + 1;
//        }
//
//        mc->mc_mutex.unLock();
//        return err;
//    }
//
//
//    MC_ERR_t setMcAll(MotorController *mc)
//    {
//        MC_ERR_t err = mcEnable(mc);
//        if (err != NO_ERR) { return err; }
//
//        mc->mc_mutex.lock();
//
//        if (mc->up_to_date & UPDATE_TARGET_POS)
//        {
//            if (setMcRegVal(mc->i2c_addr, REG_TARGET_POSITION, sizeof(mc->target_pos), &(mc->target_pos)) == NO_ERR) {
//                mc->up_to_date &= ~UPDATE_TARGET_POS;
//            }
//        }
//
//        if (mc->up_to_date & UPDATE_TARGET_SPEED)
//        {
//            if (setMcRegVal(mc->i2c_addr, REG_TARGET_SPEED, sizeof(mc->target_vel), &(mc->target_vel)) == NO_ERR) {
//                mc->up_to_date &= ~UPDATE_TARGET_SPEED;
//            }
//        }
//
//        if (mc->up_to_date & UPDATE_CURRENT_PI)
//        {
//            if (setMcRegVal(mc->i2c_addr, REG_P_CURRENT, sizeof(mc->current_p_val), &(mc->current_p_val)) == NO_ERR &&
//                setMcRegVal(mc->i2c_addr, REG_I_CURRENT, sizeof(mc->current_i_val), &(mc->current_i_val)) == NO_ERR)
//            {
//                mc->up_to_date &= ~UPDATE_CURRENT_PI;
//            }
//        }
//
//        if (mc->up_to_date & UPDATE_VEL_PI)
//        {
//            if (setMcRegVal(mc->i2c_addr, REG_P_SPEED, sizeof(mc->vel_p_val), &(mc->vel_p_val)) == NO_ERR &&
//                setMcRegVal(mc->i2c_addr, REG_I_SPEED, sizeof(mc->vel_i_val), &(mc->vel_i_val)) == NO_ERR)
//            {
//                mc->up_to_date &= ~UPDATE_VEL_PI;
//            }
//        }
//
//        if (mc->up_to_date & UPDATE_ACC)
//        {
//            if (setMcRegVal(mc->i2c_addr, REG_ACC_RATE, sizeof(mc->acc_val), &(mc->acc_val)) == NO_ERR &&
//                setMcRegVal(mc->i2c_addr, REG_DEC_RATE, sizeof(mc->dec_val), &(mc->dec_val)) == NO_ERR)
//            {
//                mc->up_to_date &= ~UPDATE_ACC;
//            }
//        }
//
//        if (mc->up_to_date != 0)
//        {
//            err = ERR_UPDATING_ALL_PARAMS;
//        }
//
//        mc->mc_mutex.unLock();
//
//        return err;
//    }
//
//
//
//    /**
//     * ----- STATE MACHINE TRANSITIONS -----
//     */
//
//    /**
//     * mcEnable - Allow parameter updates
//     */
//    MC_ERR_t mcEnable(MotorController *mc)
//    {
//        MC_ERR_t err = NO_ERR;
//        mc->mc_mutex.lock();
//
//        // Get MC current state
//        err = getMcRegVal(mc->i2c_addr, REG_MC_STATUS, sizeof(mc->state), &(mc->state));
//
//        // Send CMD_UPDATE_CONFIG to go from IDLE to ENABLE
//        if (err == NO_ERR && mc->state == STATE_IDLE) {
//
//            CommandValue update_config = CMD_UPDATE_CONFIG;
//            err = setMcRegVal(mc->i2c_addr, REG_MC_CTRL, sizeof(update_config), &update_config);
//            if (err == NO_ERR) {
//                mc->state = STATE_ENABLED;
//            }
//
//        }
//
//        if (err == NO_ERR && mc->state != STATE_ENABLED) {
//            err = ERR_BAD_STATE;
//        }
//
//        mc->mc_mutex.unLock();
//        return err;
//    }
//
//    /**
//     * mcArm - Wait for drive command
//     */
//    MC_ERR_t mcArm(MotorController *mc)
//    {
//        MC_ERR_t err = NO_ERR;
//        mc->mc_mutex.lock();
//
//        // Get MC current state
//        err = getMcRegVal(mc->i2c_addr, REG_MC_STATUS, sizeof(mc->state), &(mc->state));
//
//        if (err == NO_ERR && mc->state == STATE_ENABLED) {
//            CommandValue enable_driver = CMD_ENABLE_DRIVER;
//            err = setMcRegVal(mc->i2c_addr, REG_MC_CTRL, sizeof(enable_driver), &enable_driver);
//            if (err == NO_ERR) {
//                mc->state = STATE_ARMED;
//            }
//
//        }
//
//        if (err == NO_ERR && mc->state != STATE_ARMED) {
//            err = ERR_BAD_STATE;
//        }
//
//        mc->mc_mutex.unLock();
//        return err;
//    }
//
//    /**
//     * mcDrive - start movement
//     */
//    MC_ERR_t mcDrive(MotorController *mc)
//    {
//        MC_ERR_t err = NO_ERR;
//        mc->mc_mutex.lock();
//
//        // Get MC current state
//        err = getMcRegVal(mc->i2c_addr, REG_MC_STATUS, sizeof(mc->state), &(mc->state));
//
//        if (err == NO_ERR && mc->state == STATE_ARMED) {
//            CommandValue start_drive = CMD_EXECUTE_DRIVE;
//            err = setMcRegVal(mc->i2c_addr, REG_MC_CTRL, sizeof(start_drive), &start_drive);
//            if (err == NO_ERR) {
//                mc->state = STATE_RUNNING;
//            }
//
//        }
//
//        if (err == NO_ERR && mc->state != STATE_RUNNING) {
//            err = ERR_BAD_STATE;
//        }
//
//        mc->mc_mutex.unLock();
//        return err;
//    }
//
//    /**
//     * mcStop - stop movement
//     */
//    MC_ERR_t mcStop(MotorController *mc)
//    {
//        MC_ERR_t err = NO_ERR;
//        mc->mc_mutex.lock();
//
//        CommandValue stop = CMD_DISABLE_DRIVER;
//        err = setMcRegVal(mc->i2c_addr, REG_MC_CTRL, sizeof(stop), &stop);
//        if (err == NO_ERR) {
//            mc->state = STATE_DISABLED;
//        }
//
//        if (err == NO_ERR && mc->state != STATE_DISABLED) {
//            err = ERR_BAD_STATE;
//        }
//
//        mc->mc_mutex.unLock();
//        return err;
//    }
//
//    /**
//     * mcIdle - goto Idle
//     */
//    MC_ERR_t mcIdle(MotorController *mc)
//    {
//        MC_ERR_t err = NO_ERR;
//
//        mc->mc_mutex.lock();
//
//        // Get MC current state
//        err = getMcRegVal(mc->i2c_addr, REG_MC_STATUS, sizeof(mc->state), &(mc->state));
//        if(err != NO_ERR) {
//            mc->mc_mutex.unLock();
//            return err;
//        }
//
//        CommandValue disable = CMD_DISABLE_DRIVER;
//        CommandValue idle = CMD_IDLE;
//
//        switch (mc->state)
//        {
//        case STATE_ARMED:
//        case STATE_TARGET_REACHED:
//            err = setMcRegVal(mc->i2c_addr, REG_MC_CTRL, sizeof(disable), &disable);
//        case STATE_ENABLED:
//        case STATE_DISABLED:
//            err = setMcRegVal(mc->i2c_addr, REG_MC_CTRL, sizeof(idle), &idle);
//        case STATE_IDLE:
//            break;
//        default:
//            err = ERR_BAD_STATE;
//        }
//
//        if (err == NO_ERR) {
//            mc->state = STATE_IDLE;
//        }
//
//        mc->mc_mutex.unLock();
//        return err;
//    }
//
//    /**
//     * mcClearFaults
//     */
//    MC_ERR_t mcClearFaults(MotorController *mc)
//    {
//        MC_ERR_t err = getMcFault(mc);
//        if (err != NO_ERR || mc->fault == NO_FAULT) {
//            return err;
//        }
//        // SEND MOTOR FAULTS EVENT
//
//        mc->mc_mutex.lock();
//
//        // Get MC current state
//        err = getMcRegVal(mc->i2c_addr, REG_MC_STATUS, sizeof(mc->state), &(mc->state));
//
//        if (err == NO_ERR && mc->state == STATE_IDLE) {
//            CommandValue clearFaults = CMD_CLEAR_FAULTS;
//            err = setMcRegVal(mc->i2c_addr, REG_MC_CTRL, sizeof(clearFaults), &clearFaults);
//        }
//        if (err == NO_ERR) {
//            mc->fault = NO_FAULT;
//        }
//
//        mc->mc_mutex.unLock();
//        return err;
//    }
//
//    /**
//     * mcReset
//     */
//    MC_ERR_t mcReset(MotorController *mc)
//    {
//        MC_ERR_t err = NO_ERR;
//        mc->mc_mutex.lock();
//
//        CommandValue reset = CMD_RESET_CONTROLLER;
//        err = setMcRegVal(mc->i2c_addr, REG_MC_CTRL, sizeof(reset), &reset);
//
//        if (err == NO_ERR) {
//            mc->state = STATE_DISABLED;
//        }
//
//        mc->mc_mutex.unLock();
//        return err;
//    }
//
//
//
//    void setTargetPos(MotorController *mc, int32_t target_pos)
//    {
//        mc->mc_mutex.lock();
//        mc->target_pos = target_pos;
//        mc->up_to_date |= UPDATE_TARGET_POS;
//        mc->mc_mutex.unLock();
//    }
//    void setTargetVel(MotorController *mc, int16_t target_vel)
//    {
//        mc->mc_mutex.lock();
//        mc->target_vel = target_vel;
//        mc->up_to_date |= UPDATE_TARGET_SPEED;
//        mc->mc_mutex.unLock();
//    }
//    void setCurrentP(MotorController *mc, int8_t current_p_val)
//    {
//        mc->mc_mutex.lock();
//        mc->current_p_val = current_p_val;
//        mc->up_to_date |= UPDATE_CURRENT_PI;
//        mc->mc_mutex.unLock();
//    }
//    void setCurrentI(MotorController *mc, int8_t current_i_val)
//    {
//        mc->mc_mutex.lock();
//        mc->current_i_val = current_i_val;
//        mc->up_to_date |= UPDATE_CURRENT_PI;
//        mc->mc_mutex.unLock();
//    }
//    void setVelP(MotorController *mc, int8_t vel_p_val)
//    {
//        mc->mc_mutex.lock();
//        mc->vel_p_val = vel_p_val;
//        mc->up_to_date |= UPDATE_VEL_PI;
//        mc->mc_mutex.unLock();
//    }
//    void setVelI(MotorController *mc, int8_t vel_i_val)
//    {
//        mc->mc_mutex.lock();
//        mc->vel_p_val = vel_i_val;
//        mc->up_to_date |= UPDATE_VEL_PI;
//        mc->mc_mutex.unLock();
//    }
//    void setAccVal(MotorController *mc, int8_t acc_val)
//    {
//        mc->mc_mutex.lock();
//        mc->acc_val = acc_val;
//        mc->up_to_date |= UPDATE_ACC;
//        mc->mc_mutex.unLock();
//    }
//    void setDecVal(MotorController *mc, int8_t dec_val)
//    {
//        mc->mc_mutex.lock();
//        mc->dec_val = dec_val;
//        mc->up_to_date |= UPDATE_ACC;
//        mc->mc_mutex.unLock();
//    }

};
