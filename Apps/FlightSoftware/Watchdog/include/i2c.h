/**
 * @file i2c.h
 * @brief File for interfacing with I2C protocol hardware module
 * File for interfacing with I2C protocol hardware module.
 */
#ifndef __I2C_INC
#define __I2C_INC

#include "buffer.h"

extern void (*i2c_rx_handler)(uint16_t len, struct buffer *buf);

/**
 * @brief Initialize I2C hardware.
 * Sets up the interrupts and whatnot for I2C.
 */
void i2c_init();

void i2c_tx_blocking(uint16_t len, struct buffer *buf);

#endif /* __I2C_INC */
