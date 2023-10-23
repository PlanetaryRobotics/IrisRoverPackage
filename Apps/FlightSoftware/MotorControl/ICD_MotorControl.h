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

#include <stdint.h>
#include <stdio.h>

#define I2C_RX_BUFFER_MAX_SIZE      8
#define I2C_TX_BUFFER_MAX_SIZE      8
#define I2C_MAX_DATA_SIZE           4
#define I2C_MAX_NUM_REG             16

#define MC_SLAVE_I2C_ADDR_BASE      0x48


#define DEFAULT_TARGET_POS  20000

// _IQ15(A) = ((_iq15)((A) * ((_iq15)1 << 15)))
// _iq15 = int32_t but can fit in uint16_t

// control related constants (gains, thresholds, setpoints, default values)
#define DEFAULT_TARGET_SPEED_perc   0.7
#define DEFAULT_TARGET_SPEED        70
#define MAX_TARGET_SPEED            100U

#define DEFAULT_KP_SPD 1.5
#define DEFAULT_KI_SPD 0.0009
#define DEFAULT_KP_CUR 0.95
#define DEFAULT_KI_CUR 0.002

#define DEFAULT_TARGET_SPEED_IQ 0x5999  // 0.7
#define DEFAULT_MAX_SPEED_IQ    0x8000  // 1.0

#define DEFAULT_SPEED_KP_IQ     0xC000  // 1.5
#define DEFAULT_SPEED_KI_IQ     0x001D  // 0.0009
#define DEFAULT_CURRENT_KP_IQ   0x7999  // 0.95
#define DEFAULT_CURRENT_KI_IQ   0x0041  // 0.002
#define DEFAULT_MAX_CURRENT

typedef uint8_t McI2cAddr_t;
typedef uint8_t McCtrlVal_t;
typedef uint8_t McStateVal_t;
typedef uint8_t McFaultMask_t;

// --- MC i2c Register
typedef enum MC_ICD_RegAddr
{
    MC_REG_I2C_ADDRESS          = 0,    // RO - 1 Byte
    MC_REG_TARGET_POSITION      = 1,    // RW - 4 Bytes
    MC_REG_TARGET_SPEED         = 2,    // RW - 1 Byte  (protected > MAX)
    MC_REG_CURRENT_POSITION     = 3,    // RO - 2 Bytes
    MC_REG_CURRENT_SPEED        = 4,    // RO - 1 Bytes
    MC_REG_MOTOR_CURRENT        = 5,    // RO - 4 Bytes
    MC_REG_P_CURRENT            = 6,    // RW - 2 Bytes (protected write)
    MC_REG_I_CURRENT            = 7,    // RW - 2 Bytes (protected write)
    MC_REG_P_SPEED              = 8,    // RW - 2 Bytes (protected write)
    MC_REG_I_SPEED              = 9,    // RW - 2 Bytes (protected write)
    // MC_REG_ACC_RATE            = 10,   // RW - 2 Bytes
    // MC_REG_DEC_RATE            = 11,   // RW - 2 Bytes
    MC_REG_MAX_CURRENT          = 10,   // RW - 2 Bytes (protected write)
    MC_REG_DISABLE_FAULT_MASK    = 11,   // RW - 1 Byte  (protected write)
    MC_REG_MC_CTRL              = 12,   // RW - 1 Byte
    MC_REG_MC_STATUS            = 13,   // RO - 1 Byte
    MC_REG_MC_FAULT             = 14,   // RO - 1 Byte
    MC_REG_SIZE                 = 15
} MC_ICD_RegAddr;

// --- MC i2c COMMAND Register Values
typedef enum MC_ICD_Ctrl
{
    MC_NO_CMD                   = 0,
    MC_CMD_DRIVE_OPEN_LOOP      = 1,
        // msp430 set Drive Open Loop
        // MC_CMD_UPDATE_CONFIG       = 1,    // if STATE_ENABLED : goto STATE_IDLE
    MC_CMD_CLEAR_FAULTS         = 2,
        // msp430 attempt to clear faults
        // (PREV : if STATE_FAULT : Clear Faults , goto STATE_IDLE
    MC_CMD_DISABLE_DRIVER       = 4,
        // *****Careful with both DISABLE_DRIVER & ENABLE_DRIVER
        // msp430 DISABLE_STATE_MACHINE : cmdState = DISABLE
        //      => disablesGateDriver, targetPos = 0, currentPos = 0 state = IDLE
        // MC_CMD_ENABLE_DRIVER       = 4,    // if STATE_IDLE & params set : goto STATE_ENABLED
    MC_CMD_ENABLE_DRIVER        = 8,    
        // *****Careful with both DISABLE_DRIVER & ENABLE_DRIVER
        // msp430 ENABLE_STATE_MACHINE : cmdState = RUN
        //  IF state==IDLE => enableGateDriver, targetDir = 1 | -1, currPos = 0,
        //                      targetReached = false, state = RUNNING
        // MC_CMD_DISABLE_DRIVER      = 8,    // Disable Drivers , goto STATE_IDLE
    MC_CMD_RESET_CONTROLLER     = 16,
        // (Not Implemented: Reset Controller)
    MC_CMD_EXECUTE_DRIVE        = 32,
        // msp430 start/continue driving
        // (PREV : if STATE_ENABLED : goto STATE_RUNNING)
    MC_CMD_OVERRIDE_PROTECTED   = 64,   // Updates FaultRegFlags via FaultReg
        // (Not Implemented: Override Protected Regs)
    // MC_CMD_E_STOP              = 128   // Disable Drivers, etc
} MC_ICD_Ctrl;

// --- MC i2c STATE Register Values
typedef enum MC_ICD_State
{
    MC_STATE_IDLE               = 0,    // Driver Disabled
                                        //      Periodically Checks w/StateMachine:
                                        //          MC_CMD_UPDATE_CONFIG, MC_CMD_ENABLE_DRIVER,
                                        //          MC_CMD_RESET_CONTROLLER, MC_CMD_OVERRIDE_PROTECTED
                                        //      Standard param vals may be updated
    MC_STATUS_DRIVE_OPEN_LOOP   = 1,    // msp430 set to drive open loop
    //MC_STATE_ENABLED            = 1,    // Driver Enabled
                                        //      Periodically Checks w/StateMachine:
                                        //          MC_CMD_UPDATE_CONFIG, MC_CMD_DISABLE_DRIVER,
                                        //          MC_CMD_RESET_CONTROLLER
                                        //      Target vals may be updated
    MC_STATUS_CLEAR_FAULTS      = 2,    // msp430 Attempted to clear fault in motor driver
    // MC_STATE_ARMED              = 2,    // Vigilant for MC_CMD_EXECUTE_DRIVE
                                        //      If timeout: check MC_CMD_DISABLE_DRIVER,
                                        //          MC_CMD_RESET_CONTROLLER, MC_CMD_E_STOP
                                        //          Else: MC_FAULT_UNEXPECTED_STATE
    MC_STATE_DISABLE            = 4,    // msp430 Driver Disabled
    // MC_STATE_RUNNING            = 4,    // In Drive Loop
                                        //      Driver Enabled + Driving
                                        //      Waiting for target_reached, timeout (MC_FAULT_STALL),
                                        //      or MC_CMD_E_STOP (goto STATE_DISABLE)
    MC_STATE_TARGET_REACHED     = 8,    // msp430 Position Converged
                                        //      goto STATE_DISABLE
    MC_STATE_ARMED              = 16,   // msp430 has set all received param requests
    // MC_STATE_DISABLE            = 16,   // Disable Drivers, update Targets, etc
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
    MC_NO_FAULT                 = 0,
    MC_FAULT_DRIVER             = 1,    // msp430 fault in DRV8304 motor drivers
    // MC_FAULT_I2C_ERROR         = 1,
    MC_FAULT_POSITION           = 2,    // msp430 position readings not changing / cannot converge
                                        // => maybe dead hall sensors?
    // MC_FAULT_BAD_CONFIG_VAL    = 2,
    MC_FAULT_DRIVE_DIRECTION    = 4,    // msp430 motor driving wrong direction
    // MC_FAULT_UNEXPECTED_STATE  = 4,
    MC_FAULT_DRIVE_TIMEOUT      = 8,    // msp430 does not converge within DRIVING_TIMEOUT_THRESHOLD
    // MC_FAULT_OVERCURRENT       = 8,
    // MC_FAULT_STALL             = 16,    // Target Not Reached by Timeout
    MC_FAULT_OVERCURRENT        = 16,    // msp430 overcurrent
    // MC_FAULT_POS_SENSOR         = 32,    // see POSITION_NOT_CHANGING
    MC_FAULT_I2C_ERROR          = 32,
    MC_FAULT_MC_WATCHDOG        = 64,
    MC_FAULT_OTHER              = 128
} MC_IDC_Fault;

typedef enum MC_ICD_AccessRW
{
    READ_ONLY       = 0,
    READ_WRITE      = 1,
    PROTECTED_WRITE = 2
} MC_ICD_AccessRW;

// --- MC msp430 Instance of Shared Data
typedef struct MC_ICD_RegStruct
{
    McI2cAddr_t mc_i2c_addr;

    int32_t mc_target_pos; // ticks
    uint8_t mc_target_speed; // 0-100%

    int32_t mc_curr_pos; // ticks
    uint8_t mc_curr_speed; // 0-100%
    int32_t mc_curr_current; // mA

    McCtrlVal_t mc_ctrlReg;     // msp430 RO herc WO 
    McStateVal_t mc_stateReg;   // msp430 RW herc RO 
    McFaultMask_t mc_faultReg;  // msp430 RW herc RO

    // PROTECTED:
    uint16_t mc_piCurKp; // Linear Format
    uint16_t mc_piCurKi;
    uint16_t mc_piSpdKp;
    uint16_t mc_piSpdKi;
    // uint16_t mc_acc_val; // ticks*s-2
    // uint16_t mc_dec_val;
    uint8_t mc_maxCurrent;
    McFaultMask_t mc_ignoreFaults;

} MC_ICD_RegStruct;

typedef struct McI2cDataPkt
{
    McI2cAddr_t addr;
    uint8_t regID;
    uint16_t dataLen;
    void *data;
    MC_ICD_AccessRW access;
}McI2cDataPkt;



inline uint8_t regSizeMap(MC_ICD_RegAddr reg)
{
    switch (reg)
    {
        case MC_REG_I2C_ADDRESS:
        case MC_REG_TARGET_SPEED:
        case MC_REG_MC_CTRL:
        case MC_REG_MC_FAULT:
        case MC_REG_MC_STATUS:
        case MC_REG_DISABLE_FAULT_MASK:
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
};



void initMcRegStruct(MC_ICD_RegStruct *mcReg, McI2cAddr_t addr);
McI2cDataPkt makeMcI2cDataPkt(MC_ICD_RegStruct *mcReg, MC_ICD_RegAddr regID);
uint8_t checkRegWritePermission(MC_ICD_RegAddr reg);

// uint32_t regSizeMap(MC_ICD_RegAddr reg);
void getReg(MC_ICD_RegStruct *mcReg, MC_ICD_RegAddr regID, void **data);
void setReg(MC_ICD_RegStruct *mcReg, MC_ICD_RegAddr regID, void *data);

#ifdef __cplusplus
}
#endif

#endif /* MCICD_H_ */
