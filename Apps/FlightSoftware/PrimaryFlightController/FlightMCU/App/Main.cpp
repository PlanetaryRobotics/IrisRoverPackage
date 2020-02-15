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
    MOTOR_FRONT_RIGHT =   0x01,
    MOTOR_REAR_LEFT =     0x02,
    MOTOR_REAR_RIGHT =    0x03
}Motor;

void vApplicationIdleHook( void ){
    run1cycle();
    //gioToggleBit(gioPORTB, 1);
    //for(uint32_t i=0; i<2000000; i++) asm("  NOP");
}

using namespace Wf121;

uint8_t g_rxBuffer[RX_RING_BUFFER_SIZE];
uint8_t g_txBuffer[8];

void handleI2cMotorControlCommand(Motor motorId, I2cRegisterIds reg, uint8_t *data, uint8_t dataSize){
    uint16_t i;

    i2cSetSlaveAdd(i2cREG1, 0x48+motorId);
    i2cSetDirection(i2cREG1, I2C_TRANSMITTER);

    /* Set mode as Master */
    i2cSetMode(i2cREG1, I2C_MASTER);

    /* Set Stop after programmed Count */
    i2cSetStop(i2cREG1);

    /* Transmit Start Condition */
    i2cSetStart(i2cREG1);

    i2cSendByte(i2cREG1, reg); // send the address register

    /* Wait until Bus Busy is cleared */
    while(i2cIsBusBusy(i2cREG1) == true);

    switch(reg){
        case I2C_ADDRESS:
            break;
        case RELATIVE_TARGET_POSITION:
            break;
        case TARGET_SPEED:
            break;
        case CURRENT_POSITION:
            i2cSetDirection(i2cREG1, I2C_RECEIVER);
            i2cReceive(i2cREG1, dataSize, data);
            /* Wait until Bus Busy is cleared */
            while(i2cIsBusBusy(i2cREG1) == true);
            break;
        case CURRENT_SPEED:
            break;
        case MOTOR_CURRENT:
            break;
        case P_CURRENT:
            break;
        case I_CURRENT:
            break;
        case P_SPEED:
            break;
        case I_SPEED:
            break;
        case ACC_RATE:
            break;
        case DEC_RATE:
            break;
        case CONTROL_REGISTER:
            break;
        case STATUS_REGISTER:
            break;
        case FAULT_REGISTER:
            break;
    }

    /* Wait until Stop is detected */
    while(i2cIsStopDetected(i2cREG1) == 0);

    /* Clear the Stop condition */
    i2cClearSCD(i2cREG1);
}

void main(void)
{
    CubeRoverNetworkManager wf121;
    uint16_t commandPacketSize = 8;    // 8 bytes
    uint16_t byteRead = 0;
    uint32_t payloadSize = 0;

    /* USER CODE BEGIN (3) */
    gioInit();
    i2cInit();
    sciInit();

    constructApp();


    while(1){

        //wf121.UpdateNetworkManager();
        //wf121.ReceiveUdpData(g_rxBuffer, commandPacketSize, &byteRead, UdpReadMode::WAIT_UNTIL_READY | UdpReadMode::NORMAL_READ, 10);

        //if(byteRead == commandPacketSize){
        //    handleI2cMotorControlCommand(g_rxBuffer);
        //    g_txBuffer[0] = 0xAA;   // send back acknowledgment
        //    wf121.SendUdpData(g_txBuffer, g_txBuffer, 10000);
        // }
        //cmd = g_rxBuffer[0];
        //g_txBuffer[0] = g_rxBuffer[0];

        switch(cmd){
            case TGT_SPEED_MOTOR_LEFT:
                handleI2cMotorControlCommand(MOTOR_FRONT_LEFT, TARGET_SPEED, g_rxBuffer+1, 1);
                handleI2cMotorControlCommand(MOTOR_REAR_LEFT, TARGET_SPEED, g_rxBuffer+1, 1);
                break;
            case TGT_SPEED_MOTOR_RIGHT:
                handleI2cMotorControlCommand(MOTOR_FRONT_RIGHT, TARGET_SPEED, g_rxBuffer+1, 1);
                handleI2cMotorControlCommand(MOTOR_REAR_RIGHT, TARGET_SPEED, g_rxBuffer+1, 1);
                break;
            case TGT_POSITION_MOTOR_LEFT:
                handleI2cMotorControlCommand(MOTOR_FRONT_LEFT, RELATIVE_TARGET_POSITION, g_rxBuffer+1, 4);
                handleI2cMotorControlCommand(MOTOR_REAR_LEFT, RELATIVE_TARGET_POSITION, g_rxBuffer+1, 4);
                break;
            case TGT_POSITION_MOTOR_RIGHT:
                handleI2cMotorControlCommand(MOTOR_FRONT_RIGHT, RELATIVE_TARGET_POSITION, g_rxBuffer+1, 4);
                handleI2cMotorControlCommand(MOTOR_REAR_RIGHT, RELATIVE_TARGET_POSITION, g_rxBuffer+1, 4);
                break;
            case RUN:
            case STOP:
                handleI2cMotorControlCommand(MOTOR_FRONT_RIGHT, CONTROL_REGISTER, g_rxBuffer+1, 1);
                handleI2cMotorControlCommand(MOTOR_FRONT_LEFT, CONTROL_REGISTER, g_rxBuffer+1, 1);
                handleI2cMotorControlCommand(MOTOR_REAR_RIGHT, CONTROL_REGISTER, g_rxBuffer+1, 1);
                handleI2cMotorControlCommand(MOTOR_REAR_LEFT, CONTROL_REGISTER, g_rxBuffer+1, 1);
                break;
            case GET_CURRENT:
                handleI2cMotorControlCommand(MOTOR_FRONT_RIGHT, MOTOR_CURRENT, g_txBuffer+1, 2);
                handleI2cMotorControlCommand(MOTOR_FRONT_LEFT, MOTOR_CURRENT, g_txBuffer+3, 2);
                handleI2cMotorControlCommand(MOTOR_REAR_RIGHT, MOTOR_CURRENT, g_txBuffer+5, 2);
                handleI2cMotorControlCommand(MOTOR_REAR_LEFT, MOTOR_CURRENT, g_txBuffer+7, 2);
                break;
            case GET_POSITION:
                handleI2cMotorControlCommand(MOTOR_FRONT_RIGHT, CURRENT_POSITION, g_txBuffer+1, 4);
                handleI2cMotorControlCommand(MOTOR_FRONT_LEFT, CURRENT_POSITION, g_txBuffer+5, 4);
                handleI2cMotorControlCommand(MOTOR_REAR_RIGHT, CURRENT_POSITION, g_txBuffer+9, 4);
                handleI2cMotorControlCommand(MOTOR_REAR_LEFT, CURRENT_POSITION, g_txBuffer+13, 4);
                break;
            case GET_STATUS:
                handleI2cMotorControlCommand(MOTOR_FRONT_RIGHT, CURRENT_POSITION, g_txBuffer+1, 4);
                handleI2cMotorControlCommand(MOTOR_FRONT_LEFT, CURRENT_POSITION, g_txBuffer+1, 4);
                handleI2cMotorControlCommand(MOTOR_REAR_RIGHT, CURRENT_POSITION, g_txBuffer+1, 4);
                handleI2cMotorControlCommand(MOTOR_REAR_LEFT, CURRENT_POSITION, g_txBuffer+1, 4);
                break;
        }
    }

    vTaskStartScheduler();

    //if it reaches that point, there is a problem with RTOS.

/* USER CODE END */
}
