// i2c.h

#ifndef __I2C_H__
#define __I2C_H__

#include <msp430.h>
#include <stdint.h>

typedef enum I2C__TransactionType {
    I2C__TYPE__READ,
    I2C__TYPE__WRITE
} I2C__TransactionType;

typedef enum I2C__TransactionState {
    I2C__TRANSACTION__UNKNOWN = 0, /* Only used prior to transaction being initialized */
    I2C__TRANSACTION__WAIT_FOR_STOP,
    I2C__TRANSACTION__TX_START,
    I2C__TRANSACTION__CONFIRM_START,
    I2C__TRANSACTION__TX_REG_ADDRESS,
    I2C__TRANSACTION__TX_DATA,
    I2C__TRANSACTION__RX_START,
    I2C__TRANSACTION__RX_DATA_AND_STOP,
    I2C__TRANSACTION__DONE_SUCCESS,
    I2C__TRANSACTION__DONE_ERROR_NACK
} I2C__TransactionState;

typedef struct I2C__TransactionStatus {
    uint8_t devAddr;
    uint8_t regAddr;
    I2C__TransactionType type;
    I2C__TransactionState state;
    uint8_t data;
} I2C__TransactionStatus;

typedef enum I2C__Status {
    I2C__STATUS__SUCCESS = 0, /* Operation suceeded. */
    I2C__STATUS__ERROR__NULL = -1, /* A required argument or a member of an argument was NULL */
    I2C__STATUS__ERROR__ALREADY_ACTIVE_TRANSACTION = -2, /* Couldn't create a new transaction b/c one is active */
    I2C__STATUS__ERROR__NO_TRANSACTION = -3 /* Couldn't get status because there has been no transaction */
} I2C__Status;

/* Note: this implementation only handles one byte reads and writes */

void I2C__init();

I2C__Status I2C__write(uint8_t dev_addr, uint8_t reg_addr, uint8_t data);

I2C__Status I2C__read(uint8_t dev_addr, uint8_t reg_addr);

I2C__Status I2C__getTransactionStatus(I2C__TransactionStatus* tStatus);

void I2C__spinOnce();

#endif // #ifndef __I2C_H__
