#ifndef I2C_H_
#define I2C_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <stdbool.h>

#include "i2c.h"

    typedef uint8_t I2cSlaveAddress_t;

    // Max allowable delay for any I2C Master Read or Write Operation in Cycles
    static const uint32_t I2C_MASTER_READ_WRITE_MAX_DELAY_CYCLES = 16000000; // 16M cycles ~= 1s (probably greater because we do a few ops during each delay loop)

    i2cBASE_t *m_i2c;

    // bool i2cMasterTransmit(i2cBASE_t *i2c, I2cSlaveAddress_t sadd, uint32_t length, uint8_t *data);
    bool i2cMasterTransmit(i2cBASE_t *i2c, I2cSlaveAddress_t sadd, uint8_t slaveRegToWriteAddr, uint16_t length, uint8_t *data);
    bool i2cMasterReadData(i2cBASE_t *i2c, I2cSlaveAddress_t sadd, uint8_t slaveRegToReadAddr, uint16_t length, uint8_t *data);

    void delayForI2C();

#ifdef __cplusplus
}
#endif

#endif // I2C_H_
