/*
 * Main.cpp
 *
 *  Created on: Sep 30, 2019
 *      Author: cedric
 */

#include <CubeRoverConfig.hpp>
#include "FreeRTOS.h"
#include "os_task.h"
#include "CubeRover/Top/Topology.hpp"
#include "CubeRover/CubeRoverNetworkManager/CubeRoverNetworkManager.hpp"

#include "gio.h"
#include "i2c.h"

extern "C" {
    void vApplicationIdleHook( void );
}

#define SLAVE_BASE_ADDRESS  0x48

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

typedef enum CommandList{
  TGT_SPEED_MOTOR_LEFT =      0x00,
  TGT_SPEED_MOTOR_RIGHT =     0x01,
  TGT_POSITION_MOTOR_LEFT =   0x02,
  TGT_POSITION_MOTOR_RIGHT =  0x03,
  RUN =                   0x04,
  STOP =                  0x05,
  GET_CURRENT =           0x06,
  GET_POSITION =          0x07,
  GET_STATUS =            0x08
}CommandList;

typedef enum Motor{
    MOTOR_FRONT_LEFT  =   0x00,
    MOTOR_FRONT_RIGHT =   0x03,
    MOTOR_REAR_LEFT =     0x01,
    MOTOR_REAR_RIGHT =    0x02
}Motor;

void vApplicationIdleHook( void ){
    run1cycle();
    //gioToggleBit(gioPORTB, 1);
    //for(uint32_t i=0; i<2000000; i++) asm("  NOP");
}

using namespace Wf121;

uint8_t g_rxBuffer[RX_RING_BUFFER_SIZE];
uint8_t g_txBuffer[32];

void i2cReadRegisterSlave(uint8_t slaveAddr, I2cRegisterIds reg, uint8_t *data, uint8_t dataSize){

    uint32_t delay;

    i2cSetSlaveAdd(i2cREG1, slaveAddr);

    i2cSetDirection(i2cREG1, I2C_TRANSMITTER);

    i2cSetCount(i2cREG1, 1);

    /* Set mode as Master */
    i2cSetMode(i2cREG1, I2C_MASTER);

    /* Set Stop after programmed Count */
    i2cSetStop(i2cREG1);

    /* Transmit Start Condition */
    i2cSetStart(i2cREG1);

    i2cSendByte(i2cREG1, reg); // send the address register

    /* Wait until Bus Busy is cleared */
    while(i2cIsBusBusy(i2cREG1) == true);

    /* Wait until Stop is detected */
    while(i2cIsStopDetected(i2cREG1) == 0);

    /* Clear the Stop condition */
    i2cClearSCD(i2cREG1);

    // some delay to let the motor control process command
    for(delay=0; delay<100000; delay++);

    while(i2cIsMasterReady(i2cREG1) != true);
    i2cSetSlaveAdd(i2cREG1, slaveAddr);
    i2cSetDirection(i2cREG1, I2C_RECEIVER);
    i2cSetCount(i2cREG1, dataSize);
    i2cSetMode(i2cREG1, I2C_MASTER);
    i2cSetStop(i2cREG1);
    i2cSetStart(i2cREG1);
    i2cReceive(i2cREG1, dataSize, data);
    while(i2cIsBusBusy(i2cREG1) == true);
    while(i2cIsStopDetected(i2cREG1) == 0);
    i2cClearSCD(i2cREG1);
    while(i2cIsMasterReady(i2cREG1) != true);
}

void i2cWriteRegisterSlave(uint8_t slaveAddr, I2cRegisterIds reg, uint8_t *data, uint8_t dataSize){

    /* Configure address of Slave to talk to */
    i2cSetSlaveAdd(i2cREG1, slaveAddr);

    /* Set direction to Transmitter */
    /* Note: Optional - It is done in Init */
    i2cSetDirection(i2cREG1, I2C_TRANSMITTER);

    /* Configure Data count */
    /* Data Count + 1 ( Word Address) */
    i2cSetCount(i2cREG1, dataSize + 1);

    /* Set mode as Master */
    i2cSetMode(i2cREG1, I2C_MASTER);

    /* Set Stop after programmed Count */
    i2cSetStop(i2cREG1);

    /* Transmit Start Condition */
    i2cSetStart(i2cREG1);

    /* Send the Word Address */
    i2cSendByte(i2cREG1, reg);

    /* Tranmit DATA_COUNT number of data in Polling mode */
    i2cSend(i2cREG1, dataSize, data);

    /* Wait until Bus Busy is cleared */
    while(i2cIsBusBusy(i2cREG1) == true);

    /* Wait until Stop is detected */
    while(i2cIsStopDetected(i2cREG1) == 0);

    /* Clear the Stop condition */
    i2cClearSCD(i2cREG1);

    while(i2cIsMasterReady(i2cREG1) != true);
}

void handleI2cMotorControlCommand(Motor motorId, I2cRegisterIds reg, uint8_t *data, uint8_t dataSize){

    switch(reg){
        case I2C_ADDRESS:
            i2cReadRegisterSlave(SLAVE_BASE_ADDRESS+motorId, reg, data, dataSize);
            break;
        case RELATIVE_TARGET_POSITION:
            i2cWriteRegisterSlave(SLAVE_BASE_ADDRESS+motorId, reg, data, dataSize);
            break;
        case TARGET_SPEED:
            i2cWriteRegisterSlave(SLAVE_BASE_ADDRESS+motorId, reg, data, dataSize);
            break;
        case CURRENT_POSITION:
            i2cReadRegisterSlave(SLAVE_BASE_ADDRESS+motorId, reg, data, dataSize);
            break;
        case CURRENT_SPEED:
            i2cReadRegisterSlave(SLAVE_BASE_ADDRESS+motorId, reg, data, dataSize);
            break;
        case MOTOR_CURRENT:
            i2cReadRegisterSlave(SLAVE_BASE_ADDRESS+motorId, reg, data, dataSize);
            break;
        case P_CURRENT:
            i2cWriteRegisterSlave(SLAVE_BASE_ADDRESS+motorId, reg, data, dataSize);
            break;
        case I_CURRENT:
            i2cWriteRegisterSlave(SLAVE_BASE_ADDRESS+motorId, reg, data, dataSize);
            break;
        case P_SPEED:
            i2cWriteRegisterSlave(SLAVE_BASE_ADDRESS+motorId, reg, data, dataSize);
            break;
        case I_SPEED:
            i2cWriteRegisterSlave(SLAVE_BASE_ADDRESS+motorId, reg, data, dataSize);
            break;
        case ACC_RATE:
            i2cWriteRegisterSlave(SLAVE_BASE_ADDRESS+motorId, reg, data, dataSize);
            break;
        case DEC_RATE:
            i2cWriteRegisterSlave(SLAVE_BASE_ADDRESS+motorId, reg, data, dataSize);
            break;
        case CONTROL_REGISTER:
            i2cWriteRegisterSlave(SLAVE_BASE_ADDRESS+motorId, reg, data, dataSize);
            break;
        case STATUS_REGISTER:
            i2cReadRegisterSlave(SLAVE_BASE_ADDRESS+motorId, reg, data, dataSize);
            break;
        case FAULT_REGISTER:
            i2cReadRegisterSlave(SLAVE_BASE_ADDRESS+motorId, reg, data, dataSize);
            break;
    }
}

int32_t g_targetPositionLeft;
int32_t g_targetPositionRight;

void main(void)
{
    CubeRoverNetworkManager wf121;
    uint16_t byteRead = 0;

    /* USER CODE BEGIN (3) */
    gioInit();
    i2cInit();
    sciInit();

    constructApp();

    g_targetPositionLeft = 0;
    g_targetPositionRight = 0;

    while(1){

        wf121.UpdateNetworkManager();

        // Get the header first
        wf121.ReceiveUdpData(g_rxBuffer, 1, &byteRead, UdpReadMode::WAIT_UNTIL_READY | UdpReadMode::NORMAL_READ, 1000);

        if(byteRead > 0){
            uint8_t sizeOfReply = 0;
            switch(g_rxBuffer[0]){
                case TGT_SPEED_MOTOR_LEFT:
                    wf121.ReceiveUdpData(g_rxBuffer, 2, &byteRead, UdpReadMode::WAIT_UNTIL_READY | UdpReadMode::NORMAL_READ, 10);
                    handleI2cMotorControlCommand(MOTOR_FRONT_LEFT, TARGET_SPEED, g_rxBuffer, 4);
                    handleI2cMotorControlCommand(MOTOR_REAR_LEFT, TARGET_SPEED, g_rxBuffer, 4);
                    break;
                case TGT_SPEED_MOTOR_RIGHT:
                    wf121.ReceiveUdpData(g_rxBuffer, 2, &byteRead, UdpReadMode::WAIT_UNTIL_READY | UdpReadMode::NORMAL_READ, 10);
                    handleI2cMotorControlCommand(MOTOR_FRONT_RIGHT, TARGET_SPEED, g_rxBuffer, 4);
                    handleI2cMotorControlCommand(MOTOR_REAR_RIGHT, TARGET_SPEED, g_rxBuffer, 4);
                    break;
                case TGT_POSITION_MOTOR_LEFT:
                    wf121.ReceiveUdpData(g_rxBuffer, 4, &byteRead, UdpReadMode::WAIT_UNTIL_READY | UdpReadMode::NORMAL_READ, 10);
                    memcpy(&g_targetPositionLeft, g_rxBuffer+1, 4);
                    break;
                case TGT_POSITION_MOTOR_RIGHT:
                    wf121.ReceiveUdpData(g_rxBuffer, 4, &byteRead, UdpReadMode::WAIT_UNTIL_READY | UdpReadMode::NORMAL_READ, 10);
                    memcpy(&g_targetPositionRight, g_rxBuffer+1, 4);
                    break;
                case RUN:
                case STOP:
                    wf121.ReceiveUdpData(g_rxBuffer, 1, &byteRead, UdpReadMode::WAIT_UNTIL_READY | UdpReadMode::NORMAL_READ, 10);
                    handleI2cMotorControlCommand(MOTOR_FRONT_LEFT, CONTROL_REGISTER, g_rxBuffer, 1);
                    handleI2cMotorControlCommand(MOTOR_FRONT_RIGHT, CONTROL_REGISTER, g_rxBuffer, 1);
                    handleI2cMotorControlCommand(MOTOR_REAR_RIGHT, CONTROL_REGISTER, g_rxBuffer, 1);
                    handleI2cMotorControlCommand(MOTOR_REAR_LEFT, CONTROL_REGISTER, g_rxBuffer, 1);
                    break;
                case GET_CURRENT:
                    handleI2cMotorControlCommand(MOTOR_FRONT_LEFT, MOTOR_CURRENT, g_txBuffer+1, 4);
                    handleI2cMotorControlCommand(MOTOR_REAR_LEFT, MOTOR_CURRENT, g_txBuffer+5, 4);
                    handleI2cMotorControlCommand(MOTOR_FRONT_RIGHT, MOTOR_CURRENT, g_txBuffer+9, 4);
                    handleI2cMotorControlCommand(MOTOR_REAR_RIGHT, MOTOR_CURRENT, g_txBuffer+13, 4);
                    sizeOfReply = 17;
                    break;
                case GET_POSITION:
                    handleI2cMotorControlCommand(MOTOR_FRONT_LEFT, CURRENT_POSITION, g_txBuffer+1, 4);
                    handleI2cMotorControlCommand(MOTOR_REAR_LEFT, CURRENT_POSITION, g_txBuffer+5, 4);
                    handleI2cMotorControlCommand(MOTOR_FRONT_RIGHT, CURRENT_POSITION, g_txBuffer+9, 4);
                    handleI2cMotorControlCommand(MOTOR_REAR_RIGHT, CURRENT_POSITION, g_txBuffer+13, 4);
                    sizeOfReply = 17;
                    break;
                case GET_STATUS:
                    handleI2cMotorControlCommand(MOTOR_FRONT_LEFT, STATUS_REGISTER, g_txBuffer+1, 1);
                    //handleI2cMotorControlCommand(MOTOR_FRONT_RIGHT, STATUS_REGISTER, g_txBuffer+2, 1);
                    //handleI2cMotorControlCommand(MOTOR_REAR_RIGHT, STATUS_REGISTER, g_txBuffer+3, 1);
                    //handleI2cMotorControlCommand(MOTOR_REAR_LEFT, STATUS_REGISTER, g_txBuffer+4, 1);
                    sizeOfReply = 1;
                    break;
            }

            g_txBuffer[0] = g_rxBuffer[0];
            wf121.SendUdpData(g_txBuffer, sizeOfReply, 10000);
        }
    }

    vTaskStartScheduler();

    //if it reaches that point, there is a problem with RTOS.

/* USER CODE END */
}
