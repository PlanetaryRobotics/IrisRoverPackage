#ifndef __WATCHDOG_ROVER_STATE_BASE_HPP__
#define __WATCHDOG_ROVER_STATE_BASE_HPP__

#include "comms/watchdog_cmd_msgs.h"
#include "stateMachine/RoverState.hpp"
#include "stateMachine/RoverContext.hpp"

#include "flags.h"

namespace iris
{
    class RoverStateBase
    {
        public:
            explicit RoverStateBase(RoverState state);

            RoverState getState();

            virtual bool canEnterLowPowerMode(RoverContext& theContext);

            // The functions to handle events
            virtual RoverState handleLanderData(RoverContext& theContext);
            virtual RoverState handleHerculesData(RoverContext& theContext);
            virtual RoverState handleHighTemp(RoverContext& theContext);

            virtual RoverState handleTimerTick(RoverContext& theContext) = 0;
            virtual RoverState handlePowerIssue(RoverContext& theContext) = 0;
            virtual RoverState spinOnce(RoverContext& theContext) = 0;

            virtual RoverState transitionTo(RoverContext& theContext) = 0;

            static void herculesMsgCallback(HercMsgs__Header* header,
                                            uint8_t* payloadBuffer,
                                            size_t payloadSize,
                                            void* userArg);

            static void landerMsgCallback(uint8_t* rxDataBuffer, size_t rxDataLen, void* userArg);

        protected:
            virtual void initiateNextI2cAction(RoverContext& theContext);

            virtual void heaterControl(RoverContext& theContext);

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

            void sendLanderResponse(RoverContext& theContext, WdCmdMsgs__Response& response);

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

            virtual RoverState doGndCmdDangForceBattState(RoverContext& theContext,
                                                          const WdCmdMsgs__Message& msg,
                                                          WdCmdMsgs__Response& response,
                                                          WdCmdMsgs__Response& deployNotificationResponse,
                                                          bool& sendDeployNotificationResponse);

            virtual RoverState doGndCmdSetChargeEnState(RoverContext& theContext,
                                                        const WdCmdMsgs__Message& msg,
                                                        WdCmdMsgs__Response& response,
                                                        WdCmdMsgs__Response& deployNotificationResponse,
                                                        bool& sendDeployNotificationResponse);

            virtual RoverState doGndCmdSetChargeRegEnState(RoverContext& theContext,
                                                           const WdCmdMsgs__Message& msg,
                                                           WdCmdMsgs__Response& response,
                                                           WdCmdMsgs__Response& deployNotificationResponse,
                                                           bool& sendDeployNotificationResponse);

            virtual RoverState doGndCmdSetBattEnState(RoverContext& theContext,
                                                      const WdCmdMsgs__Message& msg,
                                                      WdCmdMsgs__Response& response,
                                                      WdCmdMsgs__Response& deployNotificationResponse,
                                                      bool& sendDeployNotificationResponse);

            virtual RoverState doGndCmdSetBattCtrlEnState(RoverContext& theContext,
                                                          const WdCmdMsgs__Message& msg,
                                                          WdCmdMsgs__Response& response,
                                                          WdCmdMsgs__Response& deployNotificationResponse,
                                                          bool& sendDeployNotificationResponse);

            virtual RoverState doGndCmdSetLatchBattState(RoverContext& theContext,
                                                         const WdCmdMsgs__Message& msg,
                                                         WdCmdMsgs__Response& response,
                                                         WdCmdMsgs__Response& deployNotificationResponse,
                                                         bool& sendDeployNotificationResponse);

            virtual RoverState doGndCmdLatchSetPulseLow(RoverContext& theContext,
                                                        const WdCmdMsgs__Message& msg,
                                                        WdCmdMsgs__Response& response,
                                                        WdCmdMsgs__Response& deployNotificationResponse,
                                                        bool& sendDeployNotificationResponse);

            virtual RoverState doGndCmdLatchResetPulseLow(RoverContext& theContext,
                                                          const WdCmdMsgs__Message& msg,
                                                          WdCmdMsgs__Response& response,
                                                          WdCmdMsgs__Response& deployNotificationResponse,
                                                          bool& sendDeployNotificationResponse);

            virtual RoverState doGndCmdClearResetMemory(RoverContext& theContext,
                                                        const WdCmdMsgs__Message& msg,
                                                        WdCmdMsgs__Response& response,
                                                        WdCmdMsgs__Response& deployNotificationResponse,
                                                        bool& sendDeployNotificationResponse);

            virtual RoverState doGndCmdRequestDetailedReport(RoverContext& theContext,
                                                             const WdCmdMsgs__Message& msg,
                                                             WdCmdMsgs__Response& response,
                                                             WdCmdMsgs__Response& deployNotificationResponse,
                                                             bool& sendDeployNotificationResponse);

            virtual void sendDetailedReportToLander(RoverContext& theContext);

            // Specific reset command handling
            void doConditionalResetSpecific(RoverContext& theContext,
                                            WdCmdMsgs__ResetSpecificId resetValue,
                                            WdCmdMsgs__Response* response,
                                            bool allowPowerOn,
                                            bool allowDisableRs422,
                                            bool allowDeploy,
                                            bool allowUndeploy,
                                            bool& needToWriteIoExpander);
        private:
            RoverState m_state;

            RoverState m_pumpMsgsFromLanderReturnState;
            RoverState m_pumpMsgsFromHerculesReturnState;
    };

}

#endif /* __WATCHDOG_ROVER_STATE_BASE_HPP__ */
