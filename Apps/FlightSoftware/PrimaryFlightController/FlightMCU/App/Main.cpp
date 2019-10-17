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

void main(void)
{
/* USER CODE BEGIN (3) */
    gioInit();
    constructApp();

    vTaskStartScheduler();

    for (;;) {
       //run1cycle();
       gioToggleBit(gioPORTB, 1);
    }

/* USER CODE END */
}
