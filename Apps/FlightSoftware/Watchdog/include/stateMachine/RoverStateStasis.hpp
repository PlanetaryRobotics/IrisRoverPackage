#ifndef __WATCHDOG_ROVER_STATE_STASIS_HPP__
#define __WATCHDOG_ROVER_STATE_STASIS_HPP__

#include "stateMachine/RoverStateEnteringStasis.hpp"

namespace iris
{
    class RoverStateStasis : public RoverStateEnteringStasis
    {
        public:
            explicit RoverStateStasis();

            bool canEnterLowPowerMode(RoverContext& theContext) override;

            // The functions to handle events
            RoverState handleHerculesData(RoverContext& theContext) override;
            RoverState handlePowerIssue(RoverContext& theContext) override;
            RoverState spinOnce(RoverContext& theContext) override;

            RoverState transitionTo(RoverContext& theContext) override;

        protected:
            RoverState handleRadioExitStasisCommand(RoverContext& theContext) override;
    };

}

#endif /* __WATCHDOG_ROVER_STATE_STASIS_HPP__ */
