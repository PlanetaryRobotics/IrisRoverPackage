/*
 * allVars.c
 *
 *  Created on: Oct 21, 2023
 *      Author: iris
 */

#include "allVars.h"

/*
 * @brief       TODO
 */
void initializeSensorVariables(void)
{
    g_currentOffsetPhaseA = 0;
    g_currentOffsetPhaseB = 0;
    g_currentOffsetPhaseC = 0;
    g_commState = 0;
    g_hallSensor.Pattern = 0;
    g_hallSensor.OldPattern = 0;

    g_currentPosition = 0;
    g_oldPosition = g_currentPosition;
//    g_targetDirection = 1;
    g_targetPosition = 0;

//    g_targetSpeed = MAX_TARGET_SPEED;

//    g_readSensors = false;
    g_drivingTimeoutCtr = 0;
//    g_errorCounter = 0;
}

/*
 * @brief       TODO
 */
void initializeSoftwareControlVariables(void)
{
    // software control related variables (rate groups, internal state machine)
    g_controlPrescaler = PI_SPD_CONTROL_PRESCALER;
    g_closedLoop = false;
    g_state = RUNNING;
    g_cmdState = NO_CMD;
    g_controlRegister = 0; // see main.h for bits
}

/*
 * @breif   initialize motor controller related variables (PI controllers for speed and current)
 */
void initializeControllerVariables(void)
{
    g_targetSpeed = MAX_TARGET_SPEED;

    g_openLoopTorque = _IQ(OPEN_LOOP_TORQUE);
    g_impulse.Period = PERIOD_IMPULSE;
    g_targetDirection = 1;

    resetPiController(&g_piSpd);
    resetPiController(&g_piCur);

    g_piSpd.Kp = _IQ(DEFAULT_KP_SPD);
    g_piSpd.Ki = _IQ(DEFAULT_KI_SPD);
    g_piCur.Kp = _IQ(DEFAULT_KP_CUR);
    g_piCur.Ki = _IQ(DEFAULT_KI_CUR);

    g_closeLoopThreshold = _IQ(CLOSE_LOOP_THRESHOLD);
    g_closedLoop = false;
}


/**
 * @brief      Initializes the hall interface.
 */
void initializeHallInterface(void)
{
    g_hallMap[0] = 0xff;
    g_hallMap[7] = 0xff;

    g_hallMap[5] = 0;
    g_hallMap[1] = 1;
    g_hallMap[3] = 2;
    g_hallMap[2] = 3;
    g_hallMap[6] = 4;
    g_hallMap[4] = 5;
}


//void initAllVars()
//{
//    initializeSensorVariables();
//    initializeSoftwareControlVariables();
//    initializeControllerVariables();
//    initializeHallInterface();
//}
