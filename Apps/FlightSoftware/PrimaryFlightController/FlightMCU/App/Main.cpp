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
#include "spi.h"
#include "adc.h"
#include "lin.h"

extern "C" {
    void vApplicationIdleHook(void);
}

int test = 0;

void vApplicationIdleHook(void) {
    if (test)
        camera.triggerImageCapture(53);
    run1cycle();
    //gioToggleBit(gioPORTB, 1);
    //for(uint32_t i=0; i<2000000; i++) asm("  NOP");
}


void main(void)
{
    /* USER CODE BEGIN (3) */
    gioInit();
    i2cInit();
    sciInit();
    adcInit();
    spiInit();
    linInit();

    camera.m_fpgaFlash.setupDevice();

    constructApp();

    vTaskStartScheduler();

    //if it reaches that point, there is a problem with RTOS.

/* USER CODE END */
}
