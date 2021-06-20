// i2c.h

#ifndef __I2C_H__
#define __I2C_H__

#include <msp430.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @defgroup watchdogI2c I2C Driver
 * @addtogroup watchdogI2c
 * @{
 */

/**
 * @brief Possible I2C transaction types.
 */
typedef enum I2C__TransactionType {
    I2C__TYPE__READ, //!< Reading
    I2C__TYPE__WRITE //!< Writing
} I2C__TransactionType;

/**
 * @brief Possible states of an I2C transaction.
 */
typedef enum I2C__TransactionState {
    I2C__TRANSACTION__UNKNOWN = 0, //!< Only used prior to transaction being initialized
    I2C__TRANSACTION__WAIT_FOR_STOP, //!< Looking for STOP condition of previous transaction to have cleared.
    I2C__TRANSACTION__TX_START, //!< Looking for being ready to transmit the device address.
    I2C__TRANSACTION__CONFIRM_START, //!< Looking for the START condition to have cleared.
    I2C__TRANSACTION__TX_REG_ADDRESS, //!< Looking for the register address send to have completed.
    I2C__TRANSACTION__TX_DATA, //!< Looking for the data byte send to have completed.
    I2C__TRANSACTION__RX_START, //!< Looking for the repeated START condition to be cleared.
    I2C__TRANSACTION__RX_DATA_AND_STOP, //!< Looking for the data to have been received.
    I2C__TRANSACTION__DONE_SUCCESS, //!< Done, and transaction was a success.
    I2C__TRANSACTION__DONE_ERROR_NACK //!< Done, and transaction failed due to a NACK.
} I2C__TransactionState;

/**
 * @brief The status of an I2C transaction.
 */
typedef struct I2C__TransactionStatus {
    uint8_t devAddr; //!< The device address that this transaction is reading from or writing to.
    uint8_t regAddr; //!< The register address that this transaction is reading from or writing to.
    I2C__TransactionType type; //!< The type of this transaction.
    I2C__TransactionState state; //!< The state of this transaction.
    uint8_t data; //!< The data to write or the data that was read.
} I2C__TransactionStatus;

/**
 * @brief Possible return statuses of I2C functions.
 */
typedef enum I2C__Status {
    I2C__STATUS__SUCCESS = 0, /** Operation succeeded. */
    I2C__STATUS__ERROR__NULL = -1, /** A required argument or a member of an argument was NULL */
    I2C__STATUS__ERROR__ALREADY_ACTIVE_TRANSACTION = -2, /** Couldn't create a new transaction b/c one is active */
    I2C__STATUS__ERROR__NO_TRANSACTION = -3 /** Couldn't get status because there has been no transaction */
} I2C__Status;

/* Note: this implementation only handles one byte reads and writes */

/**
 * @brief Initializes the I2C module.
 */
void I2C__init();

/**
 * @brief Initiates a write transaction. Will not block.
 *
 * `I2C__spinOnce()` needs to be called periodically in order for this transaction to progress. Call
 * `I2C__getTransactionStatus()` to determine when this transaction has completed.
 *
 * @param dev_addr The device address to which to write.
 * @param reg_addr The register address to which to write.
 * @param data The byte of data to write to the specified device and register.
 *
 * @return One of the following:
 *   - I2C__STATUS__SUCCESS: The function was successful.
 *   - I2C__STATUS__ERROR__ALREADY_ACTIVE_TRANSACTION: Cannot start a new transaction because one is already active.
 */
I2C__Status I2C__write(uint8_t dev_addr, uint8_t reg_addr, uint8_t data);

/**
 * @brief Initiates a read transaction. Will not block.
 *
 * `I2C__spinOnce()` needs to be called periodically in order for this transaction to progress. Call
 * `I2C__getTransactionStatus()` to determine when this transaction has completed.
 *
 * @param dev_addr The device address to read from.
 * @param reg_addr The register address to read from.
 *
 * @return One of the following:
 *   - I2C__STATUS__SUCCESS: The function was successful.
 *   - I2C__STATUS__ERROR__ALREADY_ACTIVE_TRANSACTION: Cannot start a new transaction because one is already active.
 */
I2C__Status I2C__read(uint8_t dev_addr, uint8_t reg_addr);

/**
 * @brief Gets the status of the currently active transaction, if there is one. Will not block.
 *
 * @param tStatus A return parameter that will be set to the status of the current transaction if the return status
 *                is `I2C__STATUS__SUCCESS`.
 *
 * @return One of the following:
 *   - I2C__STATUS__SUCCESS: The function was successful.
 *   - I2C__STATUS__ERROR__NULL: `tStatus` was NULL.
 *   - I2C__STATUS__ERROR__NO_TRANSACTION: No transaction has ever been performed.
 */
I2C__Status I2C__getTransactionStatus(I2C__TransactionStatus* tStatus);

/**
 * @brief Spins the I2C module. If there is no active transaction this will return immediately. Will not block.
 *
 * This must be called periodically to advance any active transactions.
 */
void I2C__spinOnce();

/**
 * @}
 */

#ifdef __cplusplus
} /* close extern "C" */
#endif

#endif // #ifndef __I2C_H__
