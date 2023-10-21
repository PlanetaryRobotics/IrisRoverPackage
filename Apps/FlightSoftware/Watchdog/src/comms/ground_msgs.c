#include "comms/ground_msgs.h"
#include <stdint.h>
#include <msp430.h>

#include "comms/debug_comms.h"
#include "comms/ip_udp.h"
#include "comms/i2c_sensors.h"
#include "drivers/adc.h"
#include "drivers/bsp.h"
#include "drivers/uart.h"
#include "flags.h"
#include "utils/serialization.h"
#include "watchdog.h"

#define ENABLE_DEBUGGING_PRINT_OF_FAKE_REPORT 0
#define ENABLE_FORMATTED_DEBUG_PRINT 0

#define CHECK_SERIALIZATION_RESULT(RESULT_VAR)    \
    if (0 > RESULT_VAR)                           \
    {                                             \
        return GND_MSGS__STATUS__ERROR__INTERNAL; \
    }

GroundMsgs__Status GroundMsgs__generateFlightEarthHeartbeat(I2C_Sensors__Readings *i2cReadings,
                                                            AdcValues *adcValues,
                                                            HeaterParams *hParams,
                                                            FlightEarthHeartbeat *hb)
{
    if (NULL == i2cReadings || NULL == adcValues || NULL == hb)
    {
        return GND_MSGS__STATUS__ERROR_NULL;
    }

    hb->magicNumber = 0xFF;
    hb->batt_charge_telem = (uint8_t)(i2cReadings->batt_charge_telem & 0x7F);
    hb->heating = hParams->m_heating ? 1 : 0;
    hb->batt_curr_telem = (uint8_t)(i2cReadings->batt_curr_telem & 0x7F);
    // send voltage nominal status (1=good, 0=too low)
    // check if batt voltage is above 16.59 V (~10% above discharge cutoff)
    // 16.59 * 4095.0 / 3.3 / (274.0+2000.0)*274.0 (NOTE: this thresh is likely
    // too high since FM1 RC testing on Earth showed that at 23.80V-VBS, the
    // divider was reading low by a factor of 2928/3560, meaning the voltage
    // could actually be as high as 20.17V when this alarm sounds).
    // Also, note, this is mostly meaningless in KA since the battery is
    // disconnected anyway, so we won't be able to read it.
    hb->battery_voltage_good = (adcValues->vBattSense > 2480) ? 1 : 0;
    hb->battTemp = (uint8_t)(adcValues->battRT >> 4);

    return GND_MSGS__STATUS__SUCCESS;
}

GroundMsgs__Status GroundMsgs__generateFullEarthHeartbeat(I2C_Sensors__Readings *i2cReadings,
                                                          AdcValues *adcValues,
                                                          HeaterParams *hParams,
                                                          uint8_t stateAsUint,
                                                          FullEarthHeartbeat *hb)
{
    if (NULL == i2cReadings || NULL == adcValues || NULL == hParams || NULL == hb)
    {
        return GND_MSGS__STATUS__ERROR_NULL;
    }

    // build the packet
    hb->magicNumber = 0xFF;

    // send adc value temperature
    hb->battTemp = adcValues->battRT;

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

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)       \
    (byte & 0x80 ? '1' : '0'),     \
        (byte & 0x40 ? '1' : '0'), \
        (byte & 0x20 ? '1' : '0'), \
        (byte & 0x10 ? '1' : '0'), \
        (byte & 0x08 ? '1' : '0'), \
        (byte & 0x04 ? '1' : '0'), \
        (byte & 0x02 ? '1' : '0'), \
        (byte & 0x01 ? '1' : '0')

GroundMsgs__Status GroundMsgs__generateDetailedReport(I2C_Sensors__Readings *i2cReadings,
                                                      AdcValues *adcValues,
                                                      WatchdogStateDetails *details,
                                                      DetailedReport *report,
                                                      uint8_t *reportBuffer)
{
    static uint8_t sequenceNumber = 0;

    if (NULL == i2cReadings || NULL == adcValues || NULL == details || NULL == report)
    {
        return GND_MSGS__STATUS__ERROR_NULL;
    }

    // Update values read from digital inputs on WD chip itself (not on I/O expander)
    readOnChipInputs();

    uint8_t *dstIntPtr = (uint8_t *)reportBuffer;

    report->magic = 0xD5;

    uint8_t magicAsUint = 0xD5;
    short serializationResult = Serialization__serializeAs8Bit(&(magicAsUint), dstIntPtr, SERIALIZATION__LITTLE_ENDIAN);
    CHECK_SERIALIZATION_RESULT(serializationResult);
    dstIntPtr += sizeof(magicAsUint);

    report->chargeStat1 = (details->m_inputPinAndStateBits & IPASBI_MASK(IPASBI__CHARGE_STAT1)) ? 1 : 0;
    report->chargeStat2 = (details->m_inputPinAndStateBits & IPASBI_MASK(IPASBI__CHARGE_STAT2)) ? 1 : 0;
    report->battStat = (details->m_inputPinAndStateBits & IPASBI_MASK(IPASBI__BATT_STAT)) ? 1 : 0;
    report->latchStat = (details->m_inputPinAndStateBits & IPASBI_MASK(IPASBI__LATCH_STAT)) ? 1 : 0;
    report->pg12 = (details->m_inputPinAndStateBits & IPASBI_MASK(IPASBI__PG12)) ? 1 : 0;
    report->pg18 = (details->m_inputPinAndStateBits & IPASBI_MASK(IPASBI__PG18)) ? 1 : 0;
    report->pg33 = (details->m_inputPinAndStateBits & IPASBI_MASK(IPASBI__PG33)) ? 1 : 0;
    report->pg50 = (details->m_inputPinAndStateBits & IPASBI_MASK(IPASBI__PG50)) ? 1 : 0;

    uint8_t data = 0;
    data |= (uint8_t)((((details->m_inputPinAndStateBits & IPASBI_MASK(IPASBI__CHARGE_STAT1)) ? 1 : 0) & 0x1) << 7);
    data |= (uint8_t)((((details->m_inputPinAndStateBits & IPASBI_MASK(IPASBI__CHARGE_STAT2)) ? 1 : 0) & 0x1) << 6);
    data |= (uint8_t)((((details->m_inputPinAndStateBits & IPASBI_MASK(IPASBI__BATT_STAT)) ? 1 : 0) & 0x1) << 5);
    data |= (uint8_t)((((details->m_inputPinAndStateBits & IPASBI_MASK(IPASBI__LATCH_STAT)) ? 1 : 0) & 0x1) << 4);
    data |= (uint8_t)((((details->m_inputPinAndStateBits & IPASBI_MASK(IPASBI__PG12)) ? 1 : 0) & 0x1) << 3);
    data |= (uint8_t)((((details->m_inputPinAndStateBits & IPASBI_MASK(IPASBI__PG18)) ? 1 : 0) & 0x1) << 2);
    data |= (uint8_t)((((details->m_inputPinAndStateBits & IPASBI_MASK(IPASBI__PG33)) ? 1 : 0) & 0x1) << 1);
    data |= (uint8_t)((((details->m_inputPinAndStateBits & IPASBI_MASK(IPASBI__PG50)) ? 1 : 0) & 0x1) << 0);

    serializationResult = Serialization__serializeAs8Bit(&(data), dstIntPtr, SERIALIZATION__LITTLE_ENDIAN);
    CHECK_SERIALIZATION_RESULT(serializationResult);
    dstIntPtr += sizeof(data);

    report->state = details->m_stateAsUint;

    serializationResult = Serialization__serializeAs8Bit(&(details->m_stateAsUint), dstIntPtr, SERIALIZATION__LITTLE_ENDIAN);
    CHECK_SERIALIZATION_RESULT(serializationResult);
    dstIntPtr += sizeof(details->m_stateAsUint);

    uint8_t deploymentStatus;
    if (details->m_inputPinAndStateBits & IPASBI_MASK(IPASBI__DEPLOYED))
    {
        report->deploymentStatus = 2;
        deploymentStatus = 2;
    }
    else if (details->m_inputPinAndStateBits & IPASBI_MASK(IPASBI__DEPLOYING))
    {
        report->deploymentStatus = 1;
        deploymentStatus = 1;
    }
    else
    {
        report->deploymentStatus = 0;
        deploymentStatus = 0;
    }

    report->uart0Initialized = (details->m_inputPinAndStateBits & IPASBI_MASK(IPASBI__UART0_INITIALIZED)) ? 1 : 0;
    report->uart1Initialized = (details->m_inputPinAndStateBits & IPASBI_MASK(IPASBI__UART1_INITIALIZED)) ? 1 : 0;
    report->adcBattRT = (uint16_t)(adcValues->battRT && 0x0FFF);

    uint16_t data16 = 0;
    data16 |= (uint16_t)(((uint16_t)(((uint8_t)deploymentStatus) & 0x3)) << 14);
    data16 |= (uint16_t)(((uint16_t)(((details->m_inputPinAndStateBits & IPASBI_MASK(IPASBI__UART0_INITIALIZED)) ? 1 : 0) & 0x1)) << 13);
    data16 |= (uint16_t)(((uint16_t)(((details->m_inputPinAndStateBits & IPASBI_MASK(IPASBI__UART1_INITIALIZED)) ? 1 : 0) & 0x1)) << 12);
    data16 |= (uint16_t)(adcValues->battRT && 0x0FFF);

    serializationResult = Serialization__serializeAs16Bit(&(data16), dstIntPtr, SERIALIZATION__LITTLE_ENDIAN);
    CHECK_SERIALIZATION_RESULT(serializationResult);
    dstIntPtr += sizeof(data16);

    report->sequenceNumber = sequenceNumber++;

    serializationResult = Serialization__serializeAs8Bit(&(report->sequenceNumber), dstIntPtr, SERIALIZATION__LITTLE_ENDIAN);
    CHECK_SERIALIZATION_RESULT(serializationResult);
    dstIntPtr += sizeof(report->sequenceNumber);

    report->outputPinStateBits = details->m_outputPinBits;

    serializationResult = Serialization__serializeAs32Bit(&(details->m_outputPinBits), dstIntPtr, SERIALIZATION__LITTLE_ENDIAN);
    CHECK_SERIALIZATION_RESULT(serializationResult);
    dstIntPtr += sizeof(details->m_outputPinBits);

    report->lowerResetActionBits = (uint32_t)(0xFFFFFFFF & (details->m_resetActionBits));
    report->upperResetActionBits = (uint8_t)(((details->m_resetActionBits) >> 32) & 0xFF);

    uint8_t buffer64bit[8] = {0};
    serializationResult = Serialization__serializeAs64Bit(&(details->m_resetActionBits), buffer64bit, SERIALIZATION__LITTLE_ENDIAN);
    CHECK_SERIALIZATION_RESULT(serializationResult);

    for (int i = 5; i >= 0; i--)
    {
        *dstIntPtr = buffer64bit[i];
        dstIntPtr++;
    }

    report->vLanderSense = (uint8_t)(((adcValues->vLanderSense) >> 5) & 0x7F); // Top 7 bits of 12
    report->battTemp = (uint16_t)(((adcValues->battTemp) >> 3) & 0x1FF);       // Top 9 bits of 12

    data16 = 0;
    data16 |= (uint16_t)(((uint16_t)(((adcValues->vLanderSense) >> 5) & 0x7F)) << 9);
    data16 |= (uint16_t)(((adcValues->battTemp) >> 3) & 0x1FF);

    serializationResult = Serialization__serializeAs16Bit(&(data16), dstIntPtr, SERIALIZATION__LITTLE_ENDIAN);
    CHECK_SERIALIZATION_RESULT(serializationResult);
    dstIntPtr += sizeof(data16);

    report->vSysAllSens = (uint8_t)(((adcValues->vSysAllSense) >> 7) & 0x1F); // Top 5 bits of 12
    report->iSysAllSense = (uint16_t)((adcValues->iSysAllSense) & 0x1FF);     // Bottom 9 bits of 12
    report->vBattSense = (uint16_t)(((adcValues->vBattSense) >> 3) & 0x1FF);  // Top 9 bits of 12
    report->vcc24 = (uint8_t)(((adcValues->vcc24) >> 5) & 0x7F);              // Top 7 bits of 12
    report->heatingControlEnabled = (details->m_hParams.persistent->m_heatingControlEnabled) ? 1 : 0;
    report->heating = (details->m_hParams.m_heating) ? 1 : 0;

    uint32_t data32 = 0;
    data32 |= (uint32_t)(((uint32_t)(((adcValues->vSysAllSense) >> 7) & 0x1F)) << 27);
    data32 |= (uint32_t)(((uint32_t)((adcValues->iSysAllSense) & 0x1FF)) << 18);
    data32 |= (uint32_t)(((uint32_t)(((adcValues->vBattSense) >> 3) & 0x1FF)) << 9);
    data32 |= (uint32_t)(((uint32_t)(((adcValues->vcc24) >> 5) & 0x7F)) << 2);
    data32 |= (uint32_t)(((details->m_hParams.persistent->m_heatingControlEnabled) ? 1 : 0) << 1);
    data32 |= (uint32_t)((details->m_hParams.m_heating) ? 1 : 0);

    serializationResult = Serialization__serializeAs32Bit(&(data32), dstIntPtr, SERIALIZATION__LITTLE_ENDIAN);
    CHECK_SERIALIZATION_RESULT(serializationResult);
    dstIntPtr += sizeof(data32);

    report->vcc2Point5 = (uint8_t)(((adcValues->vcc2Point5) >> 7) & 0x1F); // Top 5 bits of 12
    report->vcc2Point8 = (uint8_t)(((adcValues->vcc2Point8) >> 7) & 0x1F); // Top 5 bits of 12
    report->vcc28 = (uint8_t)(((adcValues->vcc28) >> 6) & 0x3F);           // Top 6 bits of 12

    data16 = 0;
    data16 |= (uint16_t)(((uint16_t)(((adcValues->vcc2Point5) >> 7) & 0x1F)) << 11);
    data16 |= (uint16_t)(((uint16_t)(((adcValues->vcc2Point8) >> 7) & 0x1F)) << 6);
    data16 |= (uint16_t)(((adcValues->vcc28) >> 6) & 0x3F);

    serializationResult = Serialization__serializeAs16Bit(&(data16), dstIntPtr, SERIALIZATION__LITTLE_ENDIAN);
    CHECK_SERIALIZATION_RESULT(serializationResult);
    dstIntPtr += sizeof(data16);

    report->kpHeater = details->m_hParams.persistent->m_kpHeater;
    report->heaterPwmLimit = details->m_hParams.persistent->m_pwmLimit;
    report->heaterSetpoint = details->m_hParams.persistent->m_heaterSetpoint;
    report->heaterOnValue = details->m_hParams.persistent->m_heaterOnVal;
    report->heaterOffValue = details->m_hParams.persistent->m_heaterOffVal;
    report->heaterDutyCyclePeriod = details->m_hParams.persistent->m_heaterDutyCyclePeriod;
    report->heaterPwmValue = details->m_hParams.persistent->m_heaterDutyCycle;

    serializationResult = Serialization__serializeAs16Bit(&(details->m_hParams.persistent->m_kpHeater), dstIntPtr, SERIALIZATION__LITTLE_ENDIAN);
    CHECK_SERIALIZATION_RESULT(serializationResult);
    dstIntPtr += sizeof(details->m_hParams.persistent->m_kpHeater);

    serializationResult = Serialization__serializeAs16Bit(&(details->m_hParams.persistent->m_pwmLimit), dstIntPtr, SERIALIZATION__LITTLE_ENDIAN);
    CHECK_SERIALIZATION_RESULT(serializationResult);
    dstIntPtr += sizeof(details->m_hParams.persistent->m_pwmLimit);

    serializationResult = Serialization__serializeAs16Bit(&(details->m_hParams.persistent->m_heaterSetpoint), dstIntPtr, SERIALIZATION__LITTLE_ENDIAN);
    CHECK_SERIALIZATION_RESULT(serializationResult);
    dstIntPtr += sizeof(details->m_hParams.persistent->m_heaterSetpoint);

    serializationResult = Serialization__serializeAs16Bit(&(details->m_hParams.persistent->m_heaterOnVal), dstIntPtr, SERIALIZATION__LITTLE_ENDIAN);
    CHECK_SERIALIZATION_RESULT(serializationResult);
    dstIntPtr += sizeof(details->m_hParams.persistent->m_heaterOnVal);

    serializationResult = Serialization__serializeAs16Bit(&(details->m_hParams.persistent->m_heaterOffVal), dstIntPtr, SERIALIZATION__LITTLE_ENDIAN);
    CHECK_SERIALIZATION_RESULT(serializationResult);
    dstIntPtr += sizeof(details->m_hParams.persistent->m_heaterOffVal);

    serializationResult = Serialization__serializeAs16Bit(&(details->m_hParams.persistent->m_heaterDutyCyclePeriod), dstIntPtr, SERIALIZATION__LITTLE_ENDIAN);
    CHECK_SERIALIZATION_RESULT(serializationResult);
    dstIntPtr += sizeof(details->m_hParams.persistent->m_heaterDutyCyclePeriod);

    serializationResult = Serialization__serializeAs16Bit(&(details->m_hParams.persistent->m_heaterDutyCycle), dstIntPtr, SERIALIZATION__LITTLE_ENDIAN);
    CHECK_SERIALIZATION_RESULT(serializationResult);
    dstIntPtr += sizeof(details->m_hParams.persistent->m_heaterDutyCycle);

    memcpy(report->rawBatteryCharge, i2cReadings->raw_battery_charge, sizeof(report->rawBatteryCharge));
    memcpy(report->rawBatteryVoltage, i2cReadings->raw_battery_voltage, sizeof(report->rawBatteryVoltage));
    memcpy(report->rawBatteryCurrent, i2cReadings->raw_battery_current, sizeof(report->rawBatteryCurrent));
    memcpy(report->rawFuelGaugeTemp, i2cReadings->raw_fuel_gauge_temp, sizeof(report->rawFuelGaugeTemp));

    memcpy(dstIntPtr, i2cReadings->raw_battery_charge, sizeof(i2cReadings->raw_battery_charge));
    dstIntPtr += sizeof(i2cReadings->raw_battery_charge);
    memcpy(dstIntPtr, i2cReadings->raw_battery_voltage, sizeof(i2cReadings->raw_battery_voltage));
    dstIntPtr += sizeof(i2cReadings->raw_battery_voltage);
    memcpy(dstIntPtr, i2cReadings->raw_battery_current, sizeof(i2cReadings->raw_battery_current));
    dstIntPtr += sizeof(i2cReadings->raw_battery_current);
    memcpy(dstIntPtr, i2cReadings->raw_fuel_gauge_temp, sizeof(i2cReadings->raw_fuel_gauge_temp));
    dstIntPtr += sizeof(i2cReadings->raw_fuel_gauge_temp);

    report->battChargeTelem = i2cReadings->batt_charge_telem;
    report->battCurrTelem = i2cReadings->batt_curr_telem;

    serializationResult = Serialization__serializeAs8Bit(&(i2cReadings->batt_charge_telem), dstIntPtr, SERIALIZATION__LITTLE_ENDIAN);
    CHECK_SERIALIZATION_RESULT(serializationResult);
    dstIntPtr += sizeof(i2cReadings->batt_charge_telem);

    serializationResult = Serialization__serializeAs8Bit(&(i2cReadings->batt_curr_telem), dstIntPtr, SERIALIZATION__LITTLE_ENDIAN);
    CHECK_SERIALIZATION_RESULT(serializationResult);
    dstIntPtr += sizeof(i2cReadings->batt_curr_telem);

#if defined(ENABLE_DEBUG_ONLY_CODE) && ENABLE_DEBUGGING_PRINT_OF_FAKE_REPORT
    DetailedReport report2 = {0};
    report2.magic = 0xD5;

    report2.chargeStat1 = 1;
    report2.chargeStat2 = 1;
    report2.battStat = 0;
    report2.latchStat = 0;
    report2.pg12 = 1;
    report2.pg18 = 0;
    report2.pg33 = 1;
    report2.pg50 = 0;

    report2.state = 8;
    report2.deploymentStatus = 0;

    report2.uart0Initialized = 0;
    report2.uart1Initialized = 1;
    report2.adcBattRT = 0b110001100011;

    report2.sequenceNumber = 15;

    report2.outputPinStateBits = 0b00001111101010100011001111111111;
    report2.lowerResetActionBits = 0;
    report2.upperResetActionBits = 0;

    report2.vLanderSense = 0b1111111; // Top 7 bits of 12
    report2.battTemp = 0b110010011;   // Top 9 bits of 12

    report2.vSysAllSens = 0b01110;      // Top 5 bits of 12
    report2.iSysAllSense = 0b100000111; // Bottom 9 bits of 12
    report2.vBattSense = 0b001100110;   // Top 9 bits of 12
    report2.vcc24 = 0b0011100;          // Top 7 bits of 12
    report2.heatingControlEnabled = 0;
    report2.heating = 0;

    report2.vcc2Point5 = 0b10101; // Top 5 bits of 12
    report2.vcc2Point8 = 0b01010; // Top 5 bits of 12
    report2.vcc28 = 0b110011;     // Top 6 bits of 12

    report2.kpHeater = 0x0A0B;
    report2.heaterPwmLimit = 0x0C0D;
    report2.heaterSetpoint = 0x0E0F;
    report2.heaterOnValue = 0xAABB;
    report2.heaterOffValue = 0xCCDD;
    report2.heaterDutyCyclePeriod = 0xEEFF;
    report2.heaterPwmValue = 0xF001;

    report2.rawBatteryCharge[0] = 0x1A;
    report2.rawBatteryCharge[1] = 0x1B;

    report2.rawBatteryVoltage[0] = 0x2A;
    report2.rawBatteryVoltage[1] = 0x2B;

    report2.rawBatteryCurrent[0] = 0x3A;
    report2.rawBatteryCurrent[1] = 0x3B;

    report2.rawFuelGaugeTemp[0] = 0x4A;
    report2.rawFuelGaugeTemp[1] = 0x4B;

    report2.battChargeTelem = 0x5A;
    report2.battCurrTelem = 0x5B;

    uint8_t *reportView = (uint8_t *)&report2;
    size_t rowLen = 4;

    size_t numRows = sizeof(report2) / rowLen;
    if (sizeof(report2) % rowLen > 0)
    {
        numRows++;
    }

    for (size_t r = 0; r < numRows; ++r)
    {
        DPRINTF_ERR("%s%d: ", (((rowLen * r) <= 9) ? "0" : ""), (int)(rowLen * r));

        for (size_t c = 0; c < rowLen; ++c)
        {
            size_t i = (rowLen * r) + c;

            if (i >= sizeof(report2))
            {
                break;
            }

            DPRINTF_ERR("%s%x ", ((reportView[i] <= 0xF) ? "0" : ""), reportView[i]);
        }
        DPRINTF_ERR("\n");
    }
    DPRINTF_ERR("\n");

    for (size_t r = 0; r < numRows; ++r)
    {
        DPRINTF_ERR("%s%d: ", (((rowLen * r) <= 9) ? "0" : ""), (int)(rowLen * r));

        for (size_t c = 0; c < rowLen; ++c)
        {
            size_t i = (rowLen * r) + c;

            if (i >= sizeof(report2))
            {
                break;
            }

            DPRINTF_ERR(BYTE_TO_BINARY_PATTERN " ", BYTE_TO_BINARY(reportView[i]));
        }
        DPRINTF_ERR("\n");
    }
    DPRINTF_ERR("\n");

#endif // #ifdef ENABLE_DEBUG_ONLY_CODE

    return GND_MSGS__STATUS__SUCCESS;
}
