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

#include "gio.h"
#include "i2c.h"
#include "sci.h"

extern "C" {
    void vApplicationIdleHook( void );
}

void vApplicationIdleHook( void ){
    run1cycle();
    //gioToggleBit(gioPORTB, 1);
    for(uint32_t i=0; i<2000000; i++) asm("  NOP");
}

void main(void)
{
/* USER CODE BEGIN (3) */
    gioInit();
    i2cInit();
    sciInit();

    constructApp();

    uint8_t buffer[4];
    buffer[0]= 0x08;
    buffer[1]= 0x00;
    buffer[2]= 0x01;
    buffer[3]= 0x02;


    while(1){
        // Pull low RTS
        gioSetBit(gioPORTB,3,0);

        while(gioGetBit(gioPORTB, 2)); //block until CTS goes low

        sciSend(sciREG, sizeof(buffer), buffer);

        gioSetBit(gioPORTB,3,1);

        for(int i=0;i < 20000; i++ ) asm("  NOP");
    }

    vTaskStartScheduler();

    //if it reaches that point, there is a problem with RTOS.

/* USER CODE END */
}
