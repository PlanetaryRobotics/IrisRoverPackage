#include <msp430.h>
#include "include/i2c.h"
/*
 * File for interfacing with I2C protocol hardware module
 *
 * I2C communication uses eUSCI_B0 (power circuitry)
 */

/* uart0 rx handler (will only be called after SLIP packet fully received) */
void (*i2c_rx_handler)(uint16_t len, struct buffer *buf) = (void *)(0);

/* init function */
void i2c_init() {
    
}

void i2c_tx_blocking(uint16_t len, struct buffer *buf) {
    
}
