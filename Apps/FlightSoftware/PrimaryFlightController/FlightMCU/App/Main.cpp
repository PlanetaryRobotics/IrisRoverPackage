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

#include "adc.h"
#include "gio.h"
#include "i2c.h"
#include "spi.h"
#include "adc.h"
#include "lin.h"

extern "C" {
    void vApplicationIdleHook(void);
    void vApplicationStackOverflowHook(void *xTask, char *pcTaskName);
}

void vApplicationIdleHook(void) {
    run1cycle();
    //gioToggleBit(gioPORTB, 1);
    //for(uint32_t i=0; i<2000000; i++) asm("  NOP");
}

void vApplicationStackOverflowHook(void *xTask, char *pcTaskName) {
    // something really bad happened
}

void main(void)
{
    /* USER CODE BEGIN (3) */

    gioInit();
    i2cInit();
    sciInit();
    adcInit();
    spiInit();
    //linInit();            //linInit() changes the settings of watchdog interface port, please do not use

    constructApp();

    vTaskStartScheduler();

    //if it reaches that point, there is a problem with RTOS.

/* USER CODE END */
}
