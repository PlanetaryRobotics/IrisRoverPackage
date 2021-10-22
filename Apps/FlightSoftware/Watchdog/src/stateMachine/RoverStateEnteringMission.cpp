#include "stateMachine/RoverStateEnteringMission.hpp"

#include "comms/i2c_sensors.h"

#include "drivers/adc.h"
#include "drivers/bsp.h"

#include "utils/time.h"

#include "ground_cmd.h"
#include "watchdog.h"

#include <cassert>

namespace iris
{
    RoverStateEnteringMission::RoverStateEnteringMission()
            : RoverStateBase(RoverState::ENTERING_MISSION)
    {
    }

    bool RoverStateEnteringMission::canEnterLowPowerMode(RoverContext& /*theContext*/)
    {
        // Don't allow entering low power mode while entering mission. Waiting for ADC transaction to complete, waiting
        // for wifi to be ready, and the I2C transactions involved with initializing the fuel gauge are all things that
        // won't work if we enter LPM.
        return false;
    }

    RoverState RoverStateEnteringMission::handleTimerTick(RoverContext& theContext)
    {
        // NOTE: Don't try to sample the ADC as we may be setting it up for mission configuration

        /* send heartbeat with collected data */
        static FullEarthHeartbeat hb = { 0 };
        GroundCmd__Status gcStatus = GroundCmd__generateFullEarthHeartbeat(&(theContext.m_i2cReadings),
                                                                           &(theContext.m_adcValues),
                                                                           &(theContext.m_heaterParams),
                                                                           static_cast<uint8_t>(getState()),
                                                                           &hb);

        assert(GND_CMD__STATUS__SUCCESS == gcStatus);

        LanderComms__Status lcStatus = LanderComms__txData(theContext.m_lcState,
                                                           hb.heartbeatOutBuffer,
                                                           sizeof(hb.heartbeatOutBuffer));

        assert(LANDER_COMMS__STATUS__SUCCESS == lcStatus);
        if (LANDER_COMMS__STATUS__SUCCESS != lcStatus) {
            //!< @todo Handling?
        }

        if (theContext.m_heaterParams.m_heatingControlEnabled) {
            // calculate PWM duty cycle (if any) to apply to heater
            heaterControl(theContext);
        }

        BOOL writeIoExpander = FALSE;
        watchdog_monitor(theContext.m_hcState,
                         &(theContext.m_watchdogFlags),
                         &(theContext.m_watchdogOpts),
                         &writeIoExpander);

        if (writeIoExpander && true /** @todo Replace true with whether I2C has been initialized */) {
            theContext.m_queuedI2cActions |= 1 << ((uint16_t) I2C_SENSORS__ACTIONS__WRITE_IO_EXPANDER);
            theContext.m_queuedIOWritePort0Value = getIOExpanderPort0OutputValue();
            theContext.m_queuedIOWritePort1Value = getIOExpanderPort1OutputValue();

            if (!theContext.m_i2cActive) {
                initiateNextI2cAction(theContext);
            }
        }

        return getState();
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
            case SubState::WAITING_FOR_I2C_DONE:
                if (theContext.m_i2cActive) {
                    I2C_Sensors__Action action = I2C_SENSORS__ACTIONS__INACTIVE;
                    uint8_t readValue = 0;
                    I2C_Sensors__Status i2cStatus = I2C_Sensors__getActionStatus(&action,
                                                                                 &(theContext.m_i2cReadings),
                                                                                 &readValue);

                    if (I2C_SENSORS__STATUS__INCOMPLETE != i2cStatus) {
                        DEBUG_LOG_CHECK_STATUS(I2C_SENSORS__STATUS__SUCCESS_DONE, i2cStatus, "I2C action failed");

                        I2C_Sensors__clearLastAction();
                        theContext.m_i2cActive = false;
                        initiateNextI2cAction(theContext);

                        // If m_i2cActive is still false after calling initiateNextI2cAction, it means all queued i2c
                        // actions have been performed and there is no active i2c action
                        if (!(theContext.m_i2cActive)) {
                            return transitionToWaitingForIoExpanderWrite1(theContext);
                        }
                    }
                } else {
                    return transitionToWaitingForIoExpanderWrite1(theContext);
                }
                break;

            case SubState::WAITING_FOR_IO_EXPANDER_WRITE_1:
                {
                    I2C_Sensors__Action action = I2C_SENSORS__ACTIONS__INACTIVE;
                    uint8_t readValue = 0;
                    I2C_Sensors__Status i2cStatus = I2C_Sensors__getActionStatus(&action,
                                                                                 &(theContext.m_i2cReadings),
                                                                                 &readValue);

                    // Sanity check
                    assert(I2C_SENSORS__ACTIONS__WRITE_IO_EXPANDER == action);

                    if (I2C_SENSORS__STATUS__INCOMPLETE != i2cStatus) {
                        DEBUG_LOG_CHECK_STATUS(I2C_SENSORS__STATUS__SUCCESS_DONE, i2cStatus, "I2C action failed");

                        I2C_Sensors__clearLastAction();
                        theContext.m_i2cActive = false;

                        return transitionToWaitingForAdcDone(theContext);
                    }
                }
                break;

            case SubState::WAITING_FOR_ADC_DONE:
                if (isAdcSampleDone() == TRUE) {
                    return transitionToWaitingForIoExpanderWrite2(theContext);
                }
                break;

            case SubState::WAITING_FOR_IO_EXPANDER_WRITE_2:
                {
                    I2C_Sensors__Action action = I2C_SENSORS__ACTIONS__INACTIVE;
                    uint8_t readValue = 0;
                    I2C_Sensors__Status i2cStatus = I2C_Sensors__getActionStatus(&action,
                                                                                 &(theContext.m_i2cReadings),
                                                                                 &readValue);

                    // Sanity check
                    assert(I2C_SENSORS__ACTIONS__WRITE_IO_EXPANDER == action);

                    if (I2C_SENSORS__STATUS__INCOMPLETE != i2cStatus) {
                        DEBUG_LOG_CHECK_STATUS(I2C_SENSORS__STATUS__SUCCESS_DONE, i2cStatus, "I2C action failed");

                        I2C_Sensors__clearLastAction();
                        theContext.m_i2cActive = false;

                        return transitionToWaitingForFuelGaugeOrTimeout(theContext);
                    }
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

                        // First clear the last I2C_Sensors action so we can start a new one later
                        I2C_Sensors__clearLastAction();
                        theContext.m_i2cActive = false;

                        // Then move forward
                        return transitionToWatitingForWifiReadyOrTimeout(theContext);
                    } else if (I2C_SENSORS__STATUS__ERROR__DONE_WITH_NACKS == i2cStatus) {
                        // Gauge initialization failed, but we haven't timed out yet. Therefore we retry the
                        // initialization action.

                        // First clear the current (completed) action from the I2C_Sensors module so we can start a new
                        // one.
                        I2C_Sensors__clearLastAction();
                        theContext.m_i2cActive = false;

                        // Kick off the series of I2C transactions to initialize the fuel gauge.
                        theContext.m_queuedI2cActions |= 1 << ((uint16_t) I2C_SENSORS__ACTIONS__GAUGE_INIT);
                        initiateNextI2cAction(theContext);
                    } else {
                        DEBUG_LOG_CHECK_STATUS(I2C_SENSORS__STATUS__SUCCESS_DONE,
                                               i2cStatus,
                                               "Unexpected failure while getting I2C status");
                        // I guess retry entering mission?
                        //!< @todo What to do here? Enter FAULT?
                        return transitionToWaitingForI2cDone(theContext);
                    }

                    break;
                }

            case SubState::WAITING_FOR_WIFI_READY_OR_TIMEOUT:
                {
                    // First of all, if we've timed out we can simply move forward.
                    uint16_t timePassed = Time__getTimeInCentiseconds() - m_startWifiReadyTimeCentiseconds;

                    if (timePassed > WIFI_READY_TIMEOUT_CENTISECONDS) {
                        DPRINTF("Wait for wifi timed out\n");
                        return transitionToWaitingForFinalIoExpanderWrite(theContext);
                    }
                    break;
                }

            case SubState::WAITING_FOR_FINAL_IO_EXPANDER_WRITE:
                {
                    I2C_Sensors__Action action = I2C_SENSORS__ACTIONS__INACTIVE;
                    uint8_t readValue = 0;
                    I2C_Sensors__Status i2cStatus = I2C_Sensors__getActionStatus(&action,
                                                                                 &(theContext.m_i2cReadings),
                                                                                 &readValue);

                    // Sanity check
                    assert(I2C_SENSORS__ACTIONS__WRITE_IO_EXPANDER == action);

                    if (I2C_SENSORS__STATUS__INCOMPLETE != i2cStatus) {
                        DEBUG_LOG_CHECK_STATUS(I2C_SENSORS__STATUS__SUCCESS_DONE, i2cStatus, "I2C action failed");

                        I2C_Sensors__clearLastAction();
                        theContext.m_i2cActive = false;

                        return RoverState::MISSION;
                    }
                }
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
        return transitionToWaitingForI2cDone(theContext);
    }

    RoverState RoverStateEnteringMission::performResetCommand(RoverContext& theContext,
                                                              WdCmdMsgs__ResetSpecificId resetValue,
                                                              WdCmdMsgs__Response* response)
    {
        RoverStateBase::doConditionalResetSpecific(theContext,
                                                   resetValue,
                                                   response,
                                                   true, // whether or not to allow power on
                                                   false, // whether or not to allow disabling RS422
                                                   false, // whether or not to allow deploy
                                                   false); // whether or not to allow undeploy
        return getState();
    }

    RoverState RoverStateEnteringMission::transitionToWaitingForI2cDone(RoverContext& theContext)
    {
        if (theContext.m_i2cActive) {
            m_currentSubstate = SubState::WAITING_FOR_I2C_DONE;
            return getState();
        } else {
            return transitionToWaitingForIoExpanderWrite1(theContext);
        }
    }

    RoverState RoverStateEnteringMission::transitionToWaitingForIoExpanderWrite1(RoverContext& theContext)
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

        theContext.m_queuedI2cActions |= 1 << ((uint16_t) I2C_SENSORS__ACTIONS__WRITE_IO_EXPANDER);
        theContext.m_queuedIOWritePort0Value = getIOExpanderPort0OutputValue();
        theContext.m_queuedIOWritePort1Value = getIOExpanderPort1OutputValue();

        // I2C being inactive should be a prerequisite of entering this state
        assert(!(theContext.m_i2cActive));

        initiateNextI2cAction(theContext);

        m_currentSubstate = SubState::WAITING_FOR_IO_EXPANDER_WRITE_1;
        return getState();
    }

    RoverState RoverStateEnteringMission::transitionToWaitingForAdcDone(RoverContext& theContext)
    {
        // We want to set up the ADC for reading values when not attached to the lander. In order to do this setup, any
        // existing ADC reading must be done. If it's not done, we won't move forward until it is.
        bool lastSampleDone = (isAdcSampleDone() == TRUE);

        if (lastSampleDone) {
            // The last sample is done already, so advance immediately
            return transitionToWaitingForIoExpanderWrite2(theContext);
        } else {
            // We still need to wait for the last sample to complete, so remain in this state and this substate.
            m_currentSubstate = SubState::WAITING_FOR_ADC_DONE;
            return getState();
        }
    }

    RoverState RoverStateEnteringMission::transitionToWaitingForIoExpanderWrite2(RoverContext& theContext)
    {
        // The last ADC sample being done should be a prerequisite of entering this state, which means that
        // this call shouldn't fail. However, we check the return value anyway and will transition back to the previous
        // state if it did fail.
        bool setupSuccess = (setupAdcForMission() == TRUE);

        if (!setupSuccess) {
            return transitionToWaitingForAdcDone(theContext);
        }

        // Power stuff on. These are simply setting/clearing bits, so they are instant. However, powering on the radio
        // requires writing the I/O Expander.
        powerOnFpga();
        powerOnMotors();
        powerOnRadio();
        stopChargingBatteries();

        theContext.m_queuedI2cActions |= 1 << ((uint16_t) I2C_SENSORS__ACTIONS__WRITE_IO_EXPANDER);
        theContext.m_queuedIOWritePort0Value = getIOExpanderPort0OutputValue();
        theContext.m_queuedIOWritePort1Value = getIOExpanderPort1OutputValue();

        // I2C being inactive should be a prerequisite of entering this state
        assert(!(theContext.m_i2cActive));

        initiateNextI2cAction(theContext);

        m_currentSubstate = SubState::WAITING_FOR_IO_EXPANDER_WRITE_2;
        return getState();
    }

    RoverState RoverStateEnteringMission::transitionToWaitingForFuelGaugeOrTimeout(RoverContext& theContext)
    {
        // I2C being inactive should be a prerequisite of entering this state
        assert(!(theContext.m_i2cActive));

        // Record the start time for the time period in which we'll wait for the fuel gauge to be initialized
        // successfully. If it isn't done being initialized by the end of the timeout period, then we'll move forward
        // without the fuel gauge.
        m_startFuelGaugeInitTimeCentiseconds = Time__getTimeInCentiseconds();

        // Kick off the series of I2C transactions to initialize the fuel gauge.
        theContext.m_queuedI2cActions |= 1 << ((uint16_t) I2C_SENSORS__ACTIONS__GAUGE_INIT);
        initiateNextI2cAction(theContext);

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

    RoverState RoverStateEnteringMission::transitionToWaitingForFinalIoExpanderWrite(RoverContext& theContext)
    {
        // These are simply setting/clearing bits, so they are instant
        powerOnHercules();
        releaseMotorsReset();
        releaseHerculesReset();

        theContext.m_queuedI2cActions |= 1 << ((uint16_t) I2C_SENSORS__ACTIONS__WRITE_IO_EXPANDER);
        theContext.m_queuedIOWritePort0Value = getIOExpanderPort0OutputValue();
        theContext.m_queuedIOWritePort1Value = getIOExpanderPort1OutputValue();

        // I2C being inactive should be a prerequisite of entering this state
        assert(!(theContext.m_i2cActive));

        initiateNextI2cAction(theContext);

        m_currentSubstate = SubState::WAITING_FOR_FINAL_IO_EXPANDER_WRITE;
        return getState();
    }

} // End namespace iris
