#ifndef __WATCHDOG_ROVER_STATE_HPP__
#define __WATCHDOG_ROVER_STATE_HPP__

namespace iris
{
    /**
     * @brief The possible states of the rover state machine.
     */
    enum class RoverState
    {
        INIT = 0, //!< The state that the state machine is in during initialization.
        ENTERING_KEEP_ALIVE = 1, //!< Performing all non-instant tasks to set up the appropriate system state for KEEP_ALIVE
        KEEP_ALIVE = 2, //!< In keep alive mode, used in transit to use little power but stay in communication
        ENTERING_SERVICE = 3, //!< Performing all non-instant tasks to set up the appropriate system state for SERVICE
        SERVICE = 4, //!< In diagnostic service mode, which is also a precondition of entering MISSION
        ENTERING_MISSION = 5, //!< Performing all non-instant tasks to set up the appropriate system state for MISSION.
        MISSION = 6, //!< In the fully-powered on mode to perform the mission.

        // Not implemented yet
        //ENTERING_FAULT = 7, //!< Performing all non-instant tasks to set up the appropriate system state for FAULT.
        //FAULT = 8 //!< Handle fault if possible, or halt
    };

}

#endif /* __WATCHDOG_ROVER_STATE_HPP__ */
