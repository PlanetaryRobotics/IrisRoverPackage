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

extern "C" {
    void vApplicationIdleHook(void);
    void vApplicationTickHook(void);
    void vApplicationStackOverflowHook(void *xTask, char *pcTaskName);
}

extern CubeRover::MotorControlComponentImpl motorControl;
bool test = false;
int dist = 5;
int angle = 45;
void vApplicationIdleHook(void) {
    run1cycle();
    if (test) {
        for (int i = 80; i; i--) {
            motorControl.moveAllMotorsStraight(-1*dist, i);
            motorControl.pollStatus();
        }

        for (int i = 20; i; i--) {
            motorControl.moveAllMotorsStraight(dist, 100);
            motorControl.pollStatus();
        }

        for (int i = 100000; i; i--);

        for (int i = 40; i; i--) {
            motorControl.rotateAllMotors(angle, 100);
            motorControl.pollStatus();
            motorControl.rotateAllMotors(angle, 100);
            motorControl.pollStatus();
            for (int i = 10000; i; i--);
            motorControl.rotateAllMotors(-1*angle, 100);
            motorControl.pollStatus();
            motorControl.rotateAllMotors(-1*angle, 100);
            motorControl.pollStatus();
        }

        for (int i = 100000; i; i--);
    }
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

    constructApp();

    rtiInit();                  // Initialize RTI for RTOS Tick last

    vTaskStartScheduler();      // Automatically enables IRQs

    //Something went very wrong with the RTOS if we end up here

    /* USER CODE END */
}

