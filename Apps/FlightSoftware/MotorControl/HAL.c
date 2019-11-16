/*
 * Hardware Abstraction Layer for Motor Controller.
 *
 * Author: Connor W. Colombo, CMU
 * Some SMBus functions taken from or based off of example code in MSPWare by Texas Instruments Incorporated, BSD Copyright (c) 2015.
 */

#include <HAL.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "msp430.h"

//
// Local functions
//
static void MCU_Clock_Init(void);
static void MCU_GPIO_Init(void);

//*****************************************************************************
//
//! MCU initialization
//!
//! Initializes the MSP430
//!  Sets GPIOs to default state
//!  Configures clocks
//!
//! \return None
//
// *****************************************************************************
void HAL_MCU_Init(void)
{
    // Initialize GPIOs
    MCU_GPIO_Init();

    // Initialize the MSP430 Clocks
    MCU_Clock_Init();

    return;
}

//*****************************************************************************
//
//! Clock Initialization
//!
//! This demo sets SMCLK/MCLK = DCO @ MCLK_MHZ (defined in project settings)
//!                ACLK = VLO
//!
//! \return None
//
// *****************************************************************************
static void MCU_Clock_Init(void)
{
#if (MCLK_MHZ == 1)
    if(CALBC1_1MHZ == 0xFF)                    // If calibration constant erased
    {
        while(1)
        {
            ;                                   // do not load, trap CPU!!
        }
    }
    DCOCTL = 0;                                 // Select lowest DCOx and MODx settings
    BCSCTL1 = CALBC1_1MHZ;                      // Set range
    DCOCTL = CALDCO_1MHZ;                       // Set DCO step + modulation */
#elif (MCLK_MHZ == 8)
    if(CALBC1_8MHZ == 0xFF)                    // If calibration constant erased
    {
        while(1)
        {
            ;                                  // do not load, trap CPU!!
        }
    }
    DCOCTL = 0;                                // Select lowest DCOx and MODx settings
    BCSCTL1 = CALBC1_8MHZ;                     // Set range
    DCOCTL = CALDCO_8MHZ;                      // Set DCO step + modulation */
#else
#error "Frequency not supported, change project settings"
#endif

    BCSCTL3 = LFXT1S_2;                         // VLO for ACLK (~12Khz)

    return;
}

//*****************************************************************************
//
//! GPIO Initialization
//!
//! Set GPIO pins to a default state
//!
//! \return None
//
// *****************************************************************************
static void MCU_GPIO_Init(void)
{
    // Set all pins as output low, except
    // I2C (P1[6:7])
    P1OUT &= ~(BIT0 | BIT1 | BIT2 | BIT3 | BIT4 | BIT5);
    P2OUT &= ~(BIT0 | BIT1 | BIT2 | BIT3 | BIT4 | BIT5 | BIT6 | BIT7);
    P3OUT &= ~(BIT0 | BIT1 | BIT2 | BIT3 | BIT4 | BIT5 | BIT6 | BIT7);
    P1DIR |= (BIT0 | BIT1 | BIT2 | BIT3 | BIT4 | BIT5);
    P2DIR |= (BIT0 | BIT1 | BIT2 | BIT3 | BIT4 | BIT5 | BIT6 | BIT7);
    P3DIR |= (BIT0 | BIT1 | BIT2 | BIT3 | BIT4 | BIT5 | BIT6 | BIT7);

    return;
}

//*****************************************************************************
//
//! Initialization of I2C GPIOs
//!
//! Initialize GPIOs required for I2C
//!
//! \return None
//
// *****************************************************************************
void HAL_I2C_InitGPIO(void)
{
    // Set P1[6:7]pins for I2C functionality
    P1SEL |= BIT6 + BIT7;                     // Assign I2C pins to USCI_B0
    P1SEL2 |= BIT6 + BIT7;                    // Assign I2C pins to USCI_B0
}

//*****************************************************************************
//
//! Get Base Address of I2C used for SMBus
//!
//! The base address is used by smbuslib PHY layer for FR5xx eUSCI.
//! The simpler implementation from 2xx doesn't use base address.
//!
//! \return Base address of I2C module, NULL if no base address used
//
// *****************************************************************************
uint32_t HAL_I2C_GetBaseAddr(void)
{
    // G2xx implementation doesn't use base address, the module is hardcoded
    // in smbuslib PHY
    return(NULL);
}

//
// Interrupt Service Routines
//
//*****************************************************************************
//
//! I2C Interrupt Service Routine
//!
//! ISR for I2C module
//!   This ISR generates an event which should be declared in application
//!
//! \return None
//
// *****************************************************************************
#pragma vector=USCIAB0TX_VECTOR, USCIAB0RX_VECTOR
__interrupt void HAL_I2C_ISR(void)
{
    // This event should be declared in Application
    if(HAL_I2C_Event() == true)
    {
        LPM3_EXIT;
    }
}

//*****************************************************************************
//
//! Timeout Timer ISR
//!
//! Interrupt Service routine for Timeout Timer. Used to detect timeout from I2C.
//!
//! \return  None
//
// *****************************************************************************
#ifdef __MSP430_HAS_USCI__
#pragma vector=TIMER0_A0_VECTOR
__interrupt void HAL_TIMEOUT_ISR(void)
{
    // Call the SMBUS function to handle a timeout error and restart the SMBUS
    if(HAL_Timeout_Event() == true)
    {
        LPM3_EXIT;
    }
}

#endif

//*****************************************************************************
//
//! Low level initialization
//!
//! Disables the watchdog after reset
//!
//! \return 1: Initialize all variables
//
// *****************************************************************************
#ifdef __IAR_SYSTEMS_ICC__
int __low_level_init(void)
#elif defined (__TI_COMPILER_VERSION__)
int _system_pre_init(void)
#endif
{
    WDTCTL = WDTPW + WDTHOLD;   // Stop WDT
    return(1);
}

//*****************************************************************************
//
//! Dummy ISR
//!
//! Dummy ISR in case an interrupt is enabled unintentionally
//!
//! \return none
//
// *****************************************************************************
#pragma vector=TRAPINT_VECTOR,PORT1_VECTOR,PORT2_VECTOR, \
    TIMER0_A1_VECTOR,WDT_VECTOR,COMPARATORA_VECTOR, \
    NMI_VECTOR
__interrupt void Dummy_ISR(void)
{
    while(1)
    {
        ;
    }
}
