// i2c_sensors.h

#ifndef __I2C_SENSORS_H__
#define __I2C_SENSORS_H__

#include <stdint.h>

#define I2C_SLAVE_ADDR      100 //0b1100100 //i2c address of LTC2944IDD#PBF fuel gauge

// Bit flags that, if set, indicate that data was not updated because
// the slave device did not acknowledge a transmitted byte.
typedef enum I2C_Sensors__NACK_Status {
    I2C_SENSORS__NACK__BATT_CHARGE = 0x01,
    I2C_SENSORS__NACK__BATT_VOLTAGE = 0x02,
    I2C_SENSORS__NACK__BATT_CURRENT = 0x04,
    I2C_SENSORS__NACK__FUEL_GAUGE_TEMP = 0x08
} I2C_Sensors__NACK_Status;

typedef struct I2C_Sensors__Readings {
    uint8_t raw_battery_charge[2];
    uint8_t raw_battery_voltage[2];
    uint8_t raw_battery_current[2];
    uint8_t raw_fuel_gauge_temp[2];
    uint8_t batt_charge_telem;
    uint8_t batt_curr_telem;

    uint8_t nackMask;
} I2C_Sensors__Readings;

typedef enum I2C_Sensors__RegisterAddrs {
    REG_ADDR__STATUS = 0,                 //read only
    REG_ADDR__CONTROL = 1,
    REG_ADDR__ACCUMULATED_CHARGE_MSB = 2,
    REG_ADDR__ACCUMULATED_CHARGE_LSB = 3,
    REG_ADDR__CHARGE_THRESHOLD_HIGH_MSB = 4,
    REG_ADDR__CHARGE_THRESHOLD_HIGH_LSB = 5,
    REG_ADDR__CHARGE_THRESHOLD_LOW_MSB = 6,
    REG_ADDR__CHARGE_THRESHOLD_LOW_LSB = 7,
    REG_ADDR__VOLTAGE_MSB = 8,                //read only
    REG_ADDR__VOLTAGE_LSB = 9,                //read only
    REG_ADDR__VOLTAGE_THRESHOLD_HIGH_MSB = 10,
    REG_ADDR__VOLTAGE_THRESHOLD_HIGH_LSB = 11,
    REG_ADDR__VOLTAGE_THRESHOLD_LOW_MSB = 12,
    REG_ADDR__VOLTAGE_THRESHOLD_LOW_LSB = 13,
    REG_ADDR__CURRENT_MSB = 14,                //read only
    REG_ADDR__CURRENT_LSB = 15,                //read only
    REG_ADDR__CURRENT_THRESHOLD_HIGH_MSB = 16,
    REG_ADDR__CURRENT_THRESHOLD_HIGH_LSB = 17,
    REG_ADDR__CURRENT_THRESHOLD_LOW_MSB = 18,
    REG_ADDR__CURRENT_THRESHOLD_LOW_LSB = 19,
    REG_ADDR__TEMPERATURE_MSB = 20,           //read only
    REG_ADDR__TEMPERATURE_LSB = 21,           //read only
    REG_ADDR__TEMPERATURE_THRESHOLD_HIGH = 22,
    REG_ADDR__TEMPERATURE_THRESHOLD_LOW = 23,
    REG_ADDR__MAX_NB_CMDS = 24
} I2C_Sensors__RegisterAddrs;

typedef enum I2C_Sensors__Status {
    I2C_SENSORS__STATUS__INCOMPLETE = 1, /* Operation still waiting to complete */
    I2C_SENSORS__STATUS__SUCCESS_DONE = 0, /* Operation suceeded. */
    I2C_SENSORS__STATUS__ERROR__NULL = -1, /* A required argument or a member of an argument was NULL */

    /* Couldn't get one or more readings because I2C slave didn't acknowledge a byte */
    I2C_SENSORS__STATUS__ERROR__DONE_WITH_NACKS = -2,

    I2C_SENSORS__STATUS__ERROR__READINGS_NOT_STARTED = -3,
    I2C_SENSORS__STATUS__ERROR__READINGS_IN_PROGRESS = -4,
    I2C_SENSORS__STATUS__ERROR__INTERNAL = -255
} I2C_Sensors__Status;

typedef enum GaugeReadingState {
    GRS__UNKNOWN = 0,
    GRS__CHARGE_LSB,
    GRS__CHARGE_MSB,
    GRS__VOLTAGE_LSB,
    GRS__VOLTAGE_MSB,
    GRS__CURRENT_LSB,
    GRS__CURRENT_MSB,
    GRS__GAUGE_TEMP_LSB,
    GRS__GAUGE_TEMP_MSB,
    GRS__DONE
} GaugeReadingState;

typedef struct INS_Sensors__InternalState {
    GaugeReadingState gState;
    I2C_Sensors__Readings readings;
} INS_Sensors__InternalState;

extern INS_Sensors__InternalState internals;

void I2C_Sensors__init();

void I2C_Sensors__initiateGaugeReadings();

I2C_Sensors__Status
I2C_Sensors__getGaugeReadingStatus(I2C_Sensors__Readings* readings);

I2C_Sensors__Status I2C_Sensors__fuelGaugeLowPowerBlocking();

I2C_Sensors__Status I2C_Sensors__initializeFuelGaugeBlocking();

I2C_Sensors__Status I2C_Sensors__readFuelGaugeControlRegisterBlocking(uint8_t* data);

void I2C_Sensors__spinOnce();

#endif // #ifndef __I2C_SENSORS_H__
