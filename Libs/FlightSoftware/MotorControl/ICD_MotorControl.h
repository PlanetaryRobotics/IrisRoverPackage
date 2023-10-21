/*
 * mc.h
 *
 *  Created on: Oct 19, 2023
 *      Author: Raewyn
 */

#ifndef MCICD_H_
#define MCICD_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifdef FPRIME_BUILD
#include "Fw/Types/BasicTypes.hpp"
#else
#include <stdint.h>
#endif

#define I2C_RX_BUFFER_MAX_SIZE      8
#define I2C_TX_BUFFER_MAX_SIZE      8
#define I2C_MAX_DATA_SIZE           4

#define MC_SLAVE_I2C_ADDR_BASE      0x48


#define DEFAULT_TARGET_POS  20000
#define DEFAULT_TARGET_SPEED  70
#define MAX_TARGET_SPEED      100

#define DEFAULT_SPEED_P       15000   // e-4 (1.5000) (0x3A98)
#define DEFAULT_SPEED_I       9       // e-4 (0.0009) (0x0009)
#define DEFAULT_CURRENT_P   9500    // e-4 (0.9500) (0x005F)
#define DEFAULT_CURRENT_I   20      // e-4 (0.0020) (0x0014)


typedef uint8_t McI2cAddr_t;
typedef uint8_t McCtrlVal_t;
typedef uint8_t McStateVal_t;
typedef uint8_t McFaultMask_t;

// --- MC i2c Register
typedef enum MC_ICD_RegAddr
{
    MC_REG_I2C_ADDRESS         = 0,    // RO - 1 Byte
    MC_REG_TARGET_POSITION     = 1,    // RW - 4 Bytes
    MC_REG_TARGET_SPEED        = 2,    // RW - 1 Byte
    MC_REG_CURRENT_POSITION    = 3,    // RO - 4 Bytes
    MC_REG_CURRENT_SPEED       = 4,    // RO - 1 Bytes
    MC_REG_MOTOR_CURRENT       = 5,    // RO - 4 Bytes
    MC_REG_P_CURRENT           = 6,    // RW - 2 Bytes
    MC_REG_I_CURRENT           = 7,    // RW - 2 Bytes
    MC_REG_P_SPEED             = 8,    // RW - 2 Bytes
    MC_REG_I_SPEED             = 9,    // RW - 2 Bytes
    MC_REG_ACC_RATE            = 10,   // RW - 2 Bytes
    MC_REG_DEC_RATE            = 11,   // RW - 2 Bytes
    MC_REG_MC_CTRL             = 12,   // RW - 1 Byte
    MC_REG_MC_STATUS           = 13,   // RO - 1 Byte
    MC_REG_MC_FAULT            = 14,   // RO - 1 Byte
    MC_REG_MAX                 = 16
} MC_ICD_RegAddr;

//uint8_t mc_i2cCmdLen[MC_REG_MAX] = {
//                                 1, // MC_REG_I2C_ADDRESS
//                                 4, // MC_REG_TARGET_POSITION
//                                 1, // MC_REG_TARGET_SPEED
//                                 4, // MC_REG_CURRENT_POSITION
//                                 1, // MC_REG_CURRENT_SPEED
//                                 2, // MC_REG_MOTOR_CURRENT
//                                 2, // MC_REG_P_CURRENT
//                                 2, // MC_REG_I_CURRENT
//                                 2, // MC_REG_P_SPEED
//                                 2, // MC_REG_I_SPEED
//                                 2, // MC_REG_ACC_RATE
//                                 2, // MC_REG_DEC_RATE
//                                 1, // MC_REG_MC_CTRL
//                                 1, // MC_REG_MC_STATUS
//                                 1, // MC_REG_MC_FAULT
//                                 0  // ...err
//};

// --- MC i2c COMMAND Register Values
typedef enum MC_ICD_Ctrl
{
    MC_NO_CMD                  = 0,
    MC_CMD_UPDATE_CONFIG       = 1,    // if STATE_ENABLED : goto STATE_IDLE
    MC_CMD_CLEAR_FAULTS        = 2,    // if STATE_FAULT : Clear Faults , goto STATE_IDLE
    MC_CMD_ENABLE_DRIVER       = 4,    // if STATE_IDLE & params set : goto STATE_ENABLED
    MC_CMD_DISABLE_DRIVER      = 8,    // Disable Drivers , goto STATE_IDLE
    MC_CMD_RESET_CONTROLLER    = 16,   // Reset Controller
    MC_CMD_EXECUTE_DRIVE       = 32,   // if STATE_ENABLED : goto STATE_RUNNING
    MC_CMD_OVERRIDE_PROTECTED  = 64,   // Updates FaultRegFlags via FaultReg
    MC_CMD_E_STOP              = 128   // Disable Drivers, etc
} MC_ICD_Ctrl;

// --- MC i2c STATE Register Values
typedef enum MC_ICD_State
{
    MC_STATE_IDLE               = 0,    // Driver Disabled
                                        //      Periodically Checks w/StateMachine:
                                        //          MC_CMD_UPDATE_CONFIG, MC_CMD_ENABLE_DRIVER,
                                        //          MC_CMD_RESET_CONTROLLER, MC_CMD_OVERRIDE_PROTECTED
                                        //      Standard param vals may be updated
    MC_STATE_ENABLED            = 1,    // Driver Enabled
                                        //      Periodically Checks w/StateMachine:
                                        //          MC_CMD_UPDATE_CONFIG, MC_CMD_DISABLE_DRIVER,
                                        //          MC_CMD_RESET_CONTROLLER
                                        //      Target vals may be updated
    MC_STATE_ARMED              = 2,    // Vigilant for MC_CMD_EXECUTE_DRIVE
                                        //      If timeout: check MC_CMD_DISABLE_DRIVER,
                                        //          MC_CMD_RESET_CONTROLLER, MC_CMD_E_STOP
                                        //          Else: MC_FAULT_UNEXPECTED_STATE
    MC_STATE_RUNNING            = 4,    // In Drive Loop
                                        //      Driver Enabled + Driving
                                        //      Waiting for target_reached, timeout (MC_FAULT_STALL),
                                        //      or MC_CMD_E_STOP (goto STATE_DISABLE)
    MC_STATE_TARGET_REACHED     = 8,    // Target reached
                                        //      goto STATE_DISABLE
    MC_STATE_DISABLE            = 16,   // Disable Drivers, update Targets, etc
                                        //      if MC_NOFAULT goto STATE_IDLE, else goto MC_STATE_FAULT
    MC_STATE_WRITE_PROTECTED    = 64,   // Updates all params received from Herc
                                        //      Waiting for MC_NO_CMD to goto STATE_IDLE
                                        //      Periodically Checks w/StateMachine:
                                        //      MC_CMD_CLEAR_FAULTS,
    MC_STATE_FAULT              = 128   // Fault Detected
} MC_ICD_State;

// --- MC i2c FAULT Register Values
typedef enum MC_IDC_Fault
{
    MC_NO_FAULT                = 0,
    MC_FAULT_I2C_ERROR         = 1,
    MC_FAULT_BAD_CONFIG_VAL    = 2,
    MC_FAULT_UNEXPECTED_STATE  = 4,
    MC_FAULT_OVERCURRENT       = 8,
    MC_FAULT_STALL             = 16,    // Target Not Reached by Timeout
    MC_FAULT_POS_SENSOR_FAULT  = 32,
    MC_FAULT_MC_WATCHDOG       = 64,
    MC_FAULT_OTHER             = 128
} MC_IDC_Fault;

// --- MC msp430 Instance of Shared Data
typedef struct MC_ICD_RegStruct
{
    McI2cAddr_t mc_i2c_addr;

    int32_t mc_target_pos; // ticks
    uint8_t mc_target_speed; // 0-100%

    int32_t mc_curr_pos; // ticks
    uint8_t mc_curr_speed; // 0-100%
    int16_t mc_curr_current; // mA

    uint16_t mc_current_p_val; // Linear Format
    uint16_t mc_current_i_val;
    uint16_t mc_speed_p_val;
    uint16_t mc_speed_i_val;
    uint16_t mc_acc_val; // ticks*s-2
    uint16_t mc_dec_val;

    McCtrlVal_t mc_ctrlReg;
    McStateVal_t mc_stateReg;
    McFaultMask_t mc_faultReg;
} MC_ICD_RegStruct;

typedef struct McI2cDataPkt
{
    McI2cAddr_t addr;
    uint8_t regID;
    uint16_t dataLen;
    uint8_t *data;
}McI2cDataPkt;

void initMcRegStruct(MC_ICD_RegStruct *mcReg, McI2cAddr_t addr);
McI2cDataPkt makeMcI2cDataPkt(MC_ICD_RegStruct *mcReg, MC_ICD_RegAddr regID);
uint8_t checkRegWritePermission(MC_ICD_RegAddr reg);
uint8_t checkCmdExecPermission(MC_ICD_Ctrl cmd);



uint32_t regSizeMap(MC_ICD_RegAddr reg);
void getReg(MC_ICD_RegStruct *mcReg, MC_ICD_RegAddr regID, void *data);
void setReg(MC_ICD_RegStruct *mcReg, MC_ICD_RegAddr regID, void *data);

#ifdef __cplusplus
}
#endif

#endif /* MCICD_H_ */
