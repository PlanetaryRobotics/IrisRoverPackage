#include "stateMachine/RoverState.hpp"

namespace iris
{
    static const char * INIT_STR = "INIT";
    static const char * ENTERING_KEEP_ALIVE_STR = "ENTERING_KEEP_ALIVE";
    static const char * KEEP_ALIVE_STR = "KEEP_ALIVE";
    static const char * ENTERING_SERVICE_STR = "ENTERING_SERVICE";
    static const char * SERVICE_STR = "SERVICE";
    static const char * ENTERING_MISSION_STR = "ENTERING_MISSION";
    static const char * MISSION_STR = "MISSION";
    static const char * ENTERING_STASIS_STR = "ENTERING_STASIS";
    static const char * STASIS_STR = "STASIS";
    static const char * UNKNOWN_STR = "<UNKNOWN>";

    const char * stateToString(RoverState state)
    {
        switch (state) {
        case RoverState::INIT:
            return INIT_STR;
        case RoverState::ENTERING_KEEP_ALIVE:
            return ENTERING_KEEP_ALIVE_STR;
        case RoverState::KEEP_ALIVE:
            return KEEP_ALIVE_STR;
        case RoverState::ENTERING_SERVICE:
            return ENTERING_SERVICE_STR;
        case RoverState::SERVICE:
            return SERVICE_STR;
        case RoverState::ENTERING_MISSION:
            return ENTERING_MISSION_STR;
        case RoverState::MISSION:
            return MISSION_STR;
        case RoverState::ENTERING_STASIS:
            return ENTERING_STASIS_STR;
        case RoverState::STASIS:
            return STASIS_STR;
        default:
            return UNKNOWN_STR;

        }
    }
}
