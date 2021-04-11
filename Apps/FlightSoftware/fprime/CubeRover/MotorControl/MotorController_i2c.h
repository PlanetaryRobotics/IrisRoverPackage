#ifndef I2C_H_
#define I2C_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "i2c.h"

typedef uint8_t I2cSlaveAddress_t;

void i2cMasterTransmit(i2cBASE_t *i2c,
                       I2cSlaveAddress_t sadd,
                       uint32_t length,
                       uint8_t *data);

void i2cMasterReceive(i2cBASE_t *i2c,
                      I2cSlaveAddress_t sadd,
                      uint32_t length,
                      uint8_t *data);

void delayForI2C();

#ifdef __cplusplus
}
#endif

#endif  // I2C_H_
