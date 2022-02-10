#include "stateMachine/RoverStateBase.hpp"

#include "drivers/adc.h"
#include "drivers/bsp.h"
#include "drivers/blimp.h"
#include "event/event.h"
#include "event/event_queue.h"

#include "comms/ground_msgs.h"
#include "watchdog.h"

#include <cassert>
#include <msp430.h>

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

        // Exit early if we have no queued actions
        if (theContext.m_queuedI2cActions == 0) {
            return;
        }

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
                            i2cStatus = I2C_Sensors__initiateWriteIoExpander(theContext.m_queuedIOWritePort0Value,
                                                                             theContext.m_queuedIOWritePort1Value);
                        } else {
                            i2cStatus = I2C_Sensors__initiateWriteIoExpanderCurrentValues();
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
        HeaterParams& hParams = theContext.m_details.m_hParams;

        if (thermReading > hParams.m_heaterOnVal) {
            // Start heating when temperature drops low enough, which we detect via the ADC reading rising above a
            // configured (either via the default value or a value commanded from ground) ADC reading.
            enableHeater();
        } else if (thermReading < hParams.m_heaterOffVal) {
            // Start heating when temperature rises high enough, which we detect via the ADC reading falling below a
            // configured (either via the default value or a value commanded from ground) ADC reading.
            disableHeater();
        }
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

        args->m_context.m_watchdogFlags |= WDFLAG_HERCULES_KICK;

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

            case WD_CMD_MSGS__CMD_ID__SET_HEATER_DUTY_CYCLE:
                return doGndCmdSetHeaterDutyCycle(theContext,
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

            case WD_CMD_MSGS__CMD_ID__SET_THERMISTOR_V_SETPOINT:
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

            case WD_CMD_MSGS__CMD_ID__CLEAR_RESET_MEMORY:
                if ((msg.body.clearResetMem.magicOne != WD_CMD_MSGS__CONFIRM_CLR_RST_MEM_MAGIC_NUMBER_ONE) ||
                    (msg.body.clearResetMem.magicTwo != WD_CMD_MSGS__CONFIRM_CLR_RST_MEM_MAGIC_NUMBER_TWO)) {
                    /* magic bad */
                    response.statusCode = WD_CMD_MSGS__RESPONSE_STATUS__ERROR_BAD_COMMAND_PARAMETER;
                } else {
                    return doGndCmdClearResetMemory(theContext,
                                                    msg,
                                                    response,
                                                    deployNotificationResponse,
                                                    sendDeployNotificationResponse);
                }
                break;

            case WD_CMD_MSGS__CMD_ID__DANGEROUS_FORCE_BATT_STATE:
                if ((msg.body.dangForceBattState.confirmationMagicNumberOne
                            != WD_CMD_MSGS__CONFIRM_DANG_FORCE_BATT_STATE_MAGIC_NUMBER_ONE) ||
                    (msg.body.dangForceBattState.confirmationMagicNumberTwo
                            != WD_CMD_MSGS__CONFIRM_DANG_FORCE_BATT_STATE_MAGIC_NUMBER_TWO)) {
                    /* magic bad */
                    response.statusCode = WD_CMD_MSGS__RESPONSE_STATUS__ERROR_BAD_COMMAND_PARAMETER;
                } else {
                    return doGndCmdDangForceBattState(theContext,
                                                      msg,
                                                      response,
                                                      deployNotificationResponse,
                                                      sendDeployNotificationResponse);
                }
                break;

            case WD_CMD_MSGS__CMD_ID__REQUEST_DETAILED_REPORT:
                /* Enter service mode */
                if (msg.body.reqDetReport.magic != WD_CMD_MSGS__CONFIRM_REQ_DET_REPORT_MAGIC_NUMBER) {
                    /* magic bad */
                    response.statusCode = WD_CMD_MSGS__RESPONSE_STATUS__ERROR_BAD_COMMAND_PARAMETER;
                } else {
                    return doGndCmdRequestDetailedReport(theContext,
                                                         msg,
                                                         response,
                                                         deployNotificationResponse,
                                                         sendDeployNotificationResponse);
                }
                break;

            case WD_CMD_MSGS__CMD_ID__SET_CHARGE_EN_STATE:
                return doGndCmdSetChargeEnState(theContext,
                                                msg,
                                                response,
                                                deployNotificationResponse,
                                                sendDeployNotificationResponse);

            case WD_CMD_MSGS__CMD_ID__SET_CHARGE_REG_EN_STATE:
                return doGndCmdSetChargeRegEnState(theContext,
                                                   msg,
                                                   response,
                                                   deployNotificationResponse,
                                                   sendDeployNotificationResponse);

            case WD_CMD_MSGS__CMD_ID__SET_BATT_EN_STATE:
                return doGndCmdSetBattEnState(theContext,
                                              msg,
                                              response,
                                              deployNotificationResponse,
                                              sendDeployNotificationResponse);

            case WD_CMD_MSGS__CMD_ID__SET_BATT_CTRL_EN_STATE:
                return doGndCmdSetBattCtrlEnState(theContext,
                                                  msg,
                                                  response,
                                                  deployNotificationResponse,
                                                  sendDeployNotificationResponse);

            case WD_CMD_MSGS__CMD_ID__SET_LATCH_BATT_STATE:
                return doGndCmdSetLatchBattState(theContext,
                                                 msg,
                                                 response,
                                                 deployNotificationResponse,
                                                 sendDeployNotificationResponse);

            case WD_CMD_MSGS__CMD_ID__LATCH_SET_PULSE_LOW:
                return doGndCmdLatchSetPulseLow(theContext,
                                                msg,
                                                response,
                                                deployNotificationResponse,
                                                sendDeployNotificationResponse);

            case WD_CMD_MSGS__CMD_ID__LATCH_RESET_PULSE_LOW:
                return doGndCmdLatchResetPulseLow(theContext,
                                                  msg,
                                                  response,
                                                  deployNotificationResponse,
                                                  sendDeployNotificationResponse);

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
        theContext.m_details.m_hParams.m_kpHeater = msg.body.setHeaterKp.kp;
        response.statusCode = WD_CMD_MSGS__RESPONSE_STATUS__SUCCESS;
        return getState();
    }

    RoverState RoverStateBase::doGndCmdSetAutoHeaterOnValue(RoverContext& theContext,
                                                            const WdCmdMsgs__Message& msg,
                                                            WdCmdMsgs__Response& response,
                                                            WdCmdMsgs__Response& deployNotificationResponse,
                                                            bool& sendDeployNotificationResponse)
    {
        theContext.m_details.m_hParams.m_heaterOnVal = msg.body.setAutoHeaterOnValue.heaterOnValue;
        response.statusCode = WD_CMD_MSGS__RESPONSE_STATUS__SUCCESS;
        return getState();
    }

    RoverState RoverStateBase::doGndCmdSetAutoHeaterOffValue(RoverContext& theContext,
                                                             const WdCmdMsgs__Message& msg,
                                                             WdCmdMsgs__Response& response,
                                                             WdCmdMsgs__Response& deployNotificationResponse,
                                                             bool& sendDeployNotificationResponse)
    {
        theContext.m_details.m_hParams.m_heaterOffVal = msg.body.setAutoHeaterOffValue.heaterOffValue;
        response.statusCode = WD_CMD_MSGS__RESPONSE_STATUS__SUCCESS;
        return getState();
    }

    RoverState RoverStateBase::doGndCmdSetHeaterDutyCycle(RoverContext& theContext,
                                                             const WdCmdMsgs__Message& msg,
                                                             WdCmdMsgs__Response& response,
                                                             WdCmdMsgs__Response& deployNotificationResponse,
                                                             bool& sendDeployNotificationResponse)
    {
        const uint16_t& newDutyCycle = msg.body.setHeaterDutyCycle.dutyCycle;
        if (newDutyCycle <= theContext.m_details.m_hParams.m_heaterDutyCyclePeriod) {
            TB0CCR2 = newDutyCycle;
            theContext.m_details.m_hParams.m_heaterDutyCycle = newDutyCycle;
            response.statusCode = WD_CMD_MSGS__RESPONSE_STATUS__SUCCESS;
        } else {
            response.statusCode = WD_CMD_MSGS__RESPONSE_STATUS__ERROR_BAD_COMMAND_PARAMETER;
        }
        return getState();
    }

    RoverState RoverStateBase::doGndCmdSetHeaterDutyCyclePeriod(RoverContext& theContext,
                                                                const WdCmdMsgs__Message& msg,
                                                                WdCmdMsgs__Response& response,
                                                                WdCmdMsgs__Response& deployNotificationResponse,
                                                                bool& sendDeployNotificationResponse)
    {
        TB0CCR0 = msg.body.setHeaterDutyCyclePeriod.dutyCyclePeriod;
        theContext.m_details.m_hParams.m_heaterDutyCyclePeriod =
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
        theContext.m_details.m_hParams.m_heaterSetpoint =
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

    RoverState RoverStateBase::doGndCmdDangForceBattState(RoverContext& theContext,
                                                          const WdCmdMsgs__Message& msg,
                                                          WdCmdMsgs__Response& response,
                                                          WdCmdMsgs__Response& deployNotificationResponse,
                                                          bool& sendDeployNotificationResponse)
    {
        switch (msg.body.dangForceBattState.state) {
            case WD_CMD_MSGS__DANG_FORCE_BATT_STATE__LOW:
                blimp_bstat_dangerous_forceLow();
                response.statusCode = WD_CMD_MSGS__RESPONSE_STATUS__SUCCESS;
                break;

            case WD_CMD_MSGS__DANG_FORCE_BATT_STATE__HIGH:
                blimp_bstat_dangerous_forceHigh();
                response.statusCode = WD_CMD_MSGS__RESPONSE_STATUS__SUCCESS;
                break;

            case WD_CMD_MSGS__DANG_FORCE_BATT_STATE__RESTORE:
                blimp_bstat_safe_restoreInput();
                response.statusCode = WD_CMD_MSGS__RESPONSE_STATUS__SUCCESS;
                break;

            default:
                response.statusCode = WD_CMD_MSGS__RESPONSE_STATUS__ERROR_BAD_COMMAND_PARAMETER;
        }

        return getState();
    }

    RoverState RoverStateBase::doGndCmdSetChargeEnState(RoverContext& theContext,
                                                        const WdCmdMsgs__Message& msg,
                                                        WdCmdMsgs__Response& response,
                                                        WdCmdMsgs__Response& deployNotificationResponse,
                                                        bool& sendDeployNotificationResponse)
    {
        switch (msg.body.setChargeEnState.selection) {
            case WD_CMD_MSGS__CHARGE_EN__ON:
                blimp_chargerEnOn();
                response.statusCode = WD_CMD_MSGS__RESPONSE_STATUS__SUCCESS;
                break;

            case WD_CMD_MSGS__CHARGE_EN__OFF:
                blimp_chargerEnOff();
                response.statusCode = WD_CMD_MSGS__RESPONSE_STATUS__SUCCESS;
                break;

            case WD_CMD_MSGS__CHARGE_EN__FORCE_HIGH:
                blimp_chargerEnForceHigh();
                response.statusCode = WD_CMD_MSGS__RESPONSE_STATUS__SUCCESS;
                break;

            default:
                response.statusCode = WD_CMD_MSGS__RESPONSE_STATUS__ERROR_BAD_COMMAND_PARAMETER;
        }

        return getState();
    }

    RoverState RoverStateBase::doGndCmdSetChargeRegEnState(RoverContext& theContext,
                                                           const WdCmdMsgs__Message& msg,
                                                           WdCmdMsgs__Response& response,
                                                           WdCmdMsgs__Response& deployNotificationResponse,
                                                           bool& sendDeployNotificationResponse)
    {
        switch (msg.body.setChargeRegEnState.selection) {
            case WD_CMD_MSGS__CHARGE_REG_EN__ON:
                blimp_regEnOn();
                response.statusCode = WD_CMD_MSGS__RESPONSE_STATUS__SUCCESS;
                break;

            case WD_CMD_MSGS__CHARGE_REG_EN__OFF:
                blimp_regEnOff();
                response.statusCode = WD_CMD_MSGS__RESPONSE_STATUS__SUCCESS;
                break;

            default:
                response.statusCode = WD_CMD_MSGS__RESPONSE_STATUS__ERROR_BAD_COMMAND_PARAMETER;
        }

        return getState();
    }

    RoverState RoverStateBase::doGndCmdSetBattEnState(RoverContext& theContext,
                                                      const WdCmdMsgs__Message& msg,
                                                      WdCmdMsgs__Response& response,
                                                      WdCmdMsgs__Response& deployNotificationResponse,
                                                      bool& sendDeployNotificationResponse)
    {
        switch (msg.body.setBattEnState.selection) {
            case WD_CMD_MSGS__BATT_EN__ON:
                blimp_battEnOn();
                response.statusCode = WD_CMD_MSGS__RESPONSE_STATUS__SUCCESS;
                break;

            case WD_CMD_MSGS__BATT_EN__OFF:
                blimp_battEnOff();
                response.statusCode = WD_CMD_MSGS__RESPONSE_STATUS__SUCCESS;
                break;

            default:
                response.statusCode = WD_CMD_MSGS__RESPONSE_STATUS__ERROR_BAD_COMMAND_PARAMETER;
        }

        return getState();
    }

    RoverState RoverStateBase::doGndCmdSetBattCtrlEnState(RoverContext& theContext,
                                                          const WdCmdMsgs__Message& msg,
                                                          WdCmdMsgs__Response& response,
                                                          WdCmdMsgs__Response& deployNotificationResponse,
                                                          bool& sendDeployNotificationResponse)
    {
        response.statusCode = WD_CMD_MSGS__RESPONSE_STATUS__ERROR_BAD_COMMAND_SEQUENCE;
        return getState();
    }

    RoverState RoverStateBase::doGndCmdSetLatchBattState(RoverContext& theContext,
                                                         const WdCmdMsgs__Message& msg,
                                                         WdCmdMsgs__Response& response,
                                                         WdCmdMsgs__Response& deployNotificationResponse,
                                                         bool& sendDeployNotificationResponse)
    {
        switch (msg.body.setLatchBattState.selection) {
            case WD_CMD_MSGS__LATCH_BATT__ON:
                blimp_latchBattOn();
                response.statusCode = WD_CMD_MSGS__RESPONSE_STATUS__SUCCESS;
                break;

            case WD_CMD_MSGS__LATCH_BATT__OFF:
                blimp_latchBattOff();
                response.statusCode = WD_CMD_MSGS__RESPONSE_STATUS__SUCCESS;
                break;

            case WD_CMD_MSGS__LATCH_BATT__UPDATE:
                blimp_latchBattUpdate();
                response.statusCode = WD_CMD_MSGS__RESPONSE_STATUS__SUCCESS;
                break;

            default:
                response.statusCode = WD_CMD_MSGS__RESPONSE_STATUS__ERROR_BAD_COMMAND_PARAMETER;
        }

        return getState();
    }

    RoverState RoverStateBase::doGndCmdLatchSetPulseLow(RoverContext& theContext,
                                                        const WdCmdMsgs__Message& msg,
                                                        WdCmdMsgs__Response& response,
                                                        WdCmdMsgs__Response& deployNotificationResponse,
                                                        bool& sendDeployNotificationResponse)
    {
        switch (msg.body.latchSetPulseLow.selection) {
            case WD_CMD_MSGS__LATCH_SET_RESET__OFF:
                blimp_latchSetOff();
                response.statusCode = WD_CMD_MSGS__RESPONSE_STATUS__SUCCESS;
                break;

            case WD_CMD_MSGS__LATCH_SET_RESET__PULSE:
                blimp_latchSetPulseLow();
                response.statusCode = WD_CMD_MSGS__RESPONSE_STATUS__SUCCESS;
                break;

            case WD_CMD_MSGS__LATCH_SET_RESET__FORCE_HIGH:
                blimp_latchSetHigh();
                response.statusCode = WD_CMD_MSGS__RESPONSE_STATUS__SUCCESS;
                break;

            case WD_CMD_MSGS__LATCH_SET_RESET__FORCE_LOW:
                blimp_latchSetLow();
                response.statusCode = WD_CMD_MSGS__RESPONSE_STATUS__SUCCESS;
                break;

            default:
                response.statusCode = WD_CMD_MSGS__RESPONSE_STATUS__ERROR_BAD_COMMAND_PARAMETER;
        }

        return getState();
    }

    RoverState RoverStateBase::doGndCmdLatchResetPulseLow(RoverContext& theContext,
                                                          const WdCmdMsgs__Message& msg,
                                                          WdCmdMsgs__Response& response,
                                                          WdCmdMsgs__Response& deployNotificationResponse,
                                                          bool& sendDeployNotificationResponse)
    {

        switch (msg.body.latchResetPulseLow.selection) {
            case WD_CMD_MSGS__LATCH_SET_RESET__OFF:
                blimp_latchResetOff();
                response.statusCode = WD_CMD_MSGS__RESPONSE_STATUS__SUCCESS;
                break;

            case WD_CMD_MSGS__LATCH_SET_RESET__PULSE:
                blimp_latchResetPulseLow();
                response.statusCode = WD_CMD_MSGS__RESPONSE_STATUS__SUCCESS;
                break;

            case WD_CMD_MSGS__LATCH_SET_RESET__FORCE_HIGH:
                blimp_latchResetHigh();
                response.statusCode = WD_CMD_MSGS__RESPONSE_STATUS__SUCCESS;
                break;

            case WD_CMD_MSGS__LATCH_SET_RESET__FORCE_LOW:
                blimp_latchResetLow();
                response.statusCode = WD_CMD_MSGS__RESPONSE_STATUS__SUCCESS;
                break;

            default:
                response.statusCode = WD_CMD_MSGS__RESPONSE_STATUS__ERROR_BAD_COMMAND_PARAMETER;
        }

        return getState();
    }

    RoverState RoverStateBase::doGndCmdClearResetMemory(RoverContext& theContext,
                                                        const WdCmdMsgs__Message& msg,
                                                        WdCmdMsgs__Response& response,
                                                        WdCmdMsgs__Response& deployNotificationResponse,
                                                        bool& sendDeployNotificationResponse)
    {
        theContext.m_details.m_resetActionBits = 0;
        response.statusCode = WD_CMD_MSGS__RESPONSE_STATUS__SUCCESS;
        return getState();
    }

    RoverState RoverStateBase::doGndCmdRequestDetailedReport(RoverContext& theContext,
                                                             const WdCmdMsgs__Message& msg,
                                                             WdCmdMsgs__Response& response,
                                                             WdCmdMsgs__Response& deployNotificationResponse,
                                                             bool& sendDeployNotificationResponse)
    {
        theContext.m_sendDetailedReport = true;
        response.statusCode = WD_CMD_MSGS__RESPONSE_STATUS__SUCCESS;
        return getState();
    }

    void RoverStateBase::sendDetailedReportToLander(RoverContext& theContext)
    {
        /* send detailed report */
        static DetailedReport report = { 0 };
        GroundMsgs__Status gcStatus =
                GroundMsgs__generateDetailedReport(&(theContext.m_i2cReadings),
                                                   &(theContext.m_adcValues),
                                                   &(theContext.m_details),
                                                   &report);

        assert(GND_MSGS__STATUS__SUCCESS == gcStatus);

        LanderComms__Status lcStatus = LanderComms__txData(theContext.m_lcState,
                                                           (uint8_t*) &report,
                                                           sizeof(report));

        assert(LANDER_COMMS__STATUS__SUCCESS == lcStatus);
        if (LANDER_COMMS__STATUS__SUCCESS != lcStatus) {
            //!< @todo Handling?
        }
    }

    void RoverStateBase::doConditionalResetSpecific(RoverContext& theContext,
                                                    WdCmdMsgs__ResetSpecificId resetValue,
                                                    WdCmdMsgs__Response* response,
                                                    bool allowPowerOn,
                                                    bool allowDisableRs422,
                                                    bool allowDeploy,
                                                    bool allowUndeploy,
                                                    bool& writeIoExpander)
    {
        if (nullptr != response) {
            // Default to success, change if necessary
            response->statusCode = WD_CMD_MSGS__RESPONSE_STATUS__SUCCESS;
        }

        writeIoExpander = false;

        switch (resetValue) {
            case WD_CMD_MSGS__RESET_ID__NO_RESET:
                SET_RABI_IN_UINT(theContext.m_details.m_resetActionBits, RABI__NO_RESET);
                break;

            case WD_CMD_MSGS__RESET_ID__HERCULES_RESET:
                //!< @todo Should reset be allowed in KeepAlive? It's not technically powering on, but its similar.
                setHerculesReset();
                writeIoExpander = true;
                // queue up hercules unreset
                theContext.m_watchdogFlags |= WDFLAG_UNRESET_HERCULES;
                SET_RABI_IN_UINT(theContext.m_details.m_resetActionBits, RABI__HERCULES_RESET);
                break;

            case WD_CMD_MSGS__RESET_ID__HERCULES_POWER_ON:
                if (allowPowerOn) {
                    powerOnHercules();
                    SET_RABI_IN_UINT(theContext.m_details.m_resetActionBits, RABI__HERCULES_POWER_ON);
                } else if (nullptr != response) {
                    response->statusCode = WD_CMD_MSGS__RESPONSE_STATUS__ERROR_BAD_COMMAND_SEQUENCE;
                }
                break;

            case WD_CMD_MSGS__RESET_ID__HERCULES_POWER_OFF:
                powerOffHercules();
                SET_RABI_IN_UINT(theContext.m_details.m_resetActionBits, RABI__HERCULES_POWER_OFF);
                break;

            case WD_CMD_MSGS__RESET_ID__RADIO_RESET:
                //!< @todo Should reset be allowed in KeepAlive? It's not technically powering on, but its similar.
                setRadioReset();
                writeIoExpander = true;
                // queue up an radio unreset
                theContext.m_watchdogFlags |= WDFLAG_UNRESET_RADIO1;
                SET_RABI_IN_UINT(theContext.m_details.m_resetActionBits, RABI__RADIO_RESET);
                break;

            case WD_CMD_MSGS__RESET_ID__RADIO_POWER_ON:
                if (allowPowerOn) {
                    powerOnRadio();
                    writeIoExpander = true;
                    SET_RABI_IN_UINT(theContext.m_details.m_resetActionBits, RABI__RADIO_POWER_ON);
                } else if (nullptr != response) {
                    response->statusCode = WD_CMD_MSGS__RESPONSE_STATUS__ERROR_BAD_COMMAND_SEQUENCE;
                }
                break;

            case WD_CMD_MSGS__RESET_ID__RADIO_POWER_OFF:
                powerOffRadio();
                writeIoExpander = true;
                SET_RABI_IN_UINT(theContext.m_details.m_resetActionBits, RABI__RADIO_POWER_OFF);
                break;

            case WD_CMD_MSGS__RESET_ID__CAM_FPGA_RESET:
                //!< @todo Should reset be allowed in KeepAlive? It's not technically powering on, but its similar.
                setFPGAReset();
                writeIoExpander = true;
                // queue up the fpga unreset
                theContext.m_watchdogFlags |= WDFLAG_UNRESET_FPGA;
                SET_RABI_IN_UINT(theContext.m_details.m_resetActionBits, RABI__CAM_FPGA_RESET);
                break;

            case WD_CMD_MSGS__RESET_ID__CAM_FPGA_POWER_ON:
                if (allowPowerOn) {
                    powerOnFpga();
                    SET_RABI_IN_UINT(theContext.m_details.m_resetActionBits, RABI__CAM_FPGA_POWER_ON);
                } else if (nullptr != response) {
                    response->statusCode = WD_CMD_MSGS__RESPONSE_STATUS__ERROR_BAD_COMMAND_SEQUENCE;
                }
                break;

            case WD_CMD_MSGS__RESET_ID__CAM_FPGA_POWER_OFF:
                powerOffFpga();
                SET_RABI_IN_UINT(theContext.m_details.m_resetActionBits, RABI__CAM_FPGA_POWER_OFF);
                break;

            case WD_CMD_MSGS__RESET_ID__MOTOR_1_RESET:
                setMotor1Reset();
                writeIoExpander = true;
                // queue up the motor 1 unreset
                theContext.m_watchdogFlags |= WDFLAG_UNRESET_MOTOR1;
                break;

            case WD_CMD_MSGS__RESET_ID__MOTOR_2_RESET:
                setMotor2Reset();
                writeIoExpander = true;
                // queue up the motor 2 unreset
                theContext.m_watchdogFlags |= WDFLAG_UNRESET_MOTOR2;
                break;

            case WD_CMD_MSGS__RESET_ID__MOTOR_3_RESET:
                setMotor3Reset();
                writeIoExpander = true;
                // queue up the motor 3 unreset
                theContext.m_watchdogFlags |= WDFLAG_UNRESET_MOTOR3;
                break;

            case WD_CMD_MSGS__RESET_ID__MOTOR_4_RESET:
                setMotor4Reset();
                writeIoExpander = true;
                // queue up the motor 4 unreset
                theContext.m_watchdogFlags |= WDFLAG_UNRESET_MOTOR4;
                break;

            case WD_CMD_MSGS__RESET_ID__ALL_MOTORS_RESET:
                setMotorsReset();
                writeIoExpander = true;
                // queue up all the motor unresets
                theContext.m_watchdogFlags |= WDFLAG_UNRESET_MOTOR1 | WDFLAG_UNRESET_MOTOR2 |
                        WDFLAG_UNRESET_MOTOR3 | WDFLAG_UNRESET_MOTOR4;
                break;

            case WD_CMD_MSGS__RESET_ID__ALL_MOTORS_POWER_ON:
                if (allowPowerOn) {
                    powerOnMotors();
                    SET_RABI_IN_UINT(theContext.m_details.m_resetActionBits, RABI__ALL_MOTORS_POWER_ON);
                } else if (nullptr != response) {
                    response->statusCode = WD_CMD_MSGS__RESPONSE_STATUS__ERROR_BAD_COMMAND_SEQUENCE;
                }
                break;

            case WD_CMD_MSGS__RESET_ID__ALL_MOTORS_POWER_OFF:
                powerOffMotors();
                SET_RABI_IN_UINT(theContext.m_details.m_resetActionBits, RABI__ALL_MOTORS_POWER_OFF);
                break;

            case WD_CMD_MSGS__RESET_ID__3_3V_EN_RESET:
                if (allowPowerOn) {
                    disable3V3PowerRail();
                    // queue up 3V3 rail on again
                    theContext.m_watchdogFlags |= WDFLAG_UNRESET_3V3;
                    SET_RABI_IN_UINT(theContext.m_details.m_resetActionBits, RABI__3V3_EN_RESET);
                } else if (nullptr != response) {
                    response->statusCode = WD_CMD_MSGS__RESPONSE_STATUS__ERROR_BAD_COMMAND_SEQUENCE;
                }
                break;

            case WD_CMD_MSGS__RESET_ID__3_3V_EN_POWER_ON:
                if (allowPowerOn) {
                    enable3V3PowerRail();
                    blimp_vSysAllEnOn();
                    SET_RABI_IN_UINT(theContext.m_details.m_resetActionBits, RABI__3V3_EN_POWER_ON);
                } else if (nullptr != response) {
                    response->statusCode = WD_CMD_MSGS__RESPONSE_STATUS__ERROR_BAD_COMMAND_SEQUENCE;
                }
                break;

            case WD_CMD_MSGS__RESET_ID__3_3V_EN_POWER_OFF:
                disable3V3PowerRail();
                SET_RABI_IN_UINT(theContext.m_details.m_resetActionBits, RABI__3V3_EN_POWER_OFF);
                break;

            case WD_CMD_MSGS__RESET_ID__24V_EN_RESET:
                if (allowPowerOn) {
                    disable24VPowerRail();
                    // queue up 24V rail on again
                    theContext.m_watchdogFlags |= WDFLAG_UNRESET_24V;
                    SET_RABI_IN_UINT(theContext.m_details.m_resetActionBits, RABI__24V_EN_RESET);
                } else if (nullptr != response) {
                    response->statusCode = WD_CMD_MSGS__RESPONSE_STATUS__ERROR_BAD_COMMAND_SEQUENCE;
                }
                break;

            case WD_CMD_MSGS__RESET_ID__24V_EN_POWER_ON:
                if (allowPowerOn) {
                    enable24VPowerRail();
                    SET_RABI_IN_UINT(theContext.m_details.m_resetActionBits, RABI__24V_EN_POWER_ON);
                } else if (nullptr != response) {
                    response->statusCode = WD_CMD_MSGS__RESPONSE_STATUS__ERROR_BAD_COMMAND_SEQUENCE;
                }
                break;

            case WD_CMD_MSGS__RESET_ID__24V_EN_POWER_OFF:
                disable24VPowerRail();
                SET_RABI_IN_UINT(theContext.m_details.m_resetActionBits, RABI__24V_EN_POWER_OFF);
                break;

            case WD_CMD_MSGS__RESET_ID__HDRM_DEPLOY_SIGNAL_POWER_OFF:
                if (allowUndeploy) {
                    unsetDeploy();
                    SET_RABI_IN_UINT(theContext.m_details.m_resetActionBits, RABI__HDRM_DEPLOY_SIGNAL_POWER_OFF);
                } else if (nullptr != response) {
                    response->statusCode = WD_CMD_MSGS__RESPONSE_STATUS__ERROR_BAD_COMMAND_SEQUENCE;
                }
                break;

            case WD_CMD_MSGS__RESET_ID__FPGA_CAM_0_SELECT:
                fpgaCameraSelectLo();
                SET_RABI_IN_UINT(theContext.m_details.m_resetActionBits, RABI__FPGA_CAM_0_SELECT);
                break;

            case WD_CMD_MSGS__RESET_ID__FPGA_CAM_1_SELECT:
                fpgaCameraSelectHi();
                SET_RABI_IN_UINT(theContext.m_details.m_resetActionBits, RABI__FPGA_CAM_1_SELECT);
                break;

            case WD_CMD_MSGS__RESET_ID__BATTERY_CHARGE_START:
                startChargingBatteries();
                SET_RABI_IN_UINT(theContext.m_details.m_resetActionBits, RABI__BATTERY_CHARGE_START);
                break;

            case WD_CMD_MSGS__RESET_ID__BATTERY_CHARGE_STOP:
                stopChargingBatteries();
                SET_RABI_IN_UINT(theContext.m_details.m_resetActionBits, RABI__BATTERY_CHARGE_STOP);
                break;

            case WD_CMD_MSGS__RESET_ID__RS422_UART_ENABLE:
                //!< @todo IMPLEMENT
                SET_RABI_IN_UINT(theContext.m_details.m_resetActionBits, RABI__RS422_UART_ENABLE);
                break;

            case WD_CMD_MSGS__RESET_ID__RS422_UART_DISABLE:
                if (allowDisableRs422) {
                    //!< @todo IMPLEMENT
                SET_RABI_IN_UINT(theContext.m_details.m_resetActionBits, RABI__RS422_UART_DISABLE);
                } else if (nullptr != response) {
                    response->statusCode = WD_CMD_MSGS__RESPONSE_STATUS__ERROR_BAD_COMMAND_SEQUENCE;
                }
                break;

            case WD_CMD_MSGS__RESET_ID__AUTO_HEATER_CONTROLLER_ENABLE:
                theContext.m_details.m_hParams.m_heatingControlEnabled = true;
                SET_RABI_IN_UINT(theContext.m_details.m_resetActionBits, RABI__AUTO_HEATER_CONTROLLER_ENABLE);
                break;

            case WD_CMD_MSGS__RESET_ID__AUTO_HEATER_CONTROLLER_DISABLE:
                theContext.m_details.m_hParams.m_heatingControlEnabled = false;
                /**
                 * @warning TB0CCR2 should ~NOT~ be set here. It should only be set in two places: when Timer_B is
                 *          initialized (where TB0CCR2 is set to its default value), and in the handler for the
                 *          "Set Heater Duty Cycle" ground command. By respecting this, TB0CCR2 keeps whatever value it
                 *          was set with by the ground command and the heater is interacted with (by this command and
                 *          the auto heater control loop) simply by enabling/disabling the heater pin.
                 *
                 * @note Similar to the TB0CC2, the m_heating field should not be set here. Instead, it should only be
                 *       modified via calls to enableHeater() and disableHeater().
                 */
                disableHeater();
                SET_RABI_IN_UINT(theContext.m_details.m_resetActionBits, RABI__AUTO_HEATER_CONTROLLER_DISABLE);
                break;

            case WD_CMD_MSGS__RESET_ID__HERCULES_WATCHDOG_ENABLE:
                theContext.m_watchdogOpts |= WDOPT_MONITOR_HERCULES;
                SET_RABI_IN_UINT(theContext.m_details.m_resetActionBits, RABI__HERCULES_WATCHDOG_ENABLE);
                break;

            case WD_CMD_MSGS__RESET_ID__HERCULES_WATCHDOG_DISABLE:
                theContext.m_watchdogOpts &= ~WDOPT_MONITOR_HERCULES;
                SET_RABI_IN_UINT(theContext.m_details.m_resetActionBits, RABI__HERCULES_WATCHDOG_DISABLE);
                break;

            case WD_CMD_MSGS__RESET_ID__BATTERIES_ENABLE:
                if (allowPowerOn) {
                    enableBatteries();
                    SET_RABI_IN_UINT(theContext.m_details.m_resetActionBits, RABI__BATTERIES_ENABLE);
                } else if (nullptr != response) {
                    response->statusCode = WD_CMD_MSGS__RESPONSE_STATUS__ERROR_BAD_COMMAND_SEQUENCE;
                }
                break;

            case WD_CMD_MSGS__RESET_ID__BATTERIES_DISABLE:
                disableBatteries();
                SET_RABI_IN_UINT(theContext.m_details.m_resetActionBits, RABI__BATTERIES_DISABLE);
                break;

            case WD_CMD_MSGS__RESET_ID__HDRM_DEPLOY_SIGNAL_POWER_ON:
                if (allowPowerOn && allowDeploy) {
                    /* WOOT WOOT! WE ARE ON TO THE MOON, FOLKS */
                    /* ref: https://docs.google.com/document/d/1dKLlBcIIVo8t1bGu3jNiHobGMavA3I2al0cncj3ZAhE/edit */
                    setDeploy();
                    SET_RABI_IN_UINT(theContext.m_details.m_resetActionBits, RABI__HDRM_DEPLOY_SIGNAL_POWER_ON);
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
