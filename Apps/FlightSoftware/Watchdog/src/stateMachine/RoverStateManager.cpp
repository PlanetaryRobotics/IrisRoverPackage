#include "stateMachine/RoverStateManager.hpp"

#include "event/event_queue.h"
#include "common.h"

    class RoverStateManager
    {
        public:
            explicit RoverStateManager();

            void init();

            void spinForever();

        private:
            // The states
            RoverStateEnteringKeepAlive m_stateEnteringKeepAlive;
            RoverStateEnteringMission m_stateEnteringMission;
            RoverStateEnteringService m_stateEnteringService;
            RoverStateInit m_stateInit;
            RoverStateKeepAlive m_stateKeepAlive;
            RoverStateMission m_stateMission;
            RoverStateService m_stateService;

            RoverStateBase* m_roverStateEnumToObjectMap[static_cast<size_t>(RoverState::NUM_STATES)];
            RoverStateBase* m_currentState;

            RoverContext m_context;
    };

namespace iris
{
    RoverStateManager::RoverStateManager()
        : m_stateEnteringKeepAlive(),
          m_stateEnteringMission(),
          m_stateEnteringService(),
          m_stateInit(RoverState::ENTERING_KEEP_ALIVE),
          m_stateKeepAlive(),
          m_stateMission(),
          m_stateService(),
          m_currentState(m_stateInit),
          m_context(),
          m_eventQueueBuffer({})
    {

    }

    void RoverStateManager::init()
    {
        EventQueue__Status eqStatus = EventQueue__initialize(m_eventQueueBuffer, sizeof(m_eventQueueBuffer));

        // There should be no reason for initialization of the event queue to fail.
        assert(EQ__STATUS__SUCCESS == eqStatus);


    }

    void RoverStateManager::spinForever()
    {
        while (true) {
            Event__Type event = EVENT__TYPE__UNUSED;
            EventQueue__Status eqStatus = EventQueue__get(&event);
            bool gotEvent = false;

            if (EQ__STATUS__SUCCESS == eqStatus) {
                // We got an event. Have the current state handle it (performing any necessary state transitions as
                // requested by the state(s))
                handleEvent(event);
                gotEvent = true;
            } else if (EQ__STATUS__ERROR_EMPTY == eqStatus && m_currentState.canEnterLowPowerMode()) {
                //!< @todo Enter LPM here.
                continue;
            } else {
                // Any status other than success or empty is an unexpected failure.
                DEBUG_LOG_CHECK_STATUS(EQ__STATUS__SUCCESS, eqStatus, "Failed to get event from queue due to error");
            }

            RoverState currentState = m_currentState.getState();
            RoverState desiredNextState = m_currentState.spinOnce(m_context);

            if (currentState != desiredNextState) {
                transitionUntilSettled(desiredNextState);
            }
        }
    }

    RoverStateBase& RoverStateManager::getStateObjectForStateEnum(RoverState stateEnumValue)
    {
        switch (stateEnumValue) {
            case RoverState::INIT:
                return m_stateInit;

            case RoverState::ENTERING_KEEP_ALIVE:
                return m_stateEnteringKeepAlive;

            case RoverState::KEEP_ALIVE:
                return m_stateKeepAlive;

            case RoverState::ENTERING_SERVICE:
                return m_stateEnteringService;

            case RoverState::SERVICE:
                return m_stateService;

            case RoverState::ENTERING_MISSION:
                return m_stateEnteringMission;

            case RoverState::MISSION:
                return m_stateMission;

            default:
                assert(!"Reached default state in getStateObjectForStateEnum");
                return m_stateInit;
        }
    }

    void RoverStateManager::transitionUntilSettled(RoverState desiredState)
    {
        while (m_currentState.getState() != desiredState) {
            m_currentState = getStateObjectForStateEnum(desiredState);
            desiredState = m_currentState.transitionTo(m_context);
        }
    }

    void RoverStateManager::handleEvent(Event__Type event)
    {
        RoverState currentState = m_currentState.getState();
        RoverState desiredNextState = currentState;

        switch (event) {
            case EVENT__TYPE__UNUSED:
                assert(!"Trying to handle an UNUSED event type, which indicates programmer error");
                return;

            case EVENT__TYPE__LANDER_DATA:
                desiredNextState = m_currentState.handleLanderData(m_context);
                break;

            case EVENT__TYPE__HERCULES_DATA:
                desiredNextState = m_currentState.handleHerculesData(m_context);
                break;

            case EVENT__TYPE__TIMER_TICK:
                desiredNextState = m_currentState.handleTimerTick(m_context);
                break;

            case EVENT__TYPE__I2C_STARTED:
                desiredNextState = m_currentState.handleI2cStarted(m_context);
                break;

            case EVENT__TYPE__I2C_DONE:
                desiredNextState = m_currentState.handleI2cDone(m_context);
                break;

            case EVENT__TYPE__HIGH_TEMP:
                desiredNextState = m_currentState.handleHighTemp(m_context);
                break;

            case EVENT__TYPE__POWER_ISSUE:
                desiredNextState = m_currentState.handlePowerIssue(m_context);
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
