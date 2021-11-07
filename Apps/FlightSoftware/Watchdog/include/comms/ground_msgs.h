#ifndef __WATCHDOG_GROUND_MSGS_H__
#define __WATCHDOG_GROUND_MSGS_H__

#include <stdint.h>
#include "comms/watchdog_cmd_msgs.h"
#include "comms/i2c_sensors.h"
#include "common.h"
#include "flags.h"
#include "drivers/adc.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef enum GroundMsgs__Status
{
    GND_MSGS__STATUS__SUCCESS = 0, /* Operation succeeded. */
    GND_MSGS__STATUS__ERROR_NULL = -1, /* A required argument or a member of an argument was NULL */
    GND_MSGS__STATUS__ERROR_BUFFER_TOO_SMALL = -2,
    GND_MSGS__STATUS__ERROR_SERIALIZATION_ERROR = -3,
    GND_MSGS__STATUS__ERROR_UNKNOWN_MESSAGE_ID = -4,
    GND_MSGS__STATUS__ERROR_UNKNOWN_MAGIC_NUMBER = -5,
    GND_MSGS__STATUS__ERROR_WRONG_STATE = -6,
    GND_MSGS__STATUS__ERROR_UNKNOWN_RESET_VALUE = -7,

    GND_MSGS__STATUS__ERROR__INTERNAL = -255 /* An unexpected internal error occurred. */
} GroundMsgs__Status;

typedef struct __attribute__((packed)) FlightEarthHeartbeat
{
    uint8_t magicNumber;
    uint8_t batt_charge_telem : 7;
    uint8_t heating : 1;
    uint8_t batt_curr_telem : 7;
    uint8_t battery_voltage_good : 1;
    uint8_t battTemp;
} FlightEarthHeartbeat;

typedef struct __attribute__((packed)) FullEarthHeartbeat
{
    uint8_t magicNumber; // always equal to 0xFF
    uint16_t battTemp;
    uint8_t raw_battery_charge[2];
    uint8_t raw_battery_voltage[2];
    uint8_t raw_battery_current[2];
    uint8_t raw_fuel_gauge_temp[2];
    uint16_t kpHeater;
    uint16_t heaterSetpoint;
    uint16_t heaterWindow;
    uint16_t pwmLimit;
    uint8_t stateAsUint;
    uint8_t heating;
    uint8_t heatingControlEnabled;
uint16_t pwmValue;
} FullEarthHeartbeat;

typedef struct __attribute__((packed)) DetailedReport
{
    uint8_t magic; // Should always be 0xD5

    uint8_t chargeStat1 : 1;
    uint8_t chargeStat2 : 1;
    uint8_t battStat : 1;
    uint8_t latchStat : 1;
    uint8_t pg12 : 1;
    uint8_t pg18 : 1;
    uint8_t pg33 : 1;
    uint8_t pg50 : 1;

    uint8_t state;

    uint16_t deploymentStatus : 2; // 00 = not deployed, 01 = deploying, 10 = deployed
    uint16_t uart0Initialized : 1;
    uint16_t uart1Initialized : 1;
    uint16_t adcBattRT : 12;

    uint8_t sequenceNumber;

    uint32_t outputPinStateBits;

    uint32_t lowerResetActionBits;
    uint8_t upperResetActionBits;

    uint16_t vLanderSense : 7; // upper 7 bits (from 12-bit resolution data)
    uint16_t battTemp : 9; // upper 9 bits (from 12-bit resolution data)

    uint32_t vSysAllSens : 5; // upper 5 bits (from 12-bit resolution data)
    uint32_t iSysAllSense : 9; // LOWER 9 bits (from 12-bit resolution data)
    uint32_t vBattSense : 9; // upper 9 bits (from 12-bit resolution data)
    uint32_t vcc24 : 7; // upper 7 bits (from 12-bit resolution data)
    uint32_t heatingControlEnabled : 1;
    uint32_t heating : 1;

    uint16_t vcc2Point5 : 5; // upper 5 bits (from 12-bit resolution data)
    uint16_t vcc2Point8 : 5; // upper 5 bits (from 12-bit resolution data)
    uint16_t vcc28 : 6; // upper 6 bits (from 12-bit resolution data)

    uint16_t kpHeater;

    uint16_t heaterPwmLimit;

    uint16_t heaterSetpoint;

    uint16_t heaterOnValue;

    uint16_t heaterOffValue;

    uint16_t heaterDutyCyclePeriod;

    uint16_t heaterPwmValue;

    uint8_t rawBatteryCharge[2];

    uint8_t rawBatteryVoltage[2];

    uint8_t rawBatteryCurrent[2];

    uint8_t rawFuelGaugeTemp[2];

    uint8_t battChargeTelem;

    uint8_t battCurrTelem;
} DetailedReport;

/**
 * @brief Generates a transit heartbeat message for transmission to ground.
 *
 * Since the struct is packed, we want to serialize with little endianness, and the MSP430 has little endianness,
 * serializing this message it is as simple as casting the struct to uint8_t*.
 *
 * @param i2cReadings
 * @param adcValues
 * @param hParams
 * @param hb
 * @return
 */
GroundMsgs__Status GroundMsgs__generateFlightEarthHeartbeat(I2C_Sensors__Readings* i2cReadings,
                                                            AdcValues* adcValues,
                                                            HeaterParams* hParams,
                                                            FlightEarthHeartbeat* hb);

/**
 * @brief Generates a serialized full heartbeat message for transmission to ground.
 *
 * Since the struct is packed, we want to serialize with little endianness, and the MSP430 has little endianness,
 * serializing this message it is as simple as casting the struct to uint8_t*.
 *
 * Note that all uint16_t values are serialized with little endianness.
 *
 * @param i2cReadings
 * @param adcValues
 * @param hParams
 * @param stateAsUint
 * @param hb
 * @return
 */
GroundMsgs__Status GroundMsgs__generateFullEarthHeartbeat(I2C_Sensors__Readings* i2cReadings,
                                                          AdcValues* adcValues,
                                                          HeaterParams* hParams,
                                                          uint8_t stateAsUint,
                                                          FullEarthHeartbeat* hb);

GroundMsgs__Status GroundMsgs__generateDetailedReport(I2C_Sensors__Readings* i2cReadings,
                                                      AdcValues* adcValues,
                                                      WatchdogStateDetails* details,
                                                      DetailedReport* hb);

#ifdef __cplusplus
} /* close extern "C" */
#endif

#endif /* __WATCHDOG_GROUND_MSGS_H__ */
