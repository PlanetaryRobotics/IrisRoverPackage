/*
 * Main.cpp
 *
 *  Created on: Sep 30, 2019
 *      Author: cedric
 */

#include "FreeRTOS.h"
#include "os_task.h"
#include "CubeRover/Top/Topology.hpp"
#include "gio.h"
#include "main.hpp"

extern "C" {
    void vApplicationIdleHook( void );
}

void vApplicationIdleHook( void ){
    //run1cycle();
    gioToggleBit(gioPORTB, 1);
    //Os::Task::delay(1000);
}

void main(void)
{
/* USER CODE BEGIN (3) */
    gioInit();
    constructApp();

    vTaskStartScheduler();

    //if it reaches that point, there is a problem with RTOS.

/* USER CODE END */
}
