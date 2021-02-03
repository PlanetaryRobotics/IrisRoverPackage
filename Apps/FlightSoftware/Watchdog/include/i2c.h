/**
 * @file i2c.h
 * @brief File for interfacing with I2C protocol hardware module
 * File for interfacing with I2C protocol hardware module.
 */
#ifndef __I2C_INC
#define __I2C_INC

#include "buffer.h"

/**
 * @brief Initialize I2C hardware.
 * Sets up the interrupts and whatnot for I2C.
 */
void i2c_init();

void i2c_tx_blocking(uint16_t len, unsigned char *buf);

#endif /* __I2C_INC */
