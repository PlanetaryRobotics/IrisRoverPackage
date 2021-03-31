/*
 * Main.cpp
 *
 *  Created on: Sep 30, 2019
 *      Author: cedric
 */

#include "sys_common.h"
#include "sys_core.h"
#include "system.h"

#include "CubeRoverConfig.hpp"
#include "FreeRTOS.h"
#include "os_task.h"
#include "CubeRover/Top/Topology.hpp"
#include "adc.h"
#include "gio.h"
#include "i2c.h"
#include "spi.h"
#include "adc.h"
#include "rti.h"
#include "sys_dma.h"

extern "C" {
#include "sys_vim.h"    // WHY NO C++ LINKAGE :(
}

#include "App/DMA.h"

extern "C" {
    void vApplicationIdleHook(void);
    void vApplicationTickHook(void);
    void vApplicationStackOverflowHook(void *xTask, char *pcTaskName);
}

void vApplicationIdleHook(void) {
}

void vApplicationTickHook(void) {
    run1cycle();
}

void vApplicationStackOverflowHook(void *xTask, char *pcTaskName) {
    // while (true);
    // something really bad happened
}


volatile bool busy = false;
volatile bool recv = false;


extern "C" void dmaCh2_ISR(dmaInterrupt_t inttype) {}
extern "C" void dmaCh3_ISR(dmaInterrupt_t inttype) {}

void main(void)
{
    /* USER CODE BEGIN (3) */

    gioInit();
    i2cInit();
    sciInit();
    adcInit();
    spiInit();
    dmaEnable();
    scidmaInit();
    rtiInit();

    constructApp();

    vTaskStartScheduler();      // Automatically enables IRQs

    //if it reaches that point, there is a problem with RTOS.

    /* USER CODE END */
}

