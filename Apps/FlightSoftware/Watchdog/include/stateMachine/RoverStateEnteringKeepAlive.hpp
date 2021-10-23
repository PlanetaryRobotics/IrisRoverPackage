#ifndef __WATCHDOG_ROVER_STATE_ENTERING_KEEP_ALIVE_HPP__
#define __WATCHDOG_ROVER_STATE_ENTERING_KEEP_ALIVE_HPP__

#include "comms/watchdog_cmd_msgs.h"
#include "stateMachine/RoverStateBase.hpp"

namespace iris
{
    class RoverStateEnteringKeepAlive : public RoverStateBase
    {
        public:
            explicit RoverStateEnteringKeepAlive();

            bool canEnterLowPowerMode(RoverContext& theContext) override;

            // The functions to handle events
            RoverState handleHerculesData(RoverContext& theContext) override;
            RoverState handleTimerTick(RoverContext& theContext) override;
            RoverState handleHighTemp(RoverContext& theContext) override;
            RoverState handlePowerIssue(RoverContext& theContext) override;
            RoverState spinOnce(RoverContext& theContext) override;

            RoverState transitionTo(RoverContext& theContext) override;

        protected:
            explicit RoverStateEnteringKeepAlive(RoverState overridingState);

            virtual RoverState nextStateAfterSetupCompletes();

            RoverState performResetCommand(RoverContext& theContext,
                                           WdCmdMsgs__ResetSpecificId resetValue,
                                           WdCmdMsgs__Response* response) override;

            RoverState handleUplinkFromLander(RoverContext& theContext,
                                              uint8_t* rxDataBuffer,
                                              size_t rxDataLen) override;

            RoverState doGndCmdEnterKeepAliveMode(RoverContext& theContext,
                                                  const WdCmdMsgs__Message& msg,
                                                  WdCmdMsgs__Response& response,
                                                  WdCmdMsgs__Response& deployNotificationResponse,
                                                  bool& sendDeployNotificationResponse) override;

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

#endif /* __WATCHDOG_ROVER_STATE_ENTERING_KEEP_ALIVE_HPP__ */
