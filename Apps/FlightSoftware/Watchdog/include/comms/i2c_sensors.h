#ifndef __WATCHDOG_I2C_SENSORS_H__
#define __WATCHDOG_I2C_SENSORS_H__

#include <stdint.h>

/**
 * @defgroup watchdogI2cSensors I2C Sensors
 * @addtogroup watchdogI2cSensors
 * @{
 */

/**
 * @brief The i2c address of LTC2944IDD#PBF fuel gauge.
 *
 * Equivalent to 0b1100100.
 */
#define I2C_SLAVE_ADDR 100

/**
 * @brief Bit flags that, if set, indicate that data was not updated because the slave device did not acknowledge a
 *        transmitted byte.
 */
typedef enum I2C_Sensors__NACK_Status {
    I2C_SENSORS__NACK__BATT_CHARGE = 0x01,   //!< Battery charge NACK bit flag.
    I2C_SENSORS__NACK__BATT_VOLTAGE = 0x02,  //!< Battery voltage NACK bit flag.
    I2C_SENSORS__NACK__BATT_CURRENT = 0x04,  //!< Battery current NACK bit flag.
    I2C_SENSORS__NACK__FUEL_GAUGE_TEMP = 0x08//!< Fuel gauge temperature NACK bit flag.
} I2C_Sensors__NACK_Status;

/**
 * @brief The sensor readings obtained in the standard sensor reading loop.
 *
 * @todo Figure out exactly what the "processing" is doing for the `*_telem` fields.
 */
typedef struct I2C_Sensors__Readings {
    uint8_t raw_battery_charge[2]; //!< The raw battery charge reading.
    uint8_t raw_battery_voltage[2]; //!< The raw battery voltage reading.
    uint8_t raw_battery_current[2]; //!< The raw battery current reading.
    uint8_t raw_fuel_gauge_temp[2]; //!< The raw fuel gauge temperature reading.
    uint8_t batt_charge_telem; //!< The battery charge, processed for telemetry.
    uint8_t batt_curr_telem; //!< The battery current, processed for telemetry.

    /**
     * @brief A bit mask where if a bit is set, it indicates that some data was not updated because the slave device did
     *        not acknowledge a transmitted byte.
     *
     * The meaning of the bits in this value is represented by the `I2C_Sensors__NACK_Status` values.
     */
    uint8_t nackMask;
} I2C_Sensors__Readings;

/**
 * @brief Possible return statuses of I2C_Sensors functions.
 */
typedef enum I2C_Sensors__Status {
    I2C_SENSORS__STATUS__INCOMPLETE = 1, //!< Operation still waiting to complete.
    I2C_SENSORS__STATUS__SUCCESS_DONE = 0, //!< Operation succeeded.
    I2C_SENSORS__STATUS__ERROR__NULL = -1, //!< A required argument or a member of an argument was NULL.

    /**
     * @brief Couldn't get one or more readings because I2C slave didn't acknowledge a byte.
     */
    I2C_SENSORS__STATUS__ERROR__DONE_WITH_NACKS = -2,

    I2C_SENSORS__STATUS__ERROR__READINGS_NOT_STARTED = -3, //!< No gauge reading is active.
    I2C_SENSORS__STATUS__ERROR__READINGS_IN_PROGRESS = -4, //!< Gauge readings are already in progress.
    I2C_SENSORS__STATUS__ERROR__INTERNAL = -255 //!< An unexpected error occurred.
} I2C_Sensors__Status;

/**
 * @brief Initializes the I2C_Sensors module, which simply invokes `I2C__init()` to initialize the underlying I2C
 *        driver.
 */
void I2C_Sensors__init();

/**
 * @brief Initiates the next set of gauge readings. Does not block.
 *
 * This only sets the state of the internal state machine to the appropriate state to begin reading all gauges. Actually
 * initiating the I2C actions as well as advancing the state machine all occur within `I2C_Sensors__spinOnce()`. The
 * intended way to use this is that this function is called once, then `I2C_Sensors__spinOnce()` is called periodically
 * (relatively frequently), and `I2C_Sensors__getGaugeReadingStatus()` is called after each call to
 * `I2C_Sensors__spinOnce()`. The return value of `I2C_Sensors__getGaugeReadingStatus()` informs the caller of the
 * status of the gauge readings. Only after the return value indicates they are complete should another set of readings
 * be started by calling this function once more.
 *
 * Once this function has been called, the only functions in this module that should be called are
 * `I2C_Sensors__spinOnce()` and `I2C_Sensors__getGaugeReadingStatus()` until the status returned by
 * `I2C_Sensors__getGaugeReadingStatus()` is anything other than `I2C_SENSORS__STATUS__INCOMPLETE`. Before the existing
 * gauge reading process is completed, calling this function again will reset the internal state machine, interrupting
 * the process of reading all sensors that was initiated with the previous call to this function. Additionally, calling
 * any of the `*Blocking()` functions will immediately return an error (and not perform the desired action) until
 * the existing gauge reading process is complete.
 */
void I2C_Sensors__initiateGaugeReadings();

/**
 * @brief Checks the status of reading the gauges. Does not block.
 *
 * If the return status is `I2C_SENSORS__STATUS__INCOMPLETE` then the gauge reading process has been started and is
 * ongoing, so `I2C_Sensors__spinOnce()` needs to be called in conjunction with checking the status with this function
 * until a different status is returned by this function.
 *
 * If the return status is `I2C_SENSORS__STATUS__SUCCESS_DONE` then the gauge reading process is complete and all fields
 * in the `readings` structure passed to this function will have been updated with new values.
 *
 * If the return status is `I2C_SENSORS__STATUS__ERROR__DONE_WITH_NACKS` then the gauge reading process is complete.
 * However, this return status indicates that the `readings` structure that was passed to this function had at least one
 * byte for which the slave device didn't acknowledge data from the MSP430. In this case, one should check the
 * `readings->nackMask` field. Each of the lowest four bits in the `nackMask` field describes whether or not a certain
 * reading was successful. If the bit is set, then the corresponding field(s) in the `readings` will NOT have been
 * updated. Alternatively, if the bit is unset then the corresponding field(s) will have been updated. Specifically:
 *   - If `(nackMask & I2C_SENSORS__NACK__BATT_CHARGE) != 0` then `raw_battery_charge` and `batt_charge_telem` will both
 *     NOT be updated, otherwise both are updated.
 *   - If `(nackMask & I2C_SENSORS__NACK__BATT_VOLTAGE) != 0` then `raw_battery_voltage` will NOT be updated, otherwise
 *     it is updated.
 *   - If `(nackMask & I2C_SENSORS__NACK__BATT_CURRENT) != 0` then `raw_battery_current` and `batt_curr_telem` will both
 *     NOT be updated, otherwise both are updated.
 *   - If `(nackMask & I2C_SENSORS__NACK__FUEL_GAUGE_TEMP) != 0` then `raw_fuel_gauge_temp` will NOT be updated,
 *     otherwise it is updated.
 *
 * @note See the documentation of `I2C_Sensors__initiateGaugeReadings()` for an explanation of the expected use case of
 *       this function.
 *
 * @param readings An output parameter that will be filled in with the most recent sensor readings, but only if the
 *                 return status is `I2C_SENSORS__STATUS__SUCCESS_DONE` or
 *                 `I2C_SENSORS__STATUS__ERROR__DONE_WITH_NACKS`.
 *
 * @return One of the following:
 *   - I2C_SENSORS__STATUS__INCOMPLETE: The gauge readings are still ongoing.
 *   - I2C_SENSORS__STATUS__SUCCESS_DONE: The gauge readings have completed.
 *   - I2C_SENSORS__STATUS__ERROR__NULL: `readings` was NULL.
 *   - I2C_SENSORS__STATUS__ERROR__DONE_WITH_NACKS: The gauge readings have completed, but with at least one NACK.
 *   - I2C_SENSORS__STATUS__ERROR__READINGS_NOT_STARTED: `I2C_Sensors__initiateGaugeReadings()` has never been called.
 */
I2C_Sensors__Status
I2C_Sensors__getGaugeReadingStatus(I2C_Sensors__Readings* readings);

/**
 * @brief Set the fuel gauge to low power mode with a blocking register write.
 *
 * @return One of the following:
 *   - I2C_SENSORS__STATUS__SUCCESS_DONE: The write completed successfully.
 *   - I2C_SENSORS__STATUS__ERROR__DONE_WITH_NACKS: The write failed due to a NACK.
 *   - I2C_SENSORS__STATUS__ERROR__READINGS_IN_PROGRESS: Gauge readings are already in progress, preventing us from
 *                                                       performing this action.
 */
I2C_Sensors__Status I2C_Sensors__fuelGaugeLowPowerBlocking();

/**
 * @brief Initializes the fuel gauge with a sequence of three register writes. Blocks until all three writes
 *        are complete.
 *
 * @return One of the following:
 *   - I2C_SENSORS__STATUS__SUCCESS_DONE: The writes have completed successfully.
 *   - I2C_SENSORS__STATUS__ERROR__DONE_WITH_NACKS: At least one of the writes failed due to a NACK.
 *   - I2C_SENSORS__STATUS__ERROR__READINGS_IN_PROGRESS: Gauge readings are already in progress, preventing us from
 *                                                       performing this action.
 */
I2C_Sensors__Status I2C_Sensors__initializeFuelGaugeBlocking();

/**
 * @brief Reads the fuel gauge control register with a blocking read.
 *
 * @param data An output parameter set to the data read from the fuel gauge control register. Only set if the return
 *             status is `I2C_SENSORS__STATUS__SUCCESS_DONE`.
 *
 * @return One of the following:
 *   - I2C_SENSORS__STATUS__SUCCESS_DONE: The read completed successfully.
 *   - I2C_SENSORS__STATUS__ERROR__DONE_WITH_NACKS: The read failed due to a NACK.
 *   - I2C_SENSORS__STATUS__ERROR__READINGS_IN_PROGRESS: Gauge readings are already in progress, preventing us from
 *                                                       performing this action.
 */
I2C_Sensors__Status I2C_Sensors__readFuelGaugeControlRegisterBlocking(uint8_t* data);

/**
 * @brief Spins the gauge reading state machine. If no gauge reading process is active, this will return immediately.
 *
 * @note See the documentation of `I2C_Sensors__initiateGaugeReadings()` for an explanation of the expected use case of
 *       this function.
 */
void I2C_Sensors__spinOnce();

/**
 * @}
 */

#endif // #ifndef __WATCHDOG_I2C_SENSORS_H__
