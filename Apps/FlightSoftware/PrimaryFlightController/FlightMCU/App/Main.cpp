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

void vApplicationIdleHook( void ){
    run1cycle();
    //gioToggleBit(gioPORTB, 1);
    //for(uint32_t i=0; i<2000000; i++) asm("  NOP");
}

using namespace Wf121;

void main(void)
{
    CubeRoverNetworkManager wf121;

    /* USER CODE BEGIN (3) */
    gioInit();
    i2cInit();
    sciInit();

    constructApp();

    while(1){
        wf121.UpdateNetworkManager();
        for(uint32_t i=0;i < 20000; i++ ) asm("  NOP");
    }

    vTaskStartScheduler();

    //if it reaches that point, there is a problem with RTOS.

/* USER CODE END */
}
