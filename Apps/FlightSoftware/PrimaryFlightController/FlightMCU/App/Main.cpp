/*
 * Main.cpp
 *
 *  Created on: Sep 30, 2019
 *      Author: cedric
 */

#include <App/DMA.h>
#include "sys_common.h"
#include "system.h"

#include <CubeRoverConfig.hpp>
#include "FreeRTOS.h"
#include "os_task.h"
#include "CubeRover/Top/Topology.hpp"
#include "adc.h"
#include "gio.h"
#include "i2c.h"
#include "spi.h"
#include "adc.h"
#include "rti.h"

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
    rtiInit();

    scidmaInit();
    //linsci2enableMBUFF();
    
    constructApp();
    /*
    _enable_IRQ();                                      // Enable IRQ - Clear I flag in CPS register // @suppress("Function cannot be resolved")
    sciEnterResetState(scilinREG);
    sciSetBaudrate(scilinREG, 9600);
    sciExitResetState(scilinREG);
    alignas(8) char test[] = "foo, bar. This is a test!";
    while (1) {
        scidmaSend(DMA_CH1, test, sizeof(test));
    }*/

    vTaskStartScheduler();

    //if it reaches that point, there is a problem with RTOS.

    /* USER CODE END */
}
