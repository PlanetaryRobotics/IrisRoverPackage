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
#include "sys_mpu.h"

#include "App/DMA.h"
#include "App/SCILIN.h"

extern "C" {
    void vApplicationIdleHook(void);
    void vApplicationTickHook(void);
    void vApplicationStackOverflowHook(void *xTask, char *pcTaskName);
}

void vApplicationIdleHook(void) {
    run1cycle();
}

void vApplicationTickHook(void) {
    // run1cycle();
}

void vApplicationStackOverflowHook(void *xTask, char *pcTaskName) {
    // while (true);
    // something really bad happened
}

extern "C" void dmaCh2_ISR(dmaInterrupt_t inttype) {}
extern "C" void dmaCh3_ISR(dmaInterrupt_t inttype) {}

void main(void)
{
    /* USER CODE BEGIN (3) */
    _disable_interrupt_();      // Disable all interrupts during initialization (esp. important when we initialize RTI)

    _mpuInit_();

    gioInit();
    i2cInit();
    sciInit();
    adcInit();
    spiInit();
    dmaEnable();
    scidmaInit();
    scilinIntInit();

    constructApp();

    rtiInit();                  // Initialize RTI for RTOS Tick last

    vTaskStartScheduler();      // Automatically enables IRQs

    //Something went very wrong with the RTOS if we end up here

    /* USER CODE END */
}

