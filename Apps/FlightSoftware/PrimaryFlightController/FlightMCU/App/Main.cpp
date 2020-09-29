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

extern "C" {
    void vApplicationIdleHook( void );
}

void vApplicationIdleHook( void ){
    run1cycle();
    //gioToggleBit(gioPORTB, 1);
    for(uint32_t i=0; i<2000000; i++) asm("  NOP");
}

Drv::BlockDriverImpl blockDriver("blockDriver");
int dividers[3] = {220000000, 2200000, 220000};
Svc::RateGroupDriverImpl rateGroupDriver("rateGroupDriver", dividers, 3);
// FIXME: The following needs the "context" parameters filled out
Svc::ActiveRateGroupImpl rateGroupLowFreq("rateGroupLowFreq", {}, 0);   // 1Hz
Svc::ActiveRateGroupImpl rateGroupMedFreq("rateGroupMedFreq", {}, 0);   // 100Hz
Svc::ActiveRateGroupImpl rateGroupHiFreq("rateGroupHiFreq", {}, 0);     // 1kHz
// FIXME: This module needs to be autocoded
// extern Svc::TimeImpl cubeRoverTime("cubeRoverTime");
Svc::TlmChanImpl tlmChan("tlmChan");
Svc::CommandDispatcherImpl cmdDispatcher("cmdDispatcher");

void main(void)
{
/* USER CODE BEGIN (3) */
    gioInit();
    i2cInit();

    constructApp();

    vTaskStartScheduler();

    //if it reaches that point, there is a problem with RTOS.

/* USER CODE END */
}
