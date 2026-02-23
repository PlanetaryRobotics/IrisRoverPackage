#ifndef __WATCHDOG_ROVER_STATE_ENTERING_STASIS_HPP__
#define __WATCHDOG_ROVER_STATE_ENTERING_STASIS_HPP__

#include "stateMachine/RoverStateEnteringKeepAlive.hpp"

namespace iris
{
    class RoverStateEnteringStasis : public RoverStateEnteringKeepAlive
    {
        public:
            explicit RoverStateEnteringStasis();

            // The functions to handle events
            RoverState handlePowerIssue(RoverContext& theContext) override;
            RoverState handleHerculesData(RoverContext& theContext) override;
            RoverState handleTimerTick(RoverContext& theContext) override;
            RoverState spinOnce(RoverContext& theContext) override;

            RoverState transitionTo(RoverContext& theContext) override;

        protected:
            explicit RoverStateEnteringStasis(RoverState overridingState);

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

            RoverState handleUplinkFromLander(RoverContext& theContext,
                                                          uint8_t* rxDataBuffer,
                                                          size_t rxDataLen) override;

        private:
            enum class SubState
            {
                WAITING_FOR_IO_EXPANDER_WRITE,
                FINISH_UP_SETUP
            };

            SubState m_currentSubstate;

            RoverState transitionToWaitingForIoExpanderWrite(RoverContext& theContext);

            RoverState transitionToFinishUpSetup(RoverContext& theContext);
    };

}

#endif /* __WATCHDOG_ROVER_STATE_ENTERING_STASIS_HPP__ */
