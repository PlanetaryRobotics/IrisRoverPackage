#ifndef __WATCHDOG_ROVER_STATE_INIT_HPP__
#define __WATCHDOG_ROVER_STATE_INIT_HPP__

#include "stateMachine/RoverStateBase.hpp"

namespace iris
{
    class RoverStateInit : public RoverStateBase
    {
        public:
            explicit RoverStateInit(RoverState firstState);

            bool canEnterLowPowerMode(RoverContext& theContext) override;

            // The functions to handle events
            virtual RoverState handleLanderData(RoverContext& theContext) override;
            virtual RoverState handleHerculesData(RoverContext& theContext) override;
            virtual RoverState handleTimerTick(RoverContext& theContext) override;
            virtual RoverState handleHighTemp(RoverContext& theContext) override;
            virtual RoverState handlePowerIssue(RoverContext& theContext) override;
            virtual RoverState spinOnce(RoverContext& theContext) override;
            virtual RoverState transitionTo(RoverContext& theContext) override;

        protected:
            RoverState performResetCommand(RoverContext& theContext,
                                                   WdCmdMsgs__ResetSpecificId resetValue,
                                                   WdCmdMsgs__Response* response) override;

        private:
            RoverState m_firstState;
    };

}

#endif /* __WATCHDOG_ROVER_STATE_INIT_HPP__ */
