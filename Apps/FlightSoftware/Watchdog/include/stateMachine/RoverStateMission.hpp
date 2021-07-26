#ifndef __WATCHDOG_ROVER_STATE_MISSION_HPP__
#define __WATCHDOG_ROVER_STATE_MISSION_HPP__

#include "stateMachine/RoverStateBase.hpp"

namespace iris
{
    //!< @todo IMPLEMENT MISSION STATE!
    class RoverStateMission : public RoverStateBase
    {
        public:
            explicit RoverStateMission();

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

        private:
            enum class SubState
            {
                SERVICE_NORMAL,
                KEEP_ALIVE_HOLDING
            };

            SubState m_currentSubState;
            bool m_i2cTransactionActive;
    };

}

#endif /* __WATCHDOG_ROVER_STATE_MISSION_HPP__ */
