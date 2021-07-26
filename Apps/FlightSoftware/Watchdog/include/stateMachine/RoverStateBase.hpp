#ifndef __WATCHDOG_ROVER_STATE_BASE_HPP__
#define __WATCHDOG_ROVER_STATE_BASE_HPP__

#include "stateMachine/RoverState.hpp"
#include "stateMachine/RoverContext.hpp"

namespace iris
{
    class RoverStateBase
    {
        public:
            explicit RoverStateBase(RoverState state);

            RoverState getState();

            virtual bool canEnterLowPowerMode();

            // The functions to handle events
            virtual RoverState handleLanderData(RoverContext& theContext);
            virtual RoverState handleHerculesData(RoverContext& theContext);

            virtual RoverState handleTimerTick(RoverContext& theContext) = 0;
            virtual RoverState handleI2cStarted(RoverContext& theContext) = 0;
            virtual RoverState handleI2cDone(RoverContext& theContext) = 0;
            virtual RoverState handleHighTemp(RoverContext& theContext) = 0;
            virtual RoverState handlePowerIssue(RoverContext& theContext) = 0;
            virtual RoverState spinOnce(RoverContext& theContext) = 0;

            virtual RoverState transitionTo(RoverContext& theContext) = 0;

            static void herculesMsgCallback(HercMsgs__Header* header,
                                            uint8_t* payloadBuffer,
                                            size_t payloadSize,
                                            void* userArg);

            static void landerMsgCallback(uint8_t* rxDataBuffer, size_t rxDataLen, void* userArg);

        protected:
            virtual RoverState performWatchdogCommand(RoverContext& theContext,
                                                      const WdCmdMsgs__Message& msg,
                                                      WdCmdMsgs__Response& response,
                                                      WdCmdMsgs__Response& deployNotificationResponse,
                                                      bool& sendDeployNotificationResponse);

            virtual RoverState performResetCommand(RoverContext& theContext,
                                                   WdCmdMsgs__ResetSpecificId resetValue,
                                                   WdCmdMsgs__Response* response) = 0;

            virtual RoverState handleUplinkFromLander(RoverContext& theContext,
                                                      uint8_t* rxDataBuffer,
                                                      size_t rxDataLen);

            virtual RoverState handleStrokeFromHercules(RoverContext& theContext,
                                                        HercMsgs__Header* header);

            virtual RoverState handleDownlinkFromHercules(RoverContext& theContext,
                                                          HercMsgs__Header* header,
                                                          uint8_t* payloadBuffer,
                                                          size_t payloadSize);

            virtual RoverState handleResetFromHercules(RoverContext& theContext,
                                                       HercMsgs__Header* header);

            virtual RoverState pumpMsgsFromLander(RoverContext& theContext);

            virtual RoverState pumpMsgsFromHercules(RoverContext& theContext);

            void sendLanderResponse(RoverContext& theContext, WdCmdMsgs__Response& response)

            // Specific watchdog command handling
            virtual RoverState doGndCmdResetSpecific(RoverContext& theContext,
                                                     const WdCmdMsgs__Message& msg,
                                                     WdCmdMsgs__Response& response,
                                                     WdCmdMsgs__Response& deployNotificationResponse,
                                                     bool& sendDeployNotificationResponse);

            virtual RoverState doGndCmdPrepForDeploy(RoverContext& theContext,
                                                     const WdCmdMsgs__Message& msg,
                                                     WdCmdMsgs__Response& response,
                                                     WdCmdMsgs__Response& deployNotificationResponse,
                                                     bool& sendDeployNotificationResponse);

            virtual RoverState doGndCmdDeploy(RoverContext& theContext,
                                              const WdCmdMsgs__Message& msg,
                                              WdCmdMsgs__Response& response,
                                              WdCmdMsgs__Response& deployNotificationResponse,
                                              bool& sendDeployNotificationResponse);

            virtual RoverState doGndCmdSwitchConnMode(RoverContext& theContext,
                                                      const WdCmdMsgs__Message& msg,
                                                      WdCmdMsgs__Response& response,
                                                      WdCmdMsgs__Response& deployNotificationResponse,
                                                      bool& sendDeployNotificationResponse);

            virtual RoverState doGndCmdSetHeaterKp(RoverContext& theContext,
                                                   const WdCmdMsgs__Message& msg,
                                                   WdCmdMsgs__Response& response,
                                                   WdCmdMsgs__Response& deployNotificationResponse,
                                                   bool& sendDeployNotificationResponse);

            virtual RoverState doGndCmdSetAutoHeaterOnValue(RoverContext& theContext,
                                                            const WdCmdMsgs__Message& msg,
                                                            WdCmdMsgs__Response& response,
                                                            WdCmdMsgs__Response& deployNotificationResponse,
                                                            bool& sendDeployNotificationResponse);

            virtual RoverState doGndCmdSetAutoHeaterOffValue(RoverContext& theContext,
                                                             const WdCmdMsgs__Message& msg,
                                                             WdCmdMsgs__Response& response,
                                                             WdCmdMsgs__Response& deployNotificationResponse,
                                                             bool& sendDeployNotificationResponse);

            virtual RoverState doGndCmdSetHeaterDutyCycleMax(RoverContext& theContext,
                                                             const WdCmdMsgs__Message& msg,
                                                             WdCmdMsgs__Response& response,
                                                             WdCmdMsgs__Response& deployNotificationResponse,
                                                             bool& sendDeployNotificationResponse);

            virtual RoverState doGndCmdSetHeaterDutyCyclePeriod(RoverContext& theContext,
                                                                const WdCmdMsgs__Message& msg,
                                                                WdCmdMsgs__Response& response,
                                                                WdCmdMsgs__Response& deployNotificationResponse,
                                                                bool& sendDeployNotificationResponse);

            virtual RoverState doGndCmdSetThermisterVSetpoint(RoverContext& theContext,
                                                              const WdCmdMsgs__Message& msg,
                                                              WdCmdMsgs__Response& response,
                                                              WdCmdMsgs__Response& deployNotificationResponse,
                                                              bool& sendDeployNotificationResponse);

            virtual RoverState doGndCmdEnterSleepMode(RoverContext& theContext,
                                                      const WdCmdMsgs__Message& msg,
                                                      WdCmdMsgs__Response& response,
                                                      WdCmdMsgs__Response& deployNotificationResponse,
                                                      bool& sendDeployNotificationResponse);

            virtual RoverState doGndCmdEnterKeepAliveMode(RoverContext& theContext,
                                                          const WdCmdMsgs__Message& msg,
                                                          WdCmdMsgs__Response& response,
                                                          WdCmdMsgs__Response& deployNotificationResponse,
                                                          bool& sendDeployNotificationResponse);

            virtual RoverState doGndCmdEnterServiceMode(RoverContext& theContext,
                                                        const WdCmdMsgs__Message& msg,
                                                        WdCmdMsgs__Response& response,
                                                        WdCmdMsgs__Response& deployNotificationResponse,
                                                        bool& sendDeployNotificationResponse);

            // Specific reset command handling
            void doConditionalResetSpecific(RoverContext& theContext,
                                            WdCmdMsgs__ResetSpecificId resetValue,
                                            WdCmdMsgs__Response* response,
                                            bool allowPowerOn,
                                            bool allowDisableRs422,
                                            bool allowDeploy,
                                            bool allowUndeploy);
        private:
            RoverState m_state;

            RoverState m_pumpMsgsFromLanderReturnState;
            RoverState m_pumpMsgsFromHerculesReturnState;
    };

}

#endif /* __WATCHDOG_ROVER_STATE_BASE_HPP__ */