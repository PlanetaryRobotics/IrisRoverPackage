#ifndef __WATCHDOG_ROVER_STATE_ENTERING_MISSION_HPP__
#define __WATCHDOG_ROVER_STATE_ENTERING_MISSION_HPP__

#include "stateMachine/RoverStateBase.hpp"

namespace iris
{
    class RoverStateEnteringMission : public RoverStateBase
    {
        public:
            /**
             * @brief The amount of time, in centiseconds, to wait for the fuel gauge to successfully initialize.
             */
            static constexpr uint16_t FUEL_GAUGE_INIT_TIMEOUT_CENTISECONDS = 20;

            /**
             * @brief The amount of time, in centiseconds, to wait for wifi to become ready.
             */
            static constexpr uint16_t WIFI_READY_TIMEOUT_CENTISECONDS = 2500; // 25s (typ. connection is in ~8.5s)

            explicit RoverStateEnteringMission();

            bool canEnterLowPowerMode(RoverContext& theContext) override;

            // The functions to handle events
            RoverState handleTimerTick(RoverContext& theContext) override;
            RoverState handlePowerIssue(RoverContext& theContext) override;
            RoverState spinOnce(RoverContext& theContext) override;

            RoverState transitionTo(RoverContext& theContext) override;

        protected:
            RoverState performResetCommand(RoverContext& theContext,
                                           WdCmdMsgs__ResetSpecificId resetValue,
                                           WdCmdMsgs__Response* response) override;

        private:
            enum class SubState
            {
                WAITING_FOR_I2C_DONE,
                WAITING_FOR_IO_EXPANDER_WRITE_1,
                WAITING_FOR_IO_EXPANDER_WRITE_2,
                WAITING_FOR_FUEL_GAUGE_OR_TIMEOUT,
                WAITING_FOR_WIFI_READY_OR_TIMEOUT,
                WAITING_FOR_IO_EXPANDER_WRITE_3
            };

            SubState m_currentSubstate;
            uint16_t m_startFuelGaugeInitTimeCentiseconds;
            uint16_t m_startWifiReadyTimeCentiseconds;

            RoverState transitionToWaitingForI2cDone(RoverContext& theContext);

            RoverState transitionToWaitingForIoExpanderWrite1(RoverContext& theContext);

            RoverState transitionToWaitingForIoExpanderWrite2(RoverContext& theContext);

            RoverState transitionToWaitingForFuelGaugeOrTimeout(RoverContext& theContext);

            RoverState transitionToWatitingForWifiReadyOrTimeout(RoverContext& theContext);

            RoverState transitionToWaitingForIoExpanderWrite3(RoverContext& theContext);

            void enableHerculesComms(RoverContext& theContext);
    };

}

#endif /* __WATCHDOG_ROVER_STATE_ENTERING_MISSION_HPP__ */
