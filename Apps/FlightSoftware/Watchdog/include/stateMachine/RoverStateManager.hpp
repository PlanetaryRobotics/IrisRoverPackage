#ifndef __WATCHDOG_ROVER_STATE_MANAGER_HPP__
#define __WATCHDOG_ROVER_STATE_MANAGER_HPP__

#include "event/event.h"
#include "event/event_queue.h"
#include "stateMachine/RoverContext.hpp"
#include "stateMachine/RoverState.hpp"
#include "stateMachine/RoverStateEnteringKeepAlive.hpp"
#include "stateMachine/RoverStateEnteringMission.hpp"
#include "stateMachine/RoverStateEnteringService.hpp"
#include "stateMachine/RoverStateInit.hpp"
#include "stateMachine/RoverStateKeepAlive.hpp"
#include "stateMachine/RoverStateMission.hpp"
#include "stateMachine/RoverStateService.hpp"

#include <array>
#include <cstddef>

namespace iris
{

	class RoverStateManager
	{
		public:
	        RoverStateManager();

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

	        RoverStateBase& m_currentState;

	        RoverContext m_context;

	        std::array<volatile uint8_t, EVENT_QUEUE_MAX_SIZE> m_eventQueueBuffer;

	        RoverStateBase& getStateObjectForStateEnum(RoverState stateEnumValue);

	        void transitionUntilSettled(RoverState desiredState);

	        void handleEvent(Event__Type event);
	};

}

#endif /* __WATCHDOG_ROVER_STATE_MANAGER_HPP__ */
