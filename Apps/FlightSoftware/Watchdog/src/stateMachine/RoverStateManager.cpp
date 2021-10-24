#include "stateMachine/RoverStateManager.hpp"

#include "event/event_queue.h"
#include "common.h"

#include <cassert>
#include <msp430.h>

namespace iris
{
    // Declare the buffers for the UART rx and tx ring buffers. These are static
    // so that they are not on the stack.
    static volatile uint8_t uart0TxBuffer[1024] = {};
    static volatile uint8_t uart0RxBuffer[1024] = {};
    static volatile uint8_t uart1TxBuffer[1024] = {};
    static volatile uint8_t uart1RxBuffer[1024] = {};

//#pragma PERSISTENT
    static PersistentState persistentState =
    {
         .m_statusBits = PSBI_MASK(PSBI__CHRG_EN),
         .m_heaterParams =
         {
              .m_kpHeater = DEFAULT_KP_HEATER,
              .m_pwmLimit = DEFAULT_PWM_LIMIT,
              .m_heaterSetpoint = DEFAULT_HEATER_SETPOINT,
              .m_heaterWindow = DEFAULT_HEATER_WINDOW,
              .m_heaterOnVal = DEFAULT_HEATER_ON_VAL,
              .m_heaterOffVal = DEFAULT_HEATER_OFF_VAL,
              .m_heating = DEFAULT_HEATING,
              .m_heatingControlEnabled = DEFAULT_HEATING_CONTROL_ENABLED,
              .m_heaterDutyCyclePeriod = DEFAULT_HEATER_DUTY_CYCLE_PERIOD,
              .m_heaterDutyCycle = DEFAULT_HEATER_DUTY_CYCLE
         },
         .m_stateAsUint = static_cast<uint8_t>(RoverState::ENTERING_KEEP_ALIVE)
    };

#pragma PERSISTENT
    static bool persistentInMission = false;
#pragma PERSISTENT
    static bool persistentDeployed = false;

    RoverStateManager::RoverStateManager()
        : m_stateEnteringKeepAlive(),
          m_stateEnteringMission(),
          m_stateEnteringService(),
          m_stateInit(RoverState::INIT),
          m_stateKeepAlive(),
          m_stateMission(),
          m_stateService(),
          m_currentState(&m_stateInit),
          m_context(),
          m_eventQueueBuffer()
    {
    }

    void RoverStateManager::init()
    {
        EventQueue__Status eqStatus = EventQueue__initialize(m_eventQueueBuffer.data(), m_eventQueueBuffer.size());

        // There should be no reason for initialization of the event queue to fail.
        assert(EQ__STATUS__SUCCESS == eqStatus);

        // Construct context, then transition to init state. Init state should handle initializing modules as
        // appropriate, eventually based on persistent memory of the module state.
        m_context.m_uartConfig.uart0Buffers.txBuffer = uart0TxBuffer;
        m_context.m_uartConfig.uart0Buffers.txBufferSize = sizeof(uart0TxBuffer);
        m_context.m_uartConfig.uart0Buffers.rxBuffer = uart0RxBuffer;
        m_context.m_uartConfig.uart0Buffers.rxBufferSize = sizeof(uart0RxBuffer);
        m_context.m_uartConfig.uart1Buffers.txBuffer = uart1TxBuffer;
        m_context.m_uartConfig.uart1Buffers.txBufferSize = sizeof(uart1TxBuffer);
        m_context.m_uartConfig.uart1Buffers.rxBuffer = uart1RxBuffer;
        m_context.m_uartConfig.uart1Buffers.rxBufferSize = sizeof(uart1RxBuffer);

        m_context.m_persistantStatePtr = &persistentState;
        m_context.m_persistentInMission = &persistentInMission;
        m_context.m_persistentDeployed = &persistentDeployed;

        m_context.m_isDeployed = false;
        m_context.m_i2cActive = false;

        RoverState desiredState = m_currentState->transitionTo(m_context);
        transitionUntilSettled(desiredState);
    }

    void RoverStateManager::spinForever()
    {
        while (true) {
            // Sets watchdog timer to be based on ACLK, and in our clock configuration the source of ACLK is VLOCLK
            // (with no divisions, so f_ACLK == f_VLOCLK) and f_VLOCLK == 9.4 KHz. Also clears the watchdog timer count
            // and configures the watchdog timer period (i.e. how long the WDT timer will tick before resetting the
            // MSP430 if the timer has not been cleared).
            //
            // When WDTIS is 0b100 the watchdog timer frequency is f_WDT / 2^15, so the watchdog timer period is
            //    p_WDT = 1 / (9400 / 2^15) = 3.486 seconds
            //
            // When WDTIS is 0b101 the watchdog timer frequency is f_WDT / 2^13, so the watchdog timer period is
            //    p_WDT = 1 / (9400 / 2^13) = 0.871 seconds
            //
            // Previously a WDTIS of 0b100 was being used so the watchdog period would have been about 3.5 seconds,
            // though the comment above it had mistakenly described it as setting the watchdog period to 1 second. For
            // now, we'll use a WDTIS of 0b101 (a period of 0.871 seconds) and see if it causes any issues.
            WDTCTL = WDTPW + WDTCNTCL + WDTSSEL__ACLK + WDTIS2 + WDTIS0;

            Event__Type event = EVENT__TYPE__UNUSED;
            EventQueue__Status eqStatus = EventQueue__get(&event);
            bool gotEvent = false;

            if (EQ__STATUS__SUCCESS == eqStatus) {
                // We got an event. Have the current state handle it (performing any necessary state transitions as
                // requested by the state(s))
                handleEvent(event);
                gotEvent = true;
            } else if (EQ__STATUS__ERROR_EMPTY == eqStatus) {
                if (m_currentState->canEnterLowPowerMode(m_context)) {
                    //!< @todo Enter LPM here. Need to figure out how to handle LPM and WDT together.
                }
            } else {
                // Any status other than success or empty is an unexpected failure.
                DEBUG_LOG_CHECK_STATUS(EQ__STATUS__SUCCESS, eqStatus, "Failed to get event from queue due to error");
            }

            if (m_context.m_i2cActive) {
                I2C_Sensors__spinOnce();
            }

            RoverState currentState = m_currentState->getState();
            RoverState desiredNextState = m_currentState->spinOnce(m_context);

            if (currentState != desiredNextState) {
                transitionUntilSettled(desiredNextState);
            }
        }
    }

    RoverStateBase* RoverStateManager::getStateObjectForStateEnum(RoverState stateEnumValue)
    {
        switch (stateEnumValue) {
            case RoverState::INIT:
                return &m_stateInit;

            case RoverState::ENTERING_KEEP_ALIVE:
                return &m_stateEnteringKeepAlive;

            case RoverState::KEEP_ALIVE:
                return &m_stateKeepAlive;

            case RoverState::ENTERING_SERVICE:
                return &m_stateEnteringService;

            case RoverState::SERVICE:
                return &m_stateService;

            case RoverState::ENTERING_MISSION:
                return &m_stateEnteringMission;

            case RoverState::MISSION:
                return &m_stateMission;

            default:
                assert(!"Reached default state in getStateObjectForStateEnum");
                return &m_stateInit;
        }
    }

    void RoverStateManager::transitionUntilSettled(RoverState desiredState)
    {
        while (m_currentState->getState() != desiredState) {
            m_currentState = getStateObjectForStateEnum(desiredState);
            desiredState = m_currentState->transitionTo(m_context);
        }
    }

    void RoverStateManager::handleEvent(Event__Type event)
    {
        RoverState currentState = m_currentState->getState();
        RoverState desiredNextState = currentState;

        switch (event) {
            case EVENT__TYPE__UNUSED:
                assert(!"Trying to handle an UNUSED event type, which indicates programmer error");
                return;

            case EVENT__TYPE__LANDER_DATA:
                desiredNextState = m_currentState->handleLanderData(m_context);
                break;

            case EVENT__TYPE__HERCULES_DATA:
                desiredNextState = m_currentState->handleHerculesData(m_context);
                break;

            case EVENT__TYPE__TIMER_TICK:
                desiredNextState = m_currentState->handleTimerTick(m_context);
                break;

            case EVENT__TYPE__HIGH_TEMP:
                desiredNextState = m_currentState->handleHighTemp(m_context);
                break;

            case EVENT__TYPE__POWER_ISSUE:
                desiredNextState = m_currentState->handlePowerIssue(m_context);
                break;

            default:
                assert(!"In default case trying to handle event, which indicates programmer error");
                break;
        }

        if (currentState != desiredNextState) {
            transitionUntilSettled(desiredNextState);
        }
    }
}
