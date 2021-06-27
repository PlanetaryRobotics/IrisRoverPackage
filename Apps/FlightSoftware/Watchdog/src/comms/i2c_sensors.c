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

typedef enum FuelGaugeInitState {
    FGI__UNKNOWN = 0,
    FGI__ACCUMULATED_CHARGE_MSB,
    FGI__ACCUMULATED_CHARGE_LSB,
    FGI__CONTROL,
    FGI__DONE,
    FGI__FAILED_NACK
} FuelGaugeInitState;

typedef enum ReadControlState {
    RC__UNKNOWN = 0,
    RC__READING,
    RC__DONE,
    RC__FAILED_NACK
} WriteControlState;

typedef enum WriteLowPowerState {
    WLP__UNKNOWN = 0,
    WLP__WRITING,
    WLP__DONE,
    WLP__FAILED_NACK
} WriteLowPowerState;

typedef struct INS_Sensors__InternalState {
    I2C_Sensors__Action activeAction;

    GaugeReadingState grsState;
    FuelGaugeInitState fgiState;
    ReadControlState rcState;
    WriteLowPowerState wlpState;

    I2C_Sensors__Readings readings;
    uint8_t controlRegisterReadValue;
} INS_Sensors__InternalState;

static const uint8_t FUEL_GAUGE_CONTROL_LOW_POWER = 0b00101001;

// set control_reg[7:6] to 01 do one conversion, 10 to convert every 10s,
//      set to 00 to sleep, set to 11 to continuously convert
// set control_reg[5:3] to 101 for M of 1024 for coulomb counter (see datasheet)
// control_ref[2:1] not used on SBC (pin its related to is floating)
// must leave control_reg[0] to 0
static const uint8_t FUEL_GAUGE_CONTROL_INIT = 0b10101000;
static const uint8_t FUEL_GAUGE_CHARGE_ACCUM_MSB_INIT = 0xA0;
static const uint8_t FUEL_GAUGE_CHARGE_ACCUM_LSB_INIT = 0xD8

static INS_Sensors__InternalState internals = {
    .activeAction = I2C_SENSORS__ACTIONS__INACTIVE,
    .grsState = GRS__UNKNOWN,
    .fgiState = FGI__UNKNOWN,
    .rcState = RC__UNKNOWN,
    .wlpState = WLP__UNKNOWN,
    .readings = { 0 },
    .controlRegisterReadValue = 0
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
 * @param isGaugeReading Whether or not this call is part of a gauge sensor reading.
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
                                            BOOL isGaugeReading,
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
 * @param isGaugeInit Whether or not this write is part of a fuel gauge initialization sequence.
 * @param nextState The next fuel gauge initialization state to transition to after this read is done if `isGaugeInit`
 *                  is TRUE.
 * @param done A return parameter that will specify whether or not the write is complete.
 * @param success A return parameter that, if `done` is TRUE, will specify whether or not the write completed
 *                successfully.
 */
static void I2C_Sensors__writeRegNonBlocking(uint8_t devAddr,
                                             uint8_t regAddr,
                                             uint8_t data,
                                             BOOL isGaugeInit,
                                             FuelGaugeInitState nextState,
                                             BOOL* done,
                                             BOOL* success);

/**
 * @private
 *
 * @brief Spins the `GRS__CHARGE_LSB` state, which involves reading the LSB of the battery charge. Will not block.
 *
 * Populates the `raw_battery_charge[1]` value in the `I2C_Readings` struct when the read completes successfully. Upon
 * completion, will transition the gauge reading state machine to the `GRS__CHARGE_MSB` state.
 *
 * @return Whether or not to continue spinning the gauge reading state machine.
 */
static BOOL I2C_Sensors__chargeLsb(void);

/**
 * @private
 *
 * @brief Spins the `GRS__CHARGE_MSB` state, which involves reading the MSB of the battery charge. Will not block.
 *
 * Populates the `raw_battery_charge[0]` and `batt_charge_telem` values in the `I2C_Readings` struct if the read
 * completes successfully. Upon completion, will transition the gauge reading state machine to the `GRS__VOLTAGE_LSB`
 * state.
 *
 * @return Whether or not to continue spinning the gauge reading state machine.
 */
static BOOL I2C_Sensors__chargeMsb(void);

/**
 * @private
 *
 * @brief Spins the `GRS__VOLTAGE_LSB` state, which involves reading the LSB of the battery voltage. Will not block.
 *
 * Populates the `raw_battery_voltage[1]` value in the `I2C_Readings` struct when the read completes successfully. Upon
 * completion, will transition the gauge reading state machine to the `GRS__VOLTAGE_MSB` state.
 *
 * @return Whether or not to continue spinning the gauge reading state machine.
 */
static BOOL I2C_Sensors__voltageLsb(void);

/**
 * @private
 *
 * @brief Spins the `GRS__VOLTAGE_MSB` state, which involves reading the MSB of the battery voltage. Will not block.
 *
 * Populates the `raw_battery_voltage[0]` value in the `I2C_Readings` struct when the read completes successfully. Upon
 * completion, will transition the gauge reading state machine to the `GRS__CURRENT_LSB` state.
 *
 * @return Whether or not to continue spinning the gauge reading state machine.
 */
static BOOL I2C_Sensors__voltageMsb(void);

/**
 * @private
 *
 * @brief Spins the `GRS__CURRENT_LSB` state, which involves reading the LSB of the battery current. Will not block.
 *
 * Populates the `raw_battery_current[1]` value in the `I2C_Readings` struct when the read completes successfully. Upon
 * completion, will transition the gauge reading state machine to the `GRS__CURRENT_MSB` state.
 *
 * @return Whether or not to continue spinning the gauge reading state machine.
 */
static BOOL I2C_Sensors__currentLsb(void);

/**
 * @private
 *
 * @brief Spins the `GRS__CURRENT_MSB` state, which involves reading the MSB of the battery current. Will not block.
 *
 * Populates the `raw_battery_current[0]` and `batt_curr_telem` values in the `I2C_Readings` struct when the read
 * completes successfully. Upon completion, will transition the gauge reading state machine to the `GRS__GAUGE_TEMP_LSB`
 * state.
 *
 * @return Whether or not to continue spinning the gauge reading state machine.
 */
static BOOL I2C_Sensors__currentMsb(void);

/**
 * @private
 *
 * @brief Spins the `GRS__GAUGE_TEMP_LSB` state, which involves reading the LSB of the fuel gauge temperature. Will not
 *        block.
 *
 * Populates the `raw_fuel_gauge_temp[1]` value in the `I2C_Readings` struct when the read completes successfully. Upon
 * completion, will transition the gauge reading state machine to the `GRS__GAUGE_TEMP_MSB` state.
 *
 * @return Whether or not to continue spinning the gauge reading state machine.
 */
static BOOL I2C_Sensors__gaugeTempLsb(void);

/**
 * @private
 *
 * @brief Spins the `GRS__GAUGE_TEMP_MSB` state, which involves reading the MSB of the fuel gauge temperature. Will not
 *        block.
 *
 * Populates the `raw_fuel_gauge_temp[0]` value in the `I2C_Readings` struct when the read completes successfully. Upon
 * completion, will transition the gauge reading state machine to the `GRS__DONE` state.
 *
 * @return Whether or not to continue spinning the gauge reading state machine.
 */
static BOOL I2C_Sensors__gaugeTempMsb(void);

/**
 * @private
 *
 * @brief Spins the `FGI__ACCUMULATED_CHARGE_MSB` state, which involves writing the MSB of the fuel gauge accumulated
 *        charge. Will not block.
 *
 * Upon completion, will transition the fuel gauge initialization state machine to the `FGI__ACCUMULATED_CHARGE_LSB`
 *  or `FGI__FAILED_NACK` state.
 *
 * @return Whether or not to continue spinning the fuel gauge initialization state machine.
 */
static BOOL I2C_Sensors__accumulatedChargeMsb(void);

/**
 * @private
 *
 * @brief Spins the `FGI__ACCUMULATED_CHARGE_LSB` state, which involves writing the LSB of the fuel gauge accumulated
 *        charge. Will not block.
 *
 * Upon completion, will transition the fuel gauge initialization state machine to the `FGI__CONTROL` or
 * `FGI__FAILED_NACK`state.
 *
 * @return Whether or not to continue spinning the fuel gauge initialization state machine.
 */
static BOOL I2C_Sensors__accumulatedChargeLsb(void);

/**
 * @private
 *
 * @brief Spins the `FGI__CONTROL` state, which involves writing the fuel gauge control byte. Will not block.
 *
 * Upon completion, will transition the fuel gauge initialization state machine to the `FGI__DONE` or `FGI__FAILED_NACK`
 * state.
 *
 * @return Whether or not to continue spinning the fuel gauge initialization state machine.
 */
static BOOL I2C_Sensors__writeControl(void);

/**
 * @private
 *
 * @brief Spins the read of the control byte to the control register of the fuel gauge. Will not block.
 *
 * Upon completion, will transition the read control state machine to the `RC__DONE` or `RC__FAILED_NACK` state.
 *
 * @return Whether or not to continue spinning the read control state machine.
 */
static BOOL I2C_Sensors__readControl(void);

/**
 * @private
 *
 * @brief Spins the write to put the fuel gauge in low power mode. Will not block.
 *
 * Upon completion, will transition the write low power state machine to the `WLP__DONE` or `WLP__FAILED_NACK` state.
 *
 * @return Whether or not to continue spinning the write low power state machine.
 */
static BOOL I2C_Sensors__lowPower(void);

//###########################################################
// Public function definitions
//###########################################################

void I2C_Sensors__init(void)
{
    // Just init the generic I2C module we depend on
    I2C__init();
}

void I2C_Sensors__stop(void)
{
    I2C__stop();
    I2C_Sensors__clearLastAction();
}

void I2C_Sensors__clearLastAction(void)
{
    internals.activeAction = I2C_SENSORS__ACTIONS__INACTIVE;

    // We could check what the last action was and only reset that one... or we could just reset them all.
    internals.grsState = GRS__UNKNOWN;
    internals.fgiState = FGI__UNKNOWN;
    internals.rcState = RC__UNKNOWN;
    internals.wlpState = WLP__UNKNOWN;
}

I2C_Sensors__Status I2C_Sensors__initiateGaugeReadings(void)
{
    if (I2C_SENSORS__ACTIONS__INACTIVE == internals.activeAction) {
        internals.activeAction = I2C_SENSORS__ACTIONS__GAUGE_READING;
        internals.grsState = GRS__CHARGE_LSB;
        internals.readings.nackMask = 0U;
        return I2C_SENSORS__STATUS__SUCCESS_DONE;
    } else {
        return I2C_SENSORS__STATUS__ERROR__ACTION_ALREADY_IN_PROGRESS;
    }
}

I2C_Sensors__Status I2C_Sensors__initiateFuelGaugeInitialization(void)
{
    if (I2C_SENSORS__ACTIONS__INACTIVE == internals.activeAction) {
        internals.activeAction = I2C_SENSORS__ACTIONS__GAUGE_INIT;
        internals.fgiState = FGI__ACCUMULATED_CHARGE_MSB;
        return I2C_SENSORS__STATUS__SUCCESS_DONE;
    } else {
        return I2C_SENSORS__STATUS__ERROR__ACTION_ALREADY_IN_PROGRESS;
    }
}

I2C_Sensors__Status I2C_Sensors__initiateReadControl(void)
{
    if (I2C_SENSORS__ACTIONS__INACTIVE == internals.activeAction) {
        internals.activeAction = I2C_SENSORS__ACTIONS__READ_GAUGE_CONTROL_REGISTER;
        internals.rcState = RC__READING;
        internals.controlRegisterReadValue = 0;
        return I2C_SENSORS__STATUS__SUCCESS_DONE;
    } else {
        return I2C_SENSORS__STATUS__ERROR__ACTION_ALREADY_IN_PROGRESS;
    }
}

I2C_Sensors__Status I2C_Sensors__initiateWriteLowPower(void)
{
    if (I2C_SENSORS__ACTIONS__INACTIVE == internals.activeAction) {
        internals.activeAction = I2C_SENSORS__ACTIONS__WRITE_GAUGE_LOW_POWER;
        internals.wlpState = WLP__WRITING;
        return I2C_SENSORS__STATUS__SUCCESS_DONE;
    } else {
        return I2C_SENSORS__STATUS__ERROR__ACTION_ALREADY_IN_PROGRESS;
    }
}

I2C_Sensors__Status
I2C_Sensors__getActionStatus(I2C_Sensors__Action* action,
                             I2C_Sensors__Readings* readings,
                             uint8_t* controlRegisterValue)
{
    if (NULL == action) {
        return I2C_SENSORS__STATUS__ERROR__NULL;
    }

    if (I2C_SENSORS__ACTIONS__INACTIVE == internals.activeAction) {
        *action = I2C_SENSORS__ACTIONS__INACTIVE;
        return I2C_SENSORS__STATUS__ERROR__NO_ACTION_IN_PROGRESS;
    }

    *action = internals.activeAction;

    switch (internals.activeAction) {
        case I2C_SENSORS__ACTIONS__GAUGE_READING:
            {
                if (NULL == readings) {
                    return I2C_SENSORS__STATUS__ERROR__NULL;
                }

                switch (internals.grsState) {
                    case GRS__UNKNOWN:
                        *action = I2C_SENSORS__ACTIONS__INACTIVE;
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
            break;

        case I2C_SENSORS__ACTIONS__GAUGE_INIT:
            {
                switch (internals.fgiState) {
                    case FGI__UNKNOWN:
                        return I2C_SENSORS__STATUS__ERROR__INTERNAL;

                    case FGI__ACCUMULATED_CHARGE_MSB:
                    case FGI__ACCUMULATED_CHARGE_LSB:
                    case FGI__CONTROL:
                        return I2C_SENSORS__STATUS__INCOMPLETE;

                    case FGI__DONE:
                        return I2C_SENSORS__STATUS__SUCCESS_DONE;

                    case FGI__FAILED_NACK:
                        return I2C_SENSORS__STATUS__ERROR__DONE_WITH_NACKS;

                    default:
                        return I2C_SENSORS__STATUS__ERROR__INTERNAL;
                }
            }
            break;

        case I2C_SENSORS__ACTIONS__WRITE_GAUGE_LOW_POWER:
            {
                switch (internals.wlpState) {
                    case WLP__UNKNOWN:
                        return I2C_SENSORS__STATUS__ERROR__INTERNAL;

                    case WLP__WRITING:
                        return I2C_SENSORS__STATUS__INCOMPLETE;

                    case WLP__DONE:
                        return I2C_SENSORS__STATUS__SUCCESS_DONE;

                    case WLP__FAILED_NACK:
                        return I2C_SENSORS__STATUS__ERROR__DONE_WITH_NACKS;

                    default:
                        return I2C_SENSORS__STATUS__ERROR__INTERNAL;
                }
            }
            break;

        case I2C_SENSORS__ACTIONS__READ_GAUGE_CONTROL_REGISTER:
            {
                if (NULL == controlRegisterValue) {
                    return I2C_SENSORS__STATUS__ERROR__NULL;
                }

                switch (internals.wlpState) {
                    case RC__UNKNOWN:
                        return I2C_SENSORS__STATUS__ERROR__INTERNAL;

                    case RC__READING:
                        return I2C_SENSORS__STATUS__INCOMPLETE;

                    case RC__DONE:
                        *controlRegisterValue = internals.controlRegisterReadValue;
                        return I2C_SENSORS__STATUS__SUCCESS_DONE;

                    case RC__FAILED_NACK:
                        return I2C_SENSORS__STATUS__ERROR__DONE_WITH_NACKS;

                    default:
                        return I2C_SENSORS__STATUS__ERROR__INTERNAL;
                }
            }
            break;

        default:
            return I2C_SENSORS__STATUS__ERROR__INTERNAL;
    }
}

void I2C_Sensors__spinOnce(void)
{
    BOOL keepSpinning = TRUE;

    if (I2C_SENSORS__ACTIONS__INACTIVE == internals.activeAction) {
        return;
    }

    while (keepSpinning) {
        I2C__spinOnce();

        switch (internals.activeAction) {
            case I2C_SENSORS__ACTIONS__GAUGE_READING:
                {
                    switch (internals.grsState) {
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
                            keepSpinning = FALSE;
                            break;
                    }
                }
                break;

            case I2C_SENSORS__ACTIONS__GAUGE_INIT:
                {
                    switch (internals.fgiState) {
                        case FGI__UNKNOWN:
                            keepSpinning = FALSE;
                            break;

                        case FGI__ACCUMULATED_CHARGE_MSB:
                            keepSpinning = I2C_Sensors__accumulatedChargeMsb();
                            break;

                        case FGI__ACCUMULATED_CHARGE_LSB:
                            keepSpinning = I2C_Sensors__accumulatedChargeLsb();
                            break;

                        case FGI__CONTROL:
                            keepSpinning = I2C_Sensors__writeControl();
                            break;

                        case FGI__DONE:
                        case FGI__FAILED_NACK:
                        default:
                            keepSpinning = FALSE;
                            break;
                    }
                }
                break;

            case I2C_SENSORS__ACTIONS__WRITE_GAUGE_LOW_POWER:
                {
                    switch (internals.wlpState) {
                        case WLP__UNKNOWN:
                            keepSpinning = FALSE;
                            break;

                        case WLP__WRITING:
                            keepSpinning = I2C_Sensors__lowPower();
                            break;

                        case WLP__DONE:
                        case WLP__FAILED_NACK:
                        default:
                            keepSpinning = FALSE;
                            break;
                    }
                }
                break;

            case I2C_SENSORS__ACTIONS__READ_GAUGE_CONTROL_REGISTER:
                {
                    if (NULL == controlRegisterValue) {
                        return I2C_SENSORS__STATUS__ERROR__NULL;
                    }

                    switch (internals.wlpState) {
                        case RC__UNKNOWN:
                            keepSpinning = FALSE;
                            break;

                        case RC__READING:
                            keepSpinning = I2C_Sensors__readControl();
                            break;

                        case RC__DONE:
                        case RC__FAILED_NACK:
                        default:
                            keepSpinning = FALSE;
                            break;
                    }
                }
                break;

            default:
                keepSpinning = FALSE;
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
                                            BOOL isGaugeReading,
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
                if (isGaugeReading) {
                    internals.grsState = nextState;
                }

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

                if (isGaugeReading) {
                    internals.grsState = nextState;
                }

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
            if (isGaugeReading) {
                internals.grsState = nextState;
            }

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
                                             BOOL isGaugeInit,
                                             FuelGaugeInitState nextState,
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
                if (isGaugeInit) {
                    internals.fgiState = nextState;
                }
                *done = TRUE;
                *success = TRUE;
            }

            prevWriteDone = TRUE;
        } else if (I2C__TRANSACTION__DONE_ERROR_NACK == tStatus.state) {
            // We're now done with this write (and it failed)
            if (statusForRequestedTransaction) {
                if (isGaugeInit) {
                    internals.fgiState = FGI__FAILED_NACK;
                }

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

static BOOL I2C_Sensors__chargeLsb(void)
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

static BOOL I2C_Sensors__chargeMsb(void)
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

static BOOL I2C_Sensors__voltageLsb(void)
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

static BOOL I2C_Sensors__voltageMsb(void)
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

static BOOL I2C_Sensors__currentLsb(void)
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

static BOOL I2C_Sensors__currentMsb(void)
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

static BOOL I2C_Sensors__gaugeTempLsb(void)
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

static BOOL I2C_Sensors__gaugeTempMsb(void)
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

static BOOL I2C_Sensors__accumulatedChargeMsb(void)
{
    BOOL done = FALSE, success = FALSE;
    I2C_Sensors__writeRegNonBlocking(I2C_SLAVE_ADDR,
                                     REG_ADDR__ACCUMULATED_CHARGE_MSB,
                                     FUEL_GAUGE_CHARGE_ACCUM_MSB_INIT,
                                     TRUE,
                                     FGI__ACCUMULATED_CHARGE_LSB,
                                     &done,
                                     &stageSuccess);
    return (done && success) ? TRUE : FALSE;
}


static BOOL I2C_Sensors__accumulatedChargeLsb(void)
{
    BOOL done = FALSE, success = FALSE;
    I2C_Sensors__writeRegNonBlocking(I2C_SLAVE_ADDR,
                                     REG_ADDR__ACCUMULATED_CHARGE_LSB,
                                     FUEL_GAUGE_CHARGE_ACCUM_LSB_INIT,
                                     TRUE,
                                     FGI__CONTROL,
                                     &done,
                                     &stageSuccess);
    return (done && success) ? TRUE : FALSE;
}

static BOOL I2C_Sensors__writeControl(void)
{
    BOOL done = FALSE, success = FALSE;
    I2C_Sensors__writeRegNonBlocking(I2C_SLAVE_ADDR,
                                     REG_ADDR__CONTROL,
                                     FUEL_GAUGE_CONTROL_INIT,
                                     TRUE,
                                     FGI__DONE,
                                     &done,
                                     &stageSuccess);

    // Either we're not done, succeeded and are done with the gauge initialization, or failed due to a NACK. In
    // all of these cases, we want to stop spinning.
    return FALSE;
}

static BOOL I2C_Sensors__readControl(void)
{
    BOOL done = FALSE, gotOutput = FALSE;
    I2C_Sensors__readRegNonBlocking(I2C_SLAVE_ADDR,
                                    REG_ADDR__CONTROL,
                                    0,
                                    FALSE,
                                    initialState,
                                    &(internals.controlRegisterReadValue),
                                    &done,
                                    &success);

    if (done) {
        internals.rcState = success ? RC__DONE : RC__FAILED_NACK;
    }

    // Either we're not done, succeeded and are done with this action, or failed due to a NACK. In all cases, we want
    // to stop spinning
    return FALSE;
}

static BOOL I2C_Sensors__lowPower(void)
{
    BOOL done = FALSE, success = FALSE;
    I2C_Sensors__writeRegNonBlocking(I2C_SLAVE_ADDR,
                                     REG_ADDR__CONTROL,
                                     FUEL_GAUGE_CONTROL_LOW_POWER,
                                     FALSE,
                                     FGI__UNKNOWN,
                                     &done,
                                     &success);

    if (done) {
        internals.wlpState = success ? WLP__DONE : WLP__FAILED_NACK;
    }

    // Either we're not done, succeeded and are done with this action, or failed due to a NACK. In all cases, we want
    // to stop spinning
    return (done && success) ? TRUE : FALSE;
}
