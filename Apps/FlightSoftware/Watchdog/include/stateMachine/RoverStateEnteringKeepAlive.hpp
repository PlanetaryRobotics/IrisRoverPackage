#ifndef __WATCHDOG_ROVER_STATE_ENTERING_KEEP_ALIVE_HPP__
#define __WATCHDOG_ROVER_STATE_ENTERING_KEEP_ALIVE_HPP__

#include "include/stateMachine/RoverStateBase.hpp"

namespace iris
{
    class RoverStateEnteringKeepAlive : public RoverStateBase
    {
        public:
            enum class Status {
                SUCCESS,
            };

            explicit RoverStateEnteringKeepAlive();

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
            virtual RoverState nextStateAfterSetupCompletes();



            RoverState doGndCmdEnterKeepAliveMode(RoverContext& theContext,
                                                  const WdCmdMsgs__Message& msg,
                                                  WdCmdMsgs__Response& response,
                                                  WdCmdMsgs__Response& deployNotificationResponse,
                                                  bool& sendDeployNotificationResponse) override;

            RoverState performResetCommand(RoverContext& theContext,
                                           WdCmdMsgs__ResetSpecificId resetValue,
                                           WdCmdMsgs__Response* response) override;

        private:
            enum class SubState
            {
                WAITING_FOR_ADC_DONE,
                FINISH_UP_SETUP
            };

            SubState m_currentSubstate;

            RoverState transitionToWaitingForAdcDone(RoverContext& theContext);

            RoverState transitionToFinishUpSetup(RoverContext& theContext);
    };

}

#endif /* __WATCHDOG_ROVER_STATE_ENTERING_KEEP_ALIVE_HPP__ */
