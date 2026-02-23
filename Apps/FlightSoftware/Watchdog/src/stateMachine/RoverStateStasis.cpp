#include "stateMachine/RoverStateStasis.hpp"

#include "comms/debug_comms.h"
#include "drivers/adc.h"
#include "drivers/blimp.h"
#include "utils/time.h"

#include <cassert>

namespace iris
{
    RoverStateStasis::RoverStateStasis()
        : RoverStateEnteringStasis(RoverState::STASIS)
    {
    }

    bool RoverStateStasis::canEnterLowPowerMode(RoverContext& theContext)
    {
        // Handling lander data and timer ticks will both wake us up out of LPM, so it's ok for us to enter it.
        return !(theContext.m_i2cActive);
    }

    RoverState RoverStateStasis::handleHerculesData(RoverContext& /*theContext*/)
    {
        assert(!"Got hercules data event in StateStasis, which shouldn't be possible");
        return getState();
    }

    RoverState RoverStateStasis::handlePowerIssue(RoverContext& /*theContext*/)
    {
        //!< @todo Implement RoverStateStasis::handlePowerIssue
        return getState();
    }

    RoverState RoverStateStasis::spinOnce(RoverContext& theContext)
    {
        if (theContext.m_i2cActive) {
            I2C_Sensors__Action action = {};
            uint8_t readValue = 0;
            I2C_Sensors__Status i2cStatus = I2C_Sensors__getActionStatus(&action,
                                                                         &(theContext.m_i2cReadings),
                                                                         &readValue);

            if (I2C_SENSORS__STATUS__INCOMPLETE != i2cStatus) {
                DEBUG_LOG_CHECK_STATUS(I2C_SENSORS__STATUS__SUCCESS_DONE, i2cStatus, "I2C action failed");

                if (I2C_SENSORS__ACTIONS__WRITE_IO_EXPANDER == action) {
                    theContext.m_watchdogFlags &= ~WDFLAG_WAITING_FOR_IO_EXPANDER_WRITE;
                }

                if (I2C_SENSORS__ACTIONS__READ_IO_EXPANDER == action) {
                    bool chargeStat2 = (readValue & I2C_SENSORS__IOE_P1_BIT__CHARGE_STAT2 != 0);
                    bool latchStat = (readValue & I2C_SENSORS__IOE_P1_BIT__LATCH_STAT != 0);

                    if (chargeStat2) {
                        SET_IPASBI_IN_UINT(theContext.m_details.m_inputPinAndStateBits, IPASBI__CHARGE_STAT2);
                    } else {
                        CLEAR_IPASBI_IN_UINT(theContext.m_details.m_inputPinAndStateBits, IPASBI__CHARGE_STAT2);
                    }

                    if (latchStat) {
                        SET_IPASBI_IN_UINT(theContext.m_details.m_inputPinAndStateBits, IPASBI__LATCH_STAT);
                    } else {
                        CLEAR_IPASBI_IN_UINT(theContext.m_details.m_inputPinAndStateBits, IPASBI__LATCH_STAT);
                    }
                }

                I2C_Sensors__clearLastAction();
                theContext.m_i2cActive = false;
                initiateNextI2cAction(theContext);
            }
        }

        return getState();
    }

    RoverState RoverStateStasis::transitionTo(RoverContext& theContext)
    {
        // Nothing to do on this transition, which should always be from ENTERING_STASIS.
        return getState();
    }

    RoverState RoverStateStasis::handleRadioExitStasisCommand(RoverContext& theContext)
    {
        // This is our trigger to go back to mission
        return RoverState::ENTERING_MISSION;
    }

} // End namespace iris
