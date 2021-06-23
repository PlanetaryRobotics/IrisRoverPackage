#include "include/stateMachine/RoverStateEnteringService.hpp"

#include "include/drivers/adc.h"
#include "include/drivers/bsp.h"

#include "include/ground_cmd.h"

#include <cassert>

namespace iris
{
    RoverStateEnteringService::RoverStateEnteringService()
            : RoverStateBase(RoverState::ENTERING_SERVICE)
    {
    }

    RoverState RoverStateEnteringService::handleHerculesData(RoverContext& theContext)
    {
        // We want to handle Hercules data as normal in this state, so revert back to the implementation in the base
        // state class.
        return RoverStateBase::handleHerculesData(theContext);
    }

    RoverState RoverStateEnteringService::handleTimerTick(RoverContext& theContext)
    {
        // Trigger a new ADC sample if the previous one is done
        if (isAdcSampleDone()) {
            adcCheckVoltageLevels(&(theContext.m_adcValues));
        }

        /* send heartbeat with collected data */
        static FuEarthHeartbeat hb = { 0 };
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

    RoverState RoverStateEnteringService::handleHighTemp(RoverContext& /*theContext*/)
    {
        //!< @todo Implement RoverStateEnteringService::handleHighTemp
        return getState();
    }

    RoverState RoverStateEnteringService::handlePowerIssue(RoverContext& /*theContext*/)
    {
        //!< @todo Implement RoverStateEnteringService::handlePowerIssue
        return getState();
    }

    RoverState RoverStateEnteringService::nextStateAfterSetupCompletes()
    {
        return RoverState::SERVICE;
    }

    RoverState RoverStateEnteringService::performResetCommand(RoverContext& theContext,
                                                                WdCmdMsgs__ResetSpecificId resetValue,
                                                                WdCmdMsgs__Response* response)
    {
        return RoverStateBase::doConditionalResetSpecific(theContext,
                                                          resetValue,
                                                          response,
                                                          true, // whether or not to allow power on
                                                          false, // whether or not to allow disabling RS422
                                                          false, // whether or not to allow deploy
                                                          false); // whether or not to allow undeploy
    }

    RoverState RoverStateEnteringService::doGndCmdEnterKeepAliveMode(RoverContext& theContext,
                                                                       const WdCmdMsgs__Message& msg,
                                                                       WdCmdMsgs__Response& response,
                                                                       WdCmdMsgs__Response& deployNotificationResponse,
                                                                       bool& sendDeployNotificationResponse)
    {
        // In SERVICE we require EnterKeepAlive to be sent twice in a row in order to actually perform the transition,
        // so rather than implementing that a second time in this state we just refuse this command in this state.
        // We do this by reverting back to the base class's implementation of this function, which responds that the
        // WD is in the wrong state to perform the command.
        return RoverStateBase::doGndCmdEnterKeepAliveMode(theContext,
                                                          msg,
                                                          response,
                                                          deployNotificationResponse,
                                                          sendDeployNotificationResponse);
    }

    RoverState RoverStateKeepAlive::doGndCmdEnterServiceMode(RoverContext& theContext,
                                                             const WdCmdMsgs__Message& msg,
                                                             WdCmdMsgs__Response& response,
                                                             WdCmdMsgs__Response& deployNotificationResponse,
                                                             bool& sendDeployNotificationResponse)
    {
        // We're already entering service mode, but we can still re-transition into entering service mode
        // once we receive this command.
        response.statusCode = WD_CMD_MSGS__RESPONSE_STATUS__SUCCESS;
        return transitionTo(theContext);
    }

} // End namespace iris
