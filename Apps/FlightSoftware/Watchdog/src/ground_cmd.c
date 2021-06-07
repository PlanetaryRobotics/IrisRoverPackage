/*
 * ground_cmd.c
 *
 * Handle commands sent from the ground up to the watchdog
 *
 *  Created on: Nov. 29, 2020
 *      Author: ohnx
 */
#include <stdint.h>

#include "include/comms/ip_udp.h"
#include "include/comms/i2c_sensors.h"
#include "include/drivers/adc.h"
#include "include/drivers/bsp.h"
#include "include/drivers/uart.h"
#include "include/flags.h"
#include "include/ground_cmd.h"
#include "include/utils/serialization.h"
#include "include/watchdog.h"

uint8_t watchdog_opts = 0;
// variables for heater control
//#pragma PERSISTENT(Kp_heater)
//#pragma PERSISTENT(PWM_limit)
//#pragma PERSISTENT(heater_setpoint)
//#pragma PERSISTENT(heater_window)
//#pragma PERSISTENT(heater_on_val)
//#pragma PERSISTENT(heater_off_val)
uint16_t Kp_heater = 500, PWM_limit = 0, heater_setpoint = 3325, heater_window = 60;
uint16_t heater_on_val = 3670;  // -5 C thermistor voltage ADC reading
uint16_t heater_off_val = 3352; // 0 C thermistor voltage ADC reading
uint8_t heating = 0;
uint8_t heatingControlEnabled = 1;

GroundCmd__Status GroundCmd__performResetCommand(WdCmdMsgs__ResetSpecificId resetValue,
                                                 WdCmdMsgs__Response* response)
{
    if (NULL != response) {
        // Default to success, change if necessary
        response->statusCode = WD_CMD_MSGS__RESPONSE_STATUS__SUCCESS;
    }

    switch (resetValue) {
        case WD_CMD_MSGS__RESET_ID__NO_RESET:
            break;

        case WD_CMD_MSGS__RESET_ID__HERCULES_RESET:
            setHerculesReset();
            // queue up hercules unreset
            watchdog_flags |= WDFLAG_UNRESET_HERCULES;
            break;

        case WD_CMD_MSGS__RESET_ID__HERCULES_POWER_ON:
            powerOnHercules();
            break;

        case WD_CMD_MSGS__RESET_ID__HERCULES_POWER_OFF:
            powerOffHercules();
            break;

        case WD_CMD_MSGS__RESET_ID__RADIO_RESET:
            setRadioReset();
            // queue up an radio unreset
            watchdog_flags |= WDFLAG_UNRESET_RADIO1;
            break;

        case WD_CMD_MSGS__RESET_ID__RADIO_POWER_ON:
            powerOnRadio();
            break;

        case WD_CMD_MSGS__RESET_ID__RADIO_POWER_OFF:
            powerOffRadio();
            break;

        case WD_CMD_MSGS__RESET_ID__CAM_FPGA_RESET:
            setFPGAReset();
            // queue up the fpga unreset
            watchdog_flags |= WDFLAG_UNRESET_FPGA;
            break;

        case WD_CMD_MSGS__RESET_ID__CAM_FPGA_POWER_ON:
            powerOnFpga();
            break;

        case WD_CMD_MSGS__RESET_ID__CAM_FPGA_POWER_OFF:
            powerOffFpga();
            break;

        case WD_CMD_MSGS__RESET_ID__MOTOR_1_RESET:
            setMotor1Reset();
            // queue up the motor 1 unreset
            watchdog_flags |= WDFLAG_UNRESET_MOTOR1;
            break;

        case WD_CMD_MSGS__RESET_ID__MOTOR_2_RESET:
            setMotor2Reset();
            // queue up the motor 2 unreset
            watchdog_flags |= WDFLAG_UNRESET_MOTOR2;
            break;

        case WD_CMD_MSGS__RESET_ID__MOTOR_3_RESET:
            setMotor3Reset();
            // queue up the motor 3 unreset
            watchdog_flags |= WDFLAG_UNRESET_MOTOR3;
            break;

        case WD_CMD_MSGS__RESET_ID__MOTOR_4_RESET:
            setMotor4Reset();
            // queue up the motor 4 unreset
            watchdog_flags |= WDFLAG_UNRESET_MOTOR4;
            break;

        case WD_CMD_MSGS__RESET_ID__ALL_MOTORS_RESET:
            setMotorsReset();
            // queue up all the motor unresets
            watchdog_flags |= WDFLAG_UNRESET_MOTOR1 | WDFLAG_UNRESET_MOTOR2 |
                              WDFLAG_UNRESET_MOTOR3 | WDFLAG_UNRESET_MOTOR4;
            break;

        case WD_CMD_MSGS__RESET_ID__ALL_MOTORS_POWER_ON:
            powerOnMotors();
            break;

        case WD_CMD_MSGS__RESET_ID__ALL_MOTORS_POWER_OFF:
            powerOffMotors();
            break;

        case WD_CMD_MSGS__RESET_ID__3_3V_EN_RESET:
            disable3V3PowerRail();
            // queue up 3V3 rail on again
            watchdog_flags |= WDFLAG_UNRESET_3V3;
            break;

        case WD_CMD_MSGS__RESET_ID__3_3V_EN_POWER_ON:
            enable3V3PowerRail();
            break;

        case WD_CMD_MSGS__RESET_ID__3_3V_EN_POWER_OFF:
            disable3V3PowerRail();
            break;

        case WD_CMD_MSGS__RESET_ID__24V_EN_RESET:
            disable24VPowerRail();
            // queue up 24V rail on again
            watchdog_flags |= WDFLAG_UNRESET_24V;
            break;

        case WD_CMD_MSGS__RESET_ID__24V_EN_POWER_ON:
            enable24VPowerRail();
            break;

        case WD_CMD_MSGS__RESET_ID__24V_EN_POWER_OFF:
            disable24VPowerRail();
            break;

        case WD_CMD_MSGS__RESET_ID__HDRM_DEPLOY_SIGNAL_POWER_OFF:
            unsetDeploy();
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
            //!< @todo IMPLEMENT
            break;

        case WD_CMD_MSGS__RESET_ID__AUTO_HEATER_CONTROLLER_ENABLE:
            heatingControlEnabled = 1;
            break;

        case WD_CMD_MSGS__RESET_ID__AUTO_HEATER_CONTROLLER_DISABLE:
            heatingControlEnabled = 0;
            TB0CCR2 = 0;
            break;

        case WD_CMD_MSGS__RESET_ID__HERCULES_WATCHDOG_ENABLE:
            watchdog_opts |= WDFLAG_UNRESET_HERCULES;
            break;

        case WD_CMD_MSGS__RESET_ID__HERCULES_WATCHDOG_DISABLE:
            watchdog_opts &= ~WDFLAG_UNRESET_HERCULES;
            break;

        case WD_CMD_MSGS__RESET_ID__BATTERIES_ENABLE:
            disableBatteries();
            break;

        case WD_CMD_MSGS__RESET_ID__BATTERIES_DISABLE:
            enableBatteries();
            break;

        case WD_CMD_MSGS__RESET_ID__HDRM_DEPLOY_SIGNAL_POWER_ON:
            /* WOOT WOOT! WE ARE ON TO THE MOON, FOLKS */
            /* ref: https://docs.google.com/document/d/1dKLlBcIIVo8t1bGu3jNiHobGMavA3I2al0cncj3ZAhE/edit */
            setDeploy();
            break;

        default:
            /* invalid command */
            if (NULL != response) {
                response->statusCode = WD_CMD_MSGS__RESPONSE_STATUS__ERROR_BAD_COMMAND_PARAMETER;
                // If we can respond indicating there was a bad parameter, then return
                // that this function finished "successfully," since we have a valid way
                // of communicating something didn't work
            } else {
                return GND_CMD__STATUS__ERROR_UNKNOWN_RESET_VALUE;
            }
    }

    /* all ok */
    return GND_CMD__STATUS__SUCCESS;
}

GroundCmd__Status GroundCmd__performWatchdogCommand(const WdCmdMsgs__Message* msg,
                                                    WdCmdMsgs__Response* response,
                                                    WdCmdMsgs__Response* deployNotificationResponse,
                                                    BOOL* sendDeployNotificationResponse)
{
    static WdCmdMsgs__CommandId lastCommand = WD_CMD_MSGS__CMD_ID__RESET_SPECIFIC;

    if (NULL == msg 
        || NULL == response
        || NULL == deployNotificationResponse
        || NULL == sendDeployNotificationResponse) {
        return GND_CMD__STATUS__ERROR_NULL;
    }

    // Make sure that by default we don't want to send the deploy notification response
    *sendDeployNotificationResponse = FALSE;

    // We always want to set the magic number and command ID (to which this is a response) on the response
    response->magicNumber = WD_CMD_MSGS__RESPONSE_MAGIC_NUMBER;
    response->commandId = msg->commandId;

    // We always want to update our last command
    lastCommand = msg->commandId;

    switch (msg->commandId) {
        case WD_CMD_MSGS__CMD_ID__RESET_SPECIFIC:
            return GroundCmd__performResetCommand(msg->body.resetSpecific.resetId, response);

        case WD_CMD_MSGS__CMD_ID__PREP_FOR_DEPLOY:
            /* power on all systems */
            if (msg->body.prepForDeploy.confirmationMagicNumber != WD_CMD_MSGS__CONFIRM_DEPLOYMENT_MAGIC_NUMBER) {
                /* magic bad */
                response->statusCode = WD_CMD_MSGS__RESPONSE_STATUS__ERROR_BAD_COMMAND_PARAMETER;
            } else if (rovstate != RS_SERVICE) {
                /* not in the right state to transition to mission mode */
                response->statusCode = WD_CMD_MSGS__RESPONSE_STATUS__ERROR_BAD_COMMAND_SEQUENCE;
            } else {
                /* all checks pass, enter mission mode */
                enterMode(RS_MISSION);
                response->statusCode = WD_CMD_MSGS__RESPONSE_STATUS__SUCCESS;
            }
            break;

        case WD_CMD_MSGS__CMD_ID__DEPLOY:
            if (msg->body.prepForDeploy.confirmationMagicNumber != WD_CMD_MSGS__CONFIRM_DEPLOYMENT_MAGIC_NUMBER) {
                /* magic bad */
                response->statusCode = WD_CMD_MSGS__RESPONSE_STATUS__ERROR_BAD_COMMAND_PARAMETER;
            } else {
                //  disengage from lander (deploy) - watchdog only
                setDeploy();

                // We want to send two response: one to say we deployed, and another to say that this command
                // was performed successfully.
                deployNotificationResponse->magicNumber = WD_CMD_MSGS__RESPONSE_MAGIC_NUMBER;
                deployNotificationResponse->commandId = msg->commandId;
                deployNotificationResponse->statusCode = WD_CMD_MSGS__RESPONSE_STATUS__DEPLOY;
                *sendDeployNotificationResponse = TRUE;

                response->statusCode = WD_CMD_MSGS__RESPONSE_STATUS__SUCCESS;
            }
            break;

        case WD_CMD_MSGS__CMD_ID__SWITCH_CONN_MODE:
            //!< @todo IMPLEMENT!
            response->statusCode = WD_CMD_MSGS__RESPONSE_STATUS__SUCCESS;
            break;

        case WD_CMD_MSGS__CMD_ID__SET_HEATER_KP:
            //!< @todo Make heater control not through globals
            Kp_heater = msg->body.setHeaterKp.kp;
            response->statusCode = WD_CMD_MSGS__RESPONSE_STATUS__SUCCESS;
            break;

        case WD_CMD_MSGS__CMD_ID__SET_AUTO_HEATER_ON_VALUE:
            //!< @todo Make heater control not through globals
            heater_on_val = msg->body.setAutoHeaterOnValue.heaterOnValue;
            response->statusCode = WD_CMD_MSGS__RESPONSE_STATUS__SUCCESS;
            break;

        case WD_CMD_MSGS__CMD_ID__SET_AUTO_HEATER_OFF_VALUE:
            //!< @todo Make heater control not through globals
            heater_off_val = msg->body.setAutoHeaterOffValue.heaterOffValue;
            response->statusCode = WD_CMD_MSGS__RESPONSE_STATUS__SUCCESS;
            break;

        case WD_CMD_MSGS__CMD_ID__SET_HEATER_DUTY_CYCLE_MAX:
            //!< @todo Make heater control not through globals
            PWM_limit = msg->body.setHeaterDutyCycleMax.dutyCycleMax;
            response->statusCode = WD_CMD_MSGS__RESPONSE_STATUS__SUCCESS;
            break;

        case WD_CMD_MSGS__CMD_ID__SET_HEATER_DUTY_CYCLE_PERIOD:
            //!< @todo Make heater control not through globals
            TB0CCR0 = msg->body.setHeaterDutyCyclePeriod.dutyCyclePeriod;
            response->statusCode = WD_CMD_MSGS__RESPONSE_STATUS__SUCCESS;
            break;

        case WD_CMD_MSGS__CMD_ID__SET_THERMISTER_V_SETPOINT:    
            //!< @todo Make heater control not through globals
            heater_setpoint = msg->body.setThermisterVSetpoint.thermisterVSetpoint;
            response->statusCode = WD_CMD_MSGS__RESPONSE_STATUS__SUCCESS;
            break;

        case WD_CMD_MSGS__CMD_ID__ENTER_SLEEP_MODE:
            /* Enter sleep mode */
            if (msg->body.enterSleepMode.confirmationMagicNumber != WD_CMD_MSGS__CONFIRM_MODE_CHANGE_MAGIC_NUMBER) {
                /* magic bad */
                response->statusCode = WD_CMD_MSGS__RESPONSE_STATUS__ERROR_BAD_COMMAND_PARAMETER;
            } else if (rovstate != RS_KEEPALIVE) {
                /* not in the right state to transition to sleep mode */
                response->statusCode = WD_CMD_MSGS__RESPONSE_STATUS__ERROR_BAD_COMMAND_SEQUENCE;
            } else {
                /* all checks pass, enter sleep mode */
                enterMode(RS_SLEEP);
                response->statusCode = WD_CMD_MSGS__RESPONSE_STATUS__SUCCESS;
            }
            break;

        case WD_CMD_MSGS__CMD_ID__ENTER_KEEPALIVE_MODE:
            /* Enter keepalive mode */
            if (msg->body.enterKeepAliveMode.confirmationMagicNumber != WD_CMD_MSGS__CONFIRM_MODE_CHANGE_MAGIC_NUMBER) {
                /* magic bad */
                response->statusCode = WD_CMD_MSGS__RESPONSE_STATUS__ERROR_BAD_COMMAND_PARAMETER;
            } else if (rovstate != RS_SLEEP || rovstate != RS_SERVICE) {
                /* not in the right state to transition to keepalive mode */
                response->statusCode = WD_CMD_MSGS__RESPONSE_STATUS__ERROR_BAD_COMMAND_SEQUENCE;
            } else {
                /* all checks pass, enter keepalive mode */
                enterMode(RS_KEEPALIVE);
                response->statusCode = WD_CMD_MSGS__RESPONSE_STATUS__SUCCESS;
            }
            break;

        case WD_CMD_MSGS__CMD_ID__ENTER_SERVICE_MODE:
            /* Enter service mode */
            if (msg->body.enterKeepAliveMode.confirmationMagicNumber != WD_CMD_MSGS__CONFIRM_MODE_CHANGE_MAGIC_NUMBER) {
                /* magic bad */
                response->statusCode = WD_CMD_MSGS__RESPONSE_STATUS__ERROR_BAD_COMMAND_PARAMETER;
            } else if (rovstate == RS_KEEPALIVE) {
                /* Can transition directly to service mode from keepalive */
                enterMode(RS_SERVICE);
                response->statusCode = WD_CMD_MSGS__RESPONSE_STATUS__SUCCESS;
            } else if (rovstate == RS_MISSION) {
                // only enter the state if this is the second ENTER_SERVICE_MODE command in a row
                if (lastCommand == WD_CMD_MSGS__CMD_ID__ENTER_SERVICE_MODE) {
                    /* all checks pass, enter service mode */
                    enterMode(RS_SERVICE);
                }

                response->statusCode = WD_CMD_MSGS__RESPONSE_STATUS__SUCCESS;
            } else {
                /* not in the right state to transition to keepalive mode */
                response->statusCode = WD_CMD_MSGS__RESPONSE_STATUS__ERROR_BAD_COMMAND_SEQUENCE;
            }
            break;

        default:
            response->statusCode = WD_CMD_MSGS__RESPONSE_STATUS__ERROR_BAD_COMMAND_ID;
            break;
    }

    return GND_CMD__STATUS__SUCCESS;
}

GroundCmd__Status GroundCmd__generateEarthHeartbeat(I2C_Sensors__Readings* i2cReadings,
                                                    uint8_t* heartbeatOutBuffer,
                                                    size_t heartbeatOutBufferLen,
                                                    size_t* outputHeartbeatSize)
{
    if (NULL == i2cReadings || NULL == heartbeatOutBuffer || NULL == outputHeartbeatSize) {
        return GND_CMD__STATUS__ERROR_NULL;
    }

    size_t minRequiredBufferSize = 0;

    if (rovstate == RS_SERVICE || rovstate == RS_MISSION) {
        minRequiredBufferSize = 24;
    } else if (rovstate == RS_KEEPALIVE) {
        minRequiredBufferSize = 4;
    } else {
        return GND_CMD__STATUS__ERROR_WRONG_STATE;
    }

    if (heartbeatOutBufferLen < minRequiredBufferSize) {
        return GND_CMD__STATUS__ERROR_BUFFER_TOO_SMALL;
    }

    // build the packet
    heartbeatOutBuffer[0] = 0xFF;

    if (rovstate == RS_SERVICE || rovstate == RS_MISSION) {
        // send adc value temperature
        heartbeatOutBuffer[1] = (uint8_t)(adc_values[ADC_TEMP_IDX]);
        heartbeatOutBuffer[2] = (uint8_t)(adc_values[ADC_TEMP_IDX] >> 8);

        // send adc value temperature
        heartbeatOutBuffer[3] = (uint8_t)(i2cReadings->raw_battery_charge[0]);
        heartbeatOutBuffer[4] = (uint8_t)(i2cReadings->raw_battery_charge[1]);

        // send adc value temperature
        heartbeatOutBuffer[5] = (uint8_t)(i2cReadings->raw_battery_voltage[0]);
        heartbeatOutBuffer[6] = (uint8_t)(i2cReadings->raw_battery_voltage[1]);

        // send adc value temperature
        heartbeatOutBuffer[7] = (uint8_t)(i2cReadings->raw_battery_current[0]);
        heartbeatOutBuffer[8] = (uint8_t)(i2cReadings->raw_battery_current[1]);

        // send adc value temperature
        heartbeatOutBuffer[9] = (uint8_t)(i2cReadings->raw_fuel_gauge_temp[0]);
        heartbeatOutBuffer[10] = (uint8_t)(i2cReadings->raw_fuel_gauge_temp[1]);

        // send heater info Kp
        heartbeatOutBuffer[11] = (uint8_t)(Kp_heater);
        heartbeatOutBuffer[12] = (uint8_t)(Kp_heater >> 8);

        // send heater info setpoint
        heartbeatOutBuffer[13] = (uint8_t)(heater_setpoint);
        heartbeatOutBuffer[14] = (uint8_t)(heater_setpoint >> 8);

        // send heater info window ?
        heartbeatOutBuffer[15] = (uint8_t)(heater_window);
        heartbeatOutBuffer[16] = (uint8_t)(heater_window >> 8);

        // send ??
        heartbeatOutBuffer[17] = (uint8_t)(PWM_limit);
        heartbeatOutBuffer[18] = (uint8_t)(PWM_limit >> 8);

        // send the current rover state
        heartbeatOutBuffer[19] = (uint8_t) rovstate;

        // send the current heating status
        heartbeatOutBuffer[20] = heaterStatus;
        heartbeatOutBuffer[21] = heatingControlEnabled;

        // send ??
        heartbeatOutBuffer[22] = (uint8_t)(TB0CCR2);
        heartbeatOutBuffer[23] = (uint8_t)(TB0CCR2 >> 8);

        // send the current deploy state
        heartbeatOutBuffer[24] = hasDeployed;
    } else if (rovstate == RS_KEEPALIVE) {
        ////  Flight-spec heartbeats
        heartbeatOutBuffer[1] = (uint8_t)(i2cReadings->batt_charge_telem << 1);
        // send heater on status
        heartbeatOutBuffer[1] |= heaterStatus & 0x1;
        // battery current
        heartbeatOutBuffer[2] = (uint8_t)(i2cReadings->batt_curr_telem << 1);
        // send voltage nominal status (1=good, 0=too low)
        // check if batt voltage is above 16.59 V (~10% above discharge cutoff)
        heartbeatOutBuffer[2] |= (i2cReadings->raw_battery_voltage[0] > 0x3B);

        // send the thermistor temperature (12 bits to 8 bits)
        heartbeatOutBuffer[3] = (uint8_t)(adc_values[ADC_TEMP_IDX] >> 4);
    }

    return GND_CMD__STATUS__SUCCESS;
}
