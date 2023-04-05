#include "stateMachine/RoverStateEnteringService.hpp"

#include <cassert>

#include "comms/debug_comms.h"
#include "drivers/bsp.h"

namespace iris
{
    RoverStateEnteringService::RoverStateEnteringService()
            : RoverStateEnteringKeepAlive(RoverState::ENTERING_SERVICE)
    {
    }

    RoverStateEnteringService::RoverStateEnteringService(RoverState overridingState)
            : RoverStateEnteringKeepAlive(overridingState)
    {
    }

    RoverState RoverStateEnteringService::handleHerculesData(RoverContext& theContext)
    {
        // We want to handle Hercules data as normal in this state, so revert back to the implementation in the base
        // state class.
        return RoverStateBase::handleHerculesData(theContext);
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

    RoverState RoverStateEnteringService::spinOnce(RoverContext& theContext)
    {
        return RoverState::SERVICE;
    }

    RoverState RoverStateEnteringService::transitionTo(RoverContext& theContext)
    {
        *(theContext.m_persistentInMission) = false;

        DPRINTF("Defaulting MONITOR_HERCULES to OFF in SERVICE.");
        theContext.m_watchdogOpts &= ~WDOPT_MONITOR_HERCULES; // don't monitor Hercules for aliveness by default in service

        // Enable the falling edge interrupt for WD_INT (should be done after unlocking LOCKLPM5 per slau367p section 12.3.2)
        enableWdIntFallingEdgeInterrupt();

        return RoverState::SERVICE;
    }

    RoverState RoverStateEnteringService::performResetCommand(RoverContext& theContext,
                                                              WdCmdMsgs__ResetSpecificId resetValue,
                                                              WdCmdMsgs__Response* response)
    {
        bool writeIOExpander = false;
        RoverStateBase::doConditionalResetSpecific(theContext,
                                                   resetValue,
                                                   response,
                                                   true, // whether or not to allow power on
                                                   false, // whether or not to allow disabling RS422
                                                   false, // whether or not to allow deploy
                                                   false, // whether or not to allow undeploy
                                                   writeIOExpander);

        if (writeIOExpander) {
            theContext.m_queuedI2cActions |= 1 << ((uint16_t) I2C_SENSORS__ACTIONS__WRITE_IO_EXPANDER);
            theContext.m_writeCustomIoExpanderValues = false;
            theContext.m_watchdogFlags |= WDFLAG_WAITING_FOR_IO_EXPANDER_WRITE;

            if (!theContext.m_i2cActive) {
                initiateNextI2cAction(theContext);
            }
        }

        return getState();
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

    RoverState RoverStateEnteringService::doGndCmdEnterServiceMode(RoverContext& theContext,
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
