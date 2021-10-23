/*
 * ground_cmd.c
 *
 * Handle commands sent from the ground up to the watchdog
 *
 *  Created on: Nov. 29, 2020
 *      Author: ohnx
 */
#include <stdint.h>

#include "comms/ip_udp.h"
#include "comms/i2c_sensors.h"
#include "drivers/adc.h"
#include "drivers/bsp.h"
#include "drivers/uart.h"
#include "flags.h"
#include "ground_cmd.h"
#include "utils/serialization.h"
#include "watchdog.h"


GroundCmd__Status GroundCmd__generateFlightEarthHeartbeat(I2C_Sensors__Readings* i2cReadings,
                                                          AdcValues* adcValues,
                                                          HeaterParams* hParams,
                                                          FlightEarthHeartbeat* hb)
{
    if (NULL == i2cReadings || NULL == adcValues || NULL == hb) {
        return GND_CMD__STATUS__ERROR_NULL;
    }

    // build the packet
    hb->heartbeatOutBuffer[0] = 0xFF;

    ////  Flight-spec heartbeats
    hb->heartbeatOutBuffer[1] = (uint8_t)(i2cReadings->batt_charge_telem << 1);
    // send heater on status
    hb->heartbeatOutBuffer[1] |= hParams->m_heating & 0x1;
    // battery current
    hb->heartbeatOutBuffer[2] = (uint8_t)(i2cReadings->batt_curr_telem << 1);
    // send voltage nominal status (1=good, 0=too low)
    // check if batt voltage is above 16.59 V (~10% above discharge cutoff)
    hb->heartbeatOutBuffer[2] |= (i2cReadings->raw_battery_voltage[0] > 0x3B);

    // send the thermistor temperature (12 bits to 8 bits)
    hb->heartbeatOutBuffer[3] = (uint8_t)(adcValues->battTemp >> 4);

    return GND_CMD__STATUS__SUCCESS;
}

GroundCmd__Status GroundCmd__generateFullEarthHeartbeat(I2C_Sensors__Readings* i2cReadings,
                                                        AdcValues* adcValues,
                                                        HeaterParams* hParams,
                                                        uint8_t stateAsUint,
                                                        FullEarthHeartbeat* hb)
{
    if (NULL == i2cReadings || NULL == adcValues || NULL == hParams || NULL == hb) {
        return GND_CMD__STATUS__ERROR_NULL;
    }

    // build the packet
    hb->heartbeatOutBuffer[0] = 0xFF;

    // send adc value temperature
    hb->heartbeatOutBuffer[1] = (uint8_t)(adcValues->battTemp);
    hb->heartbeatOutBuffer[2] = (uint8_t)(adcValues->battTemp >> 8);

    // send adc value temperature
    hb->heartbeatOutBuffer[3] = (uint8_t)(i2cReadings->raw_battery_charge[0]);
    hb->heartbeatOutBuffer[4] = (uint8_t)(i2cReadings->raw_battery_charge[1]);

    // send adc value temperature
    hb->heartbeatOutBuffer[5] = (uint8_t)(i2cReadings->raw_battery_voltage[0]);
    hb->heartbeatOutBuffer[6] = (uint8_t)(i2cReadings->raw_battery_voltage[1]);

    // send adc value temperature
    hb->heartbeatOutBuffer[7] = (uint8_t)(i2cReadings->raw_battery_current[0]);
    hb->heartbeatOutBuffer[8] = (uint8_t)(i2cReadings->raw_battery_current[1]);

    // send adc value temperature
    hb->heartbeatOutBuffer[9] = (uint8_t)(i2cReadings->raw_fuel_gauge_temp[0]);
    hb->heartbeatOutBuffer[10] = (uint8_t)(i2cReadings->raw_fuel_gauge_temp[1]);

    // send heater info Kp
    hb->heartbeatOutBuffer[11] = (uint8_t)(hParams->m_kpHeater);
    hb->heartbeatOutBuffer[12] = (uint8_t)(hParams->m_kpHeater >> 8);

    // send heater info setpoint
    hb->heartbeatOutBuffer[13] = (uint8_t)(hParams->m_heaterSetpoint);
    hb->heartbeatOutBuffer[14] = (uint8_t)(hParams->m_heaterSetpoint >> 8);

    // send heater info window ?
    hb->heartbeatOutBuffer[15] = (uint8_t)(hParams->m_heaterWindow);
    hb->heartbeatOutBuffer[16] = (uint8_t)(hParams->m_heaterWindow >> 8);

    // send ??
    hb->heartbeatOutBuffer[17] = (uint8_t)(hParams->m_pwmLimit);
    hb->heartbeatOutBuffer[18] = (uint8_t)(hParams->m_pwmLimit >> 8);

    // send the current rover state
    hb->heartbeatOutBuffer[19] = stateAsUint;

    // send the current heating status
    hb->heartbeatOutBuffer[20] = hParams->m_heating;
    hb->heartbeatOutBuffer[21] = hParams->m_heatingControlEnabled;

    // send ??
    hb->heartbeatOutBuffer[22] = (uint8_t)(TB0CCR2);
    hb->heartbeatOutBuffer[23] = (uint8_t)(TB0CCR2 >> 8);

    // send the current deploy state
    //heartbeatOutBuffer[24] = hasDeployed;

    return GND_CMD__STATUS__SUCCESS;
}
