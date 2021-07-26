#include "stateMachine/RoverStateInit.hpp"

#include <cassert>

namespace iris
{
    RoverStateInit::RoverStateInit(RoverState firstState)
            : RoverStateBase(RoverState::INIT),
              m_firstState(firstState)
    {
    }

    RoverState RoverStateInit::handleLanderData(RoverContext& theContext)
    {
        assert(!"Handling data from lander event in INIT state, which shouldn't be possible");
        return getState();
    }

    RoverState RoverStateInit::handleHerculesData(RoverContext& /*theContext*/)
    {
        assert(!"Handling data from hercules event in INIT state, which shouldn't be possible");
        return getState();
    }

    RoverState RoverStateInit::handleTimerTick(RoverContext& theContext)
    {
        assert(!"Handling timer tick event in INIT state, which shouldn't be possible");
        return getState();
    }

    RoverState RoverStateInit::handleI2cStarted(RoverContext& /*theContext*/)
    {
        assert(!"Handling I2C started event in INIT state, which shouldn't be possible");
        return getState();
    }

    RoverState RoverStateInit::handleI2cDone(RoverContext& /*theContext*/)
    {
        assert(!"Handling I2C done event in INIT state, which shouldn't be possible");
        return getState();
    }

    RoverState RoverStateInit::handleHighTemp(RoverContext& /*theContext*/)
    {
        assert(!"Handling high temp event in INIT state, which shouldn't be possible");
        return getState();
    }

    RoverState RoverStateInit::handlePowerIssue(RoverContext& /*theContext*/)
    {
        assert(!"Handling power issue event in INIT state, which shouldn't be possible");
        return getState();
    }

    RoverState RoverStateInit::spinOnce(RoverContext& theContext)
    {
        assert(!"In spinOnce() in INIT state, which shouldn't be possible");
        return getState();
    }

    RoverState RoverStateInit::transitionTo(RoverContext& theContext)
    {
        return RoverState::ENTERING_KEEP_ALIVE;
    }

    RoverState RoverStateInit::performResetCommand(RoverContext& theContext,
                                                                WdCmdMsgs__ResetSpecificId resetValue,
                                                                WdCmdMsgs__Response* response)
    {
        assert(!"Performing reset command in INIT state, which shouldn't be possible");
        return getState();
    }
} // End namespace iris
