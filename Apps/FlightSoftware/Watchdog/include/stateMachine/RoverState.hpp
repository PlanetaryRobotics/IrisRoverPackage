#ifndef __WATCHDOG_ROVER_STATE_HPP__
#define __WATCHDOG_ROVER_STATE_HPP__

namespace iris
{
    /**
     * @brief The possible states of the rover state machine.
     */
    enum class RoverState
    {
        UNKNOWN, //!< Only used during startup
        ENTERING_KEEP_ALIVE, //!< Performing all non-instant tasks to set up the appropriate system state for KEEP_ALIVE
        KEEP_ALIVE, //!< In keep alive mode, used in transit to use little power but stay in communication
        ENTERING_SERVICE, //!< Performing all non-instant tasks to set up the appropriate system state for SERVICE
        SERVICE, //!< In diagnostic service mode, which is also a precondition of entering MISSION
        ENTERING_MISSION, //!< Performing all non-instant tasks to set up the appropriate system state for MISSION.
        MISSION, //!< In the fully-powered on mode to perform the mission.
        ENTERING_FAULT, //!< Performing all non-instant tasks to set up the appropriate system state for FAULT.
        FAULT //!< Handle fault if possible, or halt
    };

}

#endif /* __WATCHDOG_ROVER_STATE_HPP__ */
