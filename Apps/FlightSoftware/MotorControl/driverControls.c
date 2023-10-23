/*
 * driverControls.c
 *
 *  Created on: Oct 22, 2023
 *      Author: iris
 */

#include "allVars.h"

/**
 * @brief      Reads the whether the DRV8304 driver is in a "fault condition" (and should be cleared). Active low.
 */
void checkDriverFault(void)
{
    //update g_faultRegister with if there is fault in motor driver
    if ((MC_FAULT_DRIVER & g_mcIgnoredFaults) && !(PJIN & 0x02))
    {
        g_faultRegister |= MC_FAULT_DRIVER;
        g_state = FAULT;
    }
}

/**
 * @brief      Clears the DRV8304 Driver Fault Register.
 */
void clearDriverFault(void)
{
    __disable_interrupt(); // entering critical section
    // Pull high first so you can then pull it low:
    //    GPIO_setOutputHighOnPin(GPIO_PORT_PJ, GPIO_PIN0);
    PJOUT |= GPIO_PIN0;
    __delay_cycles(DELAY_100_ms);
    // Reset Fault Register by pulsing ENABLE for 5-32us (18.5us):
    //    GPIO_setOutputLowOnPin(GPIO_PORT_PJ, GPIO_PIN0);
    PJOUT &= ~GPIO_PIN0;
    __delay_cycles(296); // 18.5 us
                         //    GPIO_setOutputHighOnPin(GPIO_PORT_PJ, GPIO_PIN0);
    PJOUT |= GPIO_PIN0;
    __enable_interrupt();

    g_faultRegister &= ~MC_FAULT_DRIVER;
}


