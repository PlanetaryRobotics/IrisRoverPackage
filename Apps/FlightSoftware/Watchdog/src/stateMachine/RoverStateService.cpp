#include "stateMachine/RoverStateService.hpp"

#include "comms/debug_comms.h"
#include "comms/ground_msgs.h"
#include "drivers/adc.h"
#include "drivers/bsp.h"

#include "watchdog.h"

#include <cassert>

namespace iris
{
    RoverStateService::RoverStateService()
            : RoverStateEnteringService(RoverState::SERVICE)
    {
    }

    bool RoverStateService::canEnterLowPowerMode(RoverContext& theContext)
    {
        // Receiving data from lander or hercules and timer ticks will both wake us up out of LPM, so we can enter LPM
        // while in this state as long as there are no active I2C transactions occurring
        return !(theContext.m_i2cActive);
    }

    RoverState RoverStateService::handleTimerTick(RoverContext& theContext)
    {
        // Trigger a new ADC sample if the previous one is done
        if (isAdcSampleDone()) {
            adcCheckVoltageLevels(&(theContext.m_adcValues));
        }

        // Check for UART errors to report
        size_t count = 0;
        BOOL changed = FALSE;
        UART__Status uStatus = UART__checkRxRbErrors(theContext.m_uart1State, &count, &changed);
        DEBUG_LOG_CHECK_STATUS(UART__STATUS__SUCCESS, uStatus, "Failed to get Lander UART Rx Rb Error count");

        if (changed) {
            DebugComms__tryPrintfToLanderNonblocking("New Lander UART Rx Rb failures, total count = %u\n", count);
        }

        /* send heartbeat with collected data */
        static FullEarthHeartbeat hb = { 0 };
        GroundMsgs__Status gcStatus = GroundMsgs__generateFullEarthHeartbeat(&(theContext.m_i2cReadings),
                                                                             &(theContext.m_adcValues),
                                                                             &(theContext.m_details.m_hParams),
                                                                             static_cast<uint8_t>(getState()),
                                                                             &hb);

        assert(GND_MSGS__STATUS__SUCCESS == gcStatus);

        LanderComms__Status lcStatus = txDownlinkData(theContext,
                                                           (uint8_t*) &hb,
                                                           sizeof(hb));

        assert(LANDER_COMMS__STATUS__SUCCESS == lcStatus);
        if (LANDER_COMMS__STATUS__SUCCESS != lcStatus) {
            //!< @todo Handling?
        }

        if (theContext.m_details.m_hParams.m_heatingControlEnabled) {
            // calculate PWM duty cycle (if any) to apply to heater
            heaterControl(theContext);
        }

        BOOL writeIOExpander = FALSE;
        watchdog_monitor(theContext.m_hcState,
                         &(theContext.m_watchdogFlags),
                         &(theContext.m_watchdogOpts),
                         &writeIOExpander,
                         &(theContext.m_details));

        if (writeIOExpander) {
            theContext.m_queuedI2cActions |= 1 << ((uint16_t) I2C_SENSORS__ACTIONS__WRITE_IO_EXPANDER);
            theContext.m_writeCustomIoExpanderValues = false;
            theContext.m_watchdogFlags |= WDFLAG_WAITING_FOR_IO_EXPANDER_WRITE;

            if (!theContext.m_i2cActive) {
                initiateNextI2cAction(theContext);
            }
        }

        // Queue up a read of the IO Expander, and initiate it if no other I2C action is active
        theContext.m_queuedI2cActions |= 1 << ((uint16_t) I2C_SENSORS__ACTIONS__READ_IO_EXPANDER);

        if (!theContext.m_i2cActive) {
            initiateNextI2cAction(theContext);
        }

        return getState();
    }

    RoverState RoverStateService::handlePowerIssue(RoverContext& /*theContext*/)
    {
        //!< @todo Implement RoverStateService::handlePowerIssue
        return getState();
    }

    RoverState RoverStateService::spinOnce(RoverContext& theContext)
    {
        if (theContext.m_i2cActive) {
            I2C_Sensors__Action action = {};
            uint8_t readValue = 0;
            I2C_Sensors__Status i2cStatus = I2C_Sensors__getActionStatus(&action,
                                                                         &(theContext.m_i2cReadings),
                                                                         &readValue);

            if (I2C_SENSORS__STATUS__INCOMPLETE != i2cStatus) {
                DEBUG_LOG_CHECK_STATUS(I2C_SENSORS__STATUS__SUCCESS_DONE, i2cStatus, "I2C action failed");

                if (I2C_SENSORS__ACTIONS__WRITE_IO_EXPANDER == action) {
                    theContext.m_watchdogFlags &= ~WDFLAG_WAITING_FOR_IO_EXPANDER_WRITE;
                }

                if (I2C_SENSORS__ACTIONS__READ_IO_EXPANDER == action) {
                    bool chargeStat2 = (readValue & I2C_SENSORS__IOE_P1_BIT__CHARGE_STAT2 != 0);
                    bool latchStat = (readValue & I2C_SENSORS__IOE_P1_BIT__LATCH_STAT != 0);

                    if (chargeStat2) {
                        SET_IPASBI_IN_UINT(theContext.m_details.m_inputPinAndStateBits, IPASBI__CHARGE_STAT2);
                    } else {
                        CLEAR_IPASBI_IN_UINT(theContext.m_details.m_inputPinAndStateBits, IPASBI__CHARGE_STAT2);
                    }

                    if (latchStat) {
                        SET_IPASBI_IN_UINT(theContext.m_details.m_inputPinAndStateBits, IPASBI__LATCH_STAT);
                    } else {
                        CLEAR_IPASBI_IN_UINT(theContext.m_details.m_inputPinAndStateBits, IPASBI__LATCH_STAT);
                    }
                }

                I2C_Sensors__clearLastAction();
                theContext.m_i2cActive = false;
                initiateNextI2cAction(theContext);
            }
        }

        if (theContext.m_sendDetailedReport) {
            theContext.m_sendDetailedReport = false;
            sendDetailedReportToLander(theContext);
        }

        return getState();
    }

    RoverState RoverStateService::transitionTo(RoverContext& theContext)
    {
        // Nothing to do on this transition, which should always be from ENTERING_SERVICE.
        m_currentSubState = SubState::SERVICE_NORMAL;
        return getState();
    }

    RoverState RoverStateService::performWatchdogCommand(RoverContext& theContext,
                                                         const WdCmdMsgs__Message& msg,
                                                         WdCmdMsgs__Response& response,
                                                         WdCmdMsgs__Response& deployNotificationResponse,
                                                         bool& sendDeployNotificationResponse)
    {
        // If we're in the KEEP_ALIVE_HOLDING substate and we receive any command other than EnterKeepAlive, then we
        // switch to the SERVICE_NORMAL substate.
        if (SubState::KEEP_ALIVE_HOLDING == m_currentSubState
                && msg.commandId != WD_CMD_MSGS__CMD_ID__ENTER_KEEPALIVE_MODE) {
            m_currentSubState = SubState::SERVICE_NORMAL;
        }

        // Other than resetting the substate, we want to rely on the default implementation of this function
        return RoverStateBase::performWatchdogCommand(theContext,
                                                      msg,
                                                      response,
                                                      deployNotificationResponse,
                                                      sendDeployNotificationResponse);
    }

    RoverState RoverStateService::doGndCmdPrepForDeploy(RoverContext& theContext,
                                                        const WdCmdMsgs__Message& msg,
                                                        WdCmdMsgs__Response& response,
                                                        WdCmdMsgs__Response& deployNotificationResponse,
                                                        bool& sendDeployNotificationResponse)
    {
        /* Can transition directly to mission mode from service */
        response.statusCode = WD_CMD_MSGS__RESPONSE_STATUS__SUCCESS;
        return RoverState::ENTERING_MISSION;
    }

    RoverState RoverStateService::doGndCmdEnterKeepAliveMode(RoverContext& theContext,
                                                             const WdCmdMsgs__Message& msg,
                                                             WdCmdMsgs__Response& response,
                                                             WdCmdMsgs__Response& deployNotificationResponse,
                                                             bool& sendDeployNotificationResponse)
    {
        // We only want to actually enter keep alive if we receive it twice in a row
        if (SubState::KEEP_ALIVE_HOLDING == m_currentSubState) {
            response.statusCode = WD_CMD_MSGS__RESPONSE_STATUS__SUCCESS;
            return RoverState::ENTERING_KEEP_ALIVE;
        } else {
            // Update the substate so that we know to actually transition to keep alive mode if we receive the command
            // againas the next command.
            m_currentSubState = SubState::KEEP_ALIVE_HOLDING;

            /**
             * @todo Is returning SUCCESS in this case the correct thing to do? If we do this then there's no way to
             *       differentiate this response from the one we will send when we actually switch mode.
             */
            response.statusCode = WD_CMD_MSGS__RESPONSE_STATUS__SUCCESS;

            // Stay in this state for now
            return getState();
        }
    }

    RoverState RoverStateService::doGndCmdEnterServiceMode(RoverContext& theContext,
                                                           const WdCmdMsgs__Message& msg,
                                                           WdCmdMsgs__Response& response,
                                                           WdCmdMsgs__Response& deployNotificationResponse,
                                                           bool& sendDeployNotificationResponse)
    {
        // Do the default behavior of complaining about being in the wrong state.
        return RoverStateBase::doGndCmdEnterServiceMode(theContext,
                                                        msg,
                                                        response,
                                                        deployNotificationResponse,
                                                        sendDeployNotificationResponse);
    }

} // End namespace iris
