/*
 * MotorControl.c
 *
 *  Created on: Oct 21, 2023
 *      Author: iris
 */

#include "MotorControl.h"

void updateDriverStateMachine(void);

void initMotorControl(McI2cAddr_t i2cAddr)
{
    g_mcIgnoredFaults = 0xFF;
    initMcRegStruct(&mcRegStruct, i2cAddr);
}

void checkAllFaults(void)
{
    checkDriverFault();

    g_faultRegister &= g_mcIgnoredFaults;
}

inline void setParamUpdateFlag(MC_ICD_RegAddr reg)
{
    g_mcParamUpdates |= (1 << reg);
}
inline bool checkUpdateFlag(MC_ICD_RegAddr reg)
{
    return g_mcParamUpdates & (1 << reg);
}
inline void unsetUpdateFlag(MC_ICD_RegAddr reg) {
    g_mcParamUpdates &= ~(1 << reg);
}

void checkUpdateReq()
{
    bool writeProtected = g_statusRegister & MC_STATE_WRITE_PROTECTED;

    if (checkUpdateFlag(MC_REG_TARGET_POSITION)) {
        g_targetPosition = mcRegStruct.mc_target_pos;
        g_currentPosition = 0; // reset because target pos is relative
        g_statusRegister &= ~MC_STATE_TARGET_REACHED; // likely no longer converged (if still converged, control loop will correct for that)
        g_drivingTimeoutCtr = 0; //reset timeout counter
        unsetUpdateFlag(MC_REG_TARGET_POSITION);
    }

    if (checkUpdateFlag(MC_REG_TARGET_SPEED)) {
        if(writeProtected || mcRegStruct.mc_target_speed < MAX_TARGET_SPEED)
        {
            g_targetSpeed = mcRegStruct.mc_target_speed;
            unsetUpdateFlag(MC_REG_TARGET_SPEED);
        }
    }

    if (checkUpdateFlag(MC_REG_P_CURRENT)) {
        g_piCur.Kp = mcRegStruct.mc_piCurKp;
        unsetUpdateFlag(MC_REG_P_CURRENT);
    }

    if (checkUpdateFlag(MC_REG_I_CURRENT)) {
        g_piCur.Ki = mcRegStruct.mc_piCurKi;
        unsetUpdateFlag(MC_REG_I_CURRENT);
    }

    if (checkUpdateFlag(MC_REG_P_SPEED)) {
        g_piSpd.Kp = mcRegStruct.mc_piSpdKp;
        unsetUpdateFlag(MC_REG_P_SPEED);
    }

    if (checkUpdateFlag(MC_REG_I_SPEED)) {
        g_piSpd.Ki = mcRegStruct.mc_piSpdKi;
        unsetUpdateFlag(MC_REG_I_SPEED);
    }

    if (checkUpdateFlag(MC_REG_MAX_CURRENT) && writeProtected) {
        g_maxCurrent = mcRegStruct.mc_maxCurrent;
        unsetUpdateFlag(MC_REG_MAX_CURRENT);
    }

    if (checkUpdateFlag(MC_REG_DISABLE_FAULT_MASK) && writeProtected)
    {
        g_mcIgnoredFaults = mcRegStruct.mc_ignoreFaults;
        unsetUpdateFlag(MC_REG_DISABLE_FAULT_MASK);
        g_faultRegister &= g_mcIgnoredFaults;
        if (g_mcIgnoredFaults == 0)
            g_state != FAULT;
    }
}

void checkCtrlReg(void)
{
    if (g_controlRegister & MC_CMD_DISABLE_DRIVER || g_state == DISABLE)
    {
        disableGateDriver();
        g_targetPosition = 0;
        g_currentPosition = 0;
        g_statusRegister |= MC_STATE_DISABLE; // status reg bit 3: 1 if in disable state, 0 if not
        g_state = IDLE;
        g_controlRegister &= ~ MC_CMD_DISABLE_DRIVER;
    }
    else if (g_controlRegister & MC_CMD_ENABLE_DRIVER)
    {
        enableGateDriver();
        g_targetDirection = (g_targetPosition - g_currentPosition >= 0) ? 1 : -1;
        g_currentPosition = 0;
        g_targetReached = false;
        g_statusRegister &= ~MC_STATE_DISABLE;
        g_controlRegister &= ~MC_CMD_ENABLE_DRIVER;
    }


    if (g_controlRegister & MC_CMD_RESET_CONTROLLER)
    {
        // reset controller
        g_controlRegister &= ~MC_CMD_RESET_CONTROLLER;
    }

    if (g_controlRegister & MC_CMD_CLEAR_FAULTS)
    {
        clearDriverFault();
        g_statusRegister |= MC_STATUS_CLEAR_FAULTS; // indicates an attempt to clear fault was made
        g_controlRegister &= ~MC_CMD_CLEAR_FAULTS;
    }

    if (g_controlRegister & MC_CMD_OVERRIDE_PROTECTED)
    {
        g_statusRegister |= MC_CMD_OVERRIDE_PROTECTED;
        g_controlRegister &= ~MC_CMD_OVERRIDE_PROTECTED;
    }

    if (g_state == RUNNING) {
        return;
    }

    if(g_controlRegister & DRIVE_OPEN_LOOP){
        g_statusRegister |= DRIVE_OPEN_LOOP;
    }

    checkUpdateReq();

    if (g_state == FAULT) {
        return;
    }

//    if ((g_faultRegister & g_mcIgnoredFaults) || g_statusRegister & MC_STATE_DISABLE ||
//            g_controlRegister || g_mcParamUpdates || g_targetPosition == 0)
//    {
//        g_statusRegister &= ~MC_STATE_ARMED;
//    } else if (g_controlRegister & MC_CMD_EXECUTE_DRIVE && g_statusRegister & MC_STATE_ARMED)
//    {
    if (g_controlRegister & MC_CMD_EXECUTE_DRIVE && g_targetPosition && g_targetSpeed) {
        g_state = RUNNING;
    }
//    } else {
//        g_statusRegister |= MC_STATE_ARMED;
//    }
}


