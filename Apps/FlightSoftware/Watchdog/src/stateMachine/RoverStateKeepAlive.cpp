#include "include/stateMachine/RoverStateKeepAlive.hpp"

#include "include/drivers/adc.h"
#include "include/drivers/bsp.h"

#include "include/ground_cmd.h"

#include <cassert>

namespace iris
{
    RoverStateKeepAlive::RoverStateKeepAlive()
        : RoverStateBase(RoverState::KEEP_ALIVE)
    {
    }

    RoverState RoverStateKeepAlive::handleHerculesData(RoverContext& /*theContext*/)
    {
        assert(!"Got hercules data event in KeepAlive, which shouldn't be possible");
        return getState();
    }

    RoverState RoverStateKeepAlive::handleTimerTick(RoverContext& theContext)
    {
        // Trigger a new ADC sample if the previous one is done
        if (isAdcSampleDone()) {
            adcCheckVoltageLevels(&(theContext.m_adcValues));
        }

        // Then do everything else we did in EnteringKeepAlive timer tick
        RoverStateEnteringKeepAlive::handleTimerTick(theContext);
    }

    RoverState RoverStateKeepAlive::handleI2cStarted(RoverContext& /*theContext*/)
    {
        assert(!"Got I2C started event in KeepAlive, which shouldn't be possible");
        return getState();
    }

    RoverState RoverStateKeepAlive::handleI2cDone(RoverContext& /*theContext*/)
    {
        assert(!"Got I2C done event in KeepAlive, which shouldn't be possible");
        return getState();
    }

    RoverState RoverStateKeepAlive::handleHighTemp(RoverContext& /*theContext*/)
    {
        //!< @todo Implement RoverStateKeepAlive::handleHighTemp
        return getState();
    }

    RoverState RoverStateKeepAlive::handlePowerIssue(RoverContext& /*theContext*/)
    {
        //!< @todo Implement RoverStateKeepAlive::handlePowerIssue
        return getState();
    }

    RoverState RoverStateKeepAlive::spinOnce(RoverContext& theContext)
    {
        // This shouldn't get called, but do nothing
        return getState();
    }

    RoverState RoverStateKeepAlive::transitionTo(RoverContext& theContext)
    {
        // Nothing to do on this transition, which should always be from ENTERING_KEEP_ALIVE.
        return getState();
    }

    RoverState RoverStateKeepAlive::doGndCmdEnterKeepAliveMode(RoverContext& theContext,
                                                               const WdCmdMsgs__Message& msg,
                                                               WdCmdMsgs__Response& response,
                                                               WdCmdMsgs__Response& deployNotificationResponse,
                                                               bool& sendDeployNotificationResponse)
    {
        // We're already in keep alive mode, but we can still re-transition into keep alive
        // once we receive this command.
        response.statusCode = WD_CMD_MSGS__RESPONSE_STATUS__SUCCESS;
        return RoverState::ENTERING_KEEP_ALIVE;
    }

    RoverState RoverStateKeepAlive::doGndCmdEnterServiceMode(RoverContext& theContext,
                                                             const WdCmdMsgs__Message& msg,
                                                             WdCmdMsgs__Response& response,
                                                             WdCmdMsgs__Response& deployNotificationResponse,
                                                             bool& sendDeployNotificationResponse)
    {
        /* Can transition directly to service mode from keepalive */
        response.statusCode = WD_CMD_MSGS__RESPONSE_STATUS__SUCCESS;
        return RoverState::ENTERING_SERVICE;
    }

} // End namespace iris
