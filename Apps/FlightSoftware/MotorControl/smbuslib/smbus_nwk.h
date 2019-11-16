/* --COPYRIGHT--,BSD
 * Copyright (c) 2015, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --/COPYRIGHT--*/
#ifndef __SMBUS_NWK_H__
#define __SMBUS_NWK_H__

//*****************************************************************************
//
//! \addtogroup smbus_nwk NWK layer
//! @{
//
//*****************************************************************************

//*****************************************************************************
//
// If building with a C++ compiler, make all of the definitions in this header
// have a C binding.
//
//*****************************************************************************
#ifdef __cplusplus
extern "C"
{
#endif

//
// Include files
//
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "smbus.h"

//*****************************************************************************
// Internal functions called by the physical layer and internal smbus
// processing. Application programs should not call these functions directly.
//*****************************************************************************
//*****************************************************************************
//
//! \brief   Returns the number of received bytes from last transaction
//
//! \param smbus       Pointer to SMBus structure
//
//! \return  Number of bytes in the RX buffer. PEC byte is *not* included.
//
//*****************************************************************************
extern uint8_t SMBus_NWK_getRxPayloadSize(SMBus *smbus);

//*****************************************************************************
//
//! \brief   Adds a new value to CRC-8 Calculation
//
//! Follows format: C(x) = x8 + x2 + x + 1
//! <BR>
//! Two methods are available:
//! - SMB_CRC8_USE_LOOKUP_TABLE : Use a 256B lookup table (faster but
//!                               takes more memory)
//! - SMB_CRC8_CALCULATION      : Calculate CRC8 manually (slower but
//!                               takes less memory)
//! <BR>
//
//! \param *crc     Pointer to current CRC-8 value, will get updated
//! \param newData  New byte added to CRC-8 calulation
//
//! \return  None
//
//*****************************************************************************
extern void SMBus_NWK_addCrc8(uint8_t *crc,
                              uint8_t newData);

//*****************************************************************************
//
//! \brief Start the TX transfer process
//
//! \param smbus       Pointer to SMBus structure
//
//! \return  None
//
//*****************************************************************************
extern void SMBus_NWK_startTxTransfer(SMBus *smbus);

//*****************************************************************************
//
//! \brief Start the RX transfer process
//
//! \param smbus       Pointer to SMBus structure
//
//! \return  None
//
//*****************************************************************************
extern void SMBus_NWK_startRxTransfer(SMBus *smbus);

//*****************************************************************************
//
//! \brief   Start callback for SMBus
//
//! Handles the protocol when a Start is received. Depending on the
//! current state, the start could also mean re-start.
//
//!  \param smbus   Pointer to SMBus structure
//!  \param addrRw  Current Address+RW as received by PHY_DL
//
//! \return  SMBus_State value as follows:
//! - \b SMBus_State_Slave_NotReady    : Packet in progress
//! - \b SMBus_State_PECError          : Packet was invalid
//! - \b SMBus_State_Slave_CmdComplete : restart received after command,
//!                                      process previous packet
//
//*****************************************************************************
extern SMBus_State SMBus_NWK_slaveProcessStart(SMBus *smbus,
                                               uint8_t addrRw);

//*****************************************************************************
//
//! \brief   RX callback for SMBus slave
//
//! Handles the protocol when a reception is requested.
//
//!  \param smbus Pointer to SMBus structure
//!  \param data  Data byte as received by the PHY layer
//
//! \return  SMBus_State value as follows:
//!   - \b SMBus_State_OK
//!   - \b SMBus_State_Slave_NotReady      : Packet in progress
//!   - \b SMBus_State_Slave_FirstByte     : First byte (Command) was received
//!   - \b SMBus_State_Slave_ByteReceived  : Data byte (2-n) was received
//!   - \b SMBus_State_Slave_Error         : not expecting RX
//!   - \b SMBus_State_Slave_NotReady      : not ready for RX
//!   - \b SMBus_State_DataSizeError       : Max packet size exceeded
//
//*****************************************************************************
extern SMBus_State SMBus_NWK_slaveProcessRx(SMBus *smbus,
                                            uint8_t data);

//*****************************************************************************
//
//! \brief   TX callback for SMBus slave
//
//! Handles the protocol when a transmission is requested.
//
//!  \param smbus Pointer to SMBus structure
//!  \param *data Pointer to tranmission byte, must be written with value
//!               being sent
//
//! \return  SMBus_State value as follows:
//!   - \b SMBus_State_OK
//!   - \b SMBus_State_Slave_NotReady      : Buffer not initialized
//!   - \b SMBus_State_Slave_Error         : not expecting TX
//!   - \b SMBus_State_Slave_NotReady      : not ready for TX
//!   - \b SMBus_State_DataSizeError       : Max packet size exceeded
//
//*****************************************************************************
extern SMBus_State SMBus_NWK_slaveProcessTx(SMBus *smbus,
                                            uint8_t *data);

//*****************************************************************************
//
//! \brief   Stop callback for SMBus-based protocol
//
//! Handles SMus protocol when a stop is detected
//
//! \param smbus       Pointer to SMBus structure
//
//! \return  SMBus_State value as follows:
//! - \b SMBus_State_OK
//! - \b SMBus_State_Slave_NotReady    : Packet in progress
//! - \b SMBus_State_PECError          : Packet was invalid
//! - \b SMBus_State_Slave_QCMD        : Quick Command was detected
//! - \b SMBus_State_Slave_CmdComplete : Stop was detected and packet is ready
//!                                      to process
//
//*****************************************************************************
extern SMBus_State SMBus_NWK_slaveProcessStop(SMBus *smbus);

//*****************************************************************************
//
//! \brief   Handles the SMBus protocol when a Timeout error is detected by PHY
//
//! Used for USCI devices that do not support timeouts inherently (as in eUSCI).
//! \param smbus       Pointer to SMBus structure
//
//! \return  SMBus_State_TimeOutError
//
//*****************************************************************************
extern SMBus_State SMBus_NWK_slaveProcessTimeout(SMBus *smbus);

//*****************************************************************************
//
//! \brief   RX callback for SMBus master
//
//! Handles the protocol when a reception is requested.
//
//!  \param smbus Pointer to SMBus structure
//!  \param data  Data byte as received by the PHY layer
//
//! \return  SMBus_State value as follows:
//!   - \b SMBus_State_OK
//!   - \b SMBus_State_Master_Error       : not expecting RX
//!   - \b SMBus_State_DataSizeError      : Max packet size exceeded, or greater
//!                                         than expected
//
//*****************************************************************************
extern SMBus_State SMBus_NWK_masterProcessRx(SMBus *smbus,
                                             uint8_t data);

//*****************************************************************************
//
//! \brief   TX callback for SMBus master
//
//! Handles the protocol when a transmission is requested.
//
//!  \param smbus Pointer to SMBus structure
//!  \param *data Pointer to tranmission byte, must be written with value
//!               being sent
//
//! \return  SMBus_State value as follows:
//!   - \b SMBus_State_OK
//!   - \b SMBus_State_Master_Error        : not expecting TX
//
//*****************************************************************************
extern SMBus_State SMBus_NWK_masterProcessTx(SMBus *smbus,
                                             uint8_t *data);

//*****************************************************************************
//
//! \brief   Process a Stop condition
//
//
//!  \param smbus Pointer to SMBus structure
//
//! \return  SMBus_State value as follows:
//!   - \b SMBus_State_OK
//!   - \b SMBus_State_PECError
//
//*****************************************************************************
extern SMBus_State SMBus_NWK_masterProcessStop(SMBus *smbus);

//*****************************************************************************
//
//! \brief   Handles the SMBus protocol when a Timeout error is detected by PHY
//
//! Used for USCI devices that do not support timeouts inherently (as in eUSCI).
//
//! \param smbus       Pointer to SMBus structure
//
//! \return  SMBus_State_TimeOutError
//
//*****************************************************************************
extern SMBus_State SMBus_NWK_masterProcessTimeout(SMBus *smbus);

//*****************************************************************************
//
//! \brief   Handles the SMBus protocol when a NACK
//
//! \param smbus       Pointer to SMBus structure
//
//! \return  SMBus_State value as follows:
//!   - \b SMBus_State_OK
//!   - \b SMBus_State_Master_NACK
//
//*****************************************************************************
extern SMBus_State SMBus_NWK_masterProcessNACK(SMBus *smbus);

//*****************************************************************************
//
//! \brief   Resets the SMBus network layer
//
//! \param smbus       Pointer to SMBus structure
//
//! \return  none
//
//*****************************************************************************
extern void SMBus_NWK_masterReset(SMBus *smbus);

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************

#endif //__SMBUS_NWK_H__
