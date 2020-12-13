#ifndef I2C_H_
#define I2C_H_

#include "driverlib.h"
#include "main.h"

#define I2C_RX_BUFFER_MAX_SIZE      8
#define I2C_TX_BUFFER_MAX_SIZE      8

#define I2C_MAX_DATA_SIZE           4

#define I2C_SLAVE_ADDRESS               0x48
#define I2C_PACKET_HEADER               0xAA

typedef enum I2cRegisterIds{
    I2C_ADDRESS = 0,
    RELATIVE_TARGET_POSITION = 1,
    TARGET_SPEED = 2,
    CURRENT_POSITION = 3,
    CURRENT_SPEED = 4,
    MOTOR_CURRENT = 5,
    P_CURRENT = 6,
    I_CURRENT = 7,
    P_SPEED = 8,
    I_SPEED = 9,
    ACC_RATE = 10,
    DEC_RATE = 11,
    CONTROL_REGISTER = 12,
    STATUS_REGISTER = 13,
    FAULT_REGISTER = 14,
    MAX_NB_CMDS = 15
}I2cRegisterIds;

typedef enum I2cMode{
    IDLE_MODE,
    NACK_MODE,
    TX_REG_ADDRESS_MODE,
    RX_REG_ADDRESS_MODE,
    TX_DATA_MODE,
    RX_DATA_MODE,
    SWITCH_TO_RX_MODE,
    SWITCH_TO_TX_MODE,
    TIMEOUT_MODE
}I2cMode;


typedef struct I2cPacket{
    uint8_t header;
    uint8_t dataSize;
    uint8_t regId;
    uint8_t data[I2C_MAX_DATA_SIZE];

}I2cPacket;

bool readI2cData(I2cPacket *pckt);
void initializeI2cModule();

#endif /* I2C_H_ */
