/*
 * CubeRover Motor Controller: Main Process.
 * - Runs a tight 3 level control loop on position->velocity->current->pwm
 * -- Setpoints and control parameters are stored in registers, set over I2C (SMBus).
 * -- Controller always runs based on latest parameters.
 * -- I2C updates those parameters through interrupt during run.
 *
 * Highlevel TODO:
 * - Actually make motor turn.
 * - Nyquist stability proof + Zeigler-Nichols tuning.
 * - Pass data back up to master controller over I2C.
 * - Make MCU enter LPM until new message is received once target position reached.
 * - Feedforward trajectory preplanning + re-tuning (?).
 * - Iterative learning control to offset effects of mechanical damage (?).
 * -- Would work if switch to velocity-based pre-planning.
 *
 * Author: Connor W. Colombo, CMU
 * Some SMBus functions taken from or based off of example code in MSPWare by Texas Instruments Incorporated, BSD Copyright (c) 2015.
 */


#include <string.h> // for memcpy

#include "smbus.h"

#include <HAL.h>
#include <Driver_HAL.h>

#include "I2C_message.h"
#include "I2C_registers.h"
#include "Control.h"


#if (SMB_MANUAL_ACK_ENABLE == 1)
#error "This example doesn't implement DMA ISR for Manual ACK"
#endif

//
// Macros and definitions
//
#define SMB_DEFAULT_PEC_ENABLED //          Enable PEC by default

//
// Global variables
//
static SMBus sSMBusSlave;                       /*! SMBus Structure           */

static uint8_t* readBuffer = i2c_message.raw;  /*! SMB Reception Buffer. Have data dump straight into message union. */
static uint8_t writeBuffer[MESSAGE_BUFFER_SIZE];  /*! SMB Transmission Buffer   */

//*****************************************************************************
//
//! Main Function
//!
//! SMBus Slave demo responding to a ReadByte command from a Master
//!  The example responds to all commands by sending an echo
//!
//! \return None
//
// *****************************************************************************
void main(void) {
    Register_Init();

    HAL_MCU_Init();         // Initialize default CLK and GPIO settings

    HAL_I2C_InitGPIO();         // Initialize GPIOs for I2C functionality

    // Initialize SMBus Master using USCI_B0 always at 100kbps per SMBus spec
    SMBus_slaveInit(&sSMBusSlave, HAL_I2C_GetBaseAddr());
    SMBus_slaveSetAddress(&sSMBusSlave, i2c_register.data.address);

    // Set the RX and TX buffers for SMBus layers
    SMBus_slaveSetRxBuffer(&sSMBusSlave, readBuffer, sizeof(readBuffer));
    SMBus_slaveSetTxBuffer(&sSMBusSlave, writeBuffer, sizeof(writeBuffer));

    SMBus_slaveEnableInt(&sSMBusSlave);

    #ifdef SMB_DEFAULT_PEC_ENABLED
    SMBus_enablePEC(&sSMBusSlave);  // Enable Packet Error Check
    #endif

    while(1){
        // Wait for packets from master
        __disable_interrupt();
        {
            //_BIS_SR(LPM0_bits + GIE); // Go to sleep in LPM0 due to high freq PWM
            Update_Control();
        }
        __enable_interrupt();
    }
}

//
// Events
//
//*****************************************************************************
//
//! I2C Event
//!
//! Event called by HAL when I2C TX, RX, Start, Stop and errors are detected
//!
//! \return  true to wake-up MCU, false to stay in LPMx
//
// *****************************************************************************
bool HAL_I2C_Event(void){
    // Check the state of SMBus
    switch(SMBus_slaveProcessInt(&sSMBusSlave)){
    case SMBus_State_Slave_QCMD:
        // Quick command is not implemented
        break;
    case SMBus_State_Slave_CmdComplete:
        // Handle New Message:
        if(i2c_message.rw){ // requesting data write:
            // Copy relevant data into appropriate section of register:
            memcpy(i2c_regmap[i2c_message.regID-1], i2c_message.data, i2c_message.len);
        } else{ // requesting data return
            // TODO
        }
        sSMBusSlave.nwk.txLen = 0;      // Nothing to respond (beyond basic ACK).
        break;
    case SMBus_State_Slave_FirstByte:
        // TODO: pre-validate the command
        break;
    case SMBus_State_DataSizeError:
    case SMBus_State_PECError:
    case SMBus_State_Slave_Error:
    case SMBus_State_TimeOutError:
    default:
        break;
    }
    // Clear flags to be ready for next packet
    SMBus_processDone(&sSMBusSlave);

    return(false); // No need to wake-up
}

//*****************************************************************************
//
//! Timer Timeout Event
//!
//! Event called by HAL when Timeout timer is detected for devices without eUSCI
//!
//! \return  true to wake-up MCU, false to stay in LPMx
//
// *****************************************************************************
#ifdef __MSP430_HAS_USCI__
bool HAL_Timeout_Event(void) {
    // Call the SMBUS function to handle a timeout error and restart the SMBUS
    SMBus_slaveProcessTimeoutInt(&sSMBusSlave);
    return(false);
}

#endif
