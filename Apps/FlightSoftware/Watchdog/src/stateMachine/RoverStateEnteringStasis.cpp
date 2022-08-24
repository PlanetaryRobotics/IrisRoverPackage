#include "stateMachine/RoverStateEnteringStasis.hpp"

#include <cassert>

#include "drivers/bsp.h"

#include "comms/debug_comms.h"

namespace iris
{
    RoverStateEnteringStasis::RoverStateEnteringStasis()
            : RoverStateEnteringKeepAlive(RoverState::ENTERING_STASIS)
    {
    }

    RoverStateEnteringStasis::RoverStateEnteringStasis(RoverState overridingState)
            : RoverStateEnteringKeepAlive(overridingState)
    {
    }

    RoverState RoverStateEnteringStasis::handlePowerIssue(RoverContext& /*theContext*/)
    {
        //!< @todo Implement RoverStateEnteringStasis::handlePowerIssue
        return getState();
    }

    RoverState RoverStateEnteringStasis::handleHerculesData(RoverContext& theContext)
    {
        DPRINTF("Got hercules data event in EnteringStasis, which shouldn't be possible\n");
        assert(!"Got hercules data event in EnteringStasis, which shouldn't be possible");
        return getState();
    }

    RoverState RoverStateEnteringStasis::handleTimerTick(RoverContext& theContext)
    {
        // Do nothing
        return getState();
    }

    RoverState RoverStateEnteringStasis::spinOnce(RoverContext& theContext)
    {
        switch (m_currentSubstate) {
            case SubState::WAITING_FOR_IO_EXPANDER_WRITE:
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

                        if (I2C_SENSORS__ACTIONS__WRITE_IO_EXPANDER == action) {
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

    RoverState RoverStateEnteringStasis::performResetCommand(RoverContext& theContext,
                                                              WdCmdMsgs__ResetSpecificId resetValue,
                                                              WdCmdMsgs__Response* response)
    {
        bool writeIOExpander = false;
        RoverStateBase::doConditionalResetSpecific(theContext,
                                                   resetValue,
                                                   response,
                                                   true, // whether or not to allow power on
                                                   false, // whether or not to allow disabling RS422
                                                   false, // whether or not to allow deploy
                                                   false, // whether or not to allow undeploy
                                                   writeIOExpander);

        if (writeIOExpander) {
            theContext.m_queuedI2cActions |= 1 << ((uint16_t) I2C_SENSORS__ACTIONS__WRITE_IO_EXPANDER);
            theContext.m_writeCustomIoExpanderValues = false;
            theContext.m_watchdogFlags |= WDFLAG_WAITING_FOR_IO_EXPANDER_WRITE;

            if (!theContext.m_i2cActive) {
                initiateNextI2cAction(theContext);
            }
        }

        return getState();
    }

    RoverState RoverStateEnteringStasis::doGndCmdEnterKeepAliveMode(RoverContext& theContext,
                                                                     const WdCmdMsgs__Message& msg,
                                                                     WdCmdMsgs__Response& response,
                                                                     WdCmdMsgs__Response& deployNotificationResponse,
                                                                     bool& sendDeployNotificationResponse)
    {
        // Don't allow going to keep alive so return to the default implementation
        return RoverStateBase::doGndCmdEnterKeepAliveMode(theContext,
                                                          msg,
                                                          response,
                                                          deployNotificationResponse,
                                                          sendDeployNotificationResponse);
    }

    RoverState RoverStateEnteringStasis::doGndCmdEnterServiceMode(RoverContext& theContext,
                                                                   const WdCmdMsgs__Message& msg,
                                                                   WdCmdMsgs__Response& response,
                                                                   WdCmdMsgs__Response& deployNotificationResponse,
                                                                   bool& sendDeployNotificationResponse)
    {
        // Don't allow going to service so return to the default implementation
        return RoverStateBase::doGndCmdEnterServiceMode(theContext,
                                                          msg,
                                                          response,
                                                          deployNotificationResponse,
                                                          sendDeployNotificationResponse);
    }

    RoverState RoverStateEnteringStasis::transitionTo(RoverContext& theContext)
    {
        return transitionToWaitingForIoExpanderWrite(theContext);
    }

    RoverState RoverStateEnteringStasis::nextStateAfterSetupCompletes()
    {
        return RoverState::STASIS;
    }

    RoverState RoverStateEnteringStasis::handleUplinkFromLander(RoverContext& theContext,
                                                                   uint8_t* rxDataBuffer,
                                                                   size_t rxDataLen)
    {
        // Ignore any uplink because the Hercules shouldn't be powered on.
        //!< @todo Should we somehow respond to the lander?
        return getState();
    }

    RoverState RoverStateEnteringStasis::transitionToWaitingForIoExpanderWrite(RoverContext& theContext)
    {
        // Clear all queued I2C actions and stop any active one. This allows us to immediately perform the IO expander
        // write
        I2C_Sensors__stop();
        theContext.m_queuedI2cActions = 0;
        theContext.m_i2cActive = false;

        // Power everything off and set resets. All of these are simply setting/clearing bits, so they are instant.
        powerOffFpga();
        powerOffMotors();
        powerOffHercules();
        setFPGAReset();
        setMotorsReset();
        setHerculesReset();
        unsetDeploy();

        // Turn off voltage rails. All of these are simply setting/clearing bits, so they are instant.
        disable24VPowerRail();

        if (HerculesComms__isInitialized(theContext.m_hcState)) {
            DebugComms__registerHerculesComms(NULL);
            HerculesComms__Status hcStatus = HerculesComms__uninitialize(&(theContext.m_hcState));
            DEBUG_ASSERT_EQUAL(HERCULES_COMMS__STATUS__SUCCESS, hcStatus);
        }

        // Make sure to disable the Hercules uart so we don't dump current through that tx pin
        UART__uninit0(&(theContext.m_uart0State));

        theContext.m_queuedI2cActions |= 1 << ((uint16_t) I2C_SENSORS__ACTIONS__WRITE_IO_EXPANDER);
        theContext.m_writeCustomIoExpanderValues = false;
        initiateNextI2cAction(theContext);

        m_currentSubstate = SubState::WAITING_FOR_IO_EXPANDER_WRITE;
        return getState();
    }

    RoverState RoverStateEnteringStasis::transitionToFinishUpSetup(RoverContext& theContext)
    {
        // Enable all interrupts
        __enable_interrupt();

        DebugComms__printfToLander("Hello, Earth!\n");

        return nextStateAfterSetupCompletes();
    }


} // End namespace iris
