#include "stateMachine/RoverStateMission.hpp"

#include "comms/debug_comms.h"
#include "comms/ground_msgs.h"
#include "comms/i2c_sensors.h"
#include "drivers/adc.h"
#include "drivers/bsp.h"

#include "watchdog.h"

#include <cassert>
#include <msp430.h>

namespace iris
{
    RoverStateMission::RoverStateMission()
        : RoverStateBase(RoverState::MISSION),
          m_currentSubState(SubState::MISSION_NORMAL),
          m_currentDeployState(DeployState::NOT_DEPLOYED)
    {
    }

    bool RoverStateMission::canEnterLowPowerMode(RoverContext &theContext)
    {
        // The only thing that is done in this state that requires us to stay out of LPM is processing an I2C
        // transaction. Therefore in this state allowing entering LPM is conditional on whether or not we're actively
        // performing an I2C transaction.
        return !theContext.m_i2cActive;
    }

    RoverState RoverStateMission::handleTimerTick(RoverContext &theContext)
    {
        // Trigger a new ADC sample if the previous one is done
        if (isAdcSampleDone())
        {
            adcCheckVoltageLevels(&(theContext.m_adcValues));
        }

        // Check for UART errors to report:

        // Hercules UART
        size_t count = 0;
        BOOL changed = FALSE;
        UART__Status uStatus = UART__checkRxRbErrors(theContext.m_uart0State, &count, &changed);
        DEBUG_LOG_CHECK_STATUS(UART__STATUS__SUCCESS, uStatus, "Failed to get Hercules UART Rx Rb Error count");

        if (changed)
        {
            DebugComms__tryPrintfToLanderNonblocking("New Hercules UART Rx Rb failures, total count = %d\n", (int)count);
        }

        // Lander UART
        count = 0;
        changed = FALSE;
        uStatus = UART__checkRxRbErrors(theContext.m_uart1State, &count, &changed);
        DEBUG_LOG_CHECK_STATUS(UART__STATUS__SUCCESS, uStatus, "Failed to get Lander UART Rx Rb Error count");

        if (changed)
        {
            DebugComms__tryPrintfToLanderNonblocking("New Lander UART Rx Rb failures, total count = %d\n", (int)count);
        }

        // Enable Hercules UART if Hercules is ON:
        if ((theContext.m_details.m_outputPinBits & OPSBI__HERCULES_ON) && !(HerculesComms__isInitialized(theContext.m_hcState)))
        {
            // We should hopefully never be here during Mission...
            DebugComms__tryPrintfToLanderNonblocking("Trying to establish UART between WD and Hercules\n");
            enableHerculesComms(theContext);
        }

        /* send heartbeat with collected data */
        /**
         * @todo Check if we've deployed or if we shouldn't be able to communicate with lander for any other
         *       reason, and don't send this if that is the case.
         */
        static FullEarthHeartbeat hb = {0};
        GroundMsgs__Status gcStatus = GroundMsgs__generateFullEarthHeartbeat(&(theContext.m_i2cReadings),
                                                                             &(theContext.m_adcValues),
                                                                             &(theContext.m_details.m_hParams),
                                                                             static_cast<uint8_t>(getState()),
                                                                             &hb);

        DEBUG_ASSERT_EQUAL(GND_MSGS__STATUS__SUCCESS, gcStatus);

        LanderComms__Status lcStatus = txDownlinkData(theContext,
                                                      (uint8_t *)&hb,
                                                      sizeof(hb));

        DEBUG_ASSERT_EQUAL(LANDER_COMMS__STATUS__SUCCESS, lcStatus);
        if (LANDER_COMMS__STATUS__SUCCESS != lcStatus)
        {
            //!< @todo Handling?
        }

        if (theContext.m_details.m_hParams.m_heatingControlEnabled)
        {
            // Update the Heater State (PWM remains unchanged here):
            heaterControl(theContext);
        }

        // theContext.m_queuedI2cActions |= 1 << ((uint16_t) I2C_SENSORS__ACTIONS__GAUGE_READING);

        if (!theContext.m_i2cActive)
        {
            initiateNextI2cAction(theContext);
        }

        BOOL writeIOExpander = FALSE;
        watchdog_monitor(theContext.m_hcState,
                         &(theContext.m_watchdogFlags),
                         &(theContext.m_watchdogOpts),
                         &writeIOExpander,
                         &(theContext.m_details));

        if (writeIOExpander)
        {
            theContext.m_queuedI2cActions |= 1 << ((uint16_t)I2C_SENSORS__ACTIONS__WRITE_IO_EXPANDER);
            theContext.m_writeCustomIoExpanderValues = false;
            theContext.m_watchdogFlags |= WDFLAG_WAITING_FOR_IO_EXPANDER_WRITE;

            if (!theContext.m_i2cActive)
            {
                initiateNextI2cAction(theContext);
            }
        }

        // Queue up a read of the IO Expander, and initiate it if no other I2C action is active
        theContext.m_queuedI2cActions |= 1 << ((uint16_t)I2C_SENSORS__ACTIONS__READ_IO_EXPANDER);

        if (!theContext.m_i2cActive)
        {
            initiateNextI2cAction(theContext);
        }

        return getState();
    }

    RoverState RoverStateMission::handlePowerIssue(RoverContext & /*theContext*/)
    {
        //!< @todo Implement RoverStateMission::handlePowerIssue
        return getState();
    }

    RoverState RoverStateMission::spinOnce(RoverContext &theContext)
    {
        if (theContext.m_i2cActive)
        {
            I2C_Sensors__Action action = {};
            uint8_t readValue = 0;
            I2C_Sensors__Status i2cStatus = I2C_Sensors__getActionStatus(&action,
                                                                         &(theContext.m_i2cReadings),
                                                                         &readValue);

            if (I2C_SENSORS__STATUS__INCOMPLETE != i2cStatus)
            {
                DEBUG_LOG_CHECK_STATUS(I2C_SENSORS__STATUS__SUCCESS_DONE, i2cStatus, "I2C action failed");

                if (I2C_SENSORS__ACTIONS__WRITE_IO_EXPANDER == action)
                {
                    theContext.m_watchdogFlags &= ~WDFLAG_WAITING_FOR_IO_EXPANDER_WRITE;
                }

                if (I2C_SENSORS__ACTIONS__READ_IO_EXPANDER == action)
                {
                    bool chargeStat2 = (readValue & I2C_SENSORS__IOE_P1_BIT__CHARGE_STAT2 != 0);
                    bool latchStat = (readValue & I2C_SENSORS__IOE_P1_BIT__LATCH_STAT != 0);

                    if (chargeStat2)
                    {
                        SET_IPASBI_IN_UINT(theContext.m_details.m_inputPinAndStateBits, IPASBI__CHARGE_STAT2);
                    }
                    else
                    {
                        CLEAR_IPASBI_IN_UINT(theContext.m_details.m_inputPinAndStateBits, IPASBI__CHARGE_STAT2);
                    }

                    if (latchStat)
                    {
                        SET_IPASBI_IN_UINT(theContext.m_details.m_inputPinAndStateBits, IPASBI__LATCH_STAT);
                    }
                    else
                    {
                        CLEAR_IPASBI_IN_UINT(theContext.m_details.m_inputPinAndStateBits, IPASBI__LATCH_STAT);
                    }
                }

                I2C_Sensors__clearLastAction();
                theContext.m_i2cActive = false;
                initiateNextI2cAction(theContext);
            }
        }

        if (theContext.m_sendDetailedReport)
        {

            theContext.m_sendDetailedReport = false;
            sendDetailedReportToLander(theContext, true);
        }

        return getState();
    }

    RoverState RoverStateMission::transitionTo(RoverContext &theContext)
    {
        // Nothing to do on this transition, which should always be from ENTERING_MISSION.
        m_currentSubState = SubState::MISSION_NORMAL;
        *(theContext.m_persistentInMission) = true;

        if (*(theContext.m_persistentDeployed))
        {
            m_currentDeployState = DeployState::DEPLOYED;
            disableHeater();
        }

#ifdef HERC_PROGRAMMING_MODE
        // [CWC-03/13/2023] Warn that this is the WRONG version of the SW for Flight and should only be used for hercules programming.
        // Essentially this is a special version of the SW that disable hercules monitioring by default in mission (instead of enables) so
        // hercules can be programmed. In flight, though, we want Hercules monitoring to be on by default.
        DPRINTF("Defaulting MONITOR_HERCULES to OFF in MISSION for programming.");
        theContext.m_watchdogOpts &= ~WDOPT_MONITOR_HERCULES;
#else
        DPRINTF("Defaulting MONITOR_HERCULES to ON in MISSION.");
        theContext.m_watchdogOpts |= WDOPT_MONITOR_HERCULES; // default to monitoring Hercules for aliveness
#endif

        return getState();
    }

    void RoverStateMission::heaterControl(RoverContext &theContext)
    {
        // Only use heater when connected to the lander

        if (m_currentDeployState != DeployState::NOT_DEPLOYED)
        {
            disableHeater();
        }
        else
        {
            RoverStateBase::heaterControl(theContext);
        }
    }

    RoverState RoverStateMission::performResetCommand(RoverContext &theContext,
                                                      WdCmdMsgs__ResetSpecificId resetValue,
                                                      WdCmdMsgs__Response *response)
    {
        bool writeIOExpander = false;
        RoverStateBase::doConditionalResetSpecific(theContext,
                                                   resetValue,
                                                   response,
                                                   true, // whether or not to allow power on
                                                   // whether or not to allow disabling RS422
                                                   m_currentDeployState == DeployState::DEPLOYED,
                                                   // whether or not to allow deploy
                                                   m_currentDeployState != DeployState::DEPLOYED,
                                                   // whether or not to allow undeploy
                                                   m_currentDeployState != DeployState::NOT_DEPLOYED,
                                                   writeIOExpander);

        if (writeIOExpander)
        {
            theContext.m_queuedI2cActions |= 1 << ((uint16_t)I2C_SENSORS__ACTIONS__WRITE_IO_EXPANDER);
            theContext.m_writeCustomIoExpanderValues = false;
            theContext.m_watchdogFlags |= WDFLAG_WAITING_FOR_IO_EXPANDER_WRITE;

            if (!theContext.m_i2cActive)
            {
                initiateNextI2cAction(theContext);
            }
        }

        return getState();
    }

    RoverState RoverStateMission::performWatchdogCommand(RoverContext &theContext,
                                                         const WdCmdMsgs__Message &msg,
                                                         WdCmdMsgs__Response &response,
                                                         WdCmdMsgs__Response &deployNotificationResponse,
                                                         bool &sendDeployNotificationResponse)
    {
        // If we're in the SERVICE_ALIVE_HOLDING substate and we receive any command other than EnterService, then we
        // switch to the MISSION_NORMAL substate.
        if (SubState::SERVICE_HOLDING == m_currentSubState && msg.commandId != WD_CMD_MSGS__CMD_ID__ENTER_SERVICE_MODE)
        {
            m_currentSubState = SubState::MISSION_NORMAL;
        }

        // Other than resetting the substate, we want to rely on the default implementation of this function
        return RoverStateBase::performWatchdogCommand(theContext,
                                                      msg,
                                                      response,
                                                      deployNotificationResponse,
                                                      sendDeployNotificationResponse);
    }

    RoverState RoverStateMission::doGndCmdDeploy(RoverContext &theContext,
                                                 const WdCmdMsgs__Message &msg,
                                                 WdCmdMsgs__Response &response,
                                                 WdCmdMsgs__Response &deployNotificationResponse,
                                                 bool &sendDeployNotificationResponse)
    {
        switch (m_currentDeployState)
        {
        case DeployState::NOT_DEPLOYED:
            setDeploy();
            m_currentDeployState = DeployState::DEPLOYING;
            response.statusCode = WD_CMD_MSGS__RESPONSE_STATUS__SUCCESS;

            deployNotificationResponse.magicNumber = WD_CMD_MSGS__RESPONSE_MAGIC_NUMBER;
            deployNotificationResponse.commandId = msg.commandId;
            deployNotificationResponse.statusCode = WD_CMD_MSGS__RESPONSE_STATUS__DEPLOY;
            sendDeployNotificationResponse = true;
            *(theContext.m_persistentDeployed) = true;

            // Don't allow DebugComms to write to lander anymore
            DebugComms__registerLanderComms(NULL);
            break;

        case DeployState::DEPLOYING:
            // fall through
        case DeployState::DEPLOYED:
            // fall through
        default:
            response.statusCode = WD_CMD_MSGS__RESPONSE_STATUS__ERROR_BAD_COMMAND_SEQUENCE;
            break;
        }

        return getState();
    }

    RoverState RoverStateMission::doGndCmdEnterServiceMode(RoverContext &theContext,
                                                           const WdCmdMsgs__Message &msg,
                                                           WdCmdMsgs__Response &response,
                                                           WdCmdMsgs__Response &deployNotificationResponse,
                                                           bool &sendDeployNotificationResponse)
    {
        // We only want to actually enter keep alive if we receive it twice in a row
        if (SubState::SERVICE_HOLDING == m_currentSubState)
        {
            response.statusCode = WD_CMD_MSGS__RESPONSE_STATUS__SUCCESS;
            return RoverState::ENTERING_SERVICE;
        }
        else
        {
            // Update the substate so that we know to actually transition to keep alive mode if we receive the command
            // again as the next command.
            m_currentSubState = SubState::SERVICE_HOLDING;

            /**
             * @todo Is returning SUCCESS in this case the correct thing to do? If we do this then there's no way to
             *       differentiate this response from the one we will send when we actually switch mode.
             */
            response.statusCode = WD_CMD_MSGS__RESPONSE_STATUS__SUCCESS;

            // Stay in this state for now
            return getState();
        }
    }

} // End namespace iris
