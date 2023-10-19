#include "stateMachine/RoverStateBase.hpp"

#include "comms/debug_comms.h"
#include "comms/hercules_comms.h"
#include "comms/wd_int_mpsm.h"
#include "drivers/adc.h"
#include "drivers/bsp.h"
#include "drivers/blimp.h"
#include "drivers/uart.h"
#include "event/event.h"
#include "event/event_queue.h"
#include "utils/time.h"

#include "comms/ground_msgs.h"
#include "watchdog.h"

#include <cassert>
#include <msp430.h>
#include <cstdio>
#include <cstring>
#include <cstdarg>

#define DEBUG_REPORT false

namespace iris
{

    struct CallbackUserArg
    {
        RoverStateBase &m_state;
        RoverContext &m_context;

        explicit CallbackUserArg(RoverStateBase &state, RoverContext &context)
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

    bool RoverStateBase::canEnterLowPowerMode(RoverContext & /*theContext*/)
    {
        return true;
    }

    RoverState RoverStateBase::handleHighTemp(RoverContext &theContext)
    {
        return getState();
    }

    RoverState RoverStateBase::handleWdIntEdge(bool rising, RoverContext &theContext)
    {
        uint16_t flatDuration = watchdog_get_wd_int_flat_duration();
        WdIntMpsm__Status status = WdIntMpsm__processEdge(rising ? TRUE : FALSE, flatDuration);

        switch (status)
        {
        case WD_INT_MPSM__STATUS__POWER_CYCLE_RADIO:
            return handleRadioPowerCycleRadioCommand(theContext);
        case WD_INT_MPSM__STATUS__POWER_CYCLE_HERCULES:
            return handleRadioPowerCycleHerculesCommand(theContext);
        case WD_INT_MPSM__STATUS__PARSED_EXIT_STASIS:
            return handleRadioExitStasisCommand(theContext);
        case WD_INT_MPSM__STATUS__PARSED_ENTER_STASIS:
            return handleRadioEnterStasisCommand(theContext);
        case WD_INT_MPSM__STATUS__PARSED_GOT_WIFI:
            return handleRadioGotWifiCommand(theContext);
        }

        return getState();
    }

    RoverState RoverStateBase::handleWdIntRisingEdge(RoverContext &theContext)
    {
        return handleWdIntEdge(true, theContext);
    }

    RoverState RoverStateBase::handleWdIntFallingEdge(RoverContext &theContext)
    {
        return handleWdIntEdge(false, theContext);
    }

    RoverState RoverStateBase::handleRadioPowerCycleRadioCommand(RoverContext &)
    {
        setRadioReset();
        powerOffRadio();
        __delay_cycles(100000); // Should be at least 2 us (per blimp.h)
        powerOnRadio();
        __delay_cycles(10000);
        releaseRadioReset();

        return getState();
    }

    RoverState RoverStateBase::handleRadioPowerCycleHerculesCommand(RoverContext &)
    {
        setHerculesReset();
        powerOffHercules();
        __delay_cycles(100000); // Should be at least 2 us (per blimp.h)
        powerOnHercules();
        __delay_cycles(10000);
        releaseHerculesReset();

        return getState();
    }

    RoverState RoverStateBase::handleRadioExitStasisCommand(RoverContext &)
    {
        return getState();
    }

    RoverState RoverStateBase::handleRadioEnterStasisCommand(RoverContext &)
    {
        return RoverState::ENTERING_STASIS;
    }

    RoverState RoverStateBase::handleRadioGotWifiCommand(RoverContext &theContext)
    {
        DPRINTF("Got WiFi Event.\n");
        theContext.gotWifi = true;
        return getState();
    }

    LanderComms__Status RoverStateBase::txDownlinkData(RoverContext &theContext, void *data, size_t dataSize,
                                                       bool fromHercules)
    {
        LanderComms__Status lcStatus = LANDER_COMMS__STATUS__SUCCESS;

        // Forward comms to Hercules if you can (and it's not from Hercules):
        if (HerculesComms__isInitialized(theContext.m_hcState) && !fromHercules)
        {
            HerculesComms__Status hcStatus = HerculesComms__txDownlinkData(theContext.m_hcState,
                                                                           (uint8_t *)data,
                                                                           dataSize);
            lcStatus = (LanderComms__Status)hcStatus;
        }

        // Also forward to lander if not deployed:
        LanderComms__Status lcStatus2 = LanderComms__txData(theContext.m_lcState, (uint8_t *)data, dataSize);

        return lcStatus;
    }

    void RoverStateBase::initiateNextI2cAction(RoverContext &theContext)
    {
        // A static value so that we remember the index of the last action we performed. By doing this, we can rotate
        // through all possibles actions to make sure that a single action repeated with a high frequency doesn't
        // prevent other actions from being triggered.
        static size_t i = static_cast<size_t>(I2C_SENSORS__ACTIONS__INACTIVE);

        // Exit early if we have no queued actions
        if (theContext.m_queuedI2cActions == 0)
        {
            return;
        }

        // See what, if anything, is the next action to be performed
        for (size_t j = 0; j < static_cast<size_t>(I2C_SENSORS__ACTIONS__COUNT); ++j)
        {
            size_t actionIndex = i + j;

            // Wrap around if necessary (in theory only an if should be necessary, but a while loop seems safer)
            while (actionIndex >= static_cast<size_t>(I2C_SENSORS__ACTIONS__COUNT))
            {
                actionIndex -= static_cast<size_t>(I2C_SENSORS__ACTIONS__COUNT);
            }

            uint16_t actionMask = 1 << actionIndex;

            // If the corresponding bit in m_queuedI2cActions is set, then we want to perform that action
            if ((theContext.m_queuedI2cActions & actionMask) != 0)
            {
                // First, initiate the action
                I2C_Sensors__Action actionEnum = static_cast<I2C_Sensors__Action>(actionIndex);

                I2C_Sensors__Status i2cStatus = I2C_SENSORS__STATUS__SUCCESS_DONE;

                switch (actionEnum)
                {
                case I2C_SENSORS__ACTIONS__INACTIVE:
                    // This is always false in this case, but rechecking this condition makes the resulting output
                    // more descriptive.
                    DEBUG_ASSERT_NOT_EQUAL(actionEnum, I2C_SENSORS__ACTIONS__INACTIVE);
                    break;

                case I2C_SENSORS__ACTIONS__GAUGE_READING:
                    i2cStatus = I2C_Sensors__initiateGaugeReadings();
                    DEBUG_ASSERT_EQUAL(i2cStatus, I2C_SENSORS__STATUS__SUCCESS_DONE);
                    break;

                case I2C_SENSORS__ACTIONS__GAUGE_INIT:
                    i2cStatus = I2C_Sensors__initiateFuelGaugeInitialization();
                    DEBUG_ASSERT_EQUAL(i2cStatus, I2C_SENSORS__STATUS__SUCCESS_DONE);
                    break;

                case I2C_SENSORS__ACTIONS__WRITE_GAUGE_LOW_POWER:
                    i2cStatus = I2C_Sensors__initiateWriteLowPower();
                    DEBUG_ASSERT_EQUAL(i2cStatus, I2C_SENSORS__STATUS__SUCCESS_DONE);
                    break;

                case I2C_SENSORS__ACTIONS__READ_GAUGE_CONTROL_REGISTER:
                    i2cStatus = I2C_Sensors__initiateReadControl();
                    DEBUG_ASSERT_EQUAL(i2cStatus, I2C_SENSORS__STATUS__SUCCESS_DONE);
                    break;

                case I2C_SENSORS__ACTIONS__INIT_IO_EXPANDER:
                    i2cStatus = I2C_Sensors__initiateIoExpanderInitialization();
                    DEBUG_ASSERT_EQUAL(i2cStatus, I2C_SENSORS__STATUS__SUCCESS_DONE);
                    break;

                case I2C_SENSORS__ACTIONS__WRITE_IO_EXPANDER:
                    if (theContext.m_writeCustomIoExpanderValues)
                    {
                        i2cStatus = I2C_Sensors__initiateWriteIoExpander(theContext.m_queuedIOWritePort0Value,
                                                                         theContext.m_queuedIOWritePort1Value);
                    }
                    else
                    {
                        i2cStatus = I2C_Sensors__initiateWriteIoExpanderCurrentValues();
                    }
                    DEBUG_ASSERT_EQUAL(i2cStatus, I2C_SENSORS__STATUS__SUCCESS_DONE);
                    break;

                case I2C_SENSORS__ACTIONS__READ_IO_EXPANDER:
                    i2cStatus = I2C_Sensors__initiateReadIoExpander();
                    DEBUG_ASSERT_EQUAL(i2cStatus, I2C_SENSORS__STATUS__SUCCESS_DONE);
                    break;

                default:
                    // This is always false in this case, but rechecking this condition makes the resulting output
                    // more descriptive.
                    DEBUG_ASSERT(actionEnum >= I2C_SENSORS__ACTIONS__INACTIVE && actionEnum < I2C_SENSORS__ACTIONS__COUNT);
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

    void RoverStateBase::heaterControl(RoverContext &theContext)
    {
        HeaterParams &hParams = theContext.m_details.m_hParams;
        unsigned short thermReading;
        if (hParams.m_inputSource == HEATER_CONTROL_INPUT_CHARGER)
        {
            thermReading = theContext.m_adcValues.battTemp;
        }
        else
        {
            // Default case (in case of bitflips, etc.), use normal thermistor:
            thermReading = theContext.m_adcValues.battRT;
        }

        // Process force states and eject early (nothing more to do):
        if (hParams.m_forceState == HEATER_FORCE_ALWAYS_ON)
        {
            if (!hParams.m_heating)
            {
                enableHeater();
            }
            return;
        }
        else if (hParams.m_forceState == HEATER_FORCE_ALWAYS_OFF)
        {
            if (hParams.m_heating)
            {
                disableHeater();
            }
            return;
        }

        // Check for threshold changes that need to be handled first:
        if (hParams.m_thresholdsChanged)
        {
            // Ground operators changed the thresholds. Re-evaluate against
            // thresholds:
            if (thermReading > hParams.m_heaterOnVal)
            {
                enableHeater();
            }
            else if (thermReading < hParams.m_heaterOffVal)
            {
                disableHeater();
            }
            else
            {
                // We're somewhere in the middle, in which case, for safety, we
                // should default to ON.
                // NOTE: unless the settings were intentionally configured
                // weirdly during mission, there should be no middle ground
                // because Toff should be > Ton (that is ADCoff < ADCon b/c NTC)
                enableHeater();
            }
            hParams.m_thresholdsChanged = false;
        }

        // Check for failure cases, in which case, turn the heater ON:
        if (thermReading < 5 && Time__getTimeInCentiseconds() > 6000)
        {
            // if the sensor is giving an ADC reading of basically 0, we
            // probably have an open circuit (at max T for the thermistor
            // (+155C), we only go as low as ~45).
            // NOTE: We provide a 60s grace period for everything to initialize.
            // This is way longer than necessary but it's relatively short on
            // thermal time-scales so this is fine.
            // For safety, enable the heater (unless told explicitly not to do
            // so via a force state):
            if (!hParams.m_heating)
            {
                // Let Earth know we're turning on the Heat (this might be during an LOS):
                DebugComms__tryPrintfToLanderNonblocking("POSSIBLE BATT_RT THERMISTOR FAILURE DETECTED. ADC < 5. ENABLING HEATER FOR SAFETY. RECOMMEND MANUAL HEATER CONTROL.");
                enableHeater();
            }
        }
        else
        {
            // Normal controller state machine operation:
            if (hParams.m_heating)
            {
                // In the HEATER_ON state...
                if (thermReading < hParams.m_heaterOffVal)
                {
                    // if we should turn OFF and we're not being forced ON, transition to HEATER_OFF
                    // Start heating when temperature rises high enough, which we detect via the ADC reading falling below a
                    // configured (either via the default value or a value commanded from ground) ADC reading.
                    disableHeater();
                }
            }
            else
            {
                // In the HEATER_OFF state...
                if (thermReading > hParams.m_heaterOnVal)
                {
                    // if we should turn ON and we're not being forced OFF, transition to HEATER_ON
                    // Start heating when temperature drops low enough, which we detect via the ADC reading rising above a
                    // configured (either via the default value or a value commanded from ground) ADC reading.
                    enableHeater();
                }
            }
        }
    }

    void RoverStateBase::enableHerculesComms(RoverContext &theContext)
    {
        if (!UART__isInitialized(theContext.m_uart0State))
        {
            UART__Status uartStatus = UART__init0(&(theContext.m_uartConfig),
                                                  &(theContext.m_uart0State));

            DEBUG_LOG_CHECK_STATUS(UART__STATUS__SUCCESS, uartStatus, "Failed to init UART0");
            DEBUG_ASSERT_EQUAL(UART__STATUS__SUCCESS, uartStatus);
        }

        if (!HerculesComms__isInitialized(theContext.m_hcState))
        {
            HerculesComms__Status hcStatus = HerculesComms__init(&(theContext.m_hcState), theContext.m_uart0State);
            DEBUG_LOG_CHECK_STATUS(HERCULES_COMMS__STATUS__SUCCESS, hcStatus, "Failed to init Hercules Comms");
            DEBUG_ASSERT_EQUAL(HERCULES_COMMS__STATUS__SUCCESS, hcStatus);
        }

        DebugComms__registerHerculesComms(theContext.m_hcState);
    }

    RoverState RoverStateBase::handleLanderData(RoverContext &theContext)
    {
        return pumpMsgsFromLander(theContext);
    }

    RoverState RoverStateBase::handleHerculesData(RoverContext &theContext)
    {
        return pumpMsgsFromHercules(theContext);
    }

    void RoverStateBase::herculesMsgCallback(HercMsgs__Header *header,
                                             uint8_t *payloadBuffer,
                                             size_t payloadSize,
                                             void *userArg)
    {
        if (nullptr == userArg || nullptr == header || (payloadSize > 0 && NULL == payloadBuffer))
        {
            return;
        }

        CallbackUserArg *args = reinterpret_cast<CallbackUserArg *>(userArg);

        args->m_context.m_watchdogFlags |= WDFLAG_HERCULES_KICK;

        switch (header->lowerOpCode)
        {
        case HERCULES_COMMS__MSG_OPCODE__STROKE:
            args->m_state.handleStrokeFromHercules(args->m_context, header);
            break;

        case HERCULES_COMMS__MSG_OPCODE__DOWNLINK:
            args->m_state.handleDownlinkFromHercules(args->m_context,
                                                     header,
                                                     payloadBuffer,
                                                     payloadSize);
            break;

        case HERCULES_COMMS__MSG_OPCODE__DEBUG:
            args->m_state.handleDebugFromHercules(args->m_context,
                                                  header,
                                                  payloadBuffer,
                                                  payloadSize);
            break;

        default: // We assume this is a reset command
            args->m_state.handleResetFromHercules(args->m_context, header);
            break;
        }
    }

    void RoverStateBase::landerMsgCallback(uint8_t *rxDataBuffer, size_t rxDataLen, void *userArg)
    {
        // Statically allocate the message structure so that it's not allocated on the stack
        static WdCmdMsgs__Message wdMessage = {0};
        static WdCmdMsgs__Response response = {0};
        static WdCmdMsgs__Response deployNotificationResponse = {0};

#pragma diag_push
#pragma diag_suppress 770
        DEBUG_ASSERT_NOT_EQUAL(nullptr, userArg);
#pragma diag_pop

        CallbackUserArg *args = reinterpret_cast<CallbackUserArg *>(userArg);

        if (nullptr == rxDataBuffer)
        {
            //!< @todo reply to lander with this error
            return;
        }

        // First of all, parse the FSW common header. We assume it is part of a watchdog command message,
        // as the
        CmdMsgs__Status cmdStatus = CmdMsgs__deserializeHeader(rxDataBuffer, rxDataLen, &(wdMessage.commonHeader));

        DEBUG_ASSERT_EQUAL(CMD_MSGS__STATUS__SUCCESS, cmdStatus);
        if (CMD_MSGS__STATUS__SUCCESS != cmdStatus)
        {
            // This should only really happen if rxDataLen is the wrong size
            //!< @todo reply to lander with this error
            return;
        }

        // We identify a ground command for WD by checking the "Type Magic" field in the FSW common header.
        // If the magic number in that field indicates the command is for Watchdog, then it is a ground
        // command for us.
        if (HEADER__TYPE_MAGIC_NUM__WATCHDOG_COMMAND == wdMessage.commonHeader.typeMagicNumber)
        {
            // This is a watchdog command. Parse it as such
            WdCmdMsgs__Status wdCmdStatus = WdCmdMsgs__deserializeMessage(rxDataBuffer,
                                                                          rxDataLen,
                                                                          &wdMessage,
                                                                          FALSE); // Don't reparse the header

            DEBUG_ASSERT_EQUAL(WD_CMD_MSGS__STATUS__SUCCESS, wdCmdStatus);
            if (WD_CMD_MSGS__STATUS__SUCCESS != wdCmdStatus)
            {
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

            if (sendDeployNotificationResponse)
            {
                args->m_state.sendLanderResponse(args->m_context, deployNotificationResponse);
            }

            args->m_state.sendLanderResponse(args->m_context, response);
        }
        else
        {
            // Anything with "Type Magic" field value that isn't for Watchdog is treated as uplink for Hercules
            args->m_state.m_pumpMsgsFromLanderReturnState = args->m_state.handleUplinkFromLander(args->m_context,
                                                                                                 rxDataBuffer,
                                                                                                 rxDataLen);
        }
    }

    RoverState RoverStateBase::performWatchdogCommand(RoverContext &theContext,
                                                      const WdCmdMsgs__Message &msg,
                                                      WdCmdMsgs__Response &response,
                                                      WdCmdMsgs__Response &deployNotificationResponse,
                                                      bool &sendDeployNotificationResponse)
    {
        // Make sure that by default we don't want to send the deploy notification response
        sendDeployNotificationResponse = false;

        // We always want to set the magic number and command ID (to which this is a response) on the response
        response.magicNumber = WD_CMD_MSGS__RESPONSE_MAGIC_NUMBER;
        response.commandId = msg.commandId;

        switch (msg.commandId)
        {
        case WD_CMD_MSGS__CMD_ID__RESET_SPECIFIC:
            return doGndCmdResetSpecific(theContext,
                                         msg,
                                         response,
                                         deployNotificationResponse,
                                         sendDeployNotificationResponse);

        case WD_CMD_MSGS__CMD_ID__PREP_FOR_DEPLOY:
            /* power on all systems */
            if (msg.body.prepForDeploy.confirmationMagicNumber != WD_CMD_MSGS__CONFIRM_DEPLOYMENT_MAGIC_NUMBER)
            {
                /* magic bad */
                response.statusCode = WD_CMD_MSGS__RESPONSE_STATUS__ERROR_BAD_COMMAND_PARAMETER;
            }
            else
            {
                return doGndCmdPrepForDeploy(theContext,
                                             msg,
                                             response,
                                             deployNotificationResponse,
                                             sendDeployNotificationResponse);
            }
            break;

        case WD_CMD_MSGS__CMD_ID__DEPLOY:
            if (msg.body.deploy.confirmationMagicNumber != WD_CMD_MSGS__CONFIRM_DEPLOYMENT_MAGIC_NUMBER)
            {
                /* magic bad */
                response.statusCode = WD_CMD_MSGS__RESPONSE_STATUS__ERROR_BAD_COMMAND_PARAMETER;
            }
            else
            {
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

        case WD_CMD_MSGS__CMD_ID__SET_DEBUG_COMMS_STATE:
            if (msg.body.setDebugCommsState.magic != WD_CMD_MSGS__SET_DEBUG_COMMS_STATE_MAGIC_NUMBER)
            {
                /* magic bad */
                response.statusCode = WD_CMD_MSGS__RESPONSE_STATUS__ERROR_BAD_COMMAND_PARAMETER;
            }
            else
            {
                return doGndCmdSetDebugCommsState(theContext,
                                                  msg,
                                                  response,
                                                  deployNotificationResponse,
                                                  sendDeployNotificationResponse);
            }
            break;

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

        case WD_CMD_MSGS__CMD_ID__SET_VSAE_STATE:
            if (msg.body.setVSAEState.magic != WD_CMD_MSGS__SET_VSAE_STATE_MAGIC_NUMBER)
            {
                /* magic bad */
                response.statusCode = WD_CMD_MSGS__RESPONSE_STATUS__ERROR_BAD_COMMAND_PARAMETER;
            }
            else
            {
                return doGndCmdSetVSAEState(theContext,
                                            msg,
                                            response,
                                            deployNotificationResponse,
                                            sendDeployNotificationResponse);
            }
            break;

        case WD_CMD_MSGS__CMD_ID__ENTER_SLEEP_MODE:
            /* Enter sleep mode */
            if (msg.body.enterSleepMode.confirmationMagicNumber != WD_CMD_MSGS__CONFIRM_MODE_CHANGE_MAGIC_NUMBER)
            {
                /* magic bad */
                response.statusCode = WD_CMD_MSGS__RESPONSE_STATUS__ERROR_BAD_COMMAND_PARAMETER;
            }
            else
            {
                return doGndCmdEnterSleepMode(theContext,
                                              msg,
                                              response,
                                              deployNotificationResponse,
                                              sendDeployNotificationResponse);
            }
            break;

        case WD_CMD_MSGS__CMD_ID__ENTER_KEEPALIVE_MODE:
            /* Enter keepalive mode */
            if (msg.body.enterKeepAliveMode.confirmationMagicNumber != WD_CMD_MSGS__CONFIRM_MODE_CHANGE_MAGIC_NUMBER)
            {
                /* magic bad */
                response.statusCode = WD_CMD_MSGS__RESPONSE_STATUS__ERROR_BAD_COMMAND_PARAMETER;
            }
            else
            {
                return doGndCmdEnterKeepAliveMode(theContext,
                                                  msg,
                                                  response,
                                                  deployNotificationResponse,
                                                  sendDeployNotificationResponse);
            }
            break;

        case WD_CMD_MSGS__CMD_ID__ENTER_SERVICE_MODE:
            /* Enter service mode */
            if (msg.body.enterServiceMode.confirmationMagicNumber != WD_CMD_MSGS__CONFIRM_MODE_CHANGE_MAGIC_NUMBER)
            {
                /* magic bad */
                response.statusCode = WD_CMD_MSGS__RESPONSE_STATUS__ERROR_BAD_COMMAND_PARAMETER;
            }
            else
            {
                return doGndCmdEnterServiceMode(theContext,
                                                msg,
                                                response,
                                                deployNotificationResponse,
                                                sendDeployNotificationResponse);
            }
            break;

        case WD_CMD_MSGS__CMD_ID__CLEAR_RESET_MEMORY:
            if ((msg.body.clearResetMem.magicOne != WD_CMD_MSGS__CONFIRM_CLR_RST_MEM_MAGIC_NUMBER_ONE) ||
                (msg.body.clearResetMem.magicTwo != WD_CMD_MSGS__CONFIRM_CLR_RST_MEM_MAGIC_NUMBER_TWO))
            {
                /* magic bad */
                response.statusCode = WD_CMD_MSGS__RESPONSE_STATUS__ERROR_BAD_COMMAND_PARAMETER;
            }
            else
            {
                return doGndCmdClearResetMemory(theContext,
                                                msg,
                                                response,
                                                deployNotificationResponse,
                                                sendDeployNotificationResponse);
            }
            break;

        case WD_CMD_MSGS__CMD_ID__DANGEROUS_FORCE_BATT_STATE:
            if ((msg.body.dangForceBattState.confirmationMagicNumberOne != WD_CMD_MSGS__CONFIRM_DANG_FORCE_BATT_STATE_MAGIC_NUMBER_ONE) ||
                (msg.body.dangForceBattState.confirmationMagicNumberTwo != WD_CMD_MSGS__CONFIRM_DANG_FORCE_BATT_STATE_MAGIC_NUMBER_TWO))
            {
                /* magic bad */
                response.statusCode = WD_CMD_MSGS__RESPONSE_STATUS__ERROR_BAD_COMMAND_PARAMETER;
            }
            else
            {
                return doGndCmdDangForceBattState(theContext,
                                                  msg,
                                                  response,
                                                  deployNotificationResponse,
                                                  sendDeployNotificationResponse);
            }
            break;

        case WD_CMD_MSGS__CMD_ID__ECHO:
            return doGndCmdEcho(theContext,
                                msg,
                                response,
                                deployNotificationResponse,
                                sendDeployNotificationResponse);

        case WD_CMD_MSGS__CMD_ID__REQUEST_DETAILED_REPORT:
            /* Make sure magic is valid: */
            if (msg.body.reqDetReport.magic != WD_CMD_MSGS__CONFIRM_REQ_DET_REPORT_MAGIC_NUMBER)
            {
                /* magic bad */
                response.statusCode = WD_CMD_MSGS__RESPONSE_STATUS__ERROR_BAD_COMMAND_PARAMETER;
            }
            else
            {
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

    RoverState RoverStateBase::handleUplinkFromLander(RoverContext &theContext,
                                                      uint8_t *rxDataBuffer,
                                                      size_t rxDataLen)
    {
        // Anything with "Type Magic" field value that isn't for Watchdog is treated as uplink for Hercules
        HerculesComms__Status hcStatus = HerculesComms__txUplinkMsg(theContext.m_hcState,
                                                                    rxDataBuffer,
                                                                    rxDataLen);

        if (HERCULES_COMMS__STATUS__SUCCESS != hcStatus)
        {
            //!< @todo handling? Send response to lander maybe?
        }

        return getState();
    }

    RoverState RoverStateBase::handleStrokeFromHercules(RoverContext &theContext,
                                                        HercMsgs__Header *header)
    {
        static uint8_t telemetrySerializationBuffer[16] = {0};

#pragma diag_push
#pragma diag_suppress 770
        DEBUG_ASSERT_NOT_EQUAL(nullptr, header);
#pragma diag_pop
        // For a stroke we just reply to the Hercules with our telemetry
        watchdog_build_hercules_telem(&(theContext.m_i2cReadings),
                                      &(theContext.m_adcValues),
                                      (*(theContext.m_persistentDeployed) ? TRUE : FALSE),
                                      telemetrySerializationBuffer,
                                      sizeof(telemetrySerializationBuffer));

        HerculesComms__Status hcStatus = HerculesComms__txResponseMsg(theContext.m_hcState,
                                                                      header,
                                                                      telemetrySerializationBuffer,
                                                                      sizeof(telemetrySerializationBuffer));

        if (HERCULES_COMMS__STATUS__SUCCESS != hcStatus)
        {
            DebugComms__tryPrintfToLanderNonblocking("HerculesComms__txResponseMsg failed with error: %d in "
                                                     "RoverStateBase::handleStrokeFromHercules\n",
                                                     hcStatus);
        }

        // HerculesComms__flushTx(theContext.m_hcState);

        return getState();
    }

    RoverState RoverStateBase::handleDownlinkFromHercules(RoverContext &theContext,
                                                          HercMsgs__Header *header,
                                                          uint8_t *payloadBuffer,
                                                          size_t payloadSize)
    {
#pragma diag_push
#pragma diag_suppress 770
        DEBUG_ASSERT_NOT_EQUAL(nullptr, header);
        DEBUG_ASSERT_NOT_EQUAL(nullptr, payloadBuffer);
        DEBUG_ASSERT_NOT_EQUAL(0, payloadSize);
#pragma diag_pop

        // For downlink we first reply to the Hercules, then we send the data to the lander

        // 1) Reply to Hercules
        HerculesComms__Status hcStatus = HerculesComms__txResponseMsg(theContext.m_hcState,
                                                                      header,
                                                                      nullptr,
                                                                      0);

        if (HERCULES_COMMS__STATUS__SUCCESS != hcStatus)
        {
            DebugComms__tryPrintfToLanderNonblocking("HerculesComms__txResponseMsg failed with error: %d in "
                                                     "RoverStateBase::handleDownlinkFromHercules\n",
                                                     hcStatus);
        }

        // 2) Send data to lander
        LanderComms__Status lcStatus = txDownlinkData(theContext, payloadBuffer, payloadSize, true);

        DEBUG_LOG_CHECK_STATUS(LANDER_COMMS__STATUS__SUCCESS, lcStatus, "Downlink failed");

        // LanderComms__flushTx(theContext.m_lcState);

        return getState();
    }

    RoverState RoverStateBase::handleDebugFromHercules(RoverContext &theContext,
                                                       HercMsgs__Header *header,
                                                       uint8_t *payloadBuffer,
                                                       size_t payloadSize)
    {
#pragma diag_push
#pragma diag_suppress 770
        DEBUG_ASSERT_NOT_EQUAL(nullptr, header);
        DEBUG_ASSERT_NOT_EQUAL(nullptr, payloadBuffer);
        DEBUG_ASSERT_NOT_EQUAL(0, payloadSize);
#pragma diag_pop

        // For debug just send it to the lander
        DebugComms__tryStringBufferToLanderNonblocking(payloadBuffer, payloadSize);

        return getState();
    }

    RoverState RoverStateBase::handleResetFromHercules(RoverContext &theContext,
                                                       HercMsgs__Header *header)
    {
#pragma diag_push
#pragma diag_suppress 770
        DEBUG_ASSERT_NOT_EQUAL(nullptr, header);
#pragma diag_pop

        // For Reset_Specific we want to do the reset, then reply to Hercules
        WdCmdMsgs__ResetSpecificId resetValue = (WdCmdMsgs__ResetSpecificId)header->resetValue;

        RoverState result = performResetCommand(theContext, resetValue, nullptr);

        HerculesComms__Status hcStatus = HerculesComms__txResponseMsg(theContext.m_hcState,
                                                                      header,
                                                                      nullptr,
                                                                      0);

        if (HERCULES_COMMS__STATUS__SUCCESS != hcStatus)
        {
            DebugComms__tryPrintfToLanderNonblocking("HerculesComms__txResponseMsg failed with error: %d in "
                                                     "RoverStateBase::handleResetFromHercules\n",
                                                     hcStatus);
        }

        return result;
    }

    RoverState RoverStateBase::pumpMsgsFromLander(RoverContext &theContext)
    {
        m_pumpMsgsFromLanderReturnState = getState();
        CallbackUserArg args(*this, theContext);

        LanderComms__Status lcStatus = LanderComms__tryGetMessage(theContext.m_lcState,
                                                                  RoverStateBase::landerMsgCallback,
                                                                  (void *)&args);

        DEBUG_ASSERT_EQUAL(LANDER_COMMS__STATUS__SUCCESS, lcStatus);
        if (LANDER_COMMS__STATUS__SUCCESS != lcStatus)
        {
            //!< @todo logging?
        }

        return m_pumpMsgsFromLanderReturnState;
    }

    RoverState RoverStateBase::pumpMsgsFromHercules(RoverContext &theContext)
    {
        m_pumpMsgsFromHerculesReturnState = getState();
        CallbackUserArg args(*this, theContext);

        HerculesComms__Status hcStatus = HerculesComms__tryGetMessage(theContext.m_hcState,
                                                                      RoverStateBase::herculesMsgCallback,
                                                                      (void *)&args);

        // DEBUG_ASSERT_EQUAL(HERCULES_COMMS__STATUS__SUCCESS, hcStatus);
        if (HERCULES_COMMS__STATUS__SUCCESS != hcStatus)
        {
            DebugComms__tryPrintfToLanderNonblocking("HerculesComms__tryGetMessage failed with error: %d in "
                                                     "RoverStateBase::pumpMsgsFromHercules\n",
                                                     hcStatus);
        }

        return m_pumpMsgsFromHerculesReturnState;
    }

    void RoverStateBase::sendLanderResponse(RoverContext &theContext, WdCmdMsgs__Response &response)
    {
        static uint8_t responseSerializationBuffer[WD_CMD_MSGS__PACKED_SIZE__RESPONSE_MSG] = {0};

        WdCmdMsgs__Status wdCmdStatus = WdCmdMsgs__serializeGroundResponse(&response,
                                                                           responseSerializationBuffer,
                                                                           sizeof(responseSerializationBuffer));

        DEBUG_ASSERT_EQUAL(WD_CMD_MSGS__STATUS__SUCCESS, wdCmdStatus);

        LanderComms__Status lcStatus = txDownlinkData(theContext,
                                                      responseSerializationBuffer,
                                                      sizeof(responseSerializationBuffer));

        if (LANDER_COMMS__STATUS__SUCCESS != lcStatus)
        {
            //!< @todo handling?
        }
    }

    // Specific watchdog command handling
    RoverState RoverStateBase::doGndCmdResetSpecific(RoverContext &theContext,
                                                     const WdCmdMsgs__Message &msg,
                                                     WdCmdMsgs__Response &response,
                                                     WdCmdMsgs__Response &deployNotificationResponse,
                                                     bool &sendDeployNotificationResponse)
    {
        return performResetCommand(theContext, msg.body.resetSpecific.resetId, &response);
    }

    RoverState RoverStateBase::doGndCmdPrepForDeploy(RoverContext &theContext,
                                                     const WdCmdMsgs__Message &msg,
                                                     WdCmdMsgs__Response &response,
                                                     WdCmdMsgs__Response &deployNotificationResponse,
                                                     bool &sendDeployNotificationResponse)
    {
        /* Default to not being in the right state to transition to mission mode */
        response.statusCode = WD_CMD_MSGS__RESPONSE_STATUS__ERROR_BAD_COMMAND_SEQUENCE;
        return getState();
    }

    RoverState RoverStateBase::doGndCmdDeploy(RoverContext &theContext,
                                              const WdCmdMsgs__Message &msg,
                                              WdCmdMsgs__Response &response,
                                              WdCmdMsgs__Response &deployNotificationResponse,
                                              bool &sendDeployNotificationResponse)
    {
        /* Default to not being in the right state to transition to deploy */
        response.statusCode = WD_CMD_MSGS__RESPONSE_STATUS__ERROR_BAD_COMMAND_SEQUENCE;
        return getState();
    }

    RoverState RoverStateBase::doGndCmdSwitchConnMode(RoverContext &theContext,
                                                      const WdCmdMsgs__Message &msg,
                                                      WdCmdMsgs__Response &response,
                                                      WdCmdMsgs__Response &deployNotificationResponse,
                                                      bool &sendDeployNotificationResponse)
    {
        //!< @todo IMPLEMENT!
        response.statusCode = WD_CMD_MSGS__RESPONSE_STATUS__SUCCESS;
        return getState();
    }

    RoverState RoverStateBase::doGndCmdSetDebugCommsState(RoverContext &theContext,
                                                          const WdCmdMsgs__Message &msg,
                                                          WdCmdMsgs__Response &response,
                                                          WdCmdMsgs__Response &deployNotificationResponse,
                                                          bool &sendDeployNotificationResponse)
    {
        switch (msg.body.setDebugCommsState.selection)
        {
        case WD_CMD_MSGS__DEBUG_COMMS__ON:
            DebugComms__setEnabled(TRUE);
            DebugComms__tryPrintfToLanderNonblocking("Debug comms enabled\n");
            response.statusCode = WD_CMD_MSGS__RESPONSE_STATUS__SUCCESS;
            break;

        case WD_CMD_MSGS__DEBUG_COMMS__OFF:
            DebugComms__tryPrintfToLanderNonblocking("Disabling debug comms\n");
            DebugComms__setEnabled(FALSE);
            response.statusCode = WD_CMD_MSGS__RESPONSE_STATUS__SUCCESS;
            break;

        default:
            response.statusCode = WD_CMD_MSGS__RESPONSE_STATUS__ERROR_BAD_COMMAND_PARAMETER;
        }

        return getState();
    }

    RoverState RoverStateBase::doGndCmdSetAutoHeaterOnValue(RoverContext &theContext,
                                                            const WdCmdMsgs__Message &msg,
                                                            WdCmdMsgs__Response &response,
                                                            WdCmdMsgs__Response &deployNotificationResponse,
                                                            bool &sendDeployNotificationResponse)
    {
        theContext.m_details.m_hParams.m_heaterOnVal = msg.body.setAutoHeaterOnValue.heaterOnValue;
        // Flag that the thresholds have changed:
        theContext.m_details.m_hParams.m_thresholdsChanged = true;
        response.statusCode = WD_CMD_MSGS__RESPONSE_STATUS__SUCCESS;
        return getState();
    }

    RoverState RoverStateBase::doGndCmdSetAutoHeaterOffValue(RoverContext &theContext,
                                                             const WdCmdMsgs__Message &msg,
                                                             WdCmdMsgs__Response &response,
                                                             WdCmdMsgs__Response &deployNotificationResponse,
                                                             bool &sendDeployNotificationResponse)
    {
        theContext.m_details.m_hParams.m_heaterOffVal = msg.body.setAutoHeaterOffValue.heaterOffValue;
        // Flag that the thresholds have changed:
        theContext.m_details.m_hParams.m_thresholdsChanged = true;
        response.statusCode = WD_CMD_MSGS__RESPONSE_STATUS__SUCCESS;
        return getState();
    }

    RoverState RoverStateBase::doGndCmdSetHeaterDutyCycle(RoverContext &theContext,
                                                          const WdCmdMsgs__Message &msg,
                                                          WdCmdMsgs__Response &response,
                                                          WdCmdMsgs__Response &deployNotificationResponse,
                                                          bool &sendDeployNotificationResponse)
    {
        const uint16_t &newDutyCycle = msg.body.setHeaterDutyCycle.dutyCycle;
        // NOTE: TB0CCR0 (max count) is set to m_heaterDutyCyclePeriod-1 (so we need to be < that).
        if (newDutyCycle < theContext.m_details.m_hParams.m_heaterDutyCyclePeriod)
        {
            TB0CCR2 = newDutyCycle;
            theContext.m_details.m_hParams.m_heaterDutyCycle = newDutyCycle;
            response.statusCode = WD_CMD_MSGS__RESPONSE_STATUS__SUCCESS;
        }
        else
        {
            response.statusCode = WD_CMD_MSGS__RESPONSE_STATUS__ERROR_BAD_COMMAND_PARAMETER;
        }
        return getState();
    }

    RoverState RoverStateBase::doGndCmdSetHeaterDutyCyclePeriod(RoverContext &theContext,
                                                                const WdCmdMsgs__Message &msg,
                                                                WdCmdMsgs__Response &response,
                                                                WdCmdMsgs__Response &deployNotificationResponse,
                                                                bool &sendDeployNotificationResponse)
    {
        TB0CCR0 = msg.body.setHeaterDutyCyclePeriod.dutyCyclePeriod;
        theContext.m_details.m_hParams.m_heaterDutyCyclePeriod =
            msg.body.setHeaterDutyCyclePeriod.dutyCyclePeriod;
        response.statusCode = WD_CMD_MSGS__RESPONSE_STATUS__SUCCESS;
        return getState();
    }

    RoverState RoverStateBase::doGndCmdSetVSAEState(RoverContext &theContext,
                                                    const WdCmdMsgs__Message &msg,
                                                    WdCmdMsgs__Response &response,
                                                    WdCmdMsgs__Response &deployNotificationResponse,
                                                    bool &sendDeployNotificationResponse)
    {
        switch (msg.body.setVSAEState.selection)
        {
        case WD_CMD_MSGS__VSAE__ON:
            blimp_vSysAllEnOn();
            response.statusCode = WD_CMD_MSGS__RESPONSE_STATUS__SUCCESS;
            break;

        case WD_CMD_MSGS__VSAE__OFF:
            blimp_vSysAllEnOff();
            response.statusCode = WD_CMD_MSGS__RESPONSE_STATUS__SUCCESS;
            break;

        case WD_CMD_MSGS__VSAE__FORCE_LOW:
            blimp_vSysAllEnForceLow();
            response.statusCode = WD_CMD_MSGS__RESPONSE_STATUS__SUCCESS;
            break;

        default:
            response.statusCode = WD_CMD_MSGS__RESPONSE_STATUS__ERROR_BAD_COMMAND_PARAMETER;
        }

        return getState();
    }

    RoverState RoverStateBase::doGndCmdEnterSleepMode(RoverContext &theContext,
                                                      const WdCmdMsgs__Message &msg,
                                                      WdCmdMsgs__Response &response,
                                                      WdCmdMsgs__Response &deployNotificationResponse,
                                                      bool &sendDeployNotificationResponse)
    {
        // Sleep mode is deprecated so always return that we're in the wrong state to transition to sleep.
        response.statusCode = WD_CMD_MSGS__RESPONSE_STATUS__ERROR_BAD_COMMAND_SEQUENCE;
        return getState();
    }

    RoverState RoverStateBase::doGndCmdEnterKeepAliveMode(RoverContext &theContext,
                                                          const WdCmdMsgs__Message &msg,
                                                          WdCmdMsgs__Response &response,
                                                          WdCmdMsgs__Response &deployNotificationResponse,
                                                          bool &sendDeployNotificationResponse)
    {
        /* Default to not being in the right state to transition to keep alive mode */
        response.statusCode = WD_CMD_MSGS__RESPONSE_STATUS__ERROR_BAD_COMMAND_SEQUENCE;
        return getState();
    }

    RoverState RoverStateBase::doGndCmdEnterServiceMode(RoverContext &theContext,
                                                        const WdCmdMsgs__Message &msg,
                                                        WdCmdMsgs__Response &response,
                                                        WdCmdMsgs__Response &deployNotificationResponse,
                                                        bool &sendDeployNotificationResponse)
    {
        /* Default to not being in the right state to transition to service mode */
        response.statusCode = WD_CMD_MSGS__RESPONSE_STATUS__ERROR_BAD_COMMAND_SEQUENCE;
        return getState();
    }

    RoverState RoverStateBase::doGndCmdDangForceBattState(RoverContext &theContext,
                                                          const WdCmdMsgs__Message &msg,
                                                          WdCmdMsgs__Response &response,
                                                          WdCmdMsgs__Response &deployNotificationResponse,
                                                          bool &sendDeployNotificationResponse)
    {
        switch (msg.body.dangForceBattState.state)
        {
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

    RoverState RoverStateBase::doGndCmdSetChargeEnState(RoverContext &theContext,
                                                        const WdCmdMsgs__Message &msg,
                                                        WdCmdMsgs__Response &response,
                                                        WdCmdMsgs__Response &deployNotificationResponse,
                                                        bool &sendDeployNotificationResponse)
    {
        switch (msg.body.setChargeEnState.selection)
        {
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

    RoverState RoverStateBase::doGndCmdSetChargeRegEnState(RoverContext &theContext,
                                                           const WdCmdMsgs__Message &msg,
                                                           WdCmdMsgs__Response &response,
                                                           WdCmdMsgs__Response &deployNotificationResponse,
                                                           bool &sendDeployNotificationResponse)
    {
        switch (msg.body.setChargeRegEnState.selection)
        {
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

    RoverState RoverStateBase::doGndCmdSetBattEnState(RoverContext &theContext,
                                                      const WdCmdMsgs__Message &msg,
                                                      WdCmdMsgs__Response &response,
                                                      WdCmdMsgs__Response &deployNotificationResponse,
                                                      bool &sendDeployNotificationResponse)
    {
        switch (msg.body.setBattEnState.selection)
        {
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

    RoverState RoverStateBase::doGndCmdSetBattCtrlEnState(RoverContext &theContext,
                                                          const WdCmdMsgs__Message &msg,
                                                          WdCmdMsgs__Response &response,
                                                          WdCmdMsgs__Response &deployNotificationResponse,
                                                          bool &sendDeployNotificationResponse)
    {
        response.statusCode = WD_CMD_MSGS__RESPONSE_STATUS__ERROR_BAD_COMMAND_SEQUENCE;
        return getState();
    }

    RoverState RoverStateBase::doGndCmdSetLatchBattState(RoverContext &theContext,
                                                         const WdCmdMsgs__Message &msg,
                                                         WdCmdMsgs__Response &response,
                                                         WdCmdMsgs__Response &deployNotificationResponse,
                                                         bool &sendDeployNotificationResponse)
    {
        switch (msg.body.setLatchBattState.selection)
        {
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

    RoverState RoverStateBase::doGndCmdLatchSetPulseLow(RoverContext &theContext,
                                                        const WdCmdMsgs__Message &msg,
                                                        WdCmdMsgs__Response &response,
                                                        WdCmdMsgs__Response &deployNotificationResponse,
                                                        bool &sendDeployNotificationResponse)
    {
        switch (msg.body.latchSetPulseLow.selection)
        {
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

    RoverState RoverStateBase::doGndCmdLatchResetPulseLow(RoverContext &theContext,
                                                          const WdCmdMsgs__Message &msg,
                                                          WdCmdMsgs__Response &response,
                                                          WdCmdMsgs__Response &deployNotificationResponse,
                                                          bool &sendDeployNotificationResponse)
    {

        switch (msg.body.latchResetPulseLow.selection)
        {
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

    RoverState RoverStateBase::doGndCmdClearResetMemory(RoverContext &theContext,
                                                        const WdCmdMsgs__Message &msg,
                                                        WdCmdMsgs__Response &response,
                                                        WdCmdMsgs__Response &deployNotificationResponse,
                                                        bool &sendDeployNotificationResponse)
    {
        theContext.m_details.m_resetActionBits = 0;
        response.statusCode = WD_CMD_MSGS__RESPONSE_STATUS__SUCCESS;
        return getState();
    }

    RoverState RoverStateBase::doGndCmdEcho(RoverContext &theContext,
                                            const WdCmdMsgs__Message &msg,
                                            WdCmdMsgs__Response &response,
                                            WdCmdMsgs__Response &deployNotificationResponse,
                                            bool &sendDeployNotificationResponse)
    {
        echoToLander(theContext, msg.body.echo.numBytesToEcho, msg.body.echo.bytesToEcho);
        response.statusCode = WD_CMD_MSGS__RESPONSE_STATUS__SUCCESS;
        return getState();
    }

    RoverState RoverStateBase::doGndCmdRequestDetailedReport(RoverContext &theContext,
                                                             const WdCmdMsgs__Message &msg,
                                                             WdCmdMsgs__Response &response,
                                                             WdCmdMsgs__Response &deployNotificationResponse,
                                                             bool &sendDeployNotificationResponse)
    {
        theContext.m_sendDetailedReport = true;
        response.statusCode = WD_CMD_MSGS__RESPONSE_STATUS__SUCCESS;
        return getState();
    }

    void RoverStateBase::echoToLander(RoverContext &theContext, uint8_t numBytesToEcho, const uint8_t *bytesToEcho)
    {
        /* settings */
        static const char ECHO_HEADER[] = "ECHO: ";
        static const uint8_t ECHO_HEADER_SIZE = 6; // strlen((const char*)ECHO_HEADER);
        static char ECHO_BUFFER[ECHO_HEADER_SIZE + MAX_ECHO_LENGTH];

        memset(ECHO_BUFFER, 0xBE, sizeof(ECHO_BUFFER)); // reset buffer with sentinel
        sprintf(ECHO_BUFFER, ECHO_HEADER);

        // Clip at max echo size (this should have already happened but let's be sure):
        numBytesToEcho = (numBytesToEcho < MAX_ECHO_LENGTH) ? numBytesToEcho : MAX_ECHO_LENGTH;

        /* build echo */
        memcpy(ECHO_BUFFER + ECHO_HEADER_SIZE, bytesToEcho, numBytesToEcho);

        /* send echo */
        LanderComms__Status lcStatus = txDownlinkData(theContext,
                                                      ECHO_BUFFER,
                                                      (ECHO_HEADER_SIZE + numBytesToEcho));

        DEBUG_ASSERT_EQUAL(LANDER_COMMS__STATUS__SUCCESS, lcStatus);
        if (LANDER_COMMS__STATUS__SUCCESS != lcStatus)
        {
            //!< @todo Handling?
            // just an echo so we can probably just ignore it. Ground will just try again.
        }
    }

    void RoverStateBase::sendDetailedReportToLander(RoverContext &theContext, bool sendSupplementalData)
    {
        /* send detailed report */
        static DetailedReport report = {0};
        static uint8_t reportBuffer[sizeof(DetailedReport)] = {0};
        GroundMsgs__Status gcStatus =
            GroundMsgs__generateDetailedReport(&(theContext.m_i2cReadings),
                                               &(theContext.m_adcValues),
                                               &(theContext.m_details),
                                               &report,
                                               reportBuffer);

        DEBUG_ASSERT_EQUAL(GND_MSGS__STATUS__SUCCESS, gcStatus);

        LanderComms__Status lcStatus = txDownlinkData(theContext,
                                                      (uint8_t *)&report,
                                                      sizeof(report));

        DEBUG_ASSERT_EQUAL(LANDER_COMMS__STATUS__SUCCESS, lcStatus);
        if (LANDER_COMMS__STATUS__SUCCESS != lcStatus)
        {
            //!< @todo Handling?
        }

        if (sendSupplementalData)
        {
            // Allowing non-essential extra data to be sent (when requested):
            // Send an extra Flight HB:
            static FlightEarthHeartbeat hb = {0};
            GroundMsgs__Status gcStatus =
                GroundMsgs__generateFlightEarthHeartbeat(&(theContext.m_i2cReadings),
                                                         &(theContext.m_adcValues),
                                                         &(theContext.m_details.m_hParams),
                                                         &hb);

            assert(GND_MSGS__STATUS__SUCCESS == gcStatus);

            LanderComms__Status lcHbStatus = txDownlinkData(theContext,
                                                            (uint8_t *)&hb,
                                                            sizeof(hb));

            // Send a (brief) Safety Timer Status Report:
            // (also incl. the watchdogFlags since issues could be caused by
            // SEUs there):
            if (
                theContext.m_details.m_stateAsUint > static_cast<uint8_t>(RoverState::KEEP_ALIVE))
            {
                // Safety Timer is inactive in KA b/c `watchdog_monitor` is
                // inactive in KA (nothing to WD).
                DebugComms__tryPrintfToLanderNonblocking(
                    "[ST] ON:0x%x \t @: 0x%x/0x%x %d/10 \tWF:0x%x:%x",
                    theContext.m_details.m_safetyTimerParams.timerRebootControlOn,
                    (Time__getTimeInCentiseconds() - theContext.m_details.m_safetyTimerParams.centisecondsAtLastEvent),
                    theContext.m_details.m_safetyTimerParams.timerRebootCutoffCentisecondsTenth,
                    theContext.m_details.m_safetyTimerParams.tenthTimerExpirationCount,
                    (theContext.m_watchdogFlags >> 16) & 0xFFFF,
                    theContext.m_watchdogFlags & 0xFFFF);
            }

            assert(LANDER_COMMS__STATUS__SUCCESS == lcHbStatus);
            if (LANDER_COMMS__STATUS__SUCCESS != lcHbStatus)
            {
                //!< @todo Handling?
            }
        }

#if DEBUG_REPORT
        DebugComms_printfToLander("\n");
        DebugComms__printDataAsHexToLander((uint8_t *)&report, sizeof(DetailedReport), FALSE);
        DebugComms_printfToLander("chargeStat1: %u, ", report.chargeStat1);
        DebugComms_printfToLander("chargeStat2: %u, ", report.chargeStat2);
        DebugComms_printfToLander("battStat: %u, ", report.battStat);
        DebugComms_printfToLander("latchStat: %u, ", report.latchStat);
        DebugComms_printfToLander("pg12: %u, ", report.pg12);
        DebugComms_printfToLander("pg18: %u, ", report.pg18);
        DebugComms_printfToLander("pg33: %u, ", report.pg50);
        DebugComms_printfToLander("state: %u, ", report.state);
        DebugComms_printfToLander("deploymentStatus: %u, ", report.deploymentStatus);
        DebugComms_printfToLander("uart0Initialized: %u, ", report.uart0Initialized);
        DebugComms_printfToLander("uart1Initialized: %u, ", report.uart1Initialized);
        DebugComms_printfToLander("adcBattRT: %u, ", report.adcBattRT);
        DebugComms_printfToLander("sequenceNumber: %u, ", report.sequenceNumber);
        DebugComms_printfToLander("outputPinStateBits: %lu, ", report.outputPinStateBits);
        DebugComms_printfToLander("resetActionBits: %llu, ", theContext.m_details.m_resetActionBits);
        DebugComms_printfToLander("vLanderSense: %u, ", theContext.m_adcValues.vLanderSense);
        DebugComms_printfToLander("battTemp: %u, ", theContext.m_adcValues.battTemp);
        DebugComms_printfToLander("vSysAllSens: %u, ", theContext.m_adcValues.vSysAllSense);
        DebugComms_printfToLander("iSysAllSense: %u, ", theContext.m_adcValues.iSysAllSense);
        DebugComms_printfToLander("vBattSense: %u, ", theContext.m_adcValues.vBattSense);
        DebugComms_printfToLander("vcc24: %u, ", theContext.m_adcValues.vcc24);
        DebugComms_printfToLander("heatingControlEnabled: %u, ", report.heatingControlEnabled);
        DebugComms_printfToLander("heating: %u, ", report.heating);
        DebugComms_printfToLander("vcc2Point5: %u, ", theContext.m_adcValues.vcc2Point5);
        DebugComms_printfToLander("vcc2Point8: %u, ", theContext.m_adcValues.vcc2Point8);
        DebugComms_printfToLander("vcc28: %u, ", theContext.m_adcValues.vcc28);
        DebugComms_printfToLander("kpHeater: %u, ", report.kpHeater);
        DebugComms_printfToLander("heaterPwmLimit: %u, ", report.heaterPwmLimit);
        DebugComms_printfToLander("heaterSetpoint: %u, ", report.heaterSetpoint);
        DebugComms_printfToLander("heaterOnValue: %u, ", report.heaterOnValue);
        DebugComms_printfToLander("heaterOffValue: %u, ", report.heaterOffValue);
        DebugComms_printfToLander("heaterDutyCyclePeriod: %u, ", report.heaterDutyCyclePeriod);
        DebugComms_printfToLander("heaterPwmValue: %u, ", report.heaterPwmValue);
        DebugComms_printfToLander("rawBatteryCharge: %u %u, ", report.rawBatteryCharge[0], report.rawBatteryCharge[1]);
        DebugComms_printfToLander("rawBatteryVoltage: %u %u, ", report.rawBatteryVoltage[0], report.rawBatteryVoltage[1]);
        DebugComms_printfToLander("rawBatteryCurrent: %u %u, ", report.rawBatteryCurrent[0], report.rawBatteryCurrent[1]);
        DebugComms_printfToLander("rawFuelGaugeTemp: %u %u, ", report.rawFuelGaugeTemp[0], report.rawFuelGaugeTemp[1]);
        DebugComms_printfToLander("battChargeTelem: %u, ", report.battChargeTelem);
        DebugComms_printfToLander("battCurrTelem: %u", report.battCurrTelem);
        DebugComms_printfToLander("\n\n");
#endif // DEBUG_REPORT
    }

    void RoverStateBase::doConditionalResetSpecific(RoverContext &theContext,
                                                    WdCmdMsgs__ResetSpecificId resetValue,
                                                    WdCmdMsgs__Response *response,
                                                    bool allowPowerOn,
                                                    bool allowDisableRs422,
                                                    bool allowDeploy,
                                                    bool allowUndeploy,
                                                    bool &writeIoExpander)
    {
        if (nullptr != response)
        {
            // Default to success, change if necessary
            response->statusCode = WD_CMD_MSGS__RESPONSE_STATUS__SUCCESS;
        }

        writeIoExpander = false;

        switch (resetValue)
        {
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
            if (allowPowerOn)
            {
                powerOnHercules();
                SET_RABI_IN_UINT(theContext.m_details.m_resetActionBits, RABI__HERCULES_POWER_ON);
            }
            else if (nullptr != response)
            {
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
            if (allowPowerOn)
            {
                powerOnRadio();
                writeIoExpander = true;
                SET_RABI_IN_UINT(theContext.m_details.m_resetActionBits, RABI__RADIO_POWER_ON);
            }
            else if (nullptr != response)
            {
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
            // TOGGLE the FPGA reset state. Toggling rather than unresetting supports FPGA programming.
            if (toggleFPGAReset())
            {
                // FPGA will be put INTO reset. Log the event:
                SET_RABI_IN_UINT(theContext.m_details.m_resetActionBits, RABI__CAM_FPGA_RESET);
                DebugComms__tryPrintfToLanderNonblocking("FPGA being RESET\n");
            }
            else
            {
                // FPGA will be RELEASED from reset. Log the event:
                SET_RABI_IN_UINT(theContext.m_details.m_resetActionBits, RABI__CAM_FPGA_UNRESET);
                DebugComms__tryPrintfToLanderNonblocking("FPGA being UNRESET\n");
            }
            writeIoExpander = true;

            // Old reset, unreset impl:
            //            setFPGAReset();
            //            // queue up the fpga unreset
            //            theContext.m_watchdogFlags |= WDFLAG_UNRESET_FPGA;
            //            SET_RABI_IN_UINT(theContext.m_details.m_resetActionBits, RABI__CAM_FPGA_RESET);
            break;

        case WD_CMD_MSGS__RESET_ID__CAM_FPGA_POWER_ON:
            if (allowPowerOn)
            {
                powerOnFpga();
                SET_RABI_IN_UINT(theContext.m_details.m_resetActionBits, RABI__CAM_FPGA_POWER_ON);
            }
            else if (nullptr != response)
            {
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

        case WD_CMD_MSGS__RESET_ID__MOTOR_1_RESET_HOLD:
            setMotor1Reset();
            writeIoExpander = true;
            // just don't queue up an unreset
            break;

        case WD_CMD_MSGS__RESET_ID__MOTOR_2_RESET_HOLD:
            setMotor2Reset();
            writeIoExpander = true;
            // just don't queue up an unreset
            break;

        case WD_CMD_MSGS__RESET_ID__MOTOR_3_RESET_HOLD:
            setMotor3Reset();
            writeIoExpander = true;
            // just don't queue up an unreset
            break;

        case WD_CMD_MSGS__RESET_ID__MOTOR_4_RESET_HOLD:
            setMotor4Reset();
            writeIoExpander = true;
            // just don't queue up an unreset
            break;

        case WD_CMD_MSGS__RESET_ID__MOTOR_1_RESET_RELEASE:
            releaseMotor1Reset();
            writeIoExpander = true;
            break;

        case WD_CMD_MSGS__RESET_ID__MOTOR_2_RESET_RELEASE:
            releaseMotor2Reset();
            writeIoExpander = true;
            break;

        case WD_CMD_MSGS__RESET_ID__MOTOR_3_RESET_RELEASE:
            releaseMotor3Reset();
            writeIoExpander = true;
            break;

        case WD_CMD_MSGS__RESET_ID__MOTOR_4_RESET_RELEASE:
            releaseMotor4Reset();
            writeIoExpander = true;
            break;

        case WD_CMD_MSGS__RESET_ID__RESET_HOLD_MOTORS_LEFT:
            setMotor1Reset();
            setMotor4Reset();
            writeIoExpander = true;
            break;

        case WD_CMD_MSGS__RESET_ID__RESET_HOLD_MOTORS_RIGHT:
            setMotor2Reset();
            setMotor3Reset();
            writeIoExpander = true;
            break;

        case WD_CMD_MSGS__RESET_ID__RESET_HOLD_MOTORS_FRONT:
            setMotor1Reset();
            setMotor2Reset();
            writeIoExpander = true;
            break;

        case WD_CMD_MSGS__RESET_ID__RESET_HOLD_MOTORS_REAR:
            setMotor3Reset();
            setMotor4Reset();
            writeIoExpander = true;
            break;

        case WD_CMD_MSGS__RESET_ID__RESET_HOLD_MOTORS_DIAG_AC:
            setMotor1Reset();
            setMotor3Reset();
            writeIoExpander = true;
            break;

        case WD_CMD_MSGS__RESET_ID__RESET_HOLD_MOTORS_DIAG_DB:
            setMotor4Reset();
            setMotor2Reset();
            writeIoExpander = true;
            break;

        case WD_CMD_MSGS__RESET_ID__RESET_RELEASE_MOTORS_LEFT:
            releaseMotor1Reset();
            releaseMotor4Reset();
            writeIoExpander = true;
            break;

        case WD_CMD_MSGS__RESET_ID__RESET_RELEASE_MOTORS_RIGHT:
            releaseMotor2Reset();
            releaseMotor3Reset();
            writeIoExpander = true;
            break;

        case WD_CMD_MSGS__RESET_ID__RESET_RELEASE_MOTORS_FRONT:
            releaseMotor1Reset();
            releaseMotor2Reset();
            writeIoExpander = true;
            break;

        case WD_CMD_MSGS__RESET_ID__RESET_RELEASE_MOTORS_REAR:
            releaseMotor3Reset();
            releaseMotor4Reset();
            writeIoExpander = true;
            break;

        case WD_CMD_MSGS__RESET_ID__RESET_RELEASE_MOTORS_DIAG_AC:
            releaseMotor1Reset();
            releaseMotor3Reset();
            writeIoExpander = true;
            break;

        case WD_CMD_MSGS__RESET_ID__RESET_RELEASE_MOTORS_DIAG_DB:
            releaseMotor4Reset();
            releaseMotor2Reset();
            writeIoExpander = true;
            break;

        case WD_CMD_MSGS__RESET_ID__RESET_HOLD_ALL_MOTORS:
            setMotorsReset();
            writeIoExpander = true;
            // just don't queue up an unreset
            break;

        case WD_CMD_MSGS__RESET_ID__ALL_MOTORS_RESET:
            setMotorsReset();
            writeIoExpander = true;
            // queue up all the motor unresets
            theContext.m_watchdogFlags |= WDFLAG_UNRESET_MOTOR1 | WDFLAG_UNRESET_MOTOR2 |
                                          WDFLAG_UNRESET_MOTOR3 | WDFLAG_UNRESET_MOTOR4;
            break;

        case WD_CMD_MSGS__RESET_ID__ALL_MOTORS_POWER_ON:
            if (allowPowerOn)
            {
                powerOnMotors();
                SET_RABI_IN_UINT(theContext.m_details.m_resetActionBits, RABI__ALL_MOTORS_POWER_ON);
            }
            else if (nullptr != response)
            {
                response->statusCode = WD_CMD_MSGS__RESPONSE_STATUS__ERROR_BAD_COMMAND_SEQUENCE;
            }
            break;

        case WD_CMD_MSGS__RESET_ID__ALL_MOTORS_POWER_OFF:
            powerOffMotors();
            SET_RABI_IN_UINT(theContext.m_details.m_resetActionBits, RABI__ALL_MOTORS_POWER_OFF);
            break;

        case WD_CMD_MSGS__RESET_ID__3_3V_EN_RESET:
            if (allowPowerOn)
            {
                disable3V3PowerRail();
                // queue up 3V3 rail on again
                theContext.m_watchdogFlags |= WDFLAG_UNRESET_3V3;
                SET_RABI_IN_UINT(theContext.m_details.m_resetActionBits, RABI__3V3_EN_RESET);
            }
            else if (nullptr != response)
            {
                response->statusCode = WD_CMD_MSGS__RESPONSE_STATUS__ERROR_BAD_COMMAND_SEQUENCE;
            }
            break;

        case WD_CMD_MSGS__RESET_ID__3_3V_EN_POWER_ON:
            if (allowPowerOn)
            {
                blimp_vSysAllEnOn();
                enable3V3PowerRail();
                SET_RABI_IN_UINT(theContext.m_details.m_resetActionBits, RABI__3V3_EN_POWER_ON);
            }
            else if (nullptr != response)
            {
                response->statusCode = WD_CMD_MSGS__RESPONSE_STATUS__ERROR_BAD_COMMAND_SEQUENCE;
            }
            break;

        case WD_CMD_MSGS__RESET_ID__3_3V_EN_POWER_OFF:
            disable3V3PowerRail();
            SET_RABI_IN_UINT(theContext.m_details.m_resetActionBits, RABI__3V3_EN_POWER_OFF);
            break;

        case WD_CMD_MSGS__RESET_ID__SAFETY_TIMER_REBOOT_CTRL_ON:
            theContext.m_details.m_safetyTimerParams.timerRebootControlOn = SAFETY_TIMER__REBOOT_CONTROL_ON;
            DPRINTF("SAFETY TIMER REBOOT CONTROL: ON.");
            break;
        case WD_CMD_MSGS__RESET_ID__SAFETY_TIMER_REBOOT_CTRL_OFF:
            theContext.m_details.m_safetyTimerParams.timerRebootControlOn = SAFETY_TIMER__REBOOT_CONTROL_OFF;
            DPRINTF("SAFETY TIMER REBOOT CONTROL: OFF.");
            break;
        case WD_CMD_MSGS__RESET_ID__SAFETY_TIMER_ACK:
            // Just set a KICK flag here. Timer reset happens in the next state update in watchdog process:
            theContext.m_watchdogFlags |= WDFLAG_SAFETY_TIMER_KICK;
            DPRINTF("SAFETY TIMER: ACK received.");
            break;
        case WD_CMD_MSGS__RESET_ID__SAFETY_TIMER_CUTOFF_INC:
            // Treat this as a kick (and reset the timer):
            theContext.m_watchdogFlags |= WDFLAG_SAFETY_TIMER_KICK;
            if ((SAFETY_TIMER__CUTOFF_TENTH_MAX_VAL_CS - theContext.m_details.m_safetyTimerParams.timerRebootCutoffCentisecondsTenth) < SAFETY_TIMER__CUTOFF_TENTH_INCREMENT_CS)
            {
                // Inc will put this over max val. Stopping here.
                DPRINTF(
                    "SAFETY TIMER: Cutoff NOT inc. Max would be exceeded. Val is: 0x%x*%d cs.",
                    theContext.m_details.m_safetyTimerParams.timerRebootCutoffCentisecondsTenth,
                    SAFETY_TIMER__TENTH_TIMER_EXPIRATION_COUNT_TRIGGER);
            }
            else
            {
                theContext.m_details.m_safetyTimerParams.timerRebootCutoffCentisecondsTenth += SAFETY_TIMER__CUTOFF_TENTH_INCREMENT_CS;
                DPRINTF(
                    "SAFETY TIMER: Cutoff inc to: 0x%x*%d cs.",
                    theContext.m_details.m_safetyTimerParams.timerRebootCutoffCentisecondsTenth,
                    SAFETY_TIMER__TENTH_TIMER_EXPIRATION_COUNT_TRIGGER);
            }
            break;
        case WD_CMD_MSGS__RESET_ID__SAFETY_TIMER_CUTOFF_DEC:
            // Treat this as a kick (and reset the timer):
            theContext.m_watchdogFlags |= WDFLAG_SAFETY_TIMER_KICK;
            if ((theContext.m_details.m_safetyTimerParams.timerRebootCutoffCentisecondsTenth - SAFETY_TIMER__CUTOFF_TENTH_MIN_VAL_CS) < SAFETY_TIMER__CUTOFF_TENTH_INCREMENT_CS)
            {
                // Inc will put this over max val. Stopping here.
                DPRINTF(
                    "SAFETY TIMER: Cutoff NOT dec. Min would be exceeded. Val is: 0x%x*%d cs.",
                    theContext.m_details.m_safetyTimerParams.timerRebootCutoffCentisecondsTenth,
                    SAFETY_TIMER__TENTH_TIMER_EXPIRATION_COUNT_TRIGGER);
            }
            else
            {
                theContext.m_details.m_safetyTimerParams.timerRebootCutoffCentisecondsTenth -= SAFETY_TIMER__CUTOFF_TENTH_INCREMENT_CS;
                DPRINTF(
                    "SAFETY TIMER: Cutoff dec to: 0x%x*%d cs.",
                    theContext.m_details.m_safetyTimerParams.timerRebootCutoffCentisecondsTenth,
                    SAFETY_TIMER__TENTH_TIMER_EXPIRATION_COUNT_TRIGGER);
            }
            break;

        case WD_CMD_MSGS__RESET_ID__V_SYS_ALL_POWER_CYCLE:
            if (allowPowerOn)
            {
                disableVSysAllPowerRail();
                // queue up VSA rail on again
                theContext.m_watchdogFlags |= WDFLAG_POWER_ON_V_SYS_ALL;
                SET_RABI_IN_UINT(theContext.m_details.m_resetActionBits, RABI__V_SYS_ALL_OFF__RESET);
            }
            else if (nullptr != response)
            {
                response->statusCode = WD_CMD_MSGS__RESPONSE_STATUS__ERROR_BAD_COMMAND_SEQUENCE;
            }
            break;

        case WD_CMD_MSGS__RESET_ID__V_SYS_ALL_ON:
            if (allowPowerOn)
            {
                enableVSysAllPowerRail();
                SET_RABI_IN_UINT(theContext.m_details.m_resetActionBits, RABI__V_SYS_ALL_POWER_ON);
            }
            else if (nullptr != response)
            {
                response->statusCode = WD_CMD_MSGS__RESPONSE_STATUS__ERROR_BAD_COMMAND_SEQUENCE;
            }
            break;

        case WD_CMD_MSGS__RESET_ID__V_SYS_ALL_OFF:
            disableVSysAllPowerRail();
            SET_RABI_IN_UINT(theContext.m_details.m_resetActionBits, RABI__V_SYS_ALL_POWER_OFF);
            break;

        case WD_CMD_MSGS__RESET_ID__HDRM_DEPLOY_SIGNAL_POWER_OFF:
            if (allowUndeploy)
            {
                unsetDeploy();
                SET_RABI_IN_UINT(theContext.m_details.m_resetActionBits, RABI__HDRM_DEPLOY_SIGNAL_POWER_OFF);
            }
            else if (nullptr != response)
            {
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
            if (allowDisableRs422)
            {
                //!< @todo IMPLEMENT
                SET_RABI_IN_UINT(theContext.m_details.m_resetActionBits, RABI__RS422_UART_DISABLE);
            }
            else if (nullptr != response)
            {
                response->statusCode = WD_CMD_MSGS__RESPONSE_STATUS__ERROR_BAD_COMMAND_SEQUENCE;
            }
            break;

        case WD_CMD_MSGS__RESET_ID__AUTO_HEATER_CONTROLLER_ENABLE:
            theContext.m_details.m_hParams.m_heatingControlEnabled = true;
            enableHeater();
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

        case WD_CMD_MSGS__RESET_ID__HEATER_FORCE_OFF:
            theContext.m_details.m_hParams.m_forceState = HEATER_FORCE_ALWAYS_OFF;
            disableHeater();
            break;
        case WD_CMD_MSGS__RESET_ID__HEATER_FORCE_ON:
            theContext.m_details.m_hParams.m_forceState = HEATER_FORCE_ALWAYS_ON;
            enableHeater();
            break;
        case WD_CMD_MSGS__RESET_ID__HEATER_FORCE_NOTHING:
            theContext.m_details.m_hParams.m_forceState = HEATER_FORCE_NOTHING;
            break;
        case WD_CMD_MSGS__RESET_ID__AUTO_HEATER_CONTROLLER_USE_RT_INPUT:
            theContext.m_details.m_hParams.m_inputSource = HEATER_CONTROL_INPUT_BATT_RT;
            break;
        case WD_CMD_MSGS__RESET_ID__AUTO_HEATER_CONTROLLER_USE_CHARGER_INPUT_DEACTIVATE:
            // Disable charger circuitry so we can return to normal:
            blimp_regEnOff();
            blimp_chargerEnOff();
            blimp_battEnOff();
            break;
        case WD_CMD_MSGS__RESET_ID__AUTO_HEATER_CONTROLLER_USE_CHARGER_INPUT_ACTIVATE:
            // Assert that charging should not happen:
            blimp_chargerEnOff();
            // Turn on (but don't latch) the batteries (might be unsafe to turn
            // on charger without the batteries connected, even with CE=OFF...
            // need to test on SBC with BLiMP to determine).
            // NOTE: So long as lander power is supplied, this shouldn't do
            // anything since everything will be powered off lander power before
            // battery power and since the batteries won't be latched, once
            // lander power is removed, they should power back off.
            blimp_battEnOn();
            // Then power up the charging regulator so the charging IC can
            // create the VREF used by the charging TS:
            // (unfortunately we can't just use an internal pullup with the ADC
            // on the MSP430 so we have to do this this way)
            blimp_regEnOn();
            // NOTE: Even if CE=OFF fails for some reason, if the batteries
            // were to remain disconnected, charging wouldn't be able to
            // happen (and this open circuit shouldn't cause an issue for the
            // battery charging IC b/c it has over-voltage protection to handle
            // a battery/load disconnect case).
            break;
        case WD_CMD_MSGS__RESET_ID__AUTO_HEATER_CONTROLLER_USE_CHARGER_INPUT:
            // Just set the input source, don't change the circuitry (this might
            // be desirable and lets us arm before actually messing with the circuitry):
            theContext.m_details.m_hParams.m_inputSource = HEATER_CONTROL_INPUT_CHARGER;
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
            if (allowPowerOn)
            {
                enableBatteries();
                SET_RABI_IN_UINT(theContext.m_details.m_resetActionBits, RABI__BATTERIES_ENABLE);
            }
            else if (nullptr != response)
            {
                response->statusCode = WD_CMD_MSGS__RESPONSE_STATUS__ERROR_BAD_COMMAND_SEQUENCE;
            }
            break;

        case WD_CMD_MSGS__RESET_ID__BATTERIES_DISABLE:
            disableBatteries();
            SET_RABI_IN_UINT(theContext.m_details.m_resetActionBits, RABI__BATTERIES_DISABLE);
            break;

        case WD_CMD_MSGS__RESET_ID__CLEAR_PERSISTENT_DEPLOY:
            *(theContext.m_persistentDeployed) = false;
            break;

        case WD_CMD_MSGS__RESET_ID__HDRM_DEPLOY_SIGNAL_POWER_ON:
            if (allowPowerOn && allowDeploy)
            {
                /* WOOT WOOT! WE ARE ON TO THE MOON, FOLKS */
                /* ref: https://docs.google.com/document/d/1dKLlBcIIVo8t1bGu3jNiHobGMavA3I2al0cncj3ZAhE/edit */
                setDeploy();
                SET_RABI_IN_UINT(theContext.m_details.m_resetActionBits, RABI__HDRM_DEPLOY_SIGNAL_POWER_ON);
            }
            else if (nullptr != response)
            {
                response->statusCode = WD_CMD_MSGS__RESPONSE_STATUS__ERROR_BAD_COMMAND_SEQUENCE;
            }
            break;

        default:
            /* invalid command */
            if (nullptr != response)
            {
                response->statusCode = WD_CMD_MSGS__RESPONSE_STATUS__ERROR_BAD_COMMAND_PARAMETER;
                // If we can respond indicating there was a bad parameter, then return
                // that this function finished "successfully," since we have a valid way
                // of communicating something didn't work
            }
        }

        // Report what we did (and under what conditions):
        uint16_t resetConditions = allowPowerOn;
        resetConditions = (resetConditions << 1) & allowDisableRs422;
        resetConditions = (resetConditions << 1) & allowDeploy;
        resetConditions = (resetConditions << 1) & allowUndeploy;
        DebugComms__tryPrintfToLanderNonblocking("RESET:%d -> %d with 0x%x\n", int(resetValue), int(response->statusCode), resetConditions);
    }

} // End namespace iris
