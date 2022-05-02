#ifndef I2C_H_
#define I2C_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

#include "i2c.h"

typedef uint8_t I2cSlaveAddress_t;

//bool i2cMasterTransmit(i2cBASE_t *i2c, I2cSlaveAddress_t sadd, uint32_t length, uint8_t *data);
bool i2cMasterTransmit(i2cBASE_t *i2c, I2cSlaveAddress_t sadd, uint8_t slaveRegToWriteAddr, uint32_t length, uint8_t *data);
bool i2cMasterReadData(i2cBASE_t *i2c, I2cSlaveAddress_t sadd, uint8_t slaveRegToReadAddr, uint32_t length, uint8_t *data);

void delayForI2C();

#ifdef __cplusplus
}
#endif

#endif  // I2C_H_
