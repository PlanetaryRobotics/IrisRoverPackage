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
    mcReg->mc_target_speed = DEFAULT_TARGET_SPEED_IQ;

    mcReg->mc_curr_pos = 0; // ticks
    mcReg->mc_curr_speed = 0; // 0-100%
    mcReg->mc_curr_current = 0; // mA


    mcReg->mc_ctrlReg = MC_NO_CMD;
    mcReg->mc_stateReg = MC_STATE_IDLE;
    mcReg->mc_faultReg = MC_NO_FAULT;

    // PROTECTED => MC_CMD_OVERRIDE_PROTECTED;
    // ONLY WRITE IF (mc_stateReg & MC_STATUS_WRITE_PROTECTED);
    mcReg->mc_piCurKp = DEFAULT_CURRENT_KP_IQ; // Linear Format
    mcReg->mc_piCurKi = DEFAULT_CURRENT_KI_IQ;
    mcReg->mc_piSpdKp = DEFAULT_SPEED_KP_IQ;
    mcReg->mc_piSpdKi = DEFAULT_SPEED_KI_IQ;
    // mcReg->mc_acc_val = 0; // ticks*s-2
    // mcReg->mc_dec_val = 0;
    mcReg->mc_maxCurrent = 0;   // UNKNOWN
    mcReg->mc_ignoreFaults = 0xFF; // mask so 0 will ignore fault
}

McI2cDataPkt makeMcI2cDataPkt(MC_ICD_RegStruct *mcReg, MC_ICD_RegAddr regID)
{
    McI2cDataPkt dataPkt;
    dataPkt.addr = mcReg->mc_i2c_addr;
    dataPkt.regID = (uint8_t)regID;
    dataPkt.dataLen = 0;

    switch(regID)
    {
    case MC_REG_I2C_ADDRESS:
        dataPkt.data = &(mcReg->mc_i2c_addr);
        dataPkt.dataLen = sizeof(mcReg->mc_i2c_addr);
        dataPkt.access = READ_ONLY;
    case MC_REG_TARGET_POSITION:
        dataPkt.data = &(mcReg->mc_target_pos);
        dataPkt.dataLen = sizeof(mcReg->mc_target_pos);
        dataPkt.access = READ_WRITE;
        break;
    case MC_REG_TARGET_SPEED:
        dataPkt.data = &(mcReg->mc_target_speed);
        dataPkt.dataLen = sizeof(mcReg->mc_target_speed);
        dataPkt.access = READ_WRITE;
        break;
    case MC_REG_CURRENT_POSITION:
        dataPkt.data = &(mcReg->mc_curr_pos);
        dataPkt.dataLen = sizeof(mcReg->mc_curr_pos);
        dataPkt.access = READ_ONLY;
        break;
    case MC_REG_CURRENT_SPEED:
        dataPkt.data = &(mcReg->mc_curr_speed);
        dataPkt.dataLen = sizeof(mcReg->mc_curr_speed);
        dataPkt.access = READ_ONLY;
        break;
    case MC_REG_MOTOR_CURRENT:
        dataPkt.data = &(mcReg->mc_curr_current);
        dataPkt.dataLen = sizeof(mcReg->mc_curr_current);
        dataPkt.access = READ_ONLY;
        break;
    case MC_REG_P_CURRENT:
        dataPkt.data = &(mcReg->mc_piCurKp);
        dataPkt.dataLen = sizeof(mcReg->mc_piCurKp);
        dataPkt.access = PROTECTED_WRITE;
        break;
    case MC_REG_I_CURRENT:
        dataPkt.data = &(mcReg->mc_piCurKi);
        dataPkt.dataLen = sizeof(mcReg->mc_piCurKi);
        dataPkt.access = PROTECTED_WRITE;
        break;
    case MC_REG_P_SPEED:
        dataPkt.data = &(mcReg->mc_piSpdKp);
        dataPkt.dataLen = sizeof(mcReg->mc_piSpdKp);
        dataPkt.access = PROTECTED_WRITE;
        break;
    case MC_REG_I_SPEED:
        dataPkt.data = &(mcReg->mc_piSpdKi);
        dataPkt.dataLen = sizeof(mcReg->mc_piSpdKi);
        dataPkt.access = PROTECTED_WRITE;
        break;
    // case MC_REG_ACC_RATE:
    //     dataPkt.data = &(mcReg->mc_acc_val);
    //     dataPkt.dataLen = sizeof(mcReg->mc_acc_val);
    //     break;
    // case MC_REG_DEC_RATE:
    //     dataPkt.data = &(mcReg->mc_dec_val);
    //     dataPkt.dataLen = sizeof(mcReg->mc_dec_val);
    //     break;
    case MC_REG_MAX_CURRENT:
        dataPkt.data = &(mcReg->mc_maxCurrent);
        dataPkt.dataLen = sizeof(mcReg->mc_ctrlReg);
        dataPkt.access = PROTECTED_WRITE;
    case MC_REG_IGNORE_FAULT_MASK:
        dataPkt.data = &(mcReg->mc_ignoreFaults);
        dataPkt.dataLen = sizeof(mcReg->mc_ignoreFaults);
        dataPkt.access = PROTECTED_WRITE;
    case MC_REG_MC_CTRL:
        dataPkt.data = &(mcReg->mc_ctrlReg);
        dataPkt.dataLen = sizeof(McCtrlVal_t);
        dataPkt.access = READ_WRITE;
        break;
    case MC_REG_MC_STATUS:
        dataPkt.data = &(mcReg->mc_stateReg);
        dataPkt.dataLen = sizeof(McStateVal_t);
        dataPkt.access = READ_ONLY;
        break;
    case MC_REG_MC_FAULT:
        dataPkt.data = &(mcReg->mc_faultReg);
        dataPkt.dataLen = sizeof(McFaultMask_t);
        dataPkt.access = READ_ONLY;
        break;
    default:
        // error so *data = NULL and datalen = 0
        break;
    }
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
    // case MC_REG_ACC_RATE:
    // case MC_REG_DEC_RATE:
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

// --- ICD_MotorControl.c

uint32_t regSizeMap(MC_ICD_RegAddr reg)
{
    switch (reg)
    {
        case MC_REG_I2C_ADDRESS:
        case MC_REG_TARGET_SPEED:
        case MC_REG_MC_CTRL:
        case MC_REG_MC_FAULT:
        case MC_REG_MC_STATUS:
        case MC_REG_IGNORE_FAULT_MASK:
            return 1;
        case MC_REG_P_CURRENT:
        case MC_REG_I_CURRENT:
        case MC_REG_P_SPEED:
        case MC_REG_I_SPEED:
        case MC_REG_MOTOR_CURRENT:
        // case MC_REG_ACC_RATE:
        // case MC_REG_DEC_RATE:
        case MC_REG_MAX_CURRENT:
            return 2;
        case MC_REG_TARGET_POSITION:
        case MC_REG_CURRENT_POSITION:
            return 4;
        default:
            return 0;
    }
}

void getReg(MC_ICD_RegStruct *mcReg, MC_ICD_RegAddr regID, void **data)
{
    switch(regID)
    {
    case MC_REG_TARGET_POSITION:
        *data = &(mcReg->mc_target_pos);
        break;
    case MC_REG_TARGET_SPEED:
        *data = &(mcReg->mc_target_speed);
        break;
    case MC_REG_CURRENT_POSITION:
        *data = &(mcReg->mc_curr_pos);
        break;
    case MC_REG_CURRENT_SPEED:
        *data = &(mcReg->mc_curr_speed);
        break;
    case MC_REG_MOTOR_CURRENT:
        *data = &(mcReg->mc_curr_current);
        break;
    case MC_REG_P_CURRENT:
        *data = &(mcReg->mc_piCurKp);
        break;
    case MC_REG_I_CURRENT:
        *data = &(mcReg->mc_piCurKi);
        break;
    case MC_REG_P_SPEED:
        *data = &(mcReg->mc_piSpdKp);
        break;
    case MC_REG_I_SPEED:
        *data = &(mcReg->mc_piSpdKi);
        break;
    // case MC_REG_ACC_RATE:
    //     *data = &(mcReg->mc_acc_val);
    //     break;
    // case MC_REG_DEC_RATE:
    //     *data = &(mcReg->mc_dec_val);
    //     break;
    case MC_REG_MC_CTRL:
        *data = &(mcReg->mc_ctrlReg);
        break;
    case MC_REG_MC_STATUS:
        *data = &(mcReg->mc_stateReg);
        break;
    case MC_REG_MC_FAULT:
        *data = &(mcReg->mc_faultReg);
        break;
    default:
        *data = NULL;
        break;
    }
}

void setReg(MC_ICD_RegStruct *mcReg, MC_ICD_RegAddr regID, void *data)
{
    switch(regID)
    {
    case MC_REG_TARGET_POSITION:
        mcReg->mc_target_pos = *(int32_t *)data;
        break;
    case MC_REG_TARGET_SPEED:
        mcReg->mc_target_speed = *(uint8_t *)data;
        break;
    case MC_REG_CURRENT_POSITION:
        mcReg->mc_curr_pos = *(int32_t *)data;
        break;
    case MC_REG_CURRENT_SPEED:
        mcReg->mc_curr_speed = *(uint8_t *)data;
        break;
    case MC_REG_MOTOR_CURRENT:
        mcReg->mc_curr_current = *(int16_t *)data;
        break;
    case MC_REG_P_CURRENT:
        mcReg->mc_piCurKp = *(uint16_t *)data;
        break;
    case MC_REG_I_CURRENT:
        mcReg->mc_piCurKi = *(uint16_t *)data;
        break;
    case MC_REG_P_SPEED:
        mcReg->mc_piSpdKp = *(uint16_t *)data;
        break;
    case MC_REG_I_SPEED:
        mcReg->mc_piSpdKi = *(uint16_t *)data;
        break;
    // case MC_REG_ACC_RATE:
    //     mcReg->mc_acc_val = *(uint16_t *)data;
    //     break;
    // case MC_REG_DEC_RATE:
    //     mcReg->mc_dec_val = *(uint16_t *)data;
    //     break;
    case MC_REG_MC_CTRL:
        mcReg->mc_ctrlReg = *(uint8_t *)data;
        break;
    case MC_REG_MC_STATUS:
        mcReg->mc_stateReg = *(uint8_t *)data;
        break;
    case MC_REG_MC_FAULT:
        mcReg->mc_faultReg = *(uint8_t *)data;
        break;
    default:
        break;
    }
}
