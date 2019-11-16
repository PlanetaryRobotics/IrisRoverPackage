/*
 * I2C_message.h
 *
 * Structure of Data Packets Sent over I2C.
 */

#ifndef _CR_MOT_I2C_MESSAGE_H_
#define _CR_MOT_I2C_MESSAGE_H_

#include <stdint.h>
#include "smbus.h"

    const uint8_t MESSAGE_BUFFER_SIZE = 2 + 4; // 2 bytes overhead + 4 bytes (max data type size)

    union {
        struct {
            uint8_t regID; //                           - Register ID
            uint8_t rw:1; //                            - Whether this Message is Requesting a Read (0) or Writing Data (1)
            uint8_t len:7; //                           - Number of Bytes Being Sent
            uint8_t data[MESSAGE_BUFFER_SIZE-2]; //     - Max. Content of Message (data to set in register, will only contain (len) worth of data))
        };
        uint8_t raw[MESSAGE_BUFFER_SIZE]; //  - Raw data as bytes
    } i2c_message;

#endif // _CR_MOT_I2C_MESSAGE_H_
