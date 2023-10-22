/*
 * allVars.c
 *
 *  Created on: Oct 21, 2023
 *      Author: iris
 */

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

    g_targetDirection = 1;
    g_targetPosition = 0;
    g_targetSpeed = DEFAULT_TARGET_SPEED_IQ;

    g_readSensors = false;
    g_drivingTimeoutCtr = 0;

    g_readSensors = false;
    g_errorCounter = 0;
}

void initializeSoftwareControlVariables(void)
{
    // software control related variables (rate groups, internal state machine)
    g_controlPrescaler = PI_SPD_CONTROL_PRESCALER;
    g_closedLoop = false;
    g_state = RUNNING;
    g_cmdState = NO_CMD;
    g_controlRegister = 0; // see main.h for bits
}

void initAllVars()
{
    initializeSensorVariables();
}
