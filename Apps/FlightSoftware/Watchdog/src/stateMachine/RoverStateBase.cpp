#include "stateMachine/RoverStateBase.hpp"

#include "drivers/adc.h"
#include "drivers/bsp.h"
#include "event/event.h"
#include "event/event_queue.h"

#include "ground_cmd.h"
#include "watchdog.h"

#include <cassert>

namespace iris
{
    struct CallbackUserArg
    {
            RoverStateBase& m_state;
            RoverContext& m_context;

            explicit CallbackUserArg(RoverStateBase& state, RoverContext& context)
                    : m_state(state), m_context(context)
            {
            }
    };

    RoverStateBase::RoverStateBase(RoverState state)
            : m_state(state)
    {
    }

    RoverState RoverStateBase::getState()
    {
        return m_state;
    }

    bool RoverStateBase::canEnterLowPowerMode(RoverContext& /*theContext*/)
    {
        return true;
    }

    void RoverStateBase::initiateNextI2cAction(RoverContext& theContext)
    {
        // A static value so that we remember the index of the last action we performed. By doing this, we can rotate
        // through all possibles actions to make sure that a single action repeated with a high frequency doesn't
        // prevent other actions from being triggered.
        static size_t i = static_cast<size_t>(I2C_SENSORS__ACTIONS__INACTIVE);

        // See what, if anything, is the next action to be performed
        for (size_t j = 0; j < static_cast<size_t>(I2C_SENSORS__ACTIONS__COUNT); ++j) {
            size_t actionIndex = i + j;

            // Wrap around if necessary (in theory only an if should be necessary, but a while loop seems safer)
            while (actionIndex >= static_cast<size_t>(I2C_SENSORS__ACTIONS__COUNT)) {
                actionIndex -= static_cast<size_t>(I2C_SENSORS__ACTIONS__COUNT);
            }

            uint16_t actionMask = 1 << actionIndex;

            // If the corresponding bit in m_queuedI2cActions is set, then we want to perform that action
            if ((theContext.m_queuedI2cActions & actionMask) != 0) {
                // First, initiate the action
                I2C_Sensors__Action actionEnum = static_cast<I2C_Sensors__Action>(actionIndex);

                I2C_Sensors__Status i2cStatus = I2C_SENSORS__STATUS__SUCCESS_DONE;

                switch (actionEnum) {
                    case I2C_SENSORS__ACTIONS__INACTIVE:
                        // This is always false in this case, but rechecking this condition makes the resulting output
                        // more descriptive.
                        assert(actionEnum != I2C_SENSORS__ACTIONS__INACTIVE);
                        break;

                    case I2C_SENSORS__ACTIONS__GAUGE_READING:
                        i2cStatus = I2C_Sensors__initiateGaugeReadings();
                        assert(i2cStatus == I2C_SENSORS__STATUS__SUCCESS_DONE);
                        break;

                    case I2C_SENSORS__ACTIONS__GAUGE_INIT:
                        i2cStatus = I2C_Sensors__initiateFuelGaugeInitialization();
                        assert(i2cStatus == I2C_SENSORS__STATUS__SUCCESS_DONE);
                        break;

                    case I2C_SENSORS__ACTIONS__WRITE_GAUGE_LOW_POWER:
                        i2cStatus = I2C_Sensors__initiateWriteLowPower();
                        assert(i2cStatus == I2C_SENSORS__STATUS__SUCCESS_DONE);
                        break;

                    case I2C_SENSORS__ACTIONS__READ_GAUGE_CONTROL_REGISTER:
                        i2cStatus = I2C_Sensors__initiateReadControl();
                        assert(i2cStatus == I2C_SENSORS__STATUS__SUCCESS_DONE);
                        break;

                    case I2C_SENSORS__ACTIONS__INIT_IO_EXPANDER:
                        i2cStatus = I2C_Sensors__initiateIoExpanderInitialization();
                        assert(i2cStatus == I2C_SENSORS__STATUS__SUCCESS_DONE);
                        break;

                    case I2C_SENSORS__ACTIONS__WRITE_IO_EXPANDER:
                        if (theContext.m_writeCustomIoExpanderValues) {
                            i2cStatus = I2C_Sensors__initiateWriteIoExpanderCurrentValues();
                        } else {
                            i2cStatus = I2C_Sensors__initiateWriteIoExpander(theContext.m_queuedIOWritePort0Value,
                                                                             theContext.m_queuedIOWritePort1Value);
                        }
                        assert(i2cStatus == I2C_SENSORS__STATUS__SUCCESS_DONE);
                        break;

                    case I2C_SENSORS__ACTIONS__READ_IO_EXPANDER:
                        i2cStatus = I2C_Sensors__initiateReadIoExpander();
                        assert(i2cStatus == I2C_SENSORS__STATUS__SUCCESS_DONE);
                        break;

                    default:
                        // This is always false in this case, but rechecking this condition makes the resulting output
                        // more descriptive.
                        assert(actionEnum >= I2C_SENSORS__ACTIONS__INACTIVE
                               && actionEnum < I2C_SENSORS__ACTIONS__COUNT);
                }

                // Then, clear the queued action bit so we don't repeat this action until it is requeued elsewhere
                theContext.m_queuedI2cActions &= ~actionMask;

                // Also indicate that I2C is active in our context
                theContext.m_i2cActive = true;

                // Lastly, update our static index-tracking variable
                i = actionIndex;

                break;
            }
        }
    }

    void RoverStateBase::heaterControl(RoverContext& theContext) {
        // voltage, where LSB = 0.0008056640625V
        unsigned short thermReading = theContext.m_adcValues.battTemp;
        bool tempLow = false;
        HeaterParams& hParams = theContext.m_persistantStatePtr->m_heaterParams;

        if (thermReading > hParams.m_heaterOnVal) {
            //   start heating when temperature drops below -5 C
            tempLow = true;
        } else if (thermReading < hParams.m_heaterOffVal) {
            //   stop heating when temperature reaches 0 C
            EventQueue__put(EVENT__TYPE__HIGH_TEMP);
        }

        uint32_t pwmCycle = 0;

        // P controller output
        // setpoint is slightly above desired temp because otherwise it will get stuck slightly below it
        if (tempLow) {
            pwmCycle = hParams.m_kpHeater * (thermReading - hParams.m_heaterSetpoint);
        }


        // cannot have duty cycle greater than clock
        if (pwmCycle > hParams.m_pwmLimit) {
            pwmCycle = hParams.m_pwmLimit;
        }

        TB0CCR2 = pwmCycle;
        hParams.m_heaterDutyCycle = pwmCycle;
    }

    RoverState RoverStateBase::handleLanderData(RoverContext& theContext)
    {
        return pumpMsgsFromLander(theContext);
    }

    RoverState RoverStateBase::handleHerculesData(RoverContext& theContext)
    {
        return pumpMsgsFromHercules(theContext);
    }

    void RoverStateBase::herculesMsgCallback(HercMsgs__Header* header,
                                             uint8_t* payloadBuffer,
                                             size_t payloadSize,
                                             void* userArg)
    {
        if (nullptr == userArg || nullptr == header || (payloadSize > 0 && NULL == payloadBuffer)) {
            return;
        }

        CallbackUserArg* args = reinterpret_cast<CallbackUserArg*>(userArg);

        switch (header->lowerOpCode) {
            case HERCULES_COMMS__MSG_OPCODE__STROKE:
                args->m_state.handleStrokeFromHercules(args->m_context, header);
                break;

            case HERCULES_COMMS__MSG_OPCODE__DOWNLINK:
                args->m_state.handleDownlinkFromHercules(args->m_context,
                                                         header,
                                                         payloadBuffer,
                                                         payloadSize);
                break;

            default: // We assume this is a reset command
                args->m_state.handleResetFromHercules(args->m_context, header);
                break;
        }
    }

    void RoverStateBase::landerMsgCallback(uint8_t* rxDataBuffer, size_t rxDataLen, void* userArg)
    {
        // Statically allocate the message structure so that it's not allocated on the stack
        static WdCmdMsgs__Message wdMessage = { 0 };
        static WdCmdMsgs__Response response = { 0 };
        static WdCmdMsgs__Response deployNotificationResponse = { 0 };

        assert(nullptr != userArg);

        CallbackUserArg* args = reinterpret_cast<CallbackUserArg*>(userArg);

        if (nullptr == rxDataBuffer) {
            //!< @todo reply to lander with this error
            return;
        }

        // First of all, parse the FSW common header. We assume it is part of a watchdog command message,
        // as the
        CmdMsgs__Status cmdStatus = CmdMsgs__deserializeHeader(rxDataBuffer, rxDataLen, &(wdMessage.commonHeader));

        assert(CMD_MSGS__STATUS__SUCCESS == cmdStatus);
        if (CMD_MSGS__STATUS__SUCCESS != cmdStatus) {
            // This should only really happen if rxDataLen is the wrong size
            //!< @todo reply to lander with this error
            return;
        }

        // We identify a ground command for WD by checking the "Type Magic" field in the FSW common header.
        // If the magic number in that field indicates the command is for Watchdog, then it is a ground
        // command for us.
        if (HEADER__TYPE_MAGIC_NUM__WATCHDOG_COMMAND == wdMessage.commonHeader.typeMagicNumber) {
            // This is a watchdog command. Parse it as such
            WdCmdMsgs__Status wdCmdStatus = WdCmdMsgs__deserializeMessage(rxDataBuffer,
                                                                          rxDataLen,
                                                                          &wdMessage,
                                                                          FALSE); // Don't reparse the header

            assert(WD_CMD_MSGS__STATUS__SUCCESS == wdCmdStatus);
            if (WD_CMD_MSGS__STATUS__SUCCESS != wdCmdStatus) {
                // This should only really happen if rxDataLen is the wrong size
                //!< @todo reply to lander with this error
                return;
            }

            bool sendDeployNotificationResponse = false;
            args->m_state.m_pumpMsgsFromLanderReturnState =
                    args->m_state.performWatchdogCommand(args->m_context,
                                                         wdMessage,
                                                         response,
                                                         deployNotificationResponse,
                                                         sendDeployNotificationResponse);

            if (sendDeployNotificationResponse) {
                args->m_state.sendLanderResponse(args->m_context, deployNotificationResponse);
            }

            args->m_state.sendLanderResponse(args->m_context, response);
        } else {
            // Anything with "Type Magic" field value that isn't for Watchdog is treated as uplink for Hercules
            args->m_state.m_pumpMsgsFromLanderReturnState = args->m_state.handleUplinkFromLander(args->m_context,
                                                                                                 rxDataBuffer,
                                                                                                 rxDataLen);
        }
    }

    RoverState RoverStateBase::performWatchdogCommand(RoverContext& theContext,
                                                      const WdCmdMsgs__Message& msg,
                                                      WdCmdMsgs__Response& response,
                                                      WdCmdMsgs__Response& deployNotificationResponse,
                                                      bool& sendDeployNotificationResponse)
    {
        // Make sure that by default we don't want to send the deploy notification response
        sendDeployNotificationResponse = false;

        // We always want to set the magic number and command ID (to which this is a response) on the response
        response.magicNumber = WD_CMD_MSGS__RESPONSE_MAGIC_NUMBER;
        response.commandId = msg.commandId;

        switch (msg.commandId) {
            case WD_CMD_MSGS__CMD_ID__RESET_SPECIFIC:
                return doGndCmdResetSpecific(theContext,
                                             msg,
                                             response,
                                             deployNotificationResponse,
                                             sendDeployNotificationResponse);

            case WD_CMD_MSGS__CMD_ID__PREP_FOR_DEPLOY:
                /* power on all systems */
                if (msg.body.prepForDeploy.confirmationMagicNumber != WD_CMD_MSGS__CONFIRM_DEPLOYMENT_MAGIC_NUMBER) {
                    /* magic bad */
                    response.statusCode = WD_CMD_MSGS__RESPONSE_STATUS__ERROR_BAD_COMMAND_PARAMETER;
                } else {
                    return doGndCmdPrepForDeploy(theContext,
                                                 msg,
                                                 response,
                                                 deployNotificationResponse,
                                                 sendDeployNotificationResponse);
                }
                break;

            case WD_CMD_MSGS__CMD_ID__DEPLOY:
                if (msg.body.deploy.confirmationMagicNumber != WD_CMD_MSGS__CONFIRM_DEPLOYMENT_MAGIC_NUMBER) {
                    /* magic bad */
                    response.statusCode = WD_CMD_MSGS__RESPONSE_STATUS__ERROR_BAD_COMMAND_PARAMETER;
                } else {
                    return doGndCmdDeploy(theContext,
                                          msg,
                                          response,
                                          deployNotificationResponse,
                                          sendDeployNotificationResponse);
                }
                break;

            case WD_CMD_MSGS__CMD_ID__SWITCH_CONN_MODE:
                return doGndCmdSwitchConnMode(theContext,
                                              msg,
                                              response,
                                              deployNotificationResponse,
                                              sendDeployNotificationResponse);

            case WD_CMD_MSGS__CMD_ID__SET_HEATER_KP:
                return doGndCmdSetHeaterKp(theContext,
                                           msg,
                                           response,
                                           deployNotificationResponse,
                                           sendDeployNotificationResponse);

            case WD_CMD_MSGS__CMD_ID__SET_AUTO_HEATER_ON_VALUE:
                return doGndCmdSetAutoHeaterOnValue(theContext,
                                                    msg,
                                                    response,
                                                    deployNotificationResponse,
                                                    sendDeployNotificationResponse);

            case WD_CMD_MSGS__CMD_ID__SET_AUTO_HEATER_OFF_VALUE:
                return doGndCmdSetAutoHeaterOffValue(theContext,
                                                     msg,
                                                     response,
                                                     deployNotificationResponse,
                                                     sendDeployNotificationResponse);

            case WD_CMD_MSGS__CMD_ID__SET_HEATER_DUTY_CYCLE_MAX:
                return doGndCmdSetHeaterDutyCycleMax(theContext,
                                                     msg,
                                                     response,
                                                     deployNotificationResponse,
                                                     sendDeployNotificationResponse);

            case WD_CMD_MSGS__CMD_ID__SET_HEATER_DUTY_CYCLE_PERIOD:
                return doGndCmdSetHeaterDutyCyclePeriod(theContext,
                                                        msg,
                                                        response,
                                                        deployNotificationResponse,
                                                        sendDeployNotificationResponse);

            case WD_CMD_MSGS__CMD_ID__SET_THERMISTER_V_SETPOINT:
                return doGndCmdSetThermisterVSetpoint(theContext,
                                                      msg,
                                                      response,
                                                      deployNotificationResponse,
                                                      sendDeployNotificationResponse);

            case WD_CMD_MSGS__CMD_ID__ENTER_SLEEP_MODE:
                /* Enter sleep mode */
                if (msg.body.enterSleepMode.confirmationMagicNumber != WD_CMD_MSGS__CONFIRM_MODE_CHANGE_MAGIC_NUMBER) {
                    /* magic bad */
                    response.statusCode = WD_CMD_MSGS__RESPONSE_STATUS__ERROR_BAD_COMMAND_PARAMETER;
                } else {
                    return doGndCmdEnterSleepMode(theContext,
                                                  msg,
                                                  response,
                                                  deployNotificationResponse,
                                                  sendDeployNotificationResponse);
                }
                break;

            case WD_CMD_MSGS__CMD_ID__ENTER_KEEPALIVE_MODE:
                /* Enter keepalive mode */
                if (msg.body.enterKeepAliveMode.confirmationMagicNumber
                        != WD_CMD_MSGS__CONFIRM_MODE_CHANGE_MAGIC_NUMBER) {
                    /* magic bad */
                    response.statusCode = WD_CMD_MSGS__RESPONSE_STATUS__ERROR_BAD_COMMAND_PARAMETER;
                } else {
                    return doGndCmdEnterKeepAliveMode(theContext,
                                                      msg,
                                                      response,
                                                      deployNotificationResponse,
                                                      sendDeployNotificationResponse);
                }
                break;

            case WD_CMD_MSGS__CMD_ID__ENTER_SERVICE_MODE:
                /* Enter service mode */
                if (msg.body.enterServiceMode.confirmationMagicNumber
                        != WD_CMD_MSGS__CONFIRM_MODE_CHANGE_MAGIC_NUMBER) {
                    /* magic bad */
                    response.statusCode = WD_CMD_MSGS__RESPONSE_STATUS__ERROR_BAD_COMMAND_PARAMETER;
                } else {
                    return doGndCmdEnterServiceMode(theContext,
                                                    msg,
                                                    response,
                                                    deployNotificationResponse,
                                                    sendDeployNotificationResponse);
                }
                break;

            default:
                response.statusCode = WD_CMD_MSGS__RESPONSE_STATUS__ERROR_BAD_COMMAND_ID;
                break;
        }

        return getState();
    }

    RoverState RoverStateBase::handleUplinkFromLander(RoverContext& theContext,
                                                      uint8_t* rxDataBuffer,
                                                      size_t rxDataLen)
    {
        // Anything with "Type Magic" field value that isn't for Watchdog is treated as uplink for Hercules
        HerculesComms__Status hcStatus = HerculesComms__txUplinkMsg(theContext.m_hcState,
                                                                    rxDataBuffer,
                                                                    rxDataLen);

        if (HERCULES_COMMS__STATUS__SUCCESS != hcStatus) {
            //!< @todo handling? Send response to lander maybe?
        }

        return getState();
    }

    RoverState RoverStateBase::handleStrokeFromHercules(RoverContext& theContext,
                                                        HercMsgs__Header* header)
    {
        static uint8_t telemetrySerializationBuffer[16] = { 0 };

        assert(nullptr != header);

        // For a stroke we just reply to the Hercules with our telemetry
        watchdog_build_hercules_telem(&(theContext.m_i2cReadings),
                                      &(theContext.m_adcValues),
                                      (theContext.m_isDeployed ? TRUE : FALSE),
                                      telemetrySerializationBuffer,
                                      sizeof(telemetrySerializationBuffer));

        HerculesComms__Status hcStatus = HerculesComms__txResponseMsg(theContext.m_hcState,
                                                                      header,
                                                                      telemetrySerializationBuffer,
                                                                      sizeof(telemetrySerializationBuffer));

        if (HERCULES_COMMS__STATUS__SUCCESS != hcStatus) {
            //!< @todo Handle error
        }

        return getState();
    }

    RoverState RoverStateBase::handleDownlinkFromHercules(RoverContext& theContext,
                                                          HercMsgs__Header* header,
                                                          uint8_t* payloadBuffer,
                                                          size_t payloadSize)
    {
        assert(nullptr != header);
        assert(nullptr != payloadBuffer);
        assert(0 != payloadSize);

        // For downlink we first send the data to the lander, then we reply to the Hercules

        // 1) Send data to lander
        LanderComms__Status lcStatus = LanderComms__txData(theContext.m_lcState, payloadBuffer, payloadSize);

        if (LANDER_COMMS__STATUS__SUCCESS != lcStatus) {
            //!< @todo Handle error

            // We still want to follow through. Even if we failed to tx the data we can still
            // try to reply to the Hercules
        }

        // 2) Reply to Hercules
        HerculesComms__Status hcStatus = HerculesComms__txResponseMsg(theContext.m_hcState,
                                                                      header,
                                                                      nullptr,
                                                                      0);

        if (HERCULES_COMMS__STATUS__SUCCESS != hcStatus) {
            //!< @todo Handle error
        }

        return getState();
    }

    RoverState RoverStateBase::handleResetFromHercules(RoverContext& theContext,
                                                       HercMsgs__Header* header)
    {
        assert(nullptr != header);

        // For Reset_Specific we want to do the reset, then reply to Hercules
        WdCmdMsgs__ResetSpecificId resetValue = (WdCmdMsgs__ResetSpecificId) header->resetValue;

        RoverState result = performResetCommand(theContext, resetValue, nullptr);

        HerculesComms__Status hcStatus = HerculesComms__txResponseMsg(theContext.m_hcState,
                                                                      header,
                                                                      nullptr,
                                                                      0);

        if (HERCULES_COMMS__STATUS__SUCCESS != hcStatus) {
            //!< @todo logging/handling?
        }

        return result;
    }

    RoverState RoverStateBase::pumpMsgsFromLander(RoverContext& theContext)
    {
        m_pumpMsgsFromLanderReturnState = getState();
        CallbackUserArg args(*this, theContext);

        LanderComms__Status lcStatus = LanderComms__tryGetMessage(theContext.m_lcState,
                                                                  RoverStateBase::landerMsgCallback,
                                                                  (void*) &args);

        assert(LANDER_COMMS__STATUS__SUCCESS == lcStatus);
        if (LANDER_COMMS__STATUS__SUCCESS != lcStatus) {
            //!< @todo logging?
        }

        return m_pumpMsgsFromLanderReturnState;
    }

    RoverState RoverStateBase::pumpMsgsFromHercules(RoverContext& theContext)
    {
        m_pumpMsgsFromHerculesReturnState = getState();
        CallbackUserArg args(*this, theContext);

        HerculesComms__Status hcStatus = HerculesComms__tryGetMessage(theContext.m_hcState,
                                                                      RoverStateBase::herculesMsgCallback,
                                                                      (void*) &args);

        //assert(HERCULES_COMMS__STATUS__SUCCESS == hcStatus);
        if (HERCULES_COMMS__STATUS__SUCCESS != hcStatus) {
            //!< @todo logging?
        }

        return m_pumpMsgsFromHerculesReturnState;
    }

    void RoverStateBase::sendLanderResponse(RoverContext& theContext, WdCmdMsgs__Response& response)
    {
        static uint8_t responseSerializationBuffer[WD_CMD_MSGS__PACKED_SIZE__RESPONSE_MSG] = { 0 };

        WdCmdMsgs__Status wdCmdStatus = WdCmdMsgs__serializeGroundResponse(&response,
                                                                           responseSerializationBuffer,
                                                                           sizeof(responseSerializationBuffer));

        assert(WD_CMD_MSGS__STATUS__SUCCESS == wdCmdStatus);

        LanderComms__Status lcStatus = LanderComms__txData(theContext.m_lcState,
                                                           responseSerializationBuffer,
                                                           sizeof(responseSerializationBuffer));

        if (LANDER_COMMS__STATUS__SUCCESS != lcStatus) {
            //!< @todo handling?
        }
    }

    // Specific watchdog command handling
    RoverState RoverStateBase::doGndCmdResetSpecific(RoverContext& theContext,
                                                     const WdCmdMsgs__Message& msg,
                                                     WdCmdMsgs__Response& response,
                                                     WdCmdMsgs__Response& deployNotificationResponse,
                                                     bool& sendDeployNotificationResponse)
    {
        return performResetCommand(theContext, msg.body.resetSpecific.resetId, &response);
    }

    RoverState RoverStateBase::doGndCmdPrepForDeploy(RoverContext& theContext,
                                                     const WdCmdMsgs__Message& msg,
                                                     WdCmdMsgs__Response& response,
                                                     WdCmdMsgs__Response& deployNotificationResponse,
                                                     bool& sendDeployNotificationResponse)
    {
        /* Default to not being in the right state to transition to mission mode */
        response.statusCode = WD_CMD_MSGS__RESPONSE_STATUS__ERROR_BAD_COMMAND_SEQUENCE;
        return getState();
    }

    RoverState RoverStateBase::doGndCmdDeploy(RoverContext& theContext,
                                              const WdCmdMsgs__Message& msg,
                                              WdCmdMsgs__Response& response,
                                              WdCmdMsgs__Response& deployNotificationResponse,
                                              bool& sendDeployNotificationResponse)
    {
        /* Default to not being in the right state to transition to deploy */
        response.statusCode = WD_CMD_MSGS__RESPONSE_STATUS__ERROR_BAD_COMMAND_SEQUENCE;
        return getState();
    }

    RoverState RoverStateBase::doGndCmdSwitchConnMode(RoverContext& theContext,
                                                      const WdCmdMsgs__Message& msg,
                                                      WdCmdMsgs__Response& response,
                                                      WdCmdMsgs__Response& deployNotificationResponse,
                                                      bool& sendDeployNotificationResponse)
    {
        //!< @todo IMPLEMENT!
        response.statusCode = WD_CMD_MSGS__RESPONSE_STATUS__SUCCESS;
        return getState();
    }

    RoverState RoverStateBase::doGndCmdSetHeaterKp(RoverContext& theContext,
                                                   const WdCmdMsgs__Message& msg,
                                                   WdCmdMsgs__Response& response,
                                                   WdCmdMsgs__Response& deployNotificationResponse,
                                                   bool& sendDeployNotificationResponse)
    {
        theContext.m_persistantStatePtr->m_heaterParams.m_kpHeater = msg.body.setHeaterKp.kp;
        response.statusCode = WD_CMD_MSGS__RESPONSE_STATUS__SUCCESS;
        return getState();
    }

    RoverState RoverStateBase::doGndCmdSetAutoHeaterOnValue(RoverContext& theContext,
                                                            const WdCmdMsgs__Message& msg,
                                                            WdCmdMsgs__Response& response,
                                                            WdCmdMsgs__Response& deployNotificationResponse,
                                                            bool& sendDeployNotificationResponse)
    {
        theContext.m_persistantStatePtr->m_heaterParams.m_heaterOnVal = msg.body.setAutoHeaterOnValue.heaterOnValue;
        response.statusCode = WD_CMD_MSGS__RESPONSE_STATUS__SUCCESS;
        return getState();
    }

    RoverState RoverStateBase::doGndCmdSetAutoHeaterOffValue(RoverContext& theContext,
                                                             const WdCmdMsgs__Message& msg,
                                                             WdCmdMsgs__Response& response,
                                                             WdCmdMsgs__Response& deployNotificationResponse,
                                                             bool& sendDeployNotificationResponse)
    {
        theContext.m_persistantStatePtr->m_heaterParams.m_heaterOffVal = msg.body.setAutoHeaterOffValue.heaterOffValue;
        response.statusCode = WD_CMD_MSGS__RESPONSE_STATUS__SUCCESS;
        return getState();
    }

    RoverState RoverStateBase::doGndCmdSetHeaterDutyCycleMax(RoverContext& theContext,
                                                             const WdCmdMsgs__Message& msg,
                                                             WdCmdMsgs__Response& response,
                                                             WdCmdMsgs__Response& deployNotificationResponse,
                                                             bool& sendDeployNotificationResponse)
    {
        theContext.m_persistantStatePtr->m_heaterParams.m_pwmLimit = msg.body.setHeaterDutyCycleMax.dutyCycleMax;
        response.statusCode = WD_CMD_MSGS__RESPONSE_STATUS__SUCCESS;
        return getState();
    }

    RoverState RoverStateBase::doGndCmdSetHeaterDutyCyclePeriod(RoverContext& theContext,
                                                                const WdCmdMsgs__Message& msg,
                                                                WdCmdMsgs__Response& response,
                                                                WdCmdMsgs__Response& deployNotificationResponse,
                                                                bool& sendDeployNotificationResponse)
    {
        TB0CCR0 = msg.body.setHeaterDutyCyclePeriod.dutyCyclePeriod;
        theContext.m_persistantStatePtr->m_heaterParams.m_heaterDutyCyclePeriod =
                msg.body.setHeaterDutyCyclePeriod.dutyCyclePeriod;
        response.statusCode = WD_CMD_MSGS__RESPONSE_STATUS__SUCCESS;
        return getState();
    }

    RoverState RoverStateBase::doGndCmdSetThermisterVSetpoint(RoverContext& theContext,
                                                              const WdCmdMsgs__Message& msg,
                                                              WdCmdMsgs__Response& response,
                                                              WdCmdMsgs__Response& deployNotificationResponse,
                                                              bool& sendDeployNotificationResponse)
    {
        theContext.m_persistantStatePtr->m_heaterParams.m_heaterSetpoint =
                msg.body.setThermisterVSetpoint.thermisterVSetpoint;
        response.statusCode = WD_CMD_MSGS__RESPONSE_STATUS__SUCCESS;
        return getState();
    }

    RoverState RoverStateBase::doGndCmdEnterSleepMode(RoverContext& theContext,
                                                      const WdCmdMsgs__Message& msg,
                                                      WdCmdMsgs__Response& response,
                                                      WdCmdMsgs__Response& deployNotificationResponse,
                                                      bool& sendDeployNotificationResponse)
    {
        // Sleep mode is deprecated so always return that we're in the wrong state to transition to sleep.
        response.statusCode = WD_CMD_MSGS__RESPONSE_STATUS__ERROR_BAD_COMMAND_SEQUENCE;
        return getState();
    }

    RoverState RoverStateBase::doGndCmdEnterKeepAliveMode(RoverContext& theContext,
                                                          const WdCmdMsgs__Message& msg,
                                                          WdCmdMsgs__Response& response,
                                                          WdCmdMsgs__Response& deployNotificationResponse,
                                                          bool& sendDeployNotificationResponse)
    {
        /* Default to not being in the right state to transition to keep alive mode */
        response.statusCode = WD_CMD_MSGS__RESPONSE_STATUS__ERROR_BAD_COMMAND_SEQUENCE;
        return getState();
    }

    RoverState RoverStateBase::doGndCmdEnterServiceMode(RoverContext& theContext,
                                                        const WdCmdMsgs__Message& msg,
                                                        WdCmdMsgs__Response& response,
                                                        WdCmdMsgs__Response& deployNotificationResponse,
                                                        bool& sendDeployNotificationResponse)
    {
        /* Default to not being in the right state to transition to service mode */
        response.statusCode = WD_CMD_MSGS__RESPONSE_STATUS__ERROR_BAD_COMMAND_SEQUENCE;
        return getState();
    }

    void RoverStateBase::doConditionalResetSpecific(RoverContext& theContext,
                                                    WdCmdMsgs__ResetSpecificId resetValue,
                                                    WdCmdMsgs__Response* response,
                                                    bool allowPowerOn,
                                                    bool allowDisableRs422,
                                                    bool allowDeploy,
                                                    bool allowUndeploy)
    {
        if (nullptr != response) {
            // Default to success, change if necessary
            response->statusCode = WD_CMD_MSGS__RESPONSE_STATUS__SUCCESS;
        }

        switch (resetValue) {
            case WD_CMD_MSGS__RESET_ID__NO_RESET:
                break;

            case WD_CMD_MSGS__RESET_ID__HERCULES_RESET:
                //!< @todo Should reset be allowed in KeepAlive? It's not technically powering on, but its similar.
                setHerculesReset();
                // queue up hercules unreset
                theContext.m_watchdogFlags |= WDFLAG_UNRESET_HERCULES;
                break;

            case WD_CMD_MSGS__RESET_ID__HERCULES_POWER_ON:
                if (allowPowerOn) {
                    powerOnHercules();
                } else if (nullptr != response) {
                    response->statusCode = WD_CMD_MSGS__RESPONSE_STATUS__ERROR_BAD_COMMAND_SEQUENCE;
                }
                break;

            case WD_CMD_MSGS__RESET_ID__HERCULES_POWER_OFF:
                powerOffHercules();
                break;

            case WD_CMD_MSGS__RESET_ID__RADIO_RESET:
                //!< @todo Should reset be allowed in KeepAlive? It's not technically powering on, but its similar.
                setRadioReset();
                // queue up an radio unreset
                theContext.m_watchdogFlags |= WDFLAG_UNRESET_RADIO1;
                break;

            case WD_CMD_MSGS__RESET_ID__RADIO_POWER_ON:
                if (allowPowerOn) {
                    powerOnRadio();
                } else if (nullptr != response) {
                    response->statusCode = WD_CMD_MSGS__RESPONSE_STATUS__ERROR_BAD_COMMAND_SEQUENCE;
                }
                break;

            case WD_CMD_MSGS__RESET_ID__RADIO_POWER_OFF:
                powerOffRadio();
                break;

            case WD_CMD_MSGS__RESET_ID__CAM_FPGA_RESET:
                //!< @todo Should reset be allowed in KeepAlive? It's not technically powering on, but its similar.
                setFPGAReset();
                // queue up the fpga unreset
                theContext.m_watchdogFlags |= WDFLAG_UNRESET_FPGA;
                break;

            case WD_CMD_MSGS__RESET_ID__CAM_FPGA_POWER_ON:
                if (allowPowerOn) {
                    powerOnFpga();
                } else if (nullptr != response) {
                    response->statusCode = WD_CMD_MSGS__RESPONSE_STATUS__ERROR_BAD_COMMAND_SEQUENCE;
                }
                break;

            case WD_CMD_MSGS__RESET_ID__CAM_FPGA_POWER_OFF:
                powerOffFpga();
                break;

            case WD_CMD_MSGS__RESET_ID__MOTOR_1_RESET:
                setMotor1Reset();
                // queue up the motor 1 unreset
                theContext.m_watchdogFlags |= WDFLAG_UNRESET_MOTOR1;
                break;

            case WD_CMD_MSGS__RESET_ID__MOTOR_2_RESET:
                setMotor2Reset();
                // queue up the motor 2 unreset
                theContext.m_watchdogFlags |= WDFLAG_UNRESET_MOTOR2;
                break;

            case WD_CMD_MSGS__RESET_ID__MOTOR_3_RESET:
                setMotor3Reset();
                // queue up the motor 3 unreset
                theContext.m_watchdogFlags |= WDFLAG_UNRESET_MOTOR3;
                break;

            case WD_CMD_MSGS__RESET_ID__MOTOR_4_RESET:
                setMotor4Reset();
                // queue up the motor 4 unreset
                theContext.m_watchdogFlags |= WDFLAG_UNRESET_MOTOR4;
                break;

            case WD_CMD_MSGS__RESET_ID__ALL_MOTORS_RESET:
                setMotorsReset();
                // queue up all the motor unresets
                theContext.m_watchdogFlags |= WDFLAG_UNRESET_MOTOR1 | WDFLAG_UNRESET_MOTOR2 |
                        WDFLAG_UNRESET_MOTOR3 | WDFLAG_UNRESET_MOTOR4;
                break;

            case WD_CMD_MSGS__RESET_ID__ALL_MOTORS_POWER_ON:
                if (allowPowerOn) {
                    powerOnMotors();
                } else if (nullptr != response) {
                    response->statusCode = WD_CMD_MSGS__RESPONSE_STATUS__ERROR_BAD_COMMAND_SEQUENCE;
                }
                break;

            case WD_CMD_MSGS__RESET_ID__ALL_MOTORS_POWER_OFF:
                powerOffMotors();
                break;

            case WD_CMD_MSGS__RESET_ID__3_3V_EN_RESET:
                if (allowPowerOn) {
                    disable3V3PowerRail();
                    // queue up 3V3 rail on again
                    theContext.m_watchdogFlags |= WDFLAG_UNRESET_3V3;
                } else if (nullptr != response) {
                    response->statusCode = WD_CMD_MSGS__RESPONSE_STATUS__ERROR_BAD_COMMAND_SEQUENCE;
                }
                break;

            case WD_CMD_MSGS__RESET_ID__3_3V_EN_POWER_ON:
                if (allowPowerOn) {
                    enable3V3PowerRail();
                } else if (nullptr != response) {
                    response->statusCode = WD_CMD_MSGS__RESPONSE_STATUS__ERROR_BAD_COMMAND_SEQUENCE;
                }
                break;

            case WD_CMD_MSGS__RESET_ID__3_3V_EN_POWER_OFF:
                disable3V3PowerRail();
                break;

            case WD_CMD_MSGS__RESET_ID__24V_EN_RESET:
                if (allowPowerOn) {
                    disable24VPowerRail();
                    // queue up 24V rail on again
                    theContext.m_watchdogFlags |= WDFLAG_UNRESET_24V;
                } else if (nullptr != response) {
                    response->statusCode = WD_CMD_MSGS__RESPONSE_STATUS__ERROR_BAD_COMMAND_SEQUENCE;
                }
                break;

            case WD_CMD_MSGS__RESET_ID__24V_EN_POWER_ON:
                if (allowPowerOn) {
                    enable24VPowerRail();
                } else if (nullptr != response) {
                    response->statusCode = WD_CMD_MSGS__RESPONSE_STATUS__ERROR_BAD_COMMAND_SEQUENCE;
                }
                break;

            case WD_CMD_MSGS__RESET_ID__24V_EN_POWER_OFF:
                disable24VPowerRail();
                break;

            case WD_CMD_MSGS__RESET_ID__HDRM_DEPLOY_SIGNAL_POWER_OFF:
                if (allowUndeploy) {
                    unsetDeploy();
                } else if (nullptr != response) {
                    response->statusCode = WD_CMD_MSGS__RESPONSE_STATUS__ERROR_BAD_COMMAND_SEQUENCE;
                }
                break;

            case WD_CMD_MSGS__RESET_ID__FPGA_CAM_0_SELECT:
                fpgaCameraSelectLo();
                break;

            case WD_CMD_MSGS__RESET_ID__FPGA_CAM_1_SELECT:
                fpgaCameraSelectHi();
                break;

            case WD_CMD_MSGS__RESET_ID__BATTERY_CHARGE_START:
                startChargingBatteries();
                break;

            case WD_CMD_MSGS__RESET_ID__BATTERY_CHARGE_STOP:
                stopChargingBatteries();
                break;

            case WD_CMD_MSGS__RESET_ID__RS422_UART_ENABLE:
                //!< @todo IMPLEMENT
                break;

            case WD_CMD_MSGS__RESET_ID__RS422_UART_DISABLE:
                if (allowDisableRs422) {
                    //!< @todo IMPLEMENT
                } else if (nullptr != response) {
                    response->statusCode = WD_CMD_MSGS__RESPONSE_STATUS__ERROR_BAD_COMMAND_SEQUENCE;
                }
                break;

            case WD_CMD_MSGS__RESET_ID__AUTO_HEATER_CONTROLLER_ENABLE:
                theContext.m_persistantStatePtr->m_heaterParams.m_heatingControlEnabled = true;
                break;

            case WD_CMD_MSGS__RESET_ID__AUTO_HEATER_CONTROLLER_DISABLE:
                theContext.m_persistantStatePtr->m_heaterParams.m_heatingControlEnabled = false;
                TB0CCR2 = 0;
                theContext.m_persistantStatePtr->m_heaterParams.m_heaterDutyCycle = 0;
                break;

            case WD_CMD_MSGS__RESET_ID__HERCULES_WATCHDOG_ENABLE:
                theContext.m_watchdogOpts |= WDFLAG_UNRESET_HERCULES;
                break;

            case WD_CMD_MSGS__RESET_ID__HERCULES_WATCHDOG_DISABLE:
                theContext.m_watchdogOpts &= ~WDFLAG_UNRESET_HERCULES;
                break;

            case WD_CMD_MSGS__RESET_ID__BATTERIES_ENABLE:
                if (allowPowerOn) {
                    disableBatteries();
                } else if (nullptr != response) {
                    response->statusCode = WD_CMD_MSGS__RESPONSE_STATUS__ERROR_BAD_COMMAND_SEQUENCE;
                }
                break;

            case WD_CMD_MSGS__RESET_ID__BATTERIES_DISABLE:
                enableBatteries();
                break;

            case WD_CMD_MSGS__RESET_ID__HDRM_DEPLOY_SIGNAL_POWER_ON:
                if (allowPowerOn && allowDeploy) {
                    /* WOOT WOOT! WE ARE ON TO THE MOON, FOLKS */
                    /* ref: https://docs.google.com/document/d/1dKLlBcIIVo8t1bGu3jNiHobGMavA3I2al0cncj3ZAhE/edit */
                    setDeploy();
                } else if (nullptr != response) {
                    response->statusCode = WD_CMD_MSGS__RESPONSE_STATUS__ERROR_BAD_COMMAND_SEQUENCE;
                }
                break;

            default:
                /* invalid command */
                if (nullptr != response) {
                    response->statusCode = WD_CMD_MSGS__RESPONSE_STATUS__ERROR_BAD_COMMAND_PARAMETER;
                    // If we can respond indicating there was a bad parameter, then return
                    // that this function finished "successfully," since we have a valid way
                    // of communicating something didn't work
                }
        }
    }

} // End namespace iris
