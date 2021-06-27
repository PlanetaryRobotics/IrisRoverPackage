#include "include/stateMachine/RoverStateEnteringMission.hpp"

#include "include/comms/i2c_sensors.h"

#include "include/drivers/adc.h"
#include "include/drivers/bsp.h"

#include "include/utils/time.h"

#include "include/ground_cmd.h"

#include <cassert>

namespace iris
{
    RoverStateEnteringMission::RoverStateEnteringMission()
            : RoverStateBase(RoverState::ENTERING_MISSION)
    {
    }

    RoverState RoverStateEnteringMission::handleHerculesData(RoverContext& /*theContext*/)
    {
        /**
         * @todo Should we re-enter this state upon this occurring, in order to re-set the bit that should power off
         *       the Hercules?
         */
        assert(!"Got hercules data event in EnteringKeepAlive, which shouldn't be possible");
        return getState();
    }

    RoverState RoverStateEnteringMission::handleTimerTick(RoverContext& theContext)
    {
        // NOTE: Don't try to sample the ADC as we may be setting it up for mission configuration

        /* send heartbeat with collected data */
        static FullEarthHeartbeat hb = { 0 };
        GroundCmd__Status gcStatus = GroundCmd__generateFullEarthHeartbeat(&(theContext.i2cReadings),
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

        if (heatingControlEnabled) {
            // calculate PWM duty cycle (if any) to apply to heater
            heaterControl();
        }

        watchdog_monitor(theContext.m_hcState,
                         &(theContext.m_watchdogFlags),
                         &(theContext.m_watchdogOpts));
    }

    RoverState RoverStateEnteringMission::handleHighTemp(RoverContext& /*theContext*/)
    {
        //!< @todo Implement RoverStateEnteringMission::handleHighTemp
        return getState();
    }

    RoverState RoverStateEnteringMission::handlePowerIssue(RoverContext& /*theContext*/)
    {
        //!< @todo Implement RoverStateEnteringMission::handlePowerIssue
        return getState();
    }

    RoverState RoverStateEnteringMission::spinOnce(RoverContext& theContext)
    {
        switch (m_currentSubstate) {
            case SubState::WAITING_FOR_ADC_DONE:
                if (isAdcSampleDone() == TRUE) {
                    return transitionToWaitingForFuelGaugeOrTimeout(theContext);
                }
                break;

            case SubState::WAITING_FOR_FUEL_GAUGE_OR_TIMEOUT:
                {
                    // First of all, if we've timed out we can simply move forward.
                    uint16_t timePassed = Time__getTimeInCentiseconds() - m_startFuelGaugeInitTimeCentiseconds;

                    if (timePassed > FUEL_GAUGE_INIT_TIMEOUT_CENTISECONDS) {
                        DPRINTF("Setting up fuel gauge timed out\n");
                        return transitionToWatitingForWifiReadyOrTimeout(theContext);
                    }

                    // Next, check if initializing the fuel gauge is complete (either due to success or failure).
                    I2C_Sensors__Action currentAction = I2C_SENSORS__ACTIONS__INACTIVE;
                    I2C_Sensors__Status i2cStatus = I2C_Sensors__getActionStatus(&currentAction, NULL, NULL);

                    // First of all, we should always be doing the action to initialize the fuel gauges at this point
                    assert(I2C_SENSORS__ACTIONS__GAUGE_INIT == currentAction);

                    if (I2C_SENSORS__STATUS__SUCCESS_DONE == i2cStatus) {
                        // Gauge initialization completed successfully, so move forward

                        // First clear the last I2C_Sensors action so we cna start a new one later
                        I2C_Sensors__clearLastAction();

                        // Then move forward
                        return transitionToWatitingForWifiReadyOrTimeout(theContext);
                    } else if (I2C_SENSORS__STATUS__ERROR__DONE_WITH_NACKS == i2cStatus) {
                        // Gauge initialization failed, but we haven't timed out yet. Therefore we retry the
                        // initialization action.

                        // First clear the current (completed) action from the I2C_Sensors module so we can start a new
                        // one.
                        I2C_Sensors__clearLastAction();

                        // Kick off the series of I2C transactions to initialize the fuel gauge.
                        I2C_Sensors__Status i2cStatus = I2C_Sensors__initiateFuelGaugeInitialization();

                        // Since we cleared the I2C transaction before starting this call, it should never fail
                        assert(I2C_SENSORS__STATUS__SUCCESS_DONE == i2cStatus);
                    } else {
                        DEBUG_LOG_CHECK_STATUS(I2C_SENSORS__STATUS__SUCCESS_DONE,
                                               i2cStatus,
                                               "Unexpected failure while getting I2C status");
                        // I guess retry entering mission?
                        //!< @todo What to do here? Enter FAULT?
                        return transitionToWaitingForAdcDone(theContext);
                    }

                    break;
                }

            case SubState::WAITING_FOR_FUEL_GAUGE_OR_TIMEOUT:
                {
                    // First of all, if we've timed out we can simply move forward.
                    uint16_t timePassed = Time__getTimeInCentiseconds() - m_startWifiReadyTimeCentiseconds;

                    if (timePassed > WIFI_READY_TIMEOUT_CENTISECONDS) {
                        DPRINTF("Wait for wifi timed out\n");
                        return transitionToFinishUpSetup(theContext);
                    }
                    break;
                }

            case SubState::FINISH_UP_SETUP:
                assert(!"In spinOnce() not in FINISH_UP_SETUP substate, which shouldn't be possible");
                break;

            default:
                assert(!"In spinOnce() in default case, which shouldn't be possible");
                break;
        }

        // Remain in the current state.
        return getState();
    }

    RoverState RoverStateEnteringMission::transitionTo(RoverContext& theContext)
    {
        return transitionToWaitingForAdcDone(theContext);
    }

    RoverState RoverStateEnteringMission::performResetCommand(RoverContext& theContext,
                                                              WdCmdMsgs__ResetSpecificId resetValue,
                                                              WdCmdMsgs__Response* response)
    {
        return RoverStateBase::doConditionalResetSpecific(theContext,
                                                          resetValue,
                                                          response,
                                                          true, // whether or not to allow power on
                                                          false, // whether or not to allow disabling RS422
                                                          false, // whether or not to allow deploy
                                                          false); // whether or not to allow undeploy
    }

    RoverState RoverStateEnteringMission::transitionToWaitingForAdcDone(RoverContext& theContext)
    {
        /* bootup process - enable all rails */
        enable3V3PowerRail();
        enable24VPowerRail();
        enableBatteries();
        disableHeater();
        unsetDeploy();

        /* enable hercules uart */
        //!< @todo Reset UART0 to a clean slate.
        /* power everything on and release resets */
        releaseRadioReset();
        releaseFPGAReset();

        // We want to set up the ADC for reading values when not attached to the lander. In order to do this setup, any
        // existing ADC reading must be done. If it's not done, we won't move forward until it is.
        bool lastSampleDone = (isAdcSampleDone() == TRUE);

        if (lastSampleDone) {
            // The last sample is done already, so advance immediately to FINISH_UP_SETUP
            return transitionToWaitingForFuelGaugeOrTimeout(theContext);
        } else {
            // We still need to wait for the last sample to complete, so remain in this state and this substate.
            m_currentSubstate = SubState::WAITING_FOR_ADC_DONE;
            return getState();
        }
    }

    RoverState RoverStateEnteringMission::transitionToWaitingForFuelGaugeOrTimeout(RoverContext& theContext)
    {
        // The last ADC sample being done should be a prerequisite of entering this state, which means that
        // this call shouldn't fail. However, we check the return value anyway and will transition back to the previous
        // state if it did fail.
        bool setupSuccess = (setupAdcForMission(&(theContext.m_watchdogFlags)) == TRUE);

        if (!setupSuccess) {
            return transitionToWaitingForAdcDone(theContext);
        }

        /* Power stuff on. These are simply setting/clearing bits, so they are instant. */
        powerOnFpga();
        powerOnMotors();
        powerOnRadio();
        stopChargingBatteries();

        // Stop any ongoing I2C transaction, as initializing the fuel gauge should take priority over finishing any
        // existing one. I'm not even sure how we'd get into a state where another I2C transaction was already in
        // progress, but we'll stop it here anyway just in case.
        I2C_Sensors__stop();

        // Record the start time for the time period in which we'll wait for the fuel gauge to be initialized
        // successfully. If it isn't done being initialized by the end of the timeout period, then we'll move forward
        // without the fuel gauge.
        m_startFuelGaugeInitTimeCentiseconds = Time__getTimeInCentiseconds();

        // Kick off the series of I2C transactions to initialize the fuel gauge.
        I2C_Sensors__Status i2cStatus = I2C_Sensors__initiateFuelGaugeInitialization();

        // Since we stopped any ongoing I2C transaction before starting this call, it should never fail
        assert(I2C_SENSORS__STATUS__SUCCESS_DONE == i2cStatus);

        // We're done transitioning. The spinOnce() for this substate will monitor for completion of initialization
        // and either restart it (if it failed with NACK), or move forward to the next substate (if it succeeded). The
        // spinOnce() for this substate will also monitor for the timeout elapsing.
        m_currentSubstate = SubState::WAITING_FOR_FUEL_GAUGE_OR_TIMEOUT;
        return getState();
    }

    RoverState RoverStateEnteringMission::transitionToWatitingForWifiReadyOrTimeout(RoverContext& theContext)
    {
        // Record the start time for the time period in which we'll wait for the wifi to become ready for use. If it
        // isn't ready by the end of the timeout period, then we'll move forward without wifi.
        //!< @todo Is it the right thing to move forward into MISSION without wifi?
        m_startWifiReadyTimeCentiseconds = Time__getTimeInCentiseconds();

        /**
         * @todo Currently there is no way to programmatically check for the wifi being ready, so we rely purely on the
         *       timeout and assume it's ready after that time elapses. Ideally we should add a way to do this
         *       programmatically.
         */

        // We're done transitioning. The spinOnce() for this substate will monitor for wifi being ready or for the
        // timeout elapsing.
        m_currentSubstate = SubState::WAITING_FOR_WIFI_READY_OR_TIMEOUT;
        return getState();
    }

    RoverState RoverStateEnteringMission::transitionToFinishUpSetup(RoverContext& theContext)
    {
        // These are simply setting/clearing bits, so they are instant
        powerOnHercules();
        releaseMotorsReset();
        releaseHerculesReset();

        return RoverState::MISSION;
    }

} // End namespace iris
