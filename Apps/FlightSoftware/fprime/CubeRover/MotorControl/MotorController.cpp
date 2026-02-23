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
        initMcRegStruct(&(mc->herc_McRegStruct), mc->i2c_addr);

        mc->updateConfigVals = 0;
        mc->currState = STATE_POWERED_OFF;

        mc->target_pos = 0;
        mc->target_speed = 0;
        mc->current_p_val = DEFAULT_CURRENT_KP_IQ;
        mc->current_i_val = DEFAULT_CURRENT_KI_IQ;
        mc->speed_p_val = DEFAULT_SPEED_KP_IQ;
        mc->speed_i_val = DEFAULT_SPEED_KI_IQ;
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
        void *data;
        getReg(&(mc->msp430_McRegStruct), reg, &data);

        if (data == NULL || reg >= MC_REG_MAX) {
            return ERR_GETTING_PARAMS;
        }

        MC_ERR_t err = NO_ERR;
        taskENTER_CRITICAL();
        if (!i2cMasterReadData(MOTOR_CONTROL_I2CREG,
                               addr, reg_buffer, dataLen, (uint8_t*)data))
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

        McI2cDataPkt pkt = makeMcI2cDataPkt(&(mc->herc_McRegStruct), reg);
        I2cSlaveAddress_t addr = pkt.addr;
        uint8_t reg_buffer = pkt.regID;
        uint32_t dataLen = pkt.dataLen;
        uint8_t *data = static_cast<uint8_t *>(pkt.data);

        if (data == NULL) {
            return ERR_GETTING_PARAMS;
        }

        MC_ERR_t err = NO_ERR;
        taskENTER_CRITICAL();
        if (!i2cMasterTransmit(m_i2c,
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

#define NUM_RO_REGS 5
    MC_ICD_RegAddr readOnlyRegs[NUM_RO_REGS] =
    {
     MC_REG_CURRENT_POSITION,
     MC_REG_CURRENT_SPEED,
     MC_REG_MOTOR_CURRENT,
     MC_REG_MC_STATUS,
     MC_REG_MC_FAULT
    };
    
    MC_ERR_t getMcRegAll(MotorControllerStruct *mc)
    {
        bool err = false;
        MC_ERR_t errWrite = NO_ERR;

//        mc->herc_McRegStruct.mc_ctrlReg = MC_CMD_E_STOP;
//        errWrite = writeMcRegVal(mc, MC_REG_MC_CTRL);
//        if (errWrite != NO_ERR) {
//            return errWrite;
//        }

        for (uint8_t i = 0; i < NUM_RO_REGS; i++)
        {
            if (readMcRegVal(mc, readOnlyRegs[i]) != NO_ERR)
            {
                err = true;
            }
        }

//        mc->herc_McRegStruct.mc_ctrlReg = MC_NO_CMD;
//        errWrite = writeMcRegVal(mc, MC_REG_MC_CTRL);

        if (err) {
            return ERR_GETTING_PARAMS;
        } else {
            return errWrite;
        }
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

    void setTargetPos(MotorControllerStruct *mc, int32_t target_pos)
    {
        mc->herc_McRegStruct.mc_target_pos = target_pos;
        mc->updateConfigVals |= UPDATE_TARGET_POS;
    };
    void setTargetSpeed(MotorControllerStruct *mc, int32_t target_speed)
    {
        mc->herc_McRegStruct.mc_target_speed = target_speed;
        mc->updateConfigVals |= UPDATE_TARGET_SPEED;
    };
    void setCurrentP(MotorControllerStruct *mc, int8_t current_p_val)
    {
        mc->herc_McRegStruct.mc_piCurKp = current_p_val;
        mc->updateConfigVals |= UPDATE_CURRENT_P;
    };
    void setCurrentI(MotorControllerStruct *mc, int8_t current_i_val)
    {
        mc->herc_McRegStruct.mc_piCurKi = current_i_val;
        mc->updateConfigVals |= UPDATE_CURRENT_I;
    };
    void setSpeedP(MotorControllerStruct *mc, int8_t speed_p_val)
    {
        mc->herc_McRegStruct.mc_piSpdKp = speed_p_val;
        mc->updateConfigVals |= UPDATE_SPEED_P;
    };
    void setSpeedI(MotorControllerStruct *mc, int8_t speed_i_val)
    {
        mc->herc_McRegStruct.mc_piSpdKi = speed_i_val;
        mc->updateConfigVals |= UPDATE_SPEED_I;
    };
    void setAccVal(MotorControllerStruct *mc, int8_t acc_val)
    {
        mc->herc_McRegStruct.mc_acc_val = acc_val;
        mc->updateConfigVals |= UPDATE_ACC_RATE;
    };
    void setDecVal(MotorControllerStruct *mc, int8_t dec_val)
    {
        mc->herc_McRegStruct.mc_dec_val = dec_val;
        mc->updateConfigVals |= UPDATE_DEC_RATE;
    };





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

        if (mc->updateConfigVals & UPDATE_TARGET_SPEED) {
            err = writeMcRegVal(mc, MC_REG_TARGET_SPEED);

            if (err == NO_ERR) {
                mc->updateConfigVals &= ~UPDATE_TARGET_SPEED;
            }
        }

        return err;
    }

    /**
     * mcDrive - start movement
     */
    MC_ERR_t mcTestSetPos(MotorControllerStruct *mc)
    {
        MC_ERR_t err = NO_ERR;

        if (mc->updateConfigVals & UPDATE_TARGET_POS)
        {
            err = writeMcRegVal(mc, MC_REG_TARGET_POSITION);

            if (err == NO_ERR) {
                mc->updateConfigVals &= ~UPDATE_TARGET_POS;
            }
        }

        return err;
    }

    /**
     * mcTestDrive - start movement
     */
    MC_ERR_t mcTestDrive(MotorControllerStruct *mc)
    {
        MC_ERR_t err = NO_ERR;

        if (mc->currState == STATE_ARMED) {
            err = writeMcRegVal(mc, MC_REG_MC_CTRL);
            if (err == NO_ERR) {
                mc->currState = STATE_RUNNING;
            }
        }

        return err;
    }


};
