#include "stateMachine/RoverStateEnteringKeepAlive.hpp"

#include "drivers/adc.h"
#include "drivers/blimp.h"
#include "drivers/bsp.h"

#include "comms/debug_comms.h"
#include "comms/ground_msgs.h"
#include "watchdog.h"

#include <msp430.h>
#include <cassert>

namespace iris
{
    RoverStateEnteringKeepAlive::RoverStateEnteringKeepAlive()
        : RoverStateBase(RoverState::ENTERING_KEEP_ALIVE)
    {
    }

    RoverStateEnteringKeepAlive::RoverStateEnteringKeepAlive(RoverState overridingState)
        : RoverStateBase(overridingState)
    {
    }

    bool RoverStateEnteringKeepAlive::canEnterLowPowerMode(RoverContext & /*theContext*/)
    {
        // Don't allow entering low power mode while entering keep alive. The only non-instant thing this state
        // does is to wait for the previous ADC transaction to complete. Since we don't wake from LPM after an ADC
        // reading, we can't enter LPM while waiting for the transaction to complete.
        return false;
    }

    RoverState RoverStateEnteringKeepAlive::handleHerculesData(RoverContext & /*theContext*/)
    {
        /**
         * @todo Should we re-enter this state upon this occurring, in order to re-set the bit that should power off
         *       the Hercules?
         */
        DPRINTF("Got hercules data event in EnteringKeepAlive, which shouldn't be possible\n");
        assert(!"Got hercules data event in EnteringKeepAlive, which shouldn't be possible");
        return getState();
    }

    RoverState RoverStateEnteringKeepAlive::handleTimerTick(RoverContext &theContext)
    {
        theContext.m_keepAliveTickCount++;
        theContext.m_keepAliveTickCountForDetailedReport++;

        /* only send every 4 timer ticks (28s) */
        if (theContext.m_keepAliveTickCount >= 4)
        {
            theContext.m_keepAliveTickCount = 0;

            /* send heartbeat with collected data */
            static FlightEarthHeartbeat hb = {0};
            GroundMsgs__Status gcStatus =
                GroundMsgs__generateFlightEarthHeartbeat(&(theContext.m_i2cReadings),
                                                         &(theContext.m_adcValues),
                                                         &(theContext.m_details.m_hParams),
                                                         &hb);

            assert(GND_MSGS__STATUS__SUCCESS == gcStatus);

            LanderComms__Status lcStatus = txDownlinkData(theContext,
                                                          (uint8_t *)&hb,
                                                          sizeof(hb));

            assert(LANDER_COMMS__STATUS__SUCCESS == lcStatus);
            if (LANDER_COMMS__STATUS__SUCCESS != lcStatus)
            {
                //!< @todo Handling?
            }
        }

        /* only send every 70 timer ticks (490s) */
        // NOTE: This is just the base rate for ENTERING KeepAlive (if stuck here). Actual KeepAlive also sends its own detailed status packets.
        if (theContext.m_keepAliveTickCountForDetailedReport >= 70)
        {
            theContext.m_keepAliveTickCountForDetailedReport = 0;
            sendDetailedReportToLander(theContext);
        }

        // Check for UART errors to report
        size_t count = 0;
        BOOL changed = FALSE;
        UART__Status uStatus = UART__checkRxRbErrors(theContext.m_uart1State, &count, &changed);
        DEBUG_LOG_CHECK_STATUS(UART__STATUS__SUCCESS, uStatus, "Failed to get Lander UART Rx Rb Error count");

        if (changed)
        {
            DebugComms__tryPrintfToLanderNonblocking("New Lander UART Rx Rb failures, total count = %u\n", count);
        }

        if (theContext.m_details.m_hParams.persistent->m_heatingControlEnabled)
        {
            // Update the Heater State (PWM remains unchanged here):
            heaterControl(theContext);
        }

        // Queue up a read of the IO Expander, and initiate it if no other I2C action is active
        theContext.m_queuedI2cActions |= 1 << ((uint16_t)I2C_SENSORS__ACTIONS__READ_IO_EXPANDER);

        if (!theContext.m_i2cActive)
        {
            initiateNextI2cAction(theContext);
        }

        return getState();
    }

    RoverState RoverStateEnteringKeepAlive::handlePowerIssue(RoverContext & /*theContext*/)
    {
        //!< @todo Implement RoverStateEnteringKeepAlive::handlePowerIssue
        return getState();
    }

    RoverState RoverStateEnteringKeepAlive::spinOnce(RoverContext &theContext)
    {
        switch (m_currentSubstate)
        {
        case SubState::WAITING_FOR_IO_EXPANDER_WRITE:
        {
            I2C_Sensors__Action action = I2C_SENSORS__ACTIONS__INACTIVE;
            uint8_t readValue = 0;
            I2C_Sensors__Status i2cStatus = I2C_Sensors__getActionStatus(&action,
                                                                         &(theContext.m_i2cReadings),
                                                                         &readValue);

            // Sanity check
            assert(I2C_SENSORS__ACTIONS__WRITE_IO_EXPANDER == action);

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

                return transitionToFinishUpSetup(theContext);
            }
        }
        break;

        case SubState::FINISH_UP_SETUP:
            // Fall through
        default:
            // We should only ever spin in this state when the SubState is WAITING_FOR_IO_EXPANDER_WRITE. Really we
            // don't need the substates, but I've kept them because they are representative of the stages of this
            // state.
            assert(!"In spinOnce() not in WAITING_FOR_IO_EXPANDER_WRITE substate, which shouldn't be possible");
            break;
        }

        // Last ADC sample still not done, so remain in the current state.
        return getState();
    }

    RoverState RoverStateEnteringKeepAlive::transitionTo(RoverContext &theContext)
    {
        *(theContext.m_persistentInMission) = false;
        theContext.m_keepAliveTickCount = 0;
        theContext.m_keepAliveTickCountForDetailedReport = 0;
        return transitionToWaitingForIoExpanderWrite(theContext);
    }

    RoverState RoverStateEnteringKeepAlive::nextStateAfterSetupCompletes()
    {
        return RoverState::KEEP_ALIVE;
    }

    RoverState RoverStateEnteringKeepAlive::performResetCommand(RoverContext &theContext,
                                                                WdCmdMsgs__ResetSpecificId resetValue,
                                                                WdCmdMsgs__Response *response)
    {
        bool writeIOExpander = false;
        RoverStateBase::doConditionalResetSpecific(theContext,
                                                   resetValue,
                                                   response,
                                                   false, // whether or not to allow power on
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

    RoverState RoverStateEnteringKeepAlive::handleUplinkFromLander(RoverContext &theContext,
                                                                   uint8_t *rxDataBuffer,
                                                                   size_t rxDataLen)
    {
        // Ignore any uplink because the Hercules shouldn't be powered on.
        //!< @todo Should we somehow respond to the lander?
        return getState();
    }

    RoverState RoverStateEnteringKeepAlive::doGndCmdEnterKeepAliveMode(RoverContext &theContext,
                                                                       const WdCmdMsgs__Message &msg,
                                                                       WdCmdMsgs__Response &response,
                                                                       WdCmdMsgs__Response &deployNotificationResponse,
                                                                       bool &sendDeployNotificationResponse)
    {
        // We're already entering keep alive mode, but we can still re-transition into keep alive
        // once we receive this command.
        response.statusCode = WD_CMD_MSGS__RESPONSE_STATUS__SUCCESS;
        return transitionTo(theContext);
    }

    RoverState RoverStateEnteringKeepAlive::transitionToWaitingForIoExpanderWrite(RoverContext &theContext)
    {
        // Clear all queued I2C actions and stop any active one. This allows us to immediately perform the IO expander
        // write
        I2C_Sensors__stop();
        theContext.m_queuedI2cActions = 0;
        theContext.m_i2cActive = false;

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

        theContext.gotWifi = false; // reset

        // Turn off voltage rails. All of these are simply setting/clearing bits, so they are instant.
        disable3V3PowerRail();
        disableVSysAllPowerRail();
        blimp_normalBoot(); // Restore BLiMP state if returning to KA from a higher state. Shouldn't do anything if we're pushing straight through KA the first time.

        // Turn off Herc comms (used if returning to KA from a higher state):
        if (HerculesComms__isInitialized(theContext.m_hcState))
        {
            DebugComms__registerHerculesComms(NULL);
            HerculesComms__Status hcStatus = HerculesComms__uninitialize(&(theContext.m_hcState));
            DEBUG_ASSERT_EQUAL(HERCULES_COMMS__STATUS__SUCCESS, hcStatus);
        }

        // Make sure to disable the Hercules uart so we don't dump current through that tx pin
        UART__uninit0(&(theContext.m_uart0State));

        theContext.m_queuedI2cActions |= 1 << ((uint16_t)I2C_SENSORS__ACTIONS__WRITE_IO_EXPANDER);
        theContext.m_writeCustomIoExpanderValues = false;
        initiateNextI2cAction(theContext);

        m_currentSubstate = SubState::WAITING_FOR_IO_EXPANDER_WRITE;
        return getState();
    }

    RoverState RoverStateEnteringKeepAlive::transitionToFinishUpSetup(RoverContext &theContext)
    {
        // These are simply setting/clearing bits, so they are instant.
        enableHeater();

        // Enable all interrupts
        __enable_interrupt();

        DebugComms__tryPrintfToLanderNonblocking("Hello, Earth!\n");

        return nextStateAfterSetupCompletes();
    }

} // End namespace iris
