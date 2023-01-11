#include "stateMachine/RoverStateEnteringMission.hpp"

#include "comms/debug_comms.h"
#include "comms/ground_msgs.h"
#include "comms/i2c_sensors.h"

#include "drivers/adc.h"
#include "drivers/bsp.h"
#include "drivers/blimp.h"

#include "utils/time.h"

#include "watchdog.h"

#include <cassert>

namespace iris
{

    RoverStateEnteringMission::RoverStateEnteringMission()
        : RoverStateBase(RoverState::ENTERING_MISSION)
    {
    }

    bool RoverStateEnteringMission::canEnterLowPowerMode(RoverContext & /*theContext*/)
    {
        // Don't allow entering low power mode while entering mission. Waiting for ADC transaction to complete, waiting
        // for wifi to be ready, and the I2C transactions involved with initializing the fuel gauge are all things that
        // won't work if we enter LPM.
        return false;
    }

    RoverState RoverStateEnteringMission::handleTimerTick(RoverContext &theContext)
    {
        // Check for UART errors to report
        size_t count = 0;
        BOOL changed = FALSE;
        UART__Status uStatus = UART__checkRxRbErrors(theContext.m_uart1State, &count, &changed);
        DEBUG_LOG_CHECK_STATUS(UART__STATUS__SUCCESS, uStatus, "Failed to get Lander UART Rx Rb Error count");

        if (changed)
        {
            DebugComms__tryPrintfToLanderNonblocking("New Lander UART Rx Rb failures, total count = %u\n", count);
        }

        /* send heartbeat with collected data */
        static FullEarthHeartbeat hb = {0};
        GroundMsgs__Status gcStatus = GroundMsgs__generateFullEarthHeartbeat(&(theContext.m_i2cReadings),
                                                                             &(theContext.m_adcValues),
                                                                             &(theContext.m_details.m_hParams),
                                                                             static_cast<uint8_t>(getState()),
                                                                             &hb);

        DEBUG_ASSERT_EQUAL(GND_MSGS__STATUS__SUCCESS, gcStatus);
        LanderComms__Status lcStatus = LanderComms__txDataUntilSendOrTimeout(theContext.m_lcState,
                                                                             (uint8_t *)&hb,
                                                                             sizeof(hb),
                                                                             200);

        DEBUG_ASSERT_EQUAL(LANDER_COMMS__STATUS__SUCCESS, lcStatus);
        if (LANDER_COMMS__STATUS__SUCCESS != lcStatus)
        {
            //!< @todo Handling?
        }

        if (theContext.m_details.m_hParams.m_heatingControlEnabled)
        {
            // Update the Heater State (PWM remains unchanged here):
            heaterControl(theContext);
        }

        BOOL writeIoExpander = FALSE;
        watchdog_monitor(theContext.m_hcState,
                         &(theContext.m_watchdogFlags),
                         &(theContext.m_watchdogOpts),
                         &writeIoExpander,
                         &(theContext.m_details));

#if 0 // Due to lots of race conditions w/ code handling transitions, I'm disabling reading/writing I2C here fo this state
        if (writeIoExpander && true /** @todo Replace true with whether I2C has been initialized */) {
            theContext.m_queuedI2cActions |= 1 << ((uint16_t) I2C_SENSORS__ACTIONS__WRITE_IO_EXPANDER);
            theContext.m_writeCustomIoExpanderValues = false;
            theContext.m_watchdogFlags |= WDFLAG_WAITING_FOR_IO_EXPANDER_WRITE;

            if (!theContext.m_i2cActive) {
                initiateNextI2cAction(theContext);
            }
        }

        // Queue up a read of the IO Expander, and initiate it if no other I2C action is active
        theContext.m_queuedI2cActions |= 1 << ((uint16_t) I2C_SENSORS__ACTIONS__READ_IO_EXPANDER);

        if (!theContext.m_i2cActive) {
            initiateNextI2cAction(theContext);
        }
#endif

        return getState();
    }

    RoverState RoverStateEnteringMission::handlePowerIssue(RoverContext & /*theContext*/)
    {
        //!< @todo Implement RoverStateEnteringMission::handlePowerIssue
        return getState();
    }

    RoverState RoverStateEnteringMission::spinOnce(RoverContext &theContext)
    {
        switch (m_currentSubstate)
        {
        case SubState::WAITING_FOR_I2C_DONE:
            if (theContext.m_i2cActive)
            {
                I2C_Sensors__Action action = I2C_SENSORS__ACTIONS__INACTIVE;
                uint8_t readValue = 0;
                I2C_Sensors__Status i2cStatus = I2C_Sensors__getActionStatus(&action,
                                                                             &(theContext.m_i2cReadings),
                                                                             &readValue);

                if (I2C_SENSORS__STATUS__INCOMPLETE != i2cStatus)
                {
                    DEBUG_LOG_CHECK_STATUS(I2C_SENSORS__STATUS__SUCCESS_DONE, i2cStatus, "I2C action failed");

                    I2C_Sensors__clearLastAction();
                    theContext.m_i2cActive = false;
                    initiateNextI2cAction(theContext);

                    // If m_i2cActive is still false after calling initiateNextI2cAction, it means all queued i2c
                    // actions have been performed and there is no active i2c action
                    if (!(theContext.m_i2cActive))
                    {
                        return transitionToWaitingForIoExpanderWrite1(theContext);
                    }
                }
            }
            else
            {
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
            DEBUG_ASSERT_EQUAL(I2C_SENSORS__ACTIONS__WRITE_IO_EXPANDER, action);

            if (I2C_SENSORS__STATUS__INCOMPLETE != i2cStatus)
            {
                DEBUG_LOG_CHECK_STATUS(I2C_SENSORS__STATUS__SUCCESS_DONE, i2cStatus, "I2C action failed");

                if (I2C_SENSORS__ACTIONS__WRITE_IO_EXPANDER == action)
                {
                    theContext.m_watchdogFlags &= ~WDFLAG_WAITING_FOR_IO_EXPANDER_WRITE;
                }

                I2C_Sensors__clearLastAction();
                theContext.m_i2cActive = false;

                return transitionToWaitingForIoExpanderWrite2(theContext);
            }
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
            DEBUG_ASSERT_EQUAL(I2C_SENSORS__ACTIONS__WRITE_IO_EXPANDER, action);

            if (I2C_SENSORS__STATUS__INCOMPLETE != i2cStatus)
            {
                DEBUG_LOG_CHECK_STATUS(I2C_SENSORS__STATUS__SUCCESS_DONE, i2cStatus, "I2C action failed");

                if (I2C_SENSORS__ACTIONS__WRITE_IO_EXPANDER == action)
                {
                    theContext.m_watchdogFlags &= ~WDFLAG_WAITING_FOR_IO_EXPANDER_WRITE;
                }

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
            // DPRINTF_ERR("%d\n", Time__getTimeInCentiseconds());
            if (timePassed > FUEL_GAUGE_INIT_TIMEOUT_CENTISECONDS)
            {
                DPRINTF_ERR("Setting up fuel gauge timed out\n");
                I2C_Sensors__stop();
                theContext.m_queuedI2cActions = 0;
                theContext.m_i2cActive = false;

                return transitionToWatitingForWifiReadyOrTimeout(theContext);
            }

            // Next, check if initializing the fuel gauge is complete (either due to success or failure).
            I2C_Sensors__Action currentAction = I2C_SENSORS__ACTIONS__INACTIVE;
            I2C_Sensors__Status i2cStatus = I2C_Sensors__getActionStatus(&currentAction, NULL, NULL);

            // First of all, we should always be doing the action to initialize the fuel gauges at this point
            DEBUG_ASSERT_EQUAL(I2C_SENSORS__ACTIONS__GAUGE_INIT, currentAction);

            if (I2C_SENSORS__STATUS__SUCCESS_DONE == i2cStatus)
            {
                // Gauge initialization completed successfully, so move forward

                // First clear the last I2C_Sensors action so we can start a new one later
                I2C_Sensors__clearLastAction();
                theContext.m_i2cActive = false;

                // Then move forward
                return transitionToWatitingForWifiReadyOrTimeout(theContext);
            }
            else if (I2C_SENSORS__STATUS__ERROR__DONE_WITH_NACKS == i2cStatus)
            {
                // Gauge initialization failed, but we haven't timed out yet. Therefore we retry the
                // initialization action.

                // First clear the current (completed) action from the I2C_Sensors module so we can start a new
                // one.
                I2C_Sensors__clearLastAction();
                theContext.m_i2cActive = false;

                // Kick off the series of I2C transactions to initialize the fuel gauge.
                theContext.m_queuedI2cActions |= 1 << ((uint16_t)I2C_SENSORS__ACTIONS__GAUGE_INIT);
                initiateNextI2cAction(theContext);
            }
            else if (I2C_SENSORS__STATUS__INCOMPLETE != i2cStatus)
            {
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
            bool transition = false;

            if (!m_sentWaitingForWifiMessage)
            {
                // Let Ground know why they'll not hear anything from us for a bit:
                DPRINTF("Awaiting Wifi for %d cs\n", WIFI_READY_TIMEOUT_CENTISECONDS);
                m_sentWaitingForWifiMessage = true;
            }

            if (timePassed > WIFI_READY_TIMEOUT_CENTISECONDS)
            {
                DPRINTF_ERR("Wait for Wifi timed out\n");
                transition = true;
            }

            if (theContext.gotWifi)
            {
                DPRINTF("Got Wifi\n");
                transition = true;
            }

            if (transition)
            {
                // At this point the only thing that may be using I2C would be the read/write from the timer tick,
                // and transitioning to mission has priority over that. If it was a write, the changes being written
                // should be picked up and written in IoExpanderWrite3. If it was a read, then all that we lose by
                // canceling it is a short delay in updating the input values on the IO expander
                I2C_Sensors__stop();
                theContext.m_queuedI2cActions = 0;
                theContext.m_i2cActive = false;
                return transitionToWaitingForIoExpanderWrite3(theContext);
            }
            break;
        }

        case SubState::WAITING_FOR_IO_EXPANDER_WRITE_3:
        {
            I2C_Sensors__Action action = I2C_SENSORS__ACTIONS__INACTIVE;
            uint8_t readValue = 0;
            I2C_Sensors__Status i2cStatus = I2C_Sensors__getActionStatus(&action,
                                                                         &(theContext.m_i2cReadings),
                                                                         &readValue);

            // Sanity check
            DEBUG_ASSERT_EQUAL(I2C_SENSORS__ACTIONS__WRITE_IO_EXPANDER, action);

            if (I2C_SENSORS__STATUS__INCOMPLETE != i2cStatus)
            {
                DEBUG_LOG_CHECK_STATUS(I2C_SENSORS__STATUS__SUCCESS_DONE, i2cStatus, "I2C action failed");

                if (I2C_SENSORS__ACTIONS__WRITE_IO_EXPANDER == action)
                {
                    theContext.m_watchdogFlags &= ~WDFLAG_WAITING_FOR_IO_EXPANDER_WRITE;
                }

                I2C_Sensors__clearLastAction();
                theContext.m_i2cActive = false;

                // Start the next I2C action if one is queued, if nothing queued this will return quickly
                initiateNextI2cAction(theContext);

                enableHerculesComms(theContext);

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

    RoverState RoverStateEnteringMission::transitionTo(RoverContext &theContext)
    {
        m_sentWaitingForWifiMessage = false; // reset flag
        return transitionToWaitingForI2cDone(theContext);
    }

    RoverState RoverStateEnteringMission::performResetCommand(RoverContext &theContext,
                                                              WdCmdMsgs__ResetSpecificId resetValue,
                                                              WdCmdMsgs__Response *response)
    {
        bool writeIOExpander = false;
        RoverStateBase::doConditionalResetSpecific(theContext,
                                                   resetValue,
                                                   response,
                                                   true,  // whether or not to allow power on
                                                   false, // whether or not to allow disabling RS422
                                                   false, // whether or not to allow deploy
                                                   false, // whether or not to allow undeploy
                                                   writeIOExpander);

        if (writeIOExpander)
        {
            theContext.m_queuedI2cActions |= 1 << ((uint16_t)I2C_SENSORS__ACTIONS__WRITE_IO_EXPANDER);
            theContext.m_writeCustomIoExpanderValues = false;
            theContext.m_watchdogFlags |= WDFLAG_WAITING_FOR_IO_EXPANDER_WRITE;

            if (!theContext.m_i2cActive)
            {
                initiateNextI2cAction(theContext);
            }
        }

        return getState();
    }

    RoverState RoverStateEnteringMission::transitionToWaitingForI2cDone(RoverContext &theContext)
    {
        if (theContext.m_i2cActive)
        {
            m_currentSubstate = SubState::WAITING_FOR_I2C_DONE;
            return getState();
        }
        else
        {
            return transitionToWaitingForIoExpanderWrite1(theContext);
        }
    }

    RoverState RoverStateEnteringMission::transitionToWaitingForIoExpanderWrite1(RoverContext &theContext)
    {
        /* bootup process - enable all rails */
        blimp_vSysAllEnOn(); // [CWC] new. desired behavior.
        enable3V3PowerRail();
        enableBatteries();
        disableHeater();
        unsetDeploy();

        /* enable hercules uart */
        //!< @todo Reset UART0 to a clean slate.
        /* power everything on and release resets */
        releaseRadioReset();
        releaseFPGAReset();

        theContext.m_queuedI2cActions |= 1 << ((uint16_t)I2C_SENSORS__ACTIONS__WRITE_IO_EXPANDER);
        theContext.m_writeCustomIoExpanderValues = false;

        // I2C being inactive should be a prerequisite of entering this state
        DEBUG_ASSERT(!(theContext.m_i2cActive));

        initiateNextI2cAction(theContext);

        m_currentSubstate = SubState::WAITING_FOR_IO_EXPANDER_WRITE_1;
        return getState();
    }

    RoverState RoverStateEnteringMission::transitionToWaitingForIoExpanderWrite2(RoverContext &theContext)
    {
        // Power stuff on. These are simply setting/clearing bits, so they are instant. However, powering on the radio
        // requires writing the I/O Expander.
        powerOnFpga();
        powerOffMotors();
        powerOnRadio();
        stopChargingBatteries();

        theContext.m_queuedI2cActions |= 1 << ((uint16_t)I2C_SENSORS__ACTIONS__WRITE_IO_EXPANDER);
        theContext.m_writeCustomIoExpanderValues = false;

        // I2C being inactive should be a prerequisite of entering this state
        DEBUG_ASSERT(!(theContext.m_i2cActive));

        initiateNextI2cAction(theContext);

        m_currentSubstate = SubState::WAITING_FOR_IO_EXPANDER_WRITE_2;
        return getState();
    }

    RoverState RoverStateEnteringMission::transitionToWaitingForFuelGaugeOrTimeout(RoverContext &theContext)
    {
        // I2C being inactive should be a prerequisite of entering this state
        DEBUG_ASSERT(!(theContext.m_i2cActive));

        // Record the start time for the time period in which we'll wait for the fuel gauge to be initialized
        // successfully. If it isn't done being initialized by the end of the timeout period, then we'll move forward
        // without the fuel gauge.
        m_startFuelGaugeInitTimeCentiseconds = Time__getTimeInCentiseconds();

        // Kick off the series of I2C transactions to initialize the fuel gauge.
        theContext.m_queuedI2cActions |= 1 << ((uint16_t)I2C_SENSORS__ACTIONS__GAUGE_INIT);
        initiateNextI2cAction(theContext);

        // We're done transitioning. The spinOnce() for this substate will monitor for completion of initialization
        // and either restart it (if it failed with NACK), or move forward to the next substate (if it succeeded). The
        // spinOnce() for this substate will also monitor for the timeout elapsing.
        m_currentSubstate = SubState::WAITING_FOR_FUEL_GAUGE_OR_TIMEOUT;
        return getState();
    }

    RoverState RoverStateEnteringMission::transitionToWatitingForWifiReadyOrTimeout(RoverContext &theContext)
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

    RoverState RoverStateEnteringMission::transitionToWaitingForIoExpanderWrite3(RoverContext &theContext)
    {
        // These are simply setting/clearing bits, so they are instant
        powerOnHercules();
        releaseMotorsReset();
        releaseHerculesReset();

        theContext.m_queuedI2cActions |= 1 << ((uint16_t)I2C_SENSORS__ACTIONS__WRITE_IO_EXPANDER);
        theContext.m_writeCustomIoExpanderValues = false;

        // I2C being inactive should be a prerequisite of entering this state
        DEBUG_ASSERT(!(theContext.m_i2cActive));

        initiateNextI2cAction(theContext);

        m_currentSubstate = SubState::WAITING_FOR_IO_EXPANDER_WRITE_3;
        return getState();
    }

} // End namespace iris
