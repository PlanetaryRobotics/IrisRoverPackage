#include "stateMachine/RoverStateMission.hpp"

#include "drivers/adc.h"
#include "drivers/bsp.h"

#include "ground_cmd.h"

#include <cassert>

namespace iris
{
    RoverStateMission::RoverStateMission()
            : RoverStateBase(RoverState::MISSION),
              m_i2cTransactionActive(false)
    {
    }

    bool RoverStateMission::canEnterLowPowerMode()
    {
        // The only thing that is done in this state that requires us to stay out of LPM is processing an I2C
        // transaction. Therefore in this state allowing entering LPM is conditional on whether or not we're actively
        // performing an I2C transaction.
        return !m_i2cTransactionActive;
    }

    RoverState RoverStateMission::handleTimerTick(RoverContext& theContext)
    {
        // Trigger a new ADC sample if the previous one is done
        if (isAdcSampleDone()) {
            adcCheckVoltageLevels(&(theContext.m_adcValues));
        }

        /* send heartbeat with collected data */
        static FullEarthHeartbeat hb = { 0 };
        GroundCmd__Status gcStatus = GroundCmd__generateFullEarthHeartbeat(&(theContext.i2cReadings),
                                                                           &(theContext.m_adcValues),
                                                                           &hb);

        assert(GND_CMD__STATUS__SUCCESS == gcStatus);

        LanderComms__Status lcStatus = LanderComms__txData(theContext.m_lcState,
                                                           hb->heartbeatOutBuffer,
                                                           sizeof(hb->heartbeatOutBuffer));

        assert(LANDER_COMMS__STATUS__SUCCESS == lcStatus);
        if (LANDER_COMMS__STATUS__SUCCESS != lcStatus) {
            //!< @todo Handling?
        }

        if (heatingControlEnabled) {
            // calculate PWM duty cycle (if any) to apply to heater
            heaterControl();
        }

        watchdog_monitor(theContext.m_hcState,
                         &(theContext.m_watchdogFlags),
                         &(theContext.m_watchdogOpts));
    }

    RoverState RoverStateMission::handleHighTemp(RoverContext& /*theContext*/)
    {
        //!< @todo Implement RoverStateMission::handleHighTemp
        return getState();
    }

    RoverState RoverStateMission::handlePowerIssue(RoverContext& /*theContext*/)
    {
        //!< @todo Implement RoverStateMission::handlePowerIssue
        return getState();
    }

    RoverState RoverStateMission::spinOnce(RoverContext& theContext)
    {
        // Do nothing
        return getState();
    }

    RoverState RoverStateMission::transitionTo(RoverContext& theContext)
    {
        // Nothing to do on this transition, which should always be from ENTERING_SERVICE.
        m_currentSubState = SubState::SERVICE_NORMAL;
        return getState();
    }

    RoverState RoverStateMission::performWatchdogCommand(RoverContext& theContext,
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

    RoverState RoverStateMission::doGndCmdPrepForDeploy(RoverContext& theContext,
                                                        const WdCmdMsgs__Message& msg,
                                                        WdCmdMsgs__Response& response,
                                                        WdCmdMsgs__Response& deployNotificationResponse,
                                                        bool& sendDeployNotificationResponse)
    {
        /* Can transition directly to mission mode from service */
        response.statusCode = WD_CMD_MSGS__RESPONSE_STATUS__SUCCESS;
        return RoverState::ENTERING_MISSION;
    }

    RoverState RoverStateMission::doGndCmdEnterKeepAliveMode(RoverContext& theContext,
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

    RoverState RoverStateMission::doGndCmdEnterServiceMode(RoverContext& theContext,
                                                           const WdCmdMsgs__Message& msg,
                                                           WdCmdMsgs__Response& response,
                                                           WdCmdMsgs__Response& deployNotificationResponse,
                                                           bool& sendDeployNotificationResponse)
    {
        // We're already in service mode, but we can re-enter it here.
        /**
         * @todo This will have the effect of powering off all peripherals, since transitioning to service is currently
         *       identical to transitioning to KeepAlive. Should we do this transition or should we just refuse to do
         *       this command when in this state?
         */
        response.statusCode = WD_CMD_MSGS__RESPONSE_STATUS__SUCCESS;
        return RoverState::ENTERING_SERVICE;
    }

} // End namespace iris