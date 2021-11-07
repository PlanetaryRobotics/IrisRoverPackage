#include "stateMachine/RoverStateKeepAlive.hpp"


#include "drivers/adc.h"
#include "utils/time.h"

#include <cassert>

namespace iris
{
    RoverStateKeepAlive::RoverStateKeepAlive()
        : RoverStateEnteringKeepAlive(RoverState::KEEP_ALIVE)
    {
    }

    bool RoverStateKeepAlive::canEnterLowPowerMode(RoverContext& theContext)
    {
        // Handling lander data and timer ticks will both wake us up out of LPM, so it's ok for us to enter it.
        return !(theContext.m_i2cActive);
    }

    RoverState RoverStateKeepAlive::handleHerculesData(RoverContext& /*theContext*/)
    {
        assert(!"Got hercules data event in KeepAlive, which shouldn't be possible");
        return getState();
    }

    RoverState RoverStateKeepAlive::handleTimerTick(RoverContext& theContext)
    {
        // Trigger a new ADC sample if the previous one is done
        if (isAdcSampleDone()) {
            adcCheckVoltageLevels(&(theContext.m_adcValues));
        }

        // Then do everything else we did in EnteringKeepAlive timer tick
        RoverStateEnteringKeepAlive::handleTimerTick(theContext);

        return getState();
    }

    RoverState RoverStateKeepAlive::handleHighTemp(RoverContext& /*theContext*/)
    {
        //!< @todo Implement RoverStateKeepAlive::handleHighTemp
        return getState();
    }

    RoverState RoverStateKeepAlive::handlePowerIssue(RoverContext& /*theContext*/)
    {
        //!< @todo Implement RoverStateKeepAlive::handlePowerIssue
        return getState();
    }

    RoverState RoverStateKeepAlive::spinOnce(RoverContext& theContext)
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

    RoverState RoverStateKeepAlive::transitionTo(RoverContext& theContext)
    {
        // Nothing to do on this transition, which should always be from ENTERING_KEEP_ALIVE.
        return getState();
    }

    RoverState RoverStateKeepAlive::doGndCmdEnterKeepAliveMode(RoverContext& theContext,
                                                               const WdCmdMsgs__Message& msg,
                                                               WdCmdMsgs__Response& response,
                                                               WdCmdMsgs__Response& deployNotificationResponse,
                                                               bool& sendDeployNotificationResponse)
    {
        // We're already in keep alive mode, but we can still re-transition into keep alive
        // once we receive this command.
        response.statusCode = WD_CMD_MSGS__RESPONSE_STATUS__SUCCESS;
        return RoverState::ENTERING_KEEP_ALIVE;
    }

    RoverState RoverStateKeepAlive::doGndCmdEnterServiceMode(RoverContext& theContext,
                                                             const WdCmdMsgs__Message& msg,
                                                             WdCmdMsgs__Response& response,
                                                             WdCmdMsgs__Response& deployNotificationResponse,
                                                             bool& sendDeployNotificationResponse)
    {
        /* Can transition directly to service mode from keepalive */
        response.statusCode = WD_CMD_MSGS__RESPONSE_STATUS__SUCCESS;
        return RoverState::ENTERING_SERVICE;
    }

} // End namespace iris
