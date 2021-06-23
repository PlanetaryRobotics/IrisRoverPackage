#ifndef __WATCHDOG_ROVER_STATE_ENTERING_SERVICE_HPP__
#define __WATCHDOG_ROVER_STATE_ENTERING_SERVICE_HPP__

#include "include/stateMachine/RoverStateEnteringKeepAlive.hpp"

namespace iris
{
    class RoverStateEnteringService : public RoverStateEnteringKeepAlive
    {
        public:
            explicit RoverStateEnteringService();

            // The functions to handle events
            RoverState handleHerculesData(RoverContext& theContext) override;
            RoverState handleTimerTick(RoverContext& theContext) override;
            RoverState handleHighTemp(RoverContext& theContext) override;
            RoverState handlePowerIssue(RoverContext& theContext) override;

        protected:
            RoverState nextStateAfterSetupCompletes() override;

            RoverState performResetCommand(RoverContext& theContext,
                                           WdCmdMsgs__ResetSpecificId resetValue,
                                           WdCmdMsgs__Response* response) override;

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

#endif /* __WATCHDOG_ROVER_STATE_ENTERING_SERVICE_HPP__ */
