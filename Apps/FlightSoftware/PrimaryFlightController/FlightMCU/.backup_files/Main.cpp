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
#include <CubeRover/Wf121/Wf121SerialInterface.hpp>
#include "adc.h"
#include "gio.h"
#include "i2c.h"
#include "spi.h"
#include "adc.h"
#include "rti.h"
#include "sys_dma.h"
#include "sys_mpu.h"

#include "App/DMA.h"
#include "App/SCI.h"
#include "App/SCILIN.h"

extern "C"
{
    void vApplicationIdleHook(void);
    void vApplicationTickHook(void);
    void vApplicationStackOverflowHook(void *xTask, char *pcTaskName);
    void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer,
                                       StackType_t **ppxIdleTaskStackBuffer,
                                       uint32_t *pulIdleTaskStackSize);
}

void vApplicationIdleHook(void)
{
    // Consider actually making a real FreeRTOS Task for FPrime (instead of
    // just running it on the lowest priority task, idle).
    run1cycle();
}

/* configSUPPORT_STATIC_ALLOCATION is set to 1, so the application must provide
an implementation of vApplicationGetIdleTaskMemory() to provide the memory that
is used by the Idle task.
- Needed b/c we need `configSUPPORT_STATIC_ALLOCATION=1`
  so we can statically allocate Comms Buffer Queues (so we can control memory
  use at compile time)
- Impl. taken from: https://www.freertos.org/a00110.html#configSUPPORT_STATIC_ALLOCATION
*/
#define IDLE_TASK_STACK_SIZE (configMINIMAL_STACK_SIZE + 1024) // Rn, all of FPrime runs in the idle task so we better give it some space. Use `uxTaskGetStackHighWaterMark(NULL)` to tune.
void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer,
                                   StackType_t **ppxIdleTaskStackBuffer,
                                   uint32_t *pulIdleTaskStackSize)
{
    /* If the buffers to be provided to the Idle task are declared inside this
    function then they must be declared static - otherwise they will be allocated on
    the stack and so not exist after this function exits. */
    static StaticTask_t xIdleTaskTCB;
    static StackType_t uxIdleTaskStack[IDLE_TASK_STACK_SIZE];

    /* Pass out a pointer to the StaticTask_t structure in which the Idle task's
    state will be stored. */
    *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;

    /* Pass out the array that will be used as the Idle task's stack. */
    *ppxIdleTaskStackBuffer = uxIdleTaskStack;

    /* Pass out the size of the array pointed to by *ppxIdleTaskStackBuffer.
    Note that, as the array is necessarily of type StackType_t,
    stack size is specified in words, not bytes. */
    *pulIdleTaskStackSize = IDLE_TASK_STACK_SIZE;
}

void vApplicationTickHook(void)
{
    // this is used for timers (which are inactive rn) - not a good idea to
    // drive FPrime from here but could use it for timers (if carefully
    // implemented with care taken for configSUPPORT_STATIC_ALLOCATION).
}

void vApplicationStackOverflowHook(void *xTask, char *pcTaskName)
{
    printf("STACKOVERFLOW\n");
//     while (true);
    // something really bad happened
    // TODO: [CWC] Might actually be a good idea to hang here so WatchDog resets us
    // (make sure WD actually does that)
}

void main_orig(void)
{
    /* USER CODE BEGIN (3) */
    _disable_interrupt_(); // Disable all interrupts during initialization (esp. important when we initialize RTI)

    _mpuInit_();

    gioInit();
    Wf121::Wf121Serial::NotReadyForData(); // Tell the radio we're not ready for data yet.
    i2cInit();
    sciInit();
    adcInit();
    spiInit();
    dmaEnable();
    scidmaInit();
    sciIntInit();
    scilinIntInit();

    constructApp();

    rtiInit(); // Initialize RTI for RTOS Tick last

    vTaskStartScheduler(); // Automatically enables IRQs

    // Something went very wrong with the RTOS if we end up here

    /* USER CODE END */
}


