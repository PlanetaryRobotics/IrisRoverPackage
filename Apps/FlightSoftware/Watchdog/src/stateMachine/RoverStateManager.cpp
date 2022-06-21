#include "stateMachine/RoverStateManager.hpp"

#include "comms/debug_comms.h"
#include "event/event_queue.h"
#include "common.h"

#include <cassert>
#include <msp430.h>

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = WDT_VECTOR
__interrupt void WDT_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(WDT_VECTOR))) WDT_ISR (void)
#else
#error Compiler not supported!
#endif
{
    __no_operation();
}

namespace iris
{
    // Declare the buffers for the UART rx and tx ring buffers. These are static
    // so that they are not on the stack.
    static volatile uint8_t uart0TxBuffer[512] = {};
    static volatile uint8_t uart0RxBuffer[1024] = {};
    static volatile uint8_t uart1TxBuffer[1024] = {};
    static volatile uint8_t uart1RxBuffer[512] = {};

#pragma PERSISTENT
    static bool persistentInMission = false;
#pragma PERSISTENT
    static bool persistentDeployed = false;

    RoverStateManager::RoverStateManager(const char* resetReasonString)
        : m_stateEnteringKeepAlive(),
          m_stateEnteringMission(),
          m_stateEnteringService(),
          m_stateInit(RoverState::INIT, resetReasonString),
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
        DEBUG_ASSERT_EQUAL(EQ__STATUS__SUCCESS, eqStatus);

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

        m_context.m_details.m_hParams.m_kpHeater = DEFAULT_KP_HEATER;
        m_context.m_details.m_hParams.m_pwmLimit = DEFAULT_PWM_LIMIT;
        m_context.m_details.m_hParams.m_heaterSetpoint = DEFAULT_HEATER_SETPOINT;
        m_context.m_details.m_hParams.m_heaterWindow = DEFAULT_HEATER_WINDOW;
        m_context.m_details.m_hParams.m_heaterOnVal = DEFAULT_HEATER_ON_VAL;
        m_context.m_details.m_hParams.m_heaterOffVal = DEFAULT_HEATER_OFF_VAL;
        m_context.m_details.m_hParams.m_heatingControlEnabled = DEFAULT_HEATING_CONTROL_ENABLED;
        m_context.m_details.m_hParams.m_heaterDutyCyclePeriod = DEFAULT_HEATER_DUTY_CYCLE_PERIOD;
        m_context.m_details.m_hParams.m_heaterDutyCycle = DEFAULT_HEATER_DUTY_CYCLE;
        m_context.m_details.m_stateAsUint = static_cast<uint8_t>(RoverState::ENTERING_KEEP_ALIVE);
        m_context.m_details.m_inputPinAndStateBits = 0;
        m_context.m_details.m_outputPinBits = 0;
        m_context.m_details.m_resetActionBits = 0;

        m_context.m_persistentInMission = &persistentInMission;
        m_context.m_persistentDeployed = &persistentDeployed;

        m_context.m_isDeployed = false;
        m_context.m_i2cActive = false;
        m_context.m_sendDetailedReport = false;

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
            //
            // To support entering LPM1 (not entering a deeper LPM due to errata PMM31 and PMM31, see slaz681o
            // https://www.ti.com/lit/er/slaz681o/slaz681o.pdf , and also because we need SMCLK to remain on for our
            // UARTs to be able to receive and to use as our WDT clock) we need to set the WDT so that its interval is
            // longer than the longest time we know we'll go without an interrupt. We know that we get our timer tick
            // event every 7 (or so) seconds, so we need a WDT interval greater than that. When using ACLK as the
            // source of the WDT, our option jumps from ~3.5 seconds to ~55 seconds. However, if we use SMCLK as the
            // source of the WDT, since f_SMCLK = 8 MHz we can use the 2^27 divider option to get a WDT interval of
            // just over 16.7 seconds. The 2^27 divider is selected when WDTIS is 0b001.
            //OLD: WDTCTL = WDTPW + WDTCNTCL + WDTSSEL__ACLK + WDTIS2;
            WDTCTL = WDTPW + WDTCNTCL + WDTSSEL__SMCLK + WDTIS0;

            Event__Type event = EVENT__TYPE__UNUSED;
            EventQueue__Status eqStatus = EventQueue__get(&event);
            //bool gotEvent = false;

            if (EQ__STATUS__SUCCESS == eqStatus) {
                // We got an event. Have the current state handle it (performing any necessary state transitions as
                // requested by the state(s))
                handleEvent(event);
                //gotEvent = true;
            } else if (EQ__STATUS__ERROR_EMPTY == eqStatus) {
                if (m_currentState->canEnterLowPowerMode(m_context)) {
                    //!< @todo Enter LPM here. Need to figure out how to handle LPM and WDT together.
                    __enable_interrupt(); // Make sure we haven't somehow left interrupts off
                    ENTER_DEFAULT_LPM;
                }
            } else {
                // Any status other than success or empty is an unexpected failure.
                DEBUG_LOG_CHECK_STATUS(EQ__STATUS__SUCCESS, eqStatus, "Failed to get event from queue due to error");
            }

            if (m_context.m_i2cActive) {
                I2C_Sensors__spinOnce();
            }

            RoverState currentState = m_currentState->getState();
            m_context.m_details.m_stateAsUint = static_cast<uint8_t>(currentState);
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
                DebugComms__printfToLander("Reached default state in getStateObjectForStateEnum\n");
                DebugComms__flush();
                return &m_stateInit;
        }
    }

    void RoverStateManager::transitionUntilSettled(RoverState desiredState)
    {

        while (m_currentState->getState() != desiredState) {
            const char *originalStateStr = stateToString(m_currentState->getState());
            const char *desiredStateStr =  stateToString(desiredState);
            DebugComms__printfToLander("Transitioning from %s to %s\n", originalStateStr, desiredStateStr);
            m_currentState = getStateObjectForStateEnum(desiredState);
            m_context.m_details.m_stateAsUint = static_cast<uint8_t>(m_currentState->getState());
            desiredState = m_currentState->transitionTo(m_context);
        }

    }

    void RoverStateManager::handleEvent(Event__Type event)
    {
        RoverState currentState = m_currentState->getState();
        RoverState desiredNextState = currentState;

        switch (event) {
            case EVENT__TYPE__UNUSED:
                DebugComms__printfToLander("Trying to handle an UNUSED event type, which indicates programmer error\n");
                DebugComms__flush();
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
                DebugComms__printfToLander("In default case trying to handle event, which indicates programmer error\n");
                DebugComms__flush();
                break;
        }

        if (currentState != desiredNextState) {
            transitionUntilSettled(desiredNextState);
        }
    }
}
