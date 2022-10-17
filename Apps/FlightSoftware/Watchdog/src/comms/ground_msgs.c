#include "comms/ground_msgs.h"
#include <stdint.h>
#include <msp430.h>

#include "comms/ip_udp.h"
#include "comms/i2c_sensors.h"
#include "drivers/adc.h"
#include "drivers/bsp.h"
#include "drivers/uart.h"
#include "flags.h"
#include "utils/serialization.h"
#include "watchdog.h"

GroundMsgs__Status GroundMsgs__generateFlightEarthHeartbeat(I2C_Sensors__Readings* i2cReadings,
                                                            AdcValues* adcValues,
                                                            HeaterParams* hParams,
                                                            FlightEarthHeartbeat* hb)
{
    if (NULL == i2cReadings || NULL == adcValues || NULL == hb) {
        return GND_MSGS__STATUS__ERROR_NULL;
    }

    hb->magicNumber = 0xFF;
    hb->batt_charge_telem = (uint8_t) (i2cReadings->batt_charge_telem & 0x7F);
    hb->heating = hParams->m_heating ? 1 : 0;
    hb->batt_curr_telem  = (uint8_t) (i2cReadings->batt_curr_telem & 0x7F);
    // send voltage nominal status (1=good, 0=too low)
    // check if batt voltage is above 16.59 V (~10% above discharge cutoff)
    /** @todo Update this threshold **/
    hb->battery_voltage_good  = (i2cReadings->raw_battery_voltage[0] > 0x3B) ? 1 : 0;
    hb->battTemp = (uint8_t) (adcValues->battTemp >> 4);

    return GND_MSGS__STATUS__SUCCESS;
}

GroundMsgs__Status GroundMsgs__generateFullEarthHeartbeat(I2C_Sensors__Readings* i2cReadings,
                                                          AdcValues* adcValues,
                                                          HeaterParams* hParams,
                                                          uint8_t stateAsUint,
                                                          FullEarthHeartbeat* hb)
{
    if (NULL == i2cReadings || NULL == adcValues || NULL == hParams || NULL == hb) {
        return GND_MSGS__STATUS__ERROR_NULL;
    }

    // build the packet
    hb->magicNumber = 0xFF;

    // send adc value temperature
    hb->battTemp = adcValues->battTemp;

    // send adc value temperature
    hb->raw_battery_charge[0] = i2cReadings->raw_battery_charge[0];
    hb->raw_battery_charge[1] = i2cReadings->raw_battery_charge[1];

    // send adc value temperature
    hb->raw_battery_voltage[0] = i2cReadings->raw_battery_voltage[0];
    hb->raw_battery_voltage[1] = i2cReadings->raw_battery_voltage[1];

    // send adc value temperature
    hb->raw_battery_current[0] = i2cReadings->raw_battery_current[0];
    hb->raw_battery_current[1] = i2cReadings->raw_battery_current[1];

    // send adc value temperature
    hb->raw_fuel_gauge_temp[0] = i2cReadings->raw_fuel_gauge_temp[0];
    hb->raw_fuel_gauge_temp[1] = i2cReadings->raw_fuel_gauge_temp[1];

    // send heater info Kp
    hb->kpHeater = hParams->m_kpHeater;

    // send heater info setpoint
    hb->heaterSetpoint = hParams->m_heaterSetpoint;

    // send heater info window ?
    hb->heaterWindow = hParams->m_heaterWindow;

    // send pwm value limit
    hb->pwmLimit = hParams->m_pwmLimit;

    // send the current rover state
    hb->stateAsUint = stateAsUint;

    // send the current heating status
    hb->heating = hParams->m_heating;
    hb->heatingControlEnabled = hParams->m_heatingControlEnabled;

    // send pwm value
    hb->pwmValue = TB0CCR2;

    return GND_MSGS__STATUS__SUCCESS;
}

GroundMsgs__Status GroundMsgs__generateDetailedReport(I2C_Sensors__Readings* i2cReadings,
                                                      AdcValues* adcValues,
                                                      WatchdogStateDetails* details,
                                                      DetailedReport* report)
{
    static uint8_t sequenceNumber = 0;

    if (NULL == i2cReadings || NULL == adcValues || NULL == details || NULL == report) {
        return GND_MSGS__STATUS__ERROR_NULL;
    }

    // Update values read from digital inputs on WD chip itself (not on I/O expander)
    readOnChipInputs();

    report->magic = 0xD5;

    report->chargeStat1 = (details->m_inputPinAndStateBits & IPASBI_MASK(IPASBI__CHARGE_STAT1)) ? 1 : 0;
    report->chargeStat2 = (details->m_inputPinAndStateBits & IPASBI_MASK(IPASBI__CHARGE_STAT2)) ? 1 : 0;
    report->battStat = (details->m_inputPinAndStateBits & IPASBI_MASK(IPASBI__BATT_STAT)) ? 1 : 0;
    report->latchStat = (details->m_inputPinAndStateBits & IPASBI_MASK(IPASBI__LATCH_STAT)) ? 1 : 0;
    report->pg12 = (details->m_inputPinAndStateBits & IPASBI_MASK(IPASBI__PG12)) ? 1 : 0;
    report->pg18 = (details->m_inputPinAndStateBits & IPASBI_MASK(IPASBI__PG18)) ? 1 : 0;
    report->pg33 = (details->m_inputPinAndStateBits & IPASBI_MASK(IPASBI__PG33)) ? 1 : 0;
    report->pg50 = (details->m_inputPinAndStateBits & IPASBI_MASK(IPASBI__PG50)) ? 1 : 0;

    report->state = details->m_stateAsUint;

    if (details->m_inputPinAndStateBits & IPASBI_MASK(IPASBI__DEPLOYED)) {
        report->deploymentStatus = 2;
    } else if (details->m_inputPinAndStateBits & IPASBI_MASK(IPASBI__DEPLOYING)) {
        report->deploymentStatus = 1;
    } else {
        report->deploymentStatus = 0;
    }

    report->uart0Initialized = (details->m_inputPinAndStateBits & IPASBI_MASK(IPASBI__UART0_INITIALIZED)) ? 1 : 0;
    report->uart1Initialized = (details->m_inputPinAndStateBits & IPASBI_MASK(IPASBI__UART1_INITIALIZED)) ? 1 : 0;
    report->adcBattRT = (uint16_t) (adcValues->battRT && 0x0FFF);

    report->sequenceNumber = sequenceNumber++;

    report->outputPinStateBits = details->m_outputPinBits;
    report->lowerResetActionBits = (uint32_t) (0xFFFFFFFF & (details->m_resetActionBits));
    report->upperResetActionBits = (uint8_t) (((details->m_resetActionBits) >> 32) & 0xFF);

    report->vLanderSense = (uint8_t) (((adcValues->vLanderSense) >> 5) & 0x7F); // Top 7 bits of 12
    report->battTemp = (uint16_t) (((adcValues->battTemp) >> 3) & 0x1FF); // Top 9 bits of 12

    report->vSysAllSens = (uint8_t) (((adcValues->vSysAllSense) >> 7) & 0x1F); // Top 5 bits of 12
    report->iSysAllSense = (uint16_t) ((adcValues->iSysAllSense) & 0x1FF); // Bottom 9 bits of 12
    report->vBattSense = (uint16_t) (((adcValues->vBattSense) >> 3) & 0x1FF); // Top 9 bits of 12
    report->vcc24 = (uint8_t) (((adcValues->vcc24) >> 5) & 0x7F); // Top 7 bits of 12
    report->heatingControlEnabled = (details->m_hParams.m_heatingControlEnabled) ? 1 : 0;
    report->heating = (details->m_hParams.m_heating) ? 1 : 0;

    report->vcc2Point5 = (uint8_t) (((adcValues->vcc2Point5) >> 7) & 0x1F); // Top 5 bits of 12
    report->vcc2Point8 = (uint8_t) (((adcValues->vcc2Point8) >> 7) & 0x1F); // Top 5 bits of 12
    report->vcc28 = (uint8_t) (((adcValues->vcc28) >> 6) & 0x3F); // Top 6 bits of 12

    report->kpHeater = details->m_hParams.m_kpHeater;
    report->heaterPwmLimit = details->m_hParams.m_pwmLimit;
    report->heaterSetpoint = details->m_hParams.m_heaterSetpoint;
    report->heaterOnValue = details->m_hParams.m_heaterOnVal;
    report->heaterOffValue = details->m_hParams.m_heaterOffVal;
    report->heaterDutyCyclePeriod = details->m_hParams.m_heaterDutyCyclePeriod;
    report->heaterPwmValue = details->m_hParams.m_heaterDutyCycle;

    memcpy(report->rawBatteryCharge, i2cReadings->raw_battery_charge, sizeof(report->rawBatteryCharge));
    memcpy(report->rawBatteryVoltage, i2cReadings->raw_battery_voltage, sizeof(report->rawBatteryVoltage));
    memcpy(report->rawBatteryCurrent, i2cReadings->raw_battery_current, sizeof(report->rawBatteryCurrent));
    memcpy(report->rawFuelGaugeTemp, i2cReadings->raw_fuel_gauge_temp, sizeof(report->rawFuelGaugeTemp));

    report->battChargeTelem = i2cReadings->batt_charge_telem;
    report->battCurrTelem = i2cReadings->batt_curr_telem;

    return GND_MSGS__STATUS__SUCCESS;
}

