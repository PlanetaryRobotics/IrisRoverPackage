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

void vApplicationIdleHook( void ){
    run1cycle();
    //gioToggleBit(gioPORTB, 1);
    //for(uint32_t i=0; i<2000000; i++) asm("  NOP");
}

using namespace Wf121;

uint8_t g_rxBuffer[RX_RING_BUFFER_SIZE];
uint8_t g_txBuffer[8];

void handleI2cMotorControlCommand(uint8_t *cmd){
    for(uint8_t mcId=0; mcId < 1; mcId++){
        i2cSetSlaveAdd(i2cREG1, 0x48+mcId);
        i2cSetDirection(i2cREG1, I2C_TRANSMITTER);

        /* Set mode as Master */
        i2cSetMode(i2cREG1, I2C_MASTER);

        /* Set Stop after programmed Count */
        i2cSetStop(i2cREG1);

        /* Transmit Start Condition */
        i2cSetStart(i2cREG1);

        i2cSendByte(i2cREG1, cmd[0]); // send the address register

        /* Wait until Bus Busy is cleared */
        while(i2cIsBusBusy(i2cREG1) == true);

        switch(cmd[0]){
            case I2C_ADDRESS:
                break;
            case RELATIVE_TARGET_POSITION:
                i2cSetDirection(i2cREG1, I2C_RECEIVER);
                i2cReceive(i2cREG1, 4, cmd+1);
                /* Wait until Bus Busy is cleared */
                while(i2cIsBusBusy(i2cREG1) == true);
                break;
            case TARGET_SPEED:
                break;
            case CURRENT_POSITION:
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

        // test delay
        for(uint32_t delay=0;delay<1000000;delay++);
    }
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
        g_rxBuffer[0] = 0x01;
        g_rxBuffer[1] = 100;
        handleI2cMotorControlCommand(g_rxBuffer);

//        wf121.UpdateNetworkManager();
//        wf121.ReceiveUdpData(g_rxBuffer, commandPacketSize, &byteRead, UdpReadMode::WAIT_UNTIL_READY | UdpReadMode::NORMAL_READ, 10);
//
//        if(byteRead == commandPacketSize){
//            handleI2cMotorControlCommand(g_rxBuffer);
//
//            g_txBuffer[0] = 0xAA;   // send back acknowledgment
//            wf121.SendUdpData(g_txBuffer, g_txBuffer, 10000);
//        }
    }

    vTaskStartScheduler();

    //if it reaches that point, there is a problem with RTOS.

/* USER CODE END */
}
