#include "drivers/bsp.h"
#include "stateMachine/RoverStateInit.hpp"
#include "utils/time.h"
#include "watchdog.h"

#include <cassert>
#include <msp430.h>

namespace iris
{
    RoverStateInit::RoverStateInit(RoverState firstState)
            : RoverStateBase(RoverState::INIT),
              m_firstState(firstState)
    {
    }

    bool RoverStateInit::canEnterLowPowerMode(RoverContext& /*theContext*/)
    {
        return false;
    }


    RoverState RoverStateInit::handleLanderData(RoverContext& theContext)
    {
        assert(!"Handling data from lander event in INIT state, which shouldn't be possible");
        return getState();
    }

    RoverState RoverStateInit::handleHerculesData(RoverContext& /*theContext*/)
    {
        assert(!"Handling data from hercules event in INIT state, which shouldn't be possible");
        return getState();
    }

    RoverState RoverStateInit::handleTimerTick(RoverContext& theContext)
    {
        assert(!"Handling timer tick event in INIT state, which shouldn't be possible");
        return getState();
    }

    RoverState RoverStateInit::handleHighTemp(RoverContext& /*theContext*/)
    {
        assert(!"Handling high temp event in INIT state, which shouldn't be possible");
        return getState();
    }

    RoverState RoverStateInit::handlePowerIssue(RoverContext& /*theContext*/)
    {
        assert(!"Handling power issue event in INIT state, which shouldn't be possible");
        return getState();
    }

    RoverState RoverStateInit::spinOnce(RoverContext& theContext)
    {
        assert(!"In spinOnce() in INIT state, which shouldn't be possible");
        return getState();
    }

    RoverState RoverStateInit::transitionTo(RoverContext& theContext)
    {
        //!< @todo Replace this with initialization based on persistent status

        /* initialize the board */
        initializeGpios(theContext.m_persistantStatePtr);

        /* unlock changes to registers/ports, etc. */
        PM5CTL0 &= ~LOCKLPM5;

        clockInit();

        if (!(*(theContext.m_persistentDeployed))) {
            UART__Status uartStatus = UART__init1(&(theContext.m_uartConfig),
                                                  &(theContext.m_uart1State));
            DEBUG_LOG_CHECK_STATUS(UART__STATUS__SUCCESS, uartStatus, "Failed to init UART1");
            assert(UART__STATUS__SUCCESS == uartStatus);

            LanderComms__Status lcStatus = LanderComms__init(&(theContext.m_lcState), theContext.m_uart1State);
            assert(LANDER_COMMS__STATUS__SUCCESS == lcStatus);
        }

        /* set up watchdog */
        watchdog_init(&(theContext.m_watchdogFlags),
                      Time__getPointerToCentisecondCount(),
                      &(theContext.m_persistantStatePtr->m_heaterParams));

        /* set up the ADC */
        adc_init(&(theContext.m_watchdogFlags));

        /* set up i2c */
        I2C_Sensors__init();

        theContext.m_queuedI2cActions |= 1 << ((uint16_t) I2C_SENSORS__ACTIONS__INIT_IO_EXPANDER);
        theContext.m_writeCustomIoExpanderValues = false;
        initiateNextI2cAction(theContext);

        bool done = false;
        while (!done) {
            I2C_Sensors__spinOnce();

            I2C_Sensors__Action action = I2C_SENSORS__ACTIONS__INACTIVE;
            uint8_t readValue = 0;
            I2C_Sensors__Status i2cStatus = I2C_Sensors__getActionStatus(&action,
                                                                         &(theContext.m_i2cReadings),
                                                                         &readValue);

            // Sanity check
            assert(I2C_SENSORS__ACTIONS__INIT_IO_EXPANDER == action);

            if (I2C_SENSORS__STATUS__INCOMPLETE != i2cStatus) {
                DEBUG_LOG_CHECK_STATUS(I2C_SENSORS__STATUS__SUCCESS_DONE, i2cStatus, "I2C action failed");

                I2C_Sensors__clearLastAction();
                theContext.m_i2cActive = false;

                done = true;
            }
        }

        if (*(theContext.m_persistentInMission)) {
            // Enable all interrupts
            __enable_interrupt();
            return RoverState::ENTERING_MISSION;
        } else {
            return RoverState::ENTERING_KEEP_ALIVE;
        }
    }

    RoverState RoverStateInit::performResetCommand(RoverContext& theContext,
                                                                WdCmdMsgs__ResetSpecificId resetValue,
                                                                WdCmdMsgs__Response* response)
    {
        assert(!"Performing reset command in INIT state, which shouldn't be possible");
        return getState();
    }
} // End namespace iris
