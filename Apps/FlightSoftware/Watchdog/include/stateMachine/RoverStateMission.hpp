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

            bool canEnterLowPowerMode(RoverContext& theContext) override;

            // The functions to handle events
            RoverState handleTimerTick(RoverContext& theContext) override;
            RoverState handlePowerIssue(RoverContext& theContext) override;
            RoverState spinOnce(RoverContext& theContext) override;

            RoverState transitionTo(RoverContext& theContext) override;

        protected:
            void heaterControl(RoverContext& theContext) override;

            RoverState performResetCommand(RoverContext& theContext,
                                           WdCmdMsgs__ResetSpecificId resetValue,
                                           WdCmdMsgs__Response* response) override;

            RoverState performWatchdogCommand(RoverContext& theContext,
                                              const WdCmdMsgs__Message& msg,
                                              WdCmdMsgs__Response& response,
                                              WdCmdMsgs__Response& deployNotificationResponse,
                                              bool& sendDeployNotificationResponse) override;

            RoverState doGndCmdDeploy(RoverContext& theContext,
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
                MISSION_NORMAL,
                SERVICE_HOLDING
            };

            enum class DeployState
            {
                NOT_DEPLOYED,
                DEPLOYING,
                DEPLOYED
            };

            SubState m_currentSubState;
            DeployState m_currentDeployState;

    };

}

#endif /* __WATCHDOG_ROVER_STATE_MISSION_HPP__ */
