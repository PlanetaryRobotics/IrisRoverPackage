#include "stateMachine/RoverStateEnteringKeepAlive.hpp"

#include "drivers/adc.h"
#include "drivers/bsp.h"

#include "ground_cmd.h"

#include <cassert>

namespace iris
{
    RoverStateEnteringKeepAlive::RoverStateEnteringKeepAlive()
            : RoverStateBase(RoverState::ENTERING_KEEP_ALIVE)
    {
    }

    bool RoverStateEnteringKeepAlive::canEnterLowPowerMode()
    {
        // Don't allow entering low power mode while entering keep alive. The only non-instant thing this state
        // does is to wait for the previous ADC transaction to complete. Since we don't wake from LPM after an ADC
        // reading, we can't enter LPM while waiting for the transaction to complete.
        return false;
    }

    RoverState RoverStateEnteringKeepAlive::handleHerculesData(RoverContext& /*theContext*/)
    {
        /**
         * @todo Should we re-enter this state upon this occurring, in order to re-set the bit that should power off
         *       the Hercules?
         */
        assert(!"Got hercules data event in EnteringKeepAlive, which shouldn't be possible");
        return getState();
    }

    RoverState RoverStateEnteringKeepAlive::handleTimerTick(RoverContext& theContext)
    {
        // NOTE: Don't try to sample the ADC as we're likely setting it up for the lander configuration

        theContext.m_keepAliveTickCount++;

        /* only send every 3 timer ticks (15s) */
        if (theContext.m_keepAliveTickCount >= 3) {
            theContext.m_keepAliveTickCount = 0;

            /* send heartbeat with collected data */
            static FlightEarthHeartbeat hb = { 0 };
            GroundCmd__Status gcStatus = GroundCmd__generateFlightEarthHeartbeat(&(theContext.i2cReadings),
                                                                                 &(theContext.m_adcValues),
                                                                                 &hb);

            assert(GND_CMD__STATUS__SUCCESS == gcStatus);

            LanderComms__Status lcStatus = LanderComms__txData(theContext.m_lcState,
                                                               hb->heartbeatOutBuffer,
                                                               sizeof(hb->heartbeatOutBuffer));

            assert(LANDER_COMMS__STATUS__SUCCESS == lcStatus);
            if (LANDER_COMMS__STATUS__SUCCESS != lcStatus) {
                //!< @todo Handling?
            }
        }

        if (heatingControlEnabled) {
            // calculate PWM duty cycle (if any) to apply to heater
            heaterControl();
        }
    }

    RoverState RoverStateEnteringKeepAlive::handleI2cStarted(RoverContext& /*theContext*/)
    {
        assert(!"Got I2C started event when it shouldn't be possible");
        return getState();
    }

    RoverState RoverStateEnteringKeepAlive::handleI2cDone(RoverContext& /*theContext*/)
    {
        assert(!"Got I2C done event when it shouldn't be possible");
        return getState();
    }

    RoverState RoverStateEnteringKeepAlive::handleHighTemp(RoverContext& /*theContext*/)
    {
        //!< @todo Implement RoverStateEnteringKeepAlive::handleHighTemp
        return getState();
    }

    RoverState RoverStateEnteringKeepAlive::handlePowerIssue(RoverContext& /*theContext*/)
    {
        //!< @todo Implement RoverStateEnteringKeepAlive::handlePowerIssue
        return getState();
    }

    RoverState RoverStateEnteringKeepAlive::spinOnce(RoverContext& theContext)
    {
        switch (m_currentSubstate) {
            case SubState::WAITING_FOR_ADC_DONE:
                if (isAdcSampleDone() == TRUE) {
                    return transitionToFinishUpSetup(theContext);
                }
                break;

            case SubState::FINISH_UP_SETUP:
                // Fall through
            default:
                // We should only ever spin in this state when the SubState is WAITING_FOR_ADC_DONE. Really we don't
                // need the substates, but I've kept them because they are representative of the stages of this state.
                assert(!"In spinOnce() not in WAITING_FOR_ADC_DONE substate, which shouldn't be possible");
                break;
        }

        // Last ADC sample still not done, so remain in the current state.
        return getState();
    }

    RoverState RoverStateEnteringKeepAlive::transitionTo(RoverContext& theContext)
    {
        return transitionToWaitingForAdcDone(theContext);
    }

    RoverState RoverStateEnteringKeepAlive::nextStateAfterSetupCompletes()
    {
        return RoverState::KEEP_ALIVE;
    }

    RoverState RoverStateEnteringKeepAlive::performResetCommand(RoverContext& theContext,
                                                                WdCmdMsgs__ResetSpecificId resetValue,
                                                                WdCmdMsgs__Response* response)
    {
        return RoverStateBase::doConditionalResetSpecific(theContext,
                                                          resetValue,
                                                          response,
                                                          false, // whether or not to allow power on
                                                          false, // whether or not to allow disabling RS422
                                                          false, // whether or not to allow deploy
                                                          false); // whether or not to allow undeploy
    }

    RoverState RoverStateEnteringKeepAlive::handleUplinkFromLander(RoverContext& theContext,
                                                                   uint8_t* rxDataBuffer,
                                                                   size_t rxDataLen)
    {
        // Ignore any uplink because the Hercules shouldn't be powered on.
        //!< @todo Should we somehow respond to the lander?
        return getState();
    }

    RoverState RoverStateEnteringKeepAlive::doGndCmdEnterKeepAliveMode(RoverContext& theContext,
                                                                       const WdCmdMsgs__Message& msg,
                                                                       WdCmdMsgs__Response& response,
                                                                       WdCmdMsgs__Response& deployNotificationResponse,
                                                                       bool& sendDeployNotificationResponse)
    {
        // We're already entering keep alive mode, but we can still re-transition into keep alive
        // once we receive this command.
        response.statusCode = WD_CMD_MSGS__RESPONSE_STATUS__SUCCESS;
        return transitionToWaitingForAdcDone(theContext);
    }

    RoverState RoverStateEnteringKeepAlive::transitionToWaitingForAdcDone(RoverContext& theContext)
    {
        // Power everything off and set resets. All of these are simply setting/clearing bits, so they are instant.
        powerOffFpga();
        powerOffMotors();
        powerOffRadio();
        powerOffHercules();
        setRadioReset();
        setFPGAReset();
        setMotorsReset();
        setHerculesReset();
        unsetDeploy();

        // Turn off voltage rails. All of these are simply setting/clearing bits, so they are instant.
        disable3V3PowerRail();
        disable24VPowerRail();
        disableBatteries();

        //!< @todo Stop I2C transfer if one is active.

        // We want to set up the ADC for reading values when attached to the lander. In order to do this setup, any
        // existing ADC reading must be done. If it's not done, we won't move forward until it is.
        bool lastSampleDone = (isAdcSampleDone() == TRUE);

        if (lastSampleDone) {
            // The last sample is done already, so advance immediately to FINISH_UP_SETUP
            return transitionToFinishUpSetup(theContext);
        } else {
            // We still need to wait for the last sample to complete, so remain in this state and this substate.
            m_currentSubstate = SubState::WAITING_FOR_ADC_DONE;
            return getState();
        }
    }

    RoverState RoverStateEnteringKeepAlive::transitionToFinishUpSetup(RoverContext& theContext)
    {
        // The last ADC sample being done should be a prerequisite of entering this state, which means that
        // this call shouldn't fail. However, we check the return value anyway and will transition back to the previous
        // state if it did fail.
        bool setupSuccess = (setupAdcForLander(&(theContext.m_watchdogFlags)) == TRUE);

        if (!setupSuccess) {
            return transitionToWaitingForAdcDone(theContext);
        }

        // These are simply setting/clearing bits, so they are instant.
        enableHeater();
        startChargingBatteries();

        return nextStateAfterSetupCompletes();
    }

} // End namespace iris
