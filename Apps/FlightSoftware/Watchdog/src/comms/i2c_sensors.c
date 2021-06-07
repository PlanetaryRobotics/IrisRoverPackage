// i2c_sensors.c

#include "include/comms/i2c_sensors.h"
#include "include/drivers/i2c.h"
#include "include/common.h"

#include <string.h>
#include <assert.h>

//###########################################################
// Private definitions and globals
//###########################################################

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

static INS_Sensors__InternalState internals = {
    .gState = GRS__UNKNOWN,
    .readings = { 0 }
};

//###########################################################
// Private function declarations
//###########################################################

/**
 * @private
 *
 * @brief Attempts to progress the process of reading a value from a specified device register. Will not block.
 *
 * This will start a new read if no transaction is active, monitor the status of an ongoing transaction if one is
 * active, and handle returning the read value and transitioning the state machine if a value has been received. This
 * will also handle the case that the transaction completed due to not receiving an acknowledgment from the other
 * device in response to a byte transmitted from this device. If the `nackMaskBit` is already set in the `I2C_Readings`
 * then this read will be skipped.
 *
 * @param devAddr The device address to read from.
 * @param regAddr The register address to read from.
 * @param nackMaskBit The bit flag to set if no response is received to a transmitted byte.
 * @param nextState The next state to transition to when this read completes.
 * @param output A return parameter that will be set to the value read from the specified device register if `done` and
 *               `gotOutput` are both set to TRUE.
 * @param done A return parameter that will specify whether or not reading this device register is complete.
 * @param gotOutput A return parameter that, if `done` is TRUE, will specify if whether or not we successfully read an
 *                  output byte from the specified device register.
 *
 * @return Whether or not the state machine should continue spinning.
 */
static BOOL I2C_Sensors__readRegNonBlocking(uint8_t devAddr,
                                            uint8_t regAddr,
                                            uint8_t nackMaskBit,
                                            GaugeReadingState nextState,
                                            void* output,
                                            BOOL* done,
                                            BOOL* gotOutput);

/**
 * @private
 *
 * @private Attempts to progress the process of writing a value to a specified device register. Will not block.
 *
 * This will start a new write if no transaction is active, monitor the status of an ongoing transaction if one is
 * active, and handle the case of the transaction completed due to not receiving an acknowledgment from the other device
 * in response to a byte transmitted from this device.
 *
 * @param devAddr The device address to write to.
 * @param regAddr The register address to write to.
 * @param data The value to write to the specified device register.
 * @param done A return parameter that will specify whether or not the write is complete.
 * @param success A return parameter that, if `done` is TRUE, will specify whether or not the write completed
 *                successfully.
 */
static void I2C_Sensors__writeRegNonBlocking(uint8_t devAddr,
                                             uint8_t regAddr,
                                             uint8_t data,
                                             BOOL* done,
                                             BOOL* success);

/**
 * @private
 *
 * @brief Spins the `GRS__CHARGE_LSB` state, which involves reading the LSB of the battery charge. Will not block.
 *
 * Populates the `raw_battery_charge[1]` value in the `I2C_Readings` struct when the read completes successfully. Upon
 * completion, will transition the state machine to the `GRS__CHARGE_MSB` state.
 *
 * @return Whether or not to continue spinning the state machine.
 */
static BOOL I2C_Sensors__chargeLsb();

/**
 * @private
 *
 * @brief Spins the `GRS__CHARGE_MSB` state, which involves reading the MSB of the battery charge. Will not block.
 *
 * Populates the `raw_battery_charge[0]` and `batt_charge_telem` values in the `I2C_Readings` struct if the read
 * completes successfully. Upon completion, will transition the state machine to the `GRS__VOLTAGE_LSB` state.
 *
 * @return Whether or not to continue spinning the state machine.
 */
static BOOL I2C_Sensors__chargeMsb();

/**
 * @private
 *
 * @brief Spins the `GRS__VOLTAGE_LSB` state, which involves reading the LSB of the battery voltage. Will not block.
 *
 * Populates the `raw_battery_voltage[1]` value in the `I2C_Readings` struct when the read completes successfully. Upon
 * completion, will transition the state machine to the `GRS__VOLTAGE_MSB` state.
 *
 * @return Whether or not to continue spinning the state machine.
 */
static BOOL I2C_Sensors__voltageLsb();

/**
 * @private
 *
 * @brief Spins the `GRS__VOLTAGE_MSB` state, which involves reading the MSB of the battery voltage. Will not block.
 *
 * Populates the `raw_battery_voltage[0]` value in the `I2C_Readings` struct when the read completes successfully. Upon
 * completion, will transition the state machine to the `GRS__CURRENT_LSB` state.
 *
 * @return Whether or not to continue spinning the state machine.
 */
static BOOL I2C_Sensors__voltageMsb();

/**
 * @private
 *
 * @brief Spins the `GRS__CURRENT_LSB` state, which involves reading the LSB of the battery current. Will not block.
 *
 * Populates the `raw_battery_current[1]` value in the `I2C_Readings` struct when the read completes successfully. Upon
 * completion, will transition the state machine to the `GRS__CURRENT_MSB` state.
 *
 * @return Whether or not to continue spinning the state machine.
 */
static BOOL I2C_Sensors__currentLsb();

/**
 * @private
 *
 * @brief Spins the `GRS__CURRENT_MSB` state, which involves reading the MSB of the battery current. Will not block.
 *
 * Populates the `raw_battery_current[0]` and `batt_curr_telem` values in the `I2C_Readings` struct when the read
 * completes successfully. Upon completion, will transition the state machine to the `GRS__GAUGE_TEMP_LSB` state.
 *
 * @return Whether or not to continue spinning the state machine.
 */
static BOOL I2C_Sensors__currentMsb();

/**
 * @private
 *
 * @brief Spins the `GRS__GAUGE_TEMP_LSB` state, which involves reading the LSB of the fuel gauge temperature. Will not
 *        block.
 *
 * Populates the `raw_fuel_gauge_temp[1]` value in the `I2C_Readings` struct when the read completes successfully. Upon
 * completion, will transition the state machine to the `GRS__GAUGE_TEMP_MSB` state.
 *
 * @return Whether or not to continue spinning the state machine.
 */
static BOOL I2C_Sensors__gaugeTempLsb();

/**
 * @private
 *
 * @brief Spins the `GRS__GAUGE_TEMP_MSB` state, which involves reading the MSB of the fuel gauge temperature. Will not
 *        block.
 *
 * Populates the `raw_fuel_gauge_temp[0]` value in the `I2C_Readings` struct when the read completes successfully. Upon
 * completion, will transition the state machine to the `GRS__DONE` state.
 *
 * @return Whether or not to continue spinning the state machine.
 */
static BOOL I2C_Sensors__gaugeTempMsb();

//###########################################################
// Public function definitions
//###########################################################

void I2C_Sensors__init()
{
    // Just init the generic I2C module we depend on
    I2C__init();
}

void I2C_Sensors__initiateGaugeReadings()
{
    internals.gState = GRS__CHARGE_LSB;
    internals.readings.nackMask = 0U;
}

I2C_Sensors__Status
I2C_Sensors__getGaugeReadingStatus(I2C_Sensors__Readings* readings)
{
    if (NULL == readings) {
        return I2C_SENSORS__STATUS__ERROR__NULL;
    }

    switch (internals.gState) {
        case GRS__UNKNOWN:
            return I2C_SENSORS__STATUS__ERROR__READINGS_NOT_STARTED;

        case GRS__CHARGE_LSB:
        case GRS__CHARGE_MSB:
        case GRS__VOLTAGE_LSB:
        case GRS__VOLTAGE_MSB:
        case GRS__CURRENT_LSB:
        case GRS__CURRENT_MSB:
        case GRS__GAUGE_TEMP_LSB:
        case GRS__GAUGE_TEMP_MSB:
            return I2C_SENSORS__STATUS__INCOMPLETE;

        case GRS__DONE:
            memcpy(readings, &internals.readings, sizeof(*readings));

            if (internals.readings.nackMask == 0U) {
                return I2C_SENSORS__STATUS__SUCCESS_DONE;
            } else {
                return I2C_SENSORS__STATUS__ERROR__DONE_WITH_NACKS;
            }

        default:
            return I2C_SENSORS__STATUS__ERROR__INTERNAL;
    }
}

I2C_Sensors__Status I2C_Sensors__fuelGaugeLowPowerBlocking()
{
    static const uint8_t FUEL_GAUGE_LOW_POWER = 0b00101001;

    if (!((GRS__DONE == internals.gState)
          || (GRS__UNKNOWN == internals. gState))) {
        return I2C_SENSORS__STATUS__ERROR__READINGS_IN_PROGRESS;
    }

    BOOL inProgress = TRUE;
    BOOL success = FALSE;

    while (inProgress) {
        BOOL done = FALSE;

        I2C__spinOnce();

        I2C_Sensors__writeRegNonBlocking(I2C_SLAVE_ADDR,
                                         REG_ADDR__CONTROL,
                                         FUEL_GAUGE_LOW_POWER,
                                         &done,
                                         &success);

        inProgress = !done;

        if (inProgress) {
            __delay_cycles(100);
        }
    }

    if (success) {
        return I2C_SENSORS__STATUS__SUCCESS_DONE;
    } else {
        return I2C_SENSORS__STATUS__ERROR__DONE_WITH_NACKS;
    }
}

I2C_Sensors__Status I2C_Sensors__initializeFuelGaugeBlocking()
{
    // set control_reg[7:6] to 01 do one conversion, 10 to convert every 10s,
    //      set to 00 to sleep, set to 11 to continuously convert
    // set control_reg[5:3] to 101 for M of 1024 for coulomb counter (see datasheet)
    // control_ref[2:1] not used on SBC (pin its related to is floating)
    // must leave control_reg[0] to 0
    static const uint8_t FUEL_GAUGE_INIT = 0b10101000;

    if (!((GRS__DONE == internals.gState)
          || (GRS__UNKNOWN == internals. gState))) {
        return I2C_SENSORS__STATUS__ERROR__READINGS_IN_PROGRESS;
    }


    BOOL inProgress = TRUE;
    BOOL success = TRUE;
    uint8_t stage = 1;

    while (inProgress) {
        BOOL done = FALSE;
        BOOL stageSuccess = FALSE;

        I2C__spinOnce();

        switch (stage) {
            // initialize charge register with maximum battery capacity (see data sheet
            // for conversion from 3500 mAh, M is 1048)
            case 1:
                I2C_Sensors__writeRegNonBlocking(I2C_SLAVE_ADDR,
                                                 REG_ADDR__ACCUMULATED_CHARGE_MSB,
                                                 0xA0,
                                                 &done,
                                                 &stageSuccess);
                break;

            case 2:
                I2C_Sensors__writeRegNonBlocking(I2C_SLAVE_ADDR,
                                                 REG_ADDR__ACCUMULATED_CHARGE_LSB,
                                                 0xD8,
                                                 &done,
                                                 &stageSuccess);
                break;

            case 3:
                I2C_Sensors__writeRegNonBlocking(I2C_SLAVE_ADDR,
                                                 REG_ADDR__CONTROL,
                                                 FUEL_GAUGE_INIT,
                                                 &done,
                                                 &stageSuccess);
                break;
        }

        if (done) {
            // Only consider successful if all are successful
            success = success && stageSuccess;

            if (stage >= 3) {
                inProgress = FALSE;
            } else {
                stage++;
            }
        }

        if (inProgress) {
            __delay_cycles(100);
        }
    }

    if (success) {
        return I2C_SENSORS__STATUS__SUCCESS_DONE;
    } else {
        return I2C_SENSORS__STATUS__ERROR__DONE_WITH_NACKS;
    }
}

I2C_Sensors__Status I2C_Sensors__readFuelGaugeControlRegisterBlocking(uint8_t* data)
{
    if (NULL == data) {
        return I2C_SENSORS__STATUS__ERROR__NULL;
    }

    if (!((GRS__DONE == internals.gState)
          || (GRS__UNKNOWN == internals. gState))) {
        return I2C_SENSORS__STATUS__ERROR__READINGS_IN_PROGRESS;
    }

    BOOL inProgress = TRUE;
    BOOL success = FALSE;
    GaugeReadingState initialState = internals.gState;
    uint8_t initialNackMask = internals.readings.nackMask;

    while (inProgress) {
        BOOL done = FALSE;

        I2C__spinOnce();

        I2C_Sensors__readRegNonBlocking(I2C_SLAVE_ADDR,
                                        REG_ADDR__CONTROL,
                                        0,
                                        initialState,
                                        data,
                                        &done,
                                        &success);

        inProgress = !done;

        if (inProgress) {
            __delay_cycles(100);
        }
    }

    internals.gState = initialState;
    internals.readings.nackMask = initialNackMask;

    if (success) {
        return I2C_SENSORS__STATUS__SUCCESS_DONE;
    } else {
        return I2C_SENSORS__STATUS__ERROR__DONE_WITH_NACKS;
    }
}

void I2C_Sensors__spinOnce()
{
    BOOL keepSpinning = TRUE;

    while (keepSpinning) {
        I2C__spinOnce();

        switch (internals.gState) {
            case GRS__UNKNOWN:
                keepSpinning = FALSE;
                break;

            case GRS__CHARGE_LSB:
                keepSpinning = I2C_Sensors__chargeLsb();
                break;

            case GRS__CHARGE_MSB:
                keepSpinning = I2C_Sensors__chargeMsb();
                break;

            case GRS__VOLTAGE_LSB:
                keepSpinning = I2C_Sensors__voltageLsb();
                break;

            case GRS__VOLTAGE_MSB:
                keepSpinning = I2C_Sensors__voltageMsb();
                break;

            case GRS__CURRENT_LSB:
                keepSpinning = I2C_Sensors__currentLsb();
                break;

            case GRS__CURRENT_MSB:
                keepSpinning = I2C_Sensors__currentMsb();
                break;

            case GRS__GAUGE_TEMP_LSB:
                keepSpinning = I2C_Sensors__gaugeTempLsb();
                break;

            case GRS__GAUGE_TEMP_MSB:
                keepSpinning = I2C_Sensors__gaugeTempMsb();
                break;

            case GRS__DONE:
                keepSpinning = FALSE;
                break;

            default:
                break;
        }
    }
}

//###########################################################
// Private function definitions
//###########################################################

static BOOL I2C_Sensors__readRegNonBlocking(uint8_t devAddr,
                                            uint8_t regAddr,
                                            uint8_t nackMaskBit,
                                            GaugeReadingState nextState,
                                            void* output,
                                            BOOL* done,
                                            BOOL* gotOutput)
{
    static I2C__TransactionStatus tStatus = { 0 };

    I2C__Status stat = I2C__getTransactionStatus(&tStatus);

    BOOL startNewRead = FALSE;
    BOOL continueSpinning = FALSE;

    if (I2C__STATUS__ERROR__NO_TRANSACTION == stat) {
        startNewRead = TRUE;
    } else if (I2C__STATUS__SUCCESS == stat) {
        BOOL statusForRequestedTransaction = FALSE;
        BOOL prevReadDone = FALSE;

        if ((tStatus.devAddr == devAddr)
                && (tStatus.regAddr == regAddr)
                && (tStatus.type == I2C__TYPE__READ)) {
            // This transaction status is for the currently requested read
            statusForRequestedTransaction = TRUE;
        }

        if (I2C__TRANSACTION__DONE_SUCCESS == tStatus.state) {
            // We're now done with this read (and it was successful)
            if (statusForRequestedTransaction) {
                internals.gState = nextState;
                *((uint8_t*) output) = tStatus.data;
                *done = TRUE;
                *gotOutput = TRUE;
                continueSpinning = TRUE;
            }

            prevReadDone = TRUE;
        } else if (I2C__TRANSACTION__DONE_ERROR_NACK == tStatus.state) {
            // We're now done with this read (and it failed)
            if (statusForRequestedTransaction) {
                internals.readings.nackMask |= nackMaskBit;
                internals.gState = nextState;
                *done = TRUE;
                *gotOutput = FALSE;
                continueSpinning = TRUE;
            }

            prevReadDone = TRUE;
        }

        startNewRead = (!statusForRequestedTransaction && prevReadDone);
    }

    if (startNewRead) {
        // If the NACK mask bit for this register is already set, just skip
        // this register
        if (internals.readings.nackMask & nackMaskBit != 0) {
            internals.gState = nextState;
            *done = TRUE;
            *gotOutput = FALSE;
            return TRUE;
        }

        stat = I2C__read(devAddr, regAddr);

        *done = FALSE;
        *gotOutput = FALSE;
    }


    return continueSpinning;
}

static void I2C_Sensors__writeRegNonBlocking(uint8_t devAddr,
                                             uint8_t regAddr,
                                             uint8_t data,
                                             BOOL* done,
                                             BOOL* success)
{
    static I2C__TransactionStatus tStatus = { 0 };

    I2C__Status stat = I2C__getTransactionStatus(&tStatus);

    BOOL startNewWrite = FALSE;

    if (I2C__STATUS__ERROR__NO_TRANSACTION == stat) {
        startNewWrite = TRUE;
    } else if (I2C__STATUS__SUCCESS == stat) {
        BOOL statusForRequestedTransaction = FALSE;
        BOOL prevWriteDone = FALSE;

        if ((tStatus.devAddr == devAddr)
                        && (tStatus.regAddr == regAddr)
                        && (tStatus.type == I2C__TYPE__WRITE)) {
            // This transaction status is for the currently requested write
            statusForRequestedTransaction = TRUE;
        }

        if (I2C__TRANSACTION__DONE_SUCCESS == tStatus.state) {
            // We're now done with this write (and it was successful)
            if (statusForRequestedTransaction) {
                *done = TRUE;
                *success = TRUE;
            }

            prevWriteDone = TRUE;
        } else if (I2C__TRANSACTION__DONE_ERROR_NACK == tStatus.state) {
            // We're now done with this write (and it failed)
            if (statusForRequestedTransaction) {
                *done = TRUE;
                *success = FALSE;
            }

            prevWriteDone = TRUE;
        }

        startNewWrite = (!statusForRequestedTransaction && prevWriteDone);
    }

    if (startNewWrite) {
        stat = I2C__write(devAddr, regAddr, data);

        *done = FALSE;
        *success = TRUE;
    }
}

static BOOL I2C_Sensors__chargeLsb()
{
    BOOL done = FALSE, gotOutput = FALSE;
    return I2C_Sensors__readRegNonBlocking(I2C_SLAVE_ADDR,
                                           REG_ADDR__ACCUMULATED_CHARGE_LSB,
                                           I2C_SENSORS__NACK__BATT_CHARGE,
                                           GRS__CHARGE_MSB,
                                           internals.readings.raw_battery_charge + 1,
                                           &done,
                                           &gotOutput);
}

static BOOL I2C_Sensors__chargeMsb()
{
    BOOL done = FALSE, gotOutput = FALSE;
    BOOL result = I2C_Sensors__readRegNonBlocking(I2C_SLAVE_ADDR,
                                                  REG_ADDR__ACCUMULATED_CHARGE_MSB,
                                                  I2C_SENSORS__NACK__BATT_CHARGE,
                                                  GRS__VOLTAGE_LSB,
                                                  internals.readings.raw_battery_charge,
                                                  &done,
                                                  &gotOutput);

    if (done && gotOutput) {
        uint8_t *raw_battery_charge = internals.readings.raw_battery_charge;
        uint16_t chargeUShort = (uint16_t)(raw_battery_charge[1] + (raw_battery_charge[0] << 8));
        internals.readings.batt_charge_telem = (uint8_t)(chargeUShort >> 10) * 3;
    }

    return result;
}

static BOOL I2C_Sensors__voltageLsb()
{
    BOOL done = FALSE, gotOutput = FALSE;
    return I2C_Sensors__readRegNonBlocking(I2C_SLAVE_ADDR,
                                           REG_ADDR__VOLTAGE_LSB,
                                           I2C_SENSORS__NACK__BATT_VOLTAGE,
                                           GRS__VOLTAGE_MSB,
                                           internals.readings.raw_battery_voltage + 1,
                                           &done,
                                           &gotOutput);
}

static BOOL I2C_Sensors__voltageMsb()
{
    BOOL done = FALSE, gotOutput = FALSE;
    return I2C_Sensors__readRegNonBlocking(I2C_SLAVE_ADDR,
                                           REG_ADDR__VOLTAGE_MSB,
                                           I2C_SENSORS__NACK__BATT_VOLTAGE,
                                           GRS__CURRENT_LSB,
                                           internals.readings.raw_battery_voltage,
                                           &done,
                                           &gotOutput);
}

static BOOL I2C_Sensors__currentLsb()
{
    BOOL done = FALSE, gotOutput = FALSE;
    return I2C_Sensors__readRegNonBlocking(I2C_SLAVE_ADDR,
                                           REG_ADDR__CURRENT_LSB,
                                           I2C_SENSORS__NACK__BATT_CURRENT,
                                           GRS__CURRENT_MSB,
                                           internals.readings.raw_battery_current + 1,
                                           &done,
                                           &gotOutput);
}

static BOOL I2C_Sensors__currentMsb()
{
    BOOL done = FALSE, gotOutput = FALSE;
    BOOL result = I2C_Sensors__readRegNonBlocking(I2C_SLAVE_ADDR,
                                                  REG_ADDR__CURRENT_MSB,
                                                  I2C_SENSORS__NACK__BATT_CURRENT,
                                                  GRS__GAUGE_TEMP_LSB,
                                                  internals.readings.raw_battery_current,
                                                  &done,
                                                  &gotOutput);

    if (done && gotOutput) {
        uint8_t *raw_battery_current = internals.readings.raw_battery_current;
        uint16_t bCurrUShort = (uint16_t)(32767 - raw_battery_current[1] - (raw_battery_current[0] << 8));

        if (bCurrUShort > 17407) {
            //exceeds maximum value of 0.6 A
            internals.readings.batt_curr_telem = 255;
        } else {
            internals.readings.batt_curr_telem = (uint8_t)(bCurrUShort >> 7 );
        }
    }

    return result;
}

static BOOL I2C_Sensors__gaugeTempLsb()
{
    BOOL done = FALSE, gotOutput = FALSE;
    return I2C_Sensors__readRegNonBlocking(I2C_SLAVE_ADDR,
                                           REG_ADDR__TEMPERATURE_LSB,
                                           I2C_SENSORS__NACK__FUEL_GAUGE_TEMP,
                                           GRS__GAUGE_TEMP_MSB,
                                           internals.readings.raw_fuel_gauge_temp + 1,
                                           &done,
                                           &gotOutput);
}

static BOOL I2C_Sensors__gaugeTempMsb()
{
    BOOL done = FALSE, gotOutput = FALSE;
    return I2C_Sensors__readRegNonBlocking(I2C_SLAVE_ADDR,
                                           REG_ADDR__TEMPERATURE_MSB,
                                           I2C_SENSORS__NACK__FUEL_GAUGE_TEMP,
                                           GRS__DONE,
                                           internals.readings.raw_fuel_gauge_temp,
                                           &done,
                                           &gotOutput);
}
