#include "stateMachine/RoverStateKeepAlive.hpp"

#include "drivers/adc.h"
#include "drivers/bsp.h"
#include "utils/time.h"

#include "ground_cmd.h"

#include <cassert>

namespace iris
{
    RoverStateKeepAlive::RoverStateKeepAlive()
        : RoverStateEnteringKeepAlive(RoverState::KEEP_ALIVE)
    {
    }

    bool RoverStateKeepAlive::canEnterLowPowerMode(RoverContext& /*theContext*/)
    {
        // Handling lander data and timer ticks will both wake us up out of LPM, so it's ok for us to enter it.
        return true;
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
        // Do nothing
        //DPRINTF_ERR("%d\n", Time__getTimeInCentiseconds());
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
