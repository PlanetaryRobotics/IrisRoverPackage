/*
 * mc.h
 *
 *  Created on: Oct 20, 2023
 *      Author: Raewyn
 */

#include "ICD_MotorControl.h"


void initMcRegStruct(MC_ICD_RegStruct *mcReg, McI2cAddr_t addr)
{
    mcReg->mc_i2c_addr = addr;

    mcReg->mc_target_pos = 0;
    mcReg->mc_target_speed = DEFAULT_TARGET_SPEED;

    mcReg->mc_curr_pos = 0; // ticks
    mcReg->mc_curr_speed = 0; // 0-100%
    mcReg->mc_curr_current = 0; // mA

    mcReg->mc_current_p_val = DEFAULT_CURRENT_P; // Linear Format
    mcReg->mc_current_i_val = DEFAULT_CURRENT_I;
    mcReg->mc_speed_p_val = DEFAULT_SPEED_P;
    mcReg->mc_speed_i_val = DEFAULT_SPEED_I;
    mcReg->mc_acc_val = 0; // ticks*s-2
    mcReg->mc_dec_val = 0;

    mcReg->mc_ctrlReg = MC_NO_CMD;
    mcReg->mc_stateReg = MC_STATE_IDLE;
    mcReg->mc_faultReg = MC_NO_FAULT;
}

McI2cDataPkt makeMcI2cDataPkt(MC_ICD_RegStruct *mcReg, MC_ICD_RegAddr regID)
{
    McI2cDataPkt dataPkt;
    dataPkt.addr = mcReg->mc_i2c_addr;
    dataPkt.regID = (uint8_t)regID;
    dataPkt.dataLen = 0;
    int32_t val;

    switch(regID)
    {
    case MC_REG_TARGET_POSITION:
        val = mcReg->mc_target_pos;
        dataPkt.dataLen = sizeof(int32_t);
        break;
    case MC_REG_TARGET_SPEED:
        val = mcReg->mc_target_speed;
        dataPkt.dataLen = sizeof(uint8_t);
        break;
    case MC_REG_CURRENT_POSITION:
        val = mcReg->mc_curr_pos;
        dataPkt.dataLen = sizeof(int32_t);
        break;
    case MC_REG_CURRENT_SPEED:
        val = mcReg->mc_curr_speed;
        dataPkt.dataLen = sizeof(uint8_t);
        break;
    case MC_REG_MOTOR_CURRENT:
        val = mcReg->mc_curr_current;
        dataPkt.dataLen = sizeof(int16_t);
        break;
    case MC_REG_P_CURRENT:
        val = mcReg->mc_current_p_val;
        dataPkt.dataLen = sizeof(uint16_t);
        break;
    case MC_REG_I_CURRENT:
        val = mcReg->mc_current_i_val;
        dataPkt.dataLen = sizeof(uint16_t);
        break;
    case MC_REG_P_SPEED:
        val = mcReg->mc_speed_p_val;
        dataPkt.dataLen = sizeof(uint16_t);
        break;
    case MC_REG_I_SPEED:
        val = mcReg->mc_speed_i_val;
        dataPkt.dataLen = sizeof(uint16_t);
        break;
    case MC_REG_ACC_RATE:
        val = mcReg->mc_acc_val;
        dataPkt.dataLen = sizeof(uint16_t);
        break;
    case MC_REG_DEC_RATE:
        val = mcReg->mc_dec_val;
        dataPkt.dataLen = sizeof(uint16_t);
        break;
    case MC_REG_MC_CTRL:
        val = mcReg->mc_ctrlReg;
        dataPkt.dataLen = sizeof(McCtrlVal_t);
        break;
    case MC_REG_MC_STATUS:
        val = mcReg->mc_stateReg;
        dataPkt.dataLen = sizeof(McStateVal_t);
        break;
    case MC_REG_MC_FAULT:
        val = mcReg->mc_faultReg;
        dataPkt.dataLen = sizeof(McFaultMask_t);
        break;
    default:
        // error so *data = NULL and datalen = 0
        break;
    }
    dataPkt.data = (uint8_t *)&val;
    return dataPkt;
}

uint8_t checkRegWritePermission(MC_ICD_RegAddr reg)
{
    uint8_t writePermission = 0;

    switch(reg)
    {
    // Always Allowed
    case MC_REG_MC_CTRL:
        writePermission = 4;
    // In WriteProtected, Idle, Enabled
    case MC_REG_TARGET_POSITION:
    case MC_REG_TARGET_SPEED:
        writePermission = 3;
    // In WriteProtected, Idle
    case MC_REG_P_CURRENT:
    case MC_REG_I_CURRENT:
    case MC_REG_P_SPEED:
    case MC_REG_I_SPEED:
    case MC_REG_ACC_RATE:
    case MC_REG_DEC_RATE:
        writePermission = 2;
    // In WriteProtected
    case MC_REG_MC_FAULT:
    case MC_REG_CURRENT_POSITION:
    case MC_REG_CURRENT_SPEED:
    case MC_REG_MOTOR_CURRENT:
        writePermission = 1;
    // Never
    case MC_REG_MC_STATUS:
    case MC_REG_I2C_ADDRESS:
    default:
        writePermission = 0;
        break;
    }
    
    return writePermission;
}

uint8_t checkCmdExecPermission(MC_ICD_Ctrl cmd)
{
    uint8_t execPermission = 0;

    switch(cmd)
    {
    // Always Allowed
    case MC_CMD_E_STOP:             // goto Fault ?
    case MC_CMD_DISABLE_DRIVER:     // goto Disable
    case MC_CMD_RESET_CONTROLLER:   // goto Fault ?
    case MC_NO_CMD:                 // do nothing
        execPermission = 4;
    // In Fault
    case MC_CMD_CLEAR_FAULTS:       // goto Idle
    // In Idle Only
    case MC_CMD_OVERRIDE_PROTECTED: // goto WriteProtected
    // In Idle or Enabled
    case MC_CMD_ENABLE_DRIVER:      // goto Enable
    case MC_CMD_UPDATE_CONFIG:      // Possibly goto Idle
    // In Armed Only
    case MC_CMD_EXECUTE_DRIVE:      // goto Running
    default:
    }
}