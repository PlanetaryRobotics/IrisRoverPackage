#ifndef __WATCHDOG_ROVER_STATE_KEEP_ALIVE_HPP__
#define __WATCHDOG_ROVER_STATE_KEEP_ALIVE_HPP__

#include "stateMachine/RoverStateBase.hpp"

namespace iris
{
    class RoverStateKeepAlive : public RoverStateEnteringKeepAlive
    {
        public:
            explicit RoverStateKeepAlive();

            bool canEnterLowPowerMode() override;

            // The functions to handle events
            RoverState handleHerculesData(RoverContext& theContext) override;
            RoverState handleTimerTick(RoverContext& theContext) override;
            RoverState handleI2cStarted(RoverContext& theContext) override;
            RoverState handleI2cDone(RoverContext& theContext) override;
            RoverState handleHighTemp(RoverContext& theContext) override;
            RoverState handlePowerIssue(RoverContext& theContext) override;
            RoverState spinOnce(RoverContext& theContext) override;

            RoverState transitionTo(RoverContext& theContext) override;

        protected:
            RoverState doGndCmdEnterKeepAliveMode(RoverContext& theContext,
                                                  const WdCmdMsgs__Message& msg,
                                                  WdCmdMsgs__Response& response,
                                                  WdCmdMsgs__Response& deployNotificationResponse,
                                                  bool& sendDeployNotificationResponse) override;

            RoverState doGndCmdEnterServiceMode(RoverContext& theContext,
                                                const WdCmdMsgs__Message& msg,
                                                WdCmdMsgs__Response& response,
                                                WdCmdMsgs__Response& deployNotificationResponse,
                                                bool& sendDeployNotificationResponse) override;
    };

}

#endif /* __WATCHDOG_ROVER_STATE_ENTERING_KEEP_ALIVE_HPP__ */
