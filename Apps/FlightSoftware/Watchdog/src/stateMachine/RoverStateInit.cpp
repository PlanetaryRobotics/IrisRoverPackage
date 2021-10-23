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
        initializeGpios(&(theContext.m_heaterParams));

        /* unlock changes to registers/ports, etc. */
        PM5CTL0 &= ~LOCKLPM5;

        clockInit();

        UART__Status uartStatus = UART__init1(&(theContext.m_uartConfig),
                                              &(theContext.m_uart1State));
        DEBUG_LOG_CHECK_STATUS(UART__STATUS__SUCCESS, uartStatus, "Failed to init UART1");
        assert(UART__STATUS__SUCCESS == uartStatus);

        UART__uninit0(&(theContext.m_uart0State));

        LanderComms__Status lcStatus = LanderComms__init(&(theContext.m_lcState), theContext.m_uart1State);
        assert(LANDER_COMMS__STATUS__SUCCESS == lcStatus);

        /* set up watchdog */
        watchdog_init(&(theContext.m_watchdogFlags), Time__getPointerToCentisecondCount());

        /* set up the ADC */
        adc_init(&(theContext.m_watchdogFlags));

        /* set up i2c to read from fuel gauge*/
        I2C_Sensors__init();

        return RoverState::ENTERING_KEEP_ALIVE;
    }

    RoverState RoverStateInit::performResetCommand(RoverContext& theContext,
                                                                WdCmdMsgs__ResetSpecificId resetValue,
                                                                WdCmdMsgs__Response* response)
    {
        assert(!"Performing reset command in INIT state, which shouldn't be possible");
        return getState();
    }
} // End namespace iris
