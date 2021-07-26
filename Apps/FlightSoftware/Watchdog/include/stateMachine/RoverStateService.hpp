#ifndef __WATCHDOG_ROVER_STATE_SERVICE_HPP__
#define __WATCHDOG_ROVER_STATE_SERVICE_HPP__

#include "stateMachine/RoverStateEnteringService.hpp"

namespace iris
{
    class RoverStateService : public RoverStateEnteringService
    {
        public:
            explicit RoverStateService();

            bool canEnterLowPowerMode() override;

            // The functions to handle events
            RoverState handleTimerTick(RoverContext& theContext) override;
            RoverState handleHighTemp(RoverContext& theContext) override;
            RoverState handlePowerIssue(RoverContext& theContext) override;
            RoverState spinOnce(RoverContext& theContext) override;

            RoverState transitionTo(RoverContext& theContext) override;

        protected:
            RoverState performWatchdogCommand(RoverContext& theContext,
                                              const WdCmdMsgs__Message& msg,
                                              WdCmdMsgs__Response& response,
                                              WdCmdMsgs__Response& deployNotificationResponse,
                                              bool& sendDeployNotificationResponse) override;

            RoverState doGndCmdPrepForDeploy(RoverContext& theContext,
                                             const WdCmdMsgs__Message& msg,
                                             WdCmdMsgs__Response& response,
                                             WdCmdMsgs__Response& deployNotificationResponse,
                                             bool& sendDeployNotificationResponse) override;

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

#endif /* __WATCHDOG_ROVER_STATE_SERVICE_HPP__ */
